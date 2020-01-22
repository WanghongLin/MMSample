//
// Created by Wanghong Lin on 2018/10/11.
//

#include <opencv2/opencv.hpp>
#include <iostream>

void onTrackBarChanged(int pos, void* userdata)
{
    std::cout << "on change " << pos << std::endl;
}

int main(int argc, char* argv[])
{
    cv::Mat img = cv::imread("../../footage/WindowsLogo.jpg");
    cv::Mat grey;
    cv::cvtColor(img, grey, cv::COLOR_RGB2GRAY);

    cv::Mat sobelx;
    cv::Sobel(grey, sobelx, CV_32F, 1, 0);

    double minVal, maxVal;
    cv::minMaxLoc(sobelx, &minVal, &maxVal);
    cv::Mat draw;
    sobelx.convertTo(draw, CV_8U, 255.0/(maxVal-minVal), -minVal*255.0/(maxVal-minVal));

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    int value = 50;

    cv::createTrackbar("image", "image", &value, 100, onTrackBarChanged);

    cv::imshow("image", draw);

    cv::waitKey();

    return EXIT_SUCCESS;
}

