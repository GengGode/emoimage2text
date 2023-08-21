#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    std::string model_path = "model.onnx";
    std::string image_path = "test.jpg";

    // Load model
    cv::dnn::Net net = cv::dnn::readNetFromONNX(model_path);

    // Load image
    cv::Mat image = cv::imread(image_path);

    // Preprocess
    cv::Mat inputBlob = cv::dnn::blobFromImage(image, 1.0 / 255, cv::Size(64, 64), cv::Scalar(0, 0, 0), false, false);

    // Set input
    net.setInput(inputBlob);

    // Forward
    cv::Mat output = net.forward();

    // Get max score
    cv::Point max_loc;
    double max_score;
    cv::minMaxLoc(output, nullptr, &max_score, nullptr, &max_loc);

    // Print result
    std::cout << "Emotion: " << max_loc.x << std::endl;
    std::cout << "Score: " << max_score << std::endl;

    return 0;
}
```