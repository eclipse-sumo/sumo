/****************************************************************************/
/// @file    NIVissimVehTypeClass.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// -------------------
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
#ifndef NIVissimVehTypeClass_h
#define NIVissimVehTypeClass_h
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
#include <map>
#include <utils/gfx/RGBColor.h>
#include <utils/common/IntVector.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimVehTypeClass
{
public:
    NIVissimVehTypeClass(int id, const std::string &name,
                         const RGBColor &color, IntVector &types);
    ~NIVissimVehTypeClass();
    static bool dictionary(int id, const std::string &name,
                           const RGBColor &color, IntVector &types);
    static bool dictionary(int id, NIVissimVehTypeClass *o);
    static NIVissimVehTypeClass *dictionary(int name);
    static void clearDict();
    SUMOReal meanSpeed() const;
private:
    int myID;
    std::string myName;
    RGBColor myColor;
    IntVector myTypes;
private:
    typedef std::map<int, NIVissimVehTypeClass*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

