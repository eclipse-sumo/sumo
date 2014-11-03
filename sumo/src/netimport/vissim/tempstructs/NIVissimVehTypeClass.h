/****************************************************************************/
/// @file    NIVissimVehTypeClass.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimVehTypeClass_h
#define NIVissimVehTypeClass_h


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
#include <utils/common/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimVehTypeClass {
public:
    NIVissimVehTypeClass(int id, const std::string& name,
                         const RGBColor& color, std::vector<int>& types);
    ~NIVissimVehTypeClass();
    static bool dictionary(int id, const std::string& name,
                           const RGBColor& color, std::vector<int>& types);
    static bool dictionary(int id, NIVissimVehTypeClass* o);
    static NIVissimVehTypeClass* dictionary(int name);
    static void clearDict();
    SUMOReal meanSpeed() const;
private:
    int myID;
    std::string myName;
    RGBColor myColor;
    std::vector<int> myTypes;
private:
    typedef std::map<int, NIVissimVehTypeClass*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

