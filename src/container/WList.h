/*
 * WList.h
 *
 *  Created on: 2016年7月12日
 *      Author: 58
 */

#ifndef WEDIS_BASE_CONTAINER_WLIST_H_
#define WEDIS_BASE_CONTAINER_WLIST_H_

#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Atomic.h>


/**
 * 使用自旋锁实现的一个线程安全的list
 */
namespace wRedis {

template<typename T>
class WList : public Noncopyable {
private:
typedef struct Node {
	T data;
	Node* prev;
	Node* next;
	Node(const T& v) : data(v), prev(NULLPTR), next(NULLPTR) {}
} Node;

public:
	struct iterator {
        Node* ptr;

        iterator() : ptr(NULLPTR) {}
        explicit iterator(Node* p) : ptr(p) {}
        friend bool operator==(iterator const& lhs, iterator const& rhs)
        { return lhs.ptr == rhs.ptr; }
        friend bool operator!=(iterator const& lhs, iterator const& rhs)
        { return !(lhs.ptr == rhs.ptr); }
        iterator& operator++() { ptr = ptr->next; return *this; }
        iterator operator++(int) { iterator ret = *this; ++(*this); return ret; }
        iterator& operator--() { ptr = ptr->prev; return *this; }
        iterator operator--(int) { iterator ret = *this; --(*this); return ret; }
        T& operator*() { return ptr->data; }
        T* operator->() { return (T*)(&(ptr->data)); }
    };

public:
	WList(BOOL bThreadSafe) : m_bThreadSafe(bThreadSafe) {
		m_pHead = NULLPTR;
		m_pTail = NULLPTR;
		m_Size  = 0;
	}
	~WList() { clear(); }

public:
	inline void push_back(const T& v) {
		push(v, -1);
	}

	inline void push_front(const T& v) {
		push(v, 0);
	}

	inline void push(const T& v, I32 idx) {
		Node *pNode = new Node(v);
		Assert(pNode);
		if(m_bThreadSafe)
			m_Lock.lock();

		if(m_Size >= W_INT_MAX - 1) {
			if(m_bThreadSafe)
				m_Lock.unlock();

			return;
		}

		// add tail
		if(idx < 0 || m_Size <= (UI32)idx) {
			pNode->prev = m_pTail;
			if(NULLPTR == m_pTail) {
				m_pHead = pNode;
			}
			else {
				m_pTail->next = pNode;
			}
			m_pTail = pNode;
		}
		else {
			Node* p = m_pHead;
			while((idx-- > 0) && p && (p = p->next));
			Assert(p);
			Assert(0 >= idx);

			pNode->next = p;
			pNode->prev = p->prev;
			if(pNode->prev)
				pNode->prev->next = pNode;
			else
				m_pHead = pNode;

			p->prev = pNode;
		}
		m_Size++;

		if(m_bThreadSafe)
			m_Lock.unlock();
	}

	iterator get(I32 idx) {
		if(NULLPTR == m_pHead) {
			return iterator();
		}

		if(m_Size <= (UI32)idx || 0 > idx)
			return iterator();

		if(m_bThreadSafe) {
			m_Lock.lock();
			if(m_pHead == NULLPTR) {
				m_Lock.unlock();
				return iterator();
			}

			if(m_Size <= (UI32)idx || 0 > idx) {
				m_Lock.unlock();
				return iterator();
			}
		}

		Node* p = m_pHead;
		while((idx-- > 0) && p && (p = p->next));
		if(m_bThreadSafe) {
			m_Lock.unlock();
		}

		return iterator(p);
	}

	iterator begin() { return iterator(m_pHead); }
	iterator end() { return iterator(); }

	inline T pop() {
		if(m_pHead == NULLPTR) {
			throw 1;
		}

		Node* pRet = NULLPTR;
		if(m_bThreadSafe) {
			m_Lock.lock();
			if(m_pHead == NULLPTR) {
				m_Lock.unlock();
				throw 1;
			}
		}

		pRet = m_pHead;
		if(NULLPTR == m_pHead->next) {
			m_pHead = NULLPTR;
			m_pTail = NULLPTR;
			m_Size  = 0;
		}
		else {
			m_pHead = m_pHead->next;
			m_pHead->prev = NULLPTR;
			m_Size--;
		}

		if(m_bThreadSafe)
			m_Lock.unlock();

#ifdef __GXX_CXX0X__
     	T ret(std::move(pRet->data));
#else
		T ret(pRet->data);
#endif
		W_DELETE(pRet);
		return ret;
	}

	inline void remove(const T& rVal) {
		if(m_pHead == NULLPTR) {
			return;
		}
		Node* pRemove = NULLPTR;
		if(m_bThreadSafe) {
			m_Lock.lock();
			if(m_pHead == NULLPTR) {
				m_Lock.unlock();
				return;
			}
		}

		Node* p = m_pHead;
		while(p) {
			if(p->data == rVal) {
				if(p->prev)
					p->prev->next = p->next;
				else
					m_pHead = p->next;

				if(p->next)
					p->next->prev = p->prev;
				else
					m_pTail = p->prev;

				W_DELETE(p);
				m_Size--;
				break;
			}

			p = p->next;
		}

		if(m_bThreadSafe)
			m_Lock.unlock();
	}

	inline size_t size() {
		size_t ret = 0;
		if(m_bThreadSafe)
			m_Lock.lock();

		ret = m_Size;

		if(m_bThreadSafe)
			m_Lock.unlock();

		return ret;
	}

	inline BOOL empty() {
		return 0 == size();
	}

	void clear() {
		if(m_bThreadSafe)
			m_Lock.lock();

		Node* p = m_pHead;
		Node* del = NULLPTR;
		while(p) {
			del = p;
			p = p->next;
			W_DELETE(del);
		}

		if(m_bThreadSafe)
			m_Lock.unlock();

		m_pHead = NULLPTR;
		m_pTail = NULLPTR;
		m_Size  = 0;
	}


	I32 testSize() {
		I32 ret = 0;
		Node* p = m_pHead;
		while(p) {
			ret++;
			p = p->next;
		}

		return ret;
	}

private:
	// 是否线程安全
	BOOL  m_bThreadSafe;
	// head 节点
	Node* m_pHead;
	// tail 节点
	Node* m_pTail;
	// 队列大小
	UI32  m_Size;
	// SpinLock
	SpinLock m_Lock;

};

} /* namespace wRedis */

#endif /* WEDIS_BASE_CONTAINER_WLIST_H_ */
