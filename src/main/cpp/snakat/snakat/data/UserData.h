//
// Created by Khanh Hoang Bui on 26/4/22.
//

#ifndef SNAKAT_PROXY_USERDATA_H
#define SNAKAT_PROXY_USERDATA_H


#include "../core/Pool.h"

#include <cstddef>

namespace snakat {

    class UserData {
    private:
        const char *_fileContent;
        size_t _fileSize;

        bool _isDeactive;

    protected:
        UserData();

    public:
        virtual ~UserData();

        static UserData *create();

        void reset();

        const char *getFileContent(const std::string &filename, size_t &size);

        void setDeactive();

        bool isDeactive() const;
    };

    class UserDataPool : public Pool<UserData> {
    public:
        static UserDataPool *create();
    };
}


#endif //SNAKAT_PROXY_USERDATA_H
