//---------------------------------------------------------------------------//
//                        NIVissimAbstractEdge.cpp -  ccc
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.9  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.7  2003/10/15 11:51:28  dkrajzew
// further work on vissim-import
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
#include <config.h>
#endif // HAVE_CONFIG_H


#include <map>
#include <cassert>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Line2D.h>
#include "NIVissimAbstractEdge.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


NIVissimAbstractEdge::DictType NIVissimAbstractEdge::myDict;

NIVissimAbstractEdge::NIVissimAbstractEdge(int id,
                                           const Position2DVector &geom)
    : myID(id), myGeom(geom), myNode(-1)
{
    dictionary(id, this);
}


NIVissimAbstractEdge::~NIVissimAbstractEdge()
{
}


bool
NIVissimAbstractEdge::dictionary(int id, NIVissimAbstractEdge *e)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = e;
        return true;
    }
    return false;
}


NIVissimAbstractEdge *
NIVissimAbstractEdge::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}



Position2D
NIVissimAbstractEdge::getGeomPosition(SUMOReal pos) const
{
    if(myGeom.length()<pos) {
        SUMOReal amount = pos - myGeom.length();
        Line2D l(myGeom.at(myGeom.size()-2),
            GeomHelper::extrapolate_second(
                myGeom.at(myGeom.size()-2), myGeom.at(myGeom.size()-1), amount*2));
        return l.getPositionAtDistance(pos);
    }
    return myGeom.positionAtLengthPosition(pos);
}


Position2D
NIVissimAbstractEdge::getBeginPosition() const
{
    return myGeom.at(0);
}


Position2D
NIVissimAbstractEdge::getEndPosition() const
{
    return myGeom.at(myGeom.size()-1);
}


bool
NIVissimAbstractEdge::hasGeom() const
{
    return myGeom.size()>0;
}



void
NIVissimAbstractEdge::splitAndAssignToNodes()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimAbstractEdge *e = (*i).second;
        e->splitAssigning();
    }
}

void
NIVissimAbstractEdge::splitAssigning()
{
}





bool
NIVissimAbstractEdge::crossesEdge(NIVissimAbstractEdge *c) const
{
    return myGeom.intersects(c->myGeom);
}


Position2D
NIVissimAbstractEdge::crossesEdgeAtPoint(NIVissimAbstractEdge *c) const
{
    return myGeom.intersectsAtPoint(c->myGeom);
}


SUMOReal
NIVissimAbstractEdge::crossesAtPoint(const Position2D &p1,
                                     const Position2D &p2) const
{
    // !!! not needed
    Position2D p = GeomHelper::intersection_position(
        myGeom.getBegin(), myGeom.getEnd(), p1, p2);
    return GeomHelper::nearest_position_on_line_to_point(
        myGeom.getBegin(), myGeom.getEnd(), p);
}



IntVector
NIVissimAbstractEdge::getWithin(const AbstractPoly &p, SUMOReal offset)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimAbstractEdge *e = (*i).second;
        if(e->overlapsWith(p, offset)) {
            ret.push_back(e->myID);
        }
    }
    return ret;
}


bool
NIVissimAbstractEdge::overlapsWith(const AbstractPoly &p, SUMOReal offset) const
{
    return myGeom.overlapsWith(p, offset);
}


bool
NIVissimAbstractEdge::hasNodeCluster() const
{
    return myNode!=-1;
}


int
NIVissimAbstractEdge::getID() const
{
    return myID;
}

void
NIVissimAbstractEdge::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


const Position2DVector &
NIVissimAbstractEdge::getGeometry() const
{
    return myGeom;
}


void
NIVissimAbstractEdge::addDisturbance(int disturbance)
{
    myDisturbances.push_back(disturbance);
}


const IntVector &
NIVissimAbstractEdge::getDisturbances() const
{
    return myDisturbances;
}






/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


