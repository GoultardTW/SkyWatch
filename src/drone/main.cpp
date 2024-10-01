#include <hiredis/hiredis.h>
#include <thread>
#include <vector>
#include <unistd.h>

#include "drone.cpp"

int main(int argc, char *argv[]) {

    // Allocation of redis context
    redisContext *c = connectToRedis("redis", 6379);
    createGroup(c, "Commands", "Group", true);
    
    // Read The number of drones requested from Control Center
    std::string res = ReadGroupMsgVal(c, 0, "Group", "Commands");
    int nDrones = std::stoi(res);
    destroyGroup(c, "Commands", "Group");
    deleteStream(c, "Commands");
    redisFree(c);

    // Generation of threads (one thread produces one drone and works with it)
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

    return (0);
    
}