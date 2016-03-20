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

using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    
    Mat image;
    VideoCapture cap;
    
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.open(0);
    
    namedWindow("window",1);
    
    while(1) {
        try {
            cap>>image;
            
            //Canny Edge
            //Mat edged = EdgeDetect::cannyEdgeDetect(image);
            
            //Sobel Edge
            Mat edged = EdgeDetect::sobelEdgeDetect(image);
            
            imshow("window",edged);
        } catch (Exception& e) {
            const char* err_msg = e.what();
            std::cout << "Exception caught : imshow:\n" << err_msg << std::endl;
        }
        
        waitKey(33);
    }
    
    return 0;
}
