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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
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
#include "ODDistrict.h"
#include "ODDistrictCont.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/NamedObjectCont.h>
#include <od2trips/ODDistrictCont.h>
#include <od2trips/ODDistrictHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/RandHelper.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#include <utils/options/OptionsCont.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ODDistrictCont::ODDistrictCont() {}


ODDistrictCont::~ODDistrictCont() {}


std::string
ODDistrictCont::getRandomSourceFromDistrict(const std::string& name) const {
    ODDistrict* district = get(name);
    if (district == 0) {
        throw InvalidArgument("There is no district '" + name + "'.");
    }
    return district->getRandomSource();
}


std::string
ODDistrictCont::getRandomSinkFromDistrict(const std::string& name) const {
    ODDistrict* district = get(name);
    if (district == 0) {
        throw InvalidArgument("There is no district '" + name + "'.");
    }
    return district->getRandomSink();
}

void
ODDistrictCont::loadDistricts(std::string districtfile) {
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

/****************************************************************************/

