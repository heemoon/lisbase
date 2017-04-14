//
// Created by 58 on 2016/4/18.
//

#include <stdio.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/Log.h>


namespace wRedis {

void log_debug(const I8* str) {
    if(NULLPTR == str) {
        return;
    }

    printf("[debug]%s\n", str);
}

void log_info(const I8* str) {
    if(NULLPTR == str) {
        return;
    }

    printf("[info]%s\n", str);
}

void log_warn(const I8* str) {
    if(NULLPTR == str) {
        return;
    }

    printf("[warn]%s\n", str);
}

void log_error(const I8* str) {
    if(NULLPTR == str) {
        return;
    }

    printf("[error]%s\n", str);
}
}



