/****************************************************************************/
/// @file    NIVissimVehicleType.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimVehicleType_h
#define NIVissimVehicleType_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <utils/common/RGBColor.h>
#include <string>
#include <map>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimVehicleType {
public:
    NIVissimVehicleType(int id, const std::string& name,
                        const std::string& category, SUMOReal length, const RGBColor& color,
                        SUMOReal amax, SUMOReal dmax);
    ~NIVissimVehicleType();
    static bool dictionary(int id, const std::string& name,
                           const std::string& category, SUMOReal length, const RGBColor& color,
                           SUMOReal amax, SUMOReal dmax);
    static bool dictionary(int id, NIVissimVehicleType* o);
    static NIVissimVehicleType* dictionary(int id);
    static void clearDict();

private:
    int myID;
    std::string myName;
    std::string myCategory;
    SUMOReal myLength;
    RGBColor myColor;
    SUMOReal myAMax, myDMax;
private:
    typedef std::map<int, NIVissimVehicleType*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

