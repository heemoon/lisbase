//
// Created by 58 on 2016/4/28.
//

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <wedis/base/coroutine/CoroutineManager.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {
    CoroutineManager::CoroutineManager(async_resume_fn fn, void* u) {
        m_nCursor = 0;
        m_nCap = DEFAULT_COROUTINE;
        m_RunningCoID = INVALID_VALUE;
        m_pCo = (Coroutine**) W_MALLOC(sizeof(Coroutine*) * m_nCap);
        Assert(m_pCo);
        memset(m_pCo, 0, sizeof(Coroutine*) * m_nCap);

        //TODO: 如果fn 为空则需要使用默认的容器使用
        m_asynResumeFn = fn;
        m_u = u;
        for(I32 i = 0; i < 100; ++i) {
            atomic_set(&mark[i], 0);
        }
    }

    CoroutineManager::~CoroutineManager() {
        for (I32 i=0;i < m_nCap; i++) {
            W_FREE(m_pCo[i]);
        }

        W_FREE(m_pCo);
    }


    I32 CoroutineManager::create(coroutine_func func, void* ud) {
        Coroutine* co = (Coroutine*) W_MALLOC(sizeof(*co));
        Assert(co);
        Assert(co->init(this, func, ud));

        if (m_nCursor >= m_nCap) {
            I32 id = m_nCap;
            m_pCo = (Coroutine**)W_REALLOC(m_pCo, m_nCap * 2 * sizeof(Coroutine*));
            memset(m_pCo + m_nCap , 0 , sizeof(Coroutine*) * m_nCap);
            m_pCo[m_nCap] = co;
            m_nCap *= 2;
            ++m_nCursor;
            return id;
        } else {
            for (I32 i = 0; i < m_nCap; ++i) {
                int id = (i + m_nCursor) % m_nCap;
                if (m_pCo[id] == NULLPTR) {
                    m_pCo[id] = co;
                    ++m_nCursor;
                    return id;
                }
            }
        }

        NOT_REACHED();
        return INVALID_VALUE;
    }


    void CoroutineManager::resume(I32 id) {
        Assert(INVALID_VALUE == m_RunningCoID);
        Assert(id >=0);
        Assert(id < m_nCap);

        Coroutine* co = m_pCo[id];
        if(NULLPTR == co) return;

        CoroutineStatus::EM_CO_STATUS eStatus = co->status();
        switch(eStatus) {
            case CoroutineStatus::Ready: {
                ucontext_t& ctx = co->ctx();
                getcontext(&ctx);

                ctx.uc_stack.ss_sp = m_Stack;
                ctx.uc_stack.ss_size = STACK_SIZE;
                ctx.uc_link = &m_MainCtx;
                m_RunningCoID = id;
                co->status(CoroutineStatus::Running);
                uintptr_t ptr = (uintptr_t) (this);
                makecontext(&ctx, (void (*)(void)) mainfunc, 2, (uint32_t)ptr, (uint32_t)(ptr>>32));
                swapcontext(&m_MainCtx, &ctx);
            }
                break;
            case CoroutineStatus::Suspend: {
                memcpy(m_Stack + STACK_SIZE - co->size(), co->stack(), co->size());
                m_RunningCoID = id;
                co->status(CoroutineStatus::Running);
                swapcontext(&m_MainCtx, &co->ctx());
            }
                break;
            default:
            	NOT_REACHED();
        }
    }

    void CoroutineManager::_save_stack(Coroutine* co, I8* pTop) {
        Assert(co);
        Assert(pTop);
        I8 dummy = 0;
        Assert(pTop - &dummy <= STACK_SIZE);
        if (co->capaiity() < pTop - &dummy) {
            I8* pTmpStack = co->stack();
            W_FREE(pTmpStack);
            co->capaiity(pTop - &dummy);
            co->stack((I8*)W_MALLOC((size_t)co->capaiity()));
        }
        co->size(pTop - &dummy);
        memcpy(co->stack(), &dummy, (size_t)co->size());
    }

    void CoroutineManager::yeild() {
        I32 id = m_RunningCoID;
        Assert(id >= 0);
        Coroutine * co = m_pCo[id];
        Assert((I8*) &co > m_Stack);
        _save_stack(co, m_Stack + STACK_SIZE);
        co->status(CoroutineStatus::Suspend);
        m_RunningCoID = INVALID_VALUE;
        swapcontext(&co->ctx() , &m_MainCtx);
    }

    CoroutineStatus::EM_CO_STATUS CoroutineManager::status(I32 id) {
        Assert(0 <= id && m_nCap > id);
        if(NULLPTR == m_pCo[id]) return CoroutineStatus::Dead;
        return m_pCo[id]->status();
    }

    void CoroutineManager::mainfunc(UI32 low32, UI32 hi32) {
        uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
        CoroutineManager* pManager = (CoroutineManager*) ptr;
        int id = pManager->cur_running_co_id();
        Coroutine* co = pManager->get(id);
        Assert(co);
        co->excute();
        pManager->remove(id);
        pManager->cur_running_co_id(INVALID_VALUE);
    }

    void CoroutineManager::remove(I32 id) {
        Assert(0 <= id && m_nCap > id);
        W_FREE(m_pCo[id]);
        m_nCursor--;
    }

    Coroutine* CoroutineManager::get(I32 id) {
        AssertRetNull(0 <= id && m_nCap > id);

        return m_pCo[id];
    }
}
