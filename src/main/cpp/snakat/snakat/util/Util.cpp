//
//  Util.cpp
//  snakat
//
//  Created by Khanh Hoang Bui on 12/30/15.
//  Copyright © 2015 SNAKAT. All rights reserved.
//

#include "Util.h"
#include "../core/Macros.h"

#include <sstream>

namespace snakat {

    std::string Util::format(const char *format, ...) {
        char buf[MAX_LEN];

        va_list args;
        va_start(args, format);
        vsnprintf(buf, MAX_LEN, format, args);
        va_end(args);

        return buf;
    }

}
