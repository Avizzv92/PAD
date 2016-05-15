//
//  Settings.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 14/05/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#ifndef PADSETTINGS_H
#define PADSETTINGS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

class PADSettings {

private:
    PADSettings();
    
public:
    static PADSettings& instance();
    
    XMLDocument doc;

    void load();
    
    int getCameraId();
    int getParkingLotId();
    string getPrivateKey();
        
    double getRedPixelThreshold();
    double getWhitePixelThreshold();
    
    string getServer();
    string getUsername();
    string getPassword();
    string getDB();
    int getPort();
    
    bool getIsTesting();
    
    int getFrameDelay();
    int getLogTime();
    
    string getImageUploadURL();
    
    int getVideoCaptureSource();
    
    string getLoggingImgDir();
};

#endif