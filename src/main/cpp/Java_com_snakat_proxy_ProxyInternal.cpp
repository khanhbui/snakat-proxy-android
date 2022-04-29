//
// Created by Khanh Hoang Bui on 25/4/22.
//

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "snakat/snakat/Proxy.h"

using namespace snakat;

static jobject android_java_asset_manager = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_snakat_proxy_ProxyInternal_initJNI(JNIEnv *env, jobject thiz, jobject asset_manager,
                                            jstring document_dir, jstring host_name,
                                            jboolean ssl_enabled, jstring ssl_cert_filename,
                                            jstring manifest_filename) {
    android_java_asset_manager = env->NewGlobalRef(asset_manager);

    AAssetManager *assetManager = AAssetManager_fromJava(env, android_java_asset_manager);
    const char *documentDir = env->GetStringUTFChars(document_dir, 0);
    const char *hostname = env->GetStringUTFChars(host_name, 0);
    bool sslEnabled = ssl_enabled == JNI_TRUE;
    const char *sslCertFilename = env->GetStringUTFChars(ssl_cert_filename, 0);
    const char *manifestFilename = env->GetStringUTFChars(manifest_filename, 0);

    Proxy::getInstance().init(assetManager, documentDir, hostname, sslEnabled, sslCertFilename,
                              manifestFilename);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snakat_proxy_ProxyInternal_startJNI(JNIEnv *env, jobject thiz) {
    Proxy::getInstance().start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snakat_proxy_ProxyInternal_pauseJNI(JNIEnv *env, jobject thiz) {
    Proxy::getInstance().pause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snakat_proxy_ProxyInternal_resumeJNI(JNIEnv *env, jobject thiz) {
    Proxy::getInstance().resume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snakat_proxy_ProxyInternal_stopJNI(JNIEnv *env, jobject thiz) {
    Proxy::getInstance().stop();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snakat_proxy_ProxyInternal_disposeJNI(JNIEnv *env, jobject thiz) {
    if (android_java_asset_manager) {
        env->DeleteGlobalRef(android_java_asset_manager);
        android_java_asset_manager = nullptr;
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_snakat_proxy_ProxyInternal_isStartedJNI(JNIEnv *env, jobject thiz) {
    return Proxy::getInstance().isStarted();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_snakat_proxy_ProxyInternal_isRunningJNI(JNIEnv *env, jobject thiz) {
    return Proxy::getInstance().isRunning();
}