/****************************************************************************/
/// @file    GUIGlObjectStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A storage for of displayed objects via their numerical id
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <cassert>
#include "GUIGlObject.h"
#include <map>
#include "GUIGlObjectStorage.h"
#include <iostream>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUIGlObjectStorage::GUIGlObjectStorage() throw()
        : myAktID(1) {}


GUIGlObjectStorage::~GUIGlObjectStorage() throw() {}


void
GUIGlObjectStorage::registerObject(GUIGlObject *object) throw() {
    myLock.lock();
    object->setGlID(myAktID);
    myMap[myAktID++] = object;
    myLock.unlock();
}


GLuint
GUIGlObjectStorage::getUniqueID() throw() {
    myLock.lock();
    GLuint ret = myAktID++;
    myLock.unlock();
    return ret;
}


GUIGlObject *
GUIGlObjectStorage::getObjectBlocking(GLuint id) throw() {
    myLock.lock();
    ObjectMap::iterator i=myMap.find(id);
    if (i==myMap.end()) {
        i = myBlocked.find(id);
        if (i!=myBlocked.end()) {
            GUIGlObject *o = (*i).second;
            myLock.unlock();
            return o;
        }
        myLock.unlock();
        return 0;
    }
    GUIGlObject *o = (*i).second;
    myMap.erase(id);
    myBlocked[id] = o;
    myLock.unlock();
    return o;
}


bool
GUIGlObjectStorage::remove(GLuint id) throw() {
    myLock.lock();
    ObjectMap::iterator i=myMap.find(id);
    if (i==myMap.end()) {
        i = myBlocked.find(id);
        assert(i!=myBlocked.end());
        GUIGlObject *o = (*i).second;
        myBlocked.erase(id);
        my2Delete[id] = o;
        myLock.unlock();
        return false;
    } else {
        myMap.erase(id);
        myLock.unlock();
        return true;
    }
}


void
GUIGlObjectStorage::clear() throw() {
    myLock.lock();
    myMap.clear();
    myAktID = 0;
    myLock.unlock();
}


void
GUIGlObjectStorage::unblockObject(GLuint id) throw() {
    myLock.lock();
    ObjectMap::iterator i=myBlocked.find(id);
    if (i==myBlocked.end()) {
        myLock.unlock();
        return;
    }
    GUIGlObject *o = (*i).second;
    myBlocked.erase(id);
    myMap[id] = o;
    myLock.unlock();
}


/****************************************************************************/

