//
// Created by 58 on 2016/4/18.
//

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/socket.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <dlfcn.h>
#include <math.h>
#include <stdarg.h>
#include <new>
#include <locale.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <iconv.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/Tools.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {

/////////////////////////////////////////////////////////////////////////////
//	- Strcpy, Strncpy
//	- 把pSource所指的由NULL结尾的字符串复制到pDestination所指的内存区域
//	- pDestination与pSource所指的区域不可以重叠
//	- 返回指向pDestination的指针
/////////////////////////////////////////////////////////////////////////////
I8* Tools::Strncpy(I8* pDestination, const I8* pSource, I32 nLength) {
	Assert(pDestination);
	Assert(pSource);

	if (nLength < 0) {
		return NULLPTR;
	}

	return ::strncpy(pDestination, pSource, (UI32) nLength);
}

I8* Tools::Strcpy_s(I8* pDestination, const I8* pSource,
		const I32 destSizeInByte) {
	Assert(pDestination);
	Assert(pSource);
	if (Strlen(pSource) >= destSizeInByte) {
		Assert(FALSE);
		return NULLPTR;
	}

	::strncpy(pDestination, pSource, destSizeInByte / sizeof(I8));
	pDestination[destSizeInByte / sizeof(I8) - 1] = 0;
	return pDestination;
}

/////////////////////////////////////////////////////////////////////////////
//	- Strcat, Strncat
//	- 把pSource所指的由NULL结尾的字符串添加到pDestination所指的内存区域
//	- 覆盖原来的'/0'并添加'/0'
//	- pDestination与pSource所指的区域不可以重叠
//	- 返回指向pDestination的指针
/////////////////////////////////////////////////////////////////////////////

I8* Tools::Strcat(I8* pDestination, const I8* pSource) {
	Assert(pDestination);
	Assert(pSource);
	return ::strcat(pDestination, pSource);
}

I8* Tools::Strncat(I8* pDestination, const I8* pSource, I32 nLength) {
	Assert(pDestination);
	Assert(pSource);
	if (nLength < 0) {
		return NULLPTR;
	}
	return ::strncat(pDestination, pSource, (UI32) nLength);

}

/////////////////////////////////////////////////////////////////////////////
//	- Strcmp, Strncmp
//	- 比较字符串pSource1和字符串pSource2
//	- 如果 (pSource1 < pSource2) 返回值 <0
//	- 如果 (pSource2 = pSource2) 返回值 =0
//	- 如果 (pSource1 > pSource2) 返回值 >0
/////////////////////////////////////////////////////////////////////////////

I32 Tools::Strcmp(const I8* pSource1, const I8* pSource2) {
	Assert(pSource1);
	Assert(pSource2);
	return ::strcmp(pSource1, pSource2);
}

I32 Tools::Strcasecmp(const I8* pSource1, const I8* pSource2) {
	Assert(pSource1);
	Assert(pSource2);
	return ::strcasecmp(pSource1, pSource2);
}

I32 Tools::Strncmp(const I8* pSource1, const I8* pSource2, I32 nLength) {
	Assert(pSource1);
	Assert(pSource2);
	if (nLength < 0) {
		return INVALID_VALUE;
	}
	return ::strncmp(pSource1, pSource2, (UI32) nLength);
}

/////////////////////////////////////////////////////////////////////////////
//	- Strlen
//	- 计算字符串pSource的长度
//	- 返回字符串的长度, 不包括结束符
/////////////////////////////////////////////////////////////////////////////

I32 Tools::Strlen(const I8* pSource) {
	Assert(pSource);
	return (I32) ::strlen(pSource);
}

I8** Tools::StrSplit(const I8* pSource, I32 nLen, const I8* pSep, I32 nSeplen,
		I32* pCount) {
	I32 elements = 0, slots = 30, start = 0;
	I8** tokens;

	if (NULLPTR == pSource || NULLPTR == pSep || NULLPTR == pCount)
		return NULLPTR;
	if (nSeplen < 1 || nLen < 0)
		return NULLPTR;

	tokens = (I8**) W_MALLOC(slots);
	if (tokens == NULLPTR)
		return NULLPTR;

	if (nLen == 0) {
		*pCount = 0;
		goto cleanup;
	}

	for (I32 j = 0; j < (nLen - (nSeplen - 1)); j++) {
		/* make sure there is room for the next element and the final one */
		if (slots < elements + 2) {
			I8** newtokens = NULLPTR;

			slots *= 2;
			newtokens = (I8**) W_REALLOC(tokens, slots);
			if (newtokens == NULLPTR)
				goto cleanup;
			tokens = newtokens;
		}
		/* search the separator */
		if ((nSeplen == 1 && *(pSource + j) == pSep[0])
				|| (memcmp(pSource + j, pSep, nSeplen) == 0)) {
			tokens[elements] = (I8*) W_MALLOC(j - start + 1);
			if (tokens[elements] == NULLPTR)
				goto cleanup;
			memcpy(tokens[elements], pSource + start, j - start);
			tokens[elements][j - start] = '\0';
			elements++;
			start = j + nSeplen;
			j = j + nSeplen - 1; /* skip the separator */
		}
	}
	/* Add the final element. We are sure there is room in the tokens array. */
	tokens[elements] = (I8*) W_MALLOC(nLen - start + 1);
	if (tokens[elements] == NULLPTR)
		goto cleanup;
	memcpy(tokens[elements], pSource + start, nLen - start);
	tokens[elements][nLen - start] = '\0';
	elements++;
	*pCount = elements;
	return tokens;

	cleanup: {
		for (I32 i = 0; i < elements; i++)
			W_FREE(tokens[i]);

		W_FREE(tokens);

		*pCount = 0;
		return NULLPTR;
	}
}

void Tools::StrFreeSplit(I8** tokens, I32 nCnt) {
	if (!tokens)
		return;
	while (nCnt--)
		W_FREE(tokens[nCnt]);
	W_FREE(tokens);
}

I8* Tools::Strchr(I8* pSource, I8 c) {
	Assert(pSource);
	return ::strchr(pSource, c);
}

I32 Tools::Strfind(I8* pSource, I8 c) {
	Assert(pSource);
	I8* p = ::strchr(pSource, c);
	if (NULLPTR == p) {
		return INVALID_VALUE;
	}

	return (I32) (p - pSource);
}

/////////////////////////////////////////////////////////////////////////////
//	- Utf8len
//	- 计算字符串pSource的长度
//	- 返回字符串的长度, 不包括结束符
/////////////////////////////////////////////////////////////////////////////

I32 Tools::Utf8len(const I8* s) {
	Assert(s);
	UI8 tcp;
	const UI8* buf = reinterpret_cast<const UI8*>(s);
	I32 count = 0;
	I32 len = Strlen(s);
	while ((len) > 0) {
		--len;
		tcp = *buf++;
		++count;

		if (tcp < 0x80) {
		} else if (tcp < 0xE0) {
			--len;
			++buf;
		} else if (tcp < 0xF0) {
			len -= 2;
			buf += 2;
		} else {
			len -= 3;
			buf += 3;
		}
	}
	if (len < 0) {
		//字符出错了,默认长度为0
		count = 0;
	}
	return count;
}

BOOL Tools::InsertTailUtf8(I8* pDest, I32 DestLen, const I8 *pContext) {
	if (pDest == NULLPTR || pContext == NULLPTR || DestLen <= 0) {
		return FALSE;
	}
	UI8 tcp;
	I8* buf = pDest;
	I32 len = Strlen(pDest);
	I32 contextLen = Strlen(pContext);
	I32 charLen = 0;
	I32 moveLen = 0;
	while ((len) > 0) {
		tcp = (UI8) (*buf);
		if (tcp < 0x80) {
			charLen = 1;
		} else if (tcp < 0xE0) {
			charLen = 2;
		} else if (tcp < 0xF0) {
			charLen = 3;
		} else {
			charLen = 4;
		}

		if ((DestLen - (moveLen + charLen) - 1) < contextLen) {
			break;
		} else {
			len -= charLen;
			buf += charLen;
			moveLen += charLen;
		}
	}

	if ((DestLen - moveLen - 1) >= contextLen) {
		Strncpy(pDest + moveLen, pContext, DestLen - moveLen);
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL Tools::ReplaceUtf8(I8* pString, UI8 from, UI8 to) {
	if (pString == NULLPTR) {
		return FALSE;
	}
	UI8 tcp;
	UI8* buf = reinterpret_cast<UI8*>(pString);
	I32 len = Strlen(pString);
	while ((len) > 0) {
		--len;
		tcp = *buf++;

		if (tcp < 0x80) {
			if (*(buf - 1) == from) {
				*(buf - 1) = to;
			}
		} else if (tcp < 0xE0) {
			--len;
			++buf;
		} else if (tcp < 0xF0) {
			len -= 2;
			buf += 2;
		} else {
			len -= 3;
			buf += 3;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	- Strstr
//	- 从pHaystack字符串中寻找pNeedle字符串第一次出现的位置
//	- 不比较结束符
//	- 返回第一次出现pNeedle位置的指针, 如果没有找到返回NULLPTR
/////////////////////////////////////////////////////////////////////////////

const I8* Tools::Strstr(const I8* pHaystack, const I8* pNeedle) {
	Assert(pHaystack);
	Assert(pNeedle);
	return ::strstr(pHaystack, pNeedle);
}

I8* Tools::Strrchr(I8 *pSource, I32 c) {
	Assert(pSource);
	Assert(c);
	return ::strrchr(pSource, c);
}

I32 Tools::Snprintf(I8 *pBuffer, UI32 maxlen, const I8 *pFormat, ...) {
	Assert(pBuffer);
	Assert(pFormat);

	I32 result;
	va_list argptr;
	va_start(argptr, pFormat);

	result = vsnprintf(pBuffer, maxlen, pFormat, argptr);

	va_end(argptr);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
//	- Strtok
//	- 分解字符串为一组标记串, pSource为分解的字符串, pDelimit为分隔符
//	- 首次调用时, pSource必须指向要分解的字符串,
//	- 之后的调用要把pSource设置成NULL
/////////////////////////////////////////////////////////////////////////////
I8* Tools::Strtok(I8* pSource, const I8* pDelimit) {
	Assert(pDelimit);
	return ::strtok(pSource, pDelimit);
}

I8* Tools::Strtok_s(I8* pSource, const I8* pDelimit, I8** pContext) {
	Assert(pDelimit);
	return ::strtok_r(pSource, pDelimit, pContext);
}

/////////////////////////////////////////////////////////////////////////////
//	- Atoi
//	- 将pSource指向的字符串转换为整型
//	- 跳过pSource前面的空格字符, 直到遇到数字或正负符号开始转换
//	- 遇到非数字或者字符串结束符结束转换
/////////////////////////////////////////////////////////////////////////////
I32 Tools::Atoi(const I8* pSource) {
	Assert(pSource);
	return ::atoi(pSource);
}

I64 Tools::Atoll(const I8* pSource) {
	Assert(pSource);
	return ::atoll(pSource);
}

UI32 Tools::Atou(const I8* pSource) {
	if (pSource == NULLPTR) {
		return 0;
	}
	UI32 Result = 0;
	sscanf(pSource, "%u", &Result);
	return Result;
}

UI32 Tools::AtoIntEx(const I8* pSource, BOOL IsUnsigned) {
	if (IsUnsigned == TRUE) {
		return (UI32) Atou(pSource);
	} else {
		return (UI32) Atoi(pSource);
	}
}

/////////////////////////////////////////////////////////////////////////////
//	- Itoa
//	- 将nValue整型数字转为字符存放到pSource指向的字符串空间(外部分配)
//	- nRadix是转换数字是所用的进制(二进制, 八进制, 十进制, 十六进制)
/////////////////////////////////////////////////////////////////////////////
I8* Tools::Itoa(I32 nValue, I8* pString, I32 nRadix) {
	Assert(pString);

	I8* pEnd = pString;
	I8* pStart = pString;
	I8 cTemp;
	// 待转换的数字等于0
	if (0 == nValue) {
		pString[0] = '0';
		pString[1] = 0;
		return pString;
	}

	// 待转换的数字是负数, 进制为十进制
	if (nValue < 0 && 10 == nRadix) {
		// 设置符号
		pString[0] = '-';
		pEnd++;
		pStart++;
		nValue = -nValue;
	}

	// 根据nRadix进制转换数字, 倒序字符串
	while (nValue != 0) {
		I32 nMode = nValue % nRadix;
		nValue = nValue / nRadix;
		*pEnd++ = (nMode < 10) ? '0' + nMode : 'a' + nMode - 10;
	}
	*pEnd-- = 0;

	// 反转字符串
	while (pEnd > pStart) {
		cTemp = *pEnd;
		*pEnd = *pStart;
		*pStart = cTemp;
		pEnd--;
		pStart++;
	}
	return pString;
}

I8* Tools::Utoa(UI32 uValue, I8* pString, I32 nRadix) {
	Assert(pString);

	I8* pEnd = pString;
	I8* pStart = pString;
	I8 cTemp;
	// 待转换的数字等于0
	if (0 == uValue) {
		pString[0] = '0';
		pString[1] = 0;
		return pString;
	}

	// 根据nRadix进制转换数字, 倒序字符串
	while (uValue != 0) {
		UI32 uMode = uValue % (UI32) nRadix;
		uValue = uValue / (UI32) nRadix;
		*pEnd++ = (uMode < 10) ? '0' + uMode : 'a' + uMode - 10;
	}
	*pEnd-- = 0;

	// 反转字符串
	while (pEnd > pStart) {
		cTemp = *pEnd;
		*pEnd = *pStart;
		*pStart = cTemp;
		pEnd--;
		pStart++;
	}
	return pString;
}

I8* Tools::Ltoa(I64 lValue, I8* pString, I32 nRadix) {
	Assert(pString);

	I8* pEnd = pString;
	I8* pStart = pString;
	I8 cTemp;
	// 待转换的数字等于0
	if (0 == lValue) {
		pString[0] = '0';
		pString[1] = 0;
		return pString;
	}

	// 待转换的数字是负数, 进制为十进制
	if (lValue < 0 && 10 == nRadix) {
		// 设置符号
		pString[0] = '-';
		pEnd++;
		pStart++;
		lValue = -lValue;
	}

	// 根据nRadix进制转换数字, 倒序字符串
	while (lValue != 0) {
		I64 lMode = lValue % nRadix;
		lValue = lValue / nRadix;
		*pEnd++ = (lMode < 10) ? '0' + lMode : 'a' + lMode - 10;
	}
	*pEnd-- = 0;

	// 反转字符串
	while (pEnd > pStart) {
		cTemp = *pEnd;
		*pEnd = *pStart;
		*pStart = cTemp;
		pEnd--;
		pStart++;
	}
	return pString;
}

I8* Tools::Ftoa(REAL64 dVal, I8* pString, UI32 maxLen) {
	Assert(pString);
	Snprintf(pString, maxLen, "%.4f", dVal);
	return pString;
}

/////////////////////////////////////////////////////////////////////////////
//	- Atof
//	- 将pSource指向的字符串转换为浮点型
/////////////////////////////////////////////////////////////////////////////
REAL64 Tools::Atof(const I8* pSource) {
	Assert(pSource);
	return ::atof(pSource);
}

/////////////////////////////////////////////////////////////////////////////
//	- CountDigits
/////////////////////////////////////////////////////////////////////////////
UI32 Tools::CountDigits(UI64 v) {
	UI32 result = 1;
	for (;;) {
		if (v < 10)
			return result;
		if (v < 100)
			return result + 1;
		if (v < 1000)
			return result + 2;
		if (v < 10000)
			return result + 3;
		v /= 10000U;
		result += 4;
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////
//	- Strtoint64
//	- 将pSource指向的字符串转换为64位整型, nBase代表的进制方式
//////////////////////////////////////////////////////////////////////
I64 Tools::Strtoint64(const I8* pSource, I8** pEnd, I32 nBase) {
	return ::strtoll(pSource, pEnd, nBase);
}

/////////////////////////////////////////////////////////////////////////////
//	- StrToUInt64
//	- 将pSource指向的字符串转换为64位无符号整型, nBase代表的进制方式
//////////////////////////////////////////////////////////////////////
UI64 Tools::Strtouint64(const I8* pSource, I8** pEnd, I32 nBase) {
	return ::strtoull(pSource, pEnd, nBase);
}

UI64 Tools::StrtoInt64Ex(const I8* pSource, BOOL IsUnsigned) {
	if (IsUnsigned == TRUE) {
		return (UI64) Strtouint64(pSource);
	} else {
		return (UI64) Strtoint64(pSource);
	}
}

/////////////////////////////////////////////////////////////////////////////
//	- Strtod
//	- 将pSource指向的字符串转换为浮点型
/////////////////////////////////////////////////////////////////////////////
REAL64 Tools::Strtod(const I8* pSource, I8** pEnd) {
	Assert(pSource);
	return ::strtod(pSource, pEnd);
}

/////////////////////////////////////////////////////////////////////////////
//	- EraseStrLeftBlank
//	- 删除字符串左侧的空格字符
/////////////////////////////////////////////////////////////////////////////
BOOL Tools::EraseStrLeftBlank(I8 *pSrc) {
	Assert(pSrc);

	if (pSrc == NULLPTR) {
		return FALSE;
	}

	while (*pSrc == ' ' || *pSrc == '\t') {
		memmove(pSrc, pSrc + 1, (UI32) Tools::Strlen(pSrc));
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	- EraseStrRightBlank
//	- 删除字符串右侧的空格字符
/////////////////////////////////////////////////////////////////////////////
BOOL Tools::EraseStrRightBlank(I8 *pSrc) {
	Assert(pSrc);

	if (pSrc == NULLPTR) {
		return FALSE;
	}

	I8* pTail = pSrc + Tools::Strlen(pSrc) - 1;
	while (*pTail == ' ' || *pTail == '\t') {
		*pTail = '\0';
		pTail = pSrc + Tools::Strlen(pSrc) - 1;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	- EraseStrBlank
//	- 删除字符串中的空格字符
/////////////////////////////////////////////////////////////////////////////
BOOL Tools::EraseStrBlank(I8 *pSrc) {
	Assert(pSrc);

	BOOL bErase1 = EraseStrLeftBlank(pSrc);
	BOOL bErase2 = EraseStrRightBlank(pSrc);
	return bErase1 && bErase2;
}

/////////////////////////////////////////////////////////////////////////////
//	- IsInteger
//	- 判断是否为整型字符
/////////////////////////////////////////////////////////////////////////////
BOOL Tools::IsInteger(const I8* pSrc) {
	Assert(pSrc);

	if (pSrc == NULLPTR || Tools::Strlen(pSrc) <= 0) {
		return FALSE;
	}
	if (pSrc[0] == '-') {
		if (Tools::Strlen(pSrc) == 1) {
			return FALSE;
		}
		++pSrc;
	}
	while (*pSrc != '\0') {
		if (*pSrc < '0' || *pSrc > '9') {
			return FALSE;
		}
		++pSrc;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	- IsFloat
//	- 判断是否为浮点型字符
/////////////////////////////////////////////////////////////////////////////
BOOL Tools::IsFloat(const I8* pSrc) {
	Assert(pSrc);

	if (pSrc == NULLPTR || Tools::Strlen(pSrc) <= 0) {
		return FALSE;
	}
	if (pSrc[0] == '-') {
		if (Tools::Strlen(pSrc) == 1) {
			return FALSE;
		}
		++pSrc;
	}
	while (*pSrc != '\0') {
		if ((*pSrc >= '0' && *pSrc <= '9') || *pSrc == '.') {
			++pSrc;
			continue;
		} else {
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//	- Chdir
//	- 改变目录
/////////////////////////////////////////////////////////////////////////////
I32 Tools::Chdir(const I8* pPath) {
	I32 retValue = INVALID_VALUE;
	retValue = ::chdir(pPath);
	return retValue;
}

/////////////////////////////////////////////////////////////////////////////
//	- Mkdir
//	- 创建目录
/////////////////////////////////////////////////////////////////////////////
I32 Tools::Mkdir(const I8* pPath) {
	return ::mkdir(pPath, 0);
}

/////////////////////////////////////////////////////////////////////////////
//	- CreateDir
//	- 建立文件夹
/////////////////////////////////////////////////////////////////////////////
BOOL Tools::CreateDir(const I8* pDir) {
	Assert(pDir);

	if (pDir == NULLPTR) {
		return FALSE;
	}
	I32 pathLen = Tools::Strlen(pDir);
	if (pathLen <= 0 || pathLen > MAX_FILE_NAME_LEN) {
		return FALSE;
	}
	//检测目录是否存在
	if (Chdir(pDir) == 0) {
		return TRUE;
	}

	//创建目录
	if (Mkdir(pDir) == 0) {
		return TRUE;
	}

	//取得上一级目录

	I8 ParentDir[MAX_FILE_NAME_LEN] = { 0 };
	memcpy(ParentDir, pDir, (UI32) pathLen);

	I32 iTail = pathLen;
	I8* pCh = ParentDir + iTail - 1;
	while (*pCh != '/') {
		--pCh;
	}

	*pCh = '\0';

	// 创建父目录
	if (CreateDir(ParentDir) == FALSE) {
		return FALSE;
	}
	// 创建本目录
	return CreateDir(pDir);
}

void Tools::ToLower(I8 *pStr) {
	if (pStr == NULLPTR) {
		return;
	}
	I32 StrLen = (I32) ::strlen(pStr);
	for (I32 i = 0; i < StrLen; ++i) {
		pStr[i] = (I8) ::tolower(pStr[i]);
	}
}

void Tools::Replace(I8* pStr, const I8* p, const I8 t) {
	if (NULLPTR == pStr || NULLPTR == p) {
		return;
	}
	const I8* pFindResult = Strstr(pStr, p);
	if (NULLPTR == pFindResult) {
		return;
	}
	const I32 pCharCount = Utf8len(p);
	const I32 pLen = Strlen(p);

	while (NULLPTR != pFindResult) {
		I32 pStrLen = Strlen(pStr);
		I32 replaceStrIndex = static_cast<I32>(pFindResult - pStr);
		for (I32 i = 0; i < pCharCount; ++i) {
			pStr[i + replaceStrIndex] = t;
		}

		if (pCharCount != pLen) {
			for (I32 i = 0; i < pStrLen - replaceStrIndex - pCharCount; ++i) {
				pStr[replaceStrIndex + pCharCount + i] = pStr[replaceStrIndex
						+ pLen + i];
			}
		}
		pFindResult = Strstr(pStr, p);
	}

}

const I8* Tools::GetNextCharacter(const I8* p, I32 len) {
	if (NULLPTR == p) {
		return NULLPTR;
	}

	if (0 == p[0]) {
		return p;
	}
	I32 targetLen = 0;
	UI8 tcp = *(reinterpret_cast<const UI8*>(p));
	if (tcp < 0x80) {
		targetLen = 1;
	} else if (tcp < 0xE0) {
		targetLen = 2;
	} else if (tcp < 0xF0) {
		targetLen = 3;
	} else {
		targetLen = 4;
	}
	if (len < targetLen) {
		return NULLPTR;
	}

	return p + targetLen;
}

const I8* Tools::GetNextCharacter(const I8* p, I32 len, I32 characterCount) {
	I32 strLength = len;
	const I8* nextChartet = p;
	for (I32 i = 0; i < characterCount; ++i) {
		strLength = len - (I32) (nextChartet - p);
		nextChartet = GetNextCharacter(nextChartet, strLength);
		if (NULLPTR == nextChartet) {
			return NULLPTR;
		}
	}
	return nextChartet;
}

UI32 Tools::GetUnicodeFromUTF8Character(const I8* p, I32 len) {
	if (NULLPTR == p || len <= 0 || len > 6) {
		return INVALID_UI32;
	}

	if (1 == len) {
		UI32 result = static_cast<UI32>(p[0]);
		return result;
	}
	UI32 result = 0;
	const UI8* src = reinterpret_cast<const UI8*>(p);

	I32 idx = 0;
	UI8 cu = src[idx++];
	UI32 cp = 0;
	if (cu < 0x80) {
		cp = static_cast<UI32>(cu);
	} else if (cu < 0xE0) {
		cp = (UI32) ((cu & 0x1F) << 6);
		cp |= (UI32) (src[idx++] & 0x3F);
	} else if (cu < 0xF0) {
		cp = (UI32) ((cu & 0x0F) << 12);
		cp |= (UI32) ((src[idx++] & 0x3F) << 6);
		cp |= (UI32) (src[idx++] & 0x3F);
	} else if (cu < 0xF8) {
		cp = (UI32) ((cu & 0x07) << 18);
		cp |= (UI32) ((src[idx++] & 0x3F) << 12);
		cp |= (UI32) ((src[idx++] & 0x3F) << 6);
		cp |= (UI32) (src[idx++] & 0x3F);
	} else if (cu < 0xFC) {
		cp = (UI32) ((cu & 0x07) << 24);
		cp |= (UI32) ((src[idx++] & 0x3F) << 18);
		cp |= (UI32) ((src[idx++] & 0x3F) << 12);
		cp |= (UI32) (src[idx++] & 0x3F << 6);
		cp |= (UI32) (src[idx++] & 0x3F);
	} else {
		cp = (UI32) ((cu & 0x07) << 30);
		cp |= (UI32) ((src[idx++] & 0x3F) << 24);
		cp |= (UI32) ((src[idx++] & 0x3F) << 18);
		cp |= (UI32) (src[idx++] & 0x3F << 12);
		cp |= (UI32) (src[idx++] & 0x3F << 6);
		cp |= (UI32) (src[idx++] & 0x3F);
	}
	result = cp;
	return result;
}

//UTF16和UTF8互转(只支持小于1024的字符转换)
I32 Tools::ConvertUTF16UTF8(const I8* utf16, const I32 utf16Size, I8* utf8,
		const I32 utf8Size) {
	static const I32 tempBufferSize = 1024;
	if (!utf16 || !utf8 || utf8Size <= 0 || utf16Size <= 0
			|| utf16Size >= tempBufferSize) {
		return INVALID_VALUE;
	}
	I32 utf16Len = utf16Size / 2;
	memset(utf8, 0, static_cast<UI32>(utf8Size));

	static I8 tempBuffer[tempBufferSize] = { 0 };
	//一定要保证有结束符
	memset(tempBuffer, 0, sizeof(tempBuffer));
	memcpy(tempBuffer, utf16, utf16Size);

	I32 result = INVALID_VALUE;
	iconv_t env = iconv_open("UTF-8", "UTF-16LE");
	if ((iconv_t)INVALID_VALUE == env) {
		return INVALID_VALUE;
	}

	I8 *pIn = tempBuffer;
	UI64 insize = utf16Size;
	I8 *pOut = utf8;
	UI64 outsize = utf8Size;
	result = static_cast<I32>(iconv(env, &pIn, (size_t*) (&insize), &pOut,
			(size_t*) (&outsize)));
	if (INVALID_VALUE == result) {
		return INVALID_VALUE;
	}
	iconv_close(env);
	return Strlen(utf8);
}

//UTF16和UTF8互转(只支持小于1024的字符转换)
I32 Tools::ConvertUTF8UTF16(const I8* utf8, const I32 utf8Size, I8* utf16,
		const I32 utf16Size) {
	static const I32 tempBufferSize = 1024;
	if (!utf16 || !utf8 || utf8Size <= 0 || utf16Size <= 0
			|| utf8Size >= tempBufferSize) {
		return INVALID_VALUE;
	}
	memset(utf16, 0, static_cast<UI32>(utf16Size));

	static I8 tempBuffer[tempBufferSize] = { 0 };
	//一定要保证有结束符
	memset(tempBuffer, 0, sizeof(tempBuffer));
	memcpy(tempBuffer, utf8, utf8Size);

	I32 result = INVALID_VALUE;
	iconv_t env = iconv_open("UTF-16LE", "UTF-8");
	if ((iconv_t)INVALID_VALUE == env) {
		return INVALID_VALUE;
	}

	I8 *pIn = tempBuffer;
	UI64 insize = utf8Size;
	I8 *pOut = utf16;
	UI64 outsize = utf16Size;
	result = static_cast<I32>(iconv(env, &pIn, (size_t*) (&insize), &pOut,
			(size_t*) (&outsize)));
	if (INVALID_VALUE == result) {
		return INVALID_VALUE;
	}
	iconv_close(env);
	return utf16Size - outsize;
}

BOOL Tools::IsValidUTF8Name(const I8* utf8) {
	if (utf8 == NULLPTR) {
		return FALSE;
	}

	I32 UTF8NameLen = Strlen(utf8);
	if (UTF8NameLen <= 0) {
		return FALSE;
	}

	I8 UTF16Name[1024] = { 0 };
	I32 UTF16NameLen = ConvertUTF8UTF16(utf8, UTF8NameLen, UTF16Name,
			sizeof(UTF16Name));
	if (UTF16NameLen <= 0 || (UTF16NameLen & 0x01) != 0) {
		return FALSE;
	}

	for (I32 i = 0; i < UTF16NameLen; i += 2) {
		UI16 UTF16Value = 0;
		memcpy(&UTF16Value, &UTF16Name[i], sizeof(UTF16Value));
		if (IsValidUTF16CharValue(UTF16Value) == FALSE) {
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Tools::IsValidUTF16CharValue(UI16 CharValue) {
	// a-z
	if (CharValue >= (UI16) 'a' && CharValue <= (UI16) 'z') {
		return TRUE;
	}
	// A-Z
	if (CharValue >= (UI16) 'A' && CharValue <= (UI16) 'Z') {
		return TRUE;
	}
	// 0-9
	if (CharValue >= (UI16) '0' && CharValue <= (UI16) '9') {
		return TRUE;
	}
	// 中文
	if (CharValue >= (UI16) 0x4e00 && CharValue <= (UI16) 0x9fa5) {
		return TRUE;
	}
	return FALSE;
}

BOOL Tools::IslegalString(const I8* str, I32 len) {
	const I8* charBegin = str;
	const I8* charEnd = GetNextCharacter(str, len);
	while (NULLPTR != charEnd && charBegin != charEnd) {
		UI32 unicode = GetUnicodeFromUTF8Character(charBegin,
				(I32) (charEnd - charBegin));
		if (INVALID_UI32 == unicode) {
			return FALSE;
		}
		charBegin = charEnd;
		charEnd = GetNextCharacter(charBegin, (I32) (len - (charBegin - str)));
	}
	return TRUE;
}

BOOL Tools::IsFileExists(const I8* filename) {
	return (access(filename, 0) == 0);
}

BOOL Tools::GetPwd(I8* pDir, I32 nSize) {
	return (getcwd(pDir, nSize) != NULLPTR);
}

I32 Tools::GetIntSomeBit(I32 src, I32 mask) {
	return src >> mask & 1;
}

I32 Tools::SetIntSomeBit(I32& src, I32 mask, BOOL flag) {
	if (flag) {
		src |= (0x1 << mask);
	} else {
		src &= ~(0x1 << mask);
	}
	return src;
}

I32 Tools::ConvertToBigEndian(I32 x) {
	// Check is Bigendian
	I32 testVal = 1;
	I8 testBuf[4] = {0};
	memcpy(&testBuf, &testVal, sizeof(I32));

	if (0 == testBuf[0]) {
		return x;
	}

	return (((x)& 0xff) << 24) + (((x >> 8) & 0xff) << 16) + (((x >> 16) & 0xff) << 8) + (((x >> 24) & 0xff));
}

UI64 Tools::ToUI64(UI32 high, UI32 low) {
    UI64 lRet = 0;
    UI64 lTempHigh = (UI64)high;
    lRet |= (lTempHigh << 32);
    lRet |= low;
    return lRet;
}

UI32 Tools::GetHighSection(UI64 lValue) {
    return (UI32)((lValue >> 32) & (~0));
}

UI32 Tools::GetLowSection(UI64 lValue) {
    return (UI32)(lValue & 0x00000000FFFFFFFF);
}


I32 Tools::HexStringToInt(const I8* pHex) {
	REAL64 nRet = 0;
	I32 len = Strlen(pHex);

	I32 nDelta = 0;

	if ('0' == *pHex) {
		if ('x' != *(pHex + 1)) {
			return 0;
		}
		nDelta = 2;
	}

	if (8 < (UI32) (len - nDelta))
		return -1;

	I32 i = 0;
	for (; i < len - nDelta; i++) {
		switch (*(pHex + i + nDelta)) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': {
			nRet += (*(pHex + i + nDelta) - '0')
					* pow((REAL64) 16, len - nDelta - i - 1);
			break;
		}
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f': {
			nRet += ((*(pHex + i + nDelta) - 'a') + 10)
					* pow((REAL64) 16, len - nDelta - i - 1);
			break;
		}
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F': {
			nRet += ((*(pHex + i + nDelta) - 'A') + 10)
					* pow((REAL64) 16, len - nDelta - i - 1);
			break;
		}
		default:
			return INVALID_VALUE;
		}
	}

	if ('\0' != *(pHex + i + nDelta)) {
		return INVALID_VALUE;
	}

	return (I32) nRet;
}

BOOL Tools::IsBlank(const I8 *pStr) {
	Assert(pStr);

	if(Strlen(pStr) == 0) return TRUE;

	I8* p = (I8*) pStr;
	while (*p) {
		if (*p++ != ' ')
			return FALSE;
	}

	return TRUE;
}

}

