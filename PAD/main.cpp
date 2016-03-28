//
//  main.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 20/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "EdgeDetect.hpp"
#include "DBManager.hpp"

using namespace std;
using namespace cv;

const int CAMERA_ID = 1;
const int PARKING_LOT_ID = 1;
const int FRAME_DELAY = 1000;
const int LOG_TIME = 1000*60*5;//5 mins

DBManager dbm;

Mat videoFrame; //The current video frame being looked at
Mat roisOverlay; //The overlap which draws the colored polygons for the rois
vector<roi> rois; //Holds all of the user defined regions of interest

int clicks = 0; //Keeps track of mouse clicks to know what point is being assigned.
Point one, two, three, four; //The points being given by the user to build the rois

void MouseCallBack(int event, int x, int y, int flags, void* userdata) {
    //Create ROI
    if( event == EVENT_LBUTTONDOWN ) {
        clicks++;
        
        if(clicks == 1){
            one = Point(x,y);
        } else if(clicks == 2){
            two = Point(x,y);
        } else if(clicks == 3){
            three = Point(x,y);
        } else if(clicks == 4){
            four = Point(x,y);
            
            roi insert;
            insert.a = one;
            insert.b = two;
            insert.c = three;
            insert.d = four;
            insert.parking_lot_id = PARKING_LOT_ID;
            dbm.insertROI(insert);

            rois.push_back(insert);
            
            ROIUtils::getROIMats(rois, videoFrame);

            clicks = 0;
        }
    } else if (event == EVENT_RBUTTONUP) { //Delete Roi
        int indexToDelete = -1;

        for(int i = 0; i < rois.size(); i++) {
            roi curr = rois[i];
            vector<Point> contour;
            contour.push_back(curr.a);
            contour.push_back(curr.b);
            contour.push_back(curr.c);
            contour.push_back(curr.d);
            if(pointPolygonTest(contour, Point(x,y), true) >= 0) {
                indexToDelete = i;
                break;
            }
        }
        
        if(indexToDelete != -1) {
            roi roiToDelete = rois[indexToDelete];
            dbm.deleteROI(roiToDelete.id);
            rois.erase(rois.begin() + indexToDelete);
        }
    }
}

int main(int argc, const char * argv[]) {
    //Get ROIs from the DB.
    rois = dbm.getROIs();
    int timeSinceLastLog = 0;
    
    //Set up video capture
    VideoCapture cap;
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.open(0);
    
    //Create window with mouse callback
    namedWindow("window",1);
    setMouseCallback("window", MouseCallBack, NULL);

    cap>>videoFrame;
    
    //Give this Mat an intial size equal to that of the video frame.
    roisOverlay = Mat::zeros( videoFrame.size(), CV_8UC3 );
    
    //Capturing a frame every X seconds infinetly.
    while(1) {
        try {
            //Grab the frame
            cap>>videoFrame;
            
            //Reset the overlay, we wan't a clean start everytime.
            roisOverlay = Mat::zeros( videoFrame.size(), CV_8UC3 );

            //Detect the edges
            //Canny Edge
            EdgeDetect::cannyEdgeDetect(videoFrame);
            //Sobel Edge
            //EdgeDetect::sobelEdgeDetect(videoFrame);
  
            //Get individual mats within a frame from user defined ROIs
            vector<Mat> mats = ROIUtils::getROIMats(rois, videoFrame);
            
            //Set white pixel counts for each of those mats to the individual regions
            ROIUtils::setWhitePixelCounts(mats, rois);
            
            //Set occupation status of each roi via white pixel count
            ROIUtils::whitePixelOccupied(rois);
            
            //Draw the overlay, the regions are polygons that are green when unoccupied red when occupied.
            ROIUtils::drawROIsOnImage(rois, roisOverlay);
            
            //Convert video frame back to RGB to display the overlay indicators for ROIs which are colored
            cvtColor( videoFrame, videoFrame, CV_GRAY2RGB );
            videoFrame += roisOverlay;
            
            //Log occupancy information into the DB every 10 mins? Rarely would someone be in a parking spot for < 10 mins?
            timeSinceLastLog += FRAME_DELAY;
            
            if(timeSinceLastLog >= LOG_TIME) {
                timeSinceLastLog = 0;
                dbm.logOccupancy(CAMERA_ID, rois);
            }
            
            imshow("window", videoFrame);
            
        } catch (Exception& e) {
            const char* err_msg = e.what();
            std::cout << "Exception caught : imshow:\n" << err_msg << std::endl;
        }
        
        waitKey(FRAME_DELAY);
    }
    
    return 0;
}
