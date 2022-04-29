//
// Created by Khanh Hoang Bui on 25/4/22.
//

#ifndef SNAKAT_PROXY_SINGLETON_H
#define SNAKAT_PROXY_SINGLETON_H

namespace snakat {

    template<typename T>
    class Singleton {
    public:
        static T &getInstance() {
            static const std::unique_ptr<T> instance(new T());
            return *instance;
        }

    protected:
        Singleton() {}

        ~Singleton() {}

    private:
        T *instance;

    public:
        Singleton(Singleton const &) = delete;

        Singleton &operator=(Singleton const &) = delete;
    };

}

#endif //SNAKAT_PROXY_SINGLETON_H
