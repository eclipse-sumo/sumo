/****************************************************************************/
/// @file    NIVissimTrafficDescription.cpp
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
#include <cassert>
#include "NIVissimVehicleClassVector.h"
#include "NIVissimTrafficDescription.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// member function definitions
// ===========================================================================
NIVissimTrafficDescription::DictType NIVissimTrafficDescription::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
NIVissimTrafficDescription::NIVissimTrafficDescription(
    int id, const std::string &name,
    const NIVissimVehicleClassVector &vehicleTypes)
        : myID(id), myName(name), myVehicleTypes(vehicleTypes)
{}


NIVissimTrafficDescription::~NIVissimTrafficDescription()
{
    for (NIVissimVehicleClassVector::iterator i=myVehicleTypes.begin(); i!=myVehicleTypes.end(); i++) {
        delete *i;
    }
    myVehicleTypes.clear();
}


bool
NIVissimTrafficDescription::dictionary(int id,
                                       const std::string &name,
                                       const NIVissimVehicleClassVector &vehicleTypes)
{
    NIVissimTrafficDescription *o = new NIVissimTrafficDescription(id, name, vehicleTypes);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimTrafficDescription::dictionary(int id, NIVissimTrafficDescription *o)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimTrafficDescription *
NIVissimTrafficDescription::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimTrafficDescription::clearDict()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}




SUMOReal
NIVissimTrafficDescription::meanSpeed(int id)
{
    NIVissimTrafficDescription *i = dictionary(id);
    assert(i!=0);
    return i->meanSpeed();
}


SUMOReal
NIVissimTrafficDescription::meanSpeed() const
{
    SUMOReal speed = 0;
    for (NIVissimVehicleClassVector::const_iterator i=myVehicleTypes.begin(); i!=myVehicleTypes.end(); i++) {
        speed += (*i)->getSpeed();
    }
    return speed / (SUMOReal) myVehicleTypes.size();
}



/****************************************************************************/

