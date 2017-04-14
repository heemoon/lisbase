//
// Created by 58 on 2016/4/28.
//

#ifndef WREDISCLIENT_COROUTINE_H
#define WREDISCLIENT_COROUTINE_H

#include <cstddef>
#include <ucontext.h>
#include <wedis/base/coroutine/CoroutineManager.h>
#include <wedis/base/util/CommonType.h>


namespace wRedis {
    class CoroutineManager;
    class Coroutine {
    public:
        Coroutine();
        virtual ~Coroutine();
    public:
        BOOL init(CoroutineManager* pManager, coroutine_func func, void* pUserData);
        void excute() { m_pFunc(m_pOwner, m_pUserData); }
    public:
        inline CoroutineStatus::EM_CO_STATUS status() { return m_eStatus; }
        inline void status(CoroutineStatus::EM_CO_STATUS eStatus) { m_eStatus = eStatus; }
        inline ucontext_t& ctx() { return m_Ctx; }
        inline I32 size() { return m_nSize; }
        inline void size(I32 nSize) { m_nSize = nSize; }
        inline I8* stack() { return m_pStackAddr; }
        inline void stack(I8* pStackAddr) { m_pStackAddr = pStackAddr; }
        inline I32 capaiity() { return m_nCap; }
        inline void capaiity(I32 nCap) { m_nCap = nCap; }
    private:
        // suspend func
        coroutine_func m_pFunc;
        // func private data
        void* m_pUserData;
        // stack context
        ucontext_t m_Ctx;
        // 指针偏移值
        ptrdiff_t m_nCap;
        // 指针当前偏移值
        ptrdiff_t m_nSize;
        // 协同状态
        CoroutineStatus::EM_CO_STATUS m_eStatus;
        // 协同栈地址
        I8* m_pStackAddr;
        // owner
        CoroutineManager* m_pOwner;
    };
}




#endif //WREDISCLIENT_COROUTINE_H
