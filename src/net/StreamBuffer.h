#ifndef _StreamBuffer_H_
#define _StreamBuffer_H_

#include <wedis/base/net/Socket.h>
#include <wedis/base/util/CommonType.h>

#include <string>

namespace wRedis {
class Socket;
const I32 UNIT_LENGTH = 1024*32;

class OutputStream
{
	enum
	{
		DEFAULT_BUFF_SIZE  = UNIT_LENGTH*2,
		MAX_BUFF_SIZE	   = UNIT_LENGTH*3,
		BUFF_SIZE_INC_STEP = UNIT_LENGTH,
	};
public:
	OutputStream(Socket* pSocket, I32 iLen = DEFAULT_BUFF_SIZE, I32 iMaxLen = MAX_BUFF_SIZE);
	~OutputStream();
public:
	I32				write(const I8* pBuf, I32 iLen);
	I32				writeInHead(const I8* pBuf, I32 iLen);
	BOOL            move(I32 iLen);
	void 			skip(I32 iLen);
	void            revert(I32 iLen);
	I32				flush();
	void			setBuffMaxSize(I32 iSize);
	I32				getBuffMaxSize() const;
	I32				getBuffUsedSize() const;
	BOOL			setBuffDefaultSize(I32 iSize);
	void			reset();
	I32				getBuffSize() const;
	I8*             getHead();
protected:
	BOOL			__resize(I32 newSize);
	I32				__getUsedCount() const;
	I32				__getFreeCount() const;
private:
	I8*				m_pBuf;
	I32				m_iLen;
	I32				m_iMaxLen;
	I32				m_iHead;
	I32				m_iTail;
	Socket*	m_pSocket;
};

class InputStream
{
	enum
	{
		DEFAULT_BUFF_SIZE  = UNIT_LENGTH,
		MAX_BUFF_SIZE	   = UNIT_LENGTH*32,
		BUFF_SIZE_INC_STEP = UNIT_LENGTH,
		SINGLE_RECV_LEN	   = 1024,
	};
public:
	InputStream(Socket* pSocket, I32 iLen = DEFAULT_BUFF_SIZE, I32 iMaxLen = MAX_BUFF_SIZE);
	~InputStream();
public:
	void			reset();
	I32				read(I8* pBuf, I32 iLen);
	I8 				readByte();
	I8              getByte(I32 offset);
	I32   			readLine(I32* pLen);
	I32				fill();
	BOOL			peek(I8* pBuf, I32 iLen) const;
	BOOL			checkSize(I32 iCheckSize) const;
	void			skip(I32 iOffset);
	void			setBuffMaxSize(I32 iSize);
	I32				getBuffMaxSize() const;
	I32				getBuffUsedSize() const;
	BOOL			setBuffDefaultSize(I32 iSize);
	I32				GetBuffSize() const;
	I32				find(const I8 *context, I32 iLen);
	I8*             getHead();
protected:
	BOOL			__resize(I32 newSize);
	I32				__getUsedCount() const;
	I32				__getFreeCount() const;
private:
	I8*				m_pBuf;
	I32				m_iLen;
	I32				m_iMaxLen;
	I32				m_iHead;
	I32				m_iTail;
	Socket*	m_pSocket;
};

}

#endif

