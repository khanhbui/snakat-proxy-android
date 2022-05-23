//
//  FileHandler.cpp
//  snakat
//
//  Created by Khanh Hoang Bui on 8/11/16.
//  Copyright © 2016 SNAKAT. All rights reserved.
//

#include "FileHandler.h"
#include "../core/Macros.h"
#include "../util/FileUtil.h"
#include "../util/Util.h"
#include "../Proxy.h"

#include <cstdio>
#include <cinttypes>
#include <cstdlib>

#if PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif //PLATFORM_ANDROID

#include <civetweb/include/civetweb.h>

namespace snakat {

    const char *FileHandler::URI = "/static";

    FileHandler::FileHandler() :
            _documentDir() {
    }

    FileHandler *
    FileHandler::create(const std::string &documentDir) {
        auto handler = new(std::nothrow) FileHandler();
        if (handler && handler->init(documentDir)) {
            return handler;
        }
        SAFE_DELETE(handler);
        return nullptr;
    }

    bool FileHandler::init(const std::string &documentDir) {
        if (HttpHandler::init()) {
            _documentDir = documentDir;
            return true;
        }
        return false;
    }

    bool FileHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
        LOGD("FileHandler::handleGet: _isRunning=%d", _isRunning);
        if (!_isRunning) {
            return false;
        }

        auto requestInfo = mg_get_request_info(conn);

        const char *requestUri = requestInfo->request_uri;
        LOGD("FileHandler::handleGet: uri=%s", requestUri);

        const std::string &filename = _documentDir + requestUri;
        bool exist = FileUtil::exist(filename);
        bool isDir = FileUtil::isDirectory(filename);

        LOGD("FileHandler::handleGet: filename=%s exist=%d isDir=%d", requestUri, exist, isDir);

        if (!exist) {
#if PLATFORM_ANDROID
            LOGD("FileHandler::handleGet: Copy %s => %s", requestUri + 1, filename.c_str());
            FileUtil::getInstance().copyFromApk(requestUri + 1, filename);
#endif //PLATFORM_ANDROID
        }

        LOGD("FileHandler::handleGet: Sending %s from cache", filename.c_str());
        mg_send_file(conn, filename.c_str());

        return true;
    }
}
