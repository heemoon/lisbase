//
// Created by 58 on 2016/4/28.
//

#ifndef WREDISCLIENT_COROUTINEMANAGER_H
#define WREDISCLIENT_COROUTINEMANAGER_H

#include <ucontext.h>
#include <wedis/base/util/CommonST.h>
#include <wedis/base/coroutine/Coroutine.h>
#include <wedis/base/util/Atomic.h>
#include <wedis/base/util/CommonType.h>

namespace wRedis {
#define DEFAULT_COROUTINE 16
#define STACK_SIZE (1024*1024)
typedef void (*async_resume_fn) (void* pCoMgr, I32 coID, void* u);

    class Coroutine;
    class CoroutineManager {
    public:
        CoroutineManager(async_resume_fn fn = NULLPTR, void* u = NULLPTR);
        virtual ~CoroutineManager();

    public:
        I32 create(coroutine_func func, void* ud);
        void resume(I32 id);
        void yeild();
        CoroutineStatus::EM_CO_STATUS status(I32 id);
    public:
        Coroutine* get(I32 id);
        void remove(I32 id);
    public:
        inline I32 capaiity() { return m_nCap; }
        inline I32 cur_running_co_id() { return m_RunningCoID; }
        inline void cur_running_co_id(I32 id) { m_RunningCoID = id; }

    public:
        void async_resume(I32 id) {
//            atomic_set(&mark[id], 1);

            // fn
            if(m_asynResumeFn) {
                m_asynResumeFn(this, id, m_u);
            }
        }

        void setTest(async_resume_fn fn, void* u) {
        	m_asynResumeFn = fn;
        	m_u = u;
        }

        void loop_async_resume() {
            for(I32 i = 0; i < 100; ++i) {
                if(0 < atomic_read(&mark[i])) {
                    resume(i);
                    atomic_set(&mark[i], 0);
                }
            }
        }

    private:
        static void _save_stack(Coroutine* co, I8* pTop);
        static void mainfunc(UI32 low32, UI32 hi32);

    private:
        I8 m_Stack[STACK_SIZE];
        ucontext_t m_MainCtx;
        I32 m_nCursor;
        I32 m_nCap;   //
        I32 m_RunningCoID;
        Coroutine** m_pCo;

        // TODO:实现一个无锁化队列 双CAS WQueue<T> 单生产者单消费者 线程安全队列
        // 如果业务线程不使用阻塞队列时则使用这个队列
        // TODO:实现一个BlockingQueue<T>  供业务线程使用
        atomic_t mark[100];

        async_resume_fn m_asynResumeFn;
        void*  m_u;
    };


#define __WEDIS_INIT()

}



#endif //WREDISCLIENT_COROUTINEMANAGER_H
