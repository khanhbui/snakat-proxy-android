//
// Created by Khanh Hoang Bui on 26/4/22.
//

#include "UserData.h"

#include "../core/Macros.h"
#include "../util/FileUtil.h"

#include <new>

namespace snakat {

    UserData *UserData::create() {
        auto userData = new(std::nothrow) UserData();
        if (userData) {
            return userData;
        }
        SAFE_DELETE(userData);
        return nullptr;
    }

    UserData::UserData() :
            _fileContent(nullptr),
            _fileSize(0),
            _isDeactive(false) {
    }

    UserData::~UserData() {
        SAFE_DELETE(_fileContent);
    }

    void UserData::reset() {
        _isDeactive = false;

        SAFE_DELETE(_fileContent);
        _fileSize = 0;
    }

    const char *UserData::getFileContent(const std::string &filename, size_t &size) {
        if (!_fileContent) {
            _fileContent = FileUtil::getInstance().getContent(filename.c_str(), _fileSize);
        }

        size = _fileSize;
        return _fileContent;
    }

    void UserData::setDeactive() {
        _isDeactive = true;
    }

    bool UserData::isDeactive() const {
        return _isDeactive;
    }


    UserDataPool *UserDataPool::create() {
        auto userDataPool = new(std::nothrow) UserDataPool();
        if (userDataPool) {
            return userDataPool;
        }
        SAFE_DELETE(userDataPool);
        return nullptr;
    }
}
