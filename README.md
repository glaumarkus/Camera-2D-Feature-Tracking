# Camera-2D-Feature-Tracking

The goal of this project is to implement and evaluate various keypoint detectors and descriptors in order to track features in a stream of images. With the information of how far keypoints move in between 2 or more images, a Time to Collision (TTC) can be calculated, to give useful insight on the surroundings without having access to additional sensor data as LIDAR/RADAR. This project is part of the Udacity Sensor Fusion nanodegree, official README at the bottom. 

<p align="center">
	<img src="/media/best_results.gif" alt="result"
	title="result"  />
</p>


## Data Source

10 Prefiltered images from the [Kitti dataset](http://www.cvlibs.net/datasets/kitti/) are used for the evaluation of this project. The images are then loaded into a deque of length 2, which performs better at inserting/deleting elements than a vector. 

Afterwards keypoint detection of a bunch of detectors is applied to the image (SHITOMASI, HARRIS, FAST, BRISK, ORB, AKAZE, SIFT). To focus only on the vehicle directly in front of us a region of interest filter is then applied to the detected keypoints. This helps in interpreting some of the later derived metrics to evaluate performance. The combinations of detector/descriptor may be changed in the combinations.txt within the parent directory. Deleting all but one is the equivalent of running the programm just on this. Also the visualisation code has been deleted from the source file, since all images are exported anyways in the imgs_matches/ folder.

With the result of detected and filtered keypoints, all combinations of valid keypoint descriptors (BRISK, BRIEF, ORB, FREAK, AKAZE, SIFT) are used to describe and match the keypoints of the images within the deque. Matching can be done with nearest neighbor / k- nearest neighbor. k- nearest neighbor features a distance ratio to remove wrong identified matches from the list. For the evaluation I didn't use this feature so the matches from the combination can also be evaluated. This feature as can be changed in the code at the very beginning. 

Evaluation of Combinations

<p align="center">
	<img src="/media/df_extract.PNG" alt="result"
	title="result"  />
</p>

### Detectors

The above data reflects all of the metrics used for the evaluation of the best detector/descriptor combination. Lets first take a brief look at different descriptors and their respective properties when averaging their respective statistics (pts, pts_on_car, detection_time). 

<p align="center">
	<img src="/media/detectors_plots.PNG" alt="result"
	title="result"  />
</p>

Looking at the quick evalution above we can clearly already make a statement on some of the detectors. First of all it can be seen that AKAZE, SIFT and BRISK take way too long in general to be applied in a embedded system on a car with 100ms up to >300ms. Even though their overall performance in locating keypoints on the car seems reasonable seems "middle of the pack", I would assume at this point that ORB and FAST show at least equal performance in terms of being able to locate keypoints at a much faster speed. 

Average Keypoint detections range from around 100 up to 400 for one detector. This seems to be a reasonable number considering the cropped region of interest and the percentage of points on the car. ORB in general detects not many keypoints in the original image but has a really high percentage of detecting them on cars. This may be an advantage of using ORB for car detection, since other keypoints as a road/nature are not that relevant for image processing and are better clustered by using 3D sensors. 


### Describers

When looking at different describers, it is obvious that they are way less time consuming than the detectors. Overall statistics show a median of 0.32 ms, which is really slow. The one outlier with 4ms is actually the first calculation, so my conclusion would be to exclude that from the overall statistic. In general this seems not to be a time consuming task, therefore I would not value the execution time, but rather the robustness of the description and the matching process. 

<p align="center">
	<img src="/media/describers_speed.PNG" alt="result"
	title="result"  />
</p>

For analysing the robustness, I am filtering the original data to remove BRISK, AKAZE and SIFT. Then I want to calculate the percentage of matched points from 2 observations and also give some insight in the accuracy of the detection by the mean distance between keypoints in image 1 and 2. Lastly we take into account the overall speed of the combination.

<p align="center">
	<img src="/media/combs_plots.PNG" alt="result"
	title="result"  />
</p>

Overall my recommendation would be to use either one of the following combinations:

- FAST-BRIEF
- FAST-ORB
- ORB-BRIEF

They all feature a comparatively great performance in speed and a really low distance of matched keypoints, which means a high likelyhood that the result will be accurate.  





# SFND 2D Feature Tracking

The idea of the camera course is to build a collision detection system - that's the overall goal for the Final Project. As a preparation for this, you will now build the feature tracking part and test various detector / descriptor combinations to see which ones perform best. This mid-term project consists of four parts:

* First, you will focus on loading images, setting up data structures and putting everything into a ring buffer to optimize memory load. 
* Then, you will integrate several keypoint detectors such as HARRIS, FAST, BRISK and SIFT and compare them with regard to number of keypoints and speed. 
* In the next part, you will then focus on descriptor extraction and matching using brute force and also the FLANN approach we discussed in the previous lesson. 
* In the last part, once the code framework is complete, you will test the various algorithms in different combinations and compare them with regard to some performance measures. 

See the classroom instruction and code comments for more details on each of these parts. Once you are finished with this project, the keypoint matching part will be set up and you can proceed to the next lesson, where the focus is on integrating Lidar points and on object detection using deep-learning. 

## Dependencies for Running Locally
* cmake >= 2.8
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* OpenCV >= 4.1
  * This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors.
  * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`.
