//
// Created by mutter on 5/8/16.
//

#ifdef __cplusplus
extern "C"
{
#endif

#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"

#ifdef __cplusplus
};
#endif

#define IN_PATH "/home/mutter/dwhelper/big_buck_bunny_240p_1mb.mp4"
#define OUT_PATH "/home/mutter/dwhelper/o.mp4"

int open_video_output_file(AVFormatContext *ofmt_ctx, AVCodecContext *oc_ctx, AVCodec *oc)
{
    int ret;
    AVStream* stream = avformat_new_stream(ofmt_ctx, oc);
    if (stream != NULL) {
        AVCodecParameters* parameters = stream->codecpar;
        parameters->codec_id = oc->id;
        parameters->codec_tag = 0;
        parameters->codec_type = AVMEDIA_TYPE_VIDEO;
        parameters->width = 160;
        parameters->height = 120;
        parameters->format = AV_PIX_FMT_YUV420P;
        parameters->sample_aspect_ratio = (AVRational) {1, 1};
        parameters->bit_rate = 640000;
        parameters->profile = FF_PROFILE_H264_BASELINE;
        parameters->level = 30;

        avcodec_parameters_to_context(oc_ctx, parameters);
        oc_ctx->time_base = (AVRational){1, 24};
        oc_ctx->gop_size = 40;
        oc_ctx->max_b_frames = 3;
        av_opt_set(oc_ctx->priv_data, "preset", "slow", 0);
    }

    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        oc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if ((ret = avcodec_open2(oc_ctx, oc, NULL)) < 0) {
        fprintf(stderr, "open encoder %s\n", av_err2str(ret));
        return ret;
    }

    if (oc_ctx->extradata_size) {
        // stream->codecpar->extradata = av_mallocz((size_t) (oc_ctx->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE));
        stream->codecpar->extradata = av_mallocz((size_t) (oc_ctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE));
        stream->codecpar->extradata_size = oc_ctx->extradata_size;
        memcpy(stream->codecpar->extradata, oc_ctx->extradata, (size_t) oc_ctx->extradata_size);
    }

    if ((ret = avio_open2(&ofmt_ctx->pb, OUT_PATH, AVIO_FLAG_READ_WRITE, NULL, NULL)) < 0) {
        fprintf(stderr, "open output %s\n", av_err2str(ret));
        return ret;
    }


    if ((ret = avformat_write_header(ofmt_ctx, NULL)) < 0) {
        fprintf(stderr, "write header %s\n", av_err2str(ret));
        return ret;
    }

    av_dump_format(ofmt_ctx, 0, OUT_PATH, 1);

    return ret;
}

int main(int argc, char* argv[])
{
    av_register_all();
    avcodec_register_all();

    int ret;
    AVFormatContext* ifmt_ctx;
    AVFormatContext* ofmt_ctx;
    AVCodecContext* ic_ctx;
    AVCodecContext* oc_ctx;
    AVCodec* ic;
    AVCodec* oc;
    int video_idx = 0;

    ifmt_ctx = avformat_alloc_context();
    if ((ret = avformat_open_input(&ifmt_ctx, IN_PATH, av_find_input_format("mp4"), NULL)) < 0) {
        fprintf(stderr, "open input %s\n", av_err2str(ret));
        goto end;
    }
    avformat_find_stream_info(ifmt_ctx, NULL);

    ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &ic, 0);
    if (ret == AVERROR_STREAM_NOT_FOUND) {
        fprintf(stderr, "no video stream %s\n", av_err2str(ret));
        goto end;
    }

    if (ic == NULL) {
        fprintf(stderr, "no decoder found %s\n", av_err2str(ret));
        goto end;
    }

    av_dump_format(ifmt_ctx, 0, IN_PATH, 0);

    for (unsigned i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_idx = i;
            break;
        }
    }

    ic_ctx = ifmt_ctx->streams[video_idx]->codec;

    if ((ret = avcodec_open2(ic_ctx, ic, NULL) < 0)) {
        fprintf(stderr, "open decoder %s\n", av_err2str(ret));
        goto end;
    }

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, OUT_PATH);
    oc = avcodec_find_encoder(ic->id);
    oc_ctx = avcodec_alloc_context3(oc);

    if ((ret = open_video_output_file(ofmt_ctx, oc_ctx, oc)) < 0) {
        fprintf(stderr, "open video output file %s\n", av_err2str(ret));
        goto end;
    }

    AVPacket* dec_pkt = av_packet_alloc();
    av_init_packet(dec_pkt);
    dec_pkt->data = NULL;
    dec_pkt->size = 0;

    struct SwsContext* swsContext = sws_getContext(ic_ctx->width, ic_ctx->height, ic_ctx->pix_fmt,
                                                   oc_ctx->width, oc_ctx->height, oc_ctx->pix_fmt,
                                                   SWS_BICUBIC, NULL, NULL, NULL);
    int i = 0;
    while (av_read_frame(ifmt_ctx, dec_pkt) == 0) {

        if (dec_pkt->stream_index == video_idx) {

            if ((ret = avcodec_send_packet(ic_ctx, dec_pkt)) == 0) {
                AVFrame* dec_frame = av_frame_alloc();
                av_image_alloc(dec_frame->data, dec_frame->linesize, ic_ctx->width, ic_ctx->height,
                               ic_ctx->pix_fmt, 32);

                if ((ret = avcodec_receive_frame(ic_ctx, dec_frame) == 0)) {

                    AVFrame* enc_frame = av_frame_alloc();
                    av_image_alloc(enc_frame->data, enc_frame->linesize, oc_ctx->width, oc_ctx->height,
                                   oc_ctx->pix_fmt, 32);

                    sws_scale(swsContext, (const uint8_t *const *) dec_frame->data, dec_frame->linesize, 0,
                              ic_ctx->height, enc_frame->data, enc_frame->linesize);

                    enc_frame->width = 160;
                    enc_frame->height = 120;
                    enc_frame->format = AV_PIX_FMT_YUV420P;
                    enc_frame->sample_aspect_ratio = (AVRational) {1, 1};
                    enc_frame->pts = i++;

                    AVPacket* enc_pkt = av_packet_alloc();
                    av_init_packet(enc_pkt);
                    enc_pkt->data = NULL;
                    enc_pkt->size = 0;

                    int got_packet_ptr;
                    ret = avcodec_encode_video2(oc_ctx, enc_pkt, enc_frame, &got_packet_ptr);
                    if (ret == 0) {
                        if (got_packet_ptr) {
                            av_packet_rescale_ts(enc_pkt, oc_ctx->time_base,
                                                 ofmt_ctx->streams[0]->time_base);
                            ret = av_interleaved_write_frame(ofmt_ctx, enc_pkt);
                            if (ret < 0) {
                                fprintf(stderr, "write frame %s\n", av_err2str(ret));
                            }
                        }
                    } else {
                        fprintf(stderr, "encode video %s\n", av_err2str(ret));
                    }

                    av_packet_unref(enc_pkt);
                    av_frame_unref(enc_frame);
                } else {
                    fprintf(stderr, "receive frame %s\n", av_err2str(ret));
                }

                av_frame_unref(dec_frame);
            } else {
                fprintf(stderr, "send packet %s\n", av_err2str(ret));
            }
        }
    }

    // av_interleaved_write_frame(ofmt_ctx, NULL);
    av_write_trailer(ofmt_ctx);
    avio_closep(&ofmt_ctx->pb);

    end:
    avformat_close_input(&ifmt_ctx);
    avformat_free_context(ifmt_ctx);
    avformat_free_context(ofmt_ctx);

    avcodec_close(oc_ctx);
    avcodec_close(ic_ctx);
    return 0;
}
