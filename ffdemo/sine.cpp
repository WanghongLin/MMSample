//
// This program demonstrate how to simulate ffmpeg -lavfi sine filter behavior
// by create a 5s sine wave m4a audio file.
//
// Because it is very simple, this program can also be used as a study resource
// learn how to encode audio using ffmpeg.
//
// Created by Wanghong Lin on 2019/9/17.
//

#include <iostream>
#include <cstring>
#include <cmath>

#ifdef __cplusplus
extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
};

#endif // __cplusplus

constexpr auto OUTPUT_FILENAME = "output.m4a";
constexpr auto OUTPUT_FORMAT_NAME = "m4a";

/**
 * Obtain one sine wave frame with swresample to convert s16p -> fltp
 *
 * @param enc_codec_ctx encoder context
 * @param duration_seconds total duration
 * @return a frame can be send to encoder
 */
AVFrame* obtain_one_frame_swr(AVCodecContext* enc_codec_ctx, int duration_seconds)
{
    static int64_t pts = 0;
    static int samples = 0;

    static SwrContext* swr_ctx = swr_alloc_set_opts(nullptr,
                                                    enc_codec_ctx->channel_layout,
                                                    enc_codec_ctx->sample_fmt,
                                                    enc_codec_ctx->sample_rate,
                                                    av_get_default_channel_layout(1),
                                                    AV_SAMPLE_FMT_S16P,
                                                    enc_codec_ctx->sample_rate,
                                                    0, nullptr);
    if (swr_ctx == nullptr) {
        av_log(nullptr, AV_LOG_ERROR, "swr_alloc\n");
    }
    if (!swr_is_initialized(swr_ctx)) {
        swr_init(swr_ctx);
    }

    auto total_nb_samples = duration_seconds * enc_codec_ctx->sample_rate;
    if (samples >= total_nb_samples-1) {
        if (swr_ctx && swr_is_initialized(swr_ctx)) {
            swr_free(&swr_ctx);
        }
        return nullptr;
    }

    int err = 0;
    auto s16_buf = new short[enc_codec_ctx->frame_size];
    for (int i = 0; i < enc_codec_ctx->frame_size; i++) {
        s16_buf[i] = static_cast<short>(std::numeric_limits<short>::max() * std::sinf(samples++ % 100 / 100.0f * 2 * M_PI));
    }
    auto s16p_frame = av_frame_alloc();
    err = av_samples_fill_arrays(s16p_frame->data, s16p_frame->linesize, reinterpret_cast<const uint8_t *>(s16_buf),
                                 1, enc_codec_ctx->frame_size, AV_SAMPLE_FMT_S16P, 0);
    // or use allocate then copy
    // av_samples_alloc(s16p_frame->data, s16p_frame->linesize, 1, enc_codec_ctx->frame_size, AV_SAMPLE_FMT_S16P, 0);
    // av_samples_copy(s16p_frame->data, (uint8_t *const *)(&s16_buf), 0, 0, enc_codec_ctx->frame_size, 1, AV_SAMPLE_FMT_S16P);
    if (err < 0) {
        av_log(nullptr, AV_LOG_ERROR, "av samples fill arrays %s\n", av_err2str(err));
        return nullptr;
    }
    s16p_frame->sample_rate = enc_codec_ctx->sample_rate;
    s16p_frame->format = AV_SAMPLE_FMT_S16P;
    s16p_frame->channels = 1;
    s16p_frame->channel_layout = static_cast<uint64_t>(av_get_default_channel_layout(1));
    s16p_frame->nb_samples = enc_codec_ctx->frame_size;

    auto fltp_frame = av_frame_alloc();
    err = av_samples_alloc(fltp_frame->data, fltp_frame->linesize, enc_codec_ctx->channels,
                           enc_codec_ctx->frame_size, AV_SAMPLE_FMT_FLTP, 0);
    if (err < 0) {
        av_log(nullptr, AV_LOG_ERROR, "av samples allocate %s\n", av_err2str(err));
        return nullptr;
    }

    fltp_frame->sample_rate = enc_codec_ctx->sample_rate;
    fltp_frame->format = enc_codec_ctx->sample_fmt;
    fltp_frame->channels = enc_codec_ctx->channels;
    fltp_frame->channel_layout = enc_codec_ctx->channel_layout;
    fltp_frame->nb_samples = enc_codec_ctx->frame_size;

    if ((err = swr_convert_frame(swr_ctx, fltp_frame, s16p_frame) < 0)) {
        av_log(nullptr, AV_LOG_ERROR, "swr_convert %s\n", av_err2str(err));
        return nullptr;
    }

    fltp_frame->pts = pts;
    pts += fltp_frame->nb_samples;

    av_freep(&s16p_frame->data[0]);
    av_frame_free(&s16p_frame);

    return fltp_frame;
}

/**
 * Obtain one sine wave frame with fltp format directly
 * @param enc_codec_ctx encoder context
 * @param duration_seconds total duration
 * @return a frame can be send to encoder
 */
AVFrame* obtain_one_frame(AVCodecContext* enc_codec_ctx, int duration_seconds)
{
    static int64_t pts = 0;
    static int samples = 0;

    auto total_nb_samples = duration_seconds* enc_codec_ctx->sample_rate;
    if (samples >= total_nb_samples-1) { // flushed frame
        return nullptr;
    }

    auto buffer = new float[enc_codec_ctx->frame_size];
    for (int i = 0; i < enc_codec_ctx->frame_size; i++) {
        buffer[i] = sinf(samples++ % 100 / 100.0f * 2 * M_PI);
    }

    auto frame = av_frame_alloc();
    frame->nb_samples = enc_codec_ctx->frame_size;
    frame->sample_rate = enc_codec_ctx->sample_rate;
    frame->channels = enc_codec_ctx->channels;
    frame->channel_layout = enc_codec_ctx->channel_layout;
    frame->pts = pts;
    pts += frame->nb_samples;

    avcodec_fill_audio_frame(frame, enc_codec_ctx->channels, enc_codec_ctx->sample_fmt,
                             reinterpret_cast<const uint8_t *>(buffer), enc_codec_ctx->frame_size * sizeof(float), 1);

    return frame;
}

int main(int argc, char* argv[])
{
    av_register_all();
    avcodec_register_all();

    int err {};
    AVOutputFormat* ofmt = av_guess_format(OUTPUT_FORMAT_NAME, OUTPUT_FILENAME, "audio/m4a");
    AVFormatContext* ofmt_ctx = nullptr;
    err = avformat_alloc_output_context2(&ofmt_ctx, ofmt, OUTPUT_FORMAT_NAME, OUTPUT_FILENAME);
    if (err < 0) {
        av_log(nullptr, AV_LOG_ERROR, av_err2str(err));
    }

    AVCodec* enc_codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    AVCodecContext* enc_codec_ctx = avcodec_alloc_context3(enc_codec);

    enc_codec_ctx->codec_id = enc_codec->id;
    enc_codec_ctx->codec_type = enc_codec->type;
    enc_codec_ctx->codec_tag = 0;
    enc_codec_ctx->bit_rate = 8 * 1000;
    enc_codec_ctx->sample_rate = 44100;
    enc_codec_ctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    enc_codec_ctx->time_base = AVRational {1, enc_codec_ctx->sample_rate};
    enc_codec_ctx->channels = 1;
    enc_codec_ctx->channel_layout = static_cast<uint64_t>(av_get_default_channel_layout(1));

    if (!(enc_codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)) {
        enc_codec_ctx->frame_size = 1024;
    }

    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        enc_codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    AVStream* stream = avformat_new_stream(ofmt_ctx, enc_codec);
    err = avcodec_open2(enc_codec_ctx, enc_codec, nullptr);
    if (err < 0) {
        av_log(nullptr, AV_LOG_ERROR, av_err2str(err));
    }
    avcodec_parameters_from_context(stream->codecpar, enc_codec_ctx);

    av_dump_format(ofmt_ctx, -1, OUTPUT_FILENAME, 1);

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((err = avio_open(&ofmt_ctx->pb, OUTPUT_FILENAME, AVIO_FLAG_WRITE)) < 0) {
            av_log(nullptr, AV_LOG_ERROR, "AV IO open %s\n", av_err2str(err));
            return err;
        }
    }

    err = avformat_write_header(ofmt_ctx, nullptr);
    if (err < 0) {
        av_log(nullptr, AV_LOG_ERROR, av_err2str(err));
    }

    while (true) {
        AVFrame* frame = obtain_one_frame(enc_codec_ctx, 5);

        if ((err = avcodec_send_frame(enc_codec_ctx, frame)) == 0) {
            AVPacket* pkt = av_packet_alloc();
            av_init_packet(pkt);
            pkt->size = 0;
            pkt->data = nullptr;

            if ((err = avcodec_receive_packet(enc_codec_ctx, pkt)) == 0) {
                av_packet_rescale_ts(pkt, enc_codec_ctx->time_base, stream->time_base);
                if ((err = av_interleaved_write_frame(ofmt_ctx, pkt) != 0)) {
                    av_log(nullptr, AV_LOG_ERROR, "write frame %s\n", av_err2str(err));
                }
            } else {
                av_log(nullptr, AV_LOG_ERROR, "receive packet %s\n", av_err2str(err));
            }

            av_packet_free(&pkt);
        } else {
            av_log(nullptr, AV_LOG_ERROR, "send frame %s\n", av_err2str(err));
        }

        if (frame) {
            av_freep(&frame->data[0]);
            av_frame_free(&frame);
        } else {
            break;
        }
    }

    if ((err = av_write_trailer(ofmt_ctx)) != 0) {
        av_log(nullptr, AV_LOG_ERROR, "write trailer %s\n", av_err2str(err));
    }

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&ofmt_ctx->pb);
    }
    if (enc_codec_ctx) {
        avcodec_close(enc_codec_ctx);
        avcodec_free_context(&enc_codec_ctx);
    }
    return EXIT_SUCCESS;
}
