//
//  EdgeDetect.hpp
//  PAD
//
//  Created by Aaron Vizzini and WU Weibo on 20/03/2016.
//

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class EdgeDetect {
private:
    static int const scale;
    static int const delta;
    static int const ddepth;
    
public:
    static void sobelEdgeDetect (Mat &image);
    static void cannyEdgeDetect (Mat &image);
};