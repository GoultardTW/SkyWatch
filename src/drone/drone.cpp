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
std::vector<Drone> Drone::readyOnes;
std::mutex Drone::ready_mutex;

// Adds a drone to the ready ones
void add_to_ready(Drone drone) {
    // Lock Mutex
    std::lock_guard<std::mutex> guard(Drone::ready_mutex);

        // Secure access to the vector
    Drone::readyOnes.emplace_back(drone);
    printf("Il drone #%d e' messo nei ready\n",Drone::readyOnes.back().getId());

    // Lock released automatically
}

void chargeDrone(Drone drone){
    float temp = (float)drone.getMovesLeft()/MAX_FLIGHT_MOVES;
    float missing_charge = 1-temp;
    int time_needed = missing_charge * drone.getRecharginTime() * 1000;
    std::this_thread::sleep_for(std::chrono::milliseconds(time_needed));
    drone.fillRechargingTime();
    add_to_ready(drone);
}

Drone getDrone(){
    // Lock Mutex
    std::lock_guard<std::mutex> guard(Drone::ready_mutex);

    if(!Drone::readyOnes.empty()){
        Drone res = Drone::readyOnes.back();
        printf("Il drone #%d e' appena stato pescato\n", res.getId());
        Drone::readyOnes.pop_back();
        return res;
    }else{
        Drone newdrone;
        printf("Il drone #%d e' appena stato creato\n", newdrone.getId());
        return newdrone;
    }
    
}

// It manages one path
void initDroneX(){

    std::vector<Drone> drones;
    std::vector<int> ind;
    ind.emplace_back(0);
    drones.emplace_back(getDrone());
    int id = drones.back().getId();
    redisContext *c = connectToRedis("redis", 6379);
    std::string stream = "Commands"+std::to_string(id);
    std::string group = "Group"+std::to_string(id);
    createGroup(c, stream, group, true);
    std::string msg = ReadGroupMsgVal(c, id, group.c_str(), stream.c_str());
    destroyGroup(c, stream, group);
    deleteStream(c, stream);
    redisFree(c);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    int temp = -1;
    while(true){
        for(int i=0; i<drones.size(); i++){
            if(i==temp){
                temp = -1;
                continue;
            }
            drones[i].ExecuteMove(msg[ind[i]]);
            ind[i]+=1;
            if(ind[i]==124){
                drones.emplace_back(getDrone());
                ind.emplace_back(0);
                temp = drones.size();
            }
            if(ind[i]>=msg.length()){
                std::thread chargingThread(chargeDrone, drones[i]);
                chargingThread.detach();
                drones.erase(drones.begin() + i);
                ind.erase(ind.begin()+i);
                i--;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


