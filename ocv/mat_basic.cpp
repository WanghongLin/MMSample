//
// Created by Wanghong Lin on 2018/10/11.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    cv::Mat M(2, 2, CV_8UC3, cv::Scalar(0, 0, 255));
    cout << "M = " << endl << " " << M << endl << endl;

    int sz[5] = {3, 3, 3, 3, 3};
    cv::Mat L(2, sz, CV_8UC1, cv::Scalar::all(0));
    cout << "empty " << L.empty() << endl << L.dims << " " << L.rows << 'x' << L.cols << endl;
    cout << "L = " << endl << " " << L << endl << endl;
}
