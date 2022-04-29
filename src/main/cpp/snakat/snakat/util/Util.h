//
//  Util.h
//  snakat
//
//  Created by Khanh Hoang Bui on 12/30/15.
//  Copyright © 2015 SNAKAT. All rights reserved.
//

#ifndef SNAKAT_PROXY_UTIL_H
#define SNAKAT_PROXY_UTIL_H

#include <map>
#include <sstream>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexpansion-to-defined"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#pragma clang diagnostic pop

namespace snakat {

    class Util {
    private:
        static std::string _defaultUserAgent;

        static std::string _appVersionCode;

    public:
        template<typename T>
        static std::string toString(T value) {
            std::ostringstream os;
            os << value;
            return os.str();
        }

        template<typename T>
        static T stringTo(const char *str) {
            std::istringstream is(str);
            T output;
            is >> output;
            return output;
        }

        static std::string format(const char *format, ...);
    };

}

#endif //SNAKAT_PROXY_UTIL_H
