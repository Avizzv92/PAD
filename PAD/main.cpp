//
//  main.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 20/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include <opencv/cv.h>
#include <opencv2/video.hpp>
#include <regex>
#include <opencv2/highgui/highgui.hpp>

#include "EdgeDetect.hpp"
#include "DBManager.hpp"
#include "Logger.hpp"
#include "PADSettings.hpp"
#include "MotionDetect.hpp"

using namespace std;
using namespace cv;

//User Supplied Values (Test Values Below)
int CAMERA_ID;
int PARKING_LOT_ID;
string pKey;

DBManager dbm;

Mat videoFrame; //The current video frame being looked at
Mat roisOverlay; //The overlap which draws the colored polygons for the rois
vector<ROI> rois; //Holds all of the user defined regions of interest

void MouseCallBack(int event, int x, int y, int flags, void* userdata);
Mat detectMotion(Mat originalFrame);

int main(int argc, const char * argv[]) {
    CAMERA_ID = PADSettings::instance().getCameraId();
    PARKING_LOT_ID = PADSettings::instance().getParkingLotId();
    pKey = PADSettings::instance().getPrivateKey();
    
    //Attempt to validate with the server using IDs and the pKey
    if(dbm.isValid(pKey, PARKING_LOT_ID) != true){cout<<"Invalid IDs or Private Key"<<endl; return 1;}
    
    //Get ROIs from the DB.
    rois = dbm.getROIs(PARKING_LOT_ID);
    
    //Set up video capture
    VideoCapture cap;
    //cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.open(PADSettings::instance().getVideoCaptureSource());
    
    //Create window with mouse callback
    namedWindow("Parking Availability Detection",1);
    setMouseCallback("Parking Availability Detection", MouseCallBack, NULL);
    
    cap>>videoFrame;
    
    //Give this Mat an intial size equal to that of the video frame.
    roisOverlay = Mat::zeros( videoFrame.size(), CV_8UC3 );
    
    int frameDelay = PADSettings::instance().getFrameDelay();
    
    //Capturing a frame every X seconds infinetly.
    while(1) {
        try {
            //Grab the frame
            cap>>videoFrame;
            Mat originalFrame = videoFrame.clone();//VideoFrame will be modified for edge detection, we need an unmodified version for motion detection.
            
            //Reset the overlay, we wan't a clean start everytime.
            roisOverlay = Mat::zeros( videoFrame.size(), CV_8UC3 );

            //Detect the edges using canny edge detection
            EdgeDetect::cannyEdgeDetect(videoFrame);
            
            //Detect motion and get the generated mat to display later
            Mat motionMat = MotionDetect::detectMotion(originalFrame, rois);
            
            //Get individual mats within a frame from user defined ROIs
            vector<Mat> mats_edged = ROIUtils::getROIMats(rois, videoFrame);
            
            //Set white pixel counts for each of those mats to the individual regions
            ROIUtils::setWhitePixelCounts(mats_edged, rois);
            
            //Set occupation status of each roi via white pixel count
            ROIUtils::whitePixelOccupied(rois);
            
            //Draw the overlay, the regions are polygons that are green when unoccupied red when occupied.
            ROIUtils::drawROIsOnImage(rois, roisOverlay);
            
            //Convert video frame back to RGB to display the overlay indicators for ROIs which are colored
            cvtColor( videoFrame, videoFrame, CV_GRAY2RGB );
            videoFrame += motionMat;//Add motion detection mat
            videoFrame += roisOverlay;//Add roi overlay mat
            
            Logger::handleLogging(dbm, originalFrame+roisOverlay, CAMERA_ID, PARKING_LOT_ID, rois);//Handle the logging aspect (We want to show the user the original colored image + the overlayed ROIs
            
            imshow("Parking Availability Detection", videoFrame);
            
        } catch (Exception& e) {
            const char* err_msg = e.what();
            std::cout << "Exception caught : imshow:\n" << err_msg << std::endl;
        }
        
        waitKey(frameDelay);
    }
    
    return 0;
}

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
        } else if(clicks == 4){//Create ROI
            four = Point(x,y);
            
            ROI insert;
            insert.a = one;
            insert.b = two;
            insert.c = three;
            insert.d = four;
            insert.parking_lot_id = PARKING_LOT_ID;
            
            vector<Point> contour;
            contour.push_back(insert.a);
            contour.push_back(insert.b);
            contour.push_back(insert.c);
            contour.push_back(insert.d);
            insert.contour = contour;
            
            dbm.insertROI(insert);
            
            rois.push_back(insert);
            
            ROIUtils::getROIMats(rois, videoFrame);
            
            clicks = 0;
        }
    } else if (event == EVENT_RBUTTONUP) { //Delete Roi
        int indexToDelete = -1;
        
        for(int i = 0; i < rois.size(); i++) {
            ROI curr = rois[i];
            
            if(pointPolygonTest(curr.contour, Point(x,y), true) >= 0) {
                indexToDelete = i;
                break;
            }
        }
        
        if(indexToDelete != -1) {
            ROI roiToDelete = rois[indexToDelete];
            dbm.deleteROI(roiToDelete.id, PARKING_LOT_ID);
            rois.erase(rois.begin() + indexToDelete);
        }
    }
}