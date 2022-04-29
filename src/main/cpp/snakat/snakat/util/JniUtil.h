#ifndef SNAKAT_PROXY_JNIUTIL_H
#define SNAKAT_PROXY_JNIUTIL_H

#include "JniUtil.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <android/asset_manager.h>

#include "../core/Macros.h"

namespace snakat {
    struct JniMethodInfo {
        JNIEnv *env;
        jclass classID;
        jmethodID methodID;
    };

    class JniUtil {
    public:
        static void setJavaVM(JavaVM *javaVM);

        static JavaVM *getJavaVM();

        static JNIEnv *getEnv();

        static bool getStaticMethodInfo(JniMethodInfo &methodinfo,
                                        const char *className,
                                        const char *methodName,
                                        const char *paramCode);

        static bool getMethodInfo(JniMethodInfo &methodinfo,
                                  const char *className,
                                  const char *methodName,
                                  const char *paramCode);

        static bool getMethodInfo(JniMethodInfo &methodinfo,
                                  const jobject &object,
                                  const char *methodName,
                                  const char *paramCode);

        static std::string jstring2string(jstring str);

        template<typename... Ts>
        static void callVoidMethod(const jobject &object,
                                   const std::string &methodName,
                                   Ts... xs) {
            JniMethodInfo t;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")V";
            if (JniUtil::getMethodInfo(t, object, methodName.c_str(), signature.c_str())) {
                t.env->CallVoidMethod(object, t.methodID, _convert(t, xs)...);
                t.env->DeleteLocalRef(t.classID);
                _deleteLocalRefs(t.env);
            } else {
                _reportError("object", methodName, signature);
            }
        }

        template<typename... Ts>
        static void callStaticVoidMethod(const std::string &className,
                                         const std::string &methodName,
                                         Ts... xs) {
            JniMethodInfo t;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")V";
            if (JniUtil::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
                                             signature.c_str())) {
                t.env->CallStaticVoidMethod(t.classID, t.methodID, _convert(t, xs)...);
                t.env->DeleteLocalRef(t.classID);
                _deleteLocalRefs(t.env);
            } else {
                _reportError(className, methodName, signature);
            }
        }

        template<typename... Ts>
        static bool callStaticBooleanMethod(const std::string &className,
                                            const std::string &methodName,
                                            Ts... xs) {
            jboolean jret = JNI_FALSE;
            JniMethodInfo t;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")Z";
            if (JniUtil::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
                                             signature.c_str())) {
                jret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, _convert(t, xs)...);
                t.env->DeleteLocalRef(t.classID);
                _deleteLocalRefs(t.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return (jret == JNI_TRUE);
        }

        template<typename... Ts>
        static int callStaticIntMethod(const std::string &className,
                                       const std::string &methodName,
                                       Ts... xs) {
            jint ret = 0;
            JniMethodInfo t;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")I";
            if (JniUtil::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
                                             signature.c_str())) {
                ret = t.env->CallStaticIntMethod(t.classID, t.methodID, _convert(t, xs)...);
                t.env->DeleteLocalRef(t.classID);
                _deleteLocalRefs(t.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

        template<typename... Ts>
        static float callStaticFloatMethod(const std::string &className,
                                           const std::string &methodName,
                                           Ts... xs) {
            jfloat ret = 0.0;
            JniMethodInfo t;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")F";
            if (JniUtil::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
                                             signature.c_str())) {
                ret = t.env->CallStaticFloatMethod(t.classID, t.methodID, _convert(t, xs)...);
                t.env->DeleteLocalRef(t.classID);
                _deleteLocalRefs(t.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

        template<typename... Ts>
        static double callStaticDoubleMethod(const std::string &className,
                                             const std::string &methodName,
                                             Ts... xs) {
            jdouble ret = 0.0;
            JniMethodInfo t;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")D";
            if (JniUtil::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
                                             signature.c_str())) {
                ret = t.env->CallStaticDoubleMethod(t.classID, t.methodID, _convert(t, xs)...);
                t.env->DeleteLocalRef(t.classID);
                _deleteLocalRefs(t.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

        template<typename... Ts>
        static std::string callStaticStringMethod(const std::string &className,
                                                  const std::string &methodName,
                                                  Ts... xs) {
            std::string ret;
            JniMethodInfo t;
            std::string signature =
                    "(" + std::string(_getJNISignature(xs...)) + ")Ljava/lang/String;";
            if (JniUtil::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
                                             signature.c_str())) {
                jstring jret = (jstring) t.env->CallStaticObjectMethod(t.classID, t.methodID,
                                                                       _convert(t, xs)...);
                ret = JniUtil::jstring2string(jret);
                t.env->DeleteLocalRef(t.classID);
                t.env->DeleteLocalRef(jret);
                _deleteLocalRefs(t.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

    private:
        static JavaVM *_javaVM;

        static AAssetManager *_aAssetManager;

        static std::unordered_map<JNIEnv *, std::vector<jobject>> _localRefs;

        static jclass _getClassID(const char *className, JNIEnv *env);

        static jstring _convert(JniMethodInfo &t, const char *x);

        static jstring _convert(JniMethodInfo &t, const std::string &x);

        static jvalue _convert(JniMethodInfo &t, const long &x);

        template<typename T>
        static T _convert(JniMethodInfo &, T x) {
            return x;
        }

        static void _deleteLocalRefs(JNIEnv *env);

        static std::string _getJNISignature() {
            return "";
        }

        static std::string _getJNISignature(bool) {
            return "Z";
        }

        static std::string _getJNISignature(char) {
            return "C";
        }

        static std::string _getJNISignature(short) {
            return "S";
        }

        static std::string _getJNISignature(int) {
            return "I";
        }

        static std::string _getJNISignature(long) {
            return "J";
        }

        static std::string _getJNISignature(float) {
            return "F";
        }

        static std::string _getJNISignature(double) {
            return "D";
        }

        static std::string _getJNISignature(const char *) {
            return "Ljava/lang/String;";
        }

        static std::string _getJNISignature(const std::string &) {
            return "Ljava/lang/String;";
        }

        template<typename T>
        static std::string _getJNISignature(T x) {
            ASSERT(sizeof(x) == 0, "Unsupported argument type");
            return "";
        }

        template<typename T, typename... Ts>
        static std::string _getJNISignature(T x, Ts... xs) {
            return _getJNISignature(x) + _getJNISignature(xs...);
        }

        static void _reportError(const std::string &className, const std::string &methodName,
                                 const std::string &signature);
    };
}

#endif //SNAKAT_PROXY_JNIUTIL_H
