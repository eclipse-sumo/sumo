//---------------------------------------------------------------------------//
//                        ROVehicleType_ID.cpp -
//  A type which is only described by its id
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 07.12.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.1  2006/01/09 13:29:32  dkrajzew
// debugging vehicle color usage
//
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
#include "ROVehicleType.h"
#include "ROVehicleType_ID.h"

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
ROVehicleType_ID::ROVehicleType_ID(const std::string &id)
    : ROVehicleType(id, RGBColor(-1, -1, -1), 1)
{
}

ROVehicleType_ID::~ROVehicleType_ID()
{
}

std::ostream &
ROVehicleType_ID::xmlOut(std::ostream &os) const
{
	return os;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


