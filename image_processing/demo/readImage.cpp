#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main(int argc, char** argv)
{
    cv::Mat image;
#ifdef WINDOW_DEBUG
    image = cv::imread("C:\\Users\\duong.le-thanh\\Documents\\my_code\\image_processing\\demo\\input\\bb.PNG", cv::IMREAD_COLOR);
#else
    image = cv::imread("../images/bb.PNG", cv::IMREAD_COLOR);
#endif

    if (!image.data) {
        std::cout << "Image not found or unable to open" << std::endl;
        return -1;
    }

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);

    cv::waitKey(0);
    return 0;
}
