#include <stdlib.h>
#include <math.h>
#include <mutex>
#include <condition_variable>

#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"
#include "drone.hpp"

std::mutex mtx;
std::condition_variable cv;
bool activated = false;

// It manages one drone (is used inside n threads)
void initDrone(redisContext* c, int id){
    Drone drone(id);
    std::string msg = ReadGroupMsgVal(c, id, "DroneGroup", "Commands");
    printf("%s\n", msg.c_str());
}

void clock_Drones(){
}