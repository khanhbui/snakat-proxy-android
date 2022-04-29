#ifndef SNAKAT_PROXY_MACROS_H
#define SNAKAT_PROXY_MACROS_H

#if PLATFORM_ANDROID
#include <android/log.h>
#include <jni.h>
#endif //PLATFORM_ANDROID

namespace snakat {
    extern void __LogD(const char *pszFormat, ...);
    extern void __LogI(const char *pszFormat, ...);
    extern void __LogW(const char *pszFormat, ...);
    extern void __LogE(const char *pszFormat, ...);
}

#if PLATFORM_ANDROID
#define ASSERT(cond, msg)                                           \
    if (!(cond)) {                                                  \
        __android_log_print(ANDROID_LOG_ERROR,                      \
                            LOG_TAG,                                \
                            "file:%s function:%s line:%d, %s",      \
                            __FILE__, __FUNCTION__, __LINE__, msg); \
    }
#endif //PLATFORM_ANDROID

#if DEBUG == 0
#define LOGD(...)  do {} while (0)
#define LOGI(...)  do {} while (0)
#define LOGW(...)  do {} while (0)
#define LOGE(...)  do {} while (0)
#else
#define LOGD(format, ...) snakat::__LogD(format, ##__VA_ARGS__)
#define LOGI(format,...)  snakat::__LogI(format, ##__VA_ARGS__)
#define LOGW(format,...)  snakat::__LogW(format, ##__VA_ARGS__)
#define LOGE(format,...)  snakat::__LogE(format, ##__VA_ARGS__)
#endif //DEBUG

#define LOG_TAG "SNAKAT-Proxy"
#define MAX_LEN (16*1024 + 1)

#define MG_BUF_LEN (8192)

#define SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define SAFE_DELETE_ARRAY(p)      do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define SAFE_FREE(p)              do { if(p) { free(p); (p) = 0; } } while(0)
#define SAFE_RELEASE(p)           do { if(p) { (p)->release(); } } while(0)
#define SAFE_RELEASE_NULL(p)      do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)

#endif //SNAKAT_PROXY_MACROS_H
