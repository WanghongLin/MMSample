//
// Created by Wanghong Lin on 2018/11/7.
//

#include <opencv2/opencv.hpp>
#include <iostream>

static cv::Mat src, dst;
static int morph_elem = 0;
static int morph_size = 0;
static int morph_operator = 0;
static const int max_operator = 4;
static const int max_elem = 2;
static const int max_kernel_size = 21;

const char* window_name = "Morphology Transformations Demo";

static void Morphology_Operations(int, void*);

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, "{@input | ../data/baboon.jpg | input image}");
    src = cv::imread(parser.get<cv::String>("@input"));
    if (src.empty()) {
        std::cout << "Could not open or find the image!" << std::endl;
        std::cout << "Usage: " << argv[0] << " <input-image>" << std::endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Operator:\n 0: Opening - 1: Closing \n 2: Gradient - 3: Top Hat \n 4: Black Hat",
                       window_name, &morph_operator, max_operator, Morphology_Operations);
    cv::createTrackbar("Element:\n 0: Rect - 1: Cross - 2: Ellipse", window_name,
                       &morph_elem, max_elem, Morphology_Operations);
    cv::createTrackbar("Kernel size:\n 2n+1", window_name,
                       &morph_size, max_kernel_size, Morphology_Operations);

    Morphology_Operations(0, 0);
    cv::waitKey();
    return EXIT_SUCCESS;
}

void Morphology_Operations(int value, void* data)
{
    int operation = morph_operator + cv::MORPH_OPEN;
    cv::Mat element = cv::getStructuringElement(morph_elem,
            cv::Size(2*morph_size+1, 2*morph_size+1));

    cv::morphologyEx(src, dst, operation, element);
    cv::imshow(window_name, dst);
}