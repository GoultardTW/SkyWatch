#include <hiredis/hiredis.h>
#include <postgresql/libpq-fe.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <unistd.h>
#include <vector>
#include <thread>

#include "control_center.cpp"

int main(int argc, char *argv[]) {

    // Timer Thread
    std::thread timerThread(timer);
    timerThread.detach();

    // Instantiation of Control_Center
    Control_Center center;

    // Allocation of redis context
    redisContext *c = connectToRedis("redis", 6379);
    createGroup(c, "Commands", "Group", true);

    // Produce paths
    std::vector<std::string> paths;
    findPaths(&paths, &center);

    // Send number of paths to Drone
    int nDrones = paths.size();
    SendStreamMsg(c, "Commands", std::to_string(nDrones).c_str());

    // Send a message containing a path for each drone
    for(int i=0; i<nDrones; i++){
        std::string message = paths[i];
        std::string stream = "Commands"+std::to_string(i);
        std::string group = "Group"+std::to_string(i);
        createGroup(c, stream, group, true);
        SendStreamMsg(c, stream.c_str(), message.c_str());
    }
    
    // Thread to read the report of drones
    std::thread listenThread(listenDrones, &center, nDrones);
    listenThread.detach();

    // At the end of the execution, Get the actual number of drones initialized
    std::string finalS = "Drones";
    createGroup(c, finalS, finalS, true);
    std::string res = ReadGroupMsgVal(c, 0, finalS.c_str(), finalS.c_str());
    destroyGroup(c, finalS.c_str(), finalS.c_str());
    deleteStream(c, finalS.c_str());
    nDrones = std::stoi(res);
    for (int i=0; i<nDrones; i++) {
        std::string query = "INSERT INTO drone (id_cdc, battery) VALUES ("+std::to_string(center.getCCId())+", 100)";
        center.executeQuery(query);
    }

    // MONITOR NUM DRONES
    if(nDrones>9000){
        std::string query = "INSERT INTO monitor_failure (session_, failure, message_, date_time) VALUES ("+ std::to_string(center.getSessionId())+", 'NUM_DRONES', 'The number of drones is over 9000: " + std::to_string(nDrones) + "', NOW());";
        center.executeQuery(query);
    }

    redisFree(c);
    return 0;
}