/****************************************************************************/
/// @file    GUIEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// An MSEdge extended by values needed for the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLaneChanger.h>
#include <microsim/MSGlobals.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/GeomHelper.h>
#include "GUIEdge.h"
#include "GUINet.h"
#include "GUILane.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// included modules
// ===========================================================================
GUIEdge::GUIEdge(const std::string &id, size_t numericalID,
                 GUIGlObjectStorage &idStorage)
        : MSEdge(id, numericalID),
        GUIGlObject(idStorage, "edge:" + id)
{}


GUIEdge::~GUIEdge()
{
    for (LaneWrapperVector::iterator i=_laneGeoms.begin(); i!=_laneGeoms.end(); i++) {
        delete(*i);
    }
}

void
GUIEdge::initGeometry(GUIGlObjectStorage &idStorage)
{
    // do not this twice
    if (_laneGeoms.size()>0) {
        return;
    }
    // build the lane wrapper
    LaneWrapperVector tmp;
    for (LaneCont::reverse_iterator i=myLanes->rbegin(); i<myLanes->rend(); i++) {
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
        find_if(_laneGeoms.begin(), _laneGeoms.end(), lane_wrapper_finder(*lane));
    assert(i!=_laneGeoms.end());
    return *(*i);
}


std::vector<std::string>
GUIEdge::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::iterator i=MSEdge::myDict.begin(); i!=MSEdge::myDict.end(); i++) {
        ret.push_back((*i).first);
    }
    return ret;
}


std::vector<size_t>
GUIEdge::getIDs()
{
    std::vector<size_t> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::iterator i=MSEdge::myDict.begin();i!=MSEdge::myDict.end(); i++) {
        ret.push_back(static_cast<GUIEdge*>((*i).second)->getGlID());
    }
    return ret;
}


Boundary
GUIEdge::getBoundary() const
{
    Boundary ret;
    for (LaneWrapperVector::const_iterator i=_laneGeoms.begin(); i!=_laneGeoms.end(); ++i) {
        const Position2DVector &g = (*i)->getShape();
        for (unsigned int j=0; j<g.size(); j++) {
            ret.add(g[j]);
        }
    }
    ret.grow(10);
    return ret;
}


void
GUIEdge::fill(std::vector<GUIEdge*> &netsWrappers)
{
    size_t size = MSEdge::dictSize();
    netsWrappers.reserve(size);
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        netsWrappers.push_back(static_cast<GUIEdge*>((*i).second));
    }
}



GUIGLObjectPopupMenu *
GUIEdge::getPopUpMenu(GUIMainWindow &app, GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIEdge::getParameterWindow(GUIMainWindow &,
                            GUISUMOAbstractView &)
{
    GUIParameterTableWindow *ret = 0;
    return ret;
}



GUIGlObjectType
GUIEdge::getType() const
{
    return GLO_EDGE;
}


const std::string &
GUIEdge::microsimID() const
{
    return getID();
}


bool
GUIEdge::active() const
{
    return true;
}


Boundary
GUIEdge::getCenteringBoundary() const
{
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}



/****************************************************************************/

