//
// Created by 58 on 2016/4/10.
//

#ifndef BASE_NET_CONNECTOR_H_
#define BASE_NET_CONNECTOR_H_

#include <wedis/base/net/Socket.h>
#include <wedis/base/net/StreamBuffer.h>
#include <wedis/base/protocol/Codec.h>
#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Guid.h>
#include <wedis/base/net/Event.h>

namespace wRedis {

enum EM_CONNECTOR_STATUS {
	EM_CONNECTOR_INVALID,
	EM_CONNECTOR_CONNECTING,
	EM_CONNECTOR_CONNECTED,
	EM_CONNECTOR_SENDING,
	EM_CONNECTOR_RECIVING,
	EM_CONNECTOR_NORM,
};

class Connector {
#define DEFAULT_INPUT_STREAM_SIZE (1 * 1024)
#define DEFAULT_INPUT_STREAM_MAX_SIZE (16 * 1024 * 1024)
#define DEFAULT_OUTPUT_STREAM_SIZE (8 * 1024)
#define DEFAULT_OUTPUT_STREAM_MAX_SIZE (8 * 1024 * 1024)

public:
	Connector();
	virtual ~Connector();
public:
	virtual BOOL    init(EM_SOCKET_TYPE eType, Codec* pParser);
	virtual	BOOL	init(EM_SOCKET_TYPE eType, const I8* pHost, const UI16 uPort, Codec* pParser);
	virtual BOOL	procInput();
	virtual BOOL	procOutput();
	virtual BOOL    procCommand();
	virtual	BOOL	procErr() { return TRUE; }
	virtual BOOL	sendPacket(void* pPacket);
	virtual BOOL 	sendPriorityPacket(void* pPacket);
public:
	GUID			getGUID() const { return m_GUID; }
	void			setGUID(GUID id) { m_GUID = id; }
	//网络连接接口
	Socket*			getSocket() const { return m_pSocket; }
	//是否阻塞
	BOOL            isNonBlocking() const;

	virtual SHostAndPort* getHostAndPort();
	virtual void setHostAndPort(SHostAndPort& hp);

	//断开网络连接
	virtual void	disconnect();
	//判断网络连接是否有效
	virtual	BOOL	isValid();
	//清除网络连接数据和缓存数据
	virtual	void	cleanUp();
protected:
	//Connector GUID
	GUID					m_GUID;
	//网络连接句柄
	Socket*					m_pSocket;
	//输入数据缓存
	InputStream*			m_pSocketInputStream;
	//输出数据缓存
	OutputStream*			m_pSocketOutputStream;
	//协议转码器
	Codec*                  m_pParser;
};

}


#endif
