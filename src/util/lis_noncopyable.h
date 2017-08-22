// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_UTIL_NONCOPYABLE_H__
#define __LISBASE_UTIL_NONCOPYABLE_H__

#include "lis_common_macro.h"

namespace lisbase {
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

#endif
