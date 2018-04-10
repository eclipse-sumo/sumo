/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGlObject_AbstractAdd.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// Base class for additional objects (detectors etc.)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIGlObject_AbstractAdd.h"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <utils/gui/div/GLHelper.h>


// ===========================================================================
// static member definitions
// ===========================================================================
std::map<std::string, GUIGlObject_AbstractAdd*> GUIGlObject_AbstractAdd::myObjects;
std::vector<GUIGlObject_AbstractAdd*> GUIGlObject_AbstractAdd::myObjectList;


// ===========================================================================
// method definitions
// ===========================================================================
GUIGlObject_AbstractAdd::GUIGlObject_AbstractAdd(const std::string& prefix, GUIGlObjectType type, const std::string& id) :
    GUIGlObject(prefix, type, id) {
    myObjects[getFullName()] = this;
    myObjectList.push_back(this);
}


GUIGlObject_AbstractAdd::~GUIGlObject_AbstractAdd() {}


void
GUIGlObject_AbstractAdd::clearDictionary() {
    std::map<std::string, GUIGlObject_AbstractAdd*>::iterator i;
    for (i = myObjects.begin(); i != myObjects.end(); i++) {
//!!!        delete (*i).second;
    }
    myObjects.clear();
    myObjectList.clear();
}


GUIGlObject_AbstractAdd*
GUIGlObject_AbstractAdd::get(const std::string& name) {
    std::map<std::string, GUIGlObject_AbstractAdd*>::iterator i = myObjects.find(name);
    if (i == myObjects.end()) {
        return 0;
    }
    return (*i).second;
}


void
GUIGlObject_AbstractAdd::remove(GUIGlObject_AbstractAdd* o) {
    myObjects.erase(o->getFullName());
    myObjectList.erase(std::remove(myObjectList.begin(), myObjectList.end(), o), myObjectList.end());
}


const std::vector<GUIGlObject_AbstractAdd*>&
GUIGlObject_AbstractAdd::getObjectList() {
    return myObjectList;
}


std::vector<GUIGlID>
GUIGlObject_AbstractAdd::getIDList(int typeFilter) {
    std::vector<GUIGlID> ret;
    for (std::vector<GUIGlObject_AbstractAdd*>::iterator i = myObjectList.begin(); i != myObjectList.end(); ++i) {
        if (((*i)->getType() & typeFilter) != 0) {
            ret.push_back((*i)->getGlID());
        }
    }
    return ret;
}

/****************************************************************************/

