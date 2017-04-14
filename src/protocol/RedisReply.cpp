/*
 * RedisReply.cpp
 *
 *  Created on: 2016年5月20日
 *      Author: 58
 */

#include <wedis/base/protocol/RedisReply.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {

WRedisReadTask::WRedisReadTask() {
	cleanUp();
}

void WRedisReadTask::cleanUp() {
	type = REDIS_REPLY_INVALID;
	elements = INVALID_VALUE;
	idx = INVALID_VALUE;
	obj = NULLPTR;
	parent = NULLPTR;
}


/* Create a reply object */
redisReply* createReplyObject(I32 type) {
    redisReply *r = (redisReply*)W_CALLOC(1, sizeof(*r));
    if(r == NULLPTR) return NULLPTR;

    r->type = type;

    return r;
}

/* Free a reply object */
void freeReplyObject(void *reply) {
    redisReply *r = (redisReply*)reply;
    size_t j;

    if (r == NULL)
        return;

    switch(r->type) {
    case REDIS_REPLY_INTEGER:
        break; /* Nothing to free */
    case REDIS_REPLY_ARRAY:
        if (r->element != NULL) {
            for (j = 0; j < r->elements; j++)
                if (r->element[j] != NULL)
                    freeReplyObject(r->element[j]);
            W_FREE(r->element);
        }
        break;
    case REDIS_REPLY_ERROR:
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_STRING:
        if (r->str != NULL)
        	W_FREE(r->str);
        break;
    default:
    	break;
    }
    W_FREE(r);
}


} /* namespace wRedis */
