/***************************************************************************
                          GUIVehicleType.cpp  -  Base Class for Vehicle
                          parameters.
                             -------------------
    begin                : Thu, 17. Jun 2004
    copyright            : (C) 2004 by DLR http://www.dlr.de/vf
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 08:05:25  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/07/02 08:56:12  dkrajzew
// coloring of routes and vehicle types added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "GUIVehicleType.h"

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
GUIVehicleType::~GUIVehicleType()
{
}


GUIVehicleType::GUIVehicleType(const RGBColor &c,
                               string id, SUMOReal length, SUMOReal maxSpeed,
                               SUMOReal accel, SUMOReal decel, SUMOReal dawdle )
    : MSVehicleType(id, length, maxSpeed, accel, decel, dawdle), myColor(c)
{
}


const RGBColor &
GUIVehicleType::getColor() const
{
    return myColor;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
