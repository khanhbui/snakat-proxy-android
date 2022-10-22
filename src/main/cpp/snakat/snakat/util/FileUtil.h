#ifndef SNAKAT_PROXY_FILEUTIL_H
#define SNAKAT_PROXY_FILEUTIL_H

#include <string>

#include "../core/Singleton.h"

struct AAssetManager;

namespace snakat {

    class FileUtil : public Singleton<FileUtil> {
    private:
#if PLATFORM_ANDROID
        AAssetManager *_aAssetManager;
#endif

        static int _makeDirRecursive(const std::string &dir);

    public:
        FileUtil();
        virtual ~FileUtil();

#if PLATFORM_ANDROID
        bool init(AAssetManager *assetManager);
#else
        bool init();
#endif

        const char *getContent(const std::string &filename, size_t &size);

        std::string getStringFromFile(const std::string &filename, size_t &size);

        bool copyFromApk(const std::string &srcPath, const std::string &desPath);

        static bool makeDir(const std::string &dir);

        static std::string getDirFromFilename(const std::string &filename);

        static bool exist(const std::string &path);
        static bool isDirectory(const std::string &path);

        static bool writeFile(const std::string &filename, const std::string &content);
    };
}

#endif //SNAKAT_PROXY_FILEUTIL_H
