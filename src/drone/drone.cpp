#include <stdlib.h>
#include <math.h>
#include <mutex>
#include <string>
#include <thread>
#include <chrono>

#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"
#include "drone.hpp"

bool stopflag = false;

// Number of instaces of Drone
int Drone::instance_count = 0;
std::mutex Drone::count_mutex;

// Shared Vector of ready drones
std::vector<Drone> Drone::readyOnes;
std::mutex Drone::ready_mutex;

// Adds a drone to the ready ones
void add_to_ready(Drone drone) {
    // Lock Mutex
    printf("Drone #%d: Ready to work.\n", drone.getId());
    std::lock_guard<std::mutex> guard(Drone::ready_mutex);

    // Secure access to the vector
    Drone::readyOnes.emplace_back(drone);
    // Lock released automatically
}

// Sets a drone in a charging state
void chargeDrone(Drone drone){
    float temp = (float)drone.getMovesLeft()/MAX_FLIGHT_MOVES;
    float missing_charge = 1-temp;
    int time_needed = missing_charge * drone.getRecharginTime() * 1000;
    std::this_thread::sleep_for(std::chrono::milliseconds(time_needed));
    drone.fillMovesLeft();
    add_to_ready(drone);
}

// Returns a new Drone if there isn't a ready one
Drone getDrone(){
    // Lock Mutex
    std::lock_guard<std::mutex> guard(Drone::ready_mutex);

    if(!Drone::readyOnes.empty()){
        Drone res = Drone::readyOnes.back();
        //printf("The drone #%d has just been fetched\n", res.getId());
        Drone::readyOnes.pop_back();
        return res;
    }else{
        Drone newdrone;
        //printf("The drone #%d has just been created\n", newdrone.getId());
        return newdrone;
    }
    
}

// It manages the coverage of one path
void initDroneX(){

    // Creates the first drone of the path
    std::vector<Drone> drones;
    std::vector<int> ind;
    ind.emplace_back(0);
    drones.emplace_back(getDrone());
    int id = drones.back().getId();

    // Gets a path for the drone through Redis
    redisContext *c = connectToRedis("redis", 6379);
    std::string stream = "Commands"+std::to_string(id);
    std::string group = "Group"+std::to_string(id);
    createGroup(c, stream, group, true);
    std::string msg = ReadGroupMsgVal(c, id, group.c_str(), stream.c_str());
    destroyGroup(c, stream, group);
    deleteStream(c, stream);
    
    // Waits for the others to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    // Starts execution through the path
    int temp = -1;
    std::string report = "";
    while(!stopflag){
        report = "";
        for(int i=0; i<drones.size(); i++){
            // Skip if the drone has just been created
            if(i==temp){
                temp = -1;
                continue;
            }
            // It executes the move and produces the report
            int ox = drones[i].getX(); // Get x at time t
            int oy = drones[i].getY(); // Get y at time t
            drones[i].ExecuteMove(msg[ind[i]]); // Executes move
            // Print the movement at time t+1
            //printf("Drone #%d: (%d, %d) -> (%d, %d)\n", drones[i].getId(), ox, oy, drones[i].getX(), drones[i].getY());
            ind[i]+=1;
            report = report + std::to_string(drones[i].getX())+"/"+std::to_string(drones[i].getY())+"/";

            // A drone calls a new drone to cover it up
            if(ind[i]==124){
                drones.emplace_back(getDrone());
                ind.emplace_back(0);
                temp = drones.size()-1;
            }

            // If the drone reached the end of the path
            if(ind[i]>=msg.length()){
                printf("Drone #%d: Back to CC for recharging.\n", drones[i].getId());
                std::thread chargingThread(chargeDrone, drones[i]);
                chargingThread.detach();
                drones.erase(drones.begin() + i);
                ind.erase(ind.begin()+i);
                i--;
            }
        }

        // It sends the report to the Control Center
        stream = "Reports" + std::to_string(id);
        createGroup(c, stream, stream, true);
        SendStreamMsg(c, stream.c_str(), report.c_str());

        // It simulates the time to reach the next block
        std::this_thread::sleep_for(std::chrono::milliseconds(2400));
    }

    // Elimination of stream, group and redis context
    destroyGroup(c, stream, stream);
    deleteStream(c, stream);
    redisFree(c);
}

// Thread to set stopflag to true
void timer(){
    std::this_thread::sleep_for(std::chrono::seconds(TIME));
    stopflag = true;
}


