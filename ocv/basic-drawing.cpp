//
// Created by Wanghong Lin on 2018/10/25.
//

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#define w 400

using namespace cv;
using std::cout;
using std::cin;
using std::endl;

void MyEllipse(Mat img, double angle);
void MyFilledCircle(Mat img, Point center);
void MyPolygon(Mat img);
void MyLine(Mat img, Point start, Point end);

int main(int argc, char* argv[])
{
    char atom_window[] = "Drawing 1: Atom";
    char rook_window[] = "Drawing 2: Rook";

    Mat atom_image = Mat::zeros(w, w, CV_8UC3);
    Mat rook_image = Mat::zeros(w, w, CV_8UC3);

    int angles[] = {90, 0, 45, -45};
    for (int angle : angles) {
        MyEllipse(atom_image, angle);
    }
    MyFilledCircle(atom_image, Point(w/2, w/2));

    MyPolygon(rook_image);
    rectangle(rook_image, Point(0, 7*w/8),
              Point(w, w),
              Scalar(0, 255, 255),
              FILLED,
              LINE_8);

    std::vector< std::pair<Point, Point> > points{
            std::make_pair(Point(0, 15*w/16), Point(w, 15*w/16)),
            std::make_pair(Point(w/4, 7*w/8), Point(w/4, w)),
            std::make_pair(Point(w/2, 7*w/8), Point(w/2, w)),
            std::make_pair(Point(3*w/4, 7*w/8), Point(3*w/4, w))
    };
    for (auto p : points) {
        MyLine(rook_image, p.first, p.second);
    }

    imshow(atom_window, atom_image);
    moveWindow(atom_window, 0, 200);
    imshow(rook_window, rook_image);
    moveWindow(rook_window, w, 200);

    waitKey();
    return EXIT_SUCCESS;
}

void MyEllipse(Mat img, double angle) {

    int thickness = 2;
    int lineType = LINE_8;

    ellipse(img,
            Point(w/2, w/2),
            Size(w/4, w/16),
            angle,
            0,
            360,
            Scalar(255, 0, 0),
            thickness,
            lineType);
}

void MyFilledCircle(Mat img, Point center) {
    circle(img,
           center,
           w/32,
           Scalar(0, 0, 255),
           FILLED,
           LINE_8);
}

void MyPolygon(Mat img) {
    int lineType = LINE_8;

    Point rook_points[1][20] = {
            {
                    Point(    w/4,   7*w/8 ),
                    Point(  3*w/4,   7*w/8 ),
                    Point(  3*w/4,  13*w/16),
                    Point( 11*w/16, 13*w/16),
                    Point( 19*w/32,  3*w/8 ),
                    Point(  3*w/4,   3*w/8 ),
                    Point(  3*w/4,     w/8 ),
                    Point( 26*w/40,    w/8 ),
                    Point( 26*w/40,    w/4 ),
                    Point( 22*w/40,    w/4 ),
                    Point( 22*w/40,    w/8 ),
                    Point( 18*w/40,    w/8 ),
                    Point( 18*w/40,    w/4 ),
                    Point( 14*w/40,    w/4 ),
                    Point( 14*w/40,    w/8 ),
                    Point(    w/4,     w/8 ),
                    Point(    w/4,   3*w/8 ),
                    Point( 13*w/32,  3*w/8 ),
                    Point(  5*w/16, 13*w/16),
                    Point(    w/4,  13*w/16)
            }
    };

    const Point* ppt[1] = { rook_points[0] };
    int npt[] = { 20 };
    fillPoly(img, ppt, npt, 1, Scalar(255, 255, 255), lineType);
}

void MyLine(Mat img, Point start, Point end) {
    int thickness = 2;
    int lineType = LINE_8;
    line(img, start, end, Scalar(0, 0, 0), thickness, lineType);
}

