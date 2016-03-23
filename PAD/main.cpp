//
//  main.cpp
//  PAD
//
//  Created by Aaron Vizzini on 20/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "EdgeDetect.hpp"
#include "ROIUtils.hpp"

using namespace std;
using namespace cv;

Mat videoFrame;
Mat roisOverlay;
vector<roi> rois;

int clicks = 0;
Point one, two, three, four;

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
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
            
            roi test;
            test.a = one;
            test.b = two;
            test.c = three;
            test.d = four;
            rois.push_back(test);
        
            ROIUtils::drawROIsOnImage(rois, roisOverlay);
            ROIUtils::getROIMats(rois, videoFrame);
            clicks = 0;
        }
    }
}

int main(int argc, const char * argv[]) {

    VideoCapture cap;
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.open(0);
    
    namedWindow("window",1);
    
    setMouseCallback("window", CallBackFunc, NULL);

    cap>>videoFrame;
    
    //Give this Mat an intial size equal to that of the video frame.
    roisOverlay = Mat::zeros( videoFrame.size(), CV_8UC3 );
    
    while(1) {
        try {
            cap>>videoFrame;
            
            //Canny Edge
            //EdgeDetect::cannyEdgeDetect(image);
            
            //Sobel Edge
            EdgeDetect::sobelEdgeDetect(videoFrame);
  
            //Convert back to RGB to display the overlay indicators for ROIs
            cvtColor( videoFrame, videoFrame, CV_GRAY2RGB );
            videoFrame += roisOverlay;
        
            imshow("window", videoFrame);
            
        } catch (Exception& e) {
            const char* err_msg = e.what();
            std::cout << "Exception caught : imshow:\n" << err_msg << std::endl;
        }
        
        waitKey(33);
    }
    
    return 0;
}
