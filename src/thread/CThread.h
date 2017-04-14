//
// Created by 58 on 2016/5/4.
//

#ifndef WREDISCLIENT_CTHREAD_H
#define WREDISCLIENT_CTHREAD_H

#include <wedis/base/thread/Lock.h>
#include <wedis/base/util/CommonType.h>

namespace wRedis {

void* threadProcess(void* pParam);
class CThread {
public:
	struct ThreadStatus {
		enum type {
			READY,		//准备
			RUNNING,		//运行
			EXITING,		//正在退出
			INVALID,		//无效
		};
	};

public:
	CThread();
	virtual ~CThread();

public:
	// 线程创建
	virtual BOOL create();

	// 线程启动
	BOOL start();

	// 线程退出
	virtual void exit();

	virtual void forceExit();

	// 线程运行
	virtual void run();

	// 线程准备
	void ready();

	// 是否可以安全退出(有特殊需求的派生类需要实现此接口)
	virtual BOOL canSafeExit() {
		return TRUE;
	}

	// 获得线程状态
	ThreadStatus::type status() {
		return m_eStatus;
	}

	// 设置当前线程状态
	void status(ThreadStatus::type eStatus) {
		m_eStatus = eStatus;
	}

	// 获得线程指针
	HANDLE handle();

	// 获得线程id
	ThreadID getID() {
		return m_ThreadID;
	}

private:
	// 需要改成原子操作
	ThreadStatus::type m_eStatus;				// 线程当前状态，供外部判断使用
	ThreadID m_ThreadID;			    // 线程ID
	HANDLE m_hThread;			    // 线程handle
	Lock m_Lock;                 // 控制线程状态用到的锁
	Condition m_Cond;				    // 线程状态
};
}

#endif //WREDISCLIENT_CTHREAD_H
