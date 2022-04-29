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
            _documentDir(),
            _manifestFilename(),
            _localManifestPath(),
            _localManifest(),
            _remoteManifest(),
            _updatedFiles() {
    }

    FileHandler *FileHandler::create(const std::string &documentDir, const std::string &manifestFilename) {
        auto handler = new(std::nothrow) FileHandler();
        if (handler &&
            handler->init(documentDir, manifestFilename)) {
            return handler;
        }
        SAFE_DELETE(handler);
        return nullptr;
    }

    bool FileHandler::init(const std::string &documentDir,const std::string &manifestFilename) {
        if (HttpHandler::init()) {
            _documentDir = documentDir;

            _manifestFilename = manifestFilename;
            _localManifestPath = _documentDir + "/" + _manifestFilename;

            return true;
        }
        return false;
    }

    bool FileHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
        auto requestInfo = mg_get_request_info(conn);

        const char *requestUri = requestInfo->request_uri;
        LOGD("FileHandler::handleGet: uri=%s", requestUri);

        const std::string &filename = _documentDir + requestUri;
        bool exist = FileUtil::exist(filename);
        bool isDir = FileUtil::isDirectory(filename);

        LOGD("FileHandler::handleGet: filename=%s exist=%d isDir=%d", requestUri, exist, isDir);

//        const char *localFilename = requestUri + strlen(URI) + 1;

//        if (_needToDownload(localFilename)) {
//            _downloadFile(localFilename, cacheFilename);
//        }

        if (!exist) {
            LOGD("FileHandler::handleGet: Copy %s => %s", requestUri + 1, filename.c_str());
            FileUtil::getInstance().copyFromApk(requestUri + 1, filename);
        }

        LOGD("FileHandler::handleGet: Sending %s from cache", filename.c_str());
        mg_send_file(conn, filename.c_str());

        return true;
    }

//    bool FileHandler::_needToDownload(const std::string &filename) {
//        LOG("FileHandler::_needToDownload %s", filename.c_str());
//
//        const auto remoteHash = _remoteManifest.find(filename);
//        if (remoteHash == _remoteManifest.end()) {
//            LOG("FileHandler::_needToDownload %s not found in remote manifest", filename.c_str());
//            return false;
//        }
//
//        const auto localHash = _localManifest.find(filename);
//        if (localHash == _localManifest.end()) {
//            LOG("FileHandler::_needToDownload %s not found in local manifest", filename.c_str());
//            return true;
//        }
//
//        LOG("FileHandler::_needToDownload %s comparing %s vs %s", filename.c_str(),
//            remoteHash->second.hash.c_str(), localHash->second.hash.c_str());
//
//        return remoteHash->second.hash.compare(localHash->second.hash) != 0;
//    }

//    void FileHandler::_changeState(snakat::FileHandler::BundleState to) {
//        LOG("FileHandler::_changeState: FROM %d TO %d", _bundleState, to);
//
//        _bundleState = to;
//    }

//    void FileHandler::loadBundle(const std::function<void(float progress)> &progressCallback,
//                                 const std::function<void(int errorCode,
//                                                          long data)> &completeCallback) {
//        auto error = NONE;
//        auto data = 0L;
//        auto finished = false;
//        do {
//            switch (_bundleState) {
//                case LOAD_LOCAL_MANIFEST:
//                    finished = _loadLocalManifest();
//                    break;
//
//                case LOAD_REMOTE_MANIFEST:
//                    finished = _loadRemoteManifest(error);
//                    break;
//
//                case PREPARE_UPDATED_FILES:
//                    finished = _prepareUpdatedFiles(data);
//                    break;
//
//                case DOWNLOAD_UPDATED_FILES:
//                    finished = _downloadUpdatedFiles(progressCallback, error);
//                    break;
//
//                case SAVE_REMOTE_MANIFEST:
//                    finished = _saveRemoteManifest(error);
//                    break;
//
//                case DONE_LOADING_BUNDLE:
//                    finished = true;
//                    break;
//            }
//        } while (!finished);
//
//        completeCallback(error, data);
//    }

//    bool FileHandler::_loadLocalManifest() {
//        if (_localManifest.empty()) {
//            size_t size;
//
//            const std::string &apkFilename = _localDir + "/" + _manifestFilename;
//            const std::string &apkContent = _fileUtil->getStringFromFile(apkFilename.c_str(), size);
//            _generateManifest(apkContent, _localManifest, "EMBEDDED");
//
//            const std::string &localContent = _fileUtil->getStringFromFile(
//                    _localManifestPath.c_str(), size);
//            _generateManifest(localContent, _localManifest, "LOCAL");
//        }
//
//        _changeState(LOAD_REMOTE_MANIFEST);
//        return false;
//    }

//    bool FileHandler::_loadRemoteManifest(BundleError &error) {
//        if (_remoteManifest.empty()) {
//            const std::string &tempFilename = _localManifestPath + "_temp";
//            if (!_downloadFile(_manifestFilename, tempFilename)) {
//                error = CAN_NOT_DOWNLOAD_REMOTE_MANIFEST;
//                return true;
//            }
//
//            size_t size;
//            const std::string &remoteContent = _fileUtil->getStringFromFile(tempFilename.c_str(),
//                                                                            size);
//            _generateManifest(remoteContent, _remoteManifest, "REMOTE");
//        }
//
//        _changeState(PREPARE_UPDATED_FILES);
//        return false;
//    }

//    bool FileHandler::_prepareUpdatedFiles(long &bytes) {
//        bytes = 0L;
//
//        if (_updatedFiles.empty()) {
//            if (_localManifest.empty()) {
//                for (const auto &remote: _remoteManifest) {
//                    const auto &info = remote.second;
//
//                    _updatedFiles.push_back(info);
//                    bytes += info.size;
//                }
//            } else {
//                for (const auto &remote: _remoteManifest) {
//                    const auto &filename = remote.first;
//                    const auto &info = remote.second;
//
//                    const auto &cached = _localManifest.find(filename);
//                    if (cached == _localManifest.end() ||
//                        cached->second.hash.compare(info.hash) != 0) {
//                        _updatedFiles.push_back(info);
//                        bytes += info.size;
//                    } else {
//                        LOG("FileHandler::_prepareUpdatedFiles: Hit cache %s", info.toString());
//                    }
//                }
//            }
//        }
//
//        _changeState(DOWNLOAD_UPDATED_FILES);
//        return bytes > 0;
//    }
//
//    bool
//    FileHandler::_downloadUpdatedFiles(const std::function<void(float progress)> &progressCallback,
//                                       BundleError &error) {
//        if (_updatedFiles.empty()) {
//            progressCallback(100);
//
//            _changeState(DONE_LOADING_BUNDLE);
//            return false;
//        }
//
//        auto totalBytes = 0L;
//        for (const auto it: _updatedFiles) {
//            totalBytes += it.size;
//        }
//        auto downloadBytes = 0L;
//
//        auto total = _updatedFiles.size();
//        unsigned int remaining;
//        while ((remaining = _updatedFiles.size()) > 0) {
//            const auto &info = _updatedFiles.front();
//            const auto &remoteFilename = info.name;
//            const auto &localFilename = _cachesAbsDir + "/" + _localDir + "/" + remoteFilename;
//
//            LOGINFO("FileHandler::_downloadUpdatedFiles: (%d/%d) %s => %s", total - remaining,
//                    total, remoteFilename.c_str(), localFilename.c_str());
//
//            if (_downloadFile(remoteFilename, localFilename)) {
//                _localManifest[remoteFilename] = info;
//                _updatedFiles.erase(_updatedFiles.begin());
//
//                downloadBytes += info.size;
//                progressCallback(float(downloadBytes) / totalBytes);
//            } else {
//                error = CAN_NOT_DOWNLOAD_UPDATED_FILE;
//                return true;
//            }
//        }
//
//        _changeState(SAVE_REMOTE_MANIFEST);
//        return false;
//    }
//
//    bool FileHandler::_saveRemoteManifest(BundleError &error) {
//        if (_fileUtil->makeDir(_fileUtil->getDirFromFilename(_localManifestPath))) {
//            FILE *fp = fopen(_localManifestPath.c_str(), "w+b");
//            if (fp) {
//                std::string data = "";
//                for (auto &it : _localManifest) {
//                    data.append(it.second.toString()).append("\n");
//                }
//
//                fwrite(&data[0], sizeof(char), data.length(), fp);
//                fclose(fp);
//            } else {
//                error = CAN_NOT_SAVE_REMOTE_MANIFEST;
//                return true;
//            }
//        }
//
//        LOG("FileHandler::_saveRemoteManifest: %s", _localManifestPath.c_str());
//
//        _changeState(DONE_LOADING_BUNDLE);
//        return false;
//    }
//
//    size_t FileHandler::_generateManifest(const std::string &data,
//                                          std::map<std::string, FileInfo> &manifest,
//                                          const char *label) {
//        if (data.empty()) {
//            return 0;
//        }
//
//        size_t last = 0;
//        size_t endl;
//        while ((endl = data.find('\n', last)) != std::string::npos) {
//            auto line = data.substr(last, endl - last);
//            last = endl + 1;
//
//            auto p = line.find_first_not_of(" \t\n\r\f\v");
//            if (p != std::string::npos && p > 0) {
//                line.erase(0, p);
//            }
//            p = line.find_last_not_of(" \t\n\r\f\v");
//            if (p != std::string::npos && ++p < line.length()) {
//                line.erase(p, line.length() - p);
//            }
//            if (line.empty()) {
//                continue;
//            }
//
//            auto fileInfo = FileInfo::parse(line);
//            if (fileInfo.name.empty() || fileInfo.hash.empty()) {
//                continue;
//            }
//            manifest[fileInfo.name] = fileInfo;
//
//            LOG("FileHandler::_generateManifest: %s: %s", label, fileInfo.toString());
//        }
//
//        return last;
//    }
//
//    bool FileHandler::_downloadFile(const std::string &remoteFilename,
//                                    const std::string &localFilename) {
//        int statusCode = -1;
//        char ebuf[MG_BUF_LEN];
//        mg_connection *downloadConnection = mg_download(
//                _wwwURL.c_str(), _wwwPort, _wwwSSL ? 1 : 0,
//                ebuf, sizeof(ebuf),
//                "GET /%s/%s HTTP/1.0\r\nHost: %s\r\n\r\n",
//                _wwwDir.c_str(), remoteFilename.c_str(), _wwwURL.c_str()
//        );
//
//        if (downloadConnection) {
//            auto responseInfo = mg_get_response_info(downloadConnection);
//            statusCode = responseInfo->status_code;
//
//            if (statusCode == 200) {
//                if (_fileUtil->makeDir(_fileUtil->getDirFromFilename(localFilename))) {
//                    FILE *fp = fopen(localFilename.c_str(), "w+b");
//                    if (fp) {
//                        char data[MG_BUF_LEN];
//                        while (true) {
//                            auto readNum = mg_read(downloadConnection, data, MG_BUF_LEN);
//                            if (readNum <= 0) {
//                                break;
//                            }
//
//                            fwrite(data, sizeof(char), static_cast<size_t>(readNum), fp);
//                        }
//                        fclose(fp);
//
//                        LOG("FileHandler::_downloadFile Downloaded %s://%s:%d/%s/%s",
//                            _wwwSSL ? "https" : "http", _wwwURL.c_str(), _wwwPort, _wwwDir.c_str(),
//                            remoteFilename.c_str());
//
//                        mg_close_connection(downloadConnection);
//                        return true;
//                    }
//                }
//            }
//
//            mg_close_connection(downloadConnection);
//        }
//
//        LOG("FileHandler::_downloadFile Cannot download %s://%s:%d/%s/%s [%s(%d)]",
//            _wwwSSL ? "https" : "http", _wwwURL.c_str(), _wwwPort, _wwwDir.c_str(),
//            remoteFilename.c_str(), ebuf, statusCode);
//
//        return false;
//    }
}
