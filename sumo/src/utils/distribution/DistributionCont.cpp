//---------------------------------------------------------------------------//
//                        DistributionCont.cpp -  ccc
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
// Revision 1.2  2003/06/05 14:33:44  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include "DistributionCont.h"

DistributionCont::TypedDistDict DistributionCont::myDict;

bool
DistributionCont::dictionary(const std::string &type, const std::string &id,
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
DistributionCont::dictionary(const std::string &type,
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "DistributionCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


