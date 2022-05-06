//
//  Proxy.cpp
//  snakat
//
//  Created by Khanh Hoang Bui on 08/08/16.
//  Copyright © 2015 SNAKAT. All rights reserved.
//

#include "Proxy.h"
#include "handler/ApiHandler.h"
#include "handler/EmptyHandler.h"
#include "handler/FileHandler.h"
#include "util/FileUtil.h"
#include "core/Pool.h"
#include "core/Singleton.h"

#include <memory>

#include <civetweb/include/CivetServer.h>
#include <civetweb/include/civetweb.h>

#if PLATFORM_ANDROID
#include "util/JniUtil.h"
#include <android/asset_manager.h>
#endif

namespace snakat {

    Proxy::Proxy() :
            _civetServer(nullptr),
            _civetCallbacks(nullptr),
            _apiHandler(nullptr),
            _emptyHandler(nullptr),
            _fileHandler(nullptr),
            _sslEnabled(false),
            _sslCertFilename(),
            _sslCertPath(),
            _isRunning(false) {
        options.clear();
    }

    Proxy::~Proxy() {
        options.clear();

        SAFE_DELETE(_civetCallbacks);
        SAFE_DELETE(_apiHandler);
        SAFE_DELETE(_emptyHandler);
        SAFE_DELETE(_fileHandler);
    }

#if PLATFORM_ANDROID
    bool Proxy::init(JavaVM *javaVM, AAssetManager *assetManager,
#else
    bool init(
#endif
                const std::string &documentsAbsDir, const std::string &hostname,
                bool sslEnabled, const std::string &sslCertFilename) {

        LOGI("Proxy's config: {\n\tdocumentsAbsDir: %s, \n\thostname: %s, \n\tsslEnabled: %d, \n\tsslFilename: %s\n}",
             documentsAbsDir.c_str(), hostname.c_str(), sslEnabled, sslCertFilename.c_str());

#if PLATFORM_ANDROID
        JniUtil::getInstance().init(javaVM);
        FileUtil::getInstance().init(assetManager);
#else
        FileUtil::getInstance().init();
#endif

        initOptions(documentsAbsDir, hostname, sslEnabled, sslCertFilename);
        initCivetCallback();

        _apiHandler = ApiHandler::create();
        _emptyHandler = EmptyHandler::create();
        _fileHandler = FileHandler::create(documentsAbsDir);

        return true;
    }

    void Proxy::start() {
        if (isStarted()) {
            LOGW("Proxy has been started already!");
            return;
        }

        if (_sslEnabled) {
            LOGD("PEM path %s", _sslCertPath.c_str());
            if (!FileUtil::exist(_sslCertPath)) {
                if (FileUtil::getInstance().copyFromApk(_sslCertFilename, _sslCertPath)) {
                    LOGD("Copied pem from apk to %s", _sslCertPath.c_str());
                } else {
                    LOGE("SSL is enabled but there is no SSL Cert file.");
                    LOGD("Cannot copy pem from apk to %s", _sslCertPath.c_str());
                }
            }
        }

        _isRunning = true;
        _civetServer = new CivetServer(options, _civetCallbacks);
        addHandlers();
    }

    void Proxy::pause() {
        if (!isStarted()) {
            LOGW("Proxy has NOT been started yet!");
            return;
        }
        if (!isRunning()) {
            LOGW("Proxy has been paused already!");
            return;
        }

        _isRunning = false;
        removeHandlers();
    }

    void Proxy::resume() {
        if (!isStarted()) {
            LOGW("Proxy has NOT been started yet!");
            return;
        }
        if (isRunning()) {
            LOGW("Proxy has been running already!");
            return;
        }

        _isRunning = true;
        addHandlers();
    }

    void Proxy::stop() {
        if (!isStarted()) {
            LOGW("Proxy has NOT been started yet!");
            return;
        }

        _isRunning = false;
        removeHandlers();
        _civetServer->close();
        SAFE_DELETE(_civetServer);
    }

    void Proxy::initOptions(const std::string &documentsAbsDir, const std::string &hostname,
                            bool sslEnabled, const std::string &sslCertFilename) {
        options.emplace_back("document_root");
        options.push_back(documentsAbsDir);

        options.emplace_back("listening_ports");
        options.push_back(hostname);

        options.emplace_back("num_threads");
        options.emplace_back("3");

        options.emplace_back("enable_keep_alive");
        options.emplace_back("no");

        options.emplace_back("enable_directory_listing");
        options.emplace_back("yes");

        if (sslEnabled) {
            _sslEnabled = true;
            _sslCertFilename = sslCertFilename;
            _sslCertPath = documentsAbsDir + "/.cert/" + sslCertFilename;

            options.emplace_back("ssl_certificate");
            options.emplace_back(_sslCertPath.c_str());

            options.emplace_back("ssl_protocol_version");
            options.emplace_back("3");

            options.emplace_back("ssl_cipher_list");
            options.emplace_back("DES-CBC3-SHA:AES128-SHA:AES128-GCM-SHA256");
        } else {
            _sslEnabled = false;
            _sslCertFilename = "";
            _sslCertPath = "";
        }
    }

    void Proxy::initCivetCallback() {
        _civetCallbacks = new CivetCallbacks();
#if PLATFORM_ANDROID
        _civetCallbacks->init_connection = _onInitConnection;
        _civetCallbacks->connection_close = _onConnectionClose;
#endif
        _civetCallbacks->log_message = _onLogMessage;
        _civetCallbacks->init_ssl = _onInitSSL;
    }

    void Proxy::addHandlers() {
        _civetServer->removeHandler(EmptyHandler::URI);

        _civetServer->addHandler(ApiHandler::URI, _apiHandler);
        _civetServer->addHandler(FileHandler::URI, _fileHandler);
    }

    void Proxy::removeHandlers() {
        _civetServer->removeHandler(ApiHandler::URI);
        _civetServer->removeHandler(FileHandler::URI);

        _civetServer->addHandler(EmptyHandler::URI, _emptyHandler);
    }

    int Proxy::_onInitConnection(const struct mg_connection *conn, void **conn_data) {
#ifdef DEBUG
        auto info = mg_get_request_info(conn);
        if (info) {
            LOGD("InitConnection [%s] %s", info->request_method, info->request_uri);
        } else {
            LOGD("InitConnection");
        }
#endif //DEBUG

        return 1;
    }

    void Proxy::_onConnectionClose(const struct mg_connection *conn) {
#ifdef DEBUG
        auto info = mg_get_request_info(conn);
        if (info) {
            LOGD("ConnectionClose [%s] %s", info->request_method, info->request_uri);
        }
#endif //DEBUG
    }

    int Proxy::_onLogMessage(const struct mg_connection *conn, const char *message) {
#ifdef DEBUG
        auto info = mg_get_request_info(conn);
        if (info) {
            LOGD("LogMessage [%s] %s: %s", info->request_method, info->request_uri, message);
        } else {
            LOGD("LogMessage %s", message);
        }
#endif //DEBUG

        return 1;
    }

    int Proxy::_onInitSSL(void *ssl_ctx, void *user_data) {
        return 0;
    }

}
