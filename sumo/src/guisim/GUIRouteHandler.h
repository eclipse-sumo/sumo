/****************************************************************************/
/// @file    GUIRouteHandler.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id$
///
// Parser and container for routes during their loading (gui-version)
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
#ifndef GUIRouteHandler_h
#define GUIRouteHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSRouteHandler.h>
#include <utils/common/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIRouteHandler
 * @brief Parser and container for routes during their loading (gui-version)
 */
class GUIRouteHandler : public MSRouteHandler
{
public:
    /// standard constructor
    GUIRouteHandler(const std::string &file, MSVehicleControl &vc,
                    bool addVehiclesDirectly, int incDUABase, int incDUAStage);

    /// standard destructor
    virtual ~GUIRouteHandler() throw();

protected:
    /** parses an occured vehicle type definition */
    virtual void addVehicleType(const SUMOSAXAttributes &attrs);

    /** adds the parsed vehicle type */
    virtual void addParsedVehicleType(const std::string &id,
                                      const SUMOReal length, const SUMOReal maxspeed, const SUMOReal bmax,
                                      const SUMOReal dmax, const SUMOReal sigma,
                                      SUMOReal tau, SUMOVehicleClass vclass,
                                      const RGBColor &c, SUMOReal prob);

    /// Ends route processing
    void closeRoute() throw(ProcessError);

    /// Starts route processing
    void openRoute(const SUMOSAXAttributes &attrs);

    /// Parses color from the attributes and stores it in myCurrentVehicleColor
    bool parseVehicleColor(const SUMOSAXAttributes &attrs) throw();

    /// Ends the processing of a vehicle
    bool closeVehicle() throw(ProcessError);

private:
    /// The currently parsed route's color
    RGBColor myColor;

    /// The vehicle's color
    RGBColor myCurrentVehicleColor;

private:
    /** invalid copy constructor */
    GUIRouteHandler(const GUIRouteHandler &s);

    /** invalid assignment operator */
    GUIRouteHandler &operator=(const GUIRouteHandler &s);

};


#endif

/****************************************************************************/

