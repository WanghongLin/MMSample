//
// Created by Wanghong Lin on 2018/10/10.
//


#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;

static void help(char* progName)
{
    cout << endl
         << "This program shows how to filter images with mask: the write it yourself and the"
         << "filter2d way. " << endl
         << "Usage:" << endl
         << progName << " [image_path -- default ../data/lena.jpg] [G -- grayscale] " << endl << endl;
}

void Sharpen(const cv::Mat& myImage, cv::Mat& Result);

int main(int argc, char* argv[])
{
    help(argv[0]);
    const char* filename = argc >= 2 ? argv[1] : "../data/lena.jpg";

    cv::Mat src, dst0, dst1;
    if (argc >= 3 && !strcmp("G", argv[2])) {
        src = cv::imread(filename, cv::IMREAD_GRAYSCALE);
    } else {
        src = cv::imread(filename, cv::IMREAD_COLOR);
    }

    if (src.empty()) {
        cerr << "Can't open image [" << filename << "]" << endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow("Input", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Output", cv::WINDOW_AUTOSIZE);

    cv::imshow("Input", src);
    double t = cv::getTickCount();
    // Sharpen(src, dst0);
    // t = ((double)cv::getTickCount()-t)/cv::getTickFrequency();
    // cout << "Hand written function time passed in seconds: " << t << endl;

    // cv::imshow("Output", dst0);
    // cv::waitKey();

    char data[9] = {0, -1, 0,
                    -1, 5, -1,
                    0, -1, 0};
    cv::Mat kernel(3, 3, CV_8SC1, data);
    cout << "the kernel " << kernel << endl << endl;

    t = cv::getTickCount();
    cv::filter2D(src, dst1, src.depth(), kernel);
    t = ((double)cv::getTickCount()-t)/cv::getTickFrequency();
    cout << "Built-in filter2D time passed in seconds " << t << endl;
    cv::imshow("Output", dst1);
    cv::waitKey();

    return EXIT_SUCCESS;
}

void Sharpen(const cv::Mat &myImage, cv::Mat &Result)
{
    CV_Assert(myImage.depth() == CV_8U);

    const int nChannels = myImage.channels();
    Result.create(myImage.size(), myImage.type());

    for (int j = 0; j < myImage.rows - 1; ++j) {
        const uchar* previous = myImage.ptr<uchar>(j-1);
        const uchar* current = myImage.ptr<uchar>(j);
        const uchar* next = myImage.ptr<uchar>(j+1);

        uchar* output = Result.ptr<uchar>(j);

        for (int i = nChannels; i < nChannels * (myImage.cols - 1); ++i) {
            *output++ = cv::saturate_cast<uchar>(5*current[i]-current[i-nChannels]-current[i+nChannels]-previous[i]-next[i]);
        }
    }

    Result.row(0).setTo(cv::Scalar(0));
    Result.row(Result.rows-1).setTo(cv::Scalar(0));
    Result.col(0).setTo(cv::Scalar(0));
    Result.col(Result.cols-1).setTo(cv::Scalar(0));
}
