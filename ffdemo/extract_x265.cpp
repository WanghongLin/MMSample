//
// Created by Wanghong Lin on 2018/6/26.
//


#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
};
#endif


static void av_check_error(int error, const char* what, bool force_to_exit)
{
    if (error != 0) {
        char errbuf[128] = { '0' };
        av_make_error_string(errbuf, sizeof(errbuf), error);
        av_log(nullptr, AV_LOG_ERROR, "%s: %s\n", errbuf, what);
        if (force_to_exit) {
            std::exit(0);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        av_log(nullptr, AV_LOG_ERROR, "Usage: %s <input> <output>\n", argv[0]);
        std::exit(0);
    }

    av_register_all();
    avcodec_register_all();

    const char* input_url = argv[1];
    const char* output_url = argv[2];

    int ret = 0;
    AVFormatContext* ifmt_ctx = avformat_alloc_context();
    AVCodecContext* dec_ctx = nullptr;
    AVCodec* dec = nullptr;
    int video_stream_index = -1;

    AVFormatContext* ofmt_ctx = avformat_alloc_context();
    AVCodecContext* enc_ctx = nullptr;
    AVCodec* enc = nullptr;

    ret = avformat_open_input(&ifmt_ctx, input_url, nullptr, nullptr);
    av_check_error(ret, "open input", true);
    avformat_find_stream_info(ifmt_ctx, nullptr);
    av_dump_format(ifmt_ctx, -1, input_url, 0);

    ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    for (unsigned i = 0; i < ifmt_ctx->nb_streams; ++i) {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            if (AVERROR(ret) == AVERROR_DECODER_NOT_FOUND) {
                av_log(nullptr, AV_LOG_INFO, "find decoder manually\n");
                dec = avcodec_find_decoder(ifmt_ctx->streams[i]->codecpar->codec_id);
            }
            break;
        }
    }

    if (!dec || video_stream_index == -1) {
        av_log(nullptr, AV_LOG_ERROR, "no decoder found\n");
        std::exit(0);
    }

    dec_ctx = avcodec_alloc_context3(dec);
    avcodec_parameters_to_context(dec_ctx, ifmt_ctx->streams[video_stream_index]->codecpar);
    ret = avcodec_open2(dec_ctx, dec, nullptr);
    av_check_error(ret, "open decoder\n", true);


    // prepare encoder
    AVOutputFormat* ofmt = av_guess_format("mp4", output_url, "video/mp4");
    ret = avformat_alloc_output_context2(&ofmt_ctx, ofmt, "mp4", output_url);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "allocate output context error\n");
        std::exit(0);
    }

    enc = avcodec_find_encoder(AV_CODEC_ID_H265);
    if (!enc) {
        av_log(nullptr, AV_LOG_ERROR, "encoder not found\n");
        std::exit(0);
    }

    enc_ctx = avcodec_alloc_context3(enc);
    enc_ctx->width = dec_ctx->width / 2;
    enc_ctx->height = dec_ctx->height / 2;
    enc_ctx->framerate = AVRational {25, 1};
    enc_ctx->time_base = av_make_q(1, 25);
    enc_ctx->pix_fmt = enc->pix_fmts[0];
    enc_ctx->bit_rate = 280 * 1000;
    enc_ctx->codec = enc;
    enc_ctx->codec_id = AV_CODEC_ID_H265;
    enc_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    enc_ctx->codec_tag = 0;

    AVStream* stream = avformat_new_stream(ofmt_ctx, enc);

    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    ret = avcodec_open2(enc_ctx, enc, nullptr);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_ERROR, "open encoder error\n");
        std::exit(0);
    }
    avcodec_parameters_from_context(stream->codecpar, enc_ctx);
    stream->codec = enc_ctx;
    stream->time_base = enc_ctx->time_base;
    av_dump_format(ofmt_ctx, 0, output_url, 1);

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {

        ret = avio_open2(&ofmt_ctx->pb, output_url, AVIO_FLAG_WRITE, nullptr, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "AV IO error\n");
            std::exit(0);
        }
        ret = avformat_write_header(ofmt_ctx, nullptr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_ERROR, "av write header\n");
            std::exit(0);
        }
    }

    AVPacket* packet = av_packet_alloc();
    av_init_packet(packet);
    packet->data = nullptr;
    packet->size = 0;

    SwsContext* sws_ctx = sws_getContext(dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                                         enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt,
                                         SWS_BICUBLIN, nullptr, nullptr, nullptr);

    long pts = 0;
    while (av_read_frame(ifmt_ctx, packet) == 0) {
        if (packet->stream_index != video_stream_index) {
            continue;
        }

        int send_ret;
        do {
            send_ret = avcodec_send_packet(dec_ctx, packet);
            if (send_ret == 0) {
                AVFrame* frame = av_frame_alloc();
                int recv_ret;
                do {
                    recv_ret = avcodec_receive_frame(dec_ctx, frame);
                    if (recv_ret == 0) {
                        AVFrame* enc_frame = av_frame_alloc();
                        av_image_alloc(enc_frame->data, enc_frame->linesize, enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt, 32);
                        // long pts = av_frame_get_best_effort_timestamp(frame);
                        // av_log(nullptr, AV_LOG_INFO, "decode one frame %li %dx%d\n", pts, frame->width, frame->height);
                        int h = sws_scale(sws_ctx, frame->data, frame->linesize, 0, dec_ctx->height, enc_frame->data, enc_frame->linesize);
                        if (h <= 0) {
                            av_log(nullptr, AV_LOG_ERROR, "swscale error\n");
                        } else {
                            av_log(nullptr, AV_LOG_INFO, "swscale ok %d\n", h);
                        }

                        enc_frame->pts = pts++;
                        enc_frame->width = enc_ctx->width;
                        enc_frame->height = enc_ctx->height;
                        enc_frame->format = enc_ctx->pix_fmt;

                        if (avcodec_send_frame(enc_ctx, enc_frame) == 0) {
                            AVPacket* enc_packet = av_packet_alloc();

                            if (avcodec_receive_packet(enc_ctx, enc_packet) == 0) {
                                enc_packet->stream_index = 0;
                                av_packet_rescale_ts(enc_packet, enc_ctx->time_base, ofmt_ctx->streams[0]->time_base);
                                av_interleaved_write_frame(ofmt_ctx, enc_packet);
                            }

                            av_packet_free(&enc_packet);
                        }
                        av_freep(&enc_frame->data[0]);
                        av_frame_free(&enc_frame);
                    }
                } while (AVERROR(recv_ret) == AVERROR(EAGAIN));
                av_frame_free(&frame);
            }
        } while (AVERROR(send_ret) == AVERROR(EAGAIN));
        av_packet_unref(packet);
    }

    av_write_trailer(ofmt_ctx);

    av_packet_free(&packet);
    avcodec_close(dec_ctx);
    avformat_close_input(&ifmt_ctx);
    avcodec_close(enc_ctx);
    avformat_free_context(ofmt_ctx);

    return 0;
}
