//
// Copyright 2019 Wanghong Lin
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//         http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
//         limitations under the License.
//
// Created by Wanghong Lin on 2019/12/23.
//

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <memory>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libswresample/swresample.h"

#ifdef __cplusplus
};
#endif

class Decoder {
public:
    struct Callback {
        template<typename FUNC>
        explicit Callback(const FUNC& func) : _func(func) {}

        void call(AVFormatContext* ifmt_ctx, AVCodecContext* dec_ctx, AVFrame* frame, int stream_index) {
            if (_func) return _func(ifmt_ctx, dec_ctx, frame, stream_index);
        }

    private:
        std::function<void(AVFormatContext*, AVCodecContext*, AVFrame*, int)> _func;
    };

    static std::unique_ptr<Decoder> New(const std::string& input, const AVMediaType interested_stream) {
        std::unique_ptr<Decoder> decoder(new Decoder(input, interested_stream));
        return decoder;
    }

    void read_frame(Callback&& callback) {
        AVPacket packet {.size = 0, .data = nullptr};
        av_init_packet(&packet);

        while (av_read_frame(ifmt_ctx, &packet) == 0) {
            if (packet.stream_index == target_stream_index) {
                auto ret = 0;
                ret = avcodec_send_packet(dec_ctx, &packet);

                while (ret >= 0) {
                    auto dec_frame = av_frame_alloc();

                    ret = avcodec_receive_frame(dec_ctx, dec_frame);
                    if (ret < 0) {
                        if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                            throw std::runtime_error(av_err2str(ret));
                        }
                    } else {
                        callback.call(ifmt_ctx, dec_ctx, dec_frame, target_stream_index);
                    }

                    av_frame_free(&dec_frame);
                }
            }
        }

        callback.call(ifmt_ctx, dec_ctx, nullptr, target_stream_index);
    }

    AVCodecContext* get_decoder_context() { return dec_ctx; }

    virtual ~Decoder() {
        if (ifmt_ctx) {
            avformat_close_input(&ifmt_ctx);
            avformat_free_context(ifmt_ctx);
        }
        if (dec_ctx) {
            avcodec_close(dec_ctx);
            avcodec_free_context(&dec_ctx);
        }
    }

private:
    Decoder(const std::string& input, const AVMediaType interested_stream) {
        if (input.empty()) {
            throw std::runtime_error("input cannot be empty");
        }
        if (interested_stream == AVMEDIA_TYPE_UNKNOWN) {
            throw std::runtime_error("unknown interested stream");
        }

        auto ret = 0;
        auto ifmt = av_find_input_format("mp4");
        ifmt_ctx = avformat_alloc_context();
        if ((ret = avformat_open_input(&ifmt_ctx, input.c_str(), ifmt, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        AVCodec* decoder = nullptr;
        ret = av_find_best_stream(ifmt_ctx, interested_stream, -1, -1, &decoder, 0);
        if (ret == AVERROR_STREAM_NOT_FOUND) {
            throw std::runtime_error(av_err2str(AVERROR_STREAM_NOT_FOUND));
        }

        for (unsigned i = 0; i < ifmt_ctx->nb_streams; i++) {
            if (ifmt_ctx->streams[i]->codecpar->codec_type == interested_stream) {
                target_stream_index = i;
                if (ret == AVERROR_DECODER_NOT_FOUND || decoder == nullptr) {
                    decoder = avcodec_find_decoder(ifmt_ctx->streams[i]->codecpar->codec_id);
                }
                break;
            }
        }

        if (!decoder) {
            throw std::runtime_error("could not find decoder");
        }

        if ((dec_ctx = avcodec_alloc_context3(decoder)) == nullptr) {
            throw std::runtime_error("allocate decoder context");
        }

        avcodec_parameters_to_context(dec_ctx, ifmt_ctx->streams[target_stream_index]->codecpar);
        if ((ret = avcodec_open2(dec_ctx, decoder, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        // av_dump_format(ifmt_ctx, target_stream_index, input.c_str(), 0);
    }

    AVFormatContext* ifmt_ctx {};
    AVCodecContext* dec_ctx {};
    int target_stream_index {};
};

template<typename FUNC>
Decoder::Callback make_callback(FUNC&& func)
{
    return Decoder::Callback(func);
}

class Encoder {

public:
    static std::unique_ptr<Encoder> New(const std::string& filename, const std::string& short_name, const std::string& mime_type,
                                        const AVMediaType media_type, const AVCodecParameters* enc_params) {
        std::unique_ptr<Encoder> encoder(new Encoder(filename, short_name, mime_type, media_type, enc_params));
        return encoder;
    }

    AVCodecContext* get_encoder_context() { return enc_ctx; }

    void write_frame(AVFrame* enc_frame) {
        if (enc_frame) {
            enc_frame->sample_rate = enc_ctx->sample_rate;
            enc_frame->channels = enc_ctx->channels;
            enc_frame->channel_layout = enc_ctx->channel_layout;
            enc_frame->format = enc_ctx->sample_fmt;
            enc_frame->pts = pts;
            pts += enc_frame->nb_samples;
        }

        auto ret = 0;
        ret = avcodec_send_frame(enc_ctx, enc_frame);
        while (ret >= 0) {
            AVPacket packet{.size = 0, .data = nullptr};
            av_init_packet(&packet);

            ret = avcodec_receive_packet(enc_ctx, &packet);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                if (ret == AVERROR_EOF) {
                    av_write_trailer(ofmt_ctx);
                }
                return;
            } else if (ret < 0) {
                throw std::runtime_error(av_err2str(ret));
            }

            packet.stream_index = 0;
            av_packet_rescale_ts(&packet, enc_ctx->time_base, ofmt_ctx->streams[0]->time_base);
            if ((ret = av_interleaved_write_frame(ofmt_ctx, &packet)) < 0) {
                av_log(nullptr, AV_LOG_ERROR, "%s %s\n", __FUNCTION__, av_err2str(ret));
            }
        }
    }

    virtual ~Encoder() {
        if (enc_ctx) {
            avcodec_close(enc_ctx);
            avcodec_free_context(&enc_ctx);
        }
        if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&ofmt_ctx->pb);
        }
    }
private:
    Encoder(const std::string& filename, const std::string& short_name, const std::string& mime_type,
            AVMediaType media_type, const AVCodecParameters* enc_params) {
        auto ret = 0;
        auto ofmt = av_guess_format(short_name.c_str(), filename.c_str(), mime_type.c_str());
        if ((ret = avformat_alloc_output_context2(&ofmt_ctx, ofmt, "m4a", filename.c_str())) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        auto encoder_id = av_guess_codec(ofmt, short_name.c_str(), filename.c_str(), mime_type.c_str(), media_type);
        auto encoder = avcodec_find_encoder(encoder_id);
        if ((enc_ctx = avcodec_alloc_context3(encoder)) == nullptr) {
            throw std::runtime_error("could not allocate encoder context");
        }

        avcodec_parameters_to_context(enc_ctx, enc_params);
        enc_ctx->time_base = av_make_q(1, enc_params->sample_rate);
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
        if ((ret = avcodec_open2(enc_ctx, encoder, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        AVStream* stream = avformat_new_stream(ofmt_ctx, encoder);
        if (stream == nullptr) {
            throw std::runtime_error("could not new stream");
        }
        avcodec_parameters_from_context(stream->codecpar, enc_ctx);
        stream->codec = enc_ctx;
        stream->time_base = enc_ctx->time_base;

        if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            if ((ret = avio_open2(&ofmt_ctx->pb, filename.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr)) < 0) {
                throw std::runtime_error(av_err2str(ret));
            }
        }

        if ((ret = avformat_write_header(ofmt_ctx, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }
        av_dump_format(ofmt_ctx, 0, filename.c_str(), 1);
    }

    AVFormatContext* ofmt_ctx {};
    AVCodecContext* enc_ctx {};
    uint64_t pts {};
};

class Sampler {
public:
    static std::unique_ptr<Sampler> New(const AVCodecContext* dec_ctx, const AVCodecContext* enc_ctx) {
        std::unique_ptr<Sampler> sampler(new Sampler(dec_ctx, enc_ctx));
        return sampler;
    }

    int enqueue_frame(AVFrame* dec_frame) {
        auto ret = 0;
        if (converted_frame) {
            av_freep(&converted_frame->data[0]);
            av_frame_free(&converted_frame);
        }

        converted_frame = av_frame_alloc();
        consume_nb_samples = 0;
        out_nb_samples = static_cast<int>(av_rescale_rnd(swr_get_delay(swr_ctx, dec_frame->sample_rate) + dec_frame->nb_samples,
                                                         enc_ctx->sample_rate, dec_frame->sample_rate, AV_ROUND_UP));
        if ((ret = av_samples_alloc(converted_frame->data, converted_frame->linesize, enc_ctx->channels,
                                    out_nb_samples, enc_ctx->sample_fmt, 0)) > 0) {
            if ((ret = swr_convert(swr_ctx, converted_frame->data, out_nb_samples,
                                   (const uint8_t **)(dec_frame->data), dec_frame->nb_samples)) > 0) {
                converted_frame->nb_samples = ret;
            } else {
                av_frame_free(&converted_frame);
                throw std::invalid_argument(av_err2str(ret));
            }
        } else {
            av_frame_free(&converted_frame);
            throw std::invalid_argument(av_err2str(ret));
        }
        return ret;
    }

    AVFrame* dequeue_frame() {
        auto ret = 0;
        if (converted_frame && consume_nb_samples < converted_frame->nb_samples) {
            auto enc_frame = av_frame_alloc();
            av_samples_alloc(enc_frame->data, enc_frame->linesize, enc_ctx->channels, enc_ctx->frame_size, enc_ctx->sample_fmt, 0);
            enc_frame->nb_samples = enc_ctx->frame_size;
            if ((ret = avcodec_fill_audio_frame(enc_frame, enc_ctx->channels, enc_ctx->sample_fmt,
                                                converted_frame->data[0] + consume_nb_samples, converted_frame->linesize[0], 0)) < 0) {
                throw std::runtime_error(av_err2str(ret));
            }

            consume_nb_samples += enc_frame->nb_samples;
            return enc_frame;
        }

        return nullptr;
    }

    virtual ~Sampler() {
        if (!swr_ctx) {
            swr_close(swr_ctx);
            swr_free(&swr_ctx);
            swr_ctx = nullptr;
        }
    }
private:
    explicit Sampler(const AVCodecContext* dec_ctx, const AVCodecContext* enc_ctx) {
        swr_ctx = swr_alloc_set_opts(nullptr,
                                     enc_ctx->channel_layout, enc_ctx->sample_fmt, enc_ctx->sample_rate,
                                     dec_ctx->channel_layout, dec_ctx->sample_fmt, dec_ctx->sample_rate, 0, nullptr);
        if (!swr_ctx) {
            throw std::bad_alloc();
        }

        if (int ret = swr_init(swr_ctx) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        if (!swr_is_initialized(swr_ctx)) {
            throw std::runtime_error("initialize swr context");
        }
        this->enc_ctx = enc_ctx;
    }

    SwrContext* swr_ctx;
    const AVCodecContext* enc_ctx;
    int out_nb_samples {};
    int consume_nb_samples {};
    AVFrame* converted_frame {};
};

class Filter {
public:
    static std::unique_ptr<Filter> New(const std::string& filter_desc, const AVCodecContext* dec_ctx) {
        std::unique_ptr<Filter> filter(new Filter(filter_desc, dec_ctx));
        return filter;
    }

    int add_frame(AVFrame* frame) {
        auto ret = 0;
        if ((ret = av_buffersrc_add_frame(abuffer_src_ctx, frame)) < 0) {
            av_log(nullptr, AV_LOG_ERROR, "%s %s", __FUNCTION__, av_err2str(ret));
        }
        return ret;
    }

    AVFrame* get_frame() {
        auto frame = av_frame_alloc();
        auto ret = 0;
        if ((ret = av_buffersink_get_frame(abuffer_sink_ctx, frame)) < 0) {
            av_frame_free(&frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                return nullptr;
            } else {
                throw std::invalid_argument(::av_err2str(ret));
            }
        }
        return frame;
    }

    virtual ~Filter() {
        if (!abuffer_src_ctx) {
            avfilter_free(abuffer_src_ctx);
        }
        if (!abuffer_sink_ctx) {
            avfilter_free(abuffer_sink_ctx);
        }
        if (!filter_graph) {
            avfilter_graph_free(&filter_graph);
        }
    }
private:
    Filter(const std::string& filter_desc, const AVCodecContext* dec_ctx) {
        auto ret = 0;

        auto abuffer = avfilter_get_by_name("abuffer");
        auto abuffer_sink = avfilter_get_by_name("abuffersink");
        auto inputs = avfilter_inout_alloc();
        auto outputs = avfilter_inout_alloc();
        auto filter_graph = avfilter_graph_alloc();

        if (!inputs || !outputs || !filter_graph) {
            throw std::runtime_error(av_err2str(AVERROR(ENOMEM)));
        }

        abuffer_src_ctx = nullptr;
        abuffer_sink_ctx = nullptr;

        auto params = av_buffersrc_parameters_alloc();
        params->time_base = av_make_q(1, dec_ctx->sample_rate);
        params->sample_rate = dec_ctx->sample_rate;
        params->format = dec_ctx->sample_fmt;
        params->channel_layout = dec_ctx->channel_layout;

        char args[512] {};
        snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%d:channel_layout=%" PRIu64,
                 1, dec_ctx->sample_rate,
                 dec_ctx->sample_rate, dec_ctx->sample_fmt, dec_ctx->channel_layout);

        if ((ret = avfilter_graph_create_filter(&abuffer_src_ctx, abuffer, "abuffer", args, nullptr, filter_graph)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        if ((ret = avfilter_graph_create_filter(&abuffer_sink_ctx, abuffer_sink, "abuffersink", nullptr, nullptr, filter_graph)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        outputs->name = av_strdup("in");
        outputs->pad_idx = 0;
        outputs->next = nullptr;
        outputs->filter_ctx = abuffer_src_ctx;

        inputs->name = av_strdup("out");
        inputs->pad_idx = 0;
        inputs->next = nullptr;
        inputs->filter_ctx = abuffer_sink_ctx;

        if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_desc.c_str(), &inputs, &outputs, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        if ((ret = avfilter_graph_config(filter_graph, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }
        av_buffersink_set_frame_size(abuffer_sink_ctx, 1024);

        av_free(params);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
    }

    AVFilterGraph* filter_graph {};
    AVFilterContext* abuffer_src_ctx {};
    AVFilterContext* abuffer_sink_ctx {};
};

static Sampler* ptr_sampler = nullptr;
static Encoder* ptr_encoder = nullptr;
static Filter* ptr_filter = nullptr;

void decode_callback(AVFormatContext* ifmt_ctx, AVCodecContext* dec_ctx, AVFrame* dec_frame, int stream_index)
{
    if (dec_frame) {
        if (ptr_sampler->enqueue_frame(dec_frame) > 0) {
            while (auto enc_frame = ptr_sampler->dequeue_frame()) {
                ptr_encoder->write_frame(enc_frame);
            }
        }
    } else {
        ptr_encoder->write_frame(nullptr);
    }
}

int main(int argc, char* argv[])
{
    const char* input_path = nullptr;
    const char* output_path {};
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
        std::exit(EXIT_FAILURE);
    } else {
        input_path = argv[1];
        output_path = argv[2];
    }

    av_register_all();
    avcodec_register_all();
    avfilter_register_all();

    auto enc_params = avcodec_parameters_alloc();
    enc_params->codec_id = AV_CODEC_ID_AAC;
    enc_params->codec_type = AVMEDIA_TYPE_AUDIO;
    enc_params->codec_tag = 0;
    enc_params->sample_rate = 44100;
    enc_params->frame_size = 1024;
    enc_params->channel_layout = AV_CH_LAYOUT_STEREO;
    enc_params->channels = 2;
    enc_params->format = AV_SAMPLE_FMT_FLTP;
    enc_params->bit_rate = 64 * 1000;

    char filter_desc[512] {};
    snprintf(filter_desc, sizeof(filter_desc), "aformat=sample_fmts=%s:sample_rates=%d:channel_layouts=%d",
             av_get_sample_fmt_name(AV_SAMPLE_FMT_FLTP), 44100, AV_CH_LAYOUT_MONO);

    auto decoder = Decoder::New(input_path, AVMEDIA_TYPE_AUDIO);
    auto encoder = Encoder::New(output_path, nullptr, nullptr, AVMEDIA_TYPE_AUDIO, enc_params);
    auto sampler = Sampler::New(decoder->get_decoder_context(), encoder->get_encoder_context());
    ptr_sampler = sampler.get();
    ptr_encoder = encoder.get();

    decoder->read_frame(make_callback(decode_callback));

    return EXIT_SUCCESS;
}
