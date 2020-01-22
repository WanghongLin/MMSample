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
// Created by Wanghong Lin on 2019/12/27.
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <memory>
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"

#ifdef __cplusplus
};
#endif

/**
 * The decoder part
 */
class Decoder {
public:
    /**
     * The decoder callback to receive one frame for further processing
     */
    struct Callback {
        template<typename FUNC>
        explicit Callback(FUNC&& func) : _func(func) {}
        void call(const AVFormatContext* ifmt_ctx, const AVCodecContext* dec_ctx, const AVFrame* frame, const int stream_index) {
            if(_func) _func(ifmt_ctx, dec_ctx, frame, stream_index);
        };
    private:
        std::function<void(const AVFormatContext*,const AVCodecContext*,const AVFrame*,const int)> _func;
    };

    /**
     * Create a new decoder from provided media path and media type
     * @param input_path the input path
     * @param media_type the media type
     * @return unique pointer to decoder
     */
    static std::unique_ptr<Decoder> New(const std::string& input_path, const AVMediaType media_type) {
        return std::unique_ptr<Decoder>(new Decoder(input_path, media_type));
    }

    /**
     * Read one frame from decoder, the new frame received in the callback
     * @param callback the callback to receive the new frame
     */
    void read_frame(Callback&& callback) {
        auto ret = 0;
        auto pkt = av_packet_alloc();
        av_init_packet(pkt);
        pkt->size = 0;
        pkt->data = nullptr;

        while (av_read_frame(ifmt_ctx, pkt) == 0) {
            if (pkt->stream_index == target_stream_index) {
                ret = avcodec_send_packet(dec_ctx, pkt);
                if (ret < 0) {
                    throw std::runtime_error(av_err2str(ret));
                }

                while (ret >= 0) {
                    auto frame = av_frame_alloc();
                    ret = avcodec_receive_frame(dec_ctx, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    } else if (ret < 0) {
                        throw std::runtime_error(av_err2str(ret));
                    }

                    callback.call(ifmt_ctx, dec_ctx, frame, target_stream_index);

                    av_frame_free(&frame);
                }
            }
            av_packet_unref(pkt);
        }

        callback.call(ifmt_ctx, dec_ctx, nullptr, target_stream_index);
    }

    AVCodecContext* get_codec_context() { return dec_ctx; }

    virtual ~Decoder() {
        if (ifmt_ctx) {
            avformat_free_context(ifmt_ctx);
            ifmt_ctx = nullptr;
        }

        if (dec_ctx) {
            if (avcodec_is_open(dec_ctx)) {
                avcodec_close(dec_ctx);
            }
            avcodec_free_context(&dec_ctx);
        }
    }
private:
    explicit Decoder(const std::string& input_path, AVMediaType media_type) {
        auto ret = 0;

        AVInputFormat* ifmt = av_find_input_format("mp4");
        ifmt_ctx = avformat_alloc_context();
        if ((ret = avformat_open_input(&ifmt_ctx, input_path.c_str(), ifmt, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        if ((ret = avformat_find_stream_info(ifmt_ctx, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        AVCodec* decoder {};
        ret = av_find_best_stream(ifmt_ctx, media_type, -1, -1, &decoder, 0);
        if (ret == AVERROR_STREAM_NOT_FOUND) {
            throw std::runtime_error(av_err2str(ret));
        }

        for (unsigned i = 0; i < ifmt_ctx->nb_streams; ++i) {
            if (ifmt_ctx->streams[i]->codecpar->codec_type == media_type) {
                target_stream_index = i;

                if (ret == AVERROR_DECODER_NOT_FOUND || !decoder) {
                    decoder = avcodec_find_decoder(ifmt_ctx->streams[i]->codecpar->codec_id);
                }

                break;
            }
        }

        if (!decoder) {
            throw std::runtime_error("decoder not found");
        }

        if ((dec_ctx = avcodec_alloc_context3(decoder)) == nullptr) {
            throw std::runtime_error(av_err2str(AVERROR(ENOMEM)));
        }

        if ((ret = avcodec_parameters_to_context(dec_ctx, ifmt_ctx->streams[target_stream_index]->codecpar)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        if ((ret = avcodec_open2(dec_ctx, decoder, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }
    }

    AVFormatContext* ifmt_ctx {};
    AVCodecContext* dec_ctx {};
    int target_stream_index {};
};

template<typename FUNC>
Decoder::Callback make_callback(FUNC&& func)
{
    return Decoder::Callback(std::forward<FUNC>(func));
}

/**
 * The scaler part
 */
class Scaler {
public:
    static std::unique_ptr<Scaler> New(const AVCodecContext* dec_ctx, const AVCodecContext* enc_ctx) {
        return std::unique_ptr<Scaler>(new Scaler(dec_ctx, enc_ctx));
    }

    AVFrame* scale_frame(const AVFrame* in_frame) {
        auto out_frame = av_frame_alloc();
        auto ret = 0;

        if ((ret = av_image_alloc(out_frame->data, out_frame->linesize,
                                  enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt, 1)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        ret = sws_scale(sws_ctx, in_frame->data, in_frame->linesize, 0,
                           in_frame->height, out_frame->data, out_frame->linesize);
        if (ret <= 0) {
            av_frame_free(&out_frame);
            throw std::runtime_error(av_err2str(ret));
        }

        return out_frame;
    }

    virtual ~Scaler() {
        if (sws_ctx) {
            sws_freeContext(sws_ctx);
            sws_ctx = nullptr;
        }
    }
private:
    Scaler(const AVCodecContext* dec_ctx, const AVCodecContext* enc_ctx) {
        sws_ctx = sws_getContext(dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                                 enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt,
                                 SWS_BILINEAR, nullptr, nullptr, nullptr);
        this->enc_ctx = enc_ctx;
        if (!sws_ctx) {
            throw std::runtime_error(av_err2str(AVERROR(ENOMEM)));
        }
    }

    const AVCodecContext* enc_ctx {};
    SwsContext* sws_ctx {};
};

/**
 * C++ wrapper for encoder part
 */
class Encoder {
public:
    /**
     * Create a encoder with output path specified parameters
     * @param output_path output path
     * @param parameters encoder parameters
     * @return pointer to encoder
     */
    static std::unique_ptr<Encoder> New(const std::string& output_path, const AVCodecParameters* parameters) {
        return std::unique_ptr<Encoder>(new Encoder(output_path, parameters));
    }

    /**
     * Write a frame
     * @param enc_frame frame
     */
    void write_frame(AVFrame* enc_frame) {
        if (enc_frame) {
            enc_frame->width = enc_ctx->width;
            enc_frame->height = enc_ctx->height;
            enc_frame->format = enc_ctx->pix_fmt;
            enc_frame->pts = pts++;
        }

        auto ret = 0;
        ret = avcodec_send_frame(enc_ctx, enc_frame);
        if (ret < 0) {
            throw std::runtime_error(av_err2str(ret));
        }
        while (ret >= 0) {
            AVPacket pkt = { .size = 0, .data = nullptr };
            av_init_packet(&pkt);
            ret = avcodec_receive_packet(enc_ctx, &pkt);
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                if (ret == AVERROR_EOF) {
                    if ((ret = av_write_trailer(ofmt_ctx)) < 0) {
                        throw std::runtime_error(av_err2str(ret));
                    }
                }
                return;
            } else if (ret < 0) {
                throw std::runtime_error(av_err2str(ret));
            }

            av_packet_rescale_ts(&pkt, enc_ctx->time_base, ofmt_ctx->streams[0]->time_base);
            if ((ret = av_interleaved_write_frame(ofmt_ctx, &pkt)) < 0) {
                throw std::runtime_error(av_err2str(ret));
            }
        }
    }

    AVCodecContext* get_codec_context() { return enc_ctx; }

    virtual ~Encoder() {
        if (enc_ctx) {
            if (avcodec_is_open(enc_ctx)) {
                avcodec_close(enc_ctx);
            }
            avcodec_free_context(&enc_ctx);
            enc_ctx = nullptr;
        }
    }
private:
    explicit Encoder(const std::string& output_path, const AVCodecParameters* parameters) {
        auto oformat = av_guess_format("mp4", output_path.c_str(), "video/mp4");
        auto ret = 0;
        if ((ret = avformat_alloc_output_context2(&ofmt_ctx, oformat, "mp4", output_path.c_str())) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        auto encoder = avcodec_find_encoder(parameters->codec_id);
        if (!encoder) {
            throw std::runtime_error("no such encoder");
        }

        if ((enc_ctx = avcodec_alloc_context3(encoder)) == nullptr) {
            throw std::runtime_error("allocate encoder context");
        }

        if ((ret = avcodec_parameters_to_context(enc_ctx, parameters)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }
        enc_ctx->framerate = av_make_q(25, 1);
        enc_ctx->time_base = av_inv_q(enc_ctx->framerate);
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
        av_opt_set(enc_ctx->priv_data, "profile", "baseline", AV_OPT_SEARCH_CHILDREN);
        av_opt_set(enc_ctx->priv_data, "preset", "slow", AV_OPT_SEARCH_CHILDREN);

        if ((ret = avcodec_open2(enc_ctx, encoder, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        auto stream = avformat_new_stream(ofmt_ctx, encoder);
        if ((ret = avcodec_parameters_from_context(stream->codecpar, enc_ctx)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }
        stream->codec = enc_ctx;
        stream->time_base = enc_ctx->time_base;

        if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            if ((ret = avio_open2(&ofmt_ctx->pb, output_path.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr)) < 0) {
                throw std::runtime_error(av_err2str(ret));
            }
        }

        if ((ret = avformat_write_header(ofmt_ctx, nullptr)) < 0) {
            throw std::runtime_error(av_err2str(ret));
        }

        av_dump_format(ofmt_ctx, 0, output_path.c_str(), 1);
    }

    AVFormatContext* ofmt_ctx {};
    AVCodecContext* enc_ctx {};
    int64_t pts {};
};

int main(int argc, char* argv[])
{
    const char* input_path {};
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

    auto decoder = Decoder::New(input_path, AVMEDIA_TYPE_VIDEO);

    auto parameters = avcodec_parameters_alloc();
    parameters->codec_id = AV_CODEC_ID_H264;
    parameters->codec_type = AVMEDIA_TYPE_VIDEO;
    parameters->codec_tag = 0;
    parameters->width = 320;
    parameters->height = 180;
    parameters->format = AV_PIX_FMT_YUV420P;
    parameters->bit_rate = 320 * 1000;
    auto encoder = Encoder::New(output_path, parameters);
    auto scaler = Scaler::New(decoder->get_codec_context(), encoder->get_codec_context());

    auto ptr_encoder = encoder.get();
    auto ptr_scaler = scaler.get();

    decoder->read_frame(make_callback([ptr_encoder, ptr_scaler](const AVFormatContext* ifmt_ctx, const AVCodecContext* dec_ctx,
                                                                const AVFrame* frame, const int stream_index) {
        if (frame) {
            ptr_encoder->write_frame(ptr_scaler->scale_frame(frame));
        } else {
            ptr_encoder->write_frame(nullptr);
        }
    }));
    return EXIT_SUCCESS;
}
