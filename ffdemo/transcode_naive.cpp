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
// Created by Wanghong Lin on 2019/12/20.
//
//
// a simple and naive implementation for transcoding
//
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavutil/opt.h"

#ifdef __cplusplus
};
#endif

static AVFilterContext* buffer_src_ctx = nullptr;
static AVFilterContext* buffer_sink_ctx = nullptr;
static AVFilterGraph* filter_graph = nullptr;

int init_filters(const char* filter_desc, AVCodecContext* dec_codec_ctx, AVStream* stream)
{
    auto ret = 0;
    auto buffer_src = avfilter_get_by_name("buffer");
    auto buffer_sink = avfilter_get_by_name("buffersink");
    auto outputs = avfilter_inout_alloc();
    auto inputs = avfilter_inout_alloc();
    filter_graph = avfilter_graph_alloc();

    char args[512] { 0 };

    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             dec_codec_ctx->width, dec_codec_ctx->height, dec_codec_ctx->pix_fmt,
             stream->time_base.num, stream->time_base.den,
             dec_codec_ctx->sample_aspect_ratio.num, dec_codec_ctx->sample_aspect_ratio.den);

    if ((ret = avfilter_graph_create_filter(&buffer_src_ctx, buffer_src, "in", args, nullptr, filter_graph)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "create buffersrc error %s", av_err2str(ret));
    }
    if ((ret = avfilter_graph_create_filter(&buffer_sink_ctx, buffer_sink, "out", nullptr, nullptr, filter_graph)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "create buffersink error %s", av_err2str(ret));
    }

    outputs->name = av_strdup("in");
    outputs->next = nullptr;
    outputs->pad_idx = 0;
    outputs->filter_ctx = buffer_src_ctx;

    inputs->name = av_strdup("out");
    inputs->next = nullptr;
    inputs->pad_idx = 0;
    inputs->filter_ctx = buffer_sink_ctx;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_desc, &inputs, &outputs, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "parse graph filter error %s", av_err2str(ret));
        return ret;
    }
    if ((ret = avfilter_graph_config(filter_graph, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "config filter graph error %s for %s\n", av_err2str(ret), filter_desc);
        return ret;
    }

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

static std::vector<std::string> check_and_obtain_arguments(int argc, char* argv[])
{
    std::vector<std::string> args;
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::copy(argv+1, argv+argc, std::back_inserter(args));
    return args;
}

int main(int argc, char* argv[])
{
    auto args = check_and_obtain_arguments(argc, argv);

    const char* input = args[0].c_str();
    const char* output = args[1].c_str();

    av_register_all();
    avcodec_register_all();
    avfilter_register_all();

    auto ret = 0;
    auto dec_stream_index = 0;

    AVFormatContext* ifmt_ctx = avformat_alloc_context();
    AVFormatContext* ofmt_ctx = nullptr;
    AVInputFormat* ifmt = av_find_input_format("mp4");
    AVOutputFormat* ofmt = av_guess_format("mp4", output, "video/mp4");
    AVCodec* dec_codec = nullptr;
    AVCodec* enc_codec = nullptr;
    AVCodecContext* dec_codec_ctx = nullptr;
    AVCodecContext* enc_codec_ctx = nullptr;

    AVPacket* dec_packet = av_packet_alloc();
    AVPacket* enc_packet = av_packet_alloc();

    if ((ret = avformat_open_input(&ifmt_ctx, input, ifmt, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "open input %s", av_err2str(ret));
        return EXIT_FAILURE;
    }

    avformat_find_stream_info(ifmt_ctx, nullptr);

    ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec_codec, 0);
    if (ret == AVERROR_STREAM_NOT_FOUND) {
        av_log(nullptr, AV_LOG_ERROR, "stream not found %s", av_err2str(ret));
        return EXIT_FAILURE;
    }

    if (ret == AVERROR_DECODER_NOT_FOUND && dec_codec == nullptr) {
        for (auto i = 0; i < ifmt_ctx->nb_streams; i++) {
            if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                dec_stream_index = i;
                dec_codec = avcodec_find_decoder(ifmt_ctx->streams[i]->codecpar->codec_id);
                break;
            }
        }
    }

    if (dec_codec == nullptr) {
        av_log(nullptr, AV_LOG_ERROR, "no decoder found");
        return EXIT_FAILURE;
    }

    dec_codec_ctx = avcodec_alloc_context3(dec_codec);
    avcodec_parameters_to_context(dec_codec_ctx, ifmt_ctx->streams[dec_stream_index]->codecpar);
    if ((ret = avcodec_open2(dec_codec_ctx, dec_codec, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "open decoder %s", av_err2str(ret));
        return EXIT_FAILURE;
    }

    if (!avcodec_is_open(dec_codec_ctx)) {
        return EXIT_FAILURE;
    }

    if ((ret = avformat_alloc_output_context2(&ofmt_ctx, ofmt, "mp4", output)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "allocate output context error %s", av_err2str(ret));
        return EXIT_FAILURE;
    }

    enc_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    enc_codec_ctx = avcodec_alloc_context3(enc_codec);
    enc_codec_ctx->codec_id = enc_codec->id;
    enc_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    enc_codec_ctx->codec_tag = 0;
    enc_codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc_codec_ctx->width = dec_codec_ctx->width/2;
    enc_codec_ctx->height = dec_codec_ctx->height/2;
    enc_codec_ctx->framerate = av_make_q(25, 1);
    enc_codec_ctx->time_base = av_inv_q(enc_codec_ctx->framerate);
    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        enc_codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    if ((ret = avcodec_open2(enc_codec_ctx, enc_codec, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "open encoder %s", av_err2str(ret));
        return EXIT_FAILURE;
    }

    auto stream = avformat_new_stream(ofmt_ctx, enc_codec);
    if ((ret = avcodec_parameters_from_context(stream->codecpar, enc_codec_ctx)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "fill codec parameters error %s", av_err2str(ret));
        return EXIT_FAILURE;
    }
    stream->codec = enc_codec_ctx;
    stream->time_base = enc_codec_ctx->time_base;

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open2(&ofmt_ctx->pb, output, AVIO_FLAG_WRITE, nullptr, nullptr)) < 0) {
            av_log(nullptr, AV_LOG_ERROR, "io open error %s", av_err2str(ret));
            return EXIT_FAILURE;
        }
    }

    if ((ret = avformat_write_header(ofmt_ctx, nullptr) < 0)) {
        av_log(nullptr, AV_LOG_ERROR, "write header error %s", av_err2str(ret));
        return EXIT_FAILURE;
    }

    char filter[512] { 0 };
    sprintf(filter, "scale=%d:%d", enc_codec_ctx->width, enc_codec_ctx->height);
    if ((ret = init_filters(filter, dec_codec_ctx, ifmt_ctx->streams[dec_stream_index])) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "init filter error %s", av_err2str(ret));
        return EXIT_FAILURE;
    }
    av_init_packet(dec_packet);
    dec_packet->size = 0;
    dec_packet->data = nullptr;
    auto pts = 0L;

    while (av_read_frame(ifmt_ctx, dec_packet) == 0) {
        if (dec_packet->stream_index == dec_stream_index) {
            if ((ret = avcodec_send_packet(dec_codec_ctx, dec_packet)) == 0) {
                auto dec_frame = av_frame_alloc();
                auto enc_frame = av_frame_alloc();
                if (avcodec_receive_frame(dec_codec_ctx, dec_frame) == 0) {
                    if ((ret = av_buffersrc_add_frame(buffer_src_ctx, dec_frame)) == 0) {

                        if ((ret = av_buffersink_get_frame(buffer_sink_ctx, enc_frame)) == 0) {

                            enc_frame->width = enc_codec_ctx->width;
                            enc_frame->height = enc_codec_ctx->height;
                            enc_frame->pts = pts++;
                            enc_frame->format = enc_codec_ctx->pix_fmt;
                            enc_frame->pict_type = dec_frame->pict_type;

                            if ((ret = avcodec_send_frame(enc_codec_ctx, enc_frame)) == 0) {
                                av_init_packet(enc_packet);
                                enc_packet->size = 0;
                                enc_packet->data = nullptr;
                                if ((ret = avcodec_receive_packet(enc_codec_ctx, enc_packet)) == 0) {
                                    enc_packet->stream_index = 0;
                                    av_packet_rescale_ts(enc_packet, enc_codec_ctx->time_base,
                                                         ofmt_ctx->streams[0]->time_base);
                                    av_interleaved_write_frame(ofmt_ctx, enc_packet);
                                } else {
                                    if (ret != AVERROR(EAGAIN)) {
                                        av_log(nullptr, AV_LOG_ERROR, "receive packet error %s\n", av_err2str(ret));
                                    }
                                }
                                av_packet_unref(enc_packet);
                            } else {
                                av_log(nullptr, AV_LOG_ERROR, "send frame error %s\n", av_err2str(ret));
                            }

                        } else {
                            av_log(nullptr, AV_LOG_ERROR, "buffersink get frame error %s\n", av_err2str(ret));
                        }
                    } else {
                        av_log(nullptr, AV_LOG_ERROR, "buffersrc add frame error %s\n", av_err2str(ret));
                    }
                }

                av_frame_free(&dec_frame);
                av_frame_free(&enc_frame);
            } else {
                av_log(nullptr, AV_LOG_ERROR, "send packet %s\n", av_err2str(ret));
            }
        }
        av_packet_unref(dec_packet);
    }

    av_interleaved_write_frame(ofmt_ctx, nullptr);
    av_write_trailer(ofmt_ctx);

    avfilter_free(buffer_src_ctx);
    avfilter_free(buffer_sink_ctx);
    avfilter_graph_free(&filter_graph);
    avcodec_free_context(&dec_codec_ctx);
    avcodec_free_context(&enc_codec_ctx);
    // avformat_free_context(ofmt_ctx);
    avformat_free_context(ifmt_ctx);

    return EXIT_SUCCESS;
}
