/****************************************************************************/
/// @file    NIVissimNodeDef_Edges.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimNodeDef_Edges_h
#define NIVissimNodeDef_Edges_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <string>
#include <map>
#include "NIVissimNodeParticipatingEdgeVector.h"
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimNodeDef.h"

class NIVissimNodeDef_Edges :
    public NIVissimNodeDef {
public:
    NIVissimNodeDef_Edges(int id, const std::string& name,
                          const NIVissimNodeParticipatingEdgeVector& edges);
    virtual ~NIVissimNodeDef_Edges();
    static bool dictionary(int id, const std::string& name,
                           const NIVissimNodeParticipatingEdgeVector& edges);
//    virtual void computeBounding();
//    virtual void searchAndSetConnections();
    virtual SUMOReal getEdgePosition(int edgeid) const;

    /**
     *
     */
    class id_matches {
    public:
        explicit id_matches(int id) : myEdgeID(id) { }
        bool operator()(NIVissimNodeParticipatingEdge* e) {
            return e->getID() == myEdgeID;
        }
    private:
        int myEdgeID;
    };

    class lying_within_match {
    public:
        explicit lying_within_match(NIVissimNodeParticipatingEdge* e) : myEdge(e) { }
        bool operator()(NIVissimExtendedEdgePoint* e) {
            return e->getEdgeID() == myEdge->getID() &&
                   myEdge->positionLiesWithin(e->getPosition());
        }
    private:
        NIVissimNodeParticipatingEdge* myEdge;
    };

protected:
    NIVissimNodeParticipatingEdgeVector myEdges;
};


#endif

/****************************************************************************/

