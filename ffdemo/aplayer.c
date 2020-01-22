#include <pthread.h>
#include "SDL2/SDL.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/time.h"

SDL_AudioSpec sdl_audioSpec;
SwrContext* swrContext;
AVRational audioStreamTimebase;
int64_t start_time;
static Uint8* audio_chunk;
static Uint8* audio_pos;
static Uint32 audio_len = 0;
static SDL_mutex* sdl_mutex;

typedef void (*DecodeCallback)(AVFrame* frame);

void sdl_audio_callback(void* userData, Uint8* stream, int len)
{
    SDL_LockMutex(sdl_mutex);
    if (audio_pos - audio_chunk >= audio_len) {
        SDL_UnlockMutex(sdl_mutex);
        return;
    }

    av_log(NULL, AV_LOG_INFO, "sdl audio system playing %d with thread id %ld\n", len, pthread_self());
    len = (len > audio_len) ? audio_len : (Uint32) len;
    SDL_MixAudio(stream, audio_pos, (Uint32) len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    SDL_UnlockMutex(sdl_mutex);
}

void feed(AVFrame* frame)
{
    SDL_LockMutex(sdl_mutex);
    if (frame->linesize[0] > 0) {
        memcpy(audio_chunk + audio_len, frame->data[0], (size_t) frame->linesize[0]);
        audio_len += frame->linesize[0];
    }
    SDL_UnlockMutex(sdl_mutex);
    av_frame_unref(frame);
}

void on_audio_decode(AVFrame* frame)
{
    int ret = 0;

    AVFrame* frameS16 = av_frame_alloc();
    frameS16->nb_samples = sdl_audioSpec.samples;
    frameS16->channels = sdl_audioSpec.channels;
    frameS16->sample_rate = sdl_audioSpec.freq;
    av_samples_alloc(frameS16->data, frameS16->linesize, frameS16->channels,
                     frameS16->nb_samples, AV_SAMPLE_FMT_S16, 0);
    if ((ret = swr_convert(swrContext, frameS16->data, frameS16->nb_samples,
                (const uint8_t **) frame->data, frame->nb_samples)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "swr_convert error %s\n", av_err2str(ret));
    }

    int64_t ts = av_frame_get_best_effort_timestamp(frame);
    av_log(NULL, AV_LOG_INFO, "on audio %d %d", ts, audioStreamTimebase.den);
    ts = av_rescale_q(ts, audioStreamTimebase, AV_TIME_BASE_Q);
    av_log(NULL, AV_LOG_INFO, "on_audio_decode ts=%d %ld\n", ts, pthread_self());

    while (ts+start_time > av_gettime()) {
        ;
    }

    feed(frameS16);
    av_frame_unref(frame);
}

int decode(AVCodecContext* pCodecCtx, const AVPacket* pkt, DecodeCallback callback)
{
    int ret = 0;
    int frameRet = 0;
    do {
        ret = avcodec_send_packet(pCodecCtx, pkt);
        if (ret == 0) {

            do {
                AVFrame* frame = av_frame_alloc();
                frameRet = avcodec_receive_frame(pCodecCtx, frame);
                if (frameRet == 0 && callback != NULL) {
                    frame->sample_rate = pCodecCtx->sample_rate;
                    frame->channels = pCodecCtx->channels;
                    callback(frame);
                }
            } while (frameRet == 0);
        }
    } while (ret == AVERROR(EAGAIN));

    return ret;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s file_to_play\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    av_register_all();
    avcodec_register_all();

    int ret = 0;
    AVFormatContext* pFormatCtx;

    pFormatCtx = avformat_alloc_context();
    if ((ret = avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "could not open input %s\n", av_err2str(ret));
        goto end;
    }
    avformat_find_stream_info(pFormatCtx, NULL);
    av_dump_format(pFormatCtx, 0, argv[1], 0);

    AVCodec* pCodec;
    AVCodecContext* pCodecCtx;
    int audioIndex = -1;
    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &pCodec, 0);
    if (ret == AVERROR_STREAM_NOT_FOUND) {
        av_log(NULL, AV_LOG_ERROR, "no audio stream found, quit!\n");
    } else {
        int find_decoder = 0;
        if (ret == AVERROR_DECODER_NOT_FOUND) {
            find_decoder = 1;
        }

        for (unsigned i = 0; i < pFormatCtx->nb_streams; i++) {
            if (pFormatCtx->streams[i]->codecpar->codec_type
                    == AVMEDIA_TYPE_AUDIO) {
                audioIndex = i;

                if (find_decoder) {
                    pCodec = avcodec_find_decoder(pFormatCtx->streams[i]->codecpar->codec_id);
                }
            }
        }
    }
    if (pCodec == NULL) {
        av_log(NULL, AV_LOG_ERROR, "no decoder found, quit!\n");
        goto end;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[audioIndex]->codecpar);
    audioStreamTimebase = pFormatCtx->streams[audioIndex]->time_base;
    if ((ret = avcodec_open2(pCodecCtx, pCodec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "could not open decoder %s\n", av_err2str(ret));
        goto end;
    }
    av_log(NULL, AV_LOG_INFO, "frame size %d\n", pCodecCtx->frame_size);

    SDL_version* version = av_mallocz(sizeof(SDL_version));
    SDL_VERSION(version);
    av_log(NULL, AV_LOG_INFO, "SDL VERSION %d.%d.%d\n", version->major, version->minor, version->patch);
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec desired;
    desired.callback = &sdl_audio_callback;
    desired.channels = 2;
    desired.format = AUDIO_S16SYS;
    desired.freq = 44100;
    desired.samples = (Uint16) (pCodecCtx->frame_size * 2); // it's very import, otherwise your sound might be choppy
    desired.userdata = NULL;
    if (SDL_OpenAudio(&desired, &sdl_audioSpec) < 0) {
        av_log(NULL, AV_LOG_ERROR, "could not open audio %s\n", SDL_GetError());
        goto end;
    }

    swrContext = swr_alloc_set_opts(NULL,
                                    av_get_default_channel_layout(sdl_audioSpec.channels), AV_SAMPLE_FMT_S16, sdl_audioSpec.freq,
                                    (int64_t) pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate,
                                    0, NULL);
    if ((ret = swr_init(swrContext)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "context initialize fail, %s\n", av_err2str(ret));
        goto end;
    }

    AVPacket* pkt = av_packet_alloc();
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

    start_time = av_gettime();
    audio_chunk = av_mallocz(1024 * 1024 * 48);
    audio_pos = audio_chunk;
    sdl_mutex = SDL_CreateMutex();
    SDL_PauseAudio(0);

    while (av_read_frame(pFormatCtx, pkt) == 0) {
        if (pkt->stream_index == audioIndex) {
            decode(pCodecCtx, pkt, &on_audio_decode);
        }
    }
    av_packet_free(&pkt);

//    while (audio_len > 0) {
//        SDL_Delay(100);
//    }

    end:
    swr_free(&swrContext);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);
    return 0;
}