#ifndef NIVissimConnection_h
#define NIVissimConnection_h
//---------------------------------------------------------------------------//
//                        NIVissimConnection.h -  ccc
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
// Revision 1.7  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.5  2003/09/23 14:16:37  dkrajzew
// further work on vissim-import
//
// Revision 1.4  2003/06/05 11:46:56  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <string>
#include <map>
#include <utils/common/IntVector.h>
#include "NIVissimExtendedEdgePoint.h"
#include <utils/geom/Position2D.h>
#include <utils/geom/AbstractPoly.h>
#include "NIVissimAbstractEdge.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimBoundedClusterObject.h"


class NBEdgeCont;


class NIVissimConnection
        : public NIVissimBoundedClusterObject,
          public NIVissimAbstractEdge {
public:
    enum Direction {
        NIVC_DIR_RIGHT,
        NIVC_DIR_LEFT,
        NIVC_DIR_ALL
    };

    NIVissimConnection(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &from_def,
        const NIVissimExtendedEdgePoint &to_def,
        const Position2DVector &geom,
        Direction direction, double dxnothalt, double dxeinordnen,
        double zuschlag1, double zuschlag2, double seglength,
        const IntVector &assignedVehicles,
        const NIVissimClosedLanesVector &clv);
    virtual ~NIVissimConnection();
    void computeBounding();
    int getFromEdgeID() const;
    int getToEdgeID() const;
    double getFromPosition() const;
    double getToPosition() const;
    Position2D getFromGeomPosition() const;
    Position2D getToGeomPosition() const;
    void setNodeCluster(int nodeid);
    void unsetCluster();
    const Boundary &getBoundingBox() const;

    void buildGeom();

public:
    static bool dictionary(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &from_def,
        const NIVissimExtendedEdgePoint &to_def,
        const Position2DVector &geom,
        Direction direction, double dxnothalt, double dxeinordnen,
        double zuschlag1, double zuschlag2, double seglength,
        const IntVector &assignedVehicles,
        const NIVissimClosedLanesVector &clv);
    static bool dictionary(int id, NIVissimConnection *o);
    static NIVissimConnection *dictionary(int id);
    static IntVector getWithin(const AbstractPoly &poly);
    static void buildNodeClusters();
    static IntVector getForEdge(int edgeid, bool omitNodeAssigned=true);
    static void dict_buildNBEdgeConnections(NBEdgeCont &ec);
    static void dict_assignToEdges();
    static int getMaxID();
    const IntVector &getFromLanes() const;
    const IntVector &getToLanes() const;
/*    static void assignNodes();
    static void buildFurtherNodes();
    static IntVector getOutgoingForEdge(int edgeid);
    static IntVector getIncomingForEdge(int edgeid);*/

private:
    std::string myName;
    NIVissimExtendedEdgePoint myFromDef, myToDef;
    Direction myDirection;
    double myDXNothalt, myDXEinordnen;
    double myZuschlag1, myZuschlag2;
    IntVector myAssignedVehicles;
    NIVissimClosedLanesVector myClosedLanes;
private:
    typedef std::map<int, NIVissimConnection*> DictType;
    static DictType myDict;
    static int myMaxID;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

