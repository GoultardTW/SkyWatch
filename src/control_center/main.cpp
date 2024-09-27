#include <hiredis/hiredis.h>
#include <iostream>
#include <postgresql/libpq-fe.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <unistd.h>

#include "control_center.hpp"
#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"


int main(int argc, char *argv[]) {

    // Allocation of redis context
    redisContext *c = connectToRedis("redis", 6379);

    // Send number of drones to Drone
    int nDrones = 5;
    SendStreamMsg(c, "Commands", std::to_string(nDrones).c_str());

    sleep(10);
    // Send a message for each drone
    for(int i=0; i<nDrones; i++){
        std::string message = "hello_from_drone_" + std::to_string(i) + "\n";
        SendStreamMsg(c, "Commands", message.c_str());
    }

    redisFree(c);
    return 0;

    // Instantiation of Control_Center
    Control_Center center;
    // Insertion of the control center into the database
    std::string query = "INSERT INTO controlCenter DEFAULT VALUES";
    center.executeQuery(query);

    // Insertion of drones into the database
    std::cout << "Loading drones into the database...\n";
    for (int i=0; i<nDrones; i++) {
        query = "INSERT INTO drone (controlCenter, batteryPercentage, status) VALUES (1, 100, 'READY')";
        center.executeQuery(query);
        query = std::string("SELECT * FROM drone WHERE id = ") + std::to_string(i+1);
        PGresult *res = center.getTuples(query);
        std::cout << PQgetvalue(res, 0, 0);
    }

    redisFree(c);
    return 0;
}