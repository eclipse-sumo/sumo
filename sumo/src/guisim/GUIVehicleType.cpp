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
// Revision 1.1  2004/07/02 08:56:12  dkrajzew
// coloring of routes and vehicle types added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "GUIVehicleType.h"


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
                               string id, double length, double maxSpeed,
                               double accel, double decel, double dawdle )
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
