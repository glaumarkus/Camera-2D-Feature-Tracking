# Camera-2D-Feature-Tracking

The goal of this project is to implement and evaluate various keypoint detectors and descriptors in order to track features in a stream of images. With the information of how far keypoints move in between 2 or more images, a Time to Collision (TTC) can be calculated, to give useful insight on the surroundings without having access to additional sensor data as LIDAR/RADAR. 


## Data Source

10 Prefiltered images from the [Kitti dataset](http://www.cvlibs.net/datasets/kitti/) are used for the evaluation of this project. The images are then loaded into a deque of length 2, which performs better at inserting/deleting elements than a vector. 

Afterwards keypoint detection of a bunch of detectors is applied to the image (SHITOMASI, HARRIS, FAST, BRISK, ORB, AKAZE, SIFT). To focus only on the vehicle directly in front of us a region of interest filter is then applied to the detected keypoints. This helps in interpreting some of the later derived metrics to evaluate performance. The combinations of detector/descriptor may be changed in the combinations.txt within the parent directory. Deleting all but one is the equivalent of running the programm just on this. Also the visualisation code has been deleted from the source file, since all images are exported anyways in the imgs_matches/ folder.

With the result of detected and filtered keypoints, all combinations of valid keypoint descriptors (BRISK, BRIEF, ORB, FREAK, AKAZE, SIFT) are used to describe and match the keypoints of the images within the deque. Matching can be done with nearest neighbor / k- nearest neighbor. k- nearest neighbor features a distance ratio to remove wrong identified matches from the list. For the evaluation I didn't use this feature so the matches from the combination can also be evaluated. This feature as can be changed in the code at the very beginning. 

Evaluation of Combinations

// DF extract

The above data reflects all of the metrics used for the evaluation of the best detector/descriptor combination. Lets first take a brief look at different descriptors and their respective properties when averaging their respective statistics (pts, pts_on_car, detection_time). 

// plots detectors

### Detectors

Looking at the quick evalution above we can clearly already make a statement on some of the detectors. First of all it can be seen that AKAZE, SIFT and BRISK take way too long in general to be applied in a embedded system on a car with 100ms up to >300ms. Even though their overall performance in locating keypoints on the car seems reasonable seems "middle of the pack", I would assume at this point that ORB and FAST show at least equal performance in terms of being able to locate keypoints at a much faster speed. 

Average Keypoint detections range from around 100 up to 400 for one detector. This seems to be a reasonable number considering the cropped region of interest and the percentage of points on the car. ORB in general detects not many keypoints in the original image but has a really high percentage of detecting them on cars. This may be an advantage of using ORB for car detection, since other keypoints as a road/nature are not that relevant for image processing and are better clustered by using 3D sensors. 


### Describers

When looking at different describers, it is obvious that they are way less time consuming than the detectors. Overall statistics show a median of 0.32 ms, which is really slow. The one outlier with 4ms is actually the first calculation, so my conclusion would be to exclude that from the overall statistic. In general this seems not to be a time consuming task, therefore I would not value the execution time, but rather the robustness of the description and the matching process. 

// time

For analysing the robustness, I am filtering the original data to remove BRISK, AKAZE and SIFT. Then I want to calculate the percentage of matched points from 2 observations and also give some insight in the accuracy of the detection by the mean distance between keypoints in image 1 and 2. Lastly we take into account the overall speed of the combination.

// plots describers

Overall my recommendation would be to use either one of the following combinations:

- FAST-BRIEF
- FAST-ORB
- ORB-BRIEF

They all feature a comparatively great performance in speed and a really low distance of matched keypoints, which means a high likelyhood that the result will be accurate.  
