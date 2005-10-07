#ifndef RORDGenerator_Random_h
#define RORDGenerator_Random_h
//---------------------------------------------------------------------------//
//                        RORDGenerator_Random.h -
//  A "trip loader" for random trip generation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 9. Apr 2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.3  2003/08/18 12:44:54  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.2  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.1  2003/04/09 15:41:19  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
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
#include <utils/router/IDSupplier.h>
#include <utils/gfx/RGBColor.h>
#include "ROAbstractRouteDefLoader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORDGenerator_Random
 * Base class for loaders of route which do have a certain format. XML-reading
 * loaders are not derived directly, but use the derived ROTypedXMLRoutesLoader
 * class as their upper class.
 */
class RORDGenerator_Random :
    public ROAbstractRouteDefLoader {
public:
    /// Constructor
    RORDGenerator_Random(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end, const std::string &file="");

    /// Destructor
    ~RORDGenerator_Random();

    /// Closes the reading of the routes
    void closeReading();

    /// Returns the name of the route type
    std::string getDataName() const;

    /// Returns the information whether no routes are available from this loader anymore
    bool ended() const;

    /// Returns the time the current (last read) route starts at
    SUMOTime getCurrentTimeStep() const;

    /// reader dependent initialisation
    virtual bool init(OptionsCont &options);

protected:
    /** @brief Reads the until the specified time is reached
        Do read the comments on ROAbstractRouteDefLoader::myReadRoutesAtLeastUntil
        for the modalities! */
    bool myReadRoutesAtLeastUntil(SUMOTime time);

private:
    /** @brief The number of cars to emit per second
        Values lower than one are allowed, too */
    SUMOReal myWishedPerSecond;

    /** The counter for vehicle emission */
    SUMOReal myCurrentProgress;

    /** The supplier for ids */
    IDSupplier myIDSupplier;

    /// The color random routes shall have
    RGBColor myColor;

    /// The (pseudo) information about the current time
    SUMOTime myCurrentTime;

    /// The information whether a new route was read
    bool myReadNewRoute;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

