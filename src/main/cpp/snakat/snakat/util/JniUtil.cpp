#include "JniUtil.h"

#include <android/asset_manager_jni.h>

namespace snakat {

    JniUtil::JniUtil()
            : _javaVM(nullptr),
              _localRefs() {
    }

    JniUtil::~JniUtil() {
        _javaVM = nullptr;
    }

    bool JniUtil::init(JavaVM *javaVM) {
        _javaVM = javaVM;

        return true;
    }

    JNIEnv *JniUtil::getEnv() const {
        JNIEnv *env = nullptr;

        if (_javaVM->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            LOGE("Failed to get the environment using GetEnv()");
            return nullptr;
        }

        if (_javaVM->AttachCurrentThread(&env, nullptr) < 0) {
            LOGE("Failed to get the environment using AttachCurrentThread()");
            return nullptr;
        }

        return env;
    }

    bool JniUtil::getStaticMethodInfo(const std::string &className, const std::string &methodName, const std::string &sig,
                                 MethodInfo &methodInfo) const {
        if (className.empty() || methodName.empty() || sig.empty()) {
            return false;
        }

        JNIEnv *pEnv = getEnv();
        if (!pEnv) {
            return false;
        }

        jclass clazz = _getClass(className, pEnv);

        jmethodID methodID = pEnv->GetStaticMethodID(clazz, methodName.c_str(), sig.c_str());
        if (!methodID) {
            LOGE("Failed to find static method id of %s", methodName.c_str());
            return false;
        }

        methodInfo.env = pEnv;
        methodInfo.clazz = clazz;
        methodInfo.methodID = methodID;

        return true;
    }

    bool
    JniUtil::getMethodInfo(const std::string &className, const std::string &methodName, const std::string &sig,
                           MethodInfo &methodInfo) const {
        if (className.empty() || methodName.empty() || sig.empty()) {
            return false;
        }

        JNIEnv *pEnv = getEnv();
        if (!pEnv) {
            return false;
        }

        jclass clazz = _getClass(className, pEnv);

        jmethodID methodID = pEnv->GetMethodID(clazz, methodName.c_str(), sig.c_str());
        if (!methodID) {
            LOGE("Failed to find method id of %s", methodName.c_str());
            return false;
        }

        methodInfo.env = pEnv;
        methodInfo.clazz = clazz;
        methodInfo.methodID = methodID;

        return true;
    }

    bool JniUtil::getMethodInfo(const jobject &object, const std::string &methodName, const std::string &sig,
                                MethodInfo &methodInfo) const {
        if (methodName.empty() || sig.empty()) {
            return false;
        }

        JNIEnv *pEnv = getEnv();
        if (!pEnv) {
            return false;
        }

        jclass clazz = pEnv->GetObjectClass(object);

        jmethodID methodID = pEnv->GetMethodID(clazz, methodName.c_str(), sig.c_str());
        if (!methodID) {
            LOGE("Failed to find method id of %s", methodName.c_str());
            return false;
        }

        methodInfo.env = pEnv;
        methodInfo.clazz = clazz;
        methodInfo.methodID = methodID;

        return true;
    }

    std::string JniUtil::jstring2string(jstring jstr) const {
        std::string ret;
        if (jstr != nullptr) {
            JNIEnv *env = getEnv();
            if (env) {
                const char *chars = env->GetStringUTFChars(jstr, nullptr);
                ret.assign(chars);
                env->ReleaseStringUTFChars(jstr, chars);
            }
        }
        return ret;
    }

    jclass JniUtil::_getClass(const std::string &className, JNIEnv *env) {
        JNIEnv *pEnv = env;

        jclass ret = pEnv->FindClass(className.c_str());
        if (!ret) {
            LOGE("Failed to find class of %s", className.c_str());
            return nullptr;
        }
        return ret;
    }

    jstring JniUtil::_convert(const MethodInfo &info, const char *x) {
        jstring ret = info.env->NewStringUTF(x ? x : "");
        _localRefs[info.env].push_back(ret);
        return ret;
    }

    jstring JniUtil::_convert(const MethodInfo &info, const std::string &x) {
        return _convert(info, x.c_str());
    }

    jvalue JniUtil::_convert(const MethodInfo &info, const long &x) {
        jvalue ret;
        ret.j = x;
        return ret;
    }

    void JniUtil::_deleteLocalRefs(JNIEnv *env) {
        if (!env) {
            return;
        }

        for (const auto &ref : _localRefs[env]) {
            env->DeleteLocalRef(ref);
        }
        _localRefs[env].clear();
    }

    void JniUtil::_reportError(const std::string &className, const std::string &methodName,
                               const std::string &signature) {
        LOGE("Failed to find static java method. Class name: %s, method name: %s, signature: %s ",
             className.c_str(), methodName.c_str(), signature.c_str());
    }
}
