//---------------------------------------------------------------------------//
//                        GUIEdge.cpp -
//  An MSEdge extended by values needed for the gui
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
// Revision 1.8  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.7  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.6  2003/04/14 08:27:16  dkrajzew
// new globject concept implemented
//
// Revision 1.5  2003/03/17 14:09:10  dkrajzew
// Windows eol removed
//
// Revision 1.4  2003/03/12 16:52:05  dkrajzew
// centering of objects debuggt
//
// Revision 1.3  2003/02/07 10:39:17  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <gui/GUIGlObjectStorage.h>
#include <utils/geom/GeomHelper.h>
#include "GUILaneChanger.h"
#include "GUILane.h"
#include "GUIEdge.h"
#include "GUINet.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * included modules
 * ======================================================================= */
GUIEdge::GUIEdge(std::string id)
    : MSEdge(id)
{
}


GUIEdge::~GUIEdge()
{
    for(LaneWrapperVector::iterator i=_laneGeoms.begin(); i!=_laneGeoms.end(); i++) {
        delete (*i);
    }
}

void
GUIEdge::initJunctions(MSJunction *from, MSJunction *to,
                       GUIGlObjectStorage &idStorage)
{
    // set the information about the nodes
    //  !!! not longer needed
    _from = from;
    _to = to;
    // build the lane wrapper
    LaneWrapperVector tmp;
    for(LaneCont::reverse_iterator i=myLanes->rbegin(); i<myLanes->rend(); i++) {
        tmp.push_back((*i)->buildLaneWrapper(idStorage));
    }
    _laneGeoms.reserve(tmp.size());
    copy(tmp.rbegin(), tmp.rend(), back_inserter(_laneGeoms));
/*
    // set the geomertical information for every lane
    double x1 = fromXPos();
    double y1 = fromYPos();
    double x2 = toXPos();
    double y2 = toYPos();
    double length = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
    std::pair<double, double> offsets =
        GeomHelper::getNormal90D_CW(x1, y1, x2, y2, length, 3.5);
    double xoff = offsets.first / 2.0;
    double yoff = offsets.second / 2.0;
    LaneWrapperVector tmp;
    for(LaneCont::reverse_iterator i=myLanes->rbegin(); i<myLanes->rend(); i++) {
        GUILaneWrapper *wrapper =
            new GUILaneWrapper(idStorage, *(*i), x1-xoff, y1-yoff, x2-xoff, y2-yoff);
        tmp.push_back(wrapper);
        xoff += offsets.first;
        yoff += offsets.second;
    }
    // copy reverse
    _laneGeoms.reserve(tmp.size());
    copy(tmp.rbegin(), tmp.rend(), back_inserter(_laneGeoms));*/
}

MSLane &
GUIEdge::getLane(size_t laneNo)
{
    assert(laneNo<myLanes->size());
    return *((*myLanes)[laneNo]);
}


GUILaneWrapper &
GUIEdge::getLaneGeometry(size_t laneNo)
{
    assert(laneNo<myLanes->size());
    return *(_laneGeoms[laneNo]);
}

std::vector<std::string>
GUIEdge::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSEdge::myDict.size());
    for(MSEdge::DictType::iterator i=MSEdge::myDict.begin();
        i!=MSEdge::myDict.end(); i++) {
        ret.push_back((*i).first);
    }
    return ret;
}


double
GUIEdge::toXPos() const
{
    return _to->getXCoordinate();
}


double
GUIEdge::fromXPos() const
{
    return _from->getXCoordinate();
}


double
GUIEdge::toYPos() const
{
    return _to->getYCoordinate();
}


double
GUIEdge::fromYPos() const
{
    return _from->getYCoordinate();
}


std::string
GUIEdge::getID() const
{
    return myID;
}


void
GUIEdge::initialize(AllowedLanesCont* allowed, MSLane* departLane,
                   LaneCont* lanes, EdgeBasicFunction function)
{
    myAllowed = allowed;
    myDepartLane = departLane;
    myLanes = lanes;
    _function = function;

    if ( myLanes->size() > 1 ) {
        myLaneChanger = new GUILaneChanger( myLanes );
    }
}


Position2D
GUIEdge::getLanePosition(const MSLane &lane, double pos) const
{
    LaneWrapperVector::const_iterator i =
        find_if(_laneGeoms.begin(), _laneGeoms.end(),
        lane_wrapper_finder(lane));
    // the lane should be one of this edge
    assert(i!=_laneGeoms.end());
    // compute the position and return it
    const Position2D &laneEnd = (*i)->getBegin();
    const Position2D &laneDir = (*i)->getDirection();
    double posX = laneEnd.x() - laneDir.x() * pos;
    double posY = laneEnd.y() - laneDir.y() * pos;
    return Position2D(posX, posY);
}


void
GUIEdge::fill(std::vector<GUIEdge*> &netsWrappers)
{
    size_t size = MSEdge::dictSize();
    netsWrappers.reserve(size);
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        netsWrappers.push_back(static_cast<GUIEdge*>((*i).second));
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIEdge.icc"
//#endif

// Local Variables:
// mode:C++
// End:


