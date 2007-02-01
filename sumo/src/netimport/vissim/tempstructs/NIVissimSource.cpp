/****************************************************************************/
/// @file    NIVissimSource.cpp
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
#include "NIVissimSource.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

NIVissimSource::DictType NIVissimSource::myDict;

NIVissimSource::NIVissimSource(const std::string &id, const std::string &name,
                               const std::string &edgeid, SUMOReal q,
                               bool exact, int vehicle_combination,
                               SUMOReal beg, SUMOReal end)
        : myID(id), myName(name), myEdgeID(edgeid), myQ(q), myExact(exact),
        myVehicleCombination(vehicle_combination),
        myTimeBeg(beg), myTimeEnd(end)
{}


NIVissimSource::~NIVissimSource()
{}


bool
NIVissimSource::dictionary(const std::string &id, const std::string &name,
                           const std::string &edgeid, SUMOReal q, bool exact,
                           int vehicle_combination, SUMOReal beg, SUMOReal end)
{
    NIVissimSource *o = new NIVissimSource(id, name, edgeid, q, exact,
                                           vehicle_combination, beg, end);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimSource::dictionary(const std::string &id, NIVissimSource *o)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimSource *
NIVissimSource::dictionary(const std::string &id)
{
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimSource::clearDict()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}



/****************************************************************************/

