#include <stdlib.h>
#include <math.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

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

    for(int m=0; m<msg.length(); m++){
        drone.ExecuteMove(msg[m]);
        std::this_thread::sleep_for(std::chrono::milliseconds(2400));
    }
    printf("Il drone #%d si trova nella posizione (%d, %d)\n", id, drone.getX(), drone.getY());
}
