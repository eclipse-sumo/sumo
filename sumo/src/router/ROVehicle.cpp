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
// Revision 1.4  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:22:38  dkrajzew
// debugging
//
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

#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <string>
#include <iostream>
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "ROVehicle.h"

using namespace std;

ROVehicle::ROVehicle(const std::string &id, RORouteDef *route, long depart,
                     ROVehicleType *type, long period)
	: _id(id), _type(type), _route(route), _depart(depart),
    _period(period)
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

bool
ROVehicle::reassertPeriodical()
{
    if(_period==-1) {
        return false;
    }
    // patch the name
    size_t idx = _id.rfind('_');
    if(idx!=string::npos) {
        try {
            int no = TplConvert<char>::_2int(_id.substr(idx+1).c_str());
            _id = _id.substr(0, idx+1) + toString<int>(no+1);
        } catch (NumberFormatException) {
            _id = _id + "_0";
        }
    } else {
        _id = _id + "_0";
    }
    // patch departure time
    _depart += _period;
    // patch the name of the route
    _route->patchID();
    // assign reemission
    return true;
}


bool
ROVehicle::periodical() const
{
    return _period!=-1;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROVehicle.icc"
//#endif

// Local Variables:
// mode:C++
// End:


