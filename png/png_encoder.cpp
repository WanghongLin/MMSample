//
// Copyright 2019 Wanghong Lin
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//         http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
//         limitations under the License.
//
// Created by Wanghong Lin on 2019/12/30.
//

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <png.h>

#ifdef __cplusplus
};
#endif

int main(int argc, char* argv[])
{
    std::cout << png_libpng_ver << std::endl;

    char* tmp = mktemp(strdup("/tmp/temp.XXXXXX"));
    auto out_path = static_cast<char *>(std::calloc(strlen(tmp) + 5, sizeof(char)));
    strcpy(out_path, tmp);
    strcat(out_path, ".png");

    std::cout << "output to " << out_path << std::endl;

    FILE* fp = fopen(out_path, "wb");
    if (!fp) {
        return ENOMEM;
    }

    auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                           [](png_structp structp, png_const_charp charp) {

                                           },
                                           [](png_structp structp, png_const_charp charp) {

                                           });
    if (!png_ptr) {
        throw std::runtime_error(strerror(ENOMEM));
    }

    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        throw std::runtime_error(strerror(ENOMEM));
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        return EXIT_FAILURE;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr,
                 info_ptr,
                 500, 400, 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_set_write_status_fn(png_ptr, [](png_structp structp, png_uint_32 row, int pass) {

    });

    std::ifstream is("../../footage/bitmap.raw");
    auto image = new png_bytep[400] {};
    for (int i = 0; i < 400; i++) {
        image[i] = new png_byte[500*4] {};
        is.read((char *)(image[i]), 500*4);
    }
    is.close();

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, image);
    png_write_end(png_ptr, info_ptr);

    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return EXIT_SUCCESS;
}
