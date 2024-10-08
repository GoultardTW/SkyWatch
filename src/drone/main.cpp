#include <hiredis/hiredis.h>
#include <thread>
#include <vector>
#include <unistd.h>

#include "drone.cpp"

int main(int argc, char *argv[]) {
    
    // Timer Thread
    std::thread timerThread(timer);
    timerThread.detach();

    // Allocation of redis context
    redisContext *c = connectToRedis("redis", 6379);
    createGroup(c, "Commands", "Group", true);
    
    // Read The number of paths requested from Control Center
    std::string res = ReadGroupMsgVal(c, 0, "Group", "Commands");
    int nDrones = std::stoi(res);
    destroyGroup(c, "Commands", "Group");
    deleteStream(c, "Commands");

    // Generation of threads (one thread produces one drone and works with it)
    // The drones call other drones to start and so on
    std::vector<std::thread> threads;
    for (int i = 0; i < nDrones; i++) {
        threads.emplace_back(initDroneX);
    }
    
    // Wait for all threads to stop
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Send number of drones actually created to Control Center
    std::string finalS = "Drones";
    createGroup(c, finalS, finalS, true);
    SendStreamMsg(c, finalS.c_str(), std::to_string(Drone::getInstanceCount()).c_str());
    redisFree(c);

    return (0);
}