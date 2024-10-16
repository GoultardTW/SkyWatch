#include "con2redis.h"
#include <cstring>
#include <hiredis/hiredis.h>
#include <hiredis/read.h>
#include <string>

// It returns the number of Streams
int ReadNumStreams(redisReply *r)
{
  return (r->elements);  
}  

// It reads k-th Stream name
int ReadStreamName(redisReply *r, char *streamname,  long unsigned int k)
{
  strcpy(streamname, r->element[k]->element[0]->str);
  return(0); 
}  

// It reads the number of message of the k-th Stream
int ReadStreamNumMsg(redisReply *r, long unsigned int streamnum)
{
  return(r->element[streamnum]->element[1]->elements);
}  

// It copies in msgid the id of the i-th msg, in the k-th Stream
int ReadStreamNumMsgID(redisReply *r, long unsigned int streamnum, int msgnum, char *msgid)
{
  strcpy(msgid, r->element[streamnum]->element[1]->element[msgnum]->element[0]->str);
  return(0);
}

// It returns the number of fields in a certain message, in a certain Stream
int ReadStreamMsgNumVal(redisReply *r, long unsigned int streamnum, int msgnum)
{
  return(r->element[streamnum]->element[1]->element[msgnum]->element[1]->elements);
}

// It copies in value the value of a certain field in a certain message in a certain Stream
std::string ReadStreamMsgVal(redisReply *r, long unsigned int streamnum, int msgnum, int entry)
{ 
  return std::string(r->element[streamnum]->element[1]->element[msgnum]->element[1]->element[entry]->str);
}

// It deletes a message
void DelMsg(redisContext* c, const char* stream, const char* messageId){
  redisReply* reply = (redisReply*)redisCommand(c, "XDEL %s %s", stream, messageId);
  if (reply != nullptr || reply != NULL || reply->type != REDIS_REPLY_ERROR) {
        freeReplyObject(reply);
  }
}

// Returns the message read from a consumer
std::string ReadGroupMsgVal(redisContext* c, int id, const char* group, const char* stream, const char* block = "0"){
  std::string consumerName = "Drone_" + std::to_string(id);
  redisReply *rep = (redisReply *)redisCommand(c, "XREADGROUP GROUP %s %s NOACK COUNT 1 BLOCK %s STREAMS %s >", group, consumerName.c_str(), block, stream);
  if (rep == nullptr || rep == NULL || rep->type == REDIS_REPLY_ERROR) {
        return "Null";
  }
  std::string res = ReadStreamMsgVal(rep, 0, 0, 1);
  std::string messageId = rep->element[0]->element[1]->element[0]->element[0]->str;
  DelMsg(c, stream, messageId.c_str());
  freeReplyObject(rep);
  return res;
}

// It send a message inside a Stream
void SendStreamMsgN(redisContext* c, const char* stream, char** value, int num_fields){
  std::string xadd = "XADD " + std::string(stream) + " MKSTREAM * ";
  for (int i=0; i<num_fields; i+=1){
    xadd += std::string(value[i]) + " ";
  }
  redisReply *reply = (redisReply *)redisCommand(c, xadd.c_str());
  if (reply != NULL) {
        //printf("Message added to stream: %s\n", reply->str);
        freeReplyObject(reply);
  }
}

// It send a message inside a Stream
void SendStreamMsg(redisContext* c, const char* stream, const char* value){
  std::string xadd = "XADD " + std::string(stream) + " * value " + std::string(value);
  redisReply *reply = (redisReply *)redisCommand(c, xadd.c_str());
  if (reply != NULL) {
        //printf("Message added to stream: %s\n", reply->str);
        freeReplyObject(reply);
  }
}
