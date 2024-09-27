#include "con2redis.h"
#include "local.h"
#include <hiredis/hiredis.h>

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

// To make a Group
void makeGroup(redisContext *c, const char *stream, const char *group) {
    redisReply *r = RedisCommand(c, "XGROUP CREATE %s %s 0 MKSTREAM", stream, group);
    freeReplyObject(r);
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
