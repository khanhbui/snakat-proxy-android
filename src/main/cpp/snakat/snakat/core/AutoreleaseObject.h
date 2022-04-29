#ifndef SNAKAT_PROXY_AUTORELEASEOBJECT_H
#define SNAKAT_PROXY_AUTORELEASEOBJECT_H

#include <vector>

namespace snakat {
    class AutoreleaseObject {
    private:
        unsigned int _referenceCount;

    protected:
        AutoreleaseObject();

    public:
        virtual ~AutoreleaseObject();

        void retain();
        void release();

        AutoreleaseObject* autorelease();

        unsigned int getReferenceCount() const;

        friend class AutoreleasePool;

#if defined(DEBUG)
        static std::vector<AutoreleaseObject*> __trackedObjs__;
        static void __track__(AutoreleaseObject *obj);
        static void __untrack__(AutoreleaseObject *obj);
        static void __printTrack__();
#endif
    };

    class AutoreleasePool {
    private:
        std::vector<AutoreleaseObject*> _managedObjs;

        bool _isClearing;

    public:
        AutoreleasePool();
        ~AutoreleasePool();

        static AutoreleasePool* create();

        void add(AutoreleaseObject *obj);

        void clear();

        bool contains(AutoreleaseObject *obj) const;
        bool isClearing() const;
    };
}

#endif //SNAKAT_PROXY_AUTORELEASEOBJECT_H
