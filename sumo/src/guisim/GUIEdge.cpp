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
// Revision 1.16  2004/04/02 11:20:35  dkrajzew
// changes needed to visualise the selection status
//
// Revision 1.15  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.14  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
// Revision 1.13  2003/11/11 08:13:23  dkrajzew
// consequent usage of Position2D instead of two doubles
//
// Revision 1.12  2003/09/22 12:38:51  dkrajzew
// detectors need const Lanes
//
// Revision 1.11  2003/09/05 14:58:50  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.10  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.9  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
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
#include "GUIEdge.h"
#include "GUINet.h"
#include "GUILaneChanger.h"
#include "GUILane.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * included modules
 * ======================================================================= */
GUIEdge::GUIEdge(std::string id, GUIGlObjectStorage &idStorage)
    : MSEdge(id), GUIGlObject(idStorage, string("edge:") + id),
    _from(0), _to(0)
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
    if(_from!=0&&_to!=0) {
        assert(from==_from&&to==_to);
        return;
    }
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
}


MSLane &
GUIEdge::getLane(size_t laneNo)
{
    assert(laneNo<myLanes->size());
    return *((*myLanes)[laneNo]);
}


GUILaneWrapper &
GUIEdge::getLaneGeometry(size_t laneNo) const
{
    assert(laneNo<myLanes->size());
    return *(_laneGeoms[laneNo]);
}


GUILaneWrapper &
GUIEdge::getLaneGeometry(const MSLane *lane) const
{
    LaneWrapperVector::const_iterator i=
        find_if(_laneGeoms.begin(), _laneGeoms.end(),
            lane_wrapper_finder(*lane));
    assert(i!=_laneGeoms.end());
    return *(*i);
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


std::vector<size_t>
GUIEdge::getIDs()
{
    std::vector<size_t> ret;
    ret.reserve(MSEdge::myDict.size());
    for(MSEdge::DictType::iterator i=MSEdge::myDict.begin();
        i!=MSEdge::myDict.end(); i++) {
        ret.push_back(static_cast<GUIEdge*>((*i).second)->getGlID());
    }
    return ret;
}


Boundery
GUIEdge::getBoundery() const
{
    Boundery ret;
    ret.add(_to->getPosition());
    ret.add(_from->getPosition());
    return ret;
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



GUIGlObjectType
GUIEdge::getType() const
{
    return GLO_EDGE;
}


std::string
GUIEdge::microsimID() const
{
    return id();
}


bool
GUIEdge::active() const
{
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


