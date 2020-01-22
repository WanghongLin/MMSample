//
// Created by Wanghong Lin on 2018/9/30.
//

#include <opencv2/opencv.hpp>
#include <strstream>

cv::Mat& ScanImageAndReduceIterator(cv::Mat& I, const uchar* const table) {
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();
    switch (channels) {
        case 1:
        {
            cv::MatIterator_<uchar> it, end;
            for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it) {
                *it = table[*it];
            }
        }
        case 3:
        {
            cv::MatIterator_<cv::Vec3b> it, end;
            for (it = I.begin<cv::Vec3b>(), end = I.end<cv::Vec3b>(); it != end; ++it) {
                (*it)[0] = table[(*it)[0]];
                (*it)[1] = table[(*it)[1]];
                (*it)[2] = table[(*it)[2]];
            }
        }
        default:
            break;
    }

    return I;
}

cv::Mat& ScanImageAndReduceC(cv::Mat& I, const uchar* const table) {

    CV_Assert(I.depth() == CV_8U);

    int channels = I.channels();
    int nRows = I.rows;
    int nCols = I.cols * channels;

    if (I.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    for (int i = 0; i < nRows; ++i) {
        auto p = I.ptr<uchar>(i);
        for (int j = 0; j < nCols; ++j) {
            p[j] = table[p[j]];
        }
    }

    return I;
}

cv::Mat& ScanImageAndReduceRandomAccess(cv::Mat& I, const uchar* const table)
{
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();
    switch (channels) {
        case 1:
        {
            for (int i = 0; i < I.rows; ++i) {
                for (int j = 0; j < I.cols; ++j) {
                    I.at<uchar>(i, j) = table[I.at<uchar>(i, j)];
                }
            }
        }
            break;
        case 3:
        {
            cv::Mat_<cv::Vec3b> _I = I;
            for (int i = 0; i < I.rows; ++i) {
                for (int j = 0; j < I.cols; ++j) {
                    _I(i, j)[0] = table[_I(i, j)[0]];
                    _I(i, j)[1] = table[_I(i, j)[1]];
                    _I(i, j)[2] = table[_I(i, j)[2]];
                }
            }
            I = _I;
        }
            break;
        default:
            break;
    }

    return I;
}

cv::Mat& ScanImageAndReduceCoreFunction(cv::Mat& I, const uchar* const table)
{
    cv::Mat loopUpTable(1, 256, CV_8U);
    uchar* p = loopUpTable.ptr();
    for (int j = 0; j < 256; ++j) {
        p[j] = table[j];
    }

    cv::Mat J(I);
    cv::LUT(I, loopUpTable, J);
    J.copyTo(I);
    return I;
}

typedef cv::Mat& (ScanImageAndReduceFunction)(cv::Mat&, const uchar* const);

void ScanImageAndReduce(cv::Mat& I, const uchar* const table, ScanImageAndReduceFunction function, const char* funcName)
{
    double t = cv::getTickCount();
    function(I, table);
    t = ((double)cv::getTickCount()-t) / cv::getTickFrequency();
    t *= 1000;
    std::cout << "Time passed in milli seconds: " << funcName << " " << t << std::endl;
}

int main(int argc, char* argv[])
{
    int divideWith = 0;
    std::stringstream s;
    if (argc > 1) {
        s << argv[1];
        s >> divideWith;
    }
    if (!s || !divideWith) {
        std::cerr << "Invalid number entered for dividing." << std::endl;
        return EXIT_FAILURE;
    }

    uchar table[256];
    for (int i = 0; i < 256; ++i) {
        table[i] = static_cast<uchar>(divideWith * (i / divideWith));
    }

    cv::Mat image = cv::imread("../../footage/WindowsLogo.jpg", cv::IMREAD_COLOR);

    ScanImageAndReduce(image, table, ScanImageAndReduceC, "C style");
    ScanImageAndReduce(image, table, ScanImageAndReduceIterator, "Iterator");
    ScanImageAndReduce(image, table, ScanImageAndReduceRandomAccess, "Random access");
    ScanImageAndReduce(image, table, ScanImageAndReduceCoreFunction, "Core function");

    cv::imshow("show image", image);
    cv::waitKey(0);

    return EXIT_SUCCESS;
}
