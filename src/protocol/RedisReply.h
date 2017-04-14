/*
 * RedisReply.h
 *
 *  Created on: 2016年5月20日
 *      Author: 58
 */

#ifndef BASE_NET_PROTOCOL_REDISREPLY_H_
#define BASE_NET_PROTOCOL_REDISREPLY_H_

#include <wedis/base/util/Global.h>
#include <wedis/base/util/Tools.h>
#include <string>

namespace wRedis {


#define REDIS_OK  0
#define REDIS_ERR -1

//enum EM_REDIS_REPLY_TYPE {
//	REDIS_REPLY_INVALID = -1,
//	REDIS_REPLY_STRING = 1,
//	REDIS_REPLY_ARRAY = 2,
//	REDIS_REPLY_INTEGER = 3,
//	REDIS_REPLY_NIL = 4,
//	REDIS_REPLY_STATUS = 5,
//	REDIS_REPLY_ERROR = 6,
//};

#define REDIS_REPLY_INVALID -1
#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6

struct WRedisReadTask {
public:
	I32 type;
    int elements; /* number of elements in multibulk container */
    int idx; /* index in parent (array) object */
    void *obj; /* holds user-generated value for a read task */
    struct WRedisReadTask *parent; /* parent task */
    void *privdata; /* user-settable arbitrary field */

public:
    WRedisReadTask();
    void cleanUp();
};

/* This is the reply object returned by redisCommand() */
typedef struct redisReply {
	I32 type; /* REDIS_REPLY_* */
    I64 integer; /* The integer when type is REDIS_REPLY_INTEGER */
    I32 len; /* Length of string */
    I8 *str; /* Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
    size_t elements; /* number of elements, for REDIS_REPLY_ARRAY */
    struct redisReply **element; /* elements vector for REDIS_REPLY_ARRAY */

    static std::string longToString(I64 value) {
    	I8 buf[32];
    	Tools::Snprintf(buf, sizeof(buf), "%lld", value);
    	return buf;
    }

    static std::string toString(redisReply* reply) {
    	static const I8* const types[] = { "",
    	      "REDIS_REPLY_STRING", "REDIS_REPLY_ARRAY",
    	      "REDIS_REPLY_INTEGER", "REDIS_REPLY_NIL",
    	      "REDIS_REPLY_STATUS", "REDIS_REPLY_ERROR" };
    	std::string str;

    	if (!reply) return str;

    	str += types[reply->type] + std::string("(") + longToString(reply->type) + ") ";
    	str += "{ ";
    	if (reply->type == REDIS_REPLY_STRING ||
    		reply->type == REDIS_REPLY_STATUS ||
			reply->type == REDIS_REPLY_ERROR)
    	{
    		str += '"' + std::string(reply->str, reply->len) + '"';
    	}
    	else if(reply->type == REDIS_REPLY_INTEGER) {
    	    str += longToString(reply->integer);
    	}
    	else if(reply->type == REDIS_REPLY_ARRAY) {
    	    str += longToString(reply->elements) + " ";
    	    for (size_t i = 0; i < reply->elements; i++) {
    	      str += " " + toString(reply->element[i]);
    	    }
    	}
    	str += " }";

    	return str;
    }

} redisReply;


extern redisReply* createReplyObject(I32 type);
extern void  	   freeReplyObject(void *reply);

#define FREE_REDIS_REPLY(x) if( (x)!=NULLPTR ) { freeReplyObject((x)); (x)=NULLPTR; }

} /* namespace wRedis */

#endif /* BASE_NET_PROTOCOL_REDISREPLY_H_ */
