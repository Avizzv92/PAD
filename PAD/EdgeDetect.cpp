//
//  EdgeDetect.cpp
//  PAD
//
//  Created by Aaron Vizzini and WU Weibo on 20/03/2016.
//

#include "EdgeDetect.hpp"

int EdgeDetect::scale = 1;
int EdgeDetect::delta = 0;
int EdgeDetect::ddepth = CV_16S;

//Sobel Edge Detection
Mat EdgeDetect::sobelEdgeDetect (Mat image) {
    
    Mat src_gray, grad;
        
    //Sobel Edge
    GaussianBlur( image, image, Size(3,3), 0, 0, BORDER_DEFAULT );
        
    /// Convert it to gray
    cvtColor( image, src_gray, CV_BGR2GRAY );
    
    /// Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    
    /// Gradient X
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    
    /// Gradient Y
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
        
    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
        
    return grad;
}

//Canny Edge Detection
Mat EdgeDetect::cannyEdgeDetect (Mat image) {
    Mat edge, draw;
    Canny( image, edge, 50, 150, 3);
    edge.convertTo(draw, CV_8U);
    
    return draw;
}