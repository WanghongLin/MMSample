//
// Created by Wanghong Lin on 2018/9/30.
//

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    cv::Mat A, C;

    A = cv::imread("../../footage/WindowsLogo.jpg", cv::IMREAD_COLOR);

    cv::Mat B(A);
    C = A;

    cv::Mat M(2, 2, CV_8UC3, cv::Scalar(0, 0, 255));
    std::cout << "M = " << std::endl << " " << M << std::endl << std::endl;

    cv::Mat N;
    int sz[] = {2, 3};
    N.create(2, sz, CV_8UC(1));
    std::cout << N << std::endl;

    cv::Mat R;
    R.create(3, 2, CV_8UC(3));
    cv::randu(R, cv::Scalar::all(0), cv::Scalar::all(255));
    std::cout << "R (default) = " << '\n' << R << std::endl;
    std::cout << "R (python) = " << '\n' << cv::format(R, cv::Formatter::FMT_PYTHON) << std::endl;
    std::cout << "R (csv) = " << '\n' << cv::format(R, cv::Formatter::FMT_CSV) << std::endl;
    std::cout << "R (c) = " << '\n' << cv::format(R, cv::Formatter::FMT_C) << std::endl;
    std::cout << "R (numpy) = " << '\n' << cv::format(R, cv::Formatter::FMT_NUMPY) << std::endl;

    for (int i = cv::Formatter::FMT_DEFAULT; i <= cv::Formatter::FMT_C; ++i) {
        auto type = cv::Formatter::FormatType(i);
        std::cout << "R (" << i << ')' << " " << cv::format(R, type) << std::endl;
    }

    std::cout << "Point (2D) = " << cv::Point2f(5, 1) << std::endl;
    std::cout << "Point (3D) = " << cv::Point3f(2, 6, 7) << std::endl;

    std::vector<float> v;
    v.push_back(CV_PI);
    v.push_back(2);
    v.push_back(3.01f);
    std::cout << "Vector of floats via Mat = " << cv::Mat(v) << std::endl;
}