#ifndef NIVissimNodeDef_Edges_h
#define NIVissimNodeDef_Edges_h
//---------------------------------------------------------------------------//
//                        NIVissimNodeDef_Edges.h -  ccc
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
#include <map>
#include "NIVissimNodeParticipatingEdgeVector.h"
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimNodeDef.h"

class NIVissimNodeDef_Edges :
        public NIVissimNodeDef {
public:
    NIVissimNodeDef_Edges(int id, const std::string &name,
        const NIVissimNodeParticipatingEdgeVector &edges);
    virtual ~NIVissimNodeDef_Edges();
    static bool dictionary(int id, const std::string &name,
        const NIVissimNodeParticipatingEdgeVector &edges);
    virtual void computeBounding();
    virtual void searchAndSetConnections();
    virtual double getEdgePosition(int edgeid) const;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
    class id_matches {
    public:
        explicit id_matches(int id) : myEdgeID(id) { }
        bool operator() (NIVissimNodeParticipatingEdge *e) {
            return e->getID()==myEdgeID;
        }
    private:
        int myEdgeID;
    };

    class lying_within_match {
    public:
        explicit lying_within_match(NIVissimNodeParticipatingEdge *e) : myEdge(e) { }
        bool operator() (NIVissimExtendedEdgePoint *e) {
            return e->getEdgeID()==myEdge->getID() &&
                myEdge->positionLiesWithin(e->getPosition());
        }
    private:
        NIVissimNodeParticipatingEdge *myEdge;
    };

protected:
    NIVissimNodeParticipatingEdgeVector myEdges;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimNodeDef_Edges.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

