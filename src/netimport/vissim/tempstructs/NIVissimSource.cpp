/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimSource.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <string>
#include <map>
#include "NIVissimSource.h"

NIVissimSource::DictType NIVissimSource::myDict;

NIVissimSource::NIVissimSource(const std::string& id, const std::string& name,
                               const std::string& edgeid)
    : myID(id), myName(name), myEdgeID(edgeid) {}


NIVissimSource::~NIVissimSource() {}


bool
NIVissimSource::dictionary(const std::string& id, const std::string& name,
                           const std::string& edgeid) {
    NIVissimSource* o = new NIVissimSource(id, name, edgeid);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimSource::dictionary(const std::string& id, NIVissimSource* o) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimSource*
NIVissimSource::dictionary(const std::string& id) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        return nullptr;
    }
    return (*i).second;
}


void
NIVissimSource::clearDict() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}



/****************************************************************************/

