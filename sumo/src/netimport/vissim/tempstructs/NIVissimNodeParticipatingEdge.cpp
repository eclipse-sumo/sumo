//---------------------------------------------------------------------------//
//                        NIVissimNodeParticipatingEdge.cpp -  ccc
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


#include "NIVissimEdge.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeParticipatingEdge.h"



NIVissimNodeParticipatingEdge::NIVissimNodeParticipatingEdge(
        int edgeid, double frompos, double topos)
    : myEdgeID(edgeid), myFromPos(frompos), myToPos(topos)
{
}

NIVissimNodeParticipatingEdge::~NIVissimNodeParticipatingEdge()
{
}


int
NIVissimNodeParticipatingEdge::getID() const
{
    return myEdgeID;
}


bool
NIVissimNodeParticipatingEdge::positionLiesWithin(double pos) const
{
    return
        (myFromPos<myToPos && myFromPos>=pos && myToPos<=pos)
        ||
        (myFromPos>myToPos && myFromPos<=pos && myToPos>=pos);
}

double
NIVissimNodeParticipatingEdge::getFromPos() const
{
    return myFromPos;
}


double
NIVissimNodeParticipatingEdge::getToPos() const
{
    return myToPos;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimNodeParticipatingEdge.icc"
//#endif

// Local Variables:
// mode:C++
// End:


