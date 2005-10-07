#ifndef NIVissimAbstractEdge_h
#define NIVissimAbstractEdge_h
//---------------------------------------------------------------------------//
//                        NIVissimAbstractEdge.h -  ccc
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
// Revision 1.9  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.5  2003/06/05 11:46:55  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


#include <map>
#include <utils/common/IntVector.h>
#include <utils/geom/Position2DVector.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimAbstractEdge {
public:
    NIVissimAbstractEdge(int id, const Position2DVector &geom);
    virtual ~NIVissimAbstractEdge();
    Position2D getGeomPosition(SUMOReal pos) const;
    Position2D getBeginPosition() const;
    Position2D getEndPosition() const;
    void splitAssigning();
    bool crossesEdge(NIVissimAbstractEdge *c) const;
    Position2D crossesEdgeAtPoint(NIVissimAbstractEdge *c) const;
    bool overlapsWith(const AbstractPoly &p, SUMOReal offset=0.0) const;
    virtual void setNodeCluster(int nodeid) = 0;
    bool hasNodeCluster() const;
    SUMOReal crossesAtPoint(const Position2D &p1,
        const Position2D &p2) const;

    bool hasGeom() const;
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




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

