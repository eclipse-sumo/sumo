/****************************************************************************/
/// @file    GUIGlObjectStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A storage for retrival of displayed object using a numerical id
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include "GUIGlObject.h"
#include <map>
#include "GUIGlObjectStorage.h"
#include <iostream>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUIGlObjectStorage::GUIGlObjectStorage()
        : myAktID(0)
{}


GUIGlObjectStorage::~GUIGlObjectStorage()
{}


void
GUIGlObjectStorage::registerObject(GUIGlObject *object)
{
    _lock.lock();
    object->setGlID(myAktID);
    myMap[myAktID++] = object;
    _lock.unlock();
}


void
GUIGlObjectStorage::registerObject(GUIGlObject *object, size_t id)
{
    _lock.lock();
    object->setGlID(id);
    myMap[id] = object;
    _lock.unlock();
}


size_t
GUIGlObjectStorage::getUniqueID()
{
    _lock.lock();
    size_t ret = myAktID++;
    _lock.unlock();
    return ret;
}


GUIGlObject *
GUIGlObjectStorage::getObjectBlocking(size_t id)
{
    _lock.lock();
    ObjectMap::iterator i=myMap.find(id);
    if (i==myMap.end()) {
        i = myBlocked.find(id);
        if (i!=myBlocked.end()) {
            GUIGlObject *o = (*i).second;
            _lock.unlock();
            return o;
        }
        _lock.unlock();
        return 0;
    }
    GUIGlObject *o = (*i).second;
    myMap.erase(id);
    myBlocked[id] = o;
    _lock.unlock();
    return o;
}


bool
GUIGlObjectStorage::remove(size_t id)
{
    _lock.lock();
    ObjectMap::iterator i=myMap.find(id);
    if (i==myMap.end()) {
        i = myBlocked.find(id);
        assert(i!=myBlocked.end());
        GUIGlObject *o = (*i).second;
        myBlocked.erase(id);
        my2Delete[id] = o;
        _lock.unlock();
        return false;
    } else {
        myMap.erase(id);
        _lock.unlock();
        return true;
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
    if (i==myBlocked.end()) {
        _lock.unlock();
        return;
    }
    GUIGlObject *o = (*i).second;
    myBlocked.erase(id);
    myMap[id] = o;
    _lock.unlock();
}



/****************************************************************************/

