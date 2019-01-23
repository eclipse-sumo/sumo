/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ODDistrict.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
/// @version $Id$
///
// A district (origin/destination)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <string>
#include <utility>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Named.h>
#include <utils/common/MsgHandler.h>
#include "ODDistrict.h"



// ===========================================================================
// method definitions
// ===========================================================================
ODDistrict::ODDistrict(const std::string& id)
    : Named(id) {}


ODDistrict::~ODDistrict() {}


void
ODDistrict::addSource(const std::string& id, double weight) {
    mySources.add(id, weight);
}


void
ODDistrict::addSink(const std::string& id, double weight) {
    mySinks.add(id, weight);
}


std::string
ODDistrict::getRandomSource() const {
    return mySources.get();
}


std::string
ODDistrict::getRandomSink() const {
    return mySinks.get();
}


int
ODDistrict::sinkNumber() const {
    return (int) mySinks.getVals().size();
}


int
ODDistrict::sourceNumber() const {
    return (int) mySources.getVals().size();
}



/****************************************************************************/

