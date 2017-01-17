/****************************************************************************/
/// @file    NIVissimTrafficDescription.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimTrafficDescription_h
#define NIVissimTrafficDescription_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <string>
#include <map>
#include "NIVissimVehicleClassVector.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimTrafficDescription {
public:
    NIVissimTrafficDescription(const std::string& name,
                               const NIVissimVehicleClassVector& vehicleTypes);
    ~NIVissimTrafficDescription();
    static bool dictionary(int id, const std::string& name,
                           const NIVissimVehicleClassVector& vehicleTypes);
    static bool dictionary(int id, NIVissimTrafficDescription* o);
    static NIVissimTrafficDescription* dictionary(int id);
    static void clearDict();
    static SUMOReal meanSpeed(int id);
    SUMOReal meanSpeed() const;
private:
    std::string myName;
    NIVissimVehicleClassVector myVehicleTypes;
private:
    typedef std::map<int, NIVissimTrafficDescription*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

