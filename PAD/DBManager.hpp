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

#define SERVER "localhost"
#define USERNAME "root"
#define PASSWORD "root"
#define DB "pad"
#define PORT 8889

using namespace std;

class DBManager {
private:
    MYSQL *conn;
    MYSQL_RES* performQuery(MYSQL *connection, char *sql_query);
    
public:
    /*
        Constructor and deconstructor
     */
    DBManager(void);
    ~DBManager(void);
    
    /*
        Returns a vector of ROIs from the DB
     */
    vector<ROI> getROIs();
    
    /*
        For a given camera ID, log all of the ROIs' information into the log table
     */
    void logOccupancy(int cameraID, vector<ROI> rois);
    
    /*
        Inserts a new ROI into the database for a given parkingLotID (the parking lot it belongs to)
        The DB id is then assigned back to the newROI which until this point has no id
     */
    void insertROI(ROI &newROI);
    
   /*
        Delete a ROI by its id, from the DB
    */
    void deleteROI(int id);
    
    /*
     For logging, we need to make sure the supplied pKey is correct
     */
    bool isValid(string pKey, int parking_lot_id);
};