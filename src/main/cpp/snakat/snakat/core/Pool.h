//
// Created by Khanh Hoang Bui on 6/23/20.
//

#ifndef SNAKAT_PROXY_POOL_H
#define SNAKAT_PROXY_POOL_H

#include <vector>

#include "Macros.h"

namespace snakat {

    template<class T>
    class Pool {
    private:
        std::vector<T *> _pool;

    protected:
        Pool<T>();

    public:
        ~Pool<T>();

        T *get();
    };

    template<class T>
    Pool<T>::Pool(): _pool() {
        _pool.reserve(10);
    }

    template<class T>
    Pool<T>::~Pool() {
        for (auto item : _pool) {
            SAFE_DELETE(item);
        }
        _pool.clear();
    }

    template<class T>
    T *Pool<T>::get() {
        for (const auto &item : _pool) {
            if (item->isDeactive()) {
                item->reset();
                return item;
            }
        }

        auto item = T::create();
        _pool.push_back(item);

        return item;
    }

}

#endif //SNAKAT_PROXY_POOL_H
