// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_CRC_H__
#define __LISBASE_CRC_H__


#include <stdint.h>
#include <stddef.h>
#include <stdint.h>

namespace lisbase {

class Crc {
public:
	static uint16_t crc16(const char* buf, size_t len);
	static uint32_t crc32(const char* buf, size_t len);
};

}

#endif

