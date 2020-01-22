//
// Created by Wanghong Lin on 2018/8/3.
//

#include "rgba2heif.h"
#include <libheif/heif.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

static const int W = 1080;
static const int H = 2160;
static const std::string RGBA_SUFFIX(".rgba");
static const std::string YUV_SUFFIX(".yuv");
static const char* OUTPUT = "output.heic";

static heif_image* heif_image_from_yuv_file(const char* path) {
    std::cout << "Encode YUV data" << std::endl;

    heif_image* image;
    heif_image_create(W, H, heif_colorspace_YCbCr, heif_chroma_420, &image);
    heif_image_add_plane(image, heif_channel_Y, W, H, 8);
    heif_image_add_plane(image, heif_channel_Cb, W/2, H/2, 8);
    heif_image_add_plane(image, heif_channel_Cr, W/2, H/2, 8);

    int stride_y = 0;
    int stride_u = 0;
    int stride_v = 0;
    uint8_t* py = heif_image_get_plane(image, heif_channel_Y, &stride_y);
    uint8_t* pu = heif_image_get_plane(image, heif_channel_Cb, &stride_u);
    uint8_t* pv = heif_image_get_plane(image, heif_channel_Cr, &stride_v);

    size_t size_y = W * H;
    size_t size_uv = size_y / 2;

    std::cout << stride_y  << ':' << stride_u << ':' << stride_v << std::endl;

    ifstream input(path, std::ios::binary);
    input.read(reinterpret_cast<char *>(py), size_y);
    std::cout << "current position is " << input.tellg() << std::endl;
    input.read(reinterpret_cast<char *>(pu), size_uv / 2);
    std::cout << "current position is " << input.tellg() << std::endl;
    input.read(reinterpret_cast<char *>(pv), size_uv / 2);
    std::cout << "current position is " << input.tellg() << std::endl;
    input.close();

    return image;
}

static heif_image* heif_image_from_rgba_file(const char* path) {
    std::cout << "Encode RGBA data" << std::endl;

    heif_image* image;
    heif_image_create(W, H, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, &image);

    heif_image_add_plane(image, heif_channel_interleaved, W, H, 32);

    int stride = 0;
    uint8_t* p = heif_image_get_plane(image, heif_channel_interleaved, &stride);
    std::cout << "stride of image " << stride << std::endl;

    ifstream input(path, std::ios::binary | std::ios::ate);
    int64_t fsize = input.tellg();
    auto* buffer = new char[fsize];
    input.seekg(0, std::ios::beg);
    input.read(buffer, fsize);

    std::memcpy(p, buffer, fsize);
    return image;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>" << std::endl;
        std::exit(0);
    }

    heif_context* ctx = heif_context_alloc();

    heif_encoder* encoder;
    heif_context_get_encoder_for_format(ctx, heif_compression_HEVC, &encoder);

    heif_image* image;
    string path(argv[1]);
    uint8_t alpha_channel = 0;
    if (std::equal(RGBA_SUFFIX.rbegin(), RGBA_SUFFIX.rend(), path.rbegin())) {
        image = heif_image_from_rgba_file(argv[1]);
        alpha_channel = 1;
    } else if (std::equal(YUV_SUFFIX.rbegin(), YUV_SUFFIX.rend(), path.rbegin())) {
        image = heif_image_from_yuv_file(argv[1]);
        alpha_channel = 0;
    } else {
        std::cout << "Only support " << RGBA_SUFFIX << " or " << YUV_SUFFIX << " file." << std::endl;
        std::exit(0);
    }

    heif_encoding_options* encoding_options = heif_encoding_options_alloc();
    encoding_options->save_alpha_channel = alpha_channel;

    heif_context_encode_image(ctx, image, encoder, encoding_options, nullptr);
    heif_image_release(image);
    heif_encoder_release(encoder);

    heif_error error = {};

    if ((error = heif_context_write_to_file(ctx, OUTPUT)).code != heif_error_Ok) {
        std::cerr << "Write to file error " << error.message << std::endl;
    } else {
        std::cerr << "Write to file " << OUTPUT << std::endl;
    }
    heif_context_free(ctx);
    return 0;
}
