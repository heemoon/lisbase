#include <stdio.h>
#include <wedis/base/net/StreamBuffer.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/SocketAPI.h>

#include <string>

namespace wRedis {
#define MAX_FIND_LEN  655350
InputStream::InputStream(Socket* pSocket, I32 iLen/*=DEFAULT_BUFF_SIZE*/,
		I32 iMaxLen/*=MAX_BUFF_SIZE*/) {
	m_pSocket = pSocket;
	m_iLen = iLen;
	m_iMaxLen = iMaxLen;
	m_iHead = m_iTail = 0;
	m_pBuf = new I8[iLen];
	Assert(m_pBuf);
	memset(m_pBuf, 0, m_iLen);
}

InputStream::~InputStream() {
	if (m_pBuf != NULLPTR) {
		delete[] m_pBuf;
	}
}

I32 InputStream::__getUsedCount() const {
	if (m_iHead <= m_iTail) {
		return m_iTail - m_iHead;
	} else {
		return (m_iLen - m_iHead) + m_iTail;
	}
}

I32 InputStream::__getFreeCount() const {
	return m_iLen - __getUsedCount() - 1;
}

BOOL InputStream::__resize(I32 newSize) {
	I32 iStep = m_iLen * 2;
	//I32 iInc = (iNeed>BUFF_SIZE_INC_STEP ? iNeed : BUFF_SIZE_INC_STEP);
	while (newSize > iStep) {
		iStep += iStep;
	}
	//I32 iInc = iStep;
	I32 iNewSize = iStep;
	I8* pNewBuf = new I8[iNewSize];
	if (pNewBuf == NULLPTR)
		return FALSE;
	memset(pNewBuf, 0, iNewSize);

	if (m_iHead < m_iTail) {
		memcpy(pNewBuf, &m_pBuf[m_iHead], m_iTail - m_iHead);
	} else if (m_iHead > m_iTail) {
		memcpy(pNewBuf, &m_pBuf[m_iHead], m_iLen - m_iHead);
		memcpy(pNewBuf + m_iLen - m_iHead, m_pBuf, m_iTail);
	} else {
		//printf("Network Lib Error! Head = %d, Tail = %d\n", m_iHead, m_iTail);
	}

	delete[] m_pBuf;
	m_pBuf = pNewBuf;

	m_iTail = __getUsedCount();
	m_iHead = 0;
	m_iLen = iNewSize;

	return TRUE;
}
BOOL InputStream::setBuffDefaultSize(I32 iSize) {
	if (m_iLen >= iSize) {
		return TRUE;
	}
	return __resize(iSize);
}

void InputStream::reset() {
	memset(m_pBuf, 0, sizeof(I8) * m_iLen);
	m_iHead = 0;
	m_iTail = 0;
}

I32 InputStream::find(const I8 *context, I32 iLen) {
	if (NULLPTR == context) {
		return INVALID_VALUE;
	}

	I32 nUsedCount = __getUsedCount();
	if (iLen > nUsedCount) {
		return INVALID_VALUE;
	}

	I32 j = 0;
	I32 i = 0;
	for (; i < nUsedCount && j < iLen; ++i) {
		I8 c = m_pBuf[(i + m_iHead) % m_iLen];
		if (c == context[j]) {
			j++;
			if (j == iLen) {
				return i - iLen + 1;
			}
		} else {
			j = 0;
		}
	}

	return INVALID_VALUE;
}

I32 InputStream::read(I8* pBuf, I32 iLen) {
	// iLen 不能大于已读空间
	if (iLen > __getUsedCount()) {
		iLen = __getUsedCount();
	}

	// 正序
	if (m_iHead <= m_iTail) {
		memcpy(pBuf, &m_pBuf[m_iHead], iLen);
		m_iHead += iLen;
	} else {
		I32 iTailCount = m_iLen - m_iHead;
		if (iLen <= iTailCount) {
			memcpy(pBuf, &m_pBuf[m_iHead], iLen);
			m_iHead += iLen;
		} else {
			memcpy(pBuf, &m_pBuf[m_iHead], iTailCount);
			memcpy(pBuf + iTailCount, m_pBuf, iLen - iTailCount);
			m_iHead = iLen - iTailCount;
		}
	}

	return iLen;
}

I8 InputStream::readByte() {
	// iLen 不能大于已读空间
	if (0 == __getUsedCount())
		return INVALID_VALUE;

	I8 c = m_pBuf[m_iHead];
	skip(1);
	return c;
}

I8 InputStream::getByte(I32 offset) {
	if (0 == offset)
		return m_pBuf[m_iHead];

	I32 idx = (offset + m_iHead) % m_iLen;
	if (idx < 0)
		idx += m_iLen;

	return m_pBuf[idx];
}

I32 InputStream::fill() {
	I32 iFlag = 0;
	I32 uFilled = 0;
	I32 uReceived = 0;
	I32 uFree = 0;

	if (m_iHead <= m_iTail) {
		if (m_iHead == 0) {
			// H   T		LEN=10
			// 0123456789
			// abcd......
			uFree = m_iLen - m_iTail - 1;
			uReceived = 0;
			if (uFree > 0) {
				uReceived = m_pSocket->receive(&m_pBuf[m_iTail], uFree, iFlag);
				if (uReceived <= SOCKET_ERROR) {
					if (errno == EINTR
							|| (m_pSocket->isNonBlocking()
									&& (errno == EAGAIN || errno == EWOULDBLOCK))) {
						return uFilled;
					} else {
						m_pSocket->setStatusCode(Socket::RECV_FAIL);
						return -1;
					}
				}
				if (uReceived == 0) {
					m_pSocket->setStatusCode(Socket::RECV_FAIL);
					return -1;
				}

				m_iTail += uReceived;
				uFilled += uReceived;
			}

			if (uReceived == uFree) {
				UI32 uAvail = SocketAPI::availablesocket_ex(
						m_pSocket->getSOCKET());
				if (uAvail > 0) {
					//check uAvail
					if (I32(m_iLen + uAvail + 1) > m_iMaxLen) {
						//reset
						reset();
						m_pSocket->setStatusCode(Socket::INPUT_OVERFLOW);
						return -1;
					}

					//re size
					if (!__resize(I32(m_iLen + uAvail + 1))) {
						m_pSocket->setStatusCode(Socket::RECV_FAIL);
						return -1;
					}

					// recv
					uReceived = m_pSocket->receive(&m_pBuf[m_iTail], uAvail,
							iFlag);
					if (uReceived <= SOCKET_ERROR) {
						if (errno == EINTR
								|| (m_pSocket->isNonBlocking()
										&& (errno == EAGAIN
												|| errno == EWOULDBLOCK))) {
							return uFilled;
						} else {
							m_pSocket->setStatusCode(Socket::RECV_FAIL);
							return -1;
						}
					}
					if (uReceived == 0) {
						m_pSocket->setStatusCode(Socket::RECV_FAIL);
						return -1;
					}

					m_iTail += uReceived;
					uFilled += uReceived;
				}
			}
		}
		//m_iHead != 0
		else {
			//    H   T		LEN=10
			// 0123456789
			// ...abcd...
			uFree = m_iLen - m_iTail;
			uReceived = 0;
			if (uFree > 0) {
				uReceived = m_pSocket->receive(&m_pBuf[m_iTail], uFree, iFlag);
				if (uReceived <= SOCKET_ERROR) {
					if (errno == EINTR
							|| (m_pSocket->isNonBlocking()
									&& (errno == EAGAIN || errno == EWOULDBLOCK))) {
						return uFilled;
					} else {
						m_pSocket->setStatusCode(Socket::RECV_FAIL);
						return -1;
					}
				}
				if (uReceived == 0) {
					m_pSocket->setStatusCode(Socket::RECV_FAIL);
					return -1;
				}

				m_iTail = (m_iTail + uReceived) % m_iLen;
				uFilled += uReceived;
			}

			if (uReceived == uFree) {
				UI32 uAvail = SocketAPI::availablesocket_ex(
						m_pSocket->getSOCKET());
				if (uAvail > 0) {
					uFree = m_iHead - 1;
					uReceived = 0;
					if (uFree != 0) {
						uReceived = m_pSocket->receive(&m_pBuf[0], uFree,
								iFlag);
						if (uReceived <= SOCKET_ERROR) {
							if (errno == EINTR
									|| (m_pSocket->isNonBlocking()
											&& (errno == EAGAIN
													|| errno == EWOULDBLOCK))) {
								return uFilled;
							} else {
								m_pSocket->setStatusCode(Socket::RECV_FAIL);
								return -1;
							}
						}
						if (uReceived == 0) {
							m_pSocket->setStatusCode(Socket::RECV_FAIL);
							return -1;
						}

						m_iTail += uReceived;
						uFilled += uReceived;
					}

					if (uReceived == uFree) {
						uAvail = SocketAPI::availablesocket_ex(
								m_pSocket->getSOCKET());
						if (uAvail > 0) {
							if (I32(m_iLen + uAvail + 1) > m_iMaxLen) {
								//reset
								reset();
								m_pSocket->setStatusCode(
										Socket::INPUT_OVERFLOW);
								return -1;
							}
							// re size
							if (!__resize(I32(m_iLen + uAvail + 1))) {
								m_pSocket->setStatusCode(Socket::RECV_FAIL);
								return -1;
							}
							//recv
							uReceived = m_pSocket->receive(&m_pBuf[m_iTail],
									uAvail, iFlag);
							if (uReceived <= SOCKET_ERROR) {
								if (errno == EINTR
										|| (m_pSocket->isNonBlocking()
												&& (errno == EAGAIN
														|| errno == EWOULDBLOCK))) {
									return uFilled;
								} else {
									m_pSocket->setStatusCode(Socket::RECV_FAIL);
									return -1;
								}
							}
							if (uReceived == 0) {
								m_pSocket->setStatusCode(Socket::RECV_FAIL);
								return -1;
							}

							m_iTail += uReceived;
							uFilled += uReceived;
						}
					}
				}
			}
		}
	}
	// m_iHead > m_iTail
	else {
		//     T  H		LEN=10
		// 0123456789
		// abcd...efg
		uFree = m_iHead - m_iTail - 1;
		uReceived = 0;
		if (uFree > 0) {
			//recv
			uReceived = m_pSocket->receive(&m_pBuf[m_iTail], uFree, iFlag);
			if (uReceived <= SOCKET_ERROR) {
				if (errno == EINTR
						|| (m_pSocket->isNonBlocking()
								&& (errno == EAGAIN || errno == EWOULDBLOCK))) {
					return uFilled;
				} else {
					m_pSocket->setStatusCode(Socket::RECV_FAIL);
					return -1;
				}
			}
			if (uReceived == 0) {
				m_pSocket->setStatusCode(Socket::RECV_FAIL);
				return -1;
			}

			m_iTail += uReceived;
			uFilled += uReceived;

			if (uReceived == uFree) {
				UI32 uAvail = SocketAPI::availablesocket_ex(
						m_pSocket->getSOCKET());
				if (uAvail > 0) {
					if (I32(m_iLen + uAvail + 1) > m_iMaxLen) {
						// re set
						reset();
						m_pSocket->setStatusCode(Socket::INPUT_OVERFLOW);
						return -1;
					}

					if (!__resize(I32(m_iLen + uAvail + 1))) {
						m_pSocket->setStatusCode(Socket::RECV_FAIL);
						return -1;
					}
					//recv
					uReceived = m_pSocket->receive(&m_pBuf[m_iTail], uAvail,
							iFlag);
					if (uReceived <= SOCKET_ERROR) {
						if (errno == EINTR
								|| (m_pSocket->isNonBlocking()
										&& (errno == EAGAIN
												|| errno == EWOULDBLOCK))) {
							return uFilled;
						} else {
							m_pSocket->setStatusCode(Socket::RECV_FAIL);
							return -1;
						}
					}
					if (uReceived == 0) {
						m_pSocket->setStatusCode(Socket::RECV_FAIL);
						return -1;
					}

					m_iTail += uReceived;
					uFilled += uReceived;
				}
			}
		}
	}//end of else m_iHead > m_iTail

	return uFilled;
}

BOOL InputStream::peek(I8* pBuf, I32 iLen) const {
	if (pBuf == NULLPTR) {
		return FALSE;
	}
	if (iLen <= 0) {
		return FALSE;
	}
	if (checkSize(iLen) == FALSE) {
		return FALSE;
	}

	// 正序
	if (m_iHead < m_iTail) {
		memcpy(pBuf, &m_pBuf[m_iHead], iLen);
	} else if (m_iHead > m_iTail) {
		I32 iRearCount = m_iLen - m_iHead;
		if (iRearCount >= iLen) {
			memcpy(pBuf, &m_pBuf[m_iHead], iLen);
		} else {
			// 先 copy 尾部
			memcpy(pBuf, &m_pBuf[m_iHead], iRearCount);
			// 再 copy 头部
			memcpy(pBuf + iRearCount, m_pBuf, iLen - iRearCount);
		}
	}
	return TRUE;
}

BOOL InputStream::checkSize(I32 iCheckSize) const {
	if (iCheckSize < 0) {
		return FALSE;
	}
	return __getUsedCount() >= iCheckSize;
}

void InputStream::skip(I32 iOffset) {
	if (iOffset != 0) {
		m_iHead = (iOffset + m_iHead) % m_iLen;
		if (m_iHead < 0)
			m_iHead += m_iLen;
	}
}

void InputStream::setBuffMaxSize(I32 iSize) {
	m_iMaxLen = iSize;
}

I32 InputStream::getBuffMaxSize() const {
	return m_iMaxLen;
}

I32 InputStream::getBuffUsedSize() const {
	return __getUsedCount();
}

I32 InputStream::GetBuffSize() const {
	return m_iLen;
}

I8* InputStream::getHead() {
	return &m_pBuf[m_iHead];
}

OutputStream::OutputStream(Socket* pSocket, I32 iLen/*=DEFAULT_BUFF_SIZE*/,
		I32 iMaxLen/*=MAX_BUFF_SIZE*/) {
	m_pSocket = pSocket;
	m_iLen = iLen;
	m_iMaxLen = iMaxLen;
	m_iHead = m_iTail = 0;
	m_pBuf = new I8[iLen];
	memset(m_pBuf, 0, m_iLen);
}

OutputStream::~OutputStream() {
	if (m_pBuf != NULLPTR) {
		delete[] m_pBuf;
	}
}

BOOL OutputStream::move(I32 iLen) {
	if (0 >= iLen) {
		return FALSE;
	}

	I32 iFreeCnt = __getFreeCount();
	if (iFreeCnt < iLen + 1) {
		BOOL bRet = __resize((I32) (m_iLen + iLen + 1 - iFreeCnt));
		if (bRet == FALSE) {
			return FALSE;
		}
	}

	if (m_iHead <= m_iTail) {
		I32 iFreeTail = m_iLen - m_iTail;
		if (iFreeTail >= iLen) {
			memmove(&m_pBuf[m_iHead] + iLen, &m_pBuf[m_iHead],
					m_iTail - m_iHead);
		} else {
			I32 iMoveHead = (m_iHead + iLen) % m_iLen;
			I32 iMoveTail = (m_iTail + iLen) % m_iLen;
			if (iMoveHead <= iMoveTail) {
				memcpy(&m_pBuf[iMoveTail - (m_iTail - m_iHead)],
						&m_pBuf[m_iHead], m_iTail - m_iHead);
			} else {
				memcpy(m_pBuf, &m_pBuf[m_iTail - iMoveTail], iMoveTail);
				memmove(&m_pBuf[iMoveHead], &m_pBuf[m_iHead],
						m_iLen - iMoveHead);
			}
		}
	} else {
		I32 iMoveHead = (m_iHead + iLen) % m_iLen;
		I32 iMoveTail = (m_iTail + iLen) % m_iLen;
		if (iMoveHead <= iMoveTail) {
			memmove(&m_pBuf[iMoveTail - m_iTail], m_pBuf, m_iTail);
			memcpy(&m_pBuf[iMoveHead], &m_pBuf[m_iHead], m_iLen - m_iHead);
		} else {
			memmove(&m_pBuf[iLen], m_pBuf, m_iTail);
			memmove(m_pBuf, &m_pBuf[m_iLen - iLen], iLen);
			memmove(&m_pBuf[iMoveHead], &m_pBuf[m_iHead], m_iLen - iMoveHead);
		}
	}

	// 移动游标
	m_iTail = (m_iTail + iLen) % m_iLen;

	return TRUE;
}

I32 OutputStream::writeInHead(const I8* pBuf, I32 iLen) {
	if (0 >= iLen || NULLPTR == pBuf) {
		return 0;
	}

	I32 iFreeCnt = __getFreeCount();
	if (iFreeCnt < iLen + 1) {
		BOOL bRet = __resize((I32) (m_iLen + iLen + 1 - iFreeCnt));
		if (bRet == FALSE) {
			return -1;
		}
	}

	if (m_iHead <= m_iTail) {
		I32 iFreeTail = m_iLen - m_iTail;
		if (iFreeTail >= iLen) {
			// move
			memmove(&m_pBuf[m_iHead] + iLen, &m_pBuf[m_iHead],
					m_iTail - m_iHead);
			// cpy
			memcpy(&m_pBuf[m_iHead], pBuf, iLen);
		} else {

			I32 iMoveHead = (m_iHead + iLen) % m_iLen;
			I32 iMoveTail = (m_iTail + iLen) % m_iLen;
			if (iMoveHead <= iMoveTail) {
				// move
				memcpy(&m_pBuf[iMoveTail - (m_iTail - m_iHead)],
						&m_pBuf[m_iHead], m_iTail - m_iHead);
				// cpy
				memcpy(&m_pBuf[m_iHead], pBuf, m_iLen - m_iHead);
				memcpy(m_pBuf, &pBuf[m_iLen - m_iHead],
						iLen - (m_iLen - m_iHead));
			} else {
				// move
				memcpy(m_pBuf, &m_pBuf[m_iTail - iMoveTail], iMoveTail);
				memmove(&m_pBuf[iMoveHead], &m_pBuf[m_iHead],
						m_iLen - iMoveHead);
				// cpy
				memcpy(&m_pBuf[m_iHead], pBuf, iLen);
			}
		}
	} else {
		I32 iMoveHead = (m_iHead + iLen) % m_iLen;
		I32 iMoveTail = (m_iTail + iLen) % m_iLen;
		if (iMoveHead <= iMoveTail) {
			// move
			memmove(&m_pBuf[iMoveTail - m_iTail], m_pBuf, m_iTail);
			memcpy(&m_pBuf[iMoveHead], &m_pBuf[m_iHead], m_iLen - m_iHead);
			// cpy
			memcpy(&m_pBuf[m_iHead], pBuf, m_iLen - m_iHead);
			memcpy(m_pBuf, &pBuf[m_iLen - m_iHead], iLen - (m_iLen - m_iHead));
		} else {
			// move
			memmove(&m_pBuf[iLen], m_pBuf, m_iTail);
			memmove(m_pBuf, &m_pBuf[m_iLen - iLen], iLen);
			memmove(&m_pBuf[iMoveHead], &m_pBuf[m_iHead], m_iLen - iMoveHead);
			// cpy
			memcpy(&m_pBuf[m_iHead], pBuf, iLen);
		}
	}

	// 移动游标
	m_iTail = (m_iTail + iLen) % m_iLen;

	return iLen;
}

I32 OutputStream::write(const I8* pBuf, I32 iLen) {
	I32 iFreeCnt = __getFreeCount();
	if (iFreeCnt < iLen) {
		BOOL bRet = __resize((I32) (m_iLen + iLen + 1 - iFreeCnt));
		if (bRet == FALSE) {
			return -1;
		}
	}

	if (m_iHead <= m_iTail) {
		I32 iFreeTail = m_iLen - m_iTail;
		if (iFreeTail >= iLen) {
			memcpy(&m_pBuf[m_iTail], pBuf, iLen);
		} else {
			memcpy(&m_pBuf[m_iTail], pBuf, iFreeTail);
			memcpy(m_pBuf, &pBuf[iFreeTail], iLen - iFreeTail);
		}
	} else {
		memcpy(&m_pBuf[m_iTail], pBuf, iLen);
	}

	// 移动游标
	m_iTail = (m_iTail + iLen) % m_iLen;

	return iLen;
}

void OutputStream::skip(I32 iLen) {
	if (0 >= iLen)
		return;

	// 移动tail
	m_iTail = (m_iTail + iLen) % m_iLen;
}

void OutputStream::revert(I32 iLen) {
	if(0 >= iLen || __getUsedCount() < iLen)
		return;

	if(iLen <= m_iTail)
		m_iTail -= iLen;
	else
		m_iTail = m_iLen + m_iTail - iLen;
}

BOOL OutputStream::setBuffDefaultSize(I32 iSize) {
	if (m_iLen >= iSize) {
		return TRUE;
	}
	return __resize(iSize);
}

void OutputStream::reset() {
	memset(m_pBuf, 0, sizeof(I8) * m_iLen);
	m_iHead = 0;
	m_iTail = 0;
}

I32 OutputStream::flush() {
	// 已经满了
	if (m_iLen > m_iMaxLen) {
		delete[] m_pBuf;
		m_pBuf = new I8[DEFAULT_BUFF_SIZE];

		memset(m_pBuf, 0, DEFAULT_BUFF_SIZE);
		m_iHead = m_iTail = 0;
		m_iLen = DEFAULT_BUFF_SIZE;
		m_pSocket->setStatusCode(Socket::OUTPUT_OVERFLOW);
		return -1;
	}

	I32 iFlag = MSG_NOSIGNAL;
	I32 iTotalSend = 0;
	I32 iSend = 0;
	if (m_iTail > m_iHead) {
		I32 iLen = m_iTail - m_iHead;
		while (iLen > 0) {
			iSend = m_pSocket->send(&m_pBuf[m_iHead], iLen, iFlag);
			if (iSend <= SOCKET_ERROR) {
				if (m_pSocket->isNonBlocking()
						&& (errno == EINTR || errno == EAGAIN
								|| errno == NO_BUFFERS
								|| errno == EWOULDBLOCK)) {
					return iTotalSend;
				} else if (!m_pSocket->isNonBlocking() && errno == EINTR) {
					continue;
				} else {
					m_pSocket->setStatusCode(Socket::SEND_FAIL);
					return -1;
				}
			}

			if (iSend == 0) {
				continue;
			}

			iTotalSend += iSend;
			m_iHead += iSend;
			iLen -= iSend;
		}
	} else if (m_iTail < m_iHead) {
		// head bigger than tail
		I32 iLen1 = m_iLen - m_iHead;
		while (iLen1 > 0) {
			iSend = m_pSocket->send(&m_pBuf[m_iHead], iLen1, iFlag);
			if (iSend <= SOCKET_ERROR) {
				if (m_pSocket->isNonBlocking()
						&& (errno == EINTR || errno == EAGAIN
								|| errno == NO_BUFFERS
								|| errno == EWOULDBLOCK)) {
					return iTotalSend;
				} else if (!m_pSocket->isNonBlocking() && errno == EINTR) {
					continue;
				} else {
					m_pSocket->setStatusCode(Socket::SEND_FAIL);
					return -1;
				}
			}

			if (iSend == 0) {
				continue;
			}

			iTotalSend += iSend;
			m_iHead += iSend;
			iLen1 -= iSend;
		}

		// left
		m_iHead = 0;
		I32 iLen2 = m_iTail;
		while (iLen2 > 0) {
			iSend = m_pSocket->send(&m_pBuf[m_iHead], iLen2, iFlag);
			if (iSend <= SOCKET_ERROR) {
				if (m_pSocket->isNonBlocking()
						&& (errno == EINTR || errno == EAGAIN
								|| errno == NO_BUFFERS
								|| errno == EWOULDBLOCK)) {
					return iTotalSend;
				} else if (!m_pSocket->isNonBlocking() && errno == EINTR) {
					continue;
				} else {
					m_pSocket->setStatusCode(Socket::SEND_FAIL);
					return -1;
				}
			}

			if (iSend == 0) {
				continue;
			}

			iTotalSend += iSend;
			m_iHead += iSend;
			iLen2 -= iSend;
		}
	}

	return iTotalSend;
}

void OutputStream::setBuffMaxSize(I32 iSize) {
	m_iMaxLen = iSize;
}

I32 OutputStream::getBuffMaxSize() const {
	return m_iMaxLen;
}

I32 OutputStream::getBuffUsedSize() const {
	return __getUsedCount();
}

BOOL OutputStream::__resize(I32 newSize) {
	if(m_iLen >= m_iMaxLen) {
		m_pSocket->setStatusCode(Socket::OUTPUT_OVERFLOW);
		return FALSE;
	}

	I32 iStep = m_iLen;
	while (newSize > iStep) {
		iStep += iStep;
	}

	I32 iNewSize = iStep;
	if(iNewSize > m_iMaxLen) {
		iNewSize = m_iMaxLen;
	}

	I8* pNewBuf = new I8[iNewSize];
	if (pNewBuf == NULLPTR) {
		return FALSE;
	}
	memset(pNewBuf, 0, iNewSize);

	if (m_iHead < m_iTail) {
		memcpy(pNewBuf, &m_pBuf[m_iHead], m_iTail - m_iHead);
	} else if (m_iHead > m_iTail) {
		memcpy(pNewBuf, &m_pBuf[m_iHead], m_iLen - m_iHead);
		memcpy(pNewBuf + m_iLen - m_iHead, m_pBuf, m_iTail);
	}

	delete[] m_pBuf;
	m_pBuf = pNewBuf;

	m_iTail = __getUsedCount();
	m_iHead = 0;
	m_iLen = iNewSize;

	return TRUE;
}

I32 OutputStream::__getUsedCount() const {
	if (m_iHead <= m_iTail) {
		return m_iTail - m_iHead;
	} else {
		return (m_iLen - m_iHead) + m_iTail;
	}
}

I32 OutputStream::__getFreeCount() const {
	return m_iLen - __getUsedCount() - 1;
}

I32 OutputStream::getBuffSize() const {
	return m_iLen;
}

I8* OutputStream::getHead() {
	return &m_pBuf[m_iHead];
}

}

