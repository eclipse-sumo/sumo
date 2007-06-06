/****************************************************************************/
/// @file    SUMOBaseRouteHandler.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 12.12.2005
/// @version $Id: SUMOBaseRouteHandler.h 3793 2007-04-12 15:10:18 +0200 (Do, 12 Apr 2007) dkrajzew $
///
// A base class for parsing vehicles
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
#ifndef SUMOBaseRouteHandler_h
#define SUMOBaseRouteHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/SUMOTime.h>
#include <utils/gfx/RGBColor.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOBaseRouteHandler
 * Base class for vehicle parsing.
 */
class SUMOBaseRouteHandler
{
public:
    /// Constructor
    SUMOBaseRouteHandler();

    /// Destructor
    virtual ~SUMOBaseRouteHandler();

protected:
    //{ parse helper (reporting errors)
    /// Retrieves a color definition
    RGBColor parseColor(SUMOSAXHandler &helper, const Attributes &attrs,
                        const std::string &type, const std::string &id);

    /// Retrieves the class
    SUMOVehicleClass parseVehicleClass(SUMOSAXHandler &helper,
                                       const Attributes &attrs, const std::string &type,
                                       const std::string &id);

    /// Parses and returns the departure time of the current vehicle
    SUMOTime getVehicleDepartureTime(SUMOSAXHandler &helper,
                                     const Attributes &attrs, const std::string &id);

    /// Parses information from the vehicle-tag
    bool openVehicle(SUMOSAXHandler &helper, const Attributes &attrs,
                     bool wantsVehicleColor);

    /// Closes the processing of a vehicle
    virtual void closeVehicle();
    //}


protected:
    //{ variables used during parsing of vehicles
    /// the id of the current vehicle
    std::string myActiveVehicleID;

    /// Information about how often vehicles like the currently read shall be emitted
    int myRepNumber;

    /// Information about the dureation of the period between such emissions
    int myRepOffset;

    /// name of the currently read vehicle's type
    std::string myCurrentVType;

    /// name of the currently read vehicle's route
    std::string myCurrentRouteName;

    /// The currently read vehicle's depart
    SUMOTime myCurrentDepart;

    /// infomration whether the route is read in vehicle-embedded mode
    bool myAmInEmbeddedMode;

    /// The vehicle's color
    RGBColor myCurrentVehicleColor;
    //}

    /// information whether an error occured
    bool myHadErrors;

};


#endif

/****************************************************************************/

