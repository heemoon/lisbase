//
// Created by 58 on 2016/4/18.
//

#ifndef WREDISCLIENT_TOOLS_H
#define WREDISCLIENT_TOOLS_H

#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Global.h>

namespace wRedis {

class Tools {
public:
	////////////////////////////////////////////////////
	// 字符串操作
	////////////////////////////////////////////////////

	// Strcpy, Strncpy (宽)字符串复制
	// 注意带n的函数传递的长度为字符串的长度
	static I8* Strncpy(I8* pDestination, const I8* pSource, I32 nLength);

	// Strcpy_s (宽)字符串复制安全版本，
	// 注意！destSizeInByte为目的缓冲区的sizeof，而非strlen
	static I8* Strcpy_s(I8* pDestination, const I8* pSource,
			const I32 destSizeInByte);

	// Strcat, Strncat (宽)字符串组合
	static I8* Strcat(I8* pDestination, const I8* pSource);
	static I8* Strncat(I8* pDestination, const I8* pSource, I32 nLength);

	// Strcmp, Strncmp (宽)字符串比较
	static I32 Strcmp(const I8* pSource1, const I8* pSource2);
	static I32 Strncmp(const I8* pSource1, const I8* pSource2, I32 nLength);
	static I32 Strcasecmp(const I8* pSource1, const I8* pSource2);

	// Strlen 计算(宽)字符串长度
	static I32 Strlen(const I8* pSource);

	// split
	static I8** StrSplit(const I8* pSource, I32 nLen, const I8* pSep,
			I32 nSeplen, I32* pCount);
	static void StrFreeSplit(I8** tokens, I32 nCnt);

	static I32 Utf8len(const I8* pSource);
	// 将字符串插入尾部UTF8
	static BOOL InsertTailUtf8(I8* pDest, I32 DestLen, const I8 *pContext);
	// UTF8字符替换
	static BOOL ReplaceUtf8(I8* pString, UI8 from, UI8 to);

	//传入一个utf编码的字符串,找到下-个字符并返回指针,如果没有返回NULLPTR
	static const I8* GetNextCharacter(const I8* p, I32 len);

	//传入一个utf编码的字符串,找到下n个字符并返回指针,如果没有返回NULLPTR
	static const I8* GetNextCharacter(const I8* p, I32 len, I32 characterCount);

	//从一个UTF字符中的得到该字符的Unicode 值
	//请注意这里说的是一个UTF字符,而不是UTF字符串
	//请保证传入的参数的正确, 及p是一个正确的UTF8编码字符,以及len的长度正确
	//关于Unicode与UTF8 以及UTF16 UTF32的关系
	//请阅读相关资料
	static UI32 GetUnicodeFromUTF8Character(const I8* p, I32 len);

	//UTF16和UTF8互转(只支持小于1024的字符转换)
	static I32 ConvertUTF16UTF8(const I8* utf16, const I32 utf16Size, I8* utf8,
			const I32 utf8Size);
	static I32 ConvertUTF8UTF16(const I8* utf8, const I32 utf8Size, I8* utf16,
			const I32 utf16Size);

	//UTF8名字是否有效
	static BOOL IsValidUTF8Name(const I8* utf8);
	static BOOL IsValidUTF16CharValue(UI16 CharValue);

	//根据策划配置检查UTF8编码的字符串是否合法
	static BOOL IslegalString(const I8* str, I32 len);

	// Strstr 查找(宽)字符串
	static const I8* Strstr(const I8* pHaystack, const I8* pNeedle);

	// Strrchr 字符串扫描  最后一个
	static I8* Strrchr(I8 *pSource, I32 c);

	// str find  第一个
	static I8* Strchr(I8* pSource, I8 c);

	static I32 Strfind(I8* pSource, I8 c);

	// 格式化字符串
	static I32 Snprintf(I8 *buf, UI32 maxlen, const I8 *format, ...);

	// Strtok 分解(宽)字符串
	static I8* Strtok(I8* pSource, const I8* pDelimit);
	static I8* Strtok_s(I8* pSource, const I8* pDelimit, I8** pContext);

	// Atoi 字符串转整型
	static I32 Atoi(const I8* pSource);
	static I64 Atoll(const I8* pSource);
	// Atou  字符串转无符号整型
	static UI32 Atou(const I8* pSource);
	// 字符串转整型
	static UI32 AtoIntEx(const I8* pSource, BOOL IsUnsigned);

	// Atof 字符串转浮点型
	static REAL64 Atof(const I8* pSource);

	static UI32 CountDigits(UI64 v);

	// Strtol 字符串转64位整型
	static I64 Strtoint64(const I8* pSource, I8** pEnd = NULLPTR,
			I32 nBase = 10);

	// Strtol 字符串转64位无符号整型
	static UI64 Strtouint64(const I8* pSource, I8** pEnd = NULLPTR, I32 nBase =
			10);

	static UI64 StrtoInt64Ex(const I8* pSource, BOOL IsUnsigned);

	// Strtod 字符串转浮点型
	static REAL64 Strtod(const I8* pSource, I8** pEnd);

	// Itoa 数字转(宽)字符串
	static I8* Itoa(I32 nValue, I8* pString, I32 nRadix);
	static I8* Utoa(UI32 uValue, I8* pString, I32 nRadix);
	static I8* Ltoa(I64 lValue, I8* pString, I32 nRadix);

	static I8* Ftoa(REAL64 dVal, I8* pString, UI32 maxLen);

	// 转小写
	static void ToLower(I8 *pStr);

	static BOOL EraseStrLeftBlank(I8* pSource);					// 删除字符串的左空格和Tab
	static BOOL EraseStrRightBlank(I8* pSource);				// 删除字符串的右空格和Tab
	static BOOL EraseStrBlank(I8* pSource);					// 删除字符串前后的空格和Tab

	static BOOL IsInteger(const I8* pSource);					// 判断字符串是否是整数

	static BOOL IsFloat(const I8* pSource);						// 判断字符串是否是浮点数

	static void Replace(I8* pStr, const I8* p, const I8 t);

	//十六进制字符串转换成10进制int
	//支持参数 大小写16进制 也支持不带 0x 开头的16进制数
	//成功返回int 否则返回 -1
	static I32 HexStringToInt(const I8* pHex);

	// 是否是空
	static BOOL IsBlank(const I8* pStr);

public:
	////////////////////////////////////////////////////
	// 文件操作
	////////////////////////////////////////////////////

	static I32 Chdir(const I8* pPath);							// 改变目录
	static I32 Mkdir(const I8* pPath);							// 创建目录
	static BOOL CreateDir(const I8* pDir);						// 创建目录
	static BOOL IsFileExists(const I8* filename);				// 判断文件是否存在
	static BOOL GetPwd(I8* pDir, I32 nSize);                    // 获取当前路径

public:
	////////////////////////////////////////////////////
	// 位运算
	////////////////////////////////////////////////////
	//取INT的某一位
	static I32 GetIntSomeBit(I32 src, I32 mask);

	//将INT的某一位置0或置1
	static I32 SetIntSomeBit(I32& scr, I32 mask, BOOL flag);

	// 强制转换成大端
	static I32 ConvertToBigEndian(I32 x);

	// to ui64
	static UI64 ToUI64(UI32 high, UI32 low);
	static UI32 GetHighSection(UI64 lValue);
	static UI32 GetLowSection(UI64 lValue);

};
}

#endif //WREDISCLIENT_TOOLS_H
