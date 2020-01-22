//
// Created by Wanghong Lin on 2019/9/25.
//

#include <iostream>
#include <tuple>

#ifdef __cplusplus
extern "C" {
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
};
#endif

static AVFilterGraph* g_filter_graph = nullptr;
static AVFilterContext* g_buffersrc_ctx = nullptr;
static AVFilterContext* g_buffersink_ctx = nullptr;

static int init_filters(AVStream* stream, AVCodecContext* dec_ctx, const char* filters_desc)
{
    AVFilterGraph* filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        return AVERROR(ENOMEM);
    }

    const AVFilter* buffersrc = avfilter_get_by_name("buffer");
    const AVFilter* buffersink = avfilter_get_by_name("buffersink");

    AVFilterContext* buffersrc_ctx = nullptr;
    AVFilterContext* buffersink_ctx = nullptr;

    int ret = 0;
    char args[512] = { 0 };
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
             stream->time_base.num, stream->time_base.den,
             dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);
    std::cout << args << std::endl;
    if ((ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "anyone", args, nullptr, filter_graph)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Could not create buffer source %s\n", av_err2str(ret));
        avfilter_graph_free(&filter_graph);
        return ret;
    }

    if ((ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "anything", nullptr, nullptr, filter_graph)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Could not create buffer sink %s\n", av_err2str(ret));
        avfilter_graph_free(&filter_graph);
        return ret;
    }

    AVFilterInOut* inputs = avfilter_inout_alloc();
    AVFilterInOut* outputs = avfilter_inout_alloc();
    if (!inputs || !outputs) {
        return AVERROR(ENOMEM);
    }

    outputs->filter_ctx = buffersrc_ctx;
    outputs->name = av_strdup("in");
    outputs->next = nullptr;
    outputs->pad_idx = 0;

    inputs->filter_ctx = buffersink_ctx;
    inputs->name = av_strdup("out");
    inputs->next = nullptr;
    inputs->pad_idx = 0;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_desc, &inputs, &outputs, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Could not parse graph %s\n", av_err2str(ret));
        return ret;
    }

    if ((ret = avfilter_graph_config(filter_graph, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Could not config graph %s\n", av_err2str(ret));
        return ret;
    }

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    g_filter_graph = filter_graph;
    g_buffersrc_ctx = buffersrc_ctx;
    g_buffersink_ctx = buffersink_ctx;

    return ret;
}

int main(int argc, char* argv[])
{
    const char* path = nullptr;
    const char* out_path = nullptr;
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
        std::exit(EXIT_FAILURE);
    } else {
        path = argv[1];
        out_path = argv[2];
    }

    av_register_all();
    avcodec_register_all();

    auto err = 0;

    AVFormatContext* ifmt_ctx = avformat_alloc_context();
    err = avformat_open_input(&ifmt_ctx, path, nullptr, nullptr);
    if (err < 0) {
        av_log(nullptr, AV_LOG_ERROR, "open input %s\n", av_err2str(err));
        return EXIT_FAILURE;
    }

    avformat_find_stream_info(ifmt_ctx, nullptr);
    av_dump_format(ifmt_ctx, -1, path, 0);

    AVCodec* decoder = nullptr;
    auto video_index = 0;
    err = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (err == AVERROR_STREAM_NOT_FOUND) {
        return EXIT_FAILURE;
    }

    for (unsigned i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            if (err == AVERROR_DECODER_NOT_FOUND) {
                decoder = avcodec_find_decoder(ifmt_ctx->streams[i]->codecpar->codec_id);
            }
            break;
        }
    }

    if (decoder == nullptr) {
        av_log(nullptr, AV_LOG_ERROR, "could not find decoder\n");
        return EXIT_FAILURE;
    }

    AVCodecContext* dec_ctx = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(dec_ctx, ifmt_ctx->streams[video_index]->codecpar);
    err = avcodec_open2(dec_ctx, decoder, nullptr);
    if (err < 0) {
        av_log(nullptr, AV_LOG_ERROR, "avcodec open error %s\n", av_err2str(err));
        return EXIT_FAILURE;
    }

    AVPacket* pkt = av_packet_alloc();
    av_init_packet(pkt);
    pkt->size = 0;
    pkt->data = nullptr;

    avfilter_register_all();
    auto filter = avfilter_get_by_name("greyscale");
    if (avfilter_register(const_cast<AVFilter *>(filter)) != 0) {
        av_log(nullptr, AV_LOG_ERROR, "register filter error");
    }

    if ((err = init_filters(ifmt_ctx->streams[video_index], dec_ctx, "greyscale") < 0)) {
        av_log(nullptr, AV_LOG_ERROR, "init filters error %s\n", av_err2str(err));
    }

    AVOutputFormat* ofmt = av_guess_format("mp4", out_path, "video/mp4");
    AVFormatContext* ofmt_ctx = avformat_alloc_context();
    avformat_alloc_output_context2(&ofmt_ctx, ofmt, "mp4", out_path);

    AVCodec* encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    AVCodecContext* enc_ctx = avcodec_alloc_context3(encoder);

    enc_ctx->codec_id = encoder->id;
    enc_ctx->codec_type = encoder->type;
    enc_ctx->codec_tag = 0;
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc_ctx->width = dec_ctx->width;
    enc_ctx->height = dec_ctx->height;
    enc_ctx->bit_rate = dec_ctx->bit_rate;
    enc_ctx->framerate = AVRational {30, 1};
    enc_ctx->time_base = av_inv_q(enc_ctx->framerate);

    if (ofmt_ctx->flags & AVFMT_GLOBALHEADER) {
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if ((err = avcodec_open2(enc_ctx, encoder, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "open codec error %s\n", av_err2str(err));
        return EXIT_FAILURE;
    }

    AVStream* stream = avformat_new_stream(ofmt_ctx, encoder);
    avcodec_parameters_from_context(stream->codecpar, enc_ctx);

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((err = avio_open(&ofmt_ctx->pb, out_path, AVIO_FLAG_WRITE)) < 0) {
            av_log(nullptr, AV_LOG_ERROR, "io open error %s\n", av_err2str(err));
            return EXIT_FAILURE;
        }
    }

    if ((err = avformat_write_header(ofmt_ctx, nullptr)) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "write header error %s\n", av_err2str(err));
        return EXIT_FAILURE;
    }

    av_dump_format(ofmt_ctx, -1, out_path, 1);

    int64_t out_pts = 0;

    while (av_read_frame(ifmt_ctx, pkt) == 0) {
        if (pkt->stream_index == video_index) {

            if (avcodec_send_packet(dec_ctx, pkt) == 0) {
                AVFrame* dec_frame = av_frame_alloc();

                if (avcodec_receive_frame(dec_ctx, dec_frame) == 0) {

                    auto pts = av_rescale_q(av_frame_get_best_effort_timestamp(dec_frame),
                                            ifmt_ctx->streams[video_index]->time_base,
                                            AV_TIME_BASE_Q);
//                    std::cout << pts << std::endl;

                    if (av_buffersrc_add_frame_flags(g_buffersrc_ctx, dec_frame, AV_BUFFERSRC_FLAG_KEEP_REF) == 0) {

                        while (true) {
                            AVFrame* filter_frame = av_frame_alloc();
                            filter_frame->format = enc_ctx->pix_fmt;
                            filter_frame->width = enc_ctx->width;
                            filter_frame->height = enc_ctx->height;
                            av_frame_get_buffer(filter_frame, 0);

                            err = av_buffersink_get_frame(g_buffersink_ctx, filter_frame);
                            if (err == AVERROR_EOF || err == AVERROR(EAGAIN)) {
                                break;
                            }
                            if (err < 0) {
                                return err;
                            }

                            filter_frame->pts = out_pts++;
                            if ((err = avcodec_send_frame(enc_ctx, filter_frame)) >= 0) {

                                AVPacket* enc_pkt = av_packet_alloc();
                                av_init_packet(enc_pkt);
                                enc_pkt->data = nullptr;
                                enc_pkt->size = 0;

                                if ((err = avcodec_receive_packet(enc_ctx, enc_pkt)) >= 0) {
                                    av_packet_rescale_ts(enc_pkt, enc_ctx->time_base, stream->time_base);
                                    av_interleaved_write_frame(ofmt_ctx, enc_pkt);
                                } else {
                                    av_log(nullptr, AV_LOG_ERROR, "encode receive packet %s\n", av_err2str(err));
                                }

                                av_packet_free(&enc_pkt);
                            } else {
                                av_log(nullptr, AV_LOG_ERROR, "encode send frame %s\n", av_err2str(err));
                            }

                            av_frame_unref(filter_frame);
                        }
                    }

                }

                av_frame_free(&dec_frame);
            } else {

            }
        }
    }

    av_write_trailer(ofmt_ctx);

    avfilter_free(g_buffersink_ctx);
    avfilter_free(g_buffersrc_ctx);
    avfilter_graph_free(&g_filter_graph);

    av_free_packet(pkt);
    avformat_free_context(ifmt_ctx);

    return EXIT_SUCCESS;
}