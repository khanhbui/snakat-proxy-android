#ifndef SNAKAT_PROXY_JNIUTIL_H
#define SNAKAT_PROXY_JNIUTIL_H

#include "JniUtil.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <android/asset_manager.h>

#include "../core/Macros.h"
#include "../core/Singleton.h"

namespace snakat {

    struct MethodInfo {
        JNIEnv *env;
        jclass clazz;
        jmethodID methodID;
    };

    class JniUtil : public Singleton<JniUtil> {
    public:
        JniUtil();

        virtual ~JniUtil();

        bool init(JavaVM *javaVM);

        JNIEnv *getEnv() const;

        bool getStaticMethodInfo(const std::string &className, const std::string &methodName,
                                 const std::string &signature,
                                 MethodInfo &methodInfo) const;

        bool getMethodInfo(const std::string &className, const std::string &methodName,
                           const std::string &signature,
                           MethodInfo &methodInfo) const;

        bool getMethodInfo(const jobject &object, const std::string &methodName,
                           const std::string &signature,
                           MethodInfo &methodInfo) const;

        std::string jstring2string(jstring str) const;

        template<typename... Ts>
        inline void callVoidMethod(const jobject &object,
                                   const std::string &methodName,
                                   Ts... xs) {
            MethodInfo info;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")V";
            if (getMethodInfo(object, methodName, signature, info)) {
                info.env->CallVoidMethod(object, info.methodID, _convert(info, xs)...);
                info.env->DeleteLocalRef(info.clazz);
                _deleteLocalRefs(info.env);
            } else {
                _reportError("object", methodName, signature);
            }
        }

        template<typename... Ts>
        inline void callStaticVoidMethod(const std::string &className,
                                         const std::string &methodName,
                                         Ts... xs) {
            MethodInfo info;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")V";
            if (getStaticMethodInfo(className, methodName, signature, info)) {
                info.env->CallStaticVoidMethod(info.clazz, info.methodID, _convert(info, xs)...);
                info.env->DeleteLocalRef(info.clazz);
                _deleteLocalRefs(info.env);
            } else {
                _reportError(className, methodName, signature);
            }
        }

        template<typename... Ts>
        inline bool callStaticBooleanMethod(const std::string &className,
                                            const std::string &methodName,
                                            Ts... xs) {
            jboolean jret = JNI_FALSE;
            MethodInfo info;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")Z";
            if (getStaticMethodInfo(className, methodName, signature, info)) {
                jret = info.env->CallStaticBooleanMethod(info.clazz, info.methodID,
                                                         _convert(info, xs)...);
                info.env->DeleteLocalRef(info.clazz);
                _deleteLocalRefs(info.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return (jret == JNI_TRUE);
        }

        template<typename... Ts>
        inline int callStaticIntMethod(const std::string &className,
                                       const std::string &methodName,
                                       Ts... xs) {
            jint ret = 0;
            MethodInfo info;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")I";
            if (getStaticMethodInfo(className, methodName, signature, info)) {
                ret = info.env->CallStaticIntMethod(info.clazz, info.methodID,
                                                    _convert(info, xs)...);
                info.env->DeleteLocalRef(info.clazz);
                _deleteLocalRefs(info.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

        template<typename... Ts>
        inline float callStaticFloatMethod(const std::string &className,
                                           const std::string &methodName,
                                           Ts... xs) {
            jfloat ret = 0.0;
            MethodInfo info;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")F";
            if (getStaticMethodInfo(className, methodName, signature, info)) {
                ret = info.env->CallStaticFloatMethod(info.clazz, info.methodID,
                                                      _convert(info, xs)...);
                info.env->DeleteLocalRef(info.clazz);
                _deleteLocalRefs(info.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

        template<typename... Ts>
        inline double callStaticDoubleMethod(const std::string &className,
                                             const std::string &methodName,
                                             Ts... xs) {
            jdouble ret = 0.0;
            MethodInfo info;
            std::string signature = "(" + std::string(_getJNISignature(xs...)) + ")D";
            if (getStaticMethodInfo(className, methodName, signature, info)) {
                ret = info.env->CallStaticDoubleMethod(info.clazz, info.methodID,
                                                       _convert(info, xs)...);
                info.env->DeleteLocalRef(info.clazz);
                _deleteLocalRefs(info.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

        template<typename... Ts>
        inline std::string callStaticStringMethod(const std::string &className,
                                                  const std::string &methodName,
                                                  Ts... xs) {
            std::string ret;
            MethodInfo info;
            std::string signature =
                    "(" + std::string(_getJNISignature(xs...)) + ")Ljava/lang/String;";
            if (getStaticMethodInfo(className, methodName, signature, info)) {
                jstring jret = (jstring) info.env->CallStaticObjectMethod(info.clazz, info.methodID,
                                                                          _convert(info, xs)...);
                ret = JniUtil::jstring2string(jret);
                info.env->DeleteLocalRef(info.clazz);
                info.env->DeleteLocalRef(jret);
                _deleteLocalRefs(info.env);
            } else {
                _reportError(className, methodName, signature);
            }
            return ret;
        }

    private:
        JavaVM *_javaVM;

        std::unordered_map<JNIEnv *, std::vector<jobject>> _localRefs;

        static jclass _getClass(const std::string &name, JNIEnv *env);

        jstring _convert(const MethodInfo &info, const char *x);

        jstring _convert(const MethodInfo &info, const std::string &x);

        jvalue _convert(const MethodInfo &info, const long &x);

        template<typename T>
        inline T _convert(const MethodInfo &, T x) {
            return x;
        }

        void _deleteLocalRefs(JNIEnv *env);

        inline std::string _getJNISignature() const {
            return "";
        }

        inline std::string _getJNISignature(bool) const {
            return "Z";
        }

        inline std::string _getJNISignature(char) const {
            return "C";
        }

        inline std::string _getJNISignature(short) const {
            return "S";
        }

        inline std::string _getJNISignature(int) const {
            return "I";
        }

        inline std::string _getJNISignature(long) const {
            return "J";
        }

        inline std::string _getJNISignature(float) const {
            return "F";
        }

        inline std::string _getJNISignature(double) const {
            return "D";
        }

        inline std::string _getJNISignature(const char *) const {
            return "Ljava/lang/String;";
        }

        inline std::string _getJNISignature(const std::string &) const {
            return "Ljava/lang/String;";
        }

        template<typename T>
        inline std::string _getJNISignature(T x) const {
            ASSERT(sizeof(x) == 0, "Unsupported argument type");
            return "";
        }

        template<typename T, typename... Ts>
        inline std::string _getJNISignature(T x, Ts... xs) const {
            return _getJNISignature(x) + _getJNISignature(xs...);
        }

        static void _reportError(const std::string &className, const std::string &methodName,
                                 const std::string &signature);
    };
}

#endif //SNAKAT_PROXY_JNIUTIL_H
