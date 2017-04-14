/*
 * SafeMap.h
 *
 *  Created on: 2016年5月25日
 *      Author: 58
 */

#ifndef BASE_CONTAINER_SAFEMAP_H_
#define BASE_CONTAINER_SAFEMAP_H_

#include <wedis/base/thread/Lock.h>
#include <wedis/base/util/Noncopyable.h>
#include <map>

namespace wRedis {

template<typename K, typename V>
class SafeMap {
public:
	SafeMap() : m_Lock() {}
	~SafeMap(){}
public:
	V* get(const K& key) {
		V* pRet = NULLPTR;
		m_Lock.readLock();
		typename std::map<K, V>::iterator it = m_Map.find(key);
		if(it != m_Map.end()) {
			pRet = &(it->second);
		}
		m_Lock.readUnLock();

		return pRet;
	}

	void put(const K& key, const V& val) {
		m_Lock.writeLock();
		m_Map.insert(std::pair<K, V>(key, val));
		m_Lock.writeUnLock();
	}

	BOOL putWithLimitSize(const K& key, const V& val, const I32 nLimitSize) {
		m_Lock.writeLock();
		if((I32)m_Map.size() >= nLimitSize) {
			m_Lock.writeUnLock();
			return FALSE;
		}
		m_Map.insert(std::pair<K, V>(key, val));
		m_Lock.writeUnLock();
		return TRUE;
	}

	BOOL contains(const K& key) {
		BOOL bExist = FALSE;

		m_Lock.readLock();
		typename std::map<K, V>::iterator it = m_Map.find(key);
		bExist = it != m_Map.end();
		m_Lock.readUnLock();

		return bExist;
	}

	void remove(const K& key) {
		m_Lock.writeLock();
		m_Map.erase(key);
		m_Lock.writeUnLock();
	}

	void clear() {
		m_Lock.writeLock();
		m_Map.clear();
		m_Lock.writeUnLock();
	}

	typename std::map<K, V>::iterator begin() {
		return m_Map.begin();
	}

	typename std::map<K, V>::iterator end() {
		return m_Map.end();
	}

	BOOL empty() {
		BOOL isEmpty = FALSE;
		m_Lock.readLock();
		isEmpty = m_Map.empty();
		m_Lock.readUnLock();
		return isEmpty;
	}
private:
	RWLock         m_Lock;
	std::map<K, V> m_Map;
};

} /* namespace wRedis */

#endif /* BASE_CONTAINER_SAFEMAP_H_ */
