#include <hiredis/hiredis.h>
#include <vector>
#include <thread>
#include <unistd.h>

#include "drone.cpp"

int main(int argc, char *argv[]) {

    // Allocation of redis context
    redisContext *c = connectToRedis("redis", 6379);

    // Creation of drone Group
    makeGroup(c, "Commands", "DroneGroup");

    // Read The number of drones requested from Control Center
    std::string res = ReadGroupMsgVal(c, -1, "DroneGroup", "Commands");
    int nDrones = std::stoi(res);

    // Generation of threads (one thread produces one drone and works with it)
    std::vector<std::thread> threads;
    for (int i = 0; i < nDrones; ++i) {
        threads.emplace_back(initDrone, c, i); // Ogni thread esegue `threadFunction`
    }

    // Wait for all threads to stop
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return (0);
}