#include "AutoreleaseObject.h"
#include "Macros.h"

#include <algorithm>
#include <typeinfo>

namespace snakat {

    AutoreleaseObject::AutoreleaseObject() :
            _referenceCount(1) {
#if defined(DEBUG)
        __track__(this);
#endif
    }

    AutoreleaseObject::~AutoreleaseObject() {
#if defined(DEBUG)
        __untrack__(this);
#endif
    }

    void AutoreleaseObject::retain() {
        ASSERT(_referenceCount > 0, "reference count should greater than 0")

        ++_referenceCount;
    }

    void AutoreleaseObject::release() {
        ASSERT(_referenceCount > 0, "reference count should greater than 0")

        --_referenceCount;
        if (_referenceCount == 0) {
#if defined(DEBUG)
//            ASSERT(!App::getAutoreleasePool()->isClearing() && App::getAutoreleasePool()->contains(this), "The reference shouldn't be 0 because it is still in autorelease pool.");
#endif

            delete this;
        }
    }

    AutoreleaseObject *AutoreleaseObject::autorelease() {
//        App::getAutoreleasePool()->add(this);

        return this;
    }

    unsigned int AutoreleaseObject::getReferenceCount() const {
        return _referenceCount;
    }

#if defined(DEBUG)
    std::vector<AutoreleaseObject *> AutoreleaseObject::__trackedObjs__;

    void AutoreleaseObject::__track__(AutoreleaseObject *obj) {
        __trackedObjs__.push_back(obj);
    }

    void AutoreleaseObject::__untrack__(AutoreleaseObject *obj) {
        auto iter = std::find(__trackedObjs__.cbegin(), __trackedObjs__.cend(), obj);
        if (iter != __trackedObjs__.cend()) {
            __trackedObjs__.erase(iter);
        }
    }

    void AutoreleaseObject::__printTrack__() {
        if (__trackedObjs__.empty()) {
            LOGD("[memory] All bjects successfully cleaned up (no leaks detected).\n");
        } else {
            LOGD("[memory] WARNING: %d objects still active in memory.\n", __trackedObjs__.size());

            for (const auto &obj : __trackedObjs__) {
                const char *type = typeid(*obj).name();
                LOGD("[memory] LEAK: Object '%s' still active with reference count %d.\n",
                     (type ? type : ""), obj->getReferenceCount());
            }
        }
    }

#endif


    AutoreleasePool *AutoreleasePool::create() {
        auto autoreleasePool = new(std::nothrow) AutoreleasePool();
        if (autoreleasePool) {
            return autoreleasePool;
        }
        SAFE_DELETE(autoreleasePool);
        return nullptr;
    }

    AutoreleasePool::AutoreleasePool() :
            _isClearing(false) {
        _managedObjs.reserve(150);
    }

    AutoreleasePool::~AutoreleasePool() {
        clear();
    }

    void AutoreleasePool::add(AutoreleaseObject *obj) {
        _managedObjs.push_back(obj);
    }

    void AutoreleasePool::clear() {
        _isClearing = true;

        std::vector<AutoreleaseObject *> clearingObjs;
        clearingObjs.swap(_managedObjs);
        for (const auto &obj : clearingObjs) {
            obj->release();
        }

        _isClearing = false;
    }

    bool AutoreleasePool::contains(AutoreleaseObject *obj) const {
        return std::find(_managedObjs.cbegin(), _managedObjs.cend(), obj) != _managedObjs.cend();
    }

    bool AutoreleasePool::isClearing() const {
        return _isClearing;
    }

}
