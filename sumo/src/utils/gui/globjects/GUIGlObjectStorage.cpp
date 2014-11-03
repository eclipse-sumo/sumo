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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
    AbstractMutex::ScopedLocker locker(myLock);
    GUIGlID id = myAktID++;
    myMap[id] = object;
    myFullNameMap[fullName] = object;
    return id;
}


GUIGlObject*
GUIGlObjectStorage::getObjectBlocking(GUIGlID id) {
    AbstractMutex::ScopedLocker locker(myLock);
    ObjectMap::iterator i = myMap.find(id);
    if (i == myMap.end()) {
        i = myBlocked.find(id);
        if (i != myBlocked.end()) {
            GUIGlObject* o = (*i).second;
            return o;
        }
        return 0;
    }
    GUIGlObject* o = (*i).second;
    myMap.erase(id);
    myBlocked[id] = o;
    return o;
}


GUIGlObject*
GUIGlObjectStorage::getObjectBlocking(const std::string& fullName) {
    AbstractMutex::ScopedLocker locker(myLock);
    if (myFullNameMap.count(fullName)) {
        GUIGlID id = myFullNameMap[fullName]->getGlID();
        return getObjectBlocking(id);
    }
    return 0;
}


bool
GUIGlObjectStorage::remove(GUIGlID id) {
    AbstractMutex::ScopedLocker locker(myLock);
    ObjectMap::iterator i = myMap.find(id);
    if (i == myMap.end()) {
        i = myBlocked.find(id);
        assert(i != myBlocked.end());
        GUIGlObject* o = (*i).second;
        myFullNameMap.erase(o->getFullName());
        myBlocked.erase(id);
        my2Delete[id] = o;
        return false;
    }
    myFullNameMap.erase(i->second->getFullName());
    myMap.erase(id);
    return true;
}


void
GUIGlObjectStorage::clear() {
    AbstractMutex::ScopedLocker locker(myLock);
    myMap.clear();
    myAktID = 0;
}


void
GUIGlObjectStorage::unblockObject(GUIGlID id) {
    AbstractMutex::ScopedLocker locker(myLock);
    ObjectMap::iterator i = myBlocked.find(id);
    if (i == myBlocked.end()) {
        return;
    }
    GUIGlObject* o = (*i).second;
    myBlocked.erase(id);
    myMap[id] = o;
}


std::set<GUIGlID>
GUIGlObjectStorage::getAllIDs() const {
    AbstractMutex::ScopedLocker locker(myLock);
    std::set<GUIGlID> result;
    for (ObjectMap::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
        result.insert(it->first);
    }
    return result;
}


/****************************************************************************/

