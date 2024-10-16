#include <hiredis/hiredis.h>
#include <thread>
#include <vector>
#include <unistd.h>

#include "drone.cpp"

int main(int argc, char *argv[]) {
    
    setvbuf(stdout, NULL, _IONBF, 0);

    // Timer Thread
    std::thread timerThread(timer);
    timerThread.detach();
    printf("Drones execution started\n");

    // Allocation of redis context
    redisContext *c = connectToRedis("redis", 6379);
    createGroup(c, "Commands", "Group", true);
    printf("Connected to Redis successfully\n");
    
    // Read The number of paths requested from Control Center
    std::string res = ReadGroupMsgVal(c, 0, "Group", "Commands");
    int nDrones = std::stoi(res);
    destroyGroup(c, "Commands", "Group");
    deleteStream(c, "Commands");
    printf("Number of paths requested from Control Center: %d\n", nDrones);

    // Generation of threads (one thread produces one drone and works with it)
    // The drones call other drones to start and so on
    printf("Starting drone threads...\n");
    std::vector<std::thread> threads;
    for (int i = 0; i < nDrones; i++) {
        threads.emplace_back(initDroneX);
    }
    
    
    // Wait for all threads to stop
    printf("Waiting for all drone threads to complete...\n");
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    printf("All drone threads have completed\n");

    // Send number of drones actually created to Control Center
    std::string finalS = "NumberDrones";
    createGroup(c, finalS, finalS, true);
    SendStreamMsg(c, finalS.c_str(), std::to_string(Drone::getInstanceCount()).c_str());
    printf("Sent number of created drones to Control Center: %d\n", Drone::getInstanceCount());

    redisFree(c);
    printf("Redis connection closed\n");

    return (0);
}