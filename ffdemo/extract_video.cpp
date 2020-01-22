//
// Created by Wanghong Lin on 2019/7/30.

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#ifdef __cplusplus
}
#endif

static std::vector<std::string> parse_arguments(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input output" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return std::vector<std::string>(argv+1, argv+argc);
}

int main(int argc, char* argv[])
{
    auto args = parse_arguments(argc, argv);
    const char* input_path = args[0].c_str();
    const char* output_path = args[1].c_str();

    av_register_all();
    avcodec_register_all();

    AVInputFormat* ifmt = av_find_input_format("video/mp4");
    AVFormatContext* ifmt_ctx = avformat_alloc_context();

    int error = 0;
    error = avformat_open_input(&ifmt_ctx,
                                input_path,
                                ifmt, nullptr);
    if (error < 0) {
        std::cout << av_err2str(error) << std::endl;
    }

    avformat_find_stream_info(ifmt_ctx, nullptr);
    av_dump_format(ifmt_ctx, -1, input_path, 0);

    AVCodec* dec_codec = nullptr;
    AVCodecContext* dec_ctx = avcodec_alloc_context3(nullptr);
    int find_ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec_codec, 0);
    if (find_ret == AVERROR_STREAM_NOT_FOUND) {
        std::cerr << "No video stream found" << std::endl;
    }

    int video_index = -1;
    for (unsigned i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            if (dec_codec == nullptr && find_ret == AVERROR_DECODER_NOT_FOUND) {
                dec_codec = avcodec_find_decoder(ifmt_ctx->streams[i]->codecpar->codec_id);
            }
            break;
        }
    }

    if (video_index == -1) {
        std::cout << "Video stream index not found" << std::endl;
    }

    if (dec_codec == nullptr) {
        std::cout << "decoder not found" << std::endl;
    }

    avcodec_parameters_to_context(dec_ctx, ifmt_ctx->streams[video_index]->codecpar);
    if ((error = avcodec_open2(dec_ctx, dec_codec, nullptr)) < 0) {
        std::cerr << av_err2str(error) << std::endl;
    }

    AVPacket dec_pkt {.data = nullptr, .size = 0};
    av_init_packet(&dec_pkt);

    AVOutputFormat* ofmt = av_guess_format("video/mp4", output_path, "video/mp4");
    AVFormatContext* ofmt_ctx = nullptr;
    error = avformat_alloc_output_context2(&ofmt_ctx, ofmt, "video/mp4", output_path);
    if (error < 0) {
        std::cerr << "Allocate output context" << std::endl;
    }

    AVCodec* enc_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    AVStream* enc_stream = avformat_new_stream(ofmt_ctx, enc_codec);

    AVCodecContext* enc_ctx = avcodec_alloc_context3(enc_codec);
    enc_ctx->codec = enc_codec;
    enc_ctx->codec_id = enc_codec->id;
    enc_ctx->codec_type = enc_codec->type;
    enc_ctx->codec_tag = 0;
    enc_ctx->pix_fmt = enc_codec->pix_fmts[0];
    enc_ctx->width = dec_ctx->width/4;
    enc_ctx->height = dec_ctx->height/4;
    enc_ctx->time_base = av_make_q(1, 25);
    enc_ctx->framerate = av_inv_q(enc_ctx->time_base);
    enc_ctx->bit_rate = 1000 * 1000;
//    enc_ctx->profile |= FF_PROFILE_H264_BASELINE;
//    av_opt_set(enc_ctx->priv_data, "profile", "baseline", 0);

    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    AVDictionary* opts = nullptr;
    av_dict_set(&opts, "profile", "main", 0);
    if (avcodec_open2(enc_ctx, enc_codec, &opts) < 0) {
        std::cerr << "open encoder codec error" << std::endl;
    }
    av_dict_free(&opts);
    avcodec_parameters_from_context(enc_stream->codecpar, enc_ctx);
    enc_stream->codecpar->profile = FF_PROFILE_H264_BASELINE;
    enc_stream->codec = enc_ctx;
    enc_stream->time_base = enc_ctx->time_base;
    av_dump_format(ofmt_ctx, -1, output_path, 1);

    if (avio_open2(&ofmt_ctx->pb, output_path, AVIO_FLAG_WRITE, nullptr, nullptr) < 0) {
        std::cerr << "AVIO open error" << std::endl;
    }
//    avformat_init_output(ofmt_ctx, nullptr);
    error = avformat_write_header(ofmt_ctx, nullptr);
    if (error == AVSTREAM_INIT_IN_INIT_OUTPUT) {
        std::cout << "write header ok" << std::endl;
    }

    std::cout << "tbn " << enc_stream->time_base.num << '/' << enc_stream->time_base.den << std::endl;
    std::cout << "tbc " << enc_ctx->time_base.num << '/' << enc_ctx->time_base.den << std::endl;

    SwsContext* sws_ctx = sws_getContext(dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                                         enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt,
                                         SWS_BILINEAR, nullptr, nullptr, nullptr);

    int pts = 0;
    while (av_read_frame(ifmt_ctx, &dec_pkt) == 0) {
        if (dec_pkt.stream_index == video_index) {
            int send_ret = 0;
            do {
                send_ret = avcodec_send_packet(dec_ctx, &dec_pkt);
                if (send_ret == 0) {
                    AVFrame* dec_frame = av_frame_alloc();
                    if (avcodec_receive_frame(dec_ctx, dec_frame) == 0) {
//                        std::cout << "pts " << dec_pkt.pts << ' ' << ifmt_ctx->streams[video_index]->time_base.num << '/' << ifmt_ctx->streams[video_index]->time_base.den << ' '
//                                  << dec_frame->pts << ' ' << dec_ctx->time_base.num << '/' << dec_ctx->time_base.den << std::endl;
                        std::cout << dec_frame->pts << ' ' << av_frame_get_best_effort_timestamp(dec_frame) << std::endl;
//                        std::cout << "number " << dec_ctx->framerate.num << '/' << dec_ctx->framerate.den << std::endl;
//                        std::cout << "tbn " << ifmt_ctx->streams[video_index]->time_base.num << '/'
//                                  << ifmt_ctx->streams[video_index]->time_base.den << std::endl;

//                        AVFrame* enc_frame = av_frame_alloc();
//                        av_image_alloc(enc_frame->data, enc_frame->linesize, enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt, 32);
//                        error = sws_scale(sws_ctx,
//                                          dec_frame->data, dec_frame->linesize, 0, dec_ctx->height,
//                                          enc_frame->data, enc_frame->linesize);
//
//                        enc_frame->width = enc_ctx->width;
//                        enc_frame->height = enc_ctx->height;
//                        enc_frame->format = enc_ctx->pix_fmt;
//                        enc_frame->pts = pts++;

//                        int enc_send_ret = 0;
//                        do {
//                            enc_send_ret = avcodec_send_frame(enc_ctx, enc_frame);
//                            if (enc_send_ret == 0) {
//                                AVPacket enc_pkt = {.data = nullptr, .size = 0};
//                                av_init_packet(&enc_pkt);
//                                if (avcodec_receive_packet(enc_ctx, &enc_pkt) == 0) {
//                                    std::cout << "receive packet" << std::endl;
//                                    if (enc_pkt.pts != AV_NOPTS_VALUE) {
//                                        av_packet_rescale_ts(&enc_pkt, enc_ctx->time_base, enc_stream->time_base);
//                                    }
//                                    enc_pkt.stream_index = 0;
//                                    av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
//                                }
//                                av_packet_unref(&enc_pkt);
//                            } else {
//                                std::cerr << "Send frame error " << av_err2str(enc_send_ret) << ' ' << enc_ctx->refcounted_frames << std::endl;
//                            }
//                        } while (enc_send_ret == AVERROR(EAGAIN));
                    }
                    av_frame_free(&dec_frame);
                } else {
                    std::cerr << "send packet error" << std::endl;
                }
            } while (send_ret == AVERROR(EAGAIN));
        }
        av_packet_unref(&dec_pkt);
    }
    av_write_trailer(ofmt_ctx);

    avformat_free_context(ifmt_ctx);
    return EXIT_SUCCESS;
}
