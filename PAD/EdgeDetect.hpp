//
//  EdgeDetect.hpp
//  PAD
//
//  Created by Aaron Vizzini and WU Weibo on 20/03/2016.
//

#ifndef EdgeDetect_hpp

#define EdgeDetect_hpp

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class EdgeDetect {
private:
    static int scale;
    static int delta;
    static int ddepth;
    
public:
    static Mat sobelEdgeDetect (Mat image);
    static Mat cannyEdgeDetect (Mat image);
};

#endif
