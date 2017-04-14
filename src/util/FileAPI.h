#ifndef __LISBASE_FILE_API_H__
#define __LISBASE_FILE_API_H__

#include "CommonType.h"

namespace lisbase {
namespace FileAPI {

//
// exception version of open ()
//
I32 open_ex(const I8* filename, I32 flags);

I32 open_ex(const I8* filename, I32 flags, I32 mode);

//
// exception version of close ()
//
void close_ex(I32 fd);

//
// exception version of read ()
//
UI32 read_ex(I32 fd, void* buf, UI32 len);

//
// exception version of write ()
//
UI32 write_ex(I32 fd, const void* buf, UI32 len);

//
// exception version of fcntl ()
//
I32 fcntl_ex(I32 fd, I32 cmd);

//
// exception version of fcntl ()
//
I32 fcntl_ex(I32 fd, I32 cmd, long arg);

//
// is this stream is nonblocking?
//
// using fcntl_ex()
//
BOOL getfilenonblocking_ex(I32 fd);

//
// make this strema blocking/nonblocking
//
// using fcntl_ex()
//
void setfilenonblocking_ex(I32 fd, BOOL on);

//
// exception version of ioctl ()
//
BOOL ioctl_ex(I32 fd, I32 request, void* argp);

//
// make this stream blocking/nonblocking
//
// using ioctl_ex()
//
void setfilenonblocking_ex2(I32 fd, BOOL on);

//
// how much bytes available in this stream?
//
// using ioctl_ex()
//
UI32 availablefile_ex(I32 fd);

//
// exception version of dup()
//
I32 dup_ex(I32 fd);

//
// exception version of lseek()
//
long lseek_ex(I32 fd, long offset, I32 whence);
}
;
//end of namespace FileAPI
}

#endif

