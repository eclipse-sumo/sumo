#ifndef InstancePool_h
#define InstancePool_h

#include <vector>

template<class T>
class InstancePool {
public:
    InstancePool(bool deleteOnQuit);
    ~InstancePool();
    T* getFreeInstance();
    void addFreeInstance(T *instance);
    void addFreeInstances(const std::vector<T*> instances);

private:
    std::vector<T*> myFreeInstances;
    bool myDeleteOnQuit;

};

#ifndef MSVC
#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "InstancePool.cpp"
#endif
#endif


#endif
