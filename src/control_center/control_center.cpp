#include <chrono>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <string> 
#include <thread>

#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"
#include "control_center.hpp"

bool stopflag = false; // Flag to stop to receive reports 

// Returns a path from (x, y) -> (nx, ny)
std::string goTo(int x, int y, int nx, int ny){
    std::string res = "";
    if (x > nx) {
        res = res + std::string((x-nx), 'l');
    }else if ( x < nx) {
        res = res + std::string((nx-x), 'r');
    }
    if (y > ny) {
        res = res + std::string((y-ny), 'u');
    }else if ( y < ny) {
        res = res + std::string((ny-y), 'd');
    }
    return res;
}

// Returns a path that passes through the rows
std::string goToGridX(int* x, int* y, int step){
    std::string res = "";
    int ax = *x, ay = *y;
    int temp = 0;
    bool goRight = false;

    while(step>0){
        goRight = (ay%2==0); // If it's even go to the right
        temp = (goRight) ? (299 - ax) : ax; // Steps to reach the border
        temp = std::min(temp, step); // Min between steps left and temp

        for (int k=0; k<temp; k++){
            if(step <= abs(ax-150)+abs(ay-150)){
                res = res + goTo(ax, ay, 150, 150);
                step = 0;
                break;
            }
            if(goRight){
                res = res + "r";
                ax ++;
            }else{
                res = res + "l";
                ax --;
            }
            step -= 1;
        }

        if(step>0 && ay<299){
            if(step <= abs(ax-150)+abs(ay-150)){
                res = res + goTo(ax, ay, 150, 150);
                step = 0;
                break;
            }
            res += "d"; 
            step -= 1; 
            ay += 1; 
        }
    }

    goRight = (ay%2==0);
    if(ax < 299 && goRight){
        ax++;
    }else if (ax > 0 && !goRight){
        ax--;
    }else if (((ax>=299) && goRight && (ay<299)) || ((ax<=0) && !goRight && (ay<299))){
        ay++;
    }

    *x = ax;
    *y = ay;
    return res;
}

// It determines a vector of paths
void findPaths(std::vector<std::string>* paths, Control_Center* cc) {
    int x = 0, y = 0;
    int step;
    std::string path = "";

    while (y < 299) {
        step = 750;
        path = goTo(150, 150, x, y);
        step -= path.length();
        path = path + goToGridX(&x, &y, step);
        paths->emplace_back(path);
    }

    if (x > 0) {
        path = goTo(150, 150, x, y);
        path = path + goTo(x, y, 0, 299);
        path = path + goTo(0, 299, 150, 150);
        paths->emplace_back(path);
    }
    // MONITOR PATH_CALCULATION
    if(paths->size()<226){
        printf("Calculation of paths failed: An insert monitor_failure query has been sent to the Database.\n");
        std::string query = "INSERT INTO monitor_failure (session_, failure, message_, date_time) VALUES ("+ std::to_string(cc->getSessionId())+", 'PATH_CALCULATION', 'Path vector has not been calculated correctly...', NOW());";
        cc->executeQuery(query);
    }
}

// Thread to execute a query
void doQuery(Control_Center* c, std::string query){
    c->executeQuery(query);
    //printf("Query executed: %s\n", query.c_str());
}

//Communication with Drones
void listenDronesX(Control_Center* cc, int nDrones){
    // Initiation of connection to Redis
    std::string stream = "Reports";
    redisContext *c = connectToRedis("redis", 6379);
    int i=0;

    while(!stopflag){
        if(i == nDrones) i = 0;

        // Reads report from Drones
        std::string actualStream = stream+std::to_string(i);
        createGroup(c, actualStream, actualStream, true);
        long length = getStreamLen(c, actualStream);

        // MONITOR CC_OVERLOAD
        if (length > 4000) { 
                printf("Stream length is too long: An insert monitor_failure query has been sent to the Database.\n");
                std::string query = "INSERT INTO monitor_failure (session_, failure, message_, date_time) VALUES ("+ std::to_string(cc->getSessionId())+", 'CC_OVERLOAD', 'cc Stream length is too long: " + std::to_string(length) + "', NOW());";
                std::thread newQuery(doQuery, cc, query);
                newQuery.detach();
        }

        // Read the report from the i-th drone
        std::string report = ReadGroupMsgVal(c, i, actualStream.c_str(), actualStream.c_str(), "20000");
        if(report == "Null") continue;
        std::vector<std::string> coordinates = splitMessage(report);

        // MONITOR MISSING_REPORT
        if(coordinates.size() == 0 || coordinates.size()%2 == 1){
            printf("Report empty or not valid: An insert monitor_failure query has been sent to the Database.\n");
            std::string query = "INSERT INTO monitor_failure (session_, failure, message_, date_time) VALUES ("+ std::to_string(cc->getSessionId())+", 'MISSING_REPORT', 'Empty or not valid Report', NOW());";
            std::thread newQuery(doQuery, cc, query);
            newQuery.detach();
        }

        // Updates timestamps of the grid
        int x, y;
        for(int i=0; i<coordinates.size(); i+=2){
            x = std::stoi(coordinates[i]);
            y = std::stoi(coordinates[i+1]);
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-cc->getTimeFromGrid(x, y));
            // MONITOR AREA_COVERAGE
            if(diff.count()>3002000){
                //printf("The point (%d, %d) has not been covered in 5 minutes: An insert monitor_failure query has been sent to the Database.\n", x, y);
                std::string query ="INSERT INTO monitor_failure (session_, failure, message_, date_time) VALUES ("+ std::to_string(cc->getSessionId())+", 'AREA_COVERAGE', 'The point ("+std::to_string(x)+", "+std::to_string(y)+") has not been covered in 5 minutes', NOW());";
                std::thread newQuery(doQuery, cc, query);
                newQuery.detach();
            }
            cc->updateGrid(x, y);
        }
        i++;
    }

    // Destroy all groups and streams at the end
    for(int i=0; i<nDrones; i++){
        destroyGroup(c, stream+std::to_string(i), stream+std::to_string(i));
        deleteStream(c, stream+std::to_string(i));
    }
}

// It checks if every point in the grid is visited (Not actually used, just for debug)
bool finalCheck(Control_Center* c){
    bool check = false;
    for(int i=0; i<300; i++){
        for(int j=0; j<300; j++){
            if(c->getTimeFromGrid(i, j)==std::chrono::time_point<std::chrono::system_clock>()){
                printf("(%d,%d) Not checked\n", i, j);
                check = true;
            }
        }
    }
    return check;
}

// Timer thread to set the stopflag true
void timer(){
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    stopflag = true;
}

// Thread for periodic reports about Control Center execution
void periodicReport(Control_Center* cc){
    int t = 1;
    while(!stopflag){
        std::this_thread::sleep_for(std::chrono::milliseconds(60000)); // 60 seconds normally
        printf("Control Center is now listening: %f%% coverage, %d minutes.\n", cc->computePercentage(), t);
        t+=1;
    }
}


