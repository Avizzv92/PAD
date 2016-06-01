//
//  Settings.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 14/05/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include "PADSettings.hpp"

PADSettings::PADSettings(void) {
    
    XMLError e = doc.LoadFile( "settings.xml" );
    if(e != 0) {
        cout<< "Could not load the settings file. Check that the settings.xml file is located in the same directory as the executable." <<endl;
        exit(1);
    }
    
}

PADSettings& PADSettings::instance()
{
    static PADSettings instance;
    return instance;
}

int PADSettings::getCameraId() {
    return stoi(doc.FirstChildElement( "settings" )->FirstChildElement( "camera_id" )->GetText());
}

int PADSettings::getParkingLotId() {
    return stoi(doc.FirstChildElement( "settings" )->FirstChildElement( "parking_lot_id" )->GetText());
}

string PADSettings::getPrivateKey() {
    return doc.FirstChildElement( "settings" )->FirstChildElement( "private_key" )->GetText();
}

double PADSettings::getRedPixelThreshold() {
    return stod(doc.FirstChildElement( "settings" )->FirstChildElement( "red_pixel_threshold" )->GetText());
}

double PADSettings::getWhitePixelThreshold() {
    return stod(doc.FirstChildElement( "settings" )->FirstChildElement( "white_pixel_threshold" )->GetText());
}

string PADSettings::getServer() {
    return doc.FirstChildElement( "settings" )->FirstChildElement( "server" )->GetText();
}

string PADSettings::getUsername() {
    return doc.FirstChildElement( "settings" )->FirstChildElement( "username" )->GetText();
}

string PADSettings::getPassword() {
    return doc.FirstChildElement( "settings" )->FirstChildElement( "password" )->GetText();
}

string PADSettings::getDB() {
    return doc.FirstChildElement( "settings" )->FirstChildElement( "db" )->GetText();
}

int PADSettings::getPort() {
    return stoi(doc.FirstChildElement( "settings" )->FirstChildElement( "port" )->GetText());
}

bool PADSettings::getIsTesting() {
    return stoi(doc.FirstChildElement( "settings" )->FirstChildElement( "is_testing" )->GetText()) == 1 ? true : false;
}

int PADSettings::getFrameDelay() {
    return stoi(doc.FirstChildElement( "settings" )->FirstChildElement( "frame_delay" )->GetText());
}

int PADSettings::getLogTime() {
    return stoi(doc.FirstChildElement( "settings" )->FirstChildElement( "log_time" )->GetText());
}

string PADSettings::getImageUploadURL() {
    return doc.FirstChildElement( "settings" )->FirstChildElement( "image_upload_url" )->GetText();
}

int PADSettings::getVideoCaptureSource() {
    return stoi(doc.FirstChildElement( "settings" )->FirstChildElement( "video_capture_source" )->GetText());
}

string PADSettings::getLoggingImgDir() {
    return doc.FirstChildElement( "settings" )->FirstChildElement( "logging_img_dir" )->GetText();
}