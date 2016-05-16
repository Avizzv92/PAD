//
//  DBManager.hpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 25/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#ifndef DBMANAGER_hpp
#define DBMANAGER_hpp

#include <mysql.h>
#include "ROIUtils.hpp"

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
    vector<ROI> getROIs(int parkingLotID);
    
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
    void deleteROI(int id, int parkingLotID);
    
    /*
     For logging, we need to make sure the supplied pKey is correct
     */
    bool isValid(string pKey, int parking_lot_id);
};
#endif
