/*
 * IniReader.cpp
 *
 *  Created on: 2016年7月15日
 *      Author: 58
 */

#include <wedis/base/util/IniReader.h>
#include <wedis/base/util/Tools.h>
#include <wedis/base/util/Alloctor.h>
#include <stdio.h>
#include <string.h>

namespace wRedis {

IniReader::IniReader() {
	memset(m_FileName, 0, sizeof(m_FileName));
	m_DataLen = 0;
	m_pData = NULLPTR;
	m_aSectIndex = NULLPTR;
	m_SectCount = 0;
}

IniReader::IniReader(const I8 *pFileName) {
	memset(m_FileName, 0, sizeof(m_FileName));
	Tools::Strncpy(m_FileName, pFileName, sizeof(m_FileName) - 1);

	m_DataLen = 0;
	m_pData = NULLPTR;
	m_aSectIndex = NULLPTR;
	m_SectCount = 0;

	openFile(pFileName);
}

IniReader::~IniReader() {
	W_DELETE_ARRAY(m_pData);
	W_DELETE_ARRAY(m_aSectIndex);
}

BOOL IniReader::openFile(const I8* pFileName) {
	AssertRetFalse(pFileName != NULLPTR);

	if (m_FileName[0] == '\0') {
		Tools::Strncpy(m_FileName, pFileName, sizeof(m_FileName) - 1);
	}

	FILE* pFile = ::fopen(pFileName, "rb");
	if (pFile == NULLPTR) {
		Assertf(pFile != NULLPTR, "NULLFILE:%s", pFileName);
	}

	// get file length
	::fseek(pFile, 0, SEEK_END);
	m_DataLen = ::ftell(pFile);
	AssertRetFalse(m_DataLen>0);
	::fseek(pFile, 0, SEEK_SET);

	// read in cache
	m_pData = new I8[(UI32)m_DataLen];
	AssertRetFalse(m_pData);
	::fread(m_pData, (UI32)m_DataLen, 1, pFile);

	// close
	::fclose(pFile);

	// init section index
	__initSectIndex();

	return TRUE;
}

BOOL IniReader::saveFile() {
	AssertRetFalse(m_FileName[0] != '\0');

	FILE* pFile = ::fopen(m_FileName, "wb");
	AssertRetFalse(pFile != NULLPTR);

	::fseek(pFile, 0, SEEK_SET);
	::fwrite(m_pData, (UI32)m_DataLen, 1, pFile);
	::fclose(pFile);

	return TRUE;
}

BOOL IniReader::readString(const I8* pSect, const I8* pKey, I8* pOut, I32 Len) const {
	AssertRetFalse(pSect != NULLPTR);
	AssertRetFalse(pKey != NULLPTR);

	// get setction index
	I32 Pos = __findSect(pSect);
	if (Pos == -1) {
		return FALSE;
	}

	// jump to key value line index
	Pos = __findNextLineBegin(Pos);
	if (Pos == -1) {
		return FALSE;
	}

	// find key
	Pos = __findKey(Pos, pKey);
	if (Pos == -1) {
		return FALSE;
	}

	return __getValue(Pos, pOut, Len);
}

I32 IniReader::readInt(const I8* pSect, const I8* pKey) const {
	I8 Buff[INI_VALUE_LEN] = { 0 };
	memset(Buff, 0, sizeof(Buff));

	if (!readString(pSect, pKey, Buff, INI_VALUE_LEN)) {
		return -1;
	}

	if (!Tools::IsInteger(Buff)) {
		return -1;
	}

	return Tools::Atoi(Buff);
}

REAL32 IniReader::readFloat(const I8* pSect, const I8* pKey) const {
	I8 Buff[INI_VALUE_LEN] = { 0 };
	memset(Buff, 0, sizeof(Buff));

	if (!readString(pSect, pKey, Buff, INI_VALUE_LEN)) {
		return -1;
	}

	if (!Tools::IsFloat(Buff)) {
		return -1;
	}

	return (REAL32)Tools::Atof(Buff);
}

BOOL IniReader::writeInt(const I8* pSect, const I8* pKey, I32 iValue) {
	I8 Buff[INI_KEY_LEN + INI_VALUE_LEN];
	memset(Buff, 0, sizeof(Buff));
	Tools::Snprintf(Buff, sizeof(Buff), "%d", iValue);
	return writeString(pSect, pKey, Buff);
}

BOOL IniReader::writeFloat(const I8* pSect, const I8* pKey, REAL32 fValue) {
	I8 Buff[INI_KEY_LEN + INI_VALUE_LEN];
	memset(Buff, 0, sizeof(Buff));
	Tools::Snprintf(Buff, sizeof(Buff), "%f", fValue);
	return writeString(pSect, pKey, Buff);
}

BOOL IniReader::writeString(const I8* pSect, const I8* pKey, const I8* pValue) {
	AssertRetFalse(pSect != NULLPTR);
	AssertRetFalse(pKey != NULLPTR);
	AssertRetFalse(pValue != NULLPTR);

	I32 iFind = __findSect(pSect);
	if (iFind == -1) {
		addSect(pSect);
		iFind = __findSect(pSect);
		AssertRetFalse(iFind != -1);
	}

	iFind = __findNextLineBegin(iFind);
	if (iFind == -1) {
		return addKeyValuePair(pSect, pKey, pValue);
	}

	iFind = __findKey(iFind, pKey);
	if (iFind == -1) {
		return addKeyValuePair(pSect, pKey, pValue);
	}

	return modifyKeyValuePair(pSect, pKey, pValue);
}

BOOL IniReader::addKeyValuePair(const I8* pSect, const I8* pKey, const I8* pValue) {
	I32 iFind1 = __findSect(pSect);
	if (iFind1 == -1) {
		return FALSE;
	}

	I32 iFind2 = __findKey(iFind1, pKey);
	if (iFind2 != -1) {
		return FALSE;
	}

	I32 InsertPos = -1;
	I32 Index = __getIndexByPosition(iFind1);

	if (Index < m_SectCount - 1) {
		InsertPos = m_aSectIndex[Index + 1] - 1;
	}
	else {
		InsertPos = m_DataLen;
	}

	I8 KeyValuePair[INI_KEY_LEN + INI_VALUE_LEN];
	memset(KeyValuePair, 0, sizeof(KeyValuePair));

	I32 KeyLen = (I32)Tools::Strlen(pKey);
	I32 ValueLen = (I32)Tools::Strlen(pValue);

	//添加换行
	KeyValuePair[0] = '\r';
	KeyValuePair[1] = '\n';

	Tools::Strncpy(&KeyValuePair[2], pKey, KeyLen);
	KeyValuePair[KeyLen + 2] = '=';
	Tools::Strncpy(&KeyValuePair[KeyLen + 3], pValue, ValueLen);

	I32 IncSize = 2 + KeyLen + 1 + ValueLen;

	I8* pNewData = NULLPTR;
	pNewData = new I8[(UI32)(m_DataLen + IncSize)];
	AssertRetFalse(pNewData != NULLPTR);

	if (InsertPos > 0) {
		memcpy(pNewData, m_pData, (UI32)InsertPos);
	}

	memcpy(&pNewData[InsertPos], KeyValuePair, (UI32)IncSize);

	if (m_DataLen - InsertPos > 0) {
		memcpy(&pNewData[InsertPos + IncSize], &pNewData[InsertPos], (UI32)(m_DataLen - InsertPos));
	}

	W_DELETE_ARRAY(m_pData);
	m_pData = pNewData;
	m_DataLen += IncSize;

	__initSectIndex();

	return  TRUE;
}

BOOL IniReader::modifyKeyValuePair(const I8* pSect, const I8* pKey, const I8* pValue) {
	I32 iFind = __findSect(pSect);
	AssertRetFalse(iFind != -1);

	iFind = __findNextLineBegin(iFind);
	AssertRetFalse(iFind != -1);

	iFind = __findKey(iFind, pKey);
	AssertRetFalse(iFind != -1);

	I32 OldSize = -1;
	I32 EndPos = -1;
	for (I32 i = iFind; i < m_DataLen; ++i) {
		if (m_pData[i] == '\n' || m_pData[i] == '\r') {
			OldSize = i - iFind;
			EndPos = i;
			break;
		}

		if (i == m_DataLen - 1) {
			OldSize = m_DataLen - iFind;
			EndPos = m_DataLen;
		}
	}

	I8 KeyValuePair[INI_KEY_LEN + INI_VALUE_LEN];
	memset(KeyValuePair, 0, sizeof(KeyValuePair));

	I32 KeyLen = (I32)Tools::Strlen(pKey);
	I32 ValueLen = (I32)Tools::Strlen(pValue);

	Tools::Strncpy(&KeyValuePair[0], pKey, KeyLen);
	KeyValuePair[KeyLen] = '=';
	Tools::Strncpy(&KeyValuePair[KeyLen + 1], pValue, ValueLen);

	I32 NewSize = KeyLen + 1 + ValueLen;
	I32 IncSize = NewSize - OldSize;

	I8* pNewData = NULLPTR;
	pNewData = new I8[(UI32)(m_DataLen + IncSize)];
	AssertRetFalse(pNewData != NULLPTR);

	if (iFind > 0) {
		memcpy(pNewData, m_pData, (UI32)iFind);
	}

	memcpy(&pNewData[iFind], KeyValuePair, (UI32)NewSize);

	if (m_DataLen - EndPos > 0) {
		memcpy(&pNewData[iFind + NewSize], &m_pData[EndPos], (UI32)(m_DataLen - EndPos));
	}

	W_DELETE_ARRAY(m_pData);
	m_pData = pNewData;
	m_DataLen += IncSize;

	__initSectIndex();

	return TRUE;
}

I32 IniReader::__getIndexByPosition(I32 Pos) const {
	for (I32 i = 0; i < m_SectCount; ++i) {
		if (m_aSectIndex[i] == Pos) {
			return i;
		}
	}

	return -1;
}

I32 IniReader::__findSect(const I8* pSect) const {
	AssertRet(pSect != NULLPTR, -1);

	I8 Buf[INI_VALUE_LEN] = { 0 };
	memset(Buf, 0, sizeof(Buf));

	for (I32 i = 0; i < m_SectCount; ++i) {
		I32 os = m_aSectIndex[i];
		while (m_pData[os] != ']' && os < m_DataLen) {
			++os;
		}

		memset(Buf, 0, sizeof(Buf));
		Tools::Strncpy(Buf, &m_pData[m_aSectIndex[i]], os - m_aSectIndex[i]);

		Tools::EraseStrBlank(Buf);

		if (Tools::Strcmp(Buf, pSect) == 0) {
			return m_aSectIndex[i];
		}
	}

	return -1;
}

I32 IniReader::__getLine(I8* pBuff, I32 BuffLen, I32 BeginPos) const {
	AssertRet(pBuff != 0, -1);
	AssertRet(BuffLen > 0, -1);
	AssertRet(BeginPos >= 0 && BeginPos < m_DataLen, -1);

	I32 Idx = 0;
	while (BeginPos < m_DataLen && Idx < BuffLen - 1) {
		if (m_pData[BeginPos] == '\n' || m_pData[BeginPos] == '\r')
			break;

		pBuff[Idx++] = m_pData[BeginPos++];
	}

	pBuff[Idx] = '\0';

	//跳过换行
	while (BeginPos < m_DataLen) {
		if (m_pData[BeginPos] == '\r' || m_pData[BeginPos] == '\n') {
			++BeginPos;
		}
		else {
			return BeginPos;
		}
	}

	return m_DataLen;
}

I32 IniReader::__findKey(I32 Begin, const I8* pKey) const {
	AssertRet(pKey != NULLPTR, -1);

	I8 LineBuff[INI_LINE_LEN] = { 0 };
	memset(LineBuff, 0, sizeof(LineBuff));

	I8 KeyBuff[INI_KEY_LEN] = { 0 };
	memset(KeyBuff, 0, sizeof(KeyBuff));

	while (Begin < m_DataLen) {
		memset(LineBuff, 0, sizeof(LineBuff));
		I32 NextLineBegin = __getLine(LineBuff, INI_LINE_LEN, Begin);

		I32 LineLen = (I32)Tools::Strlen(LineBuff);
		if (LineLen == 0) {
			Begin = NextLineBegin;
			continue;
		}

		if (LineBuff[0] == '#') {
			Begin = NextLineBegin;
			continue;
		}

		memset(KeyBuff, 0, sizeof(KeyBuff));
		for (I32 i = 0; i < LineLen; ++i) {
			if (LineBuff[i] == '[') {
				if (i == 0 || LineBuff[i - 1] == '\n') {
					return -1;
				}
			}

			if (LineBuff[i] != '=') {
				KeyBuff[i] = LineBuff[i];
			}
			else {
				Tools::EraseStrBlank(KeyBuff);

				if (Tools::Strcmp(KeyBuff, pKey) == 0) {
					return Begin;
				}
				else
					break;
			}
		}//end of for i

		Begin = NextLineBegin;
	}

	return -1;
}

I32 IniReader::__findNextLineBegin(I32 CurPos) const {
	for (I32 i = CurPos; i < m_DataLen; ++i) {
		if (m_pData[i] == '\n')
			return ++i;
	}

	return -1;
}

void IniReader::addSect(const I8* pSect) {
	if (Tools::Strlen(pSect) > INI_SECT_LEN - 8) return;
	if (__findSect(pSect) != -1) return;

	I8 SectBuff[INI_SECT_LEN] = { 0 };
	memset(SectBuff, 0, sizeof(SectBuff));

	SectBuff[0] = '[';
	Tools::Strncpy(&SectBuff[1], pSect, INI_SECT_LEN - 2);
	SectBuff[Tools::Strlen(pSect) + 1] = ']';

	I32 IncLen = (I32)Tools::Strlen(SectBuff);
	I8* pNewData = NULLPTR;

	if (m_pData[m_DataLen - 1] != '\n') {
		pNewData = new I8[(UI32)(m_DataLen + IncLen + 2)];
		AssertRetEmpty(pNewData != NULLPTR);

		memcpy(pNewData, m_pData, (UI32)m_DataLen);

		pNewData[m_DataLen] = '\r';
		pNewData[m_DataLen + 1] = '\n';

		memcpy(&pNewData[m_DataLen + 2], SectBuff, (UI32)IncLen);

		m_DataLen += IncLen + 2;
	}
	else {
		pNewData = new I8[(UI32)(m_DataLen + IncLen + 2)];
		AssertRetEmpty(pNewData != NULLPTR);

		memcpy(pNewData, m_pData, (UI32)m_DataLen);
		memcpy(&pNewData[m_DataLen], SectBuff, (UI32)IncLen);

		m_DataLen += IncLen;
	}

	W_DELETE_ARRAY(m_pData);
	m_pData = pNewData;

	__initSectIndex();
}

BOOL IniReader::__getValue(I32 Begin, I8* pValue, I32 Len) const {
	AssertRetFalse(Begin > 0 && Begin < m_DataLen);
	AssertRetFalse(pValue != NULLPTR);

	BOOL bFlag = FALSE;
	memset(pValue, 0, sizeof(I8)*Len);
	I32 os = 0;

	for (I32 i = Begin; i < m_DataLen; ++i) {
		if (bFlag == FALSE && m_pData[i] == '=') {
			bFlag = TRUE;
			continue;
		}

		if (m_pData[i] == '\n' || m_pData[i] == '\r') {
			break;
		}

		if (bFlag) {
			pValue[os] = m_pData[i];
			++os;
		}
	}

	if (__decrypt(pValue, Len) == FALSE) {
		return FALSE;
	}

	Tools::EraseStrBlank(pValue);
	return Tools::Strlen(pValue) > 0 ? TRUE : FALSE;
}


BOOL IniReader::__decrypt(I8* pValue, I32 Len) const {
	I32 StringLen = Tools::Strlen(pValue);
	if (StringLen <= 0) return TRUE;
	if (pValue[0] != '*') return TRUE;

	// TODO: IniReader 解密 __decrypt
	return TRUE;
}


void IniReader::__initSectIndex() {
	AssertRetEmpty(m_DataLen > 0);
	I32 i = 0;

	m_SectCount = 0;
	for (i = 0; i < m_DataLen; ++i) {
		if (m_pData[i] == '[') {
			if (i == 0 || m_pData[i - 1] == '\n') {
				++m_SectCount;
			}
		}
	}

	if (m_SectCount > 0) {
		W_DELETE_ARRAY(m_aSectIndex);
		m_aSectIndex = new I32[(UI32)m_SectCount];

		I32 os = 0;

		for (I32 i = 0; i < m_DataLen; ++i) {
			if (m_pData[i] == '[') {
				if (i == 0 || m_pData[i - 1] == '\n') {
					m_aSectIndex[os] = i + 1;
					++os;
				}
			}
		}
	}
}

} /* namespace wRedis */
