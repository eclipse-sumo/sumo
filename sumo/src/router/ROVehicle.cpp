//---------------------------------------------------------------------------//
//                        ROVehicle.cpp -
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
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "ROVehicle.h"

using namespace std;

ROVehicle::ROVehicle(const std::string &id, RORouteDef *route, long depart,
                     ROVehicleType *type)
	: _id(id), _type(type), _route(route), _depart(depart)
{
}

ROVehicle::~ROVehicle()
{
}


RORouteDef *
ROVehicle::getRoute() const
{
    return _route;
}


ROVehicleType *
ROVehicle::getType() const
{
    return _type;
}


void ROVehicle::xmlOut(std::ostream &os) const
{
    os << "<vehicle id=\"" << _id << "\"";
    os << " type=\"" << _type->getID() << "\"";
	os << " route=\"" << _route->getID() << "\"";
	os << " depart=\"" << _depart << "\"";
	os << "/>" << endl;
}

std::string
ROVehicle::getID() const
{
    return _id;
}

long
ROVehicle::getDepartureTime() const
{
    return _depart;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROVehicle.icc"
//#endif

// Local Variables:
// mode:C++
// End:


