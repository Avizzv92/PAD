//
//  MotionDetect.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 16/05/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#ifndef MotionDetect_hpp
#define MotionDetect_hpp

#include <opencv/cv.h>
#include "ROIUtils.hpp"

using namespace std;
using namespace cv;

class MotionDetect {
public:
    static Mat detectMotion(Mat originalFrame, vector<ROI> &rois);
};
#endif /* MotionDetect_hpp */
