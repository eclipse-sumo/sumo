//---------------------------------------------------------------------------//
//                        NBDistribution.cpp -
//  The base class for statistical distribution descriptions
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
// Revision 1.5  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.4  2003/06/05 11:43:34  dkrajzew
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

#include "NBDistribution.h"


/* =========================================================================
 * static member variables
 * ======================================================================= */
NBDistribution::TypedDistDict NBDistribution::myDict;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
NBDistribution::dictionary(const std::string &type, const std::string &id,
                           Distribution *d)
{
    TypedDistDict::iterator i=myDict.find(type);

    if(i==myDict.end()) {
        myDict[type][id] = d;
        return true;
    }
    DistDict &dict = (*i).second;
    DistDict::iterator j=dict.find(id);
    if(j==dict.end()) {
        myDict[type][id] = d;
        return true;
    }
    return false;
}


Distribution *
NBDistribution::dictionary(const std::string &type,
                           const std::string &id)
{
    TypedDistDict::iterator i=myDict.find(type);
    if(i==myDict.end()) {
        return 0;
    }
    DistDict &dict = (*i).second;
    DistDict::iterator j=dict.find(id);
    if(j==dict.end()) {
        return 0;
    }
    return (*j).second;
}


void
NBDistribution::clear()
{
    for(TypedDistDict::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        DistDict &dict = (*i).second;
        for(DistDict::iterator j=dict.begin(); j!=dict.end(); j++) {
            delete (*j).second;
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


