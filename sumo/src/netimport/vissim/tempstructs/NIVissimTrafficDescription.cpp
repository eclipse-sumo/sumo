//---------------------------------------------------------------------------//
//                        NIVissimTrafficDescription.cpp -  ccc
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
// Revision 1.4  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include "NIVissimVehicleClassVector.h"
#include "NIVissimTrafficDescription.h"

NIVissimTrafficDescription::DictType NIVissimTrafficDescription::myDict;

NIVissimTrafficDescription::NIVissimTrafficDescription(
    const std::string &id, const std::string &name,
    const NIVissimVehicleClassVector &vehicleTypes)
    : myID(id), myName(name), myVehicleTypes(vehicleTypes)
{
}


NIVissimTrafficDescription::~NIVissimTrafficDescription()
{
    for(NIVissimVehicleClassVector::iterator i=myVehicleTypes.begin(); i!=myVehicleTypes.end(); i++) {
        delete *i;
    }
    myVehicleTypes.clear();
}


bool
NIVissimTrafficDescription::dictionary(const std::string &id,
        const std::string &name, const NIVissimVehicleClassVector &vehicleTypes)
{
    NIVissimTrafficDescription *o = new NIVissimTrafficDescription(id, name, vehicleTypes);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimTrafficDescription::dictionary(const std::string &id, NIVissimTrafficDescription *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimTrafficDescription *
NIVissimTrafficDescription::dictionary(const std::string &id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimTrafficDescription::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}






/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimTrafficDescription.icc"
//#endif

// Local Variables:
// mode:C++
// End:


