//---------------------------------------------------------------------------//
//                        RORunningVehicle.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
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
#include "ROVehicleType.h"
#include "RORunningVehicle.h"
#include "RORouteDef.h"

using namespace std;

RORunningVehicle::RORunningVehicle(const std::string &id, RORouteDef *route,
                                   long time, ROVehicleType *type,
                                   const std::string &lane, float pos,
                                   float speed)
    : ROVehicle(id, route, time, type),
    _lane(lane), _pos(pos), _speed(speed)
{
}


RORunningVehicle::~RORunningVehicle()
{
}



void RORunningVehicle::xmlOut(std::ostream &os) const {
    os << "<vehicle id=\"" << _id << "\"";
    os << " type=\"" << _type->getID() << "\"";
	os << " route=\"" << _route->getID() << "\"";
	os << " depart=\"" << _depart << "\"";
    os << " lane=\"" << _lane << "\"";
    os << " pos=\"" << _pos << "\"";
    os << " speed=\"" << _speed << "\"";
	os << "/>" << endl;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RORunningVehicle.icc"
//#endif

// Local Variables:
// mode:C++
// End:


