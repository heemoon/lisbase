//
// Created by 58 on 2016/4/18.
//

#include <netinet/tcp.h> // for TCP_NODELAY
#include <wedis/base/net/Socket.h>
#include <sys/poll.h>

namespace wRedis {
Socket::Socket(EM_SOCKET_TYPE eSocketType)
	: m_eSocketType(eSocketType),
	  m_HostAndPort(),
	  m_eStatusCode(INVALID)
{
	m_SocketID = INVALID_SOCKET;
	memset(&m_SockAddr, 0, sizeof(m_SockAddr));
}

Socket::Socket(EM_SOCKET_TYPE eSocketType, const I8* host , const UI16 port)
	: m_eSocketType(eSocketType),
	  m_HostAndPort(),
	  m_eStatusCode(CREATE_FAIL)
{
	m_SocketID = INVALID_SOCKET;
	memset(&m_SockAddr, 0, sizeof(m_SockAddr));
	m_HostAndPort.reset(host, port);
}
	
Socket::~Socket() {
	close();
}
	
BOOL Socket::create() {
	switch(m_eSocketType) {
	case SOCKET_TYPE_TCP:
		m_SocketID = SocketAPI::socket_ex(AF_INET, SOCK_STREAM, 0);
		break;
	case SOCKET_TYPE_UDP:
		m_SocketID = SocketAPI::socket_ex(AF_INET, SOCK_DGRAM, 0);
		break;
	case SOCKET_TYPE_RAW:
		m_SocketID = SocketAPI::socket_ex(AF_INET, SOCK_RAW, 0);
		break;
	default:
		break;
	}

	if (!isValid()) {
		setStatusCode(Socket::CREATE_FAIL);
		return FALSE;
	}

	memset(&m_SockAddr, 0, sizeof(m_SockAddr));
	m_SockAddr.sin_family = AF_INET;
	return TRUE;
}

void Socket::setDest(const I8* host/* = NULLPTR*/, UI16 port/* = 0*/) {
	if(NULLPTR != host) {
		m_HostAndPort.reset(host, port);
	}
	memset(&m_SockAddr, 0, sizeof(m_SockAddr));
	if(m_HostAndPort.isValid()) {
		m_SockAddr.sin_family = AF_INET;
		m_SockAddr.sin_addr.s_addr = inet_addr( m_HostAndPort.m_IP );
		m_SockAddr.sin_port = htons(m_HostAndPort.m_uPort);
	}
}

void Socket::close() {
	if(isValid()) {
		SocketAPI::closesocket_ex(m_SocketID);
	}

	m_SocketID = INVALID_SOCKET;
	memset(&m_SockAddr, 0, sizeof(SOCKADDR_IN));
}

BOOL Socket::connect() {
	AssertRetFalse(m_HostAndPort.isValid());

	memset(&m_SockAddr, 0, sizeof(m_SockAddr));
	m_SockAddr.sin_addr.s_addr = inet_addr(m_HostAndPort.m_IP);
	// set sockaddr's port
	m_SockAddr.sin_port = htons(m_HostAndPort.m_uPort);
	m_SockAddr.sin_family = AF_INET;

	// try to connect to peer host
	return SocketAPI::connect_ex(m_SocketID, (const struct sockaddr *)&m_SockAddr, sizeof(m_SockAddr));
}

BOOL Socket::connect(const I8* host, UI16 port) {
	m_HostAndPort.reset(host, port);
	return connect();
}

BOOL Socket::connect(TIMET timeout) {
	if(isSockError())
		return FALSE;

	BOOL oldFlag = isNonBlocking();
	if(!oldFlag) {
		setNonBlocking(TRUE);
	}

	BOOL bOk = FALSE;
	do {
		if(!connect() || errno != EINPROGRESS) {
			setStatusCode(CONNECT_FAIL);
			break;
		}

		struct pollfd pfd[1];
		pfd[0].fd = m_SocketID;
		pfd[0].events = POLLOUT;

		I32 res = -1;
		if((res = ::poll(pfd, 1, timeout)) == -1) {
			setStatusCode(CONNECT_FAIL);
			break;
		}
		else if(0 == res) {
			setStatusCode(CONNECT_TIMEOUT);
			break;
		}

		if(isSockError()) {
			setStatusCode(CONNECT_FAIL);
			break;
		}

		bOk = TRUE;

	} while(FALSE);

	if(!oldFlag) {
		setNonBlocking(FALSE);
	}

	return bOk;
}

UI32 Socket::send(const void* buf, UI32 len, UI32 flags) {
	UI32 ret = 0;
	if(SOCKET_TYPE_TCP == m_eSocketType)
		ret = SocketAPI::send_ex(m_SocketID, buf, len, flags);
	else if(SOCKET_TYPE_UDP == m_eSocketType) {
		ret = SocketAPI::sendto_ex(m_SocketID, buf, len, flags, (SOCKADDR*) &m_SockAddr, sizeof(m_SockAddr));
	}
	return ret;
}

UI32 Socket::receive(void* buf, UI32 len, UI32 flags) {
	UI32 ret = 0;
	if(SOCKET_TYPE_TCP == m_eSocketType)
		ret = SocketAPI::recv_ex(m_SocketID, buf, len, flags);
	else if(SOCKET_TYPE_UDP == m_eSocketType) {
		UI32 sockLen = sizeof(m_SockAddr);
		ret = SocketAPI::recvfrom_ex(m_SocketID, buf, len, flags, (SOCKADDR*) &m_SockAddr, &sockLen);
	}
	return ret;
}

UI32 Socket::available() const {
	return SocketAPI::availablesocket_ex(m_SocketID);
}

SOCKET Socket::accept(SOCKADDR* addr, UI32* addrlen) {
	SOCKET retSocket = SocketAPI::accept_ex(m_SocketID, addr, addrlen);
	if(0 > retSocket) {
		setStatusCode(ACCEPT_FAIL);
	}
	return retSocket;
}

UI32 Socket::getLinger() const {
	struct linger ling;
	UI32 len = sizeof(ling);
	SocketAPI::getsockopt_ex(m_SocketID, SOL_SOCKET, SO_LINGER, &ling, &len);
	return ling.l_linger;
}

BOOL Socket::setLinger(UI32 lingertime) {
	struct linger ling;
	ling.l_onoff = lingertime > 0 ? 1 : 0;
	ling.l_linger = lingertime;
	return SocketAPI::setsockopt_ex(m_SocketID, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}

BOOL Socket::isNonBlocking() const {
	return SocketAPI::getsocketnonblocking_ex(m_SocketID) > 0;
}
	
BOOL Socket::setNonBlocking(BOOL on/* = TRUE*/) {
	return SocketAPI::setsocketnonblocking_ex(m_SocketID, on);
}

UI32 Socket::getReceiveBufferSize() const {
	UI32 ReceiveBufferSize;
	UI32 size = sizeof(ReceiveBufferSize);
	SocketAPI::getsockopt_ex(m_SocketID, SOL_SOCKET, SO_RCVBUF, &ReceiveBufferSize, &size);
	return ReceiveBufferSize;
}

BOOL Socket::setReceiveBufferSize(UI32 size) {
	return (BOOL)(SocketAPI::setsockopt_ex(m_SocketID, SOL_SOCKET, SO_RCVBUF, &size, sizeof(UI32)));
}

UI32 Socket::getSendBufferSize() const {
	UI32 SendBufferSize;
	UI32 size = sizeof(SendBufferSize);
	SocketAPI::getsockopt_ex(m_SocketID, SOL_SOCKET, SO_SNDBUF, &SendBufferSize, &size);
	return SendBufferSize;
}

BOOL Socket::setSendBufferSize(UI32 size) {
	return (BOOL)(SocketAPI::setsockopt_ex(m_SocketID, SOL_SOCKET, SO_SNDBUF, &size, sizeof(UI32)));
}

BOOL Socket::setSendTimeOut(UI32 millSec) {
	if(0 == millSec) return TRUE;

	time_t sec  = (time_t)(millSec / 1000);
	UI32   usec = (millSec % 1000) * 1000;
	struct timeval tv = {sec, usec};
	return (BOOL) (SocketAPI::setsockopt_ex(m_SocketID, SOL_SOCKET, SO_SNDTIMEO, (I8*) &tv, sizeof(tv)));
}

BOOL Socket::setRecvTimeOut(UI32 millSec) {
	time_t sec  = (time_t)(millSec / 1000);
	UI32   usec = (millSec % 1000) * 1000;
	struct timeval tv = {sec, usec};
	return (BOOL) (SocketAPI::setsockopt_ex(m_SocketID, SOL_SOCKET, SO_RCVTIMEO, (I8*) &tv, sizeof(tv)));
}

UI16 Socket::getPort() const {
	return m_HostAndPort.m_uPort;
}

BOOL Socket::isValid() const {
	return m_SocketID != INVALID_SOCKET;
}

SOCKET Socket::getSOCKET() const {
	return m_SocketID;
}

BOOL Socket::isSockError() const {
	I32 err = 0;
	UI32 len = sizeof(err);
		
	I32 ret = SocketAPI::getsockopt_ex2(m_SocketID, SOL_SOCKET, SO_ERROR, &err, &len);
	if(0 != ret) {
		return TRUE;
	}

	if(err) {
		errno = err;
		return TRUE;
	}

	return FALSE;
}

BOOL Socket::bind() {
	AssertRetFalse(INVALID_VALUE != m_HostAndPort.m_uPort);
	m_SockAddr.sin_port        = htons(m_HostAndPort.m_uPort);
	m_SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	BOOL result = SocketAPI::bind_ex(m_SocketID, (const SOCKADDR*)&m_SockAddr, sizeof(m_SockAddr));
	if(result)
		return TRUE;
	else {
		setStatusCode(BIND_FAIL);
		return FALSE;
	}
}

BOOL Socket::bind(UI16 port) {
	m_HostAndPort.m_uPort = port;
	m_SockAddr.sin_port        = htons(port);
	m_SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	BOOL result = SocketAPI::bind_ex(m_SocketID, (const SOCKADDR*)&m_SockAddr, sizeof(m_SockAddr));
	if(result)
		return TRUE;
	else {
		setStatusCode(BIND_FAIL);
		return FALSE;
	}
}

BOOL Socket::listen(I32 backlog) {
	BOOL result = SocketAPI::listen_ex(m_SocketID, backlog);
	if(!result) {
		setStatusCode(LISTEN_FAIL);
		return FALSE;
	}
	return TRUE;
}
	
BOOL Socket::isReuseAddr() const {
	I32 reuse;
	UI32 len = sizeof(reuse);
	SocketAPI::getsockopt_ex(m_SocketID, SOL_SOCKET, SO_REUSEADDR, &reuse, &len);
	return reuse == 1;
}
	
BOOL Socket::setReuseAddr(BOOL on/* = TRUE*/) {
	I32 opt = on == TRUE ? 1 : 0;
	return SocketAPI::setsockopt_ex(m_SocketID, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

BOOL Socket::isNodelay() const {
	I32 opt;
	UI32 len = sizeof(opt);
	SocketAPI::getsockopt_ex(m_SocketID, IPPROTO_TCP, TCP_NODELAY, &opt, &len);
	return opt == 1;
}

BOOL Socket::setNodelay(BOOL isNodelay) {
	I32 opt = isNodelay == TRUE ? 1 : 0;
	return SocketAPI::setsockopt_ex(m_SocketID, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

}


