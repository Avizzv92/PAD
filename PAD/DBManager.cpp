//
//  DBManager.cpp
//  PAD
//
//  Created by Aaron Vizzini & Wu Weibo on 23/03/2016.
//  Copyright © 2016 ESIGELEC-IS. All rights reserved.
//

#include "DBManager.hpp"

DBManager::DBManager(void)
{
    conn = mysql_init(NULL);
    mysql_autocommit(conn, true);

    if (!mysql_real_connect(conn, SERVER, USERNAME, PASSWORD, DB, PORT, NULL, 0)) {
            printf("Conection error : %s\n", mysql_error(conn));
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

vector<roi> DBManager::getROIs() {
    vector<roi> rois;
    
    MYSQL_RES *results;
    MYSQL_ROW row;
    
    results = performQuery(conn, (char *)"SELECT id, parking_lot_id, X(pointA), Y(pointA), X(pointB), Y(pointB), X(pointC), Y(pointC), X(pointD), Y(pointD), isOccupied, description, threshold FROM PARKING_SPOT;");
    
    while ((row = mysql_fetch_row(results)) != NULL) {
        roi newROI;
        newROI.id = atoi(row[0]);
        newROI.parking_lot_id = atoi(row[1]);
        newROI.a = Point(atoi(row[2]), atoi(row[3]));
        newROI.b = Point(atoi(row[4]), atoi(row[5]));
        newROI.c = Point(atoi(row[6]), atoi(row[7]));
        newROI.d = Point(atoi(row[8]), atoi(row[9]));
        newROI.occupied = atoi(row[10]);
        newROI.description = row[11];
        newROI.whitePixelCount = atoi(row[12]);
                
        rois.push_back(newROI);
    }
    
    mysql_free_result(results);
    
    return rois;
}

void DBManager::logOccupancy(int cameraID, vector<roi> rois) {
    
    for(int i = 0; i < rois.size(); i++) {
        roi curr = rois[i];
        string insert = "INSERT INTO OCCUPANCY_LOG (camera_id, parking_spot_id, isOccupied) VALUES (" + to_string(cameraID) + " , " + to_string(curr.id) + "," + to_string(curr.occupied == true ? 1 : 0) + ") ;";
    
        MYSQL_RES *results = performQuery(conn, (char *)insert.c_str());
        mysql_free_result(results);
    }
}

void DBManager::insertROI(roi &newROI, int parkingLotID) {
    string description = newROI.description.length() == 0 ? "''" : newROI.description;
    string insert = "INSERT INTO PARKING_SPOT (parking_lot_id, pointA, pointB, pointC, pointD, isOccupied, description, threshold) VALUES ( " + to_string(parkingLotID) + ", PointFromText('POINT("+to_string(newROI.a.x)+" "+to_string(newROI.a.y)+")'), PointFromText('POINT("+to_string(newROI.b.x)+" "+to_string(newROI.b.y)+")'), PointFromText('POINT("+to_string(newROI.c.x)+" "+to_string(newROI.c.y)+")'), PointFromText('POINT("+to_string(newROI.d.x)+" "+to_string(newROI.d.y)+")'), "+to_string(newROI.occupied == true ? 1 : 0)+", "+ description +", " + to_string(newROI.whitePixelCount) + " );";

    MYSQL_RES *results = performQuery(conn, (char *)insert.c_str());
    mysql_free_result(results);
    
    MYSQL_RES *results2 = performQuery(conn, (char *)"SELECT LAST_INSERT_ID();");
    MYSQL_ROW row = mysql_fetch_row(results2);
    newROI.id = atoi(row[0]);
    printf("Inserting %i \n",newROI.id );
    mysql_free_result(results2);
}

void DBManager::deleteROI(int id) {
    string deleteQuery = "DELETE FROM PARKING_SPOT WHERE id = " + to_string(id);
    MYSQL_RES *results = performQuery(conn, (char *)deleteQuery.c_str());
    printf("Deleting %i \n",id);
    mysql_free_result(results);
}

DBManager::~DBManager(void)
{
    mysql_close(conn);
}