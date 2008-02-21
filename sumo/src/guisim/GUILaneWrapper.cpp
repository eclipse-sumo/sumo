/****************************************************************************/
/// @file    GUILaneWrapper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// A MSLane extended for visualisation purposes.
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/geom/Position2DVector.h>
#include <microsim/MSNet.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUILaneWrapper.h"
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUINet.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/RandHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
SUMOReal GUILaneWrapper::myAllMaxSpeed = 0;


// ===========================================================================
// method definitions
// ===========================================================================
GUILaneWrapper::GUILaneWrapper(GUIGlObjectStorage &idStorage,
                               MSLane &lane, const Position2DVector &shape)
        : GUILaneRepresentation(idStorage, "lane:"+lane.getID()),
        myLane(lane), myShape(shape)
{
    SUMOReal x1 = shape[0].x();
    SUMOReal y1 = shape[0].y();
    SUMOReal x2 = shape[-1].x();
    SUMOReal y2 = shape[-1].y();
    SUMOReal length = getLength();
    // also the virtual length is set in here
    myVisLength = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    // check maximum speed
    if (myAllMaxSpeed<lane.maxSpeed()) {
        myAllMaxSpeed = lane.maxSpeed();
    }
    //
    myShapeRotations.reserve(myShape.size()-1);
    myShapeLengths.reserve(myShape.size()-1);
    for (size_t i=0; i<myShape.size()-1; ++i) {
        const Position2D &f = myShape[i];
        const Position2D &s = myShape[i+1];
        myShapeLengths.push_back(GeomHelper::distance(f, s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265);
    }
}


GUILaneWrapper::~GUILaneWrapper()
{}


SUMOReal
GUILaneWrapper::getLength() const
{
    return myLane.myLength;
}


SUMOReal
GUILaneWrapper::visLength() const
{
    return myVisLength;
}


MSEdge::EdgeBasicFunction
GUILaneWrapper::getPurpose() const
{
    return myLane.myEdge->getPurpose();
}


SUMOReal
GUILaneWrapper::maxSpeed() const
{
    return myLane.maxSpeed();
}


SUMOReal
GUILaneWrapper::getOverallMaxSpeed()
{
    return myAllMaxSpeed;
}


bool
GUILaneWrapper::forLane(const MSLane &lane) const
{
    return (&myLane)==(&lane);
}


GUIGLObjectPopupMenu *
GUILaneWrapper::getPopUpMenu(GUIMainWindow &app,
                             GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret, false);
    new FXMenuCommand(ret, "Add Successors To Selected", GUIIconSubSys::getIcon(ICON_EXT), ret, MID_ADDSELECT_SUCC);
    new FXMenuSeparator(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    SUMOReal pos = myShape.nearest_position_on_line_to_point(parent.getPositionInformation());
    new FXMenuCommand(ret, ("pos: " + toString(pos)).c_str(), 0, 0, 0);
    return ret;
}


GUIParameterTableWindow *
GUILaneWrapper::getParameterWindow(GUIMainWindow &app,
                                   GUISUMOAbstractView &)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    ret->mkItem("maxspeed [m/s]", false, myLane.maxSpeed());
    ret->mkItem("length [m]", false, myLane.length());
    // close building
    ret->closeBuilding();
    return ret;
}


GUIGlObjectType
GUILaneWrapper::getType() const
{
    return GLO_LANE;
}


const std::string &
GUILaneWrapper::microsimID() const
{
    return myLane.getID();
}


bool
GUILaneWrapper::active() const
{
    return true;
}


Boundary
GUILaneWrapper::getCenteringBoundary() const
{
    Boundary b;
    b.add(myShape[0]);
    b.add(myShape[-1]);
    b.grow(20);
    return b;
}




const Position2DVector &
GUILaneWrapper::getShape() const
{
    return myShape;
}


size_t
GUILaneWrapper::getLinkNumber() const
{
    return myLane.getLinkCont().size();
}


MSLink::LinkState
GUILaneWrapper::getLinkState(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getState();
}


MSLink::LinkDirection
GUILaneWrapper::getLinkDirection(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getDirection();
}


MSLane *
GUILaneWrapper::getLinkLane(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getLane();
}


int
GUILaneWrapper::getLinkRespondIndex(size_t pos) const
{
    return myLane.getLinkCont()[pos]->getRespondIndex();
}


const DoubleVector &
GUILaneWrapper::getShapeRotations() const
{
    return myShapeRotations;
}


const DoubleVector &
GUILaneWrapper::getShapeLengths() const
{
    return myShapeLengths;
}


const MSLane::VehCont &
GUILaneWrapper::getVehiclesSecure()
{
    return myLane.getVehiclesSecure();
}


void
GUILaneWrapper::releaseVehicles()
{
    myLane.releaseVehicles();
}


unsigned int
GUILaneWrapper::getLinkTLID(const GUINet &net, size_t pos) const
{
    return net.getLinkTLID(myLane.getLinkCont()[pos]);
}


int
GUILaneWrapper::getLinkTLIndex(const GUINet &net, size_t pos) const
{
    return net.getLinkTLIndex(myLane.getLinkCont()[pos]);
}


const MSEdge * const
GUILaneWrapper::getMSEdge() const
{
    return myLane.getEdge();
}



#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>

void
GUILaneWrapper::selectSucessors()
{
    SUMOReal maxDist = 2000;
    SUMOReal minDist = 1000;
    SUMOReal maxSpeed = 55.0;

    std::vector<GUILaneWrapper*> selected;
    selected.push_back(this);
    std::vector<std::pair<GUILaneWrapper*, SUMOReal> > toProc;
    toProc.push_back(std::pair<GUILaneWrapper*, SUMOReal>(this, 0));

    while (!toProc.empty()) {
        std::pair<GUILaneWrapper*, SUMOReal> laneAndDist =
            toProc.back();
        toProc.pop_back();
        if (laneAndDist.second<minDist||
                (laneAndDist.second<maxDist&&laneAndDist.first->maxSpeed()<maxSpeed)) {
            selected.push_back(laneAndDist.first);

            const GUIEdge * const e = static_cast<const GUIEdge * const>(laneAndDist.first->getMSEdge());
            std::vector<MSEdge*> followingEdges = e->getFollowingEdges();
            std::vector<MSEdge*> incomingEdges = e->getIncomingEdges();
            copy(incomingEdges.begin(), incomingEdges.end(), back_inserter(followingEdges));
            for (std::vector<MSEdge*>::iterator i=followingEdges.begin(); i!=followingEdges.end(); ++i) {
                const std::vector<MSLane*> * const lanes = (*i)->getLanes();
                for (std::vector<MSLane*>::const_iterator j=lanes->begin(); j!=lanes->end(); ++j) {
                    if (find(selected.begin(), selected.end(), &static_cast<GUIEdge*>(*i)->getLaneGeometry(*j))==selected.end()) {
                        toProc.push_back(std::pair<GUILaneWrapper*, SUMOReal>(
                                             &static_cast<GUIEdge*>(*i)->getLaneGeometry(*j),
                                             laneAndDist.second+laneAndDist.first->getLength()));
                    }
                }
            }
        }
    }

    for (std::vector<GUILaneWrapper*>::iterator k=selected.begin(); k!=selected.end(); ++k) {
        gSelected.select((*k)->getType(), (*k)->getGlID());
    }

    const Position2DVector &shape = getShape();
    Position2D initPos = shape.positionAtLengthPosition(getLength()/(SUMOReal) 2.0);
    Position2DVector poly;
    for (SUMOReal i=0; i<360; i += 40) {
        SUMOReal random1 = RandHelper::rand();
        Position2D p = initPos;
        p.add(sin(i)*30+random1*20, cos(i)*30+random1*20);
        poly.push_back(p);
    }
}


SUMOReal
GUILaneWrapper::firstWaitingTime() const
{
    return myLane.myVehicles.size()==0
           ? 0
           : (SUMOReal)(*(myLane.myVehicles.end()-1))->getWaitingTime();
}


SUMOReal
GUILaneWrapper::getDensity() const
{
    return myLane.getDensity();
}


SUMOReal
GUILaneWrapper::getEdgeLaneNumber() const
{
    return (SUMOReal) myLane.getEdge()->getLanes()->size();
}


SUMOReal
GUILaneWrapper::getEdgeEffort(SUMOTime t) const
{
    return (SUMOReal) myLane.getEdge()->getVehicleEffort(0, t);
}



/****************************************************************************/

