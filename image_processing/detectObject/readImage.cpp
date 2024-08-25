#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main(int argc, char** argv)
{
    cv::Mat image;
    image = cv::imread("/Users/thanhduong/Pictures/NinhThuan/_DSC0011.JPG", cv::IMREAD_COLOR);


    if (!image.data) {
        std::cout << "Image not found or unable to open" << std::endl;
        return -1;
    }

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);

    cv::waitKey(0);
    return 0;
}
