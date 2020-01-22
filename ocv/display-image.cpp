//
// Created by Wanghong Lin on 2018/9/27.
//

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    cv::String imageName("../../footage/WindowsLogo.jpg");
    if (argc > 1) {
        imageName = argv[1];
    }

    cv::Mat image = cv::imread(imageName, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", image);

    cv::waitKey(0);

    return EXIT_SUCCESS;
}
