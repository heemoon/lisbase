#ifndef __LISBASE_UTIL_CRC_H__
#define __LISBASE_UTIL_CRC_H__


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

