/****************************************************************************/
/// @file    ROVehicleBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
///
//
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
#ifndef ROVehicleBuilder_h
#define ROVehicleBuilder_h
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
#include <utils/common/SUMOTime.h>

// ===========================================================================
// class declarations
// ===========================================================================
class RORouteDef;
class ROVehicle;
class RORunningVehicle;
class RGBColor;
class ROVehicleType;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class ROVehicleBuilder
{
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


#endif

/****************************************************************************/

