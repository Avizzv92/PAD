//
//  EdgeDetect.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 20/03/2016.
//

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class EdgeDetect {
private:
    static int const scale;
    static int const delta;
    static int const ddepth;
    static void equalizeHistogram(Mat &image);
    
public:
    static void sobelEdgeDetect (Mat &image);
    static void cannyEdgeDetect (Mat &image);
};