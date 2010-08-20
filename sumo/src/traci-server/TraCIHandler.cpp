/****************************************************************************/
/// @file    TraCIHandler.h
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @date    2008/03/20
/// @version $Id$
///
/// XML-Handler used by TraCI to get information from sumo input files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include "TraCIHandler.h"

#ifndef NO_TRACI

#include "utils/options/OptionsCont.h"
#include "utils/common/StdDefs.h"
#include "utils/common/SUMOTime.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
namespace traci {

TraCIHandler::TraCIHandler(const std::string& file)
throw()
        :SUMOSAXHandler(file),
        totalVehicleCount(0),
        currentVehCount(0) {}

TraCIHandler::~TraCIHandler()
throw() {}

void
TraCIHandler::myStartElement(SumoXMLTag element, const SUMOSAXAttributes& attributes)
throw(ProcessError) {
    if (element == SUMO_TAG_VEHICLE) {
        openVehicleTag(attributes);
    }
}

void
TraCIHandler::myEndElement(SumoXMLTag element)
throw(ProcessError) {
    if (element == SUMO_TAG_VEHICLE) {
        totalVehicleCount += currentVehCount;
    }
}

void
TraCIHandler::openVehicleTag(const SUMOSAXAttributes& attributes) {
    int repNo;
    int period;
    int depart;
    OptionsCont& optCont = OptionsCont::getOptions();
    SUMOTime simStart = string2time(optCont.getString("begin"));
    SUMOTime simEnd = string2time(optCont.getString("end"));
    if (simEnd<0) {
        simEnd = SUMOTime_MAX;
    }
    // every found vehicle tag counts for one vehicle
    currentVehCount = 1;
    // read value for emit period and number (if any)
    try {
        repNo = attributes.getInt(SUMO_ATTR_REPNUMBER);
        period = attributes.getInt(SUMO_ATTR_PERIOD);
    } catch (...) {
        repNo = 0;
        period = 0;
    }

    // read depart time
    try {
        depart = attributes.getInt(SUMO_ATTR_DEPART);
    } catch (...) {
        // no depart time: error, don't count vehicle
        currentVehCount = 0;
//		std::cerr << "no depart time, vehicle = 0" << std::endl;
        return;
    }

    while ((depart < simStart) && (repNo >= 0)) {
        depart += period;
        repNo--;
//		std::cerr << "removing 1 vehicle from repno (depart before sim start)" << std::endl;
    }

    // don't count vehicles that depart / are emitted after the sim ends
    while (((depart + (repNo * period)) > simEnd) && (repNo >= 0)) {
        repNo--;
//		std::cerr << "removing 1 vehicle from repno (depart after sim end)" << std::endl;

    }

    // add number of vehicles, that will be emitted until sim end, to total count
    currentVehCount += repNo;
//	std::cerr << "result: " << currentVehCount << " vehicles" << std::endl;
}

int
TraCIHandler::getTotalVehicleCount() {
    return totalVehicleCount;
}

void
TraCIHandler::resetTotalVehicleCount() {
    totalVehicleCount = 0;
}

}

#endif
