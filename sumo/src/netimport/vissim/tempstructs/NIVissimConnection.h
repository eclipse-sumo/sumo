/****************************************************************************/
/// @file    NIVissimConnection.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimConnection_h
#define NIVissimConnection_h


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
#include "NIVissimExtendedEdgePoint.h"
#include <utils/geom/Position.h>
#include <utils/geom/AbstractPoly.h>
#include "NIVissimAbstractEdge.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimBoundedClusterObject.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdgeCont;


// ===========================================================================
// class definitions
// ===========================================================================
class NIVissimConnection
    : public NIVissimBoundedClusterObject,
      public NIVissimAbstractEdge {
public:
    enum Direction {
        NIVC_DIR_RIGHT,
        NIVC_DIR_LEFT,
        NIVC_DIR_ALL
    };

    NIVissimConnection(int id, const std::string& name,
                       const NIVissimExtendedEdgePoint& from_def,
                       const NIVissimExtendedEdgePoint& to_def,
                       const PositionVector& geom,
                       Direction direction, SUMOReal dxnothalt, SUMOReal dxeinordnen,
                       SUMOReal zuschlag1, SUMOReal zuschlag2, SUMOReal seglength,
                       const std::vector<int>& assignedVehicles,
                       const NIVissimClosedLanesVector& clv);
    virtual ~NIVissimConnection();
    void computeBounding();
    int getFromEdgeID() const;
    int getToEdgeID() const;
    SUMOReal getFromPosition() const;
    SUMOReal getToPosition() const;
    Position getFromGeomPosition() const;
    Position getToGeomPosition() const;
    void setNodeCluster(int nodeid);
    const Boundary& getBoundingBox() const;

    unsigned int buildEdgeConnections(NBEdgeCont& ec);

    void buildGeom();


    /** @brief Resets lane numbers if all lanes shall be used
     *
     * Calls "NIVissimExtendedEdgePoint::recheckLanes" for both used
     *  edges.
     *
     * @param[in] The built from-edge
     * @param[in] The built to-edge
     */
    void recheckLanes(const NBEdge* const fromEdge, const NBEdge* const toEdge);

public:
    const std::vector<int>& getFromLanes() const;
    const std::vector<int>& getToLanes() const;



    static bool dictionary(int id, const std::string& name,
                           const NIVissimExtendedEdgePoint& from_def,
                           const NIVissimExtendedEdgePoint& to_def,
                           const PositionVector& geom,
                           Direction direction, SUMOReal dxnothalt, SUMOReal dxeinordnen,
                           SUMOReal zuschlag1, SUMOReal zuschlag2, SUMOReal seglength,
                           const std::vector<int>& assignedVehicles,
                           const NIVissimClosedLanesVector& clv);
    static bool dictionary(int id, NIVissimConnection* o);
    static NIVissimConnection* dictionary(int id);
    static std::vector<int> getWithin(const AbstractPoly& poly);
    static void buildNodeClusters();
    static std::vector<int> getForEdge(int edgeid, bool omitNodeAssigned = true);
    static void dict_buildNBEdgeConnections(NBEdgeCont& ec);
    static void dict_assignToEdges();
    static int getMaxID();

private:
    std::string myName;
    NIVissimExtendedEdgePoint myFromDef, myToDef;
    Direction myDirection;
    SUMOReal myDXNothalt, myDXEinordnen;
    SUMOReal myZuschlag1, myZuschlag2;
    std::vector<int> myAssignedVehicles;
    NIVissimClosedLanesVector myClosedLanes;
private:
    typedef std::map<int, NIVissimConnection*> DictType;
    static DictType myDict;
    static int myMaxID;
};


#endif

/****************************************************************************/

