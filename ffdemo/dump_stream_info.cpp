//
// Created by Wanghong Lin on 2018/6/21.
//

#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

#ifdef __cplusplus
};
#endif

using namespace std;

static void av_check_error(int err, const char* description)
{
    if (err != 0) {
        char errbuf[128] = { 0 };
        av_make_error_string(errbuf, sizeof(errbuf), err);
        std::cout << description << ": " << errbuf << std::endl;
        std::exit(0);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <media-file>" << std::endl;
        std::exit(0);
    }

    av_register_all();
    avcodec_register_all();

    const char* url = argv[1];
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    AVInputFormat* pInputFormat = av_find_input_format("mp4");
    int ret = 0;

    ret = avformat_open_input(&pFormatCtx, url, pInputFormat, nullptr);
    av_check_error(ret, "open input");
    avformat_find_stream_info(pFormatCtx, nullptr);
    av_dump_format(pFormatCtx, -1, url, 0);

    return 0;
}

