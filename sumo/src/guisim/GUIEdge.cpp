/****************************************************************************/
/// @file    GUIEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A road/street connecting two junctions (gui-version)
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include <utils/gui/div/GLHelper.h>
#include <foreign/polyfonts/polyfonts.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include <microsim/MSGlobals.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// included modules
// ===========================================================================
GUIEdge::GUIEdge(const std::string &id, unsigned int numericalID,
                 GUIGlObjectStorage &idStorage) throw()
        : MSEdge(id, numericalID),
        GUIGlObject(idStorage, "edge:" + id)
{}


GUIEdge::~GUIEdge() throw()
{
    for (LaneWrapperVector::iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
        delete(*i);
    }
}


void
GUIEdge::initGeometry(GUIGlObjectStorage &idStorage) throw()
{
    // don't do this twice
    if (myLaneGeoms.size()>0) {
        return;
    }
    // build the lane wrapper
    myLaneGeoms.reserve(myLanes->size());
    for (LaneCont::reverse_iterator i=myLanes->rbegin(); i<myLanes->rend(); ++i) {
        myLaneGeoms.push_back((*i)->buildLaneWrapper(idStorage));
    }
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
    return *(myLaneGeoms[laneNo]);
}


GUILaneWrapper &
GUIEdge::getLaneGeometry(const MSLane *lane) const
{
    LaneWrapperVector::const_iterator i=
        find_if(myLaneGeoms.begin(), myLaneGeoms.end(), lane_wrapper_finder(*lane));
    assert(i!=myLaneGeoms.end());
    return *(*i);
}


std::vector<std::string>
GUIEdge::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::iterator i=MSEdge::myDict.begin(); i!=MSEdge::myDict.end(); ++i) {
        ret.push_back((*i).first);
    }
    return ret;
}


std::vector<GLuint>
GUIEdge::getIDs()
{
    std::vector<GLuint> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::iterator i=MSEdge::myDict.begin();i!=MSEdge::myDict.end(); ++i) {
        ret.push_back(static_cast<GUIEdge*>((*i).second)->getGlID());
    }
    return ret;
}


Boundary
GUIEdge::getBoundary() const
{
    Boundary ret;
    for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
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
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        netsWrappers.push_back(static_cast<GUIEdge*>((*i).second));
    }
}



GUIGLObjectPopupMenu *
GUIEdge::getPopUpMenu(GUIMainWindow &app, GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret, true);
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        buildShowParamsPopupEntry(ret, false);
    }
#endif
    return ret;
}


GUIParameterTableWindow *
GUIEdge::getParameterWindow(GUIMainWindow &app,
                            GUISUMOAbstractView &) throw()
{
    GUIParameterTableWindow *ret = 0;
#ifdef HAVE_MESOSIM
    ret = new GUIParameterTableWindow(app, *this, 5);
    // add items
    ret->mkItem("length [m]", false, myLaneGeoms[0]->getLength());
    ret->mkItem("allowed speed [m/s]", false, getAllowedSpeed());
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getDensity));
    ret->mkItem("mean vehicle speed [m/s]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getMeanSpeed));
    ret->mkItem("flow [veh/h/lane]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getFlow));
    ret->mkItem("#vehicles", true,
                new CastingFunctionBinding<GUIEdge, SUMOReal, unsigned int>(this, &GUIEdge::getVehicleNo));
    // close building
    ret->closeBuilding();
#endif
    return ret;
}


const std::string &
GUIEdge::getMicrosimID() const throw()
{
    return getID();
}


Boundary
GUIEdge::getCenteringBoundary() const throw()
{
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}


void
GUIEdge::drawGL(const GUIVisualizationSettings &s) const throw()
{
    if(s.hideConnectors&&myFunction==MSEdge::EDGEFUNCTION_CONNECTOR) {
        return;
    }
    // check whether lane boundaries shall be drawn
    if (s.scale>1.&&s.laneShowBorders&&myFunction!=MSEdge::EDGEFUNCTION_INTERNAL) {
        glPolygonOffset(0, 2);
        glColor3d(1,1,1);
        // (optional) set invalid id
        if (s.needsGlID) {
            glPushName(0);
        }
        // draw white boundings
        size_t k;
        for (k=0; k<myLanes->size(); k++) {
            GUILaneWrapper *lane = myLaneGeoms[k];
            GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), SUMO_const_halfLaneAndOffset);
        }
        glPolygonOffset(0, 1);
        for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end()-1; ++i) {
            (*i)->drawBordersGL(s);
        }
        // (optional) clear id
        if (s.needsGlID) {
            glPopName();
        }
    }
    // draw the lanes
    for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
        (*i)->drawGL(s);
    }
    // (optionally) draw the name
    if (s.drawEdgeName) {
        glColor3f(s.edgeNameColor.red(), s.edgeNameColor.green(), s.edgeNameColor.blue());
        glPolygonOffset(0, -6);
        GUILaneWrapper *lane1 = myLaneGeoms[0];
        GUILaneWrapper *lane2 = myLaneGeoms[myLaneGeoms.size()-1];
        glPushMatrix();
        Position2D p = lane1->getShape().positionAtLengthPosition(lane1->getShape().length()/(SUMOReal) 2.);
        p.add(lane2->getShape().positionAtLengthPosition(lane2->getShape().length()/(SUMOReal) 2.));
        p.mul(.5);
        glTranslated(p.x(), p.y(), 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(s.edgeNameSize / s.scale);
        SUMOReal w = pfdkGetStringWidth(getMicrosimID().c_str());
        glRotated(180, 1, 0, 0);
        SUMOReal angle = lane1->getShape().rotationDegreeAtLengthPosition(lane1->getShape().length()/(SUMOReal) 2.);
        angle += 90;
        if (angle>90&&angle<270) {
            angle -= 180;
        }
        glRotated(angle, 0, 0, 1);
        glTranslated(-w/2., .2*s.edgeNameSize / s.scale, 0);
        pfDrawString(getMicrosimID().c_str());
        glPopMatrix();
    }
}

#ifdef HAVE_MESOSIM
unsigned int
GUIEdge::getVehicleNo() const
{
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((GUIEdge*)this);
    assert(first!=0);
    unsigned int vehNo = 0;
    do {
        vehNo += first->getCarNumber();
        first = first->getNextSegment();
    } while (first!=0);
    return vehNo;
}


SUMOReal
GUIEdge::getFlow() const
{
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((GUIEdge*)this);
    assert(first!=0);
    SUMOReal flow = -1;
    int no = 0;
    do {
        SUMOReal vehNo = (SUMOReal) first->getCarNumber();
        SUMOReal v = first->getMeanSpeed();
        if (vehNo!=0) {
            if (no==0) {
                flow = (vehNo * (SUMOReal) 1000. / first->getLength()) * v / (SUMOReal) 3.6;
            } else {
                flow += (vehNo * (SUMOReal) 1000. / first->getLength()) * v / (SUMOReal) 3.6;
            }
            no++;
        }
        first = first->getNextSegment();
    } while (first!=0);
    if (flow>=0) {
        return flow/(SUMOReal)myLanes->size();
    }
    return -1;
}


SUMOReal
GUIEdge::getDensity() const
{
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((GUIEdge*)this);
    assert(first!=0);
    SUMOReal occ = 0;
    int no = 0;
    do {
        occ += first->occupancy() / first->getLength() / (SUMOReal) nLanes();
        no++;
        first = first->getNextSegment();
    } while (first!=0);
    if (no!=0) {
        return occ/(SUMOReal) no;
    }
    return -1;
}


SUMOReal
GUIEdge::getRouteSpread() const
{
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((GUIEdge*)this);
    assert(first!=0);
    SUMOReal occ = 0;
    int no = 0;
    do {
        occ += first->getRouteSpread();
        no++;
        first = first->getNextSegment();
    } while (first!=0);
    return occ/(SUMOReal) no;
}


SUMOReal
GUIEdge::getMeanSpeed() const
{
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((GUIEdge*)this);
    assert(first!=0);
    SUMOReal v = 0;
    int no = 0;
    do {
        v += first->getMeanSpeed();
        no++;
        first = first->getNextSegment();
    } while (first!=0);
    if (no!=0) {
        return v/(SUMOReal) no;
    }
    return -1;
}


SUMOReal
GUIEdge::getAllowedSpeed() const
{
    return (*myLanes)[0]->maxSpeed();
}

#endif


/****************************************************************************/

