
#ifndef _SocketDef_H_
#define _SocketDef_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/ioctl.h>

typedef struct sockaddr		SOCKADDR;
typedef struct sockaddr_in	SOCKADDR_IN;
typedef struct in_addr		UBI_INADDR;

typedef int				SOCKET_T;
typedef socklen_t		SOCKETLEN_T;
typedef fd_set			FDSET;
typedef fd_mask			FDMASK;
typedef timeval			TIMEVAL;
#define INVALID_SOCKET_VAL	-1
#define SOCKET_ERROR_VAL	-1
#define SHUTDOWN_BOTH	SHUT_RDWR
#define EWOULD_BLOCK	EWOULDBLOCK
#define NO_BUFFERS		ENOBUFS


#endif

