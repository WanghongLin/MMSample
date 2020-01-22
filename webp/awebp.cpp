//
// Created by Wanghong Lin on 2018/10/11.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <webp/demux.h>

using namespace std;

typedef void (*WebPFrameCallback)(uint8_t* decodedData, int width, int height, int duration, int frame_num, int frame_count, int loop_count);

static WebPData* webPDataFromFile(const char* filePath)
{
    WebPData* webPData = static_cast<WebPData *>(std::malloc(sizeof(struct WebPData)));
    WebPDataInit(webPData);

    ifstream in(filePath, ios::binary | ios::in);
    if (in.is_open()) {
        in.seekg(0, ios::end);
        webPData->size = static_cast<size_t>(in.tellg());
        webPData->bytes = new uint8_t[webPData->size];
        in.seekg(0, ios::beg);
        in.read((char *) webPData->bytes, webPData->size);
        in.close();
    }

    return webPData;
}

static bool isAnimatedWebP(const char* filePath)
{
    auto* webPData = webPDataFromFile(filePath);
    auto* demuxer = WebPDemux(webPData);

    if (demuxer) {
        uint32_t flags = WebPDemuxGetI(demuxer, WEBP_FF_FORMAT_FLAGS);
        if (flags & ANIMATION_FLAG) {
            return true;
        }
    }

    return false;
}

static void readAnimatedWebPV1(const char* filePath)
{
    WebPData* webPData = webPDataFromFile(filePath);

    WebPAnimDecoder* decoder = WebPAnimDecoderNew(webPData, nullptr);
    if (decoder) {
        WebPAnimInfo info;
        if (WebPAnimDecoderGetInfo(decoder, &info)) {
            cout << "awebp info " << info.canvas_width << 'x' << info.canvas_height << " loop " << info.loop_count << endl;
        }
        uint8_t* buf;
        int timestamp;

        if (WebPAnimDecoderGetNext(decoder, &buf, &timestamp)) {
            do {
                cout << "decode one frame v1 " << timestamp << endl;
            } while (WebPAnimDecoderGetNext(decoder, &buf, &timestamp));
        }

        WebPAnimDecoderDelete(decoder);
    }

    WebPDataClear(webPData);
}

static int readAnimatedWebP(const char* filePath, WebPFrameCallback webPFrameCallback)
{
    WebPData* webPData = webPDataFromFile(filePath);

    WebPDemuxer* demuxer = WebPDemux(webPData);
    if (!demuxer) {
        cerr << "create demuxer error" << endl;
        return EXIT_FAILURE;
    }

    uint32_t width = WebPDemuxGetI(demuxer, WEBP_FF_CANVAS_WIDTH);
    uint32_t height = WebPDemuxGetI(demuxer, WEBP_FF_CANVAS_HEIGHT);
    uint32_t flags = WebPDemuxGetI(demuxer, WEBP_FF_FORMAT_FLAGS);
    uint32_t frameCount = WebPDemuxGetI(demuxer, WEBP_FF_FRAME_COUNT);
    uint32_t loopCount = WebPDemuxGetI(demuxer, WEBP_FF_LOOP_COUNT);

    cout << "width " << width << " height " << height << endl;

    WebPIterator iter;
    if (WebPDemuxGetFrame(demuxer, 1, &iter)) {
        do {
            int outWidth, outHeight;
            uint8_t* decodedData = WebPDecodeRGBA(iter.fragment.bytes, iter.fragment.size, &outWidth, &outHeight);
            if (decodedData) {
                cout << "decode one frame " << outWidth << 'x' << outHeight << " duration " << iter.duration << ' '
                     << iter.frame_num << '/' << frameCount << " loop count " << loopCount << endl;

                if (webPFrameCallback) {
                    webPFrameCallback(decodedData, outWidth, outHeight, iter.duration, iter.frame_num, frameCount, loopCount);
                }

                WebPFree(decodedData);
            }
        } while (WebPDemuxNextFrame(&iter));
        WebPDemuxReleaseIterator(&iter);
    }

    WebPChunkIterator chunkIterator;
    if (flags & ANIMATION_FLAG) {
        if (WebPDemuxGetChunk(demuxer, "ANIM", 1, &chunkIterator)) {
            cout << "chunk " << chunkIterator.chunk.bytes << endl;
        }
        WebPDemuxReleaseChunkIterator(&chunkIterator);
    }

    WebPDemuxDelete(demuxer);
    WebPDataClear(webPData);

    return EXIT_SUCCESS;
}

static void readAnimatedWebPCallback(uint8_t* decodedData, int width, int height, int duration, int frame_num, int frame_count, int loop_count)
{
    cv::Mat src(height, width, CV_8UC4, decodedData);
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_RGBA2BGRA);

    cv::imshow("image", dst);
    if (duration == 0) {
        duration = 40;
    }
    cv::waitKey(duration);
}

static void help(const char* progName)
{
    cout << "Usage: " << progName << " <input-animated-webp-file>"
         << endl << endl;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        help(argv[0]);
        return EXIT_FAILURE;
    }

    if (isAnimatedWebP(argv[1])) {
        readAnimatedWebP(argv[1], &readAnimatedWebPCallback);
        cv::waitKey();

        readAnimatedWebPV1(argv[1]);
    } else {
        cerr << "not a animated webp" << endl;
    }
    return EXIT_SUCCESS;
}
