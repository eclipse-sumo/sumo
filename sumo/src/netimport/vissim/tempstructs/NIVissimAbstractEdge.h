/****************************************************************************/
/// @file    NIVissimAbstractEdge.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimAbstractEdge_h
#define NIVissimAbstractEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <utils/geom/PositionVector.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimAbstractEdge {
public:
    NIVissimAbstractEdge(int id, const PositionVector& geom);
    virtual ~NIVissimAbstractEdge();
    Position getGeomPosition(SUMOReal pos) const;
    void splitAssigning();
    bool crossesEdge(NIVissimAbstractEdge* c) const;
    Position crossesEdgeAtPoint(NIVissimAbstractEdge* c) const;
    bool overlapsWith(const AbstractPoly& p, SUMOReal offset = 0.0) const;
    virtual void setNodeCluster(int nodeid) = 0;
    bool hasNodeCluster() const;
    SUMOReal crossesAtPoint(const Position& p1,
                            const Position& p2) const;

    virtual void buildGeom() = 0;
    int getID() const;
    const PositionVector& getGeometry() const;

    void addDisturbance(int disturbance);

    const std::vector<int>& getDisturbances() const;

public:
    static bool dictionary(int id, NIVissimAbstractEdge* e);
    static NIVissimAbstractEdge* dictionary(int id);
    static void splitAndAssignToNodes();
    static std::vector<int> getWithin(const AbstractPoly& p, SUMOReal offset = 0.0);
    static void clearDict();


protected:
    int myID;
    PositionVector myGeom;
    std::vector<int> myDisturbances;
    int myNode;

private:
    typedef std::map<int, NIVissimAbstractEdge*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

