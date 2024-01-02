#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

const char* keys =
"{help h usage ? | | Usage examples: \n\t\t./object_detection_yolo.out --image=dog.jpg \n\t\t./object_detection_yolo.out --video=run_sm.mp4}"
"{image i        |<none>| input image   }"
"{video v        |<none>| input video   }"
"{device d       |<cpu>| input device   }"
;

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, keys);
    parser.about("Use this script to run object detection using YOLO3 in OpenCV.");
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    
    // Load names of classes
    std::vector<std::string> class_name;
    string classesFile = "../config/coco.names";
    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line)) {
        class_name.push_back(line);
    }

    string device = "cpu";
    device = parser.get<String>("device");
    
    // Give the configuration and weight files for the model
    String modelConfiguration = "../config/yolov3-tiny.cfg";
    String modelWeights = "../config/yolov3-tiny.weights";

    // Load the network
    Net net = readNetFromDarknet(modelConfiguration, modelWeights);

    if (device == "cpu") {
        cout << "Using CPU device" << endl;
        net.setPreferableBackend(DNN_TARGET_CPU);
    }
    else if (device == "gpu") {
        cout << "Using GPU device" << endl;
        net.setPreferableBackend(DNN_BACKEND_CUDA);
        net.setPreferableTarget(DNN_TARGET_CUDA);
    }

    // Open a video file or an image file or a camera stream.
    std::string str, outputFile;
    VideoCapture cap;
    VideoWriter video;
    Mat frame;

    str = "default.jpg";
    
    try {
        outputFile = "yolo_out_cpp.avi";
        if (parser.has("image")) {
            // Open the image file
            str = parser.get<String>("image");
            ifstream ifile(str);
            if (!ifile) {
                throw("error");
            }
            cap.open(str);
            str.replace(str.end()-4, str.end(), "_yolo_out_cpp.jpg");
            outputFile = str;
        }
        else if (parser.has("video")) {
            // Open the video file
            str = parser.get<String>("video");
            ifstream ifile(str);
            if (!ifile) throw("error");
            cap.open(str);
            str.replace(str.end()-4, str.end(), "_yolo_out_cpp.avi");
            outputFile = str;
        }
        else { // Open the webcam
            cap.open(parser.get<int>("device"));
        }
    }
    catch(...) {
        cout << "Could not open the input image/video stream" << endl;
        return 0;
    }
    
    // get the video frames with and height for proper saving videos
    int frame_width = static_cast<int>(cap.get(3));
    int frame_height = static_cast<int>(cap.get(4));
    
    // Create the VideoWriter() object
    VideoWriter out(outputFile, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(frame_width, frame_height));

    Mat image, blob, output;
    bool isSuccess;
    int image_width, image_height, box_x, box_y, box_width, box_height;

    while (cap.isOpened()) {
        isSuccess = cap.read(image);
        
        if (!isSuccess) {
            std::cout << "Image not found or unable to open" << std::endl;
            break;
        }
        
        image_width = image.cols;
        image_height = image.rows;
        
        // Create blob from image
        blob = blobFromImage(image, 1.0, cv::Size(image_width, image_height), Scalar(127.5, 127.5, 127.5), true, false);
        
        net.setInput(blob);
        
        output = net.forward();
        
        Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());
        
        for (int i = 0; i < detectionMat.rows; i++) {
            int class_id = detectionMat.at<float>(i, 1);
            float confidence = detectionMat.at<float>(i, 2);
            
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
        
        imshow("image", image);
        out.write(image);

        if (113 == (int)(waitKey(10))) {
            break;
        }
    }
    
    cap.release();
    destroyAllWindows();

    return 0;
}
