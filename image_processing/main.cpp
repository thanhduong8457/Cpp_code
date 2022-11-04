#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
using namespace std;
int main()
{
    cv::Mat image;
    image = cv::imread("C:/Users/duong.le-thanh/Pictures/Camera Roll/aa.jpg", 1);
 
    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);
    cv::waitKey(0);
    return 0;
}