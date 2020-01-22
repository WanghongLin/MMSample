//
// Created by Wanghong Lin on 2018/10/22.
//

#include <iostream>
#include <opencv2/opencv.hpp>

using std::cin;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    double alpha = 0.5;
    double beta;
    double input;

    cv::Mat src1, src2, dst;

    cout << " Simple Linear Blender " << endl;
    cout << "-----------------------" << endl;
    cout << "* Enter alpha [0.0-1.0]: ";
    cin >> input;

    if (input >= 0 && input <= 1) {
        alpha = input;
    }

    src1 = cv::imread("../../footage/LinuxLogo.jpg");
    src2 = cv::imread("../../footage/WindowsLogo.jpg");

    if (src1.empty()) { cout << "Error loading src1" << endl; return EXIT_FAILURE; }
    if (src2.empty()) { cout << "Error loading src2" << endl; return EXIT_FAILURE; }

    beta = 1.0 - alpha;
    cv::addWeighted(src1, alpha, src2, beta, 0.0, dst);

    cv::imshow("Linear blend", dst);
    cv::waitKey();

    return EXIT_SUCCESS;
}

