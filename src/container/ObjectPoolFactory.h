//
// Created by 58 on 2016/5/12.
//

#ifndef WREDISCLIENT_OBJECTPOOLFACTORY_H
#define WREDISCLIENT_OBJECTPOOLFACTORY_H

#include <wedis/base/util/Noncopyable.h>

namespace wRedis {

template<typename T>
class ObjectPoolFactory : public Noncopyable {
public:
	ObjectPoolFactory() {}
	virtual ~ObjectPoolFactory() {}
public:
	// create
	virtual T* 	 makeObject()            = 0;
	// invalid object
	virtual void destroyObject(T& var)   = 0;
	// test on borrow object
	virtual BOOL validateObject(T& var)  = 0;
	// on borrow
	virtual BOOL activateObject(T& var)  = 0;
	// on return
	virtual BOOL passivateObject(T& var) = 0;
};

}



#endif //WREDISCLIENT_OBJECTPOOLFACTORY_H
