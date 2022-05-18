//
//  HttpHandler.cpp
//  snakat
//
//  Created by Khanh Hoang Bui on 08/08/16.
//  Copyright © 2015 SNAKAT. All rights reserved.
//

#include "HttpHandler.h"
#include "../core/Macros.h"

namespace snakat {

    const char * HttpHandler::URI = "";

    HttpHandler::HttpHandler() :
        _isRunning(true) {
    }

    HttpHandler::~HttpHandler() = default;

    HttpHandler * HttpHandler::create() {
        auto httpHandler = new (std::nothrow) HttpHandler();
        if (httpHandler && httpHandler->init()) {
            return httpHandler;
        }
        SAFE_DELETE(httpHandler);
        return nullptr;
    }

    bool HttpHandler::init() {
        return true;
    }

    void HttpHandler::pause() {
        _isRunning = false;
    }

    void HttpHandler::resume() {
        _isRunning = true;
    }
}
