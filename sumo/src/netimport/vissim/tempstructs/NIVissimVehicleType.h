/****************************************************************************/
/// @file    NIVissimVehicleType.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
    NIVissimVehicleType(const std::string& name,
                        const std::string& category, const RGBColor& color);
    ~NIVissimVehicleType();
    static bool dictionary(int id, const std::string& name,
                           const std::string& category, const RGBColor& color);
    static bool dictionary(int id, NIVissimVehicleType* o);
    static NIVissimVehicleType* dictionary(int id);
    static void clearDict();

private:
    std::string myName;
    std::string myCategory;
    RGBColor myColor;
private:
    typedef std::map<int, NIVissimVehicleType*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

