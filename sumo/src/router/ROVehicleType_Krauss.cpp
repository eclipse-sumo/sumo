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
// Revision 1.5  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.3  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
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
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variable definitions
 * ======================================================================= */
double ROVehicleType_Krauss::myDefault_A = 0.8;
double ROVehicleType_Krauss::myDefault_B = 4.5;
double ROVehicleType_Krauss::myDefault_EPS = 0.5;
double ROVehicleType_Krauss::myDefault_LENGTH = 5.0;
double ROVehicleType_Krauss::myDefault_MAXSPEED = 70;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROVehicleType_Krauss::ROVehicleType_Krauss()
    : ROVehicleType("KRAUSS_DEFAULT",  RGBColor(1, 1, 0), myDefault_LENGTH),
    myA(myDefault_A), myB(myDefault_B), myEps(myDefault_EPS),
    myMaxSpeed(myDefault_MAXSPEED)
{
}

ROVehicleType_Krauss::ROVehicleType_Krauss(const std::string &id,
                                           const RGBColor &col,
                                           double length,
                                           double a, double b,
                                           double eps,
                                           double maxSpeed)
    : ROVehicleType(id, col, length), myA(a), myB(b), myEps(eps),
    myMaxSpeed(maxSpeed)
{
}

ROVehicleType_Krauss::~ROVehicleType_Krauss()
{
}

std::ostream &
ROVehicleType_Krauss::xmlOut(std::ostream &os) const
{
	os << "<vtype id=\"" << myID << "\"";
	os << " accel=\"" << myA << "\"";
	os << " decel=\"" << myB << "\"";
	os << " sigma=\"" << myEps << "\"";
	os << " length=\"" << myLength << "\"";
	os << " maxspeed=\"" << myMaxSpeed << "\"";
    os << " color=\"" << myColor << "\"";
	os << "/>" << endl;
	return os;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


