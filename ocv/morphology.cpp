//
// Created by Wanghong Lin on 2018/11/7.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

static cv::Mat src, erosion_dst, dilation_dst;

static int erosion_elem = 0;
static int erosion_size = 0;
static int dilation_elem = 0;
static int dilation_size = 0;
static const int max_elem = 2;
static const int max_kernel_size = 21;

void Erosion(int, void*);
void Dilation(int, void*);

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, "{@input | ../data/LinuxLogo.jpg | input image}");
    src = cv::imread(parser.get<cv::String>("@input"), cv::IMREAD_COLOR);

    if (src.empty()) {
        cout << "Could not open or find the image!" << endl;
        cout << "Usage: " << argv[0] << " <input-image> "<< endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow("Erosion Demo", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Dilation Demo", cv::WINDOW_AUTOSIZE);
    cv::moveWindow("Dilation Demo", src.cols, 0);

    cv::createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo",
                       &erosion_elem, max_elem, Erosion);
    cv::createTrackbar("Kernel size:\n 2n+1", "Erosion Demo",
                       &erosion_size, max_kernel_size, Erosion);
    cv::createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Dilation Demo",
                       &dilation_elem, max_elem, Dilation);
    cv::createTrackbar("Kernel size:\n 2n+1", "Dilation Demo",
                       &dilation_size, max_kernel_size, Dilation);

    Erosion(0, 0);
    Dilation(0, 0);
    cv::waitKey();
    return EXIT_SUCCESS;
}

void Erosion(int value, void * data) {

    int erosion_type = 0;
    if (erosion_elem == 0) {
        erosion_type = cv::MORPH_RECT;
    } else if (erosion_elem == 1) {
        erosion_type = cv::MORPH_CROSS;
    } else if (erosion_elem == 2) {
        erosion_type = cv::MORPH_ELLIPSE;
    }

    cv::Mat element = cv::getStructuringElement(erosion_type,
                                                cv::Size(2*erosion_size+1, 2*erosion_size+1),
                                                cv::Point(erosion_size, erosion_size));
    cv::erode(src, erosion_dst, element);
    cv::imshow("Erosion Demo", erosion_dst);
}

void Dilation(int value, void * data) {
    int dilation_type = 0;
    if (dilation_elem == 0) {
        dilation_type = cv::MORPH_RECT;
    } else if (dilation_elem == 1) {
        dilation_type = cv::MORPH_CROSS;
    } else if (dilation_elem == 2) {
        dilation_type = cv::MORPH_ELLIPSE;
    }

    cv::Mat element = cv::getStructuringElement(dilation_type,
                                                cv::Size(2*dilation_size+1, 2*dilation_size+1),
                                                cv::Point(dilation_size, dilation_size));
    cv::dilate(src, dilation_dst, element);
    cv::imshow("Dilation Demo", dilation_dst);
}

