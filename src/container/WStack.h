/*
 * WStack.h
 *
 *  Created on: 2016年8月4日
 *      Author: 58
 */

#ifndef WEDIS_BASE_CONTAINER_WSTACK_H_
#define WEDIS_BASE_CONTAINER_WSTACK_H_

#include <wedis/base/util/Atomic.h>
#include <wedis/base/util/Noncopyable.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {

template<typename T>
class WStack : public Noncopyable {
private:
typedef struct Node {
	T data;
	Node* next;
	Node(const T& v) : data(v), next(NULLPTR) {}
}Node;

public:
	WStack(BOOL bThreadSafe = FALSE) : m_pTop(NULLPTR), m_bThreadSafe(bThreadSafe) {
		atomic_set(&m_Size, 0);
	}

public:
	void push(const T& v) {
		Node *pNode = new Node(v);
		Assert(pNode);
		if(!m_bThreadSafe) {
			pNode->next = m_pTop;
			m_pTop = pNode;
		}
		else {
			while (TRUE) {
				pNode->next = m_pTop;
				if (CAS(&m_pTop, pNode->next, pNode))
					break;
			}
		}

		atomic_inc(&m_Size);
	}

	T pop() {
		if(!m_bThreadSafe) {
			Node* pNode = m_pTop;
			if(NULLPTR == pNode)
				throw 1;

			m_pTop = pNode->next;
#ifdef __GXX_CXX0X__
     		T ret(std::move(pNode->data));
#else
			T ret(pNode->data);
#endif
			atomic_dec(&m_Size);
			W_DELETE(pNode);
			return ret;
		}
		else {
			while (TRUE) {
				Node* pNode = m_pTop;
				if (pNode == NULLPTR)
					return NULLPTR;
				if (m_pTop && CAS(&m_pTop, pNode, pNode->next)) {
					T ret(pNode->data);
					atomic_dec(&m_Size);
					W_DELETE(pNode);
					return ret;
				}
			}//end of while
		}//end of if(bThreadSafe)
	}

	I32 size() {
		return atomic_read(&m_Size);
	}

	I32 testSize() {
		I32 ret = 0;
		Node* p = m_pTop;
		while(p) {
			ret++;
			p = p->next;
		}

		return ret;
	}

private:
	Node* m_pTop;
	BOOL  m_bThreadSafe;
	atomic_t m_Size;
};

} /* namespace wRedis */

#endif /* WEDIS_BASE_CONTAINER_WSTACK_H_ */
