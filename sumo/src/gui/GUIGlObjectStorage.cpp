#include "GUIGlObject.h"
#include <map>
#include "GUIGlObjectStorage.h"
#include <iostream> // !!! debug only

using namespace std; // !!! debug only


GUIGlObjectStorage::GUIGlObjectStorage()
{
}


GUIGlObjectStorage::~GUIGlObjectStorage()
{
}


void
GUIGlObjectStorage::registerObject(GUIGlObject *object)
{
    _lock.lock();
    object->setGlID(myAktID);
    myMap[myAktID++] = object;
    _lock.unlock();
}


GUIGlObject *
GUIGlObjectStorage::getObjectBlocking(size_t id)
{
    _lock.lock();
    ObjectMap::iterator i=myMap.find(id);
    if(i==myMap.end()) {
        _lock.unlock();
        return 0;
    }
    GUIGlObject *o = (*i).second;
    return o;
}


void
GUIGlObjectStorage::remove(size_t id)
{
    _lock.lock();
    myMap.erase(id);
    _lock.unlock();
}


void
GUIGlObjectStorage::clear()
{
    _lock.lock();
    myMap.clear();
    myAktID = 0;
    _lock.unlock();
}


void
GUIGlObjectStorage::unblockObject()
{
    _lock.unlock();
}

