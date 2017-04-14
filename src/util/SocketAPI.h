#ifndef __SOCKET_API_H__
#define __SOCKET_API_H__

// include files
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>     // for struct hostent
#include <sys/param.h>
#include <sys/ioctl.h>
#include <wedis/base/util/CommonType.h>

namespace wRedis {

typedef socklen_t SOCKETLEN_T;
typedef I32 SOCKET;
#define _ESIZE 256
#define SHUTDOWN_BOTH	SHUT_RDWR
#define EWOULD_BLOCK	EWOULDBLOCK
#define NO_BUFFERS		ENOBUFS
#define INVALID_SOCKET  -1
#define	SOCKET_ERROR	-1

static const I32 SOCKET_ERROR_WOULDBLOCK = -100;

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
static const UI32 szSOCKADDR_IN = sizeof(SOCKADDR_IN);

//////////////////////////////////////////////////////////////////////
//
// Platform Independent Socket API Collection (exception based)
//
//////////////////////////////////////////////////////////////////////


namespace SocketAPI {

//
// exception version of socket ()
//
SOCKET socket_ex(I32 domain, I32 type, I32 protocol);

//
// exception version of bind ()
//
BOOL bind_ex(SOCKET s, const struct sockaddr* name, UI32 namelen);

//
// exception version of connect ()
//
BOOL connect_ex(SOCKET s, const struct sockaddr* name, UI32 namelen);

//
// exception version of listen ()
//
BOOL listen_ex(SOCKET s, UI32 backlog);

//
// exception version of accept ()
//
SOCKET accept_ex(SOCKET s, struct sockaddr* addr, UI32* addrlen);

//
// exception version of getsockopt ()
//
BOOL getsockopt_ex(SOCKET s, I32 level, I32 optname, void* optval,
		UI32* optlen);

I32 getsockopt_ex2(SOCKET s, I32 level, I32 optname, void* optval,
		UI32* optlen);

//
// exception version of setsockopt ()
//
BOOL setsockopt_ex(SOCKET s, I32 level, I32 optname, const void* optval,
		UI32 optlen);

//
// exception version of send()
//
I32 send_ex(SOCKET s, const void* buf, UI32 len, I32 flags);

//
// exception version of sendto()
//
I32 sendto_ex(SOCKET s, const void* buf, I32 len, I32 flags,
		const struct sockaddr* to, I32 tolen);

//
// exception version of recv()
//
I32 recv_ex(SOCKET s, void* buf, UI32 len, I32 flags);

//
// exception version of recvfrom()
//
I32 recvfrom_ex(SOCKET s, void* buf, I32 len, I32 flags, struct sockaddr* from,
		UI32* fromlen);

//
// exception version of closesocket()
//
// *CAUTION*
//
// in UNIX, close() used instead
//
BOOL closesocket_ex(SOCKET s);

//
// check if socket is nonblocking mode
//
BOOL getsocketnonblocking_ex(SOCKET s);

//
// make socket nonblocking mode
//
BOOL setsocketnonblocking_ex(SOCKET s, BOOL on);

//
// get amount of data in socket input buffer
//
I32 availablesocket_ex(SOCKET s);

//
// exception version of shutdown()
//
BOOL shutdown_ex(SOCKET s, UI32 how);

//
// exception version of select()
//
I32 select_ex(I32 maxfdp1, fd_set* readset, fd_set* writeset, fd_set* exceptset,
		struct timeval* timeout);

//
// get hostname
//
struct hostent* gethostbyname_ex(const I8* host);

}	//end of namespace
}

#endif
