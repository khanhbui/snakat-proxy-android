//
// Created by Khanh Hoang Bui on 29/4/22.
//

#include "EmptyHandler.h"
#include "../core/Macros.h"

#include <civetweb/include/civetweb.h>

namespace snakat {

    const char *EmptyHandler::URI = "/";

    EmptyHandler::EmptyHandler() {
    }

    EmptyHandler *EmptyHandler::create() {
        auto handler = new(std::nothrow) EmptyHandler();
        if (handler && handler->init()) {
            return handler;
        }
        SAFE_DELETE(handler);
        return nullptr;
    }

    bool EmptyHandler::init() {
        if (HttpHandler::init()) {
            return true;
        }
        return false;
    }

    bool EmptyHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
        auto requestInfo = mg_get_request_info(conn);
        const char *requestUri = requestInfo->request_uri;
        LOGD("EmptyHandler::handleGet: uri=%s", requestUri);

        mg_send_http_error(conn, 503, "");

        return true;
    }

}
