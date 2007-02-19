/****************************************************************************/
/// @file    GUIVehicleType.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id$
///
// parameters.
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

#include "GUIVehicleType.h"

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
GUIVehicleType::~GUIVehicleType()
{}


GUIVehicleType::GUIVehicleType(const RGBColor &c, const string &id,
                               SUMOReal length, SUMOReal maxSpeed,
                               SUMOReal accel, SUMOReal decel,
                               SUMOReal dawdle, SUMOVehicleClass vclass)
        : MSVehicleType(id, length, maxSpeed, accel, decel, dawdle, vclass),
        myColor(c)
{}


const RGBColor &
GUIVehicleType::getColor() const
{
    return myColor;
}



/****************************************************************************/

