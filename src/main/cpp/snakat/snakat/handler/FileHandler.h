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

    class FileHandler : public HttpHandler {
    private:
        std::string _documentDir;

    public:
        static const char *URI;

        static FileHandler *create(const std::string &documentDir);

        virtual bool init(const std::string &documentDir);

        virtual bool handleGet(CivetServer *server, struct mg_connection *conn);

    protected:
        FileHandler();
    };
}

#endif //SNAKAT_PROXY_FILEHANDLER_H
