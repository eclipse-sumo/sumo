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

#include <string>
#include <iostream> // !!!
#include <utility>
#include <utils/qutils/NewQMutex.h>
#include <microsim/MSLane.h>
#include <utils/geom/Position2D.h>
#include <microsim/MSNet.h>
#include <gui/GUISUMOAbstractView.h>
#include "GUILaneWrapper.h"
#include <gui/popup/QGLObjectPopupMenu.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <gui/popup/QGLObjectPopupMenuItem.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
double GUILaneWrapper::myAllMaxSpeed = 0;

const char * const
GUILaneWrapper::myTableItems[] =
{
    "length", "maxspeed", 0
};

const TableType
GUILaneWrapper::myTableItemTypes[] =
{
    TT_DOUBLE, TT_DOUBLE
};




/* =========================================================================
 * method definitions
 * ======================================================================= */
GUILaneWrapper::GUILaneWrapper( GUIGlObjectStorage &idStorage,
                               MSLane &lane,
                               double x1, double y1, double x2, double y2)
    : GUIGlObject(idStorage, string("lane:")+lane.id()),
    myLane(lane)
{
    double length = getLength();
    _begin = Position2D(x1, y1);
    _end = Position2D(x2, y2);
    _direction = Position2D((x1-x2)/length, (y1-y2)/length);
    _rotation = atan2((x2-x1), (y1-y2))*180/3.14159265;
    // also the virtual length is set in here
    _visLength = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    // check maximum speed
    if(myAllMaxSpeed<lane.maxSpeed()) {
        myAllMaxSpeed = lane.maxSpeed();
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
GUILaneWrapper::getPopUpMenu(GUIApplicationWindow *app,
                             GUISUMOAbstractView *parent)
{
    int id;
    QGLObjectPopupMenu *ret =
        new QGLObjectPopupMenu(app, parent, this);
    // insert name
    id = ret->insertItem(
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
    ret->insertSeparator();
    id = ret->insertItem("Close");
    ret->setItemEnabled(id, FALSE);
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


void
GUILaneWrapper::fillTableParameter(double *parameter) const
{
    parameter[0] = myLane.length();
    parameter[1] = myLane.maxSpeed();
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




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILaneWrapper.icc"
//#endif

// Local Variables:
// mode:C++
// End:


