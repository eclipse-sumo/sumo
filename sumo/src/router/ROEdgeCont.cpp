//---------------------------------------------------------------------------//
//                        ROEdgeCont.cpp -
//  A container for routable edges
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
// Revision 1.4  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <map>
#include <string>
#include "ROEdge.h"
#include "ROEdgeCont.h"

ROEdgeCont::ROEdgeCont()
{
}


ROEdgeCont::~ROEdgeCont()
{
}


void
ROEdgeCont::postloadInit()
{
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i).second->postloadInit();
    }
}


/*
void
ROEdgeCont::computeWeights()
{
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i).second->computeWeight();
    }
}
*/

void
ROEdgeCont::init()
{
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i).second->init();
    }
}


std::vector<ROEdge*>
ROEdgeCont::getAllEdges() const
{
    std::vector<ROEdge*> all;
    for(myCont::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
        all.push_back((*i).second);
    }
    return all;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROEdgeCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


