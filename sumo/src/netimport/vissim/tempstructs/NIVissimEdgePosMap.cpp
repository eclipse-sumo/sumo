//---------------------------------------------------------------------------//
//                        NIVissimEdgePosMap.cpp -  ccc
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
// Revision 1.2  2003/06/05 11:46:56  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <map>
#include "NIVissimEdgePosMap.h"


NIVissimEdgePosMap::NIVissimEdgePosMap()
{
}


NIVissimEdgePosMap::~NIVissimEdgePosMap()
{
}


void
NIVissimEdgePosMap::add(int edgeid, double pos)
{
    add(edgeid, pos, pos);
}


void
NIVissimEdgePosMap::add(int edgeid, double from, double to)
{
    if(from>to) {
        double tmp = from;
        from = to;
        to = tmp;
    }
    ContType::iterator i=myCont.find(edgeid);
    if(i==myCont.end()) {
        myCont[edgeid] = Range(from, to);
    } else {
        double pfrom = (*i).second.first;
        double pto = (*i).second.second;
        if(pfrom<from) {
            from = pfrom;
        }
        if(pto>to) {
            to = pto;
        }
        myCont[edgeid] = Range(from, to);
    }
}


void
NIVissimEdgePosMap::join(NIVissimEdgePosMap &with)
{
    for(ContType::iterator i=with.myCont.begin(); i!=with.myCont.end(); i++) {
        add((*i).first, (*i).second.first, (*i).second.second);
    }
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimEdgePosMap.icc"
//#endif

// Local Variables:
// mode:C++
// End:


