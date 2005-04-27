//---------------------------------------------------------------------------//
//                        NIVissimVehicleType.cpp -  ccc
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
// Revision 1.6  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2003/10/27 10:51:55  dkrajzew
// edges speed setting implemented (only on an edges begin)
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


#include "NIVissimVehicleType.h"

NIVissimVehicleType::DictType NIVissimVehicleType::myDict;

NIVissimVehicleType::NIVissimVehicleType(int id,
        const std::string &name, const std::string &category, double length,
        const RGBColor &color, double amax, double dmax)
    : myID(id), myName(name), myCategory(category), myLength(length),
    myColor(color), myAMax(amax), myDMax(dmax)
{
}


NIVissimVehicleType::~NIVissimVehicleType()
{
}



bool
NIVissimVehicleType::dictionary(int id,
    const std::string &name, const std::string &category, double length,
    const RGBColor &color, double amax, double dmax)
{
    NIVissimVehicleType *o = new NIVissimVehicleType(id, name, category,
        length, color, amax, dmax);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimVehicleType::dictionary(int id, NIVissimVehicleType *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimVehicleType *
NIVissimVehicleType::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

void
NIVissimVehicleType::clearDict()
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


