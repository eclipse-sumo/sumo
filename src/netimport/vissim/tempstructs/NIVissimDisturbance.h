/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimDisturbance.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimDisturbance_h
#define NIVissimDisturbance_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

