//
// Created by 58 on 2016/5/4.
//

#include "CRegex.h"
#include "Global.h"
#include "Tools.h"
#include "Log.h"

namespace lisbase {
CRegex::CRegex() {
	m_uNum = 0;
}

CRegex::~CRegex() {
	destroy();
}

BOOL CRegex::init(const char* pattern, uint32_t nm) {
	if (!pattern || '\0' == pattern[0])
		return FALSE;

	destroy();  //if used then clear end

	m_uNum = nm;
	if (m_uNum > 10) {
		return FALSE;
	}

	int err = regcomp(&m_Reg, pattern, REG_EXTENDED);
	if (err) {
		char errbuf[64];
		size_t len = regerror(err, &m_Reg, errbuf, sizeof(errbuf));
		if (0 < len) {
			Logger.error("Regex::init() regcomp error:%s", errbuf);
		}
		return FALSE;
	}

	return TRUE;
}

void CRegex::destroy() {
	if (m_uNum > 0) {
		regfree(&m_Reg);
		m_uNum = 0;
	}
}

BOOL CRegex::exec(char* pDest) {
	if (!pDest || '\0' == pDest[0]) {
		return FALSE;
	}

	int err = regexec(&m_Reg, pDest, m_uNum, m_aMatch, REG_NOTBOL);
	if (err) {
		char errbuf[64];
		size_t len = regerror(err, &m_Reg, errbuf, sizeof(errbuf));
		if (0 < len) {
			Logger.error("Regex::exec() regexec error:%s", errbuf);
		}
		return FALSE;
	}

	return TRUE;
}

int CRegex::getFrom(uint32_t idx) {

	if (idx >= m_uNum)
		return INVALID_VALUE;

	return (int) m_aMatch[idx].rm_so;
}

int CRegex::getLen(uint32_t idx) {

	if (idx >= m_uNum)
		return INVALID_VALUE;

	return (int) m_aMatch[idx].rm_eo - m_aMatch[idx].rm_so;
}

int64_t CRegex::getIntByIdx(uint32_t idx, const char* pDest) {
	char szBuf[20] = { 0 };
	Tools::Strncpy(szBuf, pDest + m_aMatch[idx].rm_so,
			m_aMatch[idx].rm_eo - m_aMatch[idx].rm_so);
	return (int64_t) atol(szBuf);
}

BOOL CRegex::writeStringByIdx(uint32_t idx, const char* pDest, char* pWrite) {
	if (Tools::Strncpy(pWrite, pDest + m_aMatch[idx].rm_so,
			m_aMatch[idx].rm_eo - m_aMatch[idx].rm_so))
		return TRUE;

	return FALSE;
}

}
