//
//  ROIUtils.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 23/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include "ROIUtils.hpp"

void ROIUtils::drawROIsOnImage(vector<roi> rois, Mat &image) {
    for(int i = 0; i < rois.size(); i++) {
        const Point *pts = (const Point*) Mat(rois[i].contour).data;
        int npts = Mat(rois[i].contour).rows;
    
        //Draw the region, green polygon means unoccupied, red means occupied.
        if(!rois[i].occupied) {
            polylines(image, &pts, &npts, 1, true, Scalar(0,255,0), 1, CV_AA, 0);
        } else {
            polylines(image, &pts, &npts, 1, true, Scalar(0,0,255), 1, CV_AA, 0);
        }
        
        int totalPixels = contourArea(rois[i].contour);
        double percentCoverage = ((double)rois[i].whitePixelCount/(double)totalPixels)*100.0;
        
        stringstream ss;
        ss.precision(2);
        ss << percentCoverage;
        string percentageString = "ID:" + to_string(rois[i].id) + " " + ss.str() + "%";
        
        //Display pixel count to the user.
        putText(image, percentageString, rois[i].a, FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1.0);
    }
}

vector<Mat> ROIUtils::getROIMats(vector<roi> rois, Mat &image) {
    vector<Mat> mats;
    
    for(int i = 0; i < rois.size(); i++) {
        Mat submat;
        
        // Create Polygon from vertices
        vector<Point> ROI_Poly;
        approxPolyDP(rois[i].contour, ROI_Poly, 1.0, true);
        
        Mat mask = Mat::zeros( image.size(), image.type() );
        
        // Fill polygon white
        fillConvexPoly(mask, &ROI_Poly[0], (int)ROI_Poly.size(), 255, 8, 0);
        
        // Cut out ROI and store it in imageDest
        image.copyTo(submat, mask);
        
        mats.push_back(submat);
    }
    
    return mats;
}

void ROIUtils::setRedPixelCounts(vector<Mat> mats, vector<roi> &rois) {
    for(int i = 0; i < mats.size(); i++) {
        Mat mat = mats[i];
        vector<Mat> rgbChannels;
        split(mat, rgbChannels);
        int redPixels = countNonZero(rgbChannels[0]);
        rois[i].redPixelCount = redPixels;
    }
}

void ROIUtils::setWhitePixelCounts(vector<Mat> mats, vector<roi> &rois) {
    for(int i = 0; i < mats.size(); i++) {
        Mat mat = mats[i];
        int whitePixels = cv::countNonZero(mat >= 255/2);
        rois[i].whitePixelCount = whitePixels;
    }
}

void ROIUtils::whitePixelOccupied(vector<roi> &rois) {
    for(int i = 0; i < rois.size(); i++) {
        int totalPixels = contourArea(rois[i].contour);
        double percentCoverageWhite = ((double)rois[i].whitePixelCount/(double)totalPixels);
        double percentCoverageRed = ((double)rois[i].redPixelCount/(double)totalPixels);
        
        if(percentCoverageRed <= RED_PIXEL_COVERAGE_THRESHOLD) {
            rois[i].occupied = percentCoverageWhite <= rois[i].threshold ? false : true;
        }
    }
}