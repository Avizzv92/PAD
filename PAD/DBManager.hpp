//
//  DBManager.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 23/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "ROIUtils.hpp"

using namespace std;

class DBManager {
private:
    MYSQL *conn;
    MYSQL_RES* performQuery(MYSQL *connection, char *sql_query);
    const char *SERVER = "localhost";
    const char *USERNAME = "root";
    const char *PASSWORD = "root";
    const char *DB = "pad";
    const int PORT = 8889;
    
public:
    /*
        Constructor and deconstructor
     */
    DBManager(void);
    ~DBManager(void);
    
    /*
        Returns a vector of ROIs from the DB
     */
    vector<roi> getROIs();
    
    /*
        For a given camera ID, log all of the ROIs' information into the log table
     */
    void logOccupancy(int cameraID, vector<roi> rois);
    
    /*
        Inserts a new ROI into the database for a given parkingLotID (the parking lot it belongs to)
        The DB id is then assigned back to the newROI which until this point has no id
     */
    void insertROI(roi &newROI);
    
   /*
        Delete a ROI by its id, from the DB
    */
    void deleteROI(int id);
    
    /*
     For logging, we need to make sure the supplied pKey is correct
     */
    bool isValid(string pKey);
};