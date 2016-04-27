//
//  DBManager.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 23/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include "DBManager.hpp"

u_int totalROIs = 0;

DBManager::DBManager(void)
{
    conn = mysql_init(NULL);
    mysql_autocommit(conn, true);

    if (!mysql_real_connect(conn, SERVER, USERNAME, PASSWORD, DB, PORT, NULL, 0)) {
            printf("Conection error : %s\n", mysql_error(conn));
            printf("Verify connection settings and internet/server access, then restart the program.\n");
            exit(1);
    }
}

MYSQL_RES* DBManager::performQuery(MYSQL *connection, char *sql_query)
{
    // send the query to the database
    if (mysql_query(connection, sql_query))
    {
        printf("MySQL ERROR : %s\n", mysql_error(connection));
    }
    
    return mysql_use_result(connection);
}

vector<ROI> DBManager::getROIs() {
    vector<ROI> rois;
    
    MYSQL_RES *results;
    MYSQL_ROW row;
    
    results = performQuery(conn, (char *)"SELECT id, parking_lot_id, X(pointA), Y(pointA), X(pointB), Y(pointB), X(pointC), Y(pointC), X(pointD), Y(pointD), isOccupied, description, threshold FROM PARKING_SPOT;");
    
    while ((row = mysql_fetch_row(results)) != NULL) {
        ROI newROI;
        newROI.id = atoi(row[0]);
        newROI.parking_lot_id = atoi(row[1]);
        newROI.a = Point(atoi(row[2]), atoi(row[3]));
        newROI.b = Point(atoi(row[4]), atoi(row[5]));
        newROI.c = Point(atoi(row[6]), atoi(row[7]));
        newROI.d = Point(atoi(row[8]), atoi(row[9]));
        newROI.setOccupied(atoi(row[10]));
        newROI.description = row[11];
        newROI.threshold = atof(row[12]);
        
        vector<Point> contour;
        contour.push_back(newROI.a);
        contour.push_back(newROI.b);
        contour.push_back(newROI.c);
        contour.push_back(newROI.d);
        newROI.contour = contour;
        
        rois.push_back(newROI);
    }
    
    mysql_free_result(results);
    
    totalROIs = (u_int)rois.size();
    
    return rois;
}

void DBManager::logOccupancy(int cameraID, vector<ROI> rois) {
    
    for(int i = 0; i < rois.size(); i++) {
        ROI curr = rois[i];
        string insert = "INSERT INTO OCCUPANCY_LOG (camera_id, parking_spot_id, isOccupied, parking_lot_id, parking_spot_desc) VALUES (" + to_string(cameraID) + " , " + to_string(curr.id) + "," + to_string(curr.getOccupied() == true ? 1 : 0) + "," + to_string(curr.parking_lot_id) + ",'" + curr.description + "') ;";
        MYSQL_RES *results = performQuery(conn, (char *)insert.c_str());
        mysql_free_result(results);
    }
}

void DBManager::insertROI(ROI &newROI) {
    if(totalROIs < 999 ) {
        string description = to_string(totalROIs+1);
        string insert = "INSERT INTO PARKING_SPOT (parking_lot_id, pointA, pointB, pointC, pointD, isOccupied, description, threshold) VALUES ( " + to_string(newROI.parking_lot_id) + ", PointFromText('POINT("+to_string(newROI.a.x)+" "+to_string(newROI.a.y)+")'), PointFromText('POINT("+to_string(newROI.b.x)+" "+to_string(newROI.b.y)+")'), PointFromText('POINT("+to_string(newROI.c.x)+" "+to_string(newROI.c.y)+")'), PointFromText('POINT("+to_string(newROI.d.x)+" "+to_string(newROI.d.y)+")'), "+to_string(0)+", "+ description +", " + to_string(newROI.threshold) + " );";

        MYSQL_RES *results = performQuery(conn, (char *)insert.c_str());
        mysql_free_result(results);
    
        MYSQL_RES *results2 = performQuery(conn, (char *)"SELECT LAST_INSERT_ID();");
        MYSQL_ROW row = mysql_fetch_row(results2);
        newROI.id = atoi(row[0]);
        newROI.description = description;
        mysql_free_result(results2);
        totalROIs++;
    } else {
        printf("You can't have more than 999 spots");
    }
}

void DBManager::deleteROI(int id, int parkingLotID) {
    string deleteQuery = "DELETE FROM PARKING_SPOT WHERE id = " + to_string(id) + " AND parking_lot_id = " + to_string(parkingLotID);
    MYSQL_RES *results = performQuery(conn, (char *)deleteQuery.c_str());
    mysql_free_result(results);
    //Remove this ROI's logs too.
    string deleteQuery2 = "DELETE FROM OCCUPANCY_LOG WHERE parking_spot_id = " + to_string(id);
    MYSQL_RES *results2 = performQuery(conn, (char *)deleteQuery2.c_str());
    mysql_free_result(results2);
    totalROIs--;
}

bool DBManager::isValid(string pKey, int parking_lot_id) {
    MYSQL_RES *results;
    MYSQL_ROW row;
    
    string qry = "SELECT * FROM parking_lot WHERE pKey = \"" + pKey + "\" AND id = \"" + to_string(parking_lot_id) + "\"";
    results = performQuery(conn, (char *)qry.c_str());

    while ((row = mysql_fetch_row(results)) != NULL) {
        mysql_free_result(results);
        return true;
    }
    
    mysql_free_result(results);
    return false;
}

DBManager::~DBManager(void)
{
    mysql_close(conn);
}