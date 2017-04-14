/*
 * RedisParser.cpp
 *
 *  Created on: 2016年5月19日
 *      Author: 58
 */

#include <wedis/base/protocol/RedisCmd.h>
#include <wedis/base/protocol/RedisParser.h>
#include <wedis/base/util/Tools.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {
BOOL RedisParser::encode(void* pPacket, OutputStream* pBuffer, BOOL priroity/* = FALSE*/) {
	AssertRetFalse(pPacket);
	AssertRetFalse(pBuffer);

	cleanErr();

	RedisCmd* pMsg = (RedisCmd*) pPacket;
	BOOL bRet = FALSE;
	if(priroity) {
		// TODO: 这里只为了暂时给补充auth与database用所以只用了 栈空间，正常应该是动态生成
		I8 tmp[1024] = {0};
		I32 outLen = INVALID_VALUE;
		bRet = encode(pPacket, tmp, &outLen);
		if(bRet && 0 < outLen) {
			bRet = outLen == pBuffer->writeInHead(tmp, outLen);
		}
	}
	else {
		bRet = pMsg->encode(pBuffer);
	}

	return bRet;
}

BOOL RedisParser::encode(void* pPacket, I8* pBuffer, OUT I32* pOutLen) {
	AssertRetFalse(pPacket);
	AssertRetFalse(pBuffer);

	cleanErr();

	RedisCmd* pMsg = (RedisCmd*) pPacket;
	return pMsg->encode(pBuffer, pOutLen);
}

void* RedisParser::decode(InputStream* pInputStream, void* param) {
	Assert(pInputStream);

	void* reply = NULLPTR;
	cleanErr();

	// 判断是否是一个整包
	if(REDIS_ERR == redisReaderGetReply(pInputStream, &reply)) {
		return NULLPTR;
	}

	return reply;
}

redisReply* RedisParser::createIntegerObject(const WRedisReadTask *task, InputStream* pStream, I32 len) {
	if(NULLPTR == pStream) return NULLPTR;
	if(NULLPTR == task) return NULLPTR;

	I64 value = 0;
	if(0 < len) {
		I8 buf[32] = {0};
		if(len != pStream->read(buf, len)) return NULLPTR;
		value = Tools::Atoll(buf);
	}

	redisReply* r = createReplyObject(REDIS_REPLY_INTEGER);
    if (r == NULLPTR) return NULLPTR;

    r->integer = value;

    if (task->parent) {
        redisReply* parent = (redisReply*)task->parent->obj;
        Assert(parent->type == REDIS_REPLY_ARRAY);
        parent->element[task->idx] = r;
    }
    return r;
}

redisReply* RedisParser::createStringObject(const WRedisReadTask *task, InputStream* pStream, I32 len) {
	if(NULLPTR == pStream) return NULLPTR;
	if(NULLPTR == task) return NULLPTR;

    Assert(task->type == REDIS_REPLY_ERROR  ||
           task->type == REDIS_REPLY_STATUS ||
           task->type == REDIS_REPLY_STRING);

    I8* buf = (I8*)W_MALLOC(len+1);
    if (buf == NULLPTR) return NULLPTR;
    if(0 < len) {
    	if(len != pStream->read(buf, len)) {
    	    W_FREE(buf);
    	    return NULLPTR;
    	}
    }
    buf[len] = '\0';

    redisReply* r = createReplyObject(task->type);
    if (r == NULLPTR) return NULLPTR;

    r->str = buf;
    r->len = len;

    if (task->parent) {
    	redisReply*parent = (redisReply*)task->parent->obj;
        Assert(parent->type == REDIS_REPLY_ARRAY);
        parent->element[task->idx] = r;
    }
    return r;
}

redisReply* RedisParser::createNilObject(const WRedisReadTask *task) {
	if(NULLPTR == task) return NULLPTR;

    redisReply* r = createReplyObject(REDIS_REPLY_NIL);
    if (r == NULLPTR) return NULLPTR;

    if (task->parent) {
    	redisReply* parent = (redisReply*)task->parent->obj;
        Assert(parent->type == REDIS_REPLY_ARRAY);
        parent->element[task->idx] = r;
    }
    return r;
}

redisReply* RedisParser::createArrayObject(const WRedisReadTask *task, int elements) {
	if(NULLPTR == task) return NULLPTR;

    redisReply* r = createReplyObject(REDIS_REPLY_ARRAY);
    if (r == NULLPTR) return NULLPTR;

    if (elements > 0) {
        r->element = (redisReply**)W_CALLOC(elements, sizeof(redisReply*));
        if (r->element == NULLPTR) {
            freeReplyObject(r);
            return NULL;
        }
    }

    r->elements = elements;

    if (task->parent) {
    	redisReply* parent = (redisReply*)task->parent->obj;
        Assert(parent->type == REDIS_REPLY_ARRAY);
        parent->element[task->idx] = r;
    }
    return r;
}


I32 RedisParser::processLineItem(InputStream* pStream) {
    WRedisReadTask *cur = &(m_Stack[m_nIdx]);
    void *obj = NULLPTR;
    I32 len = INVALID_VALUE;

    if ((len = pStream->find(CRLF, 2)) != INVALID_VALUE) {
        if (cur->type == REDIS_REPLY_INTEGER) {
        	obj = createIntegerObject(cur, pStream, len);
        } else {
            obj = createStringObject(cur, pStream, len);
        }

        if (obj == NULL) {
        	err(REDIS_OOM);
            return REDIS_ERR;
        }

        // skip CRLF
        pStream->skip(2);

        /* Set reply if this is the root object. */
        if (m_nIdx == 0) m_Reply = obj;
        moveToNextTask();
        return REDIS_OK;
    }

    return REDIS_ERR;
}

I32 RedisParser::processBulkItem(InputStream* pStream) {
    WRedisReadTask *cur = &(m_Stack[m_nIdx]);
    void *obj = NULLPTR;
    I32 len = INVALID_VALUE;
    unsigned long bytelen;
    BOOL success = FALSE;

    I32 strlenlen = pStream->find(CRLF, 2);
    // 说明包未读完，再下次继续读
    if(INVALID_VALUE == strlenlen) return REDIS_ERR;

    // 1. 读取字符串长度
    I64 strlen = 0;
    if(0 < strlenlen) {
    	I8 buf[32] = {0};
    	if(FALSE == pStream->peek(buf, strlenlen)) return NULLPTR;

    	strlen = Tools::Atoll(buf);
    }

    if(0 > strlen) {
    	// skip lenlen and CRLF
    	pStream->skip(strlenlen + 2);
    	obj = createNilObject(cur);
    	success = TRUE;
    }
    else {
    	if(pStream->checkSize(strlenlen + 2 + strlen + 2)) {
    		// skip lenlen and CRLF
    		pStream->skip(strlenlen + 2);

    		// 检查尾部是否是以 CRLF 结尾
    		if(CR != pStream->getByte(strlen) || LF != pStream->getByte(strlen + 1)) {
    			err(REDIS_PROTOCOL_ERR);
    			return REDIS_ERR;
    		}

    		obj = createStringObject(cur, pStream, strlen);
    		// skip CRLF
    		pStream->skip(2);
    		success = TRUE;
    	}
    }

    /* Proceed when obj was created. */
    if (success) {
    	if (obj == NULLPTR) {
    		err(REDIS_OOM);
    		return REDIS_ERR;
    	}

    	/* Set reply if this is the root object. */
        if (m_nIdx == 0) m_Reply = obj;
        moveToNextTask();
        return REDIS_OK;
    }

    // 包没有接收全
    return REDIS_ERR;
}

/**
 * move to next
 */
void RedisParser::moveToNextTask() {
    WRedisReadTask *cur, *prv;
    while (m_nIdx >= 0) {
        /* Return a.s.a.p. when the stack is now empty. */
        if (m_nIdx == 0) {
        	m_nIdx--;
            return;
        }

        cur = &(m_Stack[m_nIdx]);
        prv = &(m_Stack[m_nIdx-1]);
        Assert(prv->type == REDIS_REPLY_ARRAY);
        if (cur->idx == prv->elements-1) {
        	m_nIdx--;
        } else {
            /* Reset the type because the next item can be anything */
            Assert(cur->idx < prv->elements);
            cur->type = REDIS_REPLY_INVALID;
            cur->elements = INVALID_VALUE;
            cur->idx++;
            return;
        }
    }
}

I32 RedisParser::processMultiBulkItem(InputStream* pStream) {
    WRedisReadTask *cur = &(m_Stack[m_nIdx]);
    void *obj = NULLPTR;
    I8 *p = NULLPTR;
	
    /* Set error for nested multi bulks with depth > 7 */
    if (m_nIdx == 8) {
    	// No support for nested multi bulk replies with depth > 7
    	err(REDIS_PROTOCOL_ERR);
        return REDIS_ERR;
    }

    I32 len = pStream->find(CRLF, 2);
    if(INVALID_VALUE == len) {
    	// 包还未读完
    	return REDIS_ERR;
    }

    I32 elements = 0;
    if(0 < len) {
    	I8 buf[32] = {0};
        if(len != pStream->read(buf, len)) return NULLPTR;

        elements = Tools::Atoll(buf);
    }

    BOOL root = (m_nIdx == 0);

    if (elements == INVALID_VALUE) {
    	obj = createNilObject(cur);
    	if (obj == NULLPTR) {
    		err(REDIS_OOM);
    		return REDIS_ERR;
    	}

        moveToNextTask();
    } else {
    	obj = createArrayObject(cur, elements);
    	if (obj == NULLPTR) {
    		err(REDIS_OOM);
    		return REDIS_ERR;
    	}

        /* Modify task stack when there are more than 0 elements. */
        if (elements > 0) {
            cur->elements = elements;
            cur->obj = obj;
            m_nIdx++;
            m_Stack[m_nIdx].cleanUp();
            m_Stack[m_nIdx].idx = 0;
            m_Stack[m_nIdx].parent = cur;
        } else {
        	moveToNextTask();
        }

		// CRLF
		pStream->skip(2);
    }

    /* Set reply if this is the root object. */
    if (root) m_Reply = obj;
    return REDIS_OK;
}



I32 RedisParser::processItem(InputStream* pStream) {
    WRedisReadTask *cur = &(m_Stack[m_nIdx]);
    I8 c = INVALID_VALUE;

    /* check if we need to read type */
    if (cur->type < 0) {
        if ((c = pStream->readByte()) != INVALID_VALUE) {
            switch (c) {
            case '-':
                cur->type = REDIS_REPLY_ERROR;
                break;
            case '+':
                cur->type = REDIS_REPLY_STATUS;
                break;
            case ':':
                cur->type = REDIS_REPLY_INTEGER;
                break;
            case '$':
                cur->type = REDIS_REPLY_STRING;
                break;
            case '*':
                cur->type = REDIS_REPLY_ARRAY;
                break;
            default:
            	err(REDIS_PROTOCOL_ERR);
                return REDIS_ERR;
            }
        } else {
            /* could not consume 1 byte */
            return REDIS_ERR;
        }
    }

    /* process typed item */
    switch(cur->type) {
    case REDIS_REPLY_ERROR:
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_INTEGER:
        return processLineItem(pStream);
    case REDIS_REPLY_STRING:
        return processBulkItem(pStream);
    case REDIS_REPLY_ARRAY:
        return processMultiBulkItem(pStream);
    default:
        Assert(NULLPTR);
        return REDIS_ERR; /* Avoid warning. */
    }
}

I32 RedisParser::redisReaderGetReply(InputStream* pStream, void **reply) {
    /* Default target pointer to NULL. */
    if (reply != NULLPTR)
        *reply = NULLPTR;

    /* When the buffer is empty, there will never be a reply. */
    if (pStream->getBuffUsedSize() == 0)
        return REDIS_OK;

    /* Set first item to process when the stack is empty. */
    if (m_nIdx == INVALID_VALUE) {
        m_Stack[0].cleanUp();
        m_nIdx = 0;
    }

    /* Process items in reply. */
    while (m_nIdx >= 0)
        if (processItem(pStream) != REDIS_OK)
            break;

    /* Return ASAP when an error occurred. */
    if (err()) {
    	destroy();
        return REDIS_ERR;
    }

    /* Emit a reply when there is one. */
    if (m_nIdx == INVALID_VALUE) {
        if (reply != NULLPTR)
            *reply = m_Reply;
        m_Reply = NULLPTR;
    }
    return REDIS_OK;
}


} /* namespace wRedis */
