#include <cassert>
#include <algorithm>
#include "InstancePool.h"


template<class T>
InstancePool<T>::InstancePool<T>(bool deleteOnQuit)
    : myDeleteOnQuit(deleteOnQuit)
{
}


template<class T>
InstancePool<T>::~InstancePool<T>()
{
    if(myDeleteOnQuit) {
        for(std::vector<T*>::iterator i=myFreeInstances.begin(); i!=myFreeInstances.end(); i++) {
            delete *i;
        }
    }
}


template<class T>
T*
InstancePool<T>::getFreeInstance()
{
    if(myFreeInstances.size()==0) {
        return 0;
    } else {
        T *instance = myFreeInstances.back();
        myFreeInstances.pop_back();
        return instance;
    }
}


template<class T>
void
InstancePool<T>::addFreeInstance(T *instance)
{
    myFreeInstances.push_back(instance);
}


template<class T>
void
InstancePool<T>::addFreeInstances(const std::vector<T*> instances)
{
    std::copy(instances.begin(), instances.end(),
        std::back_inserter(myFreeInstances));
}

