#include "con2redis.h"
#include "local.h"
#include <hiredis/hiredis.h>
#include <string>
#include <iostream>
#include <vector>

void print_reply_types()
{
  printf("REDIS_REPLY_STRING=%d,\
REDIS_REPLY_STATUS=%d,\
REDIS_REPLY_INTEGER=%d,\
REDIS_REPLY_NIL=%d,\
REDIS_REPLY_ERROR=%d,\
REDIS_REPLY_ARRAY=%d\n",
	 REDIS_REPLY_STRING,
	 REDIS_REPLY_STATUS,
	 REDIS_REPLY_INTEGER,
	 REDIS_REPLY_NIL,
	 REDIS_REPLY_ERROR,
	 REDIS_REPLY_ARRAY
	 );
}

// It checks the type of the reply
void assertReplyType(redisContext *c, redisReply *r, int type) {
    if (r == NULL)
        dbg_abort("NULL redisReply (error: %s)", c->errstr);
    if (r->type != type)
      { print_reply_types();
	    dbg_abort("Expected reply type %d but got type %d", type, r->type);
      }
}

// It checks that the reply != null
void assertReply(redisContext *c, redisReply *r) {
    if (r == NULL)
        dbg_abort("NULL redisReply (error: %s)", c->errstr);
}

// To establish a redis connection
redisContext *connectToRedis(const char *hostname, int port) {
    redisContext *c = redisConnect(hostname, port);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Can't allocate redis context\n");
        }
        return NULL;
    }
    return c;
}

void createGroup(redisContext *context, const std::string &stream, const std::string &group, bool mkstream = true) {
    // Create a group
    auto *reply = (redisReply *) redisCommand(context, "XGROUP CREATE %s %s $ MKSTREAM", stream.c_str(), group.c_str());
    if (reply == nullptr) {
        std::cerr << "createGroup: Error: " << context->errstr << std::endl;
        return;
    }
}

long destroyGroup(redisContext *context, const std::string &stream, const std::string &group) {
    // Create a group
    auto *reply = (redisReply *) redisCommand(context, "XGROUP DESTROY %s %s",
                                              stream.c_str(), group.c_str());
    if (reply == nullptr) {
        // cerr << "deleteGroup: Error: " << context->errstr << endl;
        return -1;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        //cerr << "deleteGroup: ReplyError: " << reply->str << endl;
        return -1;
        //        return "";
    }
    long result = reply->integer;
    freeReplyObject(reply);
    return result;
}

long deleteStream(redisContext *context, const std::string &stream) {
    // Create a group
    auto *reply = (redisReply *) redisCommand(context, "DEL %s", stream.c_str());
    if (reply == nullptr) {
        std::cerr << "deleteStream: Error: " << context->errstr << std::endl;
        return -1;
    }

    long result = reply->integer;
    freeReplyObject(reply);
    return result;
}

long getStreamLen(redisContext *context, const std::string &stream) {
    // Get the info
    auto *reply = (redisReply *) redisCommand(context, "XINFO STREAM %s", stream.c_str());
    if (reply == nullptr) {
        std::cerr << "printInfoStream: Error: " << context->errstr << std::endl;
        freeReplyObject(reply);
        return -1;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cerr << "printInfoStream: Error: " << reply->str << std::endl;
        freeReplyObject(reply);
        return -1;
    }

    long value = reply->element[1]->integer;
    freeReplyObject(reply);
    return value;
}

std::vector<std::string> splitMessage(std::string report){
    std::vector<std::string> res;
    int j=0;
    for(int i=1; i<report.length(); i++){
        if(report[i]=='/'){
            res.emplace_back(report.substr(j,i-j));
            j=i+1;
        }
    }
    return res;
}
