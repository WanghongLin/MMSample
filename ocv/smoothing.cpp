//
// Created by Wanghong Lin on 2018/11/7.
//

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;

const int DELAY_CAPTION = 1500;
const int DELAY_BLUR = 100;
const int MAX_KERNEL_LENGTH = 31;
const char window_name[] = "Smoothing Demo";

static cv::Mat src;
static cv::Mat dst;

int display_caption(const char* caption);
int display_dst(int delay);

int main(int argc, char* argv[])
{
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    const char* filename = argc >= 2 ? argv[1] : "../data/lena.jpg";
    src = cv::imread(filename, cv::IMREAD_COLOR);
    if (src.empty()) {
        cerr << " Error opening image" << endl;
        cerr << " Usage: " << argv[0] << " [image_name -- default ../data/lena.jpg]" << endl;
        return EXIT_FAILURE;
    }

    if (display_caption("Original Image") != 0) {
        return 0;
    }

    dst = src.clone();
    if (display_dst(DELAY_CAPTION) != 0) {
        return 0;
    }

    if (display_caption("Homogeneous Blur") != 0) {
        return 0;
    }
    for (int i = 1; i < MAX_KERNEL_LENGTH; i+=2) {
        cv::blur(src, dst, cv::Size(i, i), cv::Point(-1, -1));
        if (display_dst(DELAY_BLUR) != 0) {
            return 0;
        }
    }

    if (display_caption("Gaussian Blur") != 0) {
        return 0;
    }
    for (int j = 1; j < MAX_KERNEL_LENGTH; j+=2) {
        cv::GaussianBlur(src, dst, cv::Size(j, j), 0, 0);
        if (display_dst(DELAY_BLUR) != 0) {
            return 0;
        }
    }

    if (display_caption("Median Blur") != 0) {
        return 0;
    }
    for (int k = 1; k < MAX_KERNEL_LENGTH; k+=2) {
        cv::medianBlur(src, dst, k);
        if (display_dst(DELAY_BLUR) != 0) {
            return 0;
        }
    }

    if (display_caption("Bilateral Blur") != 0) {
        return 0;
    }
    for (int l = 1; l < MAX_KERNEL_LENGTH; l+=2) {
        cv::bilateralFilter(src, dst, l, l*2.0, l/2.0);
        if (display_dst(DELAY_BLUR) != 0) {
            return 0;
        }
    }

    display_caption("Done!");

    return EXIT_SUCCESS;
}

int display_caption(const char* caption)
{
    dst = cv::Mat::zeros(src.size(), src.type());
    cv::putText(dst, caption,
    cv::Point(src.cols/4, src.rows/2),
    cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255));

    return display_dst(DELAY_CAPTION);
}

int display_dst(int delay)
{
    cv::imshow(window_name, dst);
    int c = cv::waitKey(delay);
    if (c >= 0) {
        return -1;
    }

    return 0;
}
