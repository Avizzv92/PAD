//
//  Logger.cpp
//  PAD
//
//  Created by Aaron Vizzini on 16/05/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include "Logger.hpp"
#include "PADSettings.hpp"
#include <ctime>
#include <thread>
#include <curl/curl.h>
#include <opencv2/highgui/highgui.hpp>

time_t lastLongTime = time(0);

void Logger::handleLogging(DBManager &dbm, Mat matToLog, int CAMERA_ID, int PARKING_LOT_ID, vector<ROI> rois) {
    
    time_t now = time(0);
    
    if((now - lastLongTime) >= PADSettings::instance().getLogTime()) {
        lastLongTime = now;
        
        dbm.logOccupancy(CAMERA_ID, rois);
        
        string fileName = "logImg_"+to_string(PARKING_LOT_ID);
        string fileNameWExt = "logImg_"+to_string(PARKING_LOT_ID)+".png";
        
        imwrite(fileNameWExt, matToLog);
        
        if(PADSettings::instance().getIsTesting()) {
            string sampleFileNameWExt = PADSettings::instance().getLoggingImgDir()+"sampleImg_"+to_string(now)+".png";
            imwrite(sampleFileNameWExt, matToLog);
        }
        
        thread uploadThread(sendImageToServer, fileName, fileNameWExt);
        uploadThread.join();
    }
}

void Logger::sendImageToServer(string fileName, string fileNameWExt) {
    CURL *curl = curl_easy_init();
    CURLcode res;
    struct curl_httppost *post= NULL;
    struct curl_httppost *last= NULL;
    
    if(curl) {
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, (const char *)fileNameWExt.c_str(), CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "name", CURLFORM_COPYCONTENTS, (const char *)fileName.c_str(), CURLFORM_END);
        
        curl_easy_setopt(curl, CURLOPT_URL, PADSettings::instance().getImageUploadURL().c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
        
        res = curl_easy_perform(curl);
        curl_formfree(post);
    }
    
    curl_easy_cleanup(curl);
}