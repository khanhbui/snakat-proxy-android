//
// Created by Khanh Hoang Bui on 29/4/22.
//

#ifndef SNAKAT_APPS_EMPTYHANDLER_H
#define SNAKAT_APPS_EMPTYHANDLER_H

#include "HttpHandler.h"

#include <functional>
#include <map>

struct mg_connection;

namespace snakat {

    class EmptyHandler : public HttpHandler {
    public:
        static const char *URI;

        static EmptyHandler *create();

        virtual bool init();

        virtual bool handleGet(CivetServer *server, struct mg_connection *conn);

    protected:
        EmptyHandler();
    };
}


#endif //SNAKAT_APPS_EMPTYHANDLER_H
