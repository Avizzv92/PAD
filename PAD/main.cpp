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
#include "opencv2/videoio.hpp"
#include <opencv2/video.hpp>
#include <curl/curl.h>
#include <stdio.h>
#include <thread>
#include <regex>
#include <ctime>

#include "PADSettings.hpp"

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

void handleLogging(Mat matToLog);
void MouseCallBack(int event, int x, int y, int flags, void* userdata);
Mat detectMotion(Mat originalFrame);
void sendImageToServer(string fileName, string fileNameWExt);

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
    namedWindow("window",1);
    setMouseCallback("window", MouseCallBack, NULL);

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
            Mat motionMat = detectMotion(originalFrame);
            
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
            
            handleLogging(originalFrame+roisOverlay);//Handle the logging aspect (We want to show the user the original colored image + the overlayed ROIs
            
            imshow("window", videoFrame);
            
        } catch (Exception& e) {
            const char* err_msg = e.what();
            std::cout << "Exception caught : imshow:\n" << err_msg << std::endl;
        }
        
        waitKey(frameDelay);
    }
    
    return 0;
}

Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2(100, 16, false);

Mat detectMotion(Mat originalFrame) {
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

//Handles logging actions
time_t lastLongTime = time(0);

void handleLogging(Mat matToLog) {
    
    time_t now = time(0);

    if((now - lastLongTime) >= PADSettings::instance().getLogTime()) {
        lastLongTime = now;
        
        dbm.logOccupancy(CAMERA_ID, rois);
        
        string fileName = "logImg_"+to_string(PARKING_LOT_ID);
        string fileNameWExt = "logImg_"+to_string(PARKING_LOT_ID)+".png";
        
        imwrite(fileNameWExt, matToLog);
        
        if(PADSettings::instance().getIsTesting()) {
            string sampleFileNameWExt = "/Volumes/PADSamples/sampleImg_"+to_string(now)+".png";
            imwrite(sampleFileNameWExt, matToLog);
        }
        
        thread uploadThread(sendImageToServer, fileName, fileNameWExt);
        uploadThread.join();
    }
}

//Send a single frame to the server for logging purposes (only one ever exists at a single time for a single parking lot)
void sendImageToServer(string fileName, string fileNameWExt) {
    CURL *curl = curl_easy_init();
    CURLcode res;
    struct curl_httppost *post= NULL;
    struct curl_httppost *last= NULL;
    
    if(curl) {
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, (const char *)fileNameWExt.c_str(), CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "name", CURLFORM_COPYCONTENTS, (const char *)fileName.c_str(), CURLFORM_END);
        
        curl_easy_setopt(curl, CURLOPT_URL, PADSettings::instance().getImageUploadURL().c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
        
        res = curl_easy_perform(curl);
        curl_formfree(post);
    }
    
    curl_easy_cleanup(curl);
}