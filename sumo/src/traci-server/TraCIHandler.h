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
#ifndef TRACIHANDLER_H
#define TRACIHANDLER_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef TRACI

#include "utils/xml/SUMOSAXHandler.h"

// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIHandler
 * XML-Handler that reads information from the sumo input files.
 * The class is currently used to determine the maximum number of vehicles 
 * before simulation start.
 */
namespace traci 
{
class TraCIHandler : public SUMOSAXHandler
{
public:
	/**
	 * @brief Constructor
	 * @param file the name of the parsed file
	 */
	TraCIHandler(const std::string& file = "") throw();

	/**
	 * @brief Destructor
	 */
	virtual ~TraCIHandler() throw();

	/**
	 * @brief Get the determined maxmimum number of vehicles that will be 
	 *			emitted during the simulation.
	 */
	int getTotalVehicleCount();

	/**
	 * @brief Reset number of counted vehicles to 0.
	 */
	void resetTotalVehicleCount();

protected:
	/**
	 * @brief Called on the opening of a xml tag.
	 * @param element id of the opened element
	 * @param attributes attributes of the opened element
	 */
	virtual void myStartElement(SumoXMLTag element, 
								const Attributes& attributes) throw(ProcessError);

	/**
	 * @brief Called on the closure of a xml tag.
	 * @param element id of the closed element
	 */
	virtual void myEndElement(SumoXMLTag element) throw(ProcessError);

private:
	int totalVehicleCount;

	int currentVehCount;

private:
	/**
     * Count each vehicle and all possibly emitted vehicles defined in each tag.
	 */
	void openVehicleTag(const Attributes& attributes);
};
}

#endif

#endif
