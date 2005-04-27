//---------------------------------------------------------------------------//
//                        NIVissimSource.cpp -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include "NIVissimSource.h"

NIVissimSource::DictType NIVissimSource::myDict;

NIVissimSource::NIVissimSource(const std::string &id, const std::string &name,
                               const std::string &edgeid, double q,
                               bool exact, int vehicle_combination,
                               double beg, double end)
    : myID(id), myName(name), myEdgeID(edgeid), myQ(q), myExact(exact),
        myVehicleCombination(vehicle_combination),
        myTimeBeg(beg), myTimeEnd(end)
{
}


NIVissimSource::~NIVissimSource()
{
}


bool
NIVissimSource::dictionary(const std::string &id, const std::string &name,
                           const std::string &edgeid, double q, bool exact,
                           int vehicle_combination, double beg, double end)
{
    NIVissimSource *o = new NIVissimSource(id, name, edgeid, q, exact,
        vehicle_combination, beg, end);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimSource::dictionary(const std::string &id, NIVissimSource *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimSource *
NIVissimSource::dictionary(const std::string &id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimSource::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


