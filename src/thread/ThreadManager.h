//
// Created by 58 on 2016/5/4.
//

#ifndef WREDISCLIENT_THREADMANAGER_H
#define WREDISCLIENT_THREADMANAGER_H

#include <wedis/base/thread/CThread.h>
#include <wedis/base/util/CommonType.h>
namespace wRedis {
#define MAX_THREAD_NUM			256

class CThread;
class ThreadManager {
public:
	ThreadManager(BOOL bNeedDelete = TRUE);
	~ThreadManager();

public:
	//启动线程池线程
	BOOL startAllThread();

	//停止所有线程
	BOOL stopAllThread();

	//增加一个线程
	BOOL addThread(CThread* pThread);

	//删除一个线程
	BOOL delThread(ThreadID tid);

	//根据线程ID取得线程
	CThread* getThread(ThreadID tid);

	//根据线程索引取得线程
	CThread* getThreadByIndex(UI32 uIndex);

	//得到线程数
	UI32 getThreadCount();

private:
	CThread* m_aThreads[MAX_THREAD_NUM];		//保存的线程
	UI32 m_uCount;						//线程数量
	BOOL m_bNeedDelete;					//是否需要释放
};
}

#endif //WREDISCLIENT_THREADMANAGER_H
