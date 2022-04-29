//
//  Proxy.h
//  snakat
//
//  Created by Khanh Hoang Bui on 08/08/16.
//  Copyright © 2015 SNAKAT. All rights reserved.
//

#ifndef SNAKAT_PROXY_H
#define SNAKAT_PROXY_H

#include <functional>
#include <vector>
#include <string>

#include "core/Macros.h"
#include "core/Singleton.h"

class CivetServer;
struct CivetCallbacks;

struct mg_connection;

struct AAssetManager;

namespace snakat {

    class ApiHandler;

    class EmptyHandler;

    class FileHandler;

    class Proxy : public Singleton<Proxy> {
    private:
        std::vector<std::string> options;

        CivetServer *_civetServer;

        CivetCallbacks *_civetCallbacks;

        FileHandler *_fileHandler;
        EmptyHandler *_emptyHandler;
        ApiHandler *_apiHandler;

        bool _sslEnabled;
        std::string _sslCertFilename;
        std::string _sslCertPath;

        bool _isRunning;

    protected:
        static int _onInitConnection(const struct mg_connection *conn, void **conn_data);

        static void _onConnectionClose(const struct mg_connection *conn);

        static int _onLogMessage(const struct mg_connection *conn, const char *message);

        static int _onInitSSL(void *ssl_ctx, void *user_data);

        void addHandlers();

        void removeHandlers();

    public:
        Proxy();

        virtual ~Proxy();

        bool init(AAssetManager *assetManager, const std::string &documentsAbsDir,
                  const std::string &hostname, bool sslEnabled, const std::string &sslCertFilename,
                  const std::string &manifestFilename);

        void start();

        void pause();

        void resume();

        void stop();

        inline bool isStarted() const {
            return _civetServer != NULL;
        }

        inline bool isRunning() const {
            return isStarted() && _isRunning;
        }
    };
}

#endif //SNAKAT_PROXY_H
