// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_UTIL_STRINGSPLIT_H__
#define __LISBASE_UTIL_STRINGSPLIT_H__

namespace lisbase {

class StringSplit {
public:
	StringSplit();
	~StringSplit();
public:
	int doSplit(const char* pSource, const char* pSep);
	int doSplit(const char* pSource, int nLen, const char* pSep, int nSeplen);

	inline int elements() { return m_nElements; };
	char* getLine(int nLine);
private:
	void __destroy();
private:
	char** m_pTokens;
	int  m_nElements;
};

}

#endif
