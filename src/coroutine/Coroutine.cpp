//
// Created by 58 on 2016/4/28.
//
#include <wedis/base/coroutine/Coroutine.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {
    Coroutine::Coroutine() {
        m_pStackAddr = NULLPTR;
        m_pFunc = NULLPTR;
        m_pUserData = NULLPTR;
        m_pOwner = NULLPTR;
        m_nCap = 0;
        m_nSize = 0;
        m_eStatus = CoroutineStatus::Dead;
    }

    Coroutine::~Coroutine() {
        W_FREE(m_pStackAddr);
    }

    BOOL Coroutine::init(CoroutineManager* pManager, coroutine_func func, void* pUserData) {
        Assert(pManager);
        Assert(func);
        m_pStackAddr = NULLPTR;
        m_pFunc = func;
        m_pUserData = pUserData;
        m_pOwner = pManager;
        m_nCap = 0;
        m_nSize = 0;
        m_eStatus = CoroutineStatus::Ready;
        return TRUE;
    }



}
