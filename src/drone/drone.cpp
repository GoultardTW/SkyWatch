#include <stdlib.h>
#include <math.h>
#include <mutex>
#include <thread>
#include <chrono>

#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"
#include "drone.hpp"

// Number of instaces of Drone
int Drone::instance_count = 0;
std::mutex Drone::count_mutex;

// Shared Vector of ready drones
std::vector<Drone> readyOnes;
std::mutex ready_mutex;

// Shared Vector of threads
std::vector<std::thread> threads;
std::mutex threads_mutex;

// Adds a drone to the ready ones
void add_to_ready(Drone drone) {
    // Lock Mutex
    std::lock_guard<std::mutex> guard(ready_mutex);

    // Secure access to the vector
    readyOnes.push_back(drone);
    // Lock released automatically
}

Drone getDrone(){
    // Lock Mutex
    std::lock_guard<std::mutex> guard(ready_mutex);

    if(!readyOnes.empty()){
        Drone res = readyOnes.back();
        readyOnes.pop_back();
        return res;
    }else{
        Drone newdrone;
        return newdrone;
    }
    
}

// It manages one drone (is used inside n threads)
void initDrone(std::string msg){
    Drone drone = getDrone();
    int id = drone.getId();
    if (msg.length()==0){
        redisC:
        redisContext *c = connectToRedis("redis", 6379);
        if(c == nullptr || c->err) goto redisC;
        std::string stream = "Commands"+std::to_string(id);
        std::string group = "Group"+std::to_string(id);
        createGroup(c, stream, group, true);
        msg = ReadGroupMsgVal(c, id, group.c_str(), stream.c_str());
        destroyGroup(c, stream, group);
        deleteStream(c, stream);
        redisFree(c);
    }
    for(int m=0; m<msg.length(); m++){
        if (m == 124){
            std::lock_guard<std::mutex> guard(threads_mutex);
            threads.emplace_back(initDrone, msg);
        }
        drone.ExecuteMove(msg[m]);
        printf("Il drone #%d si trova nella posizione (%d, %d)\n", id, drone.getX(), drone.getY());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    //printf("Il drone #%d si trova nella posizione (%d, %d)\n", id, drone.getX(), drone.getY());
    drone.chargeDrone();
    add_to_ready(drone);
}


