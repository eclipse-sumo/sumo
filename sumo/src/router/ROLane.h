#ifndef ROLane_h
#define ROLane_h
//---------------------------------------------------------------------------//
//                        ROLane.h -
//  A single lane the router may use
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
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/common/Named.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class ROLane : public Named {
private:
    double _length;
    double _maxSpeed;
public:
    ROLane(const std::string &id, double length, double maxSpeed);
    ~ROLane();
//    double getWeight() const;
    double getLength() const;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROLane.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

