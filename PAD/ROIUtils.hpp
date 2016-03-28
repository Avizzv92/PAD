//
//  ROIUtils.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 23/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

struct roi {
    int id;
    int parking_lot_id;
    String description; //descriptive id - currently unused?
    Point a, b, c, d; //the four points that form this roi
    bool occupied = false; //whether or not this roi is considered occupied
    int whitePixelCount = 0; //number of white pixels in this roi
};

class ROIUtils {
private:
    //If the white pixel count exceeds this threshold, the roi is occupied
    static const int WHITE_PIXEL_THRESHOLD = 0;
public:
    //Draws ROIs as polygons on the image.
    static void drawROIsOnImage(vector<roi> rois, Mat &image);
    //Get individual mats from the defined rois.
    static vector<Mat> getROIMats(vector<roi> rois, Mat &image);
    //Set the white pixel counts of each rois, via the submats we are passing in for each parking spot.
    static void setWhitePixelCounts(vector<Mat> mats, vector<roi> &rois);
    //Determine if a given roi is occupied based on its white pixel count.
    static void whitePixelOccupied(vector<roi> &rois);
};
