#include <stdlib.h>
#include <math.h>

#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"
#include "drone.hpp"

void initDrone(redisContext* c, int id){
    Drone drone(id);
    std::string msg = ReadGroupMsgVal(c, id, "DroneGroup", "Commands");
    printf("%s", msg.c_str());
}