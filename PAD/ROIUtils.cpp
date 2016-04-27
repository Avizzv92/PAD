//
//  ROIUtils.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 23/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include "ROIUtils.hpp"

void ROIUtils::drawROIsOnImage(vector<ROI> rois, Mat &image) {
    for(int i = 0; i < rois.size(); i++) {
        const Point *pts = (const Point*) Mat(rois[i].contour).data;
        int npts = Mat(rois[i].contour).rows;
    
        //Draw the region, green polygon means unoccupied, red means occupied.
        if(!rois[i].getOccupied()) {
            polylines(image, &pts, &npts, 1, true, Scalar(0,255,0), 1, CV_AA, 0);
        } else {
            polylines(image, &pts, &npts, 1, true, Scalar(0,0,255), 1, CV_AA, 0);
        }
        
        //Display pixel count to the user.
        ROIUtils::addLabel(image, rois[i]);
    }
}

void ROIUtils::addLabel(cv::Mat& image, const ROI &roi) {
    //Get the label
    int totalPixels = contourArea(roi.contour);
    double percentCoverage = ((double)roi.whitePixelCount/(double)totalPixels)*100.0;
    
    stringstream ss;
    ss.precision(2);
    ss << percentCoverage;
    string label = roi.description + "-" + ss.str() + "%";

    //Get center point
    double avgX = ( ( (roi.a.x + roi.b.x)/2 ) + ( (roi.c.x + roi.d.x)/2 ) ) / 2;
    double avgY = ( ( (roi.a.y + roi.b.y)/2 ) + ( (roi.c.y + roi.d.y)/2 ) ) / 2;
    Point point = Point(avgX,avgY);
    int baseline = 0;
    //Draw Everything
    Size textSize = getTextSize(label, FONT_HERSHEY_PLAIN, 1.0, 1.0, &baseline);
    rectangle(image, point + Point(-textSize.width/2, baseline), point + Point(textSize.width/2, -textSize.height), CV_RGB(50,50,50), CV_FILLED);
    putText(image, label, point + Point(-textSize.width/2,3), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1.0);
}

vector<Mat> ROIUtils::getROIMats(vector<ROI> rois, Mat &image) {
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

void ROIUtils::setRedPixelCounts(vector<Mat> mats, vector<ROI> &rois) {
    for(int i = 0; i < mats.size(); i++) {
        Mat mat = mats[i];
        vector<Mat> rgbChannels;
        split(mat, rgbChannels);
        int redPixels = countNonZero(rgbChannels[0]);
        rois[i].redPixelCount = redPixels;
    }
}

void ROIUtils::setWhitePixelCounts(vector<Mat> mats, vector<ROI> &rois) {
    for(int i = 0; i < mats.size(); i++) {
        Mat mat = mats[i];
        int whitePixels = cv::countNonZero(mat >= 255/2);
        rois[i].whitePixelCount = whitePixels;
    }
}

void ROIUtils::whitePixelOccupied(vector<ROI> &rois) {
    for(int i = 0; i < rois.size(); i++) {
        int totalPixels = contourArea(rois[i].contour);
        double percentCoverageWhite = ((double)rois[i].whitePixelCount/(double)totalPixels);
        double percentCoverageRed = ((double)rois[i].redPixelCount/(double)totalPixels);
        
        if(percentCoverageRed <= RED_PIXEL_COVERAGE_THRESHOLD) {
            rois[i].setOccupied(percentCoverageWhite <= rois[i].threshold ? false : true);
        }
    }
}