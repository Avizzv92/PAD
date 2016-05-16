//
//  Logger.hpp
//  PAD
//
//  Created by Aaron Vizzini on 16/05/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#ifndef Logger_hpp
#define Logger_hpp

#include <opencv/cv.h>
#include "DBManager.hpp"

using namespace cv;
using namespace std;

class Logger {
public:
    //Log a single frame along with all of the ROIs to the DB.
    static void handleLogging(DBManager &dbm, Mat matToLog, int CAMERA_ID, int PARKING_LOT_ID, vector<ROI> rois);
private:
    //Send a single frame to the server for logging purposes (only one ever exists at a single time for a single parking lot)
    static void sendImageToServer(string fileName, string fileNameWExt);
};

#endif /* Logger_hpp */
