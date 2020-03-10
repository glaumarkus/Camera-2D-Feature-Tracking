/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <limits>
#include <utility>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "dataStructures.h"
#include "matching2D.hpp"

#include <deque>    
#include <stdio.h>

using namespace std;

// reads/stores all possible pairs of detector/descriptor from file
struct combinations{
    std::vector<std::pair<std::string, std::string>> combination;

    bool get_combinations(const std::string &path){
        std::ifstream file(path);

        if (!file.is_open()){
            std::cerr << "File not found" << std::endl;
            return false;
        }

        std::string line;
        while(getline(file,line)){
            std::pair<std::string, std::string> temp_comb;
            std::size_t pos = line.find(" ");
            temp_comb = {line.substr(0,pos), line.substr(pos+1, line.length()-1)};
            combination.push_back(temp_comb);
        }

        file.close();
        return true;
    }
};

struct timer{
    float t;
    float get_time(){
        return (static_cast<float>(cv::getTickCount())-t)/1000000;
    }
};

/* MAIN PROGRAM */
int main(int argc, const char *argv[])
{

    /*
    an input file is used to evaluate all possible combinations of detectors/descriptors,
    which are iterated through the evaluation pipeline
    */
    combinations combs;
    combs.get_combinations("../combinations.txt");

    /* 
    performance tracking file used for evaluation, deleted if already exists
    */
    if( remove( "../performance.txt" ) != 0 )
        perror( "No file to delete" );
    else
        puts( "File successfully deleted" );

    // outstream, print headers
    ofstream performance;
    performance.open("../performance.txt");
    performance << "detector" << "," 
      << "descriptor" << ","
      << "img_id" << ","
      << "pts" << ","
      << "pts_on_car" << ","
      << "matches" << ","
      << "detection_time" << ","
      << "description_time" << ","
      << "total_time" << ","
      << "match_dist"
      << std::endl;
  

    //
    timer time;

    // for each combination get everything
    for (auto& comb: combs.combination){

        // Input parameters.
        string detectorType = comb.first;           // SHITOMASI; HARRIS; FAST; BRISK; ORB; AKAZE; SIFT
        string descriptorType = comb.second;        // BRISK; BRIEF, ORB, FREAK, AKAZE, SIFT
        string matcherType = "MAT_BF";              // MAT_BF, MAT_FLANN
        string selectorType = "SEL_NN";             // SEL_NN, SEL_KNN
        bool bVis = true;                           // visualize results
      
        std::cout << detectorType + " " + descriptorType << std::endl;
        

        string descriptorTypeAdj = descriptorType.compare("SIFT") == 0 ? "DES_HOG" : "DES_BINARY"; // DES_BINARY, DES_HOG

        /* INIT VARIABLES AND DATA STRUCTURES */

        // data location
        string dataPath = "../";

        // camera
        string imgBasePath = dataPath + "images/";
        string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
        string imgFileType = ".png";
        int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
        int imgEndIndex = 9;   // last file index to load
        int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

        // misc
        int dataBufferSize = 2;       // no. of images which are held in memory (ring buffer) at the same time
        std::deque<DataFrame> dataBuffer; // Use deque for FIFO ring buffer.

        /* MAIN LOOP OVER ALL IMAGES */

        for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
        {

            /* LOAD IMAGE INTO BUFFER */

            // assemble filenames for current index
            ostringstream imgNumber;
            imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
            string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

            // load image from file and convert to grayscale
            cv::Mat img, imgGray;
            img = cv::imread(imgFullFilename);
            cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
          

            //// STUDENT ASSIGNMENT
            //// TASK MP.1 -> replace the following code with ring buffer of size dataBufferSize
            // Added the sfnd::RingBuffer data structure, which implements part of the std-container interface.

            // push image into data frame buffer
            DataFrame frame;
            frame.cameraImg = imgGray;
            dataBuffer.push_back(frame);
            if (dataBuffer.size() > dataBufferSize)
                dataBuffer.pop_front();

            //// EOF STUDENT ASSIGNMENT

            /* DETECT IMAGE KEYPOINTS */

            // extract 2D keypoints from current image
            vector<cv::KeyPoint> keypoints; // create empty feature list for current image

            //// STUDENT ASSIGNMENT
            //// TASK MP.2 -> add the following keypoint detectors in file matching2D.cpp and enable string-based selection based on detectorType
            //// -> HARRIS, FAST, BRISK, ORB, AKAZE, SIFT
            
            time.t = static_cast<float>(cv::getTickCount());

            if (detectorType.compare("SHITOMASI") == 0)
                detKeypointsShiTomasi(keypoints, imgGray, false);
            else if (detectorType.compare("HARRIS") == 0)
                detKeypointsHarris(keypoints, imgGray, false);
            else
                detKeypointsModern(keypoints, imgGray, detectorType, false);

            float time_detection = time.get_time();

            //// EOF STUDENT ASSIGNMENT

            //// STUDENT ASSIGNMENT
            //// TASK MP.3 -> only keep keypoints on the preceding vehicle

            // only keep keypoints on the preceding vehicle
            bool bFocusOnVehicle = true;
            cv::Rect vehicleRect(535, 180, 180, 150);
            
            // for performance
            size_t pts = 0;
            size_t pts_car = 0;
            pts = keypoints.size();
            
            if (bFocusOnVehicle)
            {
                std::vector<cv::KeyPoint> pts_on_vehicle;
                for (auto keypoint : keypoints)
                    if (vehicleRect.contains(keypoint.pt))
                        pts_on_vehicle.push_back(keypoint);
                
                keypoints = pts_on_vehicle;
                pts_car = keypoints.size();
            }

            //// EOF STUDENT ASSIGNMENT

            // optional : limit number of keypoints (helpful for debugging and learning)
            bool bLimitKpts = false;
            if (bLimitKpts)
            {
                int maxKeypoints = 50;

                if (detectorType.compare("SHITOMASI") == 0)
                { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                    keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
                }

                cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
            }

            // push keypoints and descriptor for current frame to end of data buffer
            (dataBuffer.end() - 1)->keypoints = keypoints;

            /* EXTRACT KEYPOINT DESCRIPTORS */

            //// STUDENT ASSIGNMENT
            //// TASK MP.4 -> add the following descriptors in file matching2D.cpp and enable string-based selection based on descriptorType
            //// -> BRIEF, ORB, FREAK, AKAZE, SIFT

            cv::Mat descriptors;
            descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType);

            // push descriptors for current frame to end of data buffer
            (dataBuffer.end() - 1)->descriptors = descriptors;
            //// EOF STUDENT ASSIGNMENT

            if (dataBuffer.size() > 1) // wait until at least two images have been processed
            {

                /* MATCH KEYPOINT DESCRIPTORS */
                vector<cv::DMatch> matches;

                time.t = static_cast<float>(cv::getTickCount());

                //// STUDENT ASSIGNMENT
                //// TASK MP.5 -> add FLANN matching in file matching2D.cpp
                //// TASK MP.6 -> add KNN match selection and perform descriptor distance ratio filtering with t=0.8 in file matching2D.cpp
                matchDescriptors(
                    (dataBuffer.end() - 2)->keypoints, 
                    (dataBuffer.end() - 1)->keypoints,
                    (dataBuffer.end() - 2)->descriptors,
                    (dataBuffer.end() - 1)->descriptors,
                    matches, 
                    descriptorTypeAdj, 
                    matcherType, 
                    selectorType
                );

                float time_describtion = time.get_time();
              
                // get match accuracy
                double total_dist = 0;
                for (auto& m: matches){
                  total_dist += m.distance;
                }
                total_dist /= matches.size();
              

                //// EOF STUDENT ASSIGNMENT

                // store matches in current data frame
                (dataBuffer.end() - 1)->kptMatches = matches;

                // visualize matches between current and previous image for export
                cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                cv::drawKeypoints ((dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints, matchImg, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
                
                // put text
                cv::putText(matchImg, detectorType + " - " + descriptorType, cv::Point(30,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cv::Scalar(0,255,0));
                cv::putText(matchImg, "Keypoints: " + to_string(pts_car), cv::Point(30,70), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cv::Scalar(0,255,0));
                cv::putText(matchImg, "Time: " + to_string(time_detection + time_describtion), cv::Point(30,110), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cv::Scalar(0,255,0));
                
          
                if (cv::imwrite("../imgs_keypoints/" + detectorType + "_"+ descriptorType + imgNumber.str() + "_org.png", matchImg))
                {}
              
                matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
              
                cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                matches, matchImg,
                                cv::Scalar::all(-1), cv::Scalar::all(-1),
                                vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
              
                // put text
                cv::putText(matchImg, detectorType + " - " + descriptorType, cv::Point(30,30), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cv::Scalar(0,255,0));
                cv::putText(matchImg, "Time: " + to_string(time_detection + time_describtion), cv::Point(30,70), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cv::Scalar(0,255,0));
              
                // save altered img & write performance on picture
                if (cv::imwrite("../imgs_matches/" + detectorType + "_"+ descriptorType + imgNumber.str() + ".png", matchImg))
                {
                  
                  performance << detectorType << "," 
                  << descriptorType << ","
                  << imgNumber.str() << ","
                  << pts << ","
                  << pts_car << ","
                  << (dataBuffer.end() - 1)->kptMatches.size() << ","
                  << time_detection << ","
                  << time_describtion << ","
                  << (time_detection + time_describtion) << ","
                  << total_dist 
                  << std::endl;
                }
                else
                  continue;
            }
        }
    }
    performance.close();
    return 0;
}