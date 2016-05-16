//
//  MotionDetect.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 16/05/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include "MotionDetect.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video.hpp>

Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2(100, 16, false);

Mat MotionDetect::detectMotion(Mat originalFrame, vector<ROI> &rois) {
    //If motion is detected it is drawn in red pixels
    Mat fgMaskMOG2;
    pMOG2->apply(originalFrame, fgMaskMOG2);
    cvtColor(fgMaskMOG2, fgMaskMOG2, CV_GRAY2RGB);
    Mat fgMaskMOG2_red;
    inRange(fgMaskMOG2,Scalar(255,255,255),Scalar(255,255,255),fgMaskMOG2_red);
    fgMaskMOG2.setTo(Scalar(0,0,255),fgMaskMOG2_red);
    //Get individual mats within a frame from user defined ROIs (for motion)
    vector<Mat> mats_motion = ROIUtils::getROIMats(rois, fgMaskMOG2_red);
    
    //Set the red pixel counts for each roi to determine if motion is occuring within it.
    ROIUtils::setRedPixelCounts(mats_motion, rois);
    
    return fgMaskMOG2;
}