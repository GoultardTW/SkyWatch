#include <stdlib.h>
#include <math.h>
#include <thread>
#include <chrono>

#include "../con2redis/src/redisfun.cpp"
#include "../con2redis/src/readreply.cpp"
#include "drone.hpp"

// It manages one drone (is used inside n threads)
void initDrone(int id){
    Drone drone(id);
    redisC:
    redisContext *c = connectToRedis("redis", 6379);
    if(c == nullptr || c->err) goto redisC;
    std::string stream = "Commands"+std::to_string(id);
    std::string group = "Group"+std::to_string(id);
    createGroup(c, stream, group, true);
    std::string msg = ReadGroupMsgVal(c, id, group.c_str(), stream.c_str());
    destroyGroup(c, stream, group);
    deleteStream(c, stream);
    redisFree(c);

    for(int m=0; m<msg.length(); m++){
        drone.ExecuteMove(msg[m]);
        //std::this_thread::sleep_for(std::chrono::milliseconds(2400));
    }
    printf("Il drone #%d si trova nella posizione (%d, %d)\n", id, drone.getX(), drone.getY());
}
