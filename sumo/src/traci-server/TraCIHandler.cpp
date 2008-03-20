/****************************************************************************/
/// @file    TraCIHandler.h
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @date    2008/03/20
/// @version $Id: 
///
/// XML-Handler used by TraCI to get information from sumo input files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// compiler pragmas
// ===========================================================================


// ===========================================================================
// included modules
// ===========================================================================
#include "TraCIHandler.h"

#ifdef TRACI

#include "utils/options/OptionsCont.h"
#include "utils/common/StdDefs.h"

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
namespace traci 
{
	
TraCIHandler::TraCIHandler(const std::string& file)
		:SUMOSAXHandler(file),
		totalVehicleCount(0),
		currentVehCount(0)
{}

TraCIHandler::~TraCIHandler()
{}

void 
TraCIHandler::myStartElement(SumoXMLTag element, const Attributes& attributes)
throw(ProcessError)
{
	if (element == SUMO_TAG_VEHICLE) {
		openVehicleTag(attributes);
	}
}

void 
TraCIHandler::myEndElement(SumoXMLTag element)
throw(ProcessError)
{
	if (element == SUMO_TAG_VEHICLE) {
		totalVehicleCount += currentVehCount;
	}
}

void
TraCIHandler::openVehicleTag(const Attributes& attributes)
{
	int repNo;
	int period;
	int depart;
	OptionsCont& optCont = OptionsCont::getOptions();
	int simStart = optCont.getInt("begin");
	int simEnd = optCont.getInt("end");
	
	// every found vehicle tag counts for one vehicle
	currentVehCount = 1;

	// read value for emit period and number (if any)
	try {
		repNo = getInt(attributes, SUMO_ATTR_REPNUMBER);
		period = getInt(attributes, SUMO_ATTR_PERIOD);
	} catch(...) {
		repNo = -1;
		period = -1;
	}

	// read depart time
	try {
		depart = getInt(attributes, SUMO_ATTR_DEPART);
	} catch (...) {
		// no depart time: error, don't count vehicle
		currentVehCount = 0;
		return;
	}

	// if vehicle departs before the sim begins...
	if (depart < simStart) {
		// decrease number of emitted vehicles accordingly
		if ( (repNo > 0) && (period > 0) ) {
			while ((depart < simStart) && (repNo > 0)) {
				depart += period;
				repNo--;
			}
		} 
		if (depart < simStart) {
			currentVehCount = 0;
			return;
		}
	}

	// don't count the vehicle, if it departs after the sim has ended
	if (depart >= simEnd) {
		currentVehCount = 0;
		return;
	}

	// don't count vehicles that are emitted after the sim ends
	while(((depart + (repNo * period)) >= simEnd) && (repNo > 0)) {
		repNo--;
	}

	// add number of vehicles, that will be emitted until sim end, to total count
	currentVehCount += repNo;
}

int 
TraCIHandler::getTotalVehicleCount()
{
	return totalVehicleCount;
}

void 
TraCIHandler::resetTotalVehicleCount()
{
	totalVehicleCount = 0;
}

}

#endif