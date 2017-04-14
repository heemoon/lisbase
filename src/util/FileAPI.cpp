#include <sys/types.h>	// for open()
#include <sys/stat.h>	// for open()
#include <unistd.h>		// for fcntl()
#include <fcntl.h>		// for fcntl()
#include <errno.h>		// for errno
#include <sys/ioctl.h>	// for ioctl()

#include <wedis/base/net/Socket.h>     // for FIONREAD
#include <wedis/base/util/FileAPI.h>
#include <wedis/base/util/Global.h>

namespace lisbase {

I32 FileAPI::open_ex(const I8 * filename, I32 flags) {

	I32 fd = open(filename, flags);
	if (fd < 0) {
		switch ( errno) {
		case EEXIST:
		case ENOENT:
		case EISDIR:
		case EACCES:
		case ENAMETOOLONG:
		case ENOTDIR:
		case ENXIO:
		case ENODEV:
		case EROFS:
		case EFAULT:
		case ENOSPC:
		case ENOMEM:
		case EMFILE:
		case ENFILE:
		default: {
			break;
		}
		} //end of switch
	}

	return fd;
}

I32 FileAPI::open_ex(const I8 * filename, I32 flags, I32 mode) {
	I32 fd = open(filename, flags, mode);
	if (fd < 0) {
		switch ( errno) {
		case EEXIST:
		case EISDIR:
		case EACCES:
		case ENAMETOOLONG:
		case ENOENT:
		case ENOTDIR:
		case ENXIO:
		case ENODEV:
		case EROFS:
		case ETXTBSY:
		case EFAULT:
		case ELOOP:
		case ENOSPC:
		case ENOMEM:
		case EMFILE:
		case ENFILE:
		default: {
			break;
		}
		}	//end of switch
	}

	return fd;
}

//////////////////////////////////////////////////////////////////////
//
// UI32 FileAPI::read_ex ( I32 fd , void * buf , UI32 len )
//
//
// exception version of read()
//
// Parameters
//     fd  - file descriptor
//     buf - reading buffer
//     len - reading length
//
// Return
//     length of reading bytes
//
//
//////////////////////////////////////////////////////////////////////
UI32 FileAPI::read_ex(I32 fd, void * buf, UI32 len) {
	I32 result = read(fd, buf, len);
	if (result < 0) {
		switch ( errno) {
		case EINTR:
		case EAGAIN:
		case EBADF:
		case EIO:
		case EISDIR:
		case EINVAL:
		case EFAULT:
		case ECONNRESET:
		default: {
			break;
		}
		}
	} else if (result == 0) {
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
// UI32 FileAPI::write_ex ( I32 fd , void * buf , UI32 len )
//
//
// exception version of write()
//
// Parameters
//     fd  - file descriptor
//     buf - writing buffer
//     len - writing length
//
// Return
//     length of reading bytes
//
//
//////////////////////////////////////////////////////////////////////
UI32 FileAPI::write_ex(I32 fd, const void * buf, UI32 len) {
	I32 result = write(fd, buf, len);
	if (result < 0) {
		switch ( errno) {
		case EAGAIN:
		case EINTR:
		case EBADF:
		case EPIPE:
		case EINVAL:
		case EFAULT:
		case ENOSPC:
		case EIO:
		case ECONNRESET:
		default: {
			break;
		}
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
// void FileAPI::close_ex ( I32 fd )
//
//
// exception version of close()
//
// Parameters
//     fd - file descriptor
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
void FileAPI::close_ex(I32 fd) {
	if (::close(fd) < 0) {
		switch ( errno) {
		case EBADF:
		default: {
			break;
		}
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
// I32 FileAPI::fcntl_ex ( I32 fd , I32 cmd )
//
//
// Parameters
//     fd  - file descriptor
//     cmd - file control command
//
// Return
//     various according to cmd
//
//
//////////////////////////////////////////////////////////////////////
I32 FileAPI::fcntl_ex(I32 fd, I32 cmd) {
	I32 result = fcntl(fd, cmd);
	if (result < 0) {
		switch ( errno) {
		case EINTR:
		case EBADF:
		case EACCES:
		case EAGAIN:
		case EDEADLK:
		case EMFILE:
		case ENOLCK:
		default: {
			break;
		}
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
//
// I32 FileAPI::fcntl_ex ( I32 fd , I32 cmd , I64 arg )
//
//
// Parameters
//     fd  - file descriptor
//     cmd - file control command
//     arg - command argument
//
// Return
//     various according to cmd
//
//
//////////////////////////////////////////////////////////////////////
I32 FileAPI::fcntl_ex(I32 fd, I32 cmd, long arg) {
	I32 result = fcntl(fd, cmd, arg);
	if (result < 0) {
		switch ( errno) {
		case EINTR:
		case EINVAL:
		case EBADF:
		case EACCES:
		case EAGAIN:
		case EDEADLK:
		case EMFILE:
		case ENOLCK:
		default: {
			break;
		}
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
//
// BOOL getfilenonblocking_ex ( I32 fd )
//
//
// check if this file is nonblocking mode
//
// Parameters
//     fd - file descriptor
//
// Return
//     TRUE if nonblocking, FALSE if blocking
//
//
//////////////////////////////////////////////////////////////////////
BOOL FileAPI::getfilenonblocking_ex(I32 fd) {
	I32 flags = fcntl_ex(fd, F_GETFL, 0);
	return flags & O_NONBLOCK;
}

//////////////////////////////////////////////////////////////////////
//
// void setfilenonblocking_ex ( I32 fd , BOOL on )
//
//
// make this file blocking/nonblocking
//
// Parameters
//     fd - file descriptor
//     on - TRUE if nonblocking, FALSE if blocking
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
void FileAPI::setfilenonblocking_ex(I32 fd, BOOL on) {
	I32 flags = fcntl_ex(fd, F_GETFL, 0);
	if (on)
		// make nonblocking fd
		flags |= O_NONBLOCK;
	else
		// make blocking fd
		flags &= ~O_NONBLOCK;

	fcntl_ex(fd, F_SETFL, flags);
}

//////////////////////////////////////////////////////////////////////
//
// void FileAPI::ioctl_ex ( I32 fd , I32 request , void * argp )
//
//
// exception version of ioctl()
//
// Parameters
//     fd      - file descriptor
//     request - i/o control request
//     argp    - argument
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
BOOL FileAPI::ioctl_ex(I32 fd, I32 request, void * argp) {
	if (ioctl(fd, request, argp) < 0) {
		switch ( errno) {
		case EBADF:
		case ENOTTY:
		case EINVAL:
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
// void FileAPI::setfilenonblocking_ex2 ( I32 fd , BOOL on )
//
//
// make this stream blocking/nonblocking using ioctl_ex()
//
// Parameters
//     fd - file descriptor
//     on - TRUE if nonblocking, FALSE else
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
void FileAPI::setfilenonblocking_ex2(I32 fd, BOOL on) {
	UI64 arg = (on == TRUE ? 1 : 0);
	ioctl_ex(fd, FIONBIO, &arg);
}

//////////////////////////////////////////////////////////////////////
//
// UI32 FileAPI::available_ex ( I32 fd )
//
//
// how much bytes available in this stream? using ioctl_ex()
//
// Parameters
//     fd - file descriptor
//
// Return
//     #bytes available
//
//
//////////////////////////////////////////////////////////////////////
UI32 FileAPI::availablefile_ex(I32 fd) {
	UI32 arg = 0;
	ioctl_ex(fd, FIONREAD, &arg);
	return arg;
}

//////////////////////////////////////////////////////////////////////
//
// I32 FileAPI::dup_ex ( I32 fd )
//
//
//////////////////////////////////////////////////////////////////////
I32 FileAPI::dup_ex(I32 fd) {
	I32 newfd = dup(fd);
	if (newfd < 0) {
		switch ( errno) {
		case EBADF:
		case EMFILE:
		default: {
			break;
		}
		}	//end of switch
	}

	return newfd;
}

//////////////////////////////////////////////////////////////////////
//
// long FileAPI::lseek_ex ( I32 fd , long offset , I32 whence )
//
//
//////////////////////////////////////////////////////////////////////
long FileAPI::lseek_ex(I32 fd, long offset, I32 whence) {
	long result = lseek(fd, offset, whence);
	if (result < 0) {
		switch ( errno) {
		case EBADF:
		case ESPIPE:
		case EINVAL:
		default:
			break;
		}
	}

	return result;
}


}

