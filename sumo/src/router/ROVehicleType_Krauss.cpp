/****************************************************************************/
/// @file    ROVehicleType_Krauss.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Description of parameters of a krauss-modeled vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"
#include <utils/xml/XMLSnippletStorage.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ROVehicleType_Krauss::ROVehicleType_Krauss(const std::string &id,
        const RGBColor &col, SUMOReal length, SUMOVehicleClass vclass,
        SUMOReal a, SUMOReal b, SUMOReal eps, SUMOReal maxSpeed,
        SUMOReal tau)
        : ROVehicleType(id, col, length, vclass), myA(a), myB(b), myEps(eps),
        myMaxSpeed(maxSpeed), myTau(tau)
{}


ROVehicleType_Krauss::~ROVehicleType_Krauss()
{}


std::ostream &
ROVehicleType_Krauss::xmlOut(std::ostream &os) const
{
    os << "   <vtype model=\"SUMO_KRAUSS\""
    << " id=\"" << myID << "\""
    << " accel=\"" << myA << "\""
    << " decel=\"" << myB << "\""
    << " sigma=\"" << myEps << "\""
    << " length=\"" << myLength << "\""
    << " maxspeed=\"" << myMaxSpeed << "\""
    << " tau=\"" << myTau << "\"";
    if (myColor!=RGBColor(-1,-1,-1)) {
        os << " color=\"" << myColor << "\"";
    }
    if (myClass!=SVC_UNKNOWN) {
        os << " class=\"" << getVehicleClassName(myClass) << "\"";
    }
    if (myEmbeddedParams!=0) {
        os << ">" << endl;
        myEmbeddedParams->flush(os, 2);
        os << "   </vtype>" << endl;
    } else {
        os << "/>" << endl;
    }
    return os;
}



/****************************************************************************/

