//
//  ApiHandler.cpp
//  snakat
//
//  Created by Khanh Hoang Bui on 8/12/16.
//  Copyright © 2016 SNAKAT. All rights reserved.
//

#include "ApiHandler.h"
#include "../core/Macros.h"

namespace snakat {
    
    const char * ApiHandler::URI = "/api";
    
    ApiHandler * ApiHandler::create() {
        auto handler = new (std::nothrow) ApiHandler();
        if (handler && handler->init()) {
            return handler;
        }
        SAFE_DELETE(handler);
        return NULL;
    }
    
    bool ApiHandler::handleGet(CivetServer * server, struct mg_connection * conn) {
        auto requestInfo = mg_get_request_info(conn);
        std::string requestUri(requestInfo->request_uri);

        LOGD("ApiHandler::handleGet: %s", requestUri.c_str());

        return false;
    }
    
    bool ApiHandler::handlePost(CivetServer * server, struct mg_connection * conn) {
        auto requestInfo = mg_get_request_info(conn);
        std::string requestUri(requestInfo->request_uri);

        LOGD("ApiHandler::handlePost: %s", requestUri.c_str());

        return false;
    }

}
