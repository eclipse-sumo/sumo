/****************************************************************************/
/// @file    GUIGlObjectStorage.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A storage for displayed objects via their numerical id
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <iostream>
#include <cassert>
#include <utils/foxtools/MFXMutex.h>
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static variables (instances in this case)
// ===========================================================================
GUIGlObjectStorage GUIGlObjectStorage::gIDStorage;


// ===========================================================================
// method definitions
// ===========================================================================
GUIGlObjectStorage::GUIGlObjectStorage()
    : myAktID(0) {}


GUIGlObjectStorage::~GUIGlObjectStorage() {}


GUIGlID
GUIGlObjectStorage::registerObject(GUIGlObject* object, const std::string& fullName) {
    myLock.lock();
    GUIGlID id = myAktID++;
    myMap[id] = object;
    myFullNameMap[fullName] = object;
    myLock.unlock();
    return id;
}


GUIGlObject*
GUIGlObjectStorage::getObjectBlocking(GUIGlID id) {
    myLock.lock();
    ObjectMap::iterator i = myMap.find(id);
    if (i == myMap.end()) {
        i = myBlocked.find(id);
        if (i != myBlocked.end()) {
            GUIGlObject* o = (*i).second;
            myLock.unlock();
            return o;
        }
        myLock.unlock();
        return 0;
    }
    GUIGlObject* o = (*i).second;
    myMap.erase(id);
    myBlocked[id] = o;
    myLock.unlock();
    return o;
}


GUIGlObject*
GUIGlObjectStorage::getObjectBlocking(const std::string& fullName) {
    myLock.lock();
    if (myFullNameMap.count(fullName)) {
        GUIGlID id = myFullNameMap[fullName]->getGlID();
        myLock.unlock();
        return getObjectBlocking(id);
    } else {
        myLock.unlock();
        return 0;
    }
}


bool
GUIGlObjectStorage::remove(GUIGlID id) {
    myLock.lock();
    ObjectMap::iterator i = myMap.find(id);
    if (i == myMap.end()) {
        i = myBlocked.find(id);
        assert(i != myBlocked.end());
        GUIGlObject* o = (*i).second;
        myFullNameMap.erase(o->getFullName());
        myBlocked.erase(id);
        my2Delete[id] = o;
        myLock.unlock();
        return false;
    } else {
        myFullNameMap.erase(i->second->getFullName());
        myMap.erase(id);
        myLock.unlock();
        return true;
    }
}


void
GUIGlObjectStorage::clear() {
    myLock.lock();
    myMap.clear();
    myAktID = 0;
    myLock.unlock();
}


void
GUIGlObjectStorage::unblockObject(GUIGlID id) {
    myLock.lock();
    ObjectMap::iterator i = myBlocked.find(id);
    if (i == myBlocked.end()) {
        myLock.unlock();
        return;
    }
    GUIGlObject* o = (*i).second;
    myBlocked.erase(id);
    myMap[id] = o;
    myLock.unlock();
}


std::set<GUIGlID>
GUIGlObjectStorage::getAllIDs() const {
    std::set<GUIGlID> result;
    for (ObjectMap::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
        result.insert(it->first);
    }
    return result;
}
/****************************************************************************/

