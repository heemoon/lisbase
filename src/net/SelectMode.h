/*
 * SelectMode.h
 *
 *  Created on: 2016年6月6日
 *      Author: 58
 */

#ifndef WEDIS_BASE_NET_SELECTMODE_H_
#define WEDIS_BASE_NET_SELECTMODE_H_

#include <wedis/base/net/EventMode.h>
#include <sys/select.h>

namespace wRedis {

typedef fd_set			FDSET;
typedef fd_mask			FDMASK;

//UBI_VOID UBI_CSocketAPI::FdZero(UBI_FDSET& rfdSet)
//{
//#if defined (_WIN32_)
//	FD_ZERO(&rfdSet);
//#elif defined (_LINUX64_)
//	/*
//	 *	#include <sys/select.h>
//         *	void FD_ZERO(fd_set *set);
//	 */
//	FD_ZERO(&rfdSet);
//#endif
//}
//
//UBI_VOID UBI_CSocketAPI::FdClear(UBI_SOCKET fd, UBI_FDSET& rfdSet)
//{
//#if defined (_WIN32_)
//	FD_CLR(fd, &rfdSet);
//#elif defined (_LINUX64_)
//	/*
//	 *	#include <sys/select.h>
//         *	void FD_CLR(int fd, fd_set *set);
//	 */
//	FD_CLR(fd, &rfdSet);
//#endif
//}
//
//UBI_VOID UBI_CSocketAPI::FdSet(UBI_SOCKET fd, UBI_FDSET& rfdSet)
//{
//#if defined (_WIN32_)
//	FD_SET(fd, &rfdSet);
//#elif defined (_LINUX64_)
//	/*
//	 *	#include <sys/select.h>
//         *	void FD_SET(int fd, fd_set *set);
//	 */
//	FD_SET(fd, &rfdSet);
//#endif
//}
//
//UBI_BOOL UBI_CSocketAPI::FdCheck(UBI_SOCKET fd, const UBI_FDSET& rfdSet)
//{
//#if defined (_WIN32_)
//	return FD_ISSET(fd, &rfdSet);
//#elif defined (_LINUX64_)
//	/*
//	 *	#include <sys/select.h>
//         *	int FD_ISSET(int fd, fd_set *set);
//	 */
//	return FD_ISSET(fd, &rfdSet);
//#endif
//}

class SelectMode : public EventMode {
public:
	SelectMode();
	virtual ~SelectMode();
public:
	virtual BOOL create();
	virtual BOOL add(Event* e, I16 ev);
	virtual BOOL del(I32 fd);
	virtual BOOL ctl(I32 fd, I16 ev);
	virtual BOOL dispatch(I64 timeOut);
private:
	static void __zero(FDSET& rFdSet);
	static void __clear(SOCKET fd, FDSET& rFdSet);
	static void __set(SOCKET fd, FDSET& rFdSet);
	static BOOL __check(SOCKET fd, FDSET& rFdSet);
private:
	// Select读套接字集合
	FDSET				m_SelReadSet;
	// Select写套接字集合
	FDSET				m_SelWriteSet;
	// Select错误套接字集合
	FDSET				m_SelErrorSet;
	// 原始套接字集合
	FDSET				m_RawSet;
	// Select时间片
	TIMET				m_TimeOut;
	// 保存所有已连接的原始套接字
	SOCKET				m_arrayConnect[FD_SETSIZE];
	// 原始套接字集合最大句柄值
	SOCKET				m_MaxHandle;
	// 原始套接字集合数组已使用的最大索引值
	I32					m_nMaxIndex;
};

} /* namespace wRedis */

#endif /* WEDIS_BASE_NET_SELECTMODE_H_ */
