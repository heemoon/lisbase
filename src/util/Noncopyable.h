//
// Created by 58 on 2016/4/18.
//

#ifndef WREDISCLIENT_NONCOPYABLE_H
#define WREDISCLIENT_NONCOPYABLE_H

#include <wedis/base/util/Global.h>

namespace wRedis {
class Noncopyable {
public:
	Noncopyable() {
	}
	virtual ~Noncopyable() {
	}
private:
	DISALLOW_COPY_AND_ASSIGN(Noncopyable);
};
}

#endif //WREDISCLIENT_NONCOPYABLE_H
