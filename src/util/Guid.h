//.h文件
#ifndef _GUID_H_
#define _GUID_H_


#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Global.h>

namespace wRedis {


// GUID高位偏移
#define GUID_HIGH_SHIFT				32

//类型最大取值
#define MAX_FIRST_SUB_TYPE			16
#define MAX_SECOND_SUB_TYPE			256
#define MAX_GUID_TYPE				65536 

//计算服务器组ID使用
#define WORLD_ID					0xFFFF000000000000
#define CLEAR_WORLD_ID				0x0000FFFFFFFFFFFF
#define WORLD_ID_SHIFT				48

//计算类型ID使用
#define TYPE_ID						0x0000FFFF00000000
#define CLEAR_TYPE_ID				0xFFFF0000FFFFFFFF
#define TYPE_ID_SHIFT				32

//计算唯一ID使用
#define UNIQUE_ID					0x00000000FFFFFFFF
#define CLEAR_UNIQUE_ID				0xFFFFFFFF00000000

//最大取值
#define MAX_VALUE					0xFFFFFFFFFFFFFFFF
#define MAX_UNIQUE_ID				0xFFFFFFFF

//动态GUID初始值
#define DYNAMIC_GUID_INITIAL_VALUE	100000000

struct GUID
{
public:
	UI64	m_ullValue;

public:

	GUID()
	{
		cleanUp();
	}

	GUID(UI64 value)
	{
		m_ullValue = value;
	}

	GUID(UI32 nHigh, UI32 nLow)
	{
		m_ullValue = (((nHigh & MAX_VALUE) << GUID_HIGH_SHIFT)) + nLow;
	}

	//清理
	void cleanUp()
	{
		m_ullValue = MAX_VALUE;
	}

	//检查
	BOOL isValid() const
	{
		if (m_ullValue == (UI64)INVALID_VALUE)
		{
			return FALSE;
		}
		return m_ullValue > 0 ? TRUE : FALSE;
	}

	UI32 getHighID() const
	{
		return (UI32)((m_ullValue >> GUID_HIGH_SHIFT) & UNIQUE_ID);
	}

	//唯一标示
	UI32 getID() const
	{
		return (UI32)(m_ullValue & UNIQUE_ID);
	}
	void setID(UI32 iID)
	{
		m_ullValue = (m_ullValue & CLEAR_UNIQUE_ID) | iID;
	}

	GUID& operator=(GUID const& guid)
	{
		m_ullValue = guid.m_ullValue;
		return *this;
	}

	GUID& operator=(UI64& value)
	{
		m_ullValue = value;
		return *this;
	}

	BOOL operator==(GUID& guid)
	{
		if (m_ullValue == guid.m_ullValue)
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL operator==(const GUID& guid) const
	{
		if (m_ullValue == guid.m_ullValue)
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL operator!=(const GUID& guid) const
	{
		if (m_ullValue != guid.m_ullValue)
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL operator > (GUID& guid)
	{
		if (m_ullValue > guid.m_ullValue)
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL operator < (GUID& guid)
	{
		if (m_ullValue < guid.m_ullValue)
		{
			return TRUE;
		}
		return FALSE;
	}

	UI16 getSize() const
	{
		return sizeof(m_ullValue);
	}
};

}

#endif
