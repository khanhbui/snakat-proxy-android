//
//  Util-android.cpp
//  snakat
//
//  Created by Khanh Hoang Bui on 12/30/15.
//  Copyright © 2015 SNAKAT. All rights reserved.
//

#include "Util.h"
#include "../core/Macros.h"
#include "../util/JniUtil.h"

#include <cstdio>

namespace snakat {

    extern void __LogD(const char *pszFormat, ...) {
            char buf[MAX_LEN];

            va_list args;
            va_start(args, pszFormat);
            vsnprintf(buf, MAX_LEN, pszFormat, args);
            va_end(args);

            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", buf);
    }

    extern void __LogI(const char *pszFormat, ...) {
        char buf[MAX_LEN];

        va_list args;
        va_start(args, pszFormat);
        vsnprintf(buf, MAX_LEN, pszFormat, args);
        va_end(args);

        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", buf);
    }

    extern void __LogW(const char *pszFormat, ...) {
        char buf[MAX_LEN];

        va_list args;
        va_start(args, pszFormat);
        vsnprintf(buf, MAX_LEN, pszFormat, args);
        va_end(args);

        __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s", buf);
    }

    extern void __LogE(const char *pszFormat, ...) {
        char buf[MAX_LEN];

        va_list args;
        va_start(args, pszFormat);
        vsnprintf(buf, MAX_LEN, pszFormat, args);
        va_end(args);

        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", buf);
    }
}
