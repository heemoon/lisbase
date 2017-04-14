/*
 * RedisParser.h
 *
 *  Created on: 2016年5月19日
 *      Author: 58
 */

#ifndef BASE_NET_PROTOCOL_REDISPARSER_H_
#define BASE_NET_PROTOCOL_REDISPARSER_H_

#include <wedis/base/protocol/Codec.h>
#include <wedis/base/protocol/RedisReply.h>

namespace wRedis {

struct WRedisReadTask;
struct redisReply;

enum REDIS_PASER_ERR {
	REDIS_OOM = 1,
	REDIS_PROTOCOL_ERR = 2,
};

class RedisParser : public Codec {
public:
	RedisParser() : Codec() {
		for(I32 i = 0; i < 9; ++i) {
			m_Stack[i].cleanUp();
		}

		m_nIdx = INVALID_VALUE;
		m_Reply  = NULLPTR;
	}
	virtual ~RedisParser() {
		FREE_REDIS_REPLY(m_Reply);
	}
public:
	void destroy() {
		for(I32 i = 0; i < 9; ++i) {
			m_Stack[i].cleanUp();
		}

		m_nIdx = INVALID_VALUE;
		
		FREE_REDIS_REPLY(m_Reply);
	}
public:
	virtual BOOL   encode(void* pPacket, OutputStream* pBuffer, BOOL priroity = FALSE);
	virtual BOOL   encode(void* pPacket, I8* pBuffer, OUT I32* pOutLen);
	virtual void*  decode(InputStream* pInputStream, void* param);
private:
	void moveToNextTask();
	I32 redisReaderGetReply(InputStream* pStream, void **reply);
	I32 processItem(InputStream* pStream);
	I32 processLineItem(InputStream* pStream);
	I32 processBulkItem(InputStream* pStream);
	I32 processMultiBulkItem(InputStream* pStream);

private:
	static redisReply* createIntegerObject(const WRedisReadTask *task, InputStream* pStream, I32 len);
	static redisReply* createStringObject(const WRedisReadTask *task, InputStream* pStream, I32 len);
	static redisReply* createNilObject(const WRedisReadTask *task);
	static redisReply* createArrayObject(const WRedisReadTask *task, int elements);
private:
	WRedisReadTask m_Stack[9];
	I32 m_nIdx; /* Index of current read task */
	void* m_Reply; /* Temporary reply pointer */
};

} /* namespace wRedis */

#endif /* BASE_NET_PROTOCOL_REDISPARSER_H_ */
