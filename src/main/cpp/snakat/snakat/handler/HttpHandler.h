//
//  HttpHandler.h
//  snakat
//
//  Created by Khanh Hoang Bui on 08/08/16.
//  Copyright © 2015 SNAKAT. All rights reserved.
//

#ifndef SNAKAT_PROXY_HTTPHANDLER_H
#define SNAKAT_PROXY_HTTPHANDLER_H

#include <civetweb/include/CivetServer.h>

namespace snakat {

    class HttpHandler : public CivetHandler {
    protected:
        HttpHandler();

    public:
        virtual ~HttpHandler();

        static const char * URI;

        static HttpHandler * create();

        virtual bool init();
    };
}

#endif //SNAKAT_PROXY_HTTPHANDLER_H
