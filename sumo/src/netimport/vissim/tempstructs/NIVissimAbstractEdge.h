/****************************************************************************/
/// @file    NIVissimAbstractEdge.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimAbstractEdge {
public:
    NIVissimAbstractEdge(int id, const Position2DVector &geom);
    virtual ~NIVissimAbstractEdge();
    Position2D getGeomPosition(SUMOReal pos) const;
    void splitAssigning();
    bool crossesEdge(NIVissimAbstractEdge *c) const;
    Position2D crossesEdgeAtPoint(NIVissimAbstractEdge *c) const;
    bool overlapsWith(const AbstractPoly &p, SUMOReal offset=0.0) const;
    virtual void setNodeCluster(int nodeid) = 0;
    bool hasNodeCluster() const;
    SUMOReal crossesAtPoint(const Position2D &p1,
                            const Position2D &p2) const;

    virtual void buildGeom() = 0;
    int getID() const;
    const Position2DVector &getGeometry() const;

    void addDisturbance(int disturbance);

    const IntVector &getDisturbances() const;

public:
    static bool dictionary(int id, NIVissimAbstractEdge *e);
    static NIVissimAbstractEdge *dictionary(int id);
    static void splitAndAssignToNodes();
    static IntVector getWithin(const AbstractPoly &p, SUMOReal offset=0.0);
    static void clearDict();


protected:
    int myID;
    Position2DVector myGeom;
    IntVector myDisturbances;
    int myNode;

private:
    typedef std::map<int, NIVissimAbstractEdge*> DictType;
    static DictType myDict;
};


#endif

/****************************************************************************/

