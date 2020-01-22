//
// Created by Wanghong Lin on 2018/11/7.
//


#include <iostream>
#include <opencv2/opencv.hpp>

#define window_width 640
#define window_height 480
#define NUMBER 128

static const char* window_name = "random drawing";

static cv::Scalar randomColor(cv::RNG& rng)
{
    int icolor = (unsigned) rng;
    return cv::Scalar(icolor&255, (icolor>>8) & 255, (icolor>>16)&255);
}

static int Drawing_Random_Lines(cv::Mat image, const char* name, cv::RNG rng)
{
    int lineType = cv::LINE_8;
    cv::Point pt1, pt2;

    for (int i = 0; i < NUMBER; ++i) {
        pt1.x = rng.uniform(0, window_width);
        pt1.y = rng.uniform(0, window_height);
        pt2.x = rng.uniform(0, window_width);
        pt2.y = rng.uniform(0, window_height);

        cv::line(image, pt1, pt2, randomColor(rng), rng.uniform(1, 10), lineType);
    }

    cv::imshow(name, image);
    cv::waitKey();
    return 0;
}

static int Displaying_Random_Text(cv::Mat image, const char* name, cv::RNG rng)
{
    int lineType = cv::LINE_8;
    for (int i = 0; i < NUMBER; ++i) {
        cv::Point org;
        org.x = rng.uniform(0, window_width);
        org.y = rng.uniform(0, window_height);

        cv::putText(image, "Testing text render", org, rng.uniform(0, 8),
                    rng.uniform(0, 100)*0.05+0.1, randomColor(rng),
                    rng.uniform(1, 10), lineType);
    }
    cv::imshow(name, image);
    cv::waitKey();
    return 0;
}

int main(int argc, char* argv[])
{
    cv::RNG rng(0xffffffff);
    cv::Mat image = cv::Mat::zeros(window_height, window_width, CV_8UC3);

    Drawing_Random_Lines(image, window_name, rng);
    Displaying_Random_Text(image, window_name, rng);

    return EXIT_SUCCESS;
}