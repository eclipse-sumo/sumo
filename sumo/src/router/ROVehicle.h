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
// Revision 1.3  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>


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
	ROVehicle(const std::string &id, RORouteDef *route, long depart,
        ROVehicleType *type, long period);

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
    long getDepartureTime() const;

    /** @brief Checks whether a next trip shall be generated
        Changes (adapts) the departure time and the vehicle id, too */
    bool reassertPeriodical();

    /** Returns the information whether more than a single vehicle with these 
        settings shall be emitted. */
    bool periodical() const;

protected:
    /// The name of the vehicle
	std::string _id;

    /// The type of the vehicle
	ROVehicleType *_type;

    /// The route the vehicle takes
	RORouteDef *_route;

    /// The time the vehicle shall be emitted at
	long _depart;

    /// The repetition period (-1 if only one vehicle shall be emitted)
    long _period;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROVehicle.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

