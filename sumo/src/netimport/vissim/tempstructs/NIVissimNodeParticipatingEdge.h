#ifndef NIVissimNodeParticipatingEdge_h
#define NIVissimNodeParticipatingEdge_h
//---------------------------------------------------------------------------//
//                        NIVissimNodeParticipatingEdge.h -  ccc
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
#include <utils/geom/Position2D.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimNodeParticipatingEdge {
public:
    NIVissimNodeParticipatingEdge(int edgeid,
        double frompos, double topos);
    ~NIVissimNodeParticipatingEdge();
    int getID() const;
    bool positionLiesWithin(double pos) const;
    double getFromPos() const;
    double getToPos() const;
private:
    int myEdgeID;
    double myFromPos, myToPos;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimNodeParticipatingEdge.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

