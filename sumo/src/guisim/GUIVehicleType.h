#ifndef GUIVehicleType_H
#define GUIVehicleType_H
/***************************************************************************
                          GUIVehicleType.h  -  Base Class for Vehicle
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
// $Log$
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

#include <map>
#include <string>
#include <microsim/MSVehicleType.h>
#include <utils/gfx/RGBColor.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class GUIVehicleType : public MSVehicleType
{
public:
    GUIVehicleType(const RGBColor &c,
        std::string id, double length, double maxSpeed,
        double accel = 0.8, double decel = 4.5, double dawdle = 0.5 );

    /// Destructor.
    ~GUIVehicleType();

    const RGBColor &getColor() const;

private:
    RGBColor myColor;

    /// Default constructor.
    GUIVehicleType();

    /// Copy constructor.
    GUIVehicleType( const GUIVehicleType& );

    /// Assignment operator.
    GUIVehicleType& operator=( const GUIVehicleType& );

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
