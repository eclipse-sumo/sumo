/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    DistributionCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for distributions
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "DistributionCont.h"


// ===========================================================================
// static variable definitions
// ===========================================================================
DistributionCont::TypedDistDict DistributionCont::myDict;


// ===========================================================================
// method definitions
// ===========================================================================
bool
DistributionCont::dictionary(const std::string& type, const std::string& id,
                             Distribution* d) {
    TypedDistDict::iterator i = myDict.find(type);

    if (i == myDict.end()) {
        myDict[type][id] = d;
        return true;
    }
    DistDict& dict = (*i).second;
    DistDict::iterator j = dict.find(id);
    if (j == dict.end()) {
        myDict[type][id] = d;
        return true;
    }
    return false;
}


Distribution*
DistributionCont::dictionary(const std::string& type,
                             const std::string& id) {
    TypedDistDict::iterator i = myDict.find(type);
    if (i == myDict.end()) {
        return 0;
    }
    DistDict& dict = (*i).second;
    DistDict::iterator j = dict.find(id);
    if (j == dict.end()) {
        return 0;
    }
    return (*j).second;
}


void
DistributionCont::clear() {
    for (TypedDistDict::iterator i = myDict.begin(); i != myDict.end(); i++) {
        DistDict& dict = (*i).second;
        for (DistDict::iterator j = dict.begin(); j != dict.end(); j++) {
            delete(*j).second;
        }
    }
}


/****************************************************************************/
