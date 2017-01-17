/****************************************************************************/
/// @file    NBDistrictCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A container for districts
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBDistrict.h"
#include "NBDistrictCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBDistrictCont::NBDistrictCont() {}


NBDistrictCont::~NBDistrictCont() {
    for (DistrictCont::iterator i = myDistricts.begin(); i != myDistricts.end(); i++) {
        delete((*i).second);
    }
    myDistricts.clear();
}


bool
NBDistrictCont::insert(NBDistrict* const district) {
    DistrictCont::const_iterator i = myDistricts.find(district->getID());
    if (i != myDistricts.end()) {
        return false;
    }
    myDistricts.insert(DistrictCont::value_type(district->getID(), district));
    return true;
}


NBDistrict*
NBDistrictCont::retrieve(const std::string& id) const {
    DistrictCont::const_iterator i = myDistricts.find(id);
    if (i == myDistricts.end()) {
        return 0;
    }
    return (*i).second;
}


int
NBDistrictCont::size() const {
    return (int)myDistricts.size();
}


bool
NBDistrictCont::addSource(const std::string& dist, NBEdge* const source,
                          SUMOReal weight) {
    NBDistrict* o = retrieve(dist);
    if (o == 0) {
        return false;
    }
    return o->addSource(source, weight);
}


bool
NBDistrictCont::addSink(const std::string& dist, NBEdge* const destination,
                        SUMOReal weight) {
    NBDistrict* o = retrieve(dist);
    if (o == 0) {
        return false;
    }
    return o->addSink(destination, weight);
}


void
NBDistrictCont::removeFromSinksAndSources(NBEdge* const e) {
    for (DistrictCont::iterator i = myDistricts.begin(); i != myDistricts.end(); i++) {
        (*i).second->removeFromSinksAndSources(e);
    }
}



/****************************************************************************/

