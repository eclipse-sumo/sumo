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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "GUILaneStateReporter.h"
#include <string>
#include <iostream> // !!!
#include <utility>
#include <utils/qutils/NewQMutex.h>
#include <microsim/MSLane.h>
#include <utils/geom/Position2DVector.h>
#include <microsim/MSNet.h>
#include <gui/GUISUMOAbstractView.h>
#include "GUILaneWrapper.h"
#include <utils/convert/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUINet.h>
#include <gui/popup/QGLObjectPopupMenuItem.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <gui/popup/QGLObjectPopupMenu.h>



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
double GUILaneWrapper::myAllMaxSpeed = 0;
size_t GUILaneWrapper::myAggregationSizes[] = {
    60, 300, 900
};



/* =========================================================================
 * method definitions
 * ======================================================================= */





GUILaneWrapper::GUILaneWrapper(GUIGlObjectStorage &idStorage,
                               MSLane &lane, const Position2DVector &shape,
                               bool allowAggregation)
    : GUIGlObject(idStorage, string("lane:")+lane.id()),
    myLane(lane), myShape(shape), myAggregatedValues(0)
{
    double x1 = shape.at(0).x();
    double y1 = shape.at(0).y();
    double x2 = shape.at(shape.size()-1).x();
    double y2 = shape.at(shape.size()-1).y();
    double length = getLength();
    _begin = Position2D(x1, y1);
    _end = Position2D(x2, y2);
    _direction = Position2D((x1-x2)/length, (y1-y2)/length);
    _rotation = atan2((x2-x1), (y1-y2))*180.0/3.14159265;
    // also the virtual length is set in here
    _visLength = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    // check maximum speed
    if(myAllMaxSpeed<lane.maxSpeed()) {
        myAllMaxSpeed = lane.maxSpeed();
    }
    // build the storage for lane wrappers when wished
    if(allowAggregation) {
        buildAggregatedValuesStorage();
    }
    //
    myShapeRotations.reserve(myShape.size()-1);
    myShapeLengths.reserve(myShape.size()-1);
    for(size_t i=0; i<myShape.size()-1; i++) {
        const Position2D &f = myShape.at(i);
        const Position2D &s = myShape.at(i+1);
        myShapeLengths.push_back(GeomHelper::distance(f, s));
        myShapeRotations.push_back(atan2((s.x()-f.x()), (f.y()-s.y()))*180.0/3.14159265);
    }
}


GUILaneWrapper::~GUILaneWrapper()
{
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


double
GUILaneWrapper::getRotation() const
{
    return _rotation;
}


double
GUILaneWrapper::getLength() const
{
    return myLane.myLength;
}


double
GUILaneWrapper::visLength() const
{
    return _visLength;
}


MSEdge::EdgeBasicFunction
GUILaneWrapper::getPurpose() const
{
    return myLane.myEdge->getPurpose();
}


double
GUILaneWrapper::maxSpeed() const
{
    return myLane.maxSpeed();
}


double
GUILaneWrapper::getOverallMaxSpeed()
{
    return myAllMaxSpeed;
}


bool
GUILaneWrapper::forLane(const MSLane &lane) const
{
    return (&myLane)==(&lane);
}


QGLObjectPopupMenu *
GUILaneWrapper::getPopUpMenu(GUIApplicationWindow &app,
                             GUISUMOAbstractView &parent)
{
    QGLObjectPopupMenu *ret = new QGLObjectPopupMenu(app, parent, *this);
    int id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add view options
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->insertSeparator();
    id = ret->insertItem("Show Parameter", ret, SLOT(showPars()));
    ret->setItemEnabled(id, TRUE);
    // add views adding options
    ret->insertSeparator();
    id = ret->insertItem("Open ValueTracker");
    ret->setItemEnabled(id, FALSE);
    // add simulation options
/*    ret->insertSeparator();
    id = ret->insertItem("Close");
    ret->setItemEnabled(id, FALSE);*/
    return ret;
}


GUIParameterTableWindow *
GUILaneWrapper::getParameterWindow(GUIApplicationWindow &app,
                                   GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this);
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

/*
const char * const
GUILaneWrapper::getTableItem(size_t pos) const
{
    return myTableItems[pos];
}


TableType
GUILaneWrapper::getTableType(size_t pos) const
{
    return myTableItemTypes[pos];
}


double
GUILaneWrapper::getTableParameter(size_t pos) const
{
    switch(pos) {
    case 0:
        return myLane.length();
    case 1:
        return myLane.maxSpeed();
    default:
        throw 1;
    }
}
*/

/*
void
GUILaneWrapper::fillTableParameter(double *parameter) const
{
    parameter[0] = myLane.length();
    parameter[1] = myLane.maxSpeed();
}
*/

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
    //
    myAggregatedValues = new LoggedValue_TimeFloating<double>*[1];
    for(size_t i=0; i<1; i++) {
        myAggregatedValues[i] =
            new LoggedValue_TimeFloating<double>(myAggregationSizes[i]);
        string id = string("*") + myLane.id() + toString<size_t>(i);
        new GUILaneStateReporter(myAggregatedValues[i],
            id, &myLane, 60,
            static_cast<GUINet*>(MSNet::getInstance())->getAggregatedValueBoundery());
    }
}


double
GUILaneWrapper::getAggregatedDensity(size_t aggregationPosition) const
{
    return myAggregatedValues[aggregationPosition]->getAbs();
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILaneWrapper.icc"
//#endif

// Local Variables:
// mode:C++
// End:


