//---------------------------------------------------------------------------//
//                        GUILaneWrapper.cpp -
//  Holds geometrical values for a lane
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25 Nov 2002
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
// Revision 1.27  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.26  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.25  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.24  2005/07/12 12:18:09  dkrajzew
// further visualisation options added
//
// Revision 1.23  2005/05/04 08:00:34  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; possibility to select lanes around a lane added
//
// Revision 1.22  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.21  2004/07/02 08:54:11  dkrajzew
// some design issues
//
// Revision 1.20  2004/04/02 11:18:37  dkrajzew
// recenter view - icon added to the popup menu
//
// Revision 1.19  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.18  2003/12/04 13:36:10  dkrajzew
// work on setting of aggregated value bounderies
//
// Revision 1.17  2003/11/26 10:58:30  dkrajzew
// messages from the simulation are now also passed to the message handler
//
// Revision 1.16  2003/11/20 13:21:17  dkrajzew
// further work on aggregated views
//
// Revision 1.15  2003/11/12 14:01:54  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.14  2003/10/30 08:59:43  dkrajzew
// first implementation of aggregated views using E2-detectors
//
// Revision 1.13  2003/10/01 11:15:09  dkrajzew
// GUILaneStateReporter moved to guisim
//
// Revision 1.12  2003/09/05 14:59:54  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.11  2003/08/21 12:50:49  dkrajzew
// retrival of a links direction added
//
// Revision 1.10  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.9  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
// Revision 1.8  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.7  2003/06/06 10:29:24  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies under linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.6  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included Makefiles added
//
// Revision 1.5  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.4  2003/04/14 08:27:17  dkrajzew
// new globject concept implemented
//
// Revision 1.3  2003/03/17 14:09:11  dkrajzew
// Windows eol removed
//
// Revision 1.2  2003/03/12 16:52:06  dkrajzew
// centering of objects debuggt
//
// Revision 1.1  2003/02/07 10:39:17  dkrajzew
// updated
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

#include "GUILaneStateReporter.h"
#include <string>
#include <iostream>
#include <utility>
#include <utils/foxtools/FXMutex.h>
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
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIGlobalSelection.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
SUMOReal GUILaneWrapper::myAllMaxSpeed = 0;
size_t GUILaneWrapper::myAggregationSizes[] = {
    60, 300, 900
};


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUILaneWrapper::GUILaneWrapper(GUIGlObjectStorage &idStorage,
                               MSLane &lane, const Position2DVector &shape)
    : GUILaneRepresentation(idStorage, string("lane:")+lane.id()),
    myLane(lane), myShape(shape), myAggregatedValues(0)
{
    SUMOReal x1 = shape.at(0).x();
    SUMOReal y1 = shape.at(0).y();
    SUMOReal x2 = shape.at(shape.size()-1).x();
    SUMOReal y2 = shape.at(shape.size()-1).y();
    SUMOReal length = getLength();
    _begin = Position2D(x1, y1);
    _end = Position2D(x2, y2);
    _direction = Position2D((x1-x2)/length, (y1-y2)/length);
    _rotation = (SUMOReal) atan2((x2-x1), (y1-y2))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    // also the virtual length is set in here
    _visLength = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    // check maximum speed
    if(myAllMaxSpeed<lane.maxSpeed()) {
        myAllMaxSpeed = lane.maxSpeed();
    }
    // build the storage for lane wrappers when wished
    if(gAllowAggregated) {
        buildAggregatedValuesStorage();
    }
    //
    myShapeRotations.reserve(myShape.size()-1);
    myShapeLengths.reserve(myShape.size()-1);
    for(size_t i=0; i<myShape.size()-1; i++) {
        const Position2D &f = myShape.at(i);
        const Position2D &s = myShape.at(i+1);
        myShapeLengths.push_back(GeomHelper::distance(f, s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265);
    }
}


GUILaneWrapper::~GUILaneWrapper()
{
    if(myAggregatedValues!=0) {
        for(size_t i=0; i<3; i++) {
            delete myAggregatedValues[i];
        }
        delete myAggregatedValues;
    }
}


const Position2D &
GUILaneWrapper::getBegin() const
{
    return _begin;
}


const Position2D &
GUILaneWrapper::getEnd() const
{
    return _end;
}


const Position2D &
GUILaneWrapper::getDirection() const
{
    return _direction;
}


SUMOReal
GUILaneWrapper::getRotation() const
{
    return _rotation;
}


SUMOReal
GUILaneWrapper::getLength() const
{
    return myLane.myLength;
}


SUMOReal
GUILaneWrapper::visLength() const
{
    return _visLength;
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
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    if(gSelected.isSelected(GLO_LANE, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    new FXMenuCommand(ret, "Add Successors To Selected",
        GUIIconSubSys::getIcon(ICON_EXT), ret, MID_ADDSELECT_SUCC);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter",
        GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);
    return ret;
}


GUIParameterTableWindow *
GUILaneWrapper::getParameterWindow(GUIMainWindow &app,
                                   GUISUMOAbstractView &parent)
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


std::string
GUILaneWrapper::microsimID() const
{
    return myLane.id();
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


void
GUILaneWrapper::buildAggregatedValuesStorage()
{
    /*
    myAggregatedValues = 0;
    if(myLane.length()<1) {
        return;
    }
    // build the second SUMORealers if allowed
    if(gAllowAggregatedFloating) {
        myAggregatedValues = new LoggedValue_TimeFloating<SUMOReal>*[3];
        for(size_t i=0; i<3; i++) {
            myAggregatedValues[i] =
                new LoggedValue_TimeFloating<SUMOReal>(60);
        }
    }
    // make them read from lane
    myAggregatedFloats[0] = 0; // density
    myAggregatedFloats[1] = (SUMOReal) myLane.maxSpeed(); // speed
    myAggregatedFloats[2] = 0; // haltings
    string id = string("*") + myLane.id();
    if(gAllowAggregatedFloating) {
        new GUILaneStateReporter(
            myAggregatedValues[0], myAggregatedValues[1], myAggregatedValues[2],
            myAggregatedFloats[0], myAggregatedFloats[1], myAggregatedFloats[2],
            id, &myLane, 60);
    } else {
        new GUILaneStateReporter(
            0, 0, 0,
            myAggregatedFloats[0], myAggregatedFloats[1], myAggregatedFloats[2],
            id, &myLane, 60);
    }
    //
    // !!!!!
    */
}


SUMOReal
GUILaneWrapper::getAggregatedNormed(E2::DetType what,
                                    size_t aggregationPosition) const
{
    if(myAggregatedValues==0) {
        return -1;
    }
    switch(what) {
    case E2::DENSITY:
        return myAggregatedValues[0]->getAvg() / (SUMOReal) 200.0;
    case E2::SPACE_MEAN_SPEED:
        return myAggregatedValues[1]->getAvg() / myAllMaxSpeed;
    case E2::HALTING_DURATION_MEAN:
        {
            SUMOReal val = myAggregatedValues[2]->getAvg();
            if(val>MSGlobals::gTimeToGridlock) {
                return 1;
            } else {
                return val / (SUMOReal) MSGlobals::gTimeToGridlock;
            }
        }
    default:
        throw 1;
    }
}


SUMOReal
GUILaneWrapper::getAggregatedFloat(E2::DetType what) const
{
    switch(what) {
    case E2::DENSITY:
        return myAggregatedFloats[0] / (SUMOReal) 200.0;
    case E2::SPACE_MEAN_SPEED:
        return myAggregatedFloats[1] / myAllMaxSpeed;
    case E2::HALTING_DURATION_MEAN:
        {
            SUMOReal val = myAggregatedFloats[2];
            if(val>MSGlobals::gTimeToGridlock) {
                return 1;
            } else {
                return val / (SUMOReal) MSGlobals::gTimeToGridlock;
            }
        }
    default:
        throw 1;
    }
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


const MSEdge&
GUILaneWrapper::getMSEdge() const
{
    return myLane.edge();
}


Boundary
GUILaneWrapper::getCenteringBoundary() const
{
	Boundary b;
	b.add(_begin);
	b.add(_end);
	b.grow(20);
	return b;
}



#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

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

    while(!toProc.empty()) {
        std::pair<GUILaneWrapper*, SUMOReal> laneAndDist =
            toProc.back();
        toProc.pop_back();
        if(laneAndDist.second<minDist||
                (laneAndDist.second<maxDist&&laneAndDist.first->maxSpeed()<maxSpeed) ) {
            selected.push_back(laneAndDist.first);

            const MSEdge &e = laneAndDist.first->getMSEdge();
            std::vector<MSEdge*> followingEdges = e.getFollowingEdges();
            std::vector<MSEdge*> incomingEdges = e.getIncomingEdges();
            copy(incomingEdges.begin(), incomingEdges.end(),
                back_inserter(followingEdges));
            for(std::vector<MSEdge*>::iterator i=followingEdges.begin(); i!=followingEdges.end(); ++i) {
                std::vector<MSLane*> *lanes = (*i)->getLanes();
                for(std::vector<MSLane*>::iterator j=lanes->begin(); j!=lanes->end(); ++j) {
                    if(find(selected.begin(), selected.end(), &static_cast<GUIEdge*>(*i)->getLaneGeometry(*j))==selected.end()) {
                        toProc.push_back(std::pair<GUILaneWrapper*, SUMOReal>(
                            &static_cast<GUIEdge*>(*i)->getLaneGeometry(*j),
                            laneAndDist.second+laneAndDist.first->getLength()));
                    }
                }
            }
        }
    }

    for(std::vector<GUILaneWrapper*>::iterator k=selected.begin(); k!=selected.end(); ++k) {
        gSelected.select((*k)->getType(), (*k)->getGlID());
    }

    const Position2DVector &shape = getShape();
    Position2D initPos = shape.positionAtLengthPosition(getLength()/(SUMOReal) 2.0);
    Position2DVector poly;
    for(SUMOReal i=0; i<360; i += 40) {
        SUMOReal random1 = SUMOReal( rand() ) /
            ( static_cast<SUMOReal>(RAND_MAX) + 1);
        SUMOReal random2 = SUMOReal( rand() ) /
            ( static_cast<SUMOReal>(RAND_MAX) + 1);
        Position2D p = initPos;
        p.add(sin(i)*30+random1*20, cos(i)*30+random1*20);
        poly.push_back(p);
    }
    /*
    GUINet::getInstance()->addPoly("bla", "bla", RGBColor(1, 0.7, 0));
    Polygon2D *ptr = MSNet::getInstance()->poly_dic["bla"];
    ptr->addPolyPosition(poly);
    */
}


SUMOReal
GUILaneWrapper::firstWaitingTime() const
{
    return myLane.myVehicles.size()==0
        ? 0
        : (SUMOReal) (*(myLane.myVehicles.end()-1))->getWaitingTime();
}


SUMOReal
GUILaneWrapper::myMagic() const
{
    return (SUMOReal) myLane.myVehicles.size() * (SUMOReal) 5. / myLane.length();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


