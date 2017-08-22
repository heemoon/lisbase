// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <string.h>
#include "lis_string_split.h"
#include "lis_assert.h"
#include "lis_common_macro.h"
#include "lis_alloctor.h"

namespace lisbase {

StringSplit::StringSplit() {
	m_pTokens = NULLPTR;
	m_nElements = 0;
}

StringSplit::~StringSplit() {
	__destroy();
}

void StringSplit::__destroy() {
	for(int i = 0; i < m_nElements; ++i) {
		LIS_FREE(m_pTokens[i]);
	}
	LIS_FREE(m_pTokens);
	m_nElements = 0;
}

int StringSplit::doSplit(const char* pSource, int nLen, const char* pSep, int nSeplen) {
	Assert(pSource);
	Assert(pSep);

	__destroy();

	int element = 0;
	for (int j = 0; j < (nLen-(nSeplen-1)); j++) {
		/* search the separator */
		if ((nSeplen == 1 && *(pSource+j) == pSep[0]) || (memcmp(pSource+j,pSep,nSeplen) == 0)) {
			element++;
		    j = j+nSeplen-1; /* skip the separator */
		}
	}

	m_pTokens = (char**) LIS_CALLOC((element + 1), sizeof(char*));
	Assert(m_pTokens);

	int start = 0;
	for (int j = 0; j < (nLen-(nSeplen-1)); j++) {
		/* search the separator */
		if ((nSeplen == 1 && *(pSource+j) == pSep[0]) || (memcmp(pSource+j,pSep,nSeplen) == 0)) {
			m_pTokens[m_nElements] = (char*)LIS_MALLOC(j-start+1);
		    Assert(m_pTokens[m_nElements]);

		    memcpy(m_pTokens[m_nElements], pSource+start, j-start);
		    m_pTokens[m_nElements][j-start] = '\0';
		    m_nElements++;
		    start = j + nSeplen;
		    j = j + nSeplen - 1; /* skip the separator */
		}
	}

	/* Add the final element. We are sure there is room in the tokens array. */
	m_pTokens[m_nElements] = (char*)LIS_MALLOC(nLen-start+1);
	Assert(m_pTokens[m_nElements]);

	memcpy(m_pTokens[m_nElements], pSource+start, nLen-start);
	m_pTokens[m_nElements][nLen-start] = '\0';
	m_nElements++;

	return m_nElements;
}

int StringSplit::doSplit(const char* pSource, const char* pSep) {
	Assert(pSource);
	Assert(pSep);

	return doSplit(pSource, strlen(pSource), pSep, strlen(pSep));
}

char* StringSplit::getLine(int nLine) {
	Assert(0 <= nLine && m_nElements > nLine);
	Assert(m_pTokens);
	return m_pTokens[nLine];
}


}
