#ifndef RORunningVehicle_h
#define RORunningVehicle_h
//---------------------------------------------------------------------------//
//                        RORunningVehicle.h -
//  A vehicle that has an initial speed (and possibly position)
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
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:35  dkrajzew
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
#include "ROVehicle.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORunningVehicle
 * A vehicle which when being emitted, already has an initial speed and
 * which also has a fixed position it shall be emitted at.
 */
class RORunningVehicle : public ROVehicle {
public:
    /// Constructor
    RORunningVehicle(const std::string &id, RORouteDef *route, long time,
        ROVehicleType *type, const std::string &lane, float pos,
        float speed, int period, int repNo);

    /// Destructor
    ~RORunningVehicle();

    /// Writes the vehicle's definition to the given stream
    void xmlOut(std::ostream &os) const;

private:
    /// The lane the vehicle shall depart from
    std::string _lane;

    /// The position on the lane the vehicle shall depart from
    float _pos;

    /// The initial speed of the vehicle
    float _speed;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "RORunningVehicle.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

