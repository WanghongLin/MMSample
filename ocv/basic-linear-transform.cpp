//
// Created by Wanghong Lin on 2018/10/24.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using std::cin;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, "{@input | ../data/lena.jpg | input image}");

    cv::Mat image = cv::imread(parser.get<cv::String>("@input"));

    if (image.empty()) {
        cout << "Could not open or find the image!\n" << endl;
        cout << "Usage: " << argv[0] << " <Input image>" << endl;
        return EXIT_FAILURE;
    }

    cv::Mat new_image = cv::Mat::zeros(image.size(), image.type());

    double alpha = 1.0;
    int beta = 0;

    cout << " Basic Linear Transforms " << endl;
    cout << "------------------------ " << endl;
    cout << "* Enter the alpha value [1.0-3.0]: "; cin >> alpha;
    cout << "* Enter the beta value [0-100]: "; cin >> beta;

    for (int j = 0; j < image.rows; ++j) {
        for (int i = 0; i < image.cols; ++i) {
            for (int k = 0; k < image.channels(); ++k) {
                new_image.at<cv::Vec3b>(i, j)[k] = cv::saturate_cast<uchar>(alpha*image.at<cv::Vec3b>(i, j)[k] + beta);
            }
        }
    }

    cv::cvtColor(image, new_image, cv::COLOR_BGR2HLS);
    cv::imshow("Original image", image);
    cv::imshow("New Image", new_image);
    cv::waitKey();

    return EXIT_SUCCESS;
}

