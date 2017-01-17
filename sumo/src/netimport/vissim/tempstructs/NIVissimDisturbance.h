/****************************************************************************/
/// @file    NIVissimDisturbance.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimDisturbance_h
#define NIVissimDisturbance_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <utils/geom/AbstractPoly.h>
#include <netbuild/NBConnection.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimBoundedClusterObject.h"
#include "NIVissimNodeParticipatingEdgeVector.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;
class NBEdge;
class NBDistrictCont;

class NIVissimDisturbance
    : public NIVissimBoundedClusterObject {
public:
    NIVissimDisturbance(int id, const std::string& name,
                        const NIVissimExtendedEdgePoint& edge,
                        const NIVissimExtendedEdgePoint& by);
    virtual ~NIVissimDisturbance();
    void computeBounding();
    bool addToNode(NBNode* node, NBDistrictCont& dc,
                   NBNodeCont& nc, NBEdgeCont& ec);
    int getEdgeID() const {
        return myEdge.getEdgeID();
    }
    int getDisturbanceID() const {
        return myDisturbance.getEdgeID();
    }
    NBConnection getConnection(NBNode* node, int aedgeid);

public:
    static bool dictionary(const std::string& name,
                           const NIVissimExtendedEdgePoint& edge,
                           const NIVissimExtendedEdgePoint& by);
    static bool dictionary(int id, NIVissimDisturbance* o);
    static NIVissimDisturbance* dictionary(int id);
    static std::vector<int> getWithin(const AbstractPoly& poly);
    static void clearDict();
    static void dict_SetDisturbances();
    static void reportRefused();

private:
    int myID;
    int myNode;
    std::string myName;
    NIVissimExtendedEdgePoint myEdge;
    NIVissimExtendedEdgePoint myDisturbance;

    typedef std::map<int, NIVissimDisturbance*> DictType;
    static DictType myDict;
    static int myRunningID;
    static int refusedProhibits;
};


#endif

/****************************************************************************/

