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
// Revision 1.10  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 05:53:29  dksumo
// fastened up the output of warnings and messages
//
// Revision 1.1  2004/10/22 12:50:25  dksumo
// initial checkin into an internal, standalone SUMO CVS
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
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/common/MsgHandler.h>
#include <string>
#include <iostream>
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "ROVehicle.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROVehicle::ROVehicle(ROVehicleBuilder &vb,
                     const std::string &id, RORouteDef *route,
                     unsigned int depart, ROVehicleType *type,
                     const RGBColor &color,
                     int period, int repNo)
    : _id(id), myColor(color), _type(type), _route(route), _depart(depart),
    _period(period), _repNo(repNo)
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
    os << " color=\"" << myColor << "\"";
    if(_period!=-1) {
        os << " period=\"" << _period << "\"";
        os << " repno=\"" << _repNo << "\"";
    }
    os << "/>" << endl;
}

std::string
ROVehicle::getID() const
{
    return _id;
}

SUMOTime
ROVehicle::getDepartureTime() const
{
    return _depart;
}


bool
ROVehicle::periodical() const
{
    return _period!=-1;
}


void
ROVehicle::saveTypeAndSelf(std::ostream &os,
                           ROVehicleType &defType) const
{
    ROVehicleType &type = getTypeForSaving(defType);
    if(!type.isSaved()) {
        os << "   ";
        type.xmlOut(os);
        type.markSaved();
    }
    os << "   ";
    xmlOut(os);
    os << endl;
}


void
ROVehicle::saveTypeAndSelf(std::ostream &os, std::ostream &altos,
                           ROVehicleType &defType) const
{
    ROVehicleType &type = getTypeForSaving(defType);
    if(!type.isSaved()) {
        os << "   ";
        type.xmlOut(os);
        altos << "   ";
        type.xmlOut(altos);
        type.markSaved();
    }
    os << "   ";
    xmlOut(os);
    os << endl;
    altos << "   ";
    xmlOut(altos);
    altos << endl;
}


ROVehicleType &
ROVehicle::getTypeForSaving(ROVehicleType &defType) const
{
    if(_type==0) {
//        type = _vehicleTypes.getDefault();
        WRITE_WARNING(string("The vehicle '") + getID()+ string("' has no valid type; Using default."));
        return defType;
    } else {
        return *_type;
    }
}


ROVehicle *
ROVehicle::copy(ROVehicleBuilder &vb,
                const std::string &id, unsigned int depTime,
                RORouteDef *newRoute)
{
    return new ROVehicle(vb, id, newRoute, depTime, _type, myColor,
        _period, _repNo);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


