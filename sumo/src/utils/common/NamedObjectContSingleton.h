#ifndef NamedObjectContSingleton_h
#define NamedObjectContSingleton_h


#include "NamedObjectCont.h"

template<class T>
class NamedObjectContSingleton {
public:
    static NamedObjectCont<T> &getInstance() { return myObjects; }

private:
    static NamedObjectCont<T> myObjects;

};

template<class T> NamedObjectCont<T> NamedObjectContSingleton<T>::myObjects;

#endif
