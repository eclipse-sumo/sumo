//---------------------------------------------------------------------------//
//                        ROVehicleType_Krauss.cpp -
//  Description of parameters of a krauss-modeled vehicle
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
#include "ROVehicleType_Krauss.h"

using namespace std;

double ROVehicleType_Krauss::_defA = 0.8;
double ROVehicleType_Krauss::_defB = 4.5;
double ROVehicleType_Krauss::_defEPS = 0.5;
double ROVehicleType_Krauss::_defLENGTH = 5.0;
double ROVehicleType_Krauss::_defMAXSPEED = 70;

ROVehicleType_Krauss::ROVehicleType_Krauss()
    : ROVehicleType("SUMO_DEFAULT_TYPE"),
    _a(_defA), _b(_defB), _eps(_defEPS), _length(_defLENGTH),
    _maxSpeed(_defMAXSPEED)
{
}

ROVehicleType_Krauss::ROVehicleType_Krauss(const std::string &id,
                                           double a, double b,
                                           double eps,
                                           double length,
                                           double maxSpeed)
    : ROVehicleType(id), _a(a), _b(b), _eps(eps), _length(length),
    _maxSpeed(maxSpeed)
{
}

ROVehicleType_Krauss::~ROVehicleType_Krauss()
{
}

std::ostream &
ROVehicleType_Krauss::xmlOut(std::ostream &os)
{
	os << "<vtype id=\"" << _id << "\"";
	os << " accel=\"" << _a << "\"";
	os << " decel=\"" << _b << "\"";
	os << " sigma=\"" << _eps << "\"";
	os << " length=\"" << _length << "\"";
	os << " maxspeed=\"" << _maxSpeed << "\"";
	os << "/>" << endl;
	return os;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROVehicleType_Krauss.icc"
//#endif

// Local Variables:
// mode:C++
// End:


