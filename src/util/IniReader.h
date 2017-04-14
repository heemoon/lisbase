/*
 * IniReader.h
 *
 *  Created on: 2016年7月15日
 *      Author: 58
 */

#ifndef WEDIS_BASE_UTIL_INIREADER_H_
#define WEDIS_BASE_UTIL_INIREADER_H_

#include <wedis/base/util/Noncopyable.h>

namespace wRedis {

class IniReader : public Noncopyable {
enum {
	INI_SECT_LEN = 256,		//段长
	INI_VALUE_LEN = 1024,		//值长
	INI_KEY_LEN = 256,		//键长
	INI_LINE_LEN = 2048,		//行长
};

public:
	IniReader();
	IniReader(const I8 *pFileName);
	virtual ~IniReader();

public:
	// 打开文件
	BOOL			openFile(const I8* pFileName);
	// 保存文件
	BOOL			saveFile();
	// 读取整数，成功返回键值，失败返回-1
	I32				readInt(const I8* pSect, const I8* pKey) const;
	// 读取浮点数，成功返回键值，失败返回-1
	REAL32			readFloat(const I8* pSect, const I8* pKey) const;
	// 读取字符串，键值通过参数pOut获取，返回值表示失败或成功
	BOOL			readString(const I8* pSect, const I8* pKey, I8* pOut, I32 Len) const;
	// 写入整数
	BOOL			writeInt(const I8* pSect, const I8* pKey, I32 iValue);
	// 写入浮点数
	BOOL			writeFloat(const I8* pSect, const I8* pKey, REAL32 fValue);
	// 写入字符串
	BOOL			writeString(const I8* pSect, const I8* pKey, const I8* pValue);
public:
	/************************************************************************/
	// 添加段
	/************************************************************************/
	void			addSect(const I8* pSect);

	/************************************************************************/
	// 添加键值对
	/************************************************************************/
	BOOL			addKeyValuePair(const I8* pSect, const I8* pKey, const I8* pValue);

	/************************************************************************/
	// 修改键值对
	/************************************************************************/
	BOOL			modifyKeyValuePair(const I8* pSect, const I8* pKey, const I8* pValue);

private:
	/************************************************************************/
	// 初始化段索引，让段指针数组成员指向各段的内存偏移位置
	/************************************************************************/
	void			__initSectIndex();

	/************************************************************************/
	// 查找段的偏移位置，失败返回-1
	/************************************************************************/
	I32				__findSect(const I8* pSect) const;

	/************************************************************************/
	// 查找键的偏移位置，失败返回-1
	/************************************************************************/
	I32				__findKey(I32 Begin, const I8* pKey) const;

	/************************************************************************/
	// 找到下一行的起始位置
	/************************************************************************/
	I32				__findNextLineBegin(I32 CurPos) const;

	/************************************************************************/
	// 获取键对应的值
	/************************************************************************/
	BOOL			__getValue(I32 Begin, I8* pValue, I32 Len) const;

	/************************************************************************/
	// 通过位置获得其在段位置数组的下标
	/************************************************************************/
	I32				__getIndexByPosition(I32 Pos) const;

	/************************************************************************/
	// 获取一行，返回下一行的起始位置
	/************************************************************************/
	I32				__getLine(I8* pBuff, I32 BuffLen, I32 BeginPos) const;

	/************************************************************************/
	// 解密
	/************************************************************************/
	BOOL			__decrypt(I8* pValue, I32 Len) const;

private:
	I8     m_FileName[MAX_FILE_NAME_LEN];
	I32    m_DataLen;
	I8*    m_pData;
	I32*   m_aSectIndex;
	I32    m_SectCount;
};

} /* namespace wRedis */

#endif /* WEDIS_BASE_UTIL_INIREADER_H_ */
