/*
 * EpollMode.cpp
 *
 *  Created on: 2016年6月6日
 *      Author: 58
 */

#include <wedis/base/net/EpollMode.h>
#include <wedis/base/util/Alloctor.h>
#include <wedis/base/util/Log.h>
#include <wedis/base/util/FileAPI.h>
#include <wedis/base/util/System.h>
#include <errno.h>
#include <string.h>  // for memset
#include <fcntl.h>	 // for fcntl()

namespace wRedis {

EpollMode::EpollMode(I32 initSize, I32 maxSize)
	: EventMode(EM_EVENT_EPOLL, initSize, maxSize) {
	m_Epfd = INVALID_VALUE;
	m_aEpollEvents = NULLPTR;
}

EpollMode::~EpollMode() {
	if(0 <= m_Epfd) {
		FileAPI::close_ex(m_Epfd);
		m_Epfd = INVALID_VALUE;
	}

	W_FREE(m_aEpollEvents);
}

BOOL EpollMode::__dealErr(I32 op, I32 fd, struct epoll_event* pEe) {
	switch (op) {
	case EPOLL_CTL_MOD:
		if (errno == ENOENT) {
			/* If a MOD operation fails with ENOENT, the
			 * fd was probably closed and re-opened.  We
			 * should retry the operation as an ADD.
			 */
			if (W_UNLIKELY(epoll_ctl(m_Epfd, EPOLL_CTL_ADD, fd, pEe) == -1)) {
				Logger.warn("Epoll MOD(%d) on %d retried as ADD; that failed too", (I32)pEe->events, fd);
				return FALSE;
			} else {
				Logger.debug("Epoll MOD(%d) on %d retried as ADD; succeeded.", (I32)pEe->events, fd);
				return TRUE;
			}
		}
		break;
	case EPOLL_CTL_ADD:
		if (errno == EEXIST) {
			/* If an ADD operation fails with EEXIST,
			 * either the operation was redundant (as with a
			 * precautionary add), or we ran into a fun
			 * kernel bug where using dup*() to duplicate the
			 * same file into the same fd gives you the same epitem
			 * rather than a fresh one.  For the second case,
			 * we must retry with MOD. */
			if (W_UNLIKELY(epoll_ctl(m_Epfd, EPOLL_CTL_MOD, fd, pEe) == -1)) {
				Logger.warn("Epoll ADD(%d) on %d retried as MOD; that failed too", (I32)pEe->events, fd);
				return FALSE;
			} else {
				Logger.debug("Epoll ADD(%d) on %d retried as MOD; succeeded.", (I32)pEe->events, fd);
				return TRUE;
			}
		}
		break;
	case EPOLL_CTL_DEL:
		if (errno == ENOENT || errno == EBADF || errno == EPERM) {
			/* If a delete fails with one of these errors,
			 * that's fine too: we closed the fd before we
			 * got around to calling epoll_dispatch. */
			Logger.debug("Epoll DEL(%d) on fd %d gave %s: DEL was unnecessary.", op, fd, strerror(errno));
			return TRUE;
		}
		break;
	default:
		break;
	}

	return FALSE;
}

BOOL EpollMode::add(Event* e, I16 ev) {
	// 1. 参数检查
	AssertRetFalse(e);
	I32 fd = e->getFd();
	AssertRetFalse(fd < m_nEventSize);
	AssertRetFalse(fd >= 0);
	AssertRetFalse(m_aEvents[fd] == NULLPTR);
	AssertRetFalse(0 == e->m_Mask);

	// 2. wrapper event
	m_aEvents[fd] = e;
	e->m_Mask = ev;

	// 3. set new events
	struct epoll_event ee;
	ee.events = 0;
	ee.data.u64 = 0;  /* avoid valgrind warning */
	ee.data.fd = fd;
	ee.events |= EPOLLET;

	if(EM_EV_OP_READ & ev)
		ee.events |= EPOLLIN;

	if(EM_EV_OP_WRITE & ev)
		ee.events |= EPOLLOUT;

	if(W_LIKELY(0 == epoll_ctl(m_Epfd, EPOLL_CTL_ADD, fd, &ee))) {
		return TRUE;
	}

	return __dealErr(EPOLL_CTL_ADD, fd, &ee);
}

BOOL EpollMode::del(I32 fd) {
	AssertRetFalse(fd < m_nEventSize);
	AssertRetFalse(fd >= 0);

	if(m_aEvents[fd] == NULLPTR) {
		Logger.error("EpollMode::del fd is not by managerd!!!");
		return FALSE;
	}

	BOOL bRet = TRUE;
	if(W_UNLIKELY(0 > epoll_ctl(m_Epfd, EPOLL_CTL_DEL, fd, NULLPTR))) {
		bRet = __dealErr(EPOLL_CTL_DEL, fd, NULLPTR);
	}

	if(bRet) {
		m_aEvents[fd]->m_Mask = 0;
		m_aEvents[fd] = NULLPTR;
	}

	return bRet;
}

BOOL EpollMode::ctl(I32 fd, I16 ev) {
	AssertRetFalse(fd < m_nEventSize);
	AssertRetFalse(fd >= 0);

	if(NULLPTR == m_aEvents[fd]) {
		Logger.error("EpollMode::ctl fd is not by managerd!!!");
		return FALSE;
	}

	if(EM_EV_OP_NONE == ev) {
		return del(fd);
	}
	else if(m_aEvents[fd]->m_Mask == ev) {
		return TRUE;
	}

	struct epoll_event ee;
	ee.events = 0;
	ee.data.u64 = 0; /* avoid valgrind warning */
	ee.data.fd = fd;
	ee.events |= EPOLLET;
	if(EM_EV_OP_READ & ev)
		ee.events |= EPOLLIN;

	if(EM_EV_OP_WRITE & ev)
		ee.events |= EPOLLOUT;

	m_aEvents[fd]->m_Mask = ev;
	if(0 == epoll_ctl(m_Epfd, EPOLL_CTL_MOD, fd, &ee)) {
		return TRUE;
	}

	return __dealErr(EPOLL_CTL_MOD, fd, &ee);
}

BOOL EpollMode::create() {
#ifdef EVENT_HAVE_EPOLL_CREATE1
	// First, try the shiny new epoll_create1 interface, if we have it.
	m_Epfd = epoll_create1(EPOLL_CLOEXEC);
#endif
	if (INVALID_VALUE == m_Epfd) {
		/* Initialize the kernel queue using the old interface.  (The
		   size field is ignored   since 2.6.8.) */
		if ((m_Epfd = epoll_create(m_nMaxEventSize)) == INVALID_VALUE) {
			if (errno != ENOSYS)
				Logger.error("epoll create failed. errno:%d, errstr:%s", errno, strerror(errno));

			return FALSE;
		}

		I32 flags = FileAPI::fcntl_ex(m_Epfd, F_GETFD, NULLPTR);
		if(W_UNLIKELY(flags < 0)) {
			Logger.warn("EpollMode::create flags fcntl F_GETFD < 0");
		}
		else if(W_UNLIKELY(FileAPI::fcntl_ex(m_Epfd, F_SETFD, flags | FD_CLOEXEC) == -1)) {
			Logger.warn("EpollMode::create flags fcntl F_SETFD < 0");
		}
	}

	// Initialize fields
	m_aEpollEvents = (struct epoll_event*) W_CALLOC(m_nEventSize, sizeof(struct epoll_event));
	if (NULLPTR == m_aEpollEvents) {
		FileAPI::close_ex(m_Epfd);
		return FALSE;
	}

	// create base class
	if(FALSE == EventMode::create()) {
		W_FREE(m_aEpollEvents);
		FileAPI::close_ex(m_Epfd);
		return FALSE;
	}

//	#ifdef USING_TIMERFD
//		/*
//		  The epoll interface ordinarily gives us one-millisecond precision,
//		  so on Linux it makes perfect sense to use the CLOCK_MONOTONIC_COARSE
//		  timer.  But when the user has set the new PRECISE_TIMER flag for an
//		  event_base, we can try to use timerfd to give them finer granularity.
//		*/
//		if ((base->flags & EVENT_BASE_FLAG_PRECISE_TIMER) &&
//		    base->monotonic_timer.monotonic_clock == CLOCK_MONOTONIC) {
//			int fd;
//			fd = epollop->timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
//			if (epollop->timerfd >= 0) {
//				struct epoll_event epev;
//				memset(&epev, 0, sizeof(epev));
//				epev.data.fd = epollop->timerfd;
//				epev.events = EPOLLIN;
//				if (epoll_ctl(epollop->epfd, EPOLL_CTL_ADD, fd, &epev) < 0) {
//					event_warn("epoll_ctl(timerfd)");
//					close(fd);
//					epollop->timerfd = -1;
//				}
//			} else {
//				if (errno != EINVAL && errno != ENOSYS) {
//					/* These errors probably mean that we were
//					 * compiled with timerfd/TFD_* support, but
//					 * we're running on a kernel that lacks those.
//					 */
//					event_warn("timerfd_create");
//				}
//				epollop->timerfd = -1;
//			}
//		} else {
//			epollop->timerfd = -1;
//		}
//	#endif
//
//		evsig_init_(base);

	return TRUE;
}

BOOL EpollMode::dispatch(I64 timeOut) {
	I32 res = epoll_wait(m_Epfd, m_aEpollEvents, m_nEventSize, timeOut);
	if(W_UNLIKELY(-1 == res)) {
		if(EINTR == errno) {
			Logger.warn("EpollMode::dispatch Interrupt!!!");
			return TRUE;
		}

		return FALSE;
	}

	Assert(res <= m_nEventSize);
	for (I32 i = 0; i < res; i++) {
		I32 what = m_aEpollEvents[i].events;
		I16 ev = 0;
		// 出错了
		if (what & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
			ev = EM_EV_RESULT_ERR;
		} else {
			// 可读
			if (what & EPOLLIN)
				ev |= EM_EV_RESULT_READ;

			// 可写
			if (what & EPOLLOUT)
				ev |= EM_EV_RESULT_WRITE;
		}

		if (!ev)
			continue;

		Event* e = m_aEvents[m_aEpollEvents[i].data.fd];
		if(NULLPTR == e) {
			Logger.error("m_aEvents is nil, fd:%d", m_aEpollEvents[i].data.fd);
			continue;
		}

		e->proc(ev);
	}

	// 扩容
	if (res == m_nEventSize && m_nEventSize < m_nMaxEventSize) {
		/* We used all of the event space this time.  We should
		   be ready for more events next time. */
		I32 newEventSize = m_nEventSize << 1;
		struct epoll_event *pNewEpollEvents = (struct epoll_event*) W_REALLOC(m_aEpollEvents, newEventSize * sizeof(struct epoll_event));
		if (W_LIKELY(pNewEpollEvents)) {
			m_aEpollEvents = pNewEpollEvents;
			m_nEventSize = newEventSize;
		}

		struct Event** pNewEvents = (Event**) W_REALLOC(m_aEvents, newEventSize * sizeof(Event*));
		if (W_LIKELY(pNewEvents)) {
			m_aEvents = pNewEvents;
		}
	}

	return TRUE;
}



} /* namespace wRedis */
