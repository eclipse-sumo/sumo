#ifndef ROVehicleBuilder_h
#define ROVehicleBuilder_h
//---------------------------------------------------------------------------//
//                        ROVehicleBuilder.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
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
// $Log$
// Revision 1.5  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 12:39:02  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/SUMOTime.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class RORouteDef;
class ROVehicle;
class RORunningVehicle;
class RGBColor;
class ROVehicleType;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class ROVehicleBuilder {
public:
    ROVehicleBuilder();

    virtual ~ROVehicleBuilder();

    virtual ROVehicle *buildVehicle(const std::string &id, RORouteDef *route,
        unsigned int depart, ROVehicleType *type,
        const RGBColor &color, int period, int repNo);

    virtual RORunningVehicle *buildRunningVehicle(const std::string &id,
        RORouteDef *route, SUMOTime time, ROVehicleType *type,
        const std::string &lane, SUMOReal pos, SUMOReal speed,
        const RGBColor &col, int period, int repNo);


};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
