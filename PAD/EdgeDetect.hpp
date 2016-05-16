//
//  EdgeDetect.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 20/03/2016.
//

#ifndef EDGEDETECT_hpp
#define EDGEDETECT_hpp

#include <opencv/cv.h>

using namespace cv;
using namespace std;

class EdgeDetect {
private:
    static int const scale;
    static int const delta;
    static int const ddepth;
    static void equalizeHistogram(Mat &image);
    
public:
    //Unused
    static void sobelEdgeDetect (Mat &image);
    //Canny Edge Detection for occupancy detection
    static void cannyEdgeDetect (Mat &image);
};

#endif