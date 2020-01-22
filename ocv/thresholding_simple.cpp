//
// Created by Wanghong Lin on 2018/11/1.
//


#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    cv::Mat img = cv::imread("../../footage/WindowsLogo.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat output(img.size(), img.type());

    cv::threshold(img, output, 127, 255, cv::THRESH_TRUNC);

    cv::imshow("Simple threshold", output);

    cv::waitKey();

    return EXIT_SUCCESS;
}