//
// Created by 58 on 2016/4/10.
//

#ifndef BASE_NET_SOCKET_H_
#define BASE_NET_SOCKET_H_

#include <wedis/base/util/CommonST.h>
#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/SocketAPI.h>

namespace wRedis {
enum EM_SOCKET_TYPE {
	SOCKET_TYPE_RAW,
	SOCKET_TYPE_TCP,
	SOCKET_TYPE_UDP,
};

class Socket {
public:
	// status code
	enum STATUS_CODE {
		INVALID = -1,
		WORK_NORMAL = 0,
		UNKNOWN_ERROR = 1,
		INPUT_OVERFLOW = 2,
		OUTPUT_OVERFLOW = 3,
		ACCEPT_FAIL = 4,
		CONNECT_FAIL = 5,
		CONNECT_TIMEOUT = 6,
		LISTEN_FAIL,
		BIND_FAIL,
		CLOSE_FAIL,
		SEND_FAIL,
		RECV_FAIL,
		SHUTDOWN_FAIL,
		CREATE_FAIL,
	};

public:
	// constructor
	Socket(EM_SOCKET_TYPE eSocketType);
	Socket(EM_SOCKET_TYPE eSocketType, const I8* host, const UI16 port);

	// destructor
	virtual ~Socket();
public:
	// create socket
	BOOL create();
	// close connection
	void close();
	// try connect to remote host
	BOOL connect();
	BOOL connect(const I8* host, UI16 port);
	BOOL connect(TIMET timeout);

	// send data to peer
	UI32 send(const void* buf, UI32 len, UI32 flags = 0);
	// receive data from peer
	UI32 receive(void* buf, UI32 len, UI32 flags = 0);
	// available ioctl check FIONREAD
	UI32 available() const;
	// accept
	SOCKET accept(SOCKADDR* addr, UI32* addrlen);
	// bind
	BOOL bind();
	BOOL bind(UI16 port);
	// listen
	BOOL listen(I32 backlog);
public:
	// get port
	UI16 getPort() const;
	// set addr
	void setDest(const I8* host = NULLPTR, UI16 port = 0);
	// check if socket is valid
	BOOL isValid() const;
	// get socket descriptor
	SOCKET getSOCKET() const;

	SHostAndPort& getHostAndPort() { return m_HostAndPort; }

//=====================================================
// Opt
//=====================================================
public :
	// get/set socket's linger status
	UI32 getLinger() const;
	BOOL setLinger(UI32 lingertime);
	// is reuse addr
	BOOL isReuseAddr() const;
	// set reuse addr
	BOOL setReuseAddr(BOOL on = TRUE);
	// get/set socket's nonblocking status
	BOOL isNonBlocking() const;
	BOOL setNonBlocking(BOOL on = TRUE);
	// get/set is nodelay
	BOOL isNodelay() const;
	BOOL setNodelay(BOOL isNodelay);
	// get/set receive buffer size
	UI32 getReceiveBufferSize() const;
	BOOL setReceiveBufferSize(UI32 size);
	// get/set send buffer size
	UI32 getSendBufferSize() const;
	BOOL setSendBufferSize(UI32 size);
	// is/get sock error
	BOOL isSockError() const;
	// set send time out  mill sec
	BOOL setSendTimeOut(UI32 millSec);
	// set recv time out  mill sec
	BOOL setRecvTimeOut(UI32 millSec);
	// get/set status code
	STATUS_CODE	getStatusCode() const { return m_eStatusCode; }
	void		setStatusCode(STATUS_CODE code) { this->m_eStatusCode = code; }
	
public :
	// socket type
	EM_SOCKET_TYPE m_eSocketType;
	// socket fd
	SOCKET m_SocketID;
	// client: dest socket address structure
	// server: server socket bind address
	SOCKADDR_IN m_SockAddr;
	// client: dest NetAddr
	// server: server bind addr
	SHostAndPort m_HostAndPort;
	// status code
	STATUS_CODE m_eStatusCode;
};

}



#endif
