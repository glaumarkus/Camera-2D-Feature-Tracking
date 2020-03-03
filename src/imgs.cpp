
/*
Pipeline

- load image
- apply cv::GaussianBlur (params: Kernel, std_dev)
- compute Intensity Gradient (Sobel / Scharr)

*/

#include <iostream>
#include <numeric>
#include <string> 
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "params.h"

static std::string paramsFilePath = "../params.txt";
static PipelineParams params;

void img_pipeline(cv::mat& img){

    // cvt color
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

    // gaussian blur
    cv::Mat img_blur;
    cv::GaussianBlur(img_gray, img_blur, cv::Size(kernel_size, kernel_size), sigma_x);

    string windowName = "Gaussian Blurring";
    cv::namedWindow(windowName, 1); // create window
    cv::imshow(windowName, img_blur);
    cv::waitKey(0); // wait for keyboard input before continuing

}

int main(){

    // start loop with loading params again
    if (!params.fromFile(paramsFilePath)) {
        std::cerr << "Error reading params file" << std::endl;
        return;
    }

    cv::mat img = cv::imread("../images/img1gray.png");
    img_pipeline(img);
}