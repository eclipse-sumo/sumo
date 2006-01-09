#ifndef SUMOBaseRouteHandler_h
#define SUMOBaseRouteHandler_h
/***************************************************************************
                          SUMOBaseRouteHandler.h
                          A base class for parsing vehicles
                             -------------------
    project              : SUMO
    begin                : Mon, 12.12.2005
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.1  2006/01/09 13:38:36  dkrajzew
// debugging vehicle color usage
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/SUMOTime.h>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SUMOBaseRouteHandler
 * Base class for vehicle parsing.
 */
class SUMOBaseRouteHandler
{
public:
    SUMOBaseRouteHandler();
    ~SUMOBaseRouteHandler();

protected:
    //{ parse helper (reporting errors)
    /// Retrieves a SUMOReal from the attributes and reports errors, if any occure
    SUMOReal getFloatReporting(SUMOSAXHandler &helper, const Attributes &attrs,
        AttrEnum attr, const std::string &id, const std::string &name);

    /// Retrieves a color definition
    RGBColor parseColor(SUMOSAXHandler &helper, const Attributes &attrs,
        const std::string &type, const std::string &id);

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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
