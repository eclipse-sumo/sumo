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
// Revision 1.11  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:53:07  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.6  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.5  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:35  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
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
#include <iostream>
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "RORunningVehicle.h"
#include "RORouteDef.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORunningVehicle::RORunningVehicle(ROVehicleBuilder &vb,
                                   const std::string &id, RORouteDef *route,
                                   SUMOTime time, ROVehicleType *type,
                                   const std::string &lane, SUMOReal pos,
                                   SUMOReal speed,
                                   const RGBColor &col,
                                   int period, int repNo)
    : ROVehicle(vb, id, route, time, type, col, period, repNo),
    _lane(lane), _pos(pos), _speed(speed)
{
}


RORunningVehicle::~RORunningVehicle()
{
}



void
RORunningVehicle::xmlOut(std::ostream &os) const
{
    os << "<vehicle id=\"" << _id << "\"";
    os << " type=\"" << _type->getID() << "\"";
    os << " route=\"" << _route->getID() << "\"";
    os << " depart=\"" << _depart << "\"";
    os << " lane=\"" << _lane << "\"";
    os << " pos=\"" << _pos << "\"";
    os << " speed=\"" << _speed << "\"";
    os << "/>" << endl;
}


ROVehicle *
RORunningVehicle::copy(ROVehicleBuilder &vb,
                       const std::string &id, unsigned int depTime,
                       RORouteDef *newRoute)
{
    return new RORunningVehicle(vb, id, newRoute, depTime, _type, _lane, _pos,
        _speed, myColor, _period, _repNo);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


