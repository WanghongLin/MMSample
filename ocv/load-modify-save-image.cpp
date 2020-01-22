//
// Created by Wanghong Lin on 2018/9/27.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{
    cv::String imageName("../../footage/WindowsLogo.jpg");

    cv::Mat image = cv::imread(imageName, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cout << "No image data" << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::imwrite(mktemp(strdup("/tmp/temp.XXXXXX")) + std::string(".png"), grayImage);

    cv::namedWindow(imageName, cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Gray image", cv::WINDOW_AUTOSIZE);

    cv::imshow(imageName, image);
    cv::imshow("Gray image", grayImage);

    cv::waitKey(0);

    return EXIT_SUCCESS;
}