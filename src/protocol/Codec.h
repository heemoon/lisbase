/*
 * Codec.h
 *
 *  Created on: 2016年5月13日
 *      Author: lixiwen
 */

#ifndef NET_CODEC_H_
#define NET_CODEC_H_

#include <wedis/base/net/StreamBuffer.h>
#include <wedis/base/util/Noncopyable.h>

namespace wRedis {

class Codec : public Noncopyable {
public:
	Codec();
	virtual ~Codec();
public:
	/**
	 * encode
	 * @param pPacket 具体的包
	 * @param pBuffer encode后写入的缓存
	 * @return byte数组长度
	 */
	virtual BOOL  encode(void* pPacket, OutputStream* pBuffer, BOOL priroity = FALSE) = 0;
	virtual BOOL  encode(void* pPacket, I8* pBuffer, OUT I32* pOutLen) = 0;

	/**
	 * decode
	 * @param buffer
	 * @param buffer的长度
	 * @return 协议返回包
	 */
	virtual void* decode(InputStream* pInputStream, void* param) = 0;

public:
	void err(I32 err) { m_Err = err; }
	I32 err() { return m_Err; }
	void cleanErr() { m_Err = 0; }
private:
	I32 m_Err;
};

} /* namespace wRedis */

#endif /* NET_PARSER_H_ */
