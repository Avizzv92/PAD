//
//  ROIUtils.hpp
//  PAD
//
//  Created by Aaron Vizzini on 23/03/2016.
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
    String id;
    Point a, b, c, d;
    bool occupied = false;
};

class ROIUtils {    
    public:
    //Draws ROIs as polygons on the image.
    static void drawROIsOnImage(vector<roi> rois, Mat &image);
    static vector<Mat> getROIMats(vector<roi> rois, Mat &image);
};
