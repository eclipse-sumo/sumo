#ifndef ROVehicle_h
#define ROVehicle_h
//---------------------------------------------------------------------------//
//                        ROVehicle.h -
//  A single vehicle
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.11  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.8  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.7  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.6  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.5  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
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
#include <iostream>
#include <utils/gfx/RGBColor.h>
#include "ROVehicleBuilder.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROVehicleType;
class RORouteDef;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehicle
 * A single vehicle holding information about his type, departure time and
 * the route he takes.
 */
class ROVehicle {
public:
    /// Constructor
    ROVehicle(ROVehicleBuilder &vb,
        const std::string &id, RORouteDef *route, unsigned int depart,
        ROVehicleType *type, const RGBColor &color, int period, int repNo);

    /// Destructor
    virtual ~ROVehicle();

    /// Returns the route the vehicle takes
    RORouteDef *getRoute() const;

    /// Returns the type of the vehicle
    ROVehicleType *getType() const;

    /// Saves information wbout the vehicle (in SUMO-XML)
    virtual void xmlOut(std::ostream &os) const;

    /// Returns the id of the vehicle
    std::string getID() const;

    /// Returns the time the vehicle starts his route
    SUMOTime getDepartureTime() const;

    /** Returns the information whether more than a single vehicle with these
        settings shall be emitted. */
    bool periodical() const;

    /** @brief Saves the vehicle type if it was not saved before and the vehicle itself
        Use this method polymorph if no route alternatives shall be generated */
    void saveTypeAndSelf(std::ostream &os, ROVehicleType &defType) const;

    /** @brief Saves the vehicle type if it was not saved before and the vehicle itself
        Use this method polymorph if route alternatives shall be written, too */
    void saveTypeAndSelf(std::ostream &os, std::ostream &altos,
        ROVehicleType &defType) const;

    /// Returns a copy of the vehicle using a new id, departure time and route
    virtual ROVehicle *copy(ROVehicleBuilder &vb,
        const std::string &id, unsigned int depTime, RORouteDef *newRoute);

protected:
    /** @brief Returns the type of the vehicle
        Returns the default vehicle type if no vehicle type was set */
    ROVehicleType &getTypeForSaving(ROVehicleType &defType) const;


protected:
    /// The name of the vehicle
    std::string _id;

    /// The color of the vehicle
    RGBColor myColor;

    /// The type of the vehicle
    ROVehicleType *_type;

    /// The route the vehicle takes
    RORouteDef *_route;

    /// The time the vehicle shall be emitted at
    unsigned int _depart;

    /// The repetition period (-1 if only one vehicle shall be emitted)
    int _period;

    /// The number of times such vehicles shall be emitted
    int _repNo;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

