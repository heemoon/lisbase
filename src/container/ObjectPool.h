//
// Created by 58 on 2016/5/12.
//

#ifndef WREDISCLIENT_OBJECTPOOL_H
#define WREDISCLIENT_OBJECTPOOL_H

#include <wedis/base/container/BlockingQueue.h>
#include <wedis/base/container/SafeMap.h>
#include <wedis/base/thread/Lock.h>
#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/Assertx.h>
#include <wedis/base/util/Alloctor.h>
#include <wedis/base/container/ObjectPoolFactory.h>
#include <map>

namespace wRedis {

/**
 * Object Pool Config
 */
struct ObjectPoolConfig {
	I32 m_nMaxSize;              // 池最大实例大小，当<=0时则视为无线大, 最大峰值21亿
	I32 m_nMinIdle;              // 最小idle的数量
	I32 m_nMaxIdle;              // 最大idle的数量
	BOOL m_bTestOnBorrow;        // 当borrow的时候是否调用factory->validate接口
	BOOL m_bTestOnReturn;        // 当return的时候是否调用factory->validate接口
	BOOL m_bLifo;                // is last in first out
	TIMET m_tBorrowTime;         // borrow的等待市场，当<=0时表无线等待

	ObjectPoolConfig() {
		m_nMaxSize = 8;
		m_nMinIdle = 0;
		m_nMaxIdle = 6;
		m_bTestOnBorrow = FALSE;
		m_bTestOnReturn = FALSE;
		m_bLifo = TRUE;
		m_tBorrowTime = INVALID_TIME;
	}
};

template<typename T>
class ObjectPool {
/**
 * Object Wrapper
 */
typedef struct ObjectWrapper {
	enum EM_POOL_STATUS {
		POOL_STATUS_INVALID = INVALID_VALUE,
		POOL_STATUS_IDLE,
		POOL_STATUS_ACTIVE,
	};
public:
	ObjectWrapper(T& obj) {
		m_pObj = &obj;
		m_eStatus = POOL_STATUS_INVALID;
	}

	~ObjectWrapper() {
		W_DELETE(m_pObj);
		m_eStatus = POOL_STATUS_INVALID;
	}
public:
	inline T* getObj() { return m_pObj; }
	inline BOOL isActive() { return POOL_STATUS_ACTIVE == m_eStatus; }
	inline void markIdle() { m_eStatus = POOL_STATUS_IDLE; }
	inline void markActive() { m_eStatus = POOL_STATUS_ACTIVE; }
private:
	T* m_pObj;
	EM_POOL_STATUS m_eStatus;

}ObjectWrapper;

typedef SafeMap<T*, ObjectWrapper*>    PoolMap_t;
typedef BlockingQueue<ObjectWrapper*>  PoolQueue_t;

public:
    ObjectPool(const ObjectPoolConfig& config, ObjectPoolFactory<T>& factory)
		: m_Config(config), m_pFactory(&factory)
	{
    	atomic_set(&m_nCurSize, 0);
    	atomic_set(&m_bIsClosed, 0);
    }

    virtual ~ObjectPool() {
    	for(it = m_AllObjs.begin(); it != m_AllObjs.end(); ++it) {
    		W_DELETE(it->second);
    	}

    	m_AllObjs.clear();
    }

public:
    T* borrowObject() {
    	// 0. ret null when closed status
    	if(isClosed()) {
    		Logger.warn("ObjectPool is closed.");
    		return NULLPTR;
    	}

    	// 1. 尝试无阻赛的poll一个
    	ObjectWrapper* pWrapper = NULLPTR;
    	_MY_TRY {
    		pWrapper = m_Idles.poll();
    	}
    	_MY_CATCH {
    		// do nothing ...
    		pWrapper = NULLPTR;
    	}

    	if(NULLPTR == pWrapper) {
    		// 2. 表示idles表已空则创建一个
    		pWrapper = create();
    	}

    	// 3. 如果创建失败，有可能已经满了，则take带超时的
    	if(NULLPTR == pWrapper) {
    		// take with timeout
    		_MY_TRY {
    			pWrapper = m_Idles.takeWithTimeOut(m_Config.m_tBorrowTime);
    		}
    		_MY_CATCH {
    		   	// do nothing ...
    			pWrapper = NULLPTR;
    		}
    	}

    	// 4. 如果这里还是空，则返回空
    	if(NULLPTR == pWrapper) {
    		Logger.warn("borrowObject get take ObjectWrapper is timeout!");
    		return NULLPTR;
    	}

    	T* obj = pWrapper->getObj();
    	AssertRetNull(obj);

    	BOOL bDestroy = FALSE;
    	do {
    		// 5. borrow时激活
    		if(!m_pFactory->activateObject(*obj)) {
    			bDestroy = TRUE;
    		    Logger.error("ObjectPool::borrow activateObject obj failed! will be destory");
    		    break;
    		}
    		// 6. 判断有效性
        	if(m_Config.m_bTestOnBorrow && !m_pFactory->validateObject(*obj)) {
        		bDestroy = TRUE;
        		Logger.error("ObjectPool::borrow validateObject obj failed! will be destory");
        		break;
        	}
    	} while(FALSE);

    	// 7. 是否destroy
    	if(bDestroy) {
    		destroy(pWrapper);
    		return NULLPTR;
    	}

    	// 8. 标记 active 状态
    	pWrapper->markActive();

    	return obj;
    }

    void returnObject(T* obj) {
    	// 1. check param
    	AssertRetEmpty(obj);

    	// 2. check wrapper
    	ObjectWrapper** pDWrapper = m_AllObjs.get(obj);
    	AssertRetEmpty(pDWrapper);
    	AssertRetEmpty(*pDWrapper);
    	AssertRetEmpty((*pDWrapper)->getObj() == obj);
    	ObjectWrapper* pWrapper = *pDWrapper;
    	if(!pWrapper->isActive()) {
    		Logger.error("ObjectPool return obj is not in active status. then force destroy this obj.");
    		// froce destroy
    		destroy(pWrapper);
    		return;
    	}

    	BOOL bDestroy = FALSE;
    	do {
    		// 3. check validate obj
        	if(m_Config.m_bTestOnReturn && !m_pFactory->validateObject(*obj)) {
        		bDestroy = TRUE;
        		Logger.error("ObjectPool::returnObject validateObject obj failed! will be destory");
        		break;
        	}

        	// 4. passivateObject
        	if(!m_pFactory->passivateObject(*obj)) {
        		bDestroy = TRUE;
        		Logger.error("ObjectPool::returnObject passivateObject obj failed! will be destory");
        		break;
        	}
    	} while(FALSE);

    	// 5. is destroy
    	if(bDestroy) {
    		destroy(pWrapper);
    		return;
    	}

    	// 6. 如果closed或者已经达到最大数了就需要销毁
    	if(isClosed() || (m_Config.m_nMaxIdle >= 0 && m_Config.m_nMaxIdle <= m_Idles.size())) {
    		// destory
    		destroy(pWrapper);
    	}
    	else {
    		// 7. must to mark idle first
    		pWrapper->markIdle();

    		// 8. is last in first out
    		if(m_Config.m_bLifo) {
    			m_Idles.putFront(pWrapper);
    		}
    		else {
    			m_Idles.put(pWrapper);
    		}
    	}
    }


    void invalidObject(T* obj) {
    	// 1. check param
    	AssertRetEmpty(obj);

    	// 2. get wrapper by obj
    	ObjectWrapper** pDWrapper = m_AllObjs.get(obj);
    	if(NULLPTR == pDWrapper || NULLPTR == *pDWrapper) {
    		Logger.error("ObjectPool::invalidObject obj not currently part of this pool");
    		return;
    	}

    	// 3. check is active
    	ObjectWrapper* pWrapper = *pDWrapper;
    	if(!pWrapper->isActive()) {
    		Logger.error("ObjectPool::invalidObject obj not in active mode");
    	}

    	// 4. destroy
    	destroy(pWrapper);
    }

    BOOL addObject(I32 nCount) {
    	// TODO: ObjectPool addObject nCount
    	// check can add objs
    	// then add make and insert into idles
    	return TRUE;
    }

    void close() { atomic_set(&m_bIsClosed, 1); }

public:
    inline I32 getActiveNum() {
    	I32 nSize = atomic_read(&m_nCurSize);
    	return nSize - getIdleNum();
    }

    inline I32 getIdleNum() { return m_Idles.size(); }
    inline ObjectPoolConfig& getConfig() { return m_Config; }
    inline BOOL isClosed() { return atomic_read(&m_bIsClosed) == 1; }
    inline BOOL canDestory() { return TRUE == isClosed() && 0 == getActiveNum(); }

private:
    void destroy(ObjectWrapper* pWrapper) {
    	AssertRetEmpty(pWrapper);

    	m_Idles.remove(pWrapper);
    	T* obj = pWrapper->getObj();
    	if(W_LIKELY(NULLPTR != obj)) {
    		// TODO: 如果这里的obj已被释放或者并不存在，现在是忽略掉all objs的 内存释放。
    		m_AllObjs.remove(obj);
    		m_pFactory->destroyObject(*obj);
    	}
    	else {
    		Logger.error("ObjectPool::destroy obj is nil");
    	}

    	atomic_dec(&m_nCurSize);
    	W_DELETE(pWrapper);
    }

    ObjectWrapper* create() {
    	I32 nMaxSize = m_Config.m_nMaxSize;
    	I32 nSize    = atomic_read(&m_nCurSize);
    	if((0 < nMaxSize && nSize >= nMaxSize) || nSize >= 2147480000L) {
    		return NULLPTR;
    	}

    	T* pObj = m_pFactory->makeObject();
    	if(NULLPTR == pObj) return NULLPTR;

    	ObjectWrapper* pWrapper = new ObjectWrapper(*pObj);
    	AssertRetNull(pWrapper);

    	// check size
    	if(FALSE == m_AllObjs.putWithLimitSize(pObj, pWrapper, nMaxSize)) {
    		W_DELETE(pWrapper);
    	}

    	atomic_inc(&m_nCurSize);
    	return pWrapper;
    }

private:
    // 配置信息
    ObjectPoolConfig m_Config;
    // 对象池工厂
    ObjectPoolFactory<T>* m_pFactory;
    // 所有对象
    PoolMap_t m_AllObjs;
    // 空闲队列
    PoolQueue_t m_Idles;
    // 当前大小
    atomic_t m_nCurSize;
    // 是否关闭
    atomic_t m_bIsClosed;

    typename std::map<T*, ObjectWrapper*>::iterator it;
};

}



#endif //WREDISCLIENT_OBJECTPOOL_H
