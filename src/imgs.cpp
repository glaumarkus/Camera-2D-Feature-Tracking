
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


// featuring blur & sobel_x & sobel_y
void img_pipeline(cv::Mat& img){

    // cvt color
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

    // gaussian blur
    cv::Mat img_blur;
    cv::GaussianBlur(img_gray, img_blur, cv::Size(params.kernel_size, params.kernel_size), params.sigma_x);

    // create filter kernels
    cv::Mat kernel_x = cv::Mat(3, 3, CV_32F, {-1, 0, +1, -2, 0, +2, -1, 0, +1});
    cv::Mat kernel_y = cv::Mat(3, 3, CV_32F, {-1, -2, -1, 0, 0, 0, +1, +2, +1});

    // apply filter
    cv::Mat result_x, result_y;
    cv::filter2D(blurred, result_x, -1, kernel_x, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    cv::filter2D(blurred, result_y, -1, kernel_y, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

    // compute magnitude image
    cv::Mat magnitude = imgGray.clone();
    for (int r = 0; r < magnitude.rows; r++)
    {
        for (int c = 0; c < magnitude.cols; c++)
        {
            magnitude.at<unsigned char>(r, c) = sqrt(pow(result_x.at<unsigned char>(r, c), 2) +
                                                     pow(result_y.at<unsigned char>(r, c), 2));
        }
    }

    std::string windowName = "Gaussian Blurring";
    cv::namedWindow(windowName, 1); // create window
    cv::imshow(windowName, img_gray);
    cv::waitKey(0); // wait for keyboard input before continuing

}

void img_pipeline2(cv::Mat& img){
    
    // Detector parameters
    int blockSize = 2; // for every pixel, a blockSize × blockSize neighborhood is considered
    int apertureSize = 3; // aperture parameter for Sobel operator (must be odd)
    int minResponse = 100; // minimum value for a corner in the 8bit scaled response matrix
    double k = 0.04; // Harris parameter (see equation for details)

    // Detect Harris corners and normalize output
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(imgGray.size(), CV_32FC1 );
    cv::cornerHarris( imgGray, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT ); 
    cv::normalize( dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );
    cv::convertScaleAbs( dst_norm, dst_norm_scaled );

}

int main(){

    // start loop with loading params again
    if (!params.fromFile(paramsFilePath)) {
        std::cerr << "Error reading params file" << std::endl;
        return 1;
    }

    cv::Mat img = cv::imread("../images/img1gray.png");
    img_pipeline(img);
    return 0;
}
