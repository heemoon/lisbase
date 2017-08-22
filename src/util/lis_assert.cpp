// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <assert.h>
#include <stdio.h>
#include "lis_assert.h"
#include "lis_system.h"


namespace lisbase {

void __show__(const char* szTemp) {
    System::dumpStack(szTemp);
#ifndef NDEBUG
    assert(0);
#endif
}

void __assert__(const char* file, uint32_t line, const char* func, const char* expr) {
    char szTemp[1024];
    snprintf(szTemp, sizeof(szTemp), "[%s][%u][%s][%s]\n", file, line, func, expr);
    szTemp[1023] = '\0';
    __show__(szTemp);
}

void __assertspecial__(const char* file, uint32_t line, const char* func, const char* expr, const char* msg) {
    char szTemp[1024];
    snprintf(szTemp, sizeof(szTemp), "[%s][%u][%s][%s]\n[%s]\n", file, line, func, expr, msg);
    szTemp[1023] = '\0';
    __show__(szTemp);
}

void __assert__format__(const char* file, uint32_t line, const char* func, const char* expr, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char formatStringBuffer[1024];
    vsprintf(formatStringBuffer, format, args);
    formatStringBuffer[1023] = '\0';
    va_end(args);

    char szTemp[2048];
    snprintf(szTemp, sizeof(szTemp), "[%s][%u][%s][%s][%s]", file, line, func, expr, formatStringBuffer);
    szTemp[2047] = '\0';
    __show__(szTemp);
}

void __show_throw__(const char* szTemp) {
    System::dumpStack(szTemp);
    throw(1);
}

void __assert_throw__(const char* file, uint32_t line, const char* func, const char* expr) {
    char szTemp[1024];
    snprintf(szTemp, sizeof(szTemp), "[%s][%u][%s][%s]\n", file, line, func, expr);
    szTemp[1023] = '\0';
    __show_throw__(szTemp);
}

void __assertspecial_throw__(const char* file, uint32_t line, const char* func, const char* expr, const char* msg) {
    char szTemp[1024];
    snprintf(szTemp, sizeof(szTemp), "[%s][%u][%s][%s]\n[%s]\n", file, line, func, expr, msg);
    szTemp[1023] = '\0';
    __show_throw__(szTemp);
}

}
