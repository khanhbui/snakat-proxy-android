//
//  FileHandler.h
//  snakat
//
//  Created by Khanh Hoang Bui on 8/11/16.
//  Copyright © 2016 SNAKAT. All rights reserved.
//

#ifndef SNAKAT_PROXY_FILEHANDLER_H
#define SNAKAT_PROXY_FILEHANDLER_H

#include "HttpHandler.h"

#include <functional>
#include <map>

struct mg_connection;

namespace snakat {

    class FileUtil;

    struct FileInfo {
        std::string name;
        std::string hash;
        long size;

        static FileInfo parse(const std::string &str) {
            auto fileInfo = FileInfo();

            auto firstComma = str.find(',');
            if (firstComma != std::string::npos) {
                fileInfo.name = str.substr(0, firstComma);

                firstComma++;

                auto secondComma = str.find(',', firstComma);
                if (secondComma != std::string::npos) {
                    fileInfo.hash = str.substr(firstComma, secondComma - firstComma);
                    fileInfo.size = std::stol(str.substr(secondComma + 1));
                } else {
                    fileInfo.hash = str.substr(firstComma);
                    fileInfo.size = 0;
                }
            }

            return fileInfo;
        }

        const std::string toString() const {
            return name + "," + hash + "," + std::to_string(size);
        }
    };

    class FileHandler : public HttpHandler {
    private:
        std::string _documentDir;
        std::string _manifestFilename;

        std::string _localManifestPath;

        std::map<std::string, FileInfo> _localManifest;
        std::map<std::string, FileInfo> _remoteManifest;
        std::vector<FileInfo> _updatedFiles;

    public:
        static const char *URI;

        static FileHandler *
        create(const std::string &documentDir, const std::string &manifestFilename);

        virtual bool init(const std::string &documentDir, const std::string &manifestFilename);

        virtual bool handleGet(CivetServer *server, struct mg_connection *conn);

    private:
//        bool _loadLocalManifest();
//        bool _loadRemoteManifest();

    protected:
        FileHandler();
    };
}

#endif //SNAKAT_PROXY_FILEHANDLER_H
