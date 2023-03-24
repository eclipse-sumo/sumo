/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIGlObjectStorage.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A storage for displayed objects via their numerical id
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utils/foxtools/fxheader.h>
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"


// ===========================================================================
// static variables (instances in this case)
// ===========================================================================
GUIGlObjectStorage GUIGlObjectStorage::gIDStorage;


// ===========================================================================
// method definitions
// ===========================================================================
GUIGlObjectStorage::GUIGlObjectStorage() :
    myNextID(1),
    myLock(true) {
    myObjects.push_back(nullptr);
}


GUIGlObjectStorage::~GUIGlObjectStorage() {}


GUIGlID
GUIGlObjectStorage::registerObject(GUIGlObject* object) {
    FXMutexLock locker(myLock);
    const GUIGlID id = myNextID;
    if (id == myObjects.size()) {
        myObjects.push_back(object);
    } else {
        myObjects[id] = object;
    }
    while (myNextID < myObjects.size() && myObjects[myNextID] != nullptr) {
        myNextID++;
    }
    return id;
}


void
GUIGlObjectStorage::changeName(GUIGlObject* object, const std::string& fullName) {
    FXMutexLock locker(myLock);
    myFullNameMap.erase(object->getFullName());
    myFullNameMap[fullName] = object;
}


GUIGlObject*
GUIGlObjectStorage::getObjectBlocking(GUIGlID id) const {
    FXMutexLock locker(myLock);
    if (id < myObjects.size() && myObjects[id] != nullptr) {
        GUIGlObject* const o = myObjects[id];
        o->setBlocked();
        return o;
    }
    return nullptr;
}


GUIGlObject*
GUIGlObjectStorage::getObjectBlocking(const std::string& fullName) const {
    FXMutexLock locker(myLock);
    auto findIt = myFullNameMap.find(fullName);
    if (findIt != myFullNameMap.end()) {
        GUIGlObject* const o = findIt->second;
        o->setBlocked();
        return o;
    }
    return nullptr;
}


bool
GUIGlObjectStorage::remove(GUIGlID id) {
    FXMutexLock locker(myLock);
    assert(id < myObjects.size() && myObjects[id] != nullptr);
    myFullNameMap.erase(myObjects[id]->getFullName());
    const bool wasBlocked = myObjects[id]->isBlocked();
    myObjects[id] = nullptr;
    if (id < myNextID) {
        myNextID = id;
    }
    return !wasBlocked;
}


void
GUIGlObjectStorage::clear() {
    FXMutexLock locker(myLock);
    myObjects.clear();
    myObjects.push_back(nullptr);
    myFullNameMap.clear();
    myNextID = 1;
}


void
GUIGlObjectStorage::unblockObject(GUIGlID id) {
    FXMutexLock locker(myLock);
    if (id < myObjects.size() && myObjects[id] != nullptr) {
        myObjects[id]->setBlocked(false);
    }
}


const std::vector<GUIGlObject*>&
GUIGlObjectStorage::getAllGLObjects() const {
    return myObjects;
}

/****************************************************************************/
