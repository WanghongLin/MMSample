//
// Created by mutter on 5/14/16.
//

#include "libavformat/avformat.h"
#include "libswresample/swresample.h"

#define OUT_PATH_AUDIO "/home/mutter/dwhelper/o.m4a"

int open_audio_output_file(AVFormatContext* ofmt_ctx, AVCodecContext* oc_ctx, AVCodec* oc)
{
    int ret = 0;
    AVStream* stream = avformat_new_stream(ofmt_ctx, oc);
    if (stream != NULL) {
        AVCodecParameters* parameters = stream->codecpar;
        parameters->codec_id = oc->id;
        parameters->codec_tag = 0;
        parameters->codec_type = AVMEDIA_TYPE_AUDIO;
        parameters->sample_rate = 44100;
        parameters->format = AV_SAMPLE_FMT_FLTP;
        parameters->channels = 2;
        parameters->channel_layout = (uint64_t) av_get_default_channel_layout(2);
        parameters->bit_rate = 192000;
        parameters->profile = FF_PROFILE_AAC_LOW; // AAC LC
        parameters->frame_size = 1024;

        if ((ret = avcodec_parameters_to_context(oc_ctx, parameters)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "codec parameters to context\n");
            goto out;
        }
    }

    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        oc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if ((ret = avcodec_open2(oc_ctx, oc, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "open audio encoder %s\n", av_err2str(ret));
        goto out;
    }

    if (oc_ctx->extradata_size) {
        stream->codecpar->extradata = av_mallocz((size_t) (oc_ctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE));
        stream->codecpar->extradata_size = oc_ctx->extradata_size;
        memcpy(stream->codecpar->extradata, oc_ctx->extradata, (size_t) oc_ctx->extradata_size);
    }

    if ((ret = avio_open2(&ofmt_ctx->pb, OUT_PATH_AUDIO, AVIO_FLAG_WRITE, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avio open audio %s\n", av_err2str(ret));
        goto out;
    }

    if ((ret = avformat_write_header(ofmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "write audio header %s\n", av_err2str(ret));
        goto out;
    }

    av_dump_format(ofmt_ctx, 0, OUT_PATH_AUDIO, 1);

    out:
    return ret;
}

int retrieve_audio_track(const char *path)
{
    int ret = 0;
    AVFormatContext* ofmt_ctx;
    avformat_alloc_output_context2(&ofmt_ctx,
                                   av_guess_format("m4a", OUT_PATH_AUDIO, "audio/mp4a-latm"),
                                   "m4a", OUT_PATH_AUDIO);
    AVCodec* oc = avcodec_find_encoder(AV_CODEC_ID_AAC);
    AVCodecContext* oc_ctx = avcodec_alloc_context3(oc);
    if ((ret = open_audio_output_file(ofmt_ctx, oc_ctx, oc)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "open audio output file %s\n", av_err2str(ret));
        goto end;
    }

    AVFormatContext* ifmt_ctx;
    AVCodecContext* ic_ctx;
    AVCodec* ic;
    int audio_index = -1;

    ifmt_ctx = avformat_alloc_context();
    if ((ret = avformat_open_input(&ifmt_ctx, path, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "open intput %s\n", av_err2str(ret));
        goto end;
    }
    avformat_find_stream_info(ifmt_ctx, NULL);

    ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &ic, 0);
    if (ret == AVERROR_STREAM_NOT_FOUND) {
        av_log(NULL, AV_LOG_ERROR, "no stream found %s\n", av_err2str(ret));
        goto end;
    }

    for (unsigned i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index = i;
            break;
        }
    }

    if (audio_index == -1) {
        av_log(NULL, AV_LOG_ERROR, "no audio stream found\n");
        goto end;
    }

    if (ret == AVERROR_DECODER_NOT_FOUND) {
        av_log(NULL, AV_LOG_ERROR, "decoder not found\n");
        ic = avcodec_find_decoder(ifmt_ctx->streams[audio_index]->codecpar->codec_id);
    }

    ic_ctx = avcodec_alloc_context3(ic);
    avcodec_parameters_to_context(ic_ctx, ifmt_ctx->streams[audio_index]->codecpar);

    if ((ret = avcodec_open2(ic_ctx, ic, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avcodec open error %s\n", av_err2str(ret));
    }

    AVPacket* pkt = av_packet_alloc();
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

    SwrContext* swrContext = swr_alloc_set_opts(NULL,
                                                oc_ctx->channel_layout, oc_ctx->sample_fmt, oc_ctx->sample_rate,
                                                ic_ctx->channel_layout, ic_ctx->sample_fmt, ic_ctx->sample_rate,
                                                0, NULL);
    swr_init(swrContext);

    uint64_t pts = 0;
    while (av_read_frame(ifmt_ctx, pkt) == 0) {

        if (pkt->stream_index == audio_index) {

            if ((ret = avcodec_send_packet(ic_ctx, pkt)) == 0) {
                AVFrame* frame = av_frame_alloc();
                while (avcodec_receive_frame(ic_ctx, frame) == 0) {
                    AVFrame* enc_frame = av_frame_alloc();
                    av_samples_alloc(enc_frame->data, enc_frame->linesize,
                                     oc_ctx->channels, oc_ctx->frame_size, oc_ctx->sample_fmt, 0);
                    swr_convert(swrContext,
                                enc_frame->data, oc_ctx->frame_size,
                                (const uint8_t **) frame->data, frame->nb_samples);
                    enc_frame->nb_samples = oc_ctx->frame_size;
                    enc_frame->sample_rate = oc_ctx->sample_rate;
                    enc_frame->format = oc_ctx->sample_fmt;
                    enc_frame->channels = oc_ctx->channels;
                    enc_frame->channel_layout = oc_ctx->channel_layout;
                    enc_frame->pts = pts;
                    pts += enc_frame->nb_samples;

                    if ((ret = avcodec_send_frame(oc_ctx, enc_frame)) == 0) {

                        AVPacket* enc_pkt = av_packet_alloc();
                        av_init_packet(enc_pkt);
                        enc_pkt->data = NULL;
                        enc_pkt->size = 0;
                        if ((ret = avcodec_receive_packet(oc_ctx, enc_pkt)) == 0) {
                            if (enc_pkt->pts != AV_NOPTS_VALUE) {
                                enc_pkt->pts = av_rescale_q(enc_pkt->pts, oc_ctx->time_base, ofmt_ctx->streams[0]->time_base);
                            }
                            if (enc_pkt->dts != AV_NOPTS_VALUE) {
                                enc_pkt->dts = av_rescale_q(enc_pkt->dts, oc_ctx->time_base, ofmt_ctx->streams[0]->time_base);
                            }

                            if ((ret = av_interleaved_write_frame(ofmt_ctx, enc_pkt)) < 0) {
                                av_log(NULL, AV_LOG_ERROR, "write frame error %s\n", av_err2str(ret));
                            }
                        } else {
                            av_log(NULL, AV_LOG_ERROR, "receive packet error %s\n", av_err2str(ret));
                        }
                    } else {
                        av_log(NULL, AV_LOG_ERROR, "send frame error %s\n", av_err2str(ret));
                    }
                }

                av_frame_unref(frame);
            }
        }
    }

    av_write_trailer(ofmt_ctx);

    end:

    avformat_close_input(&ifmt_ctx);
    avcodec_close(oc_ctx);
    avformat_free_context(ofmt_ctx);
    avformat_free_context(ifmt_ctx);

    return ret;
}


int main(int argc, char *argv[])
{
    return 0;
}
