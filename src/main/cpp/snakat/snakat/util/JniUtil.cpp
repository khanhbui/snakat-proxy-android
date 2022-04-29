#include "JniUtil.h"

#include <android/asset_manager_jni.h>

namespace snakat {
    JavaVM *JniUtil::_javaVM = nullptr;
    AAssetManager *JniUtil::_aAssetManager = nullptr;

    std::unordered_map<JNIEnv *, std::vector<jobject>> JniUtil::_localRefs;

    void JniUtil::setJavaVM(JavaVM *javaVM) {
        _javaVM = javaVM;
    }

    JavaVM *JniUtil::getJavaVM() {
        return _javaVM;
    }

    JNIEnv *JniUtil::getEnv() {
        JNIEnv *env = nullptr;

        if (snakat::JniUtil::getJavaVM()->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            LOGE("Failed to get the environment using GetEnv()");
            return nullptr;
        }

        if (snakat::JniUtil::getJavaVM()->AttachCurrentThread(&env, nullptr) < 0) {
            LOGE("Failed to get the environment using AttachCurrentThread()");
            return nullptr;
        }

        return env;
    }

    bool JniUtil::getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className,
                                      const char *methodName, const char *paramCode) {
        if ((nullptr == className) ||
            (nullptr == methodName) ||
            (nullptr == paramCode)) {
            return false;
        }

        JNIEnv *pEnv = getEnv();
        if (!pEnv) {
            return false;
        }

        jclass classID = _getClassID(className, pEnv);

        jmethodID methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);
        if (!methodID) {
            LOGE("Failed to find static method id of %s", methodName);
            return false;
        }

        methodinfo.classID = classID;
        methodinfo.env = pEnv;
        methodinfo.methodID = methodID;

        return true;
    }

    bool
    JniUtil::getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName,
                           const char *paramCode) {
        if ((nullptr == className) ||
            (nullptr == methodName) ||
            (nullptr == paramCode)) {
            return false;
        }

        JNIEnv *pEnv = getEnv();
        if (!pEnv) {
            return false;
        }

        jclass classID = _getClassID(className, pEnv);

        jmethodID methodID = pEnv->GetMethodID(classID, methodName, paramCode);
        if (!methodID) {
            LOGE("Failed to find method id of %s", methodName);
            return false;
        }

        methodinfo.classID = classID;
        methodinfo.env = pEnv;
        methodinfo.methodID = methodID;

        return true;
    }

    bool JniUtil::getMethodInfo(JniMethodInfo &methodinfo,
                                const jobject &object,
                                const char *methodName,
                                const char *paramCode) {
        if ((nullptr == methodName) ||
            (nullptr == paramCode)) {
            return false;
        }

        JNIEnv *pEnv = getEnv();
        if (!pEnv) {
            return false;
        }

        jclass classID = pEnv->GetObjectClass(object);

        jmethodID methodID = pEnv->GetMethodID(classID, methodName, paramCode);
        if (!methodID) {
            LOGE("Failed to find method id of %s", methodName);
            return false;
        }

        methodinfo.classID = classID;
        methodinfo.env = pEnv;
        methodinfo.methodID = methodID;

        return true;
    }

    std::string JniUtil::jstring2string(jstring jstr) {
        if (jstr == nullptr) {
            return "";
        }

        JNIEnv *env = getEnv();
        if (!env) {
            return 0;
        }

        const char *chars = env->GetStringUTFChars(jstr, nullptr);
        std::string ret(chars);
        env->ReleaseStringUTFChars(jstr, chars);

        return ret;
    }

    jclass JniUtil::_getClassID(const char *className, JNIEnv *env) {
        JNIEnv *pEnv = env;

        if (!pEnv) {
            pEnv = getEnv();
            if (!pEnv) {
                return nullptr;
            }
        }

        jclass ret = pEnv->FindClass(className);
        if (!ret) {
            LOGE("Failed to find class of %s", className);
            return nullptr;
        }
        return ret;
    }

    jstring JniUtil::_convert(JniMethodInfo &t, const char *x) {
        jstring ret = t.env->NewStringUTF(x ? x : "");
        _localRefs[t.env].push_back(ret);
        return ret;
    }

    jstring JniUtil::_convert(JniMethodInfo &t, const std::string &x) {
        return _convert(t, x.c_str());
    }

    jvalue JniUtil::_convert(JniMethodInfo &t, const long &x) {
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
