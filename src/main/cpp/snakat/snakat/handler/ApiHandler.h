//
//  ApiHandler.h
//  snakat
//
//  Created by Khanh Hoang Bui on 8/12/16.
//  Copyright © 2016 SNAKAT. All rights reserved.
//

#ifndef SNAKAT_PROXY_APIHANDLER_H
#define SNAKAT_PROXY_APIHANDLER_H

#include "HttpHandler.h"

namespace snakat {
    
    class ApiHandler : public HttpHandler {
    public:
        static const char * URI;
        
        static ApiHandler * create();
        
        virtual bool handleGet(CivetServer * server, struct mg_connection * conn);
        virtual bool handlePost(CivetServer * server, struct mg_connection * conn);
    };
}

#endif //SNAKAT_PROXY_APIHANDLER_H
