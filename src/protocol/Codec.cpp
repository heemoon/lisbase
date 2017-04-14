/*
 * Codec.cpp
 *
 *  Created on: 2016年5月13日
 *      Author: lixiwen
 */

#include <wedis/base/protocol/Codec.h>

namespace wRedis {

Codec::Codec() {
	cleanErr();
}

Codec::~Codec() {
	cleanErr();
}

} /* namespace wRedis */
