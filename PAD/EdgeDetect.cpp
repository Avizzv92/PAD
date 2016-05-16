//
//  EdgeDetect.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 20/03/2016.
//

#include "EdgeDetect.hpp"
#include <opencv2/imgproc/imgproc.hpp>

int const EdgeDetect::scale = 1;
int const EdgeDetect::delta = 0;
int const EdgeDetect::ddepth = CV_16S;

//Sobel Edge Detection (unused)
void EdgeDetect::sobelEdgeDetect (Mat &image) {
    Mat src_gray;
        
    //Sobel Edge
    GaussianBlur( image, image, Size(3,3), 0, 0, BORDER_DEFAULT );
        
    // Convert it to gray
    cvtColor( image, src_gray, CV_BGR2GRAY );
    
    // Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    
    // Gradient X
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    
    // Gradient Y
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
        
    // Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, image );
}

//Canny Edge Detection
void EdgeDetect::cannyEdgeDetect (Mat &image) {
    Mat edge;
    
    //Equalize histogram, could be something else to test?
    equalizeHistogram(image);
    
    //Keep/Remove (Needs Further Testing)
    //GaussianBlur( image, image, Size(3,3), 0, 0, BORDER_DEFAULT );

    //Using OpenCV's threshold method with CV_THRESH_BINARY
    //and CV_THRESH_OTSU parameters for determining automatically the
    //best thresholds for canny edge detection.
    Mat imageInGray, _garbageIMG;
    cvtColor( image, imageInGray, CV_BGR2GRAY );
    double otsuThreshold = threshold(imageInGray, _garbageIMG, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    double thresholdHigh = otsuThreshold;
    double thresholdLow = otsuThreshold * 0.5;
    
    Canny( image, edge, thresholdLow, thresholdHigh);
    edge.convertTo(image, CV_8U);
}

void EdgeDetect::equalizeHistogram(Mat &image) {
    cvtColor(image,image,CV_BGR2YCrCb);
        
    vector<Mat> channels;
    split(image,channels);
    equalizeHist(channels[0], channels[0]);
    merge(channels,image);
        
    cvtColor(image,image,CV_YCrCb2BGR);
}