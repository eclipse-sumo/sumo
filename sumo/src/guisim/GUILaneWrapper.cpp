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
#include "GUILaneWrapper.h"


/* =========================================================================
 * static member definitions
 * ======================================================================= */
double GUILaneWrapper::myAllMaxSpeed = 0;


using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
GUILaneWrapper::GUILaneWrapper( MSLane &lane,
                               double x1, double y1, double x2, double y2)
    : GUIGlObject(string("lane:")+lane.id()),
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILaneWrapper.icc"
//#endif

// Local Variables:
// mode:C++
// End:


