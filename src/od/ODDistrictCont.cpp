/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ODDistrictCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
/// @version $Id$
///
// A container for districts
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/NamedObjectCont.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/RandHelper.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include "ODDistrict.h"
#include "ODDistrictHandler.h"
#include "ODDistrictCont.h"



// ===========================================================================
// method definitions
// ===========================================================================
ODDistrictCont::ODDistrictCont() {}


ODDistrictCont::~ODDistrictCont() {}


std::string
ODDistrictCont::getRandomSourceFromDistrict(const std::string& name) const {
    ODDistrict* district = get(name);
    if (district == nullptr) {
        throw InvalidArgument("There is no district '" + name + "'.");
    }
    return district->getRandomSource();
}


std::string
ODDistrictCont::getRandomSinkFromDistrict(const std::string& name) const {
    ODDistrict* district = get(name);
    if (district == nullptr) {
        throw InvalidArgument("There is no district '" + name + "'.");
    }
    return district->getRandomSink();
}


void
ODDistrictCont::loadDistricts(std::vector<std::string> files) {
    for (auto & districtfile : files) {
        if (!FileHelpers::isReadable(districtfile)) {
            throw ProcessError("Could not access network file '" + districtfile + "' to load.");
        }
        PROGRESS_BEGIN_MESSAGE("Loading districts from '" + districtfile + "'");
        // build the xml-parser and handler
        ODDistrictHandler handler(*this, districtfile);
        if (!XMLSubSys::runParser(handler, districtfile, true)) {
            PROGRESS_FAILED_MESSAGE();
        } else {
            PROGRESS_DONE_MESSAGE();
        }
    }
}


void
ODDistrictCont::makeDistricts(const std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string> > >& districts) {
    for (const auto & district : districts) {
        ODDistrict* current = new ODDistrict(district.first);
        const std::vector<std::string>& sources = district.second.first;
        for (const auto & source : sources) {
            current->addSource(source, 1.);
        }
        const std::vector<std::string>& sinks = district.second.second;
        for (const auto & sink : sinks) {
            current->addSink(sink, 1.);
        }
        add(current->getID(), current);
    }
}


/****************************************************************************/
