#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

int main(int argc, char** argv)
{
    std::vector<std::string> class_name;
    fstream ifs(string("../config/coco.names").c_str());
    string line;
    
    while (getline(ifs, line)) {
        class_name.push_back(line);
    }
    
    // load the neural network mode
    auto model = readNet("../config/frozen_inference_graph.pb", "../config/ssd_mobilenet_v3_large_coco_2020_01_14.pbtxt", "TensorFlow");
    
    // capture video
    VideoCapture cap("../source/test_video.mp4");
    
    // get the video frames with and height for proper saving videos
    int frame_width = static_cast<int>(cap.get(3));
    int frame_height = static_cast<int>(cap.get(4));
    
    // Create the VideoWriter() object
    VideoWriter out("../output/video_result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(frame_width, frame_height));

    Mat image, blob, output;
    bool isSuccess;
    int image_width, image_height, box_x, box_y, box_width, box_height, class_id, confidence;

    while (cap.isOpened()) {
        isSuccess = cap.read(image);

        if (!image.empty()) {
            cv::resize(image, image, cv::Size(300, 200));
        
            if (!isSuccess) {
                std::cout << "Image not found or unable to open" << std::endl;
                break;
            }
            
            image_width = image.cols;
            image_height = image.rows;
            
            // Create blob from image
            blob = blobFromImage(image, 1.0, cv::Size(image_width, image_height), Scalar(127.5, 127.5, 127.5), true, false);
            
            model.setInput(blob);
            
            output = model.forward();
            
            Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());
            
            for (int i = 0; i < detectionMat.rows; i++) {
                class_id = detectionMat.at<float>(i, 1);
                confidence = detectionMat.at<float>(i, 2);
                
                // Check if the dectection is good quality
                if (confidence > 0.4) {
                    box_x = static_cast<int>(detectionMat.at<float>(i, 3)*image.cols);
                    box_y = static_cast<int>(detectionMat.at<float>(i, 4)*image.rows);
                    box_width = static_cast<int>(detectionMat.at<float>(i, 5)*image.rows - box_x);
                    box_height = static_cast<int>(detectionMat.at<float>(i, 6)*image.rows - box_y);
                    rectangle(image, Point(box_x, box_y), Point(box_x + box_width, box_y + box_height), Scalar(255, 255, 255), 2);
                    putText(image, class_name[class_id-1].c_str(), Point(box_x, box_y-5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 255), 1);
                }
            }

            cv::resize(image, image, cv::Size(500, 300));
            
            imshow("image", image);
            out.write(image);

            if (113 == (int)(waitKey(1))) {
                break;
            }
        }
        else {
            break;
        }
    }
    
    cap.release();
    destroyAllWindows();

    return 0;
}
