#include "FileUtil.h"
#include "../core/Macros.h"

#include <cerrno>
#include <cstring>
#include <sys/stat.h>

#if PLATFORM_ANDROID
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif //PLATFORM_ANDROID

namespace snakat {

    FileUtil::FileUtil()
#if PLATFORM_ANDROID
            : _aAssetManager(nullptr)
#endif //PLATFORM_ANDROID
    {
    }

    FileUtil::~FileUtil() {
#if PLATFORM_ANDROID
        _aAssetManager = nullptr;
#endif //PLATFORM_ANDROID
    }

#if PLATFORM_ANDROID

    bool FileUtil::init(AAssetManager *aAssetManager) {
        _aAssetManager = aAssetManager;
#else
        bool FileUtil::init() {
            _aAssetManager = aAssetManager;
#endif //PLATFORM_ANDROID

        return true;
    }

    const char *FileUtil::getContent(const std::string &filename, size_t &size) {
#if PLATFORM_ANDROID
        AAsset *aAsset = AAssetManager_open(_aAssetManager, filename.c_str(),
                                            AASSET_MODE_STREAMING);
        if (aAsset) {
            size = static_cast<size_t>(AAsset_getLength(aAsset));
            char *buf = new char[size + 1];
            auto readNum = AAsset_read(aAsset, buf, size);
            AAsset_close(aAsset);

            if (readNum == size) {
                buf[size + 1] = '\0';
                return buf;
            }
        } else {
            FILE *fp = fopen(filename.c_str(), "rb");
            if (fp) {
                struct stat stat;
                if (fstat(fileno(fp), &stat) == -1) {
                    fclose(fp);
                } else {
                    size = static_cast<size_t>(stat.st_size);
                }

                char *buf = new char[size + 1];
                auto readNum = fread(buf, sizeof(char), size, fp);

                fclose(fp);

                if (readNum == size) {
                    buf[size + 1] = '\0';
                    return buf;
                }
            }
        }
#endif //PLATFORM_ANDROID

        return nullptr;
    }

    std::string FileUtil::getStringFromFile(const std::string &filename, size_t &size) {
        const char *content = getContent(filename, size);
        std::string ret = content ? content : "";
        SAFE_DELETE_ARRAY(content);

        return ret;
    }

    std::string FileUtil::getDirFromFilename(const std::string &filename) {
        auto index = filename.find_last_of('/');
        return index == std::string::npos ? filename : filename.substr(0, index);
    }

#if PLATFORM_ANDROID

    bool FileUtil::copyFromApk(const std::string &srcPath, const std::string &desPath) {
        AAsset *aAsset = AAssetManager_open(_aAssetManager, srcPath.c_str(), AASSET_MODE_STREAMING);
        if (aAsset) {
            if (makeDir(getDirFromFilename(desPath))) {
                FILE *out = fopen(desPath.c_str(), "w+");
                if (out) {
                    char buf[BUFSIZ];
                    int readNum;
                    while ((readNum = AAsset_read(aAsset, buf, BUFSIZ)) > 0) {
                        fwrite(buf, static_cast<size_t>(readNum), 1, out);
                    }

                    fclose(out);
                    AAsset_close(aAsset);

                    return true;
                }
            }
            AAsset_close(aAsset);
        }

        return false;
    }

#endif //PLATFORM_ANDROID

    bool FileUtil::makeDir(const std::string &dir) {
        if (!exist(dir)) {
            if (_makeDirRecursive(dir) < 0) {
                int errsv = errno;
                if (errsv != EEXIST) {
                    LOGE("Cannot create directory %s: %s", dir.c_str(), strerror(errsv));
                    return false;
                }
            }
        }
        return true;
    }

    int FileUtil::_makeDirRecursive(const std::string &dir) {
        char tmp[256];
        size_t len;

        snprintf(tmp, sizeof(tmp), "%s", dir.c_str());
        len = strlen(tmp);
        if (tmp[len - 1] == '/') {
            tmp[len - 1] = 0;
        }
        for (char *p = tmp + 1; *p; p++) {
            if (*p == '/') {
                *p = 0;
                mkdir(tmp, 0755);
                *p = '/';
            }
        }

        return mkdir(tmp, 0755);
    }

    bool FileUtil::exist(const std::string &path) {
        struct stat st;
        return stat(path.c_str(), &st) == 0;
    }

    bool FileUtil::isDirectory(const std::string &path) {
        struct stat st;
        if (stat(path.c_str(), &st) == 0) {
            return S_ISDIR(st.st_mode);
        }
        return false;
    }

}
