#include "GUIGlObject.h"
#include <map>
#include "GUIGlObjectStorage.h"
#include <microsim/MSVehicle.h>
#include <guisim/GUIVehicle.h>
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
    myMap.erase(id);
    myBlocked[id] = o;
    _lock.unlock();
    return o;
}


void
GUIGlObjectStorage::remove(size_t id)
{
    _lock.lock();
    ObjectMap::iterator i=myMap.find(id);
    if(i==myMap.end()) {
        i = myBlocked.find(id);
        assert(i!=myBlocked.end());
        GUIGlObject *o = (*i).second;
        myMap.erase(id);
        my2Delete[id] = o;
        _lock.unlock();
    } else {
        GUIGlObject *o = (*i).second;
        myMap.erase(id);
        MSVehicle::remove(static_cast<GUIVehicle*>(o)->id());
        _lock.unlock();
    }
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
GUIGlObjectStorage::unblockObject(size_t id)
{
    _lock.lock();
    ObjectMap::iterator i=myBlocked.find(id);
    assert(i!=myBlocked.end());
    GUIGlObject *o = (*i).second;
    myBlocked.erase(id);
    myMap[id] = o;
    _lock.unlock();
}

