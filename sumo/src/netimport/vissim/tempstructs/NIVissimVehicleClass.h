/****************************************************************************/
/// @file    NIVissimVehicleClass.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimVehicleClass_h
#define NIVissimVehicleClass_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimVehicleClass {
public:
    NIVissimVehicleClass(int type, SUMOReal percentage, int vwish);
    ~NIVissimVehicleClass();
    int getSpeed() const;
private:
    int myType;
    SUMOReal myPercentage;
    int myVWish;
};


#endif

/****************************************************************************/

