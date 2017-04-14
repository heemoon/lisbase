//////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////
#include <sys/types.h>			// for accept()
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>			// for inet_xxx()
#include <netinet/in.h>
#include <errno.h>				// for errno
#include <wedis/base/util/FileAPI.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/SocketAPI.h>

namespace wRedis {
//////////////////////////////////////////////////
// external variable
//////////////////////////////////////////////////
//extern I32 errno;

using namespace wRedis::FileAPI;

I8 Error[_ESIZE];
//////////////////////////////////////////////////////////////////////
//
// SOCKET SocketAPI::socket_ex ( I32 domain , I32 type , I32 protocol )
//
// exception version of socket()
//
// Parameters
//     domain - AF_INET(I32ernet socket), AF_UNIX(I32ernal socket), ...
//	   type  - SOCK_STREAM(TCP), SOCK_DGRAM(UDP), ...
//     protocol - 0
//
// Return
//     socket descriptor
//
//
//////////////////////////////////////////////////////////////////////
SOCKET SocketAPI::socket_ex(I32 domain, I32 type, I32 protocol) {
	SOCKET s = ::socket(domain, type, protocol);
	if (s == INVALID_SOCKET) {
		switch (errno) {
		case EPROTONOSUPPORT:
		case EMFILE:
		case ENFILE:
		case EACCES:
		case ENOBUFS:
		default: {
			break;
		}
		}	//end of switch
	}

	return s;
}

//////////////////////////////////////////////////////////////////////
//
// void SocketAPI::bind_ex ( SOCKET s , const struct sockaddr * addr , UI32 addrlen )
//
// exception version of bind()
//
// Parameters
//     s       - socket descriptor
//     addr    - socket address structure ( normally struct sockaddr_in )
//     addrlen - length of socket address structure
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::bind_ex(SOCKET s, const struct sockaddr * addr, UI32 addrlen) {
	if (::bind(s, addr, addrlen) == SOCKET_ERROR) {
		switch (errno) {
		case EADDRINUSE:
		case EINVAL:
		case EACCES:
		case ENOTSOCK:
		case EBADF:
		case EROFS:
		case EFAULT:
		case ENAMETOOLONG:
		case ENOENT:
		case ENOMEM:
		case ENOTDIR:
		case ELOOP:
		default: {
			break;
		}
		}	//end of switch

		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// void SocketAPI::connect_ex ( SOCKET s , const struct sockaddr * addr , UI32 addrlen )
//
// exception version of connect() system call
//
// Parameters
//     s       - socket descriptor
//     addr    - socket address structure
//     addrlen - length of socket address structure
//
// Return
//     none
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::connect_ex(SOCKET s, const struct sockaddr * addr,
		UI32 addrlen) {
	if (::connect(s, addr, addrlen) == SOCKET_ERROR) {
		if(EISCONN == errno) {
			// 已经连接
			return TRUE;
		}

		switch (errno) {
		case EINPROGRESS:
			if (getsocketnonblocking_ex(s)) {
				return TRUE;
			}
			break;
		case EHOSTUNREACH:
		case EALREADY:
		case ECONNREFUSED:
		case ETIMEDOUT:
		case ENETUNREACH:
		case EADDRINUSE:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
		default: {
			break;
		}
		}	//end of switch
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// void SocketAPI::listen_ex ( SOCKET s , UI32 backlog )
//
// exception version of listen()
//
// Parameters
//     s       - socket descriptor
//     backlog - waiting queue length
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::listen_ex(SOCKET s, UI32 backlog) {
	if (::listen(s, backlog) == SOCKET_ERROR) {
		switch (errno) {
		case EBADF:
		case ENOTSOCK:
		case EOPNOTSUPP:
		default: {
			break;
		}
		}	//end of switch
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//SOCKET SocketAPI::accept_ex ( SOCKET s , struct sockaddr * addr , UI32 * addrlen )
//
// exception version of accept()
//
// Parameters
//     s       - socket descriptor
//     addr    - socket address structure
//     addrlen - length of socket address structure
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
SOCKET SocketAPI::accept_ex(SOCKET s, struct sockaddr * addr, UI32 * addrlen) {
	SOCKET client = ::accept(s, addr, (socklen_t*) addrlen);

	if (client == INVALID_SOCKET) {
		switch (errno) {

		case EWOULDBLOCK:
			strncpy(Error, "EWOULDBLOCK", _ESIZE);
			break;
		case ECONNRESET:
			strncpy(Error, "ECONNRESET", _ESIZE);
			break;
		case ECONNABORTED:
			strncpy(Error, "ECONNABORTED", _ESIZE);
			break;
		case EPROTO:
			strncpy(Error, "EPROTO", _ESIZE);
			break;
		case EINTR:
			// from UNIX Network Programming 2nd, 15.6
			// with nonblocking-socket, ignore above errors
			strncpy(Error, "EINTR", _ESIZE);
			break;
		case EBADF:
			strncpy(Error, "EBADF", _ESIZE);
			break;
		case ENOTSOCK:
			strncpy(Error, "ENOTSOCK", _ESIZE);
			break;
		case EOPNOTSUPP:
			strncpy(Error, "EOPNOTSUPP", _ESIZE);
			break;
		case EFAULT:
			strncpy(Error, "EFAULT", _ESIZE);
			break;
		default: {
			memset(Error, 0, _ESIZE);
			sprintf(Error, "Error: %d", errno);
			break;
		}
		}				//end of switch
	} else {
	}

	return client;
}

//////////////////////////////////////////////////////////////////////
//
// void SocketAPI::getsockopt_ex ( SOCKET s , I32 level , I32 optname , void * optval , UI32 * optlen )
//
// exception version of getsockopt()
//
// Parameters
//     s       - socket descriptor
//     level   - socket option level ( SOL_SOCKET , ... )
//     optname - socket option name ( SO_REUSEADDR , SO_LINGER , ... )
//     optval  - poI32er to contain option value
//     optlen  - length of optval
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::getsockopt_ex(SOCKET s, I32 level, I32 optname, void * optval,
		UI32 * optlen) {
	if (::getsockopt(s, level, optname, optval,
			(socklen_t*) optlen) == SOCKET_ERROR) {
		switch (errno) {
		case EBADF:
		case ENOTSOCK:
		case ENOPROTOOPT:
		case EFAULT:
		default: {
			break;
		}
		}	//end of switch

		return FALSE;
	}

	return TRUE;
}

I32 SocketAPI::getsockopt_ex2(SOCKET s, I32 level, I32 optname, void * optval,
		UI32 * optlen) {
	if (::getsockopt(s, level, optname, optval,
			(socklen_t*) optlen) == SOCKET_ERROR) {
		switch (errno) {
		case EBADF:
			return 1;
		case ENOTSOCK:
			return 2;
		case ENOPROTOOPT:
			return 3;
		case EFAULT:
			return 4;
		default:
			return 5;
		}	//end of switch
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
//
// void SocketAPI::setsockopt_ex ( SOCKET s , I32 level , I32 optname , const void * optval , UI32 optlen )
//
// exception version of setsockopt()
//
// Parameters
//     s       - socket descriptor
//     level   - socket option level ( SOL_SOCKET , ... )
//     optname - socket option name ( SO_REUSEADDR , SO_LINGER , ... )
//     optval  - poI32er to contain option value
//     optlen  - length of optval
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::setsockopt_ex(SOCKET s, I32 level, I32 optname,
		const void * optval, UI32 optlen) {
	if (::setsockopt(s, level, optname, optval, optlen) == SOCKET_ERROR) {
		switch (errno) {
		case EBADF:
		case ENOTSOCK:
		case ENOPROTOOPT:
		case EFAULT:
		default: {
			break;
		}
		}	//end of switch

		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// UI32 SocketAPI::send_ex ( SOCKET s , const void * buf , UI32 len , UI32 flags )
//
// exception version of send()
//
// Parameters
//     s     - socket descriptor
//     buf   - input buffer
//     len   - input data length
//     flags - send flag (MSG_OOB,MSG_DONTROUTE)
//
// Return
//     length of bytes sent
//
//
//////////////////////////////////////////////////////////////////////
I32 SocketAPI::send_ex(SOCKET s, const void * buf, UI32 len, I32 flags) {
	I32	nSent = ::send(s, buf, len, flags);
	if (nSent == SOCKET_ERROR) {
		switch (errno) {
		case EWOULDBLOCK:
			return SOCKET_ERROR_WOULDBLOCK;

		case ECONNRESET:
		case EPIPE:
		case EBADF:
		case ENOTSOCK:
		case EFAULT:
		case EMSGSIZE:
		case ENOBUFS:
		default:
			break;
		}	//end of switch
	} else if (nSent == 0) {
		// Do Nothing...
	}

	return nSent;
}

//////////////////////////////////////////////////////////////////////
// exception version of sendto()
//////////////////////////////////////////////////////////////////////
I32 SocketAPI::sendto_ex(SOCKET s, const void * buf, I32 len, I32 flags,
		const struct sockaddr * to, I32 tolen) {
	I32	nSent = ::sendto(s, buf, len, flags, to, tolen);
	if (nSent == SOCKET_ERROR) {
		switch (errno) {
		case EWOULDBLOCK:
			return 0;
		case ECONNRESET:
		case EPIPE:
		case EBADF:
		case ENOTSOCK:
		case EFAULT:
		case EMSGSIZE:
		case ENOBUFS:
		default:
			break;
		}
	}

	return nSent;
}

//////////////////////////////////////////////////////////////////////
//
// UI32 SocketAPI::recv_ex ( SOCKET s , void * buf , UI32 len , UI32 flags )
//
// exception version of recv()
//
// Parameters
//     s     - socket descriptor
//     buf   - input buffer
//     len   - input data length
//     flags - send flag (MSG_OOB,MSG_DONTROUTE)
//
// Return
//     length of bytes received
//
//
//////////////////////////////////////////////////////////////////////
I32 SocketAPI::recv_ex(SOCKET s, void * buf, UI32 len, I32 flags) {
	I32 nrecv = ::recv(s, buf, len, flags);
	if (nrecv == SOCKET_ERROR) {
		switch (errno) {
		case EWOULDBLOCK:
			return SOCKET_ERROR_WOULDBLOCK;

		case ECONNRESET:
		case EPIPE:

		case EBADF:
		case ENOTCONN:
		case ENOTSOCK:
		case EINTR:
		case EFAULT:

		default: {
			break;
		}
		}	//end of switch
	} else if (nrecv == 0) {
		// Do Nothing ...
	}

	return nrecv;
}

/////////////////////////////////////////////////////////////////////
// exception version of recvfrom()
/////////////////////////////////////////////////////////////////////
I32 SocketAPI::recvfrom_ex(SOCKET s, void * buf, I32 len, I32 flags,
		struct sockaddr * from, UI32 * fromlen) {
	I32 nReceived = ::recvfrom(s, buf, len, flags, from, (socklen_t*) fromlen);
	if (nReceived == SOCKET_ERROR) {
		switch (errno) {
		case EWOULDBLOCK:
			return SOCKET_ERROR_WOULDBLOCK;

		case ECONNRESET:
		case EPIPE:

		case EBADF:
		case ENOTCONN:
		case ENOTSOCK:
		case EINTR:
		case EFAULT:

		default: {
			break;
		}
		}	//end of switch
	}

	return nReceived;
}

/////////////////////////////////////////////////////////////////////
//
// void SocketAPI::closesocket_ex ( SOCKET s )
//
// exception version of closesocket()
//
// Parameters
//     s - socket descriptor
//
// Return
//     none
//
//
/////////////////////////////////////////////////////////////////////
BOOL SocketAPI::closesocket_ex(SOCKET s) {
	FileAPI::close_ex(s);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// BOOL SocketAPI::getsocketnonblocking_ex ( SOCKET s )
//
// check if this socket is nonblocking mode
//
// Parameters
//     s - socket descriptor
//
// Return
//     TRUE if nonblocking, FALSE if blocking
//
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::getsocketnonblocking_ex(SOCKET s) {
	return FileAPI::getfilenonblocking_ex(s);
}

//////////////////////////////////////////////////////////////////////
//
// void SocketAPI::setsocketnonblocking_ex ( SOCKET s , BOOL on )
//
// make this socket blocking/nonblocking
//
// Parameters
//     s  - socket descriptor
//     on - TRUE if nonblocking, FALSE if blocking
//
// Return
//     none
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::setsocketnonblocking_ex(SOCKET s, BOOL on) {
	FileAPI::setfilenonblocking_ex(s, on);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// UI32 SocketAPI::availablesocket_ex ( SOCKET s )
//
// get amount of data in socket input buffer
//
// Parameters
//    s - socket descriptor
//
// Return
//    amount of data in socket input buffer
//
//
//////////////////////////////////////////////////////////////////////
I32 SocketAPI::availablesocket_ex(SOCKET s) {
	return availablefile_ex(s);
}

//////////////////////////////////////////////////////////////////////
//
// void SocketAPI::shutdown_ex ( SOCKET s , UI32 how )
//
// shutdown all or part of connection of socket
//
// Parameters
//     s   - socket descriptor
//     how - how to close ( all , send , receive )
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
BOOL SocketAPI::shutdown_ex(SOCKET s, UI32 how) {
	if (::shutdown(s, how) < 0) {
		switch (errno) {
		case EBADF:
		case ENOTSOCK:
		case ENOTCONN:
		default: {
			break;
		}
		}
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
// I32 SocketAPI::select_ex ( I32 maxfdp1 , fd_set * readset , fd_set * writeset , fd_set * exceptset , struct timeval * timeout )
//
// system call for I/O multiplexing
//
// Parameters
//     maxfdp1   -
//     readset   -
//     writeset  -
//     exceptset -
//     timeout   -
//
// Return
//     positive count of ready descriptors
//
//
//////////////////////////////////////////////////////////////////////
I32 SocketAPI::select_ex(I32 maxfdp1, fd_set * readset, fd_set * writeset,
		fd_set * exceptset, struct timeval * timeout) {
	return ::select(maxfdp1, readset, writeset, exceptset, timeout);
}

struct hostent* SocketAPI::gethostbyname_ex(const I8* host) {
	if (NULLPTR == host) {
		return NULLPTR;
	}
	struct hostent* pRet = ::gethostbyname(host);
	return pRet;
}

}

