//---------------------------------------------------------------------------//
//                        GUISourceLane.cpp -
//  A grid of edges for faster drawing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 25 Nov 2002
//  copyright            : (C) 2001 by DLR http://ivf.dlr.de/
//  author               : Daniel Krajzewicz
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
// Revision 1.9  2004/08/02 11:57:34  dkrajzew
// debugging
//
// Revision 1.8  2004/07/02 08:52:49  dkrajzew
// numerical id added (for online-routing)
//
// Revision 1.7  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.6  2004/02/10 07:07:47  dkrajzew
// removed some dead code
//
// Revision 1.5  2003/09/05 14:59:54  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.4  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.3  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.2  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.1  2003/02/07 10:39:17  dkrajzew
// updated
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
#include <utils/foxtools/FXMutex.h>
#include <microsim/MSLane.h>
#include <utils/geom/Position2D.h>
#include <microsim/MSNet.h>
#include "GUILaneWrapper.h"
#include "GUISourceLane.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUISourceLane::GUISourceLane(MSNet &net, std::string id,
                             double maxSpeed, double length,
                             MSEdge* edge, size_t numericalID,
                             const Position2DVector &shape )
    : MSSourceLane(net, id, maxSpeed, length, edge, numericalID),
    myShape(shape)
{
}


GUISourceLane::~GUISourceLane()
{
    // just to quit cleanly on a failure
    if(_lock.locked()) {
        _lock.unlock();
    }
}


void
GUISourceLane::moveNonCritical()
{
    _lock.lock();//Display();
    MSSourceLane::moveNonCritical(/*firstNeighLane, lastNeighLane*/);
    _lock.unlock();//Display();
}


void
GUISourceLane::moveCritical()
{
    _lock.lock();//Display();
    MSSourceLane::moveCritical(/*firstNeighLane, lastNeighLane*/);
    _lock.unlock();//Display();
}


void
GUISourceLane::setCritical()
{
    _lock.lock();//Display();
    MSSourceLane::setCritical();
    _lock.unlock();//Display();
}


bool
GUISourceLane::emit( MSVehicle& newVeh )
{
    _lock.lock();//Display();
    bool ret = MSSourceLane::emit(newVeh);
    _lock.unlock();//Display();
    return ret;
}


bool
GUISourceLane::isEmissionSuccess( MSVehicle* aVehicle )
{
    _lock.lock();//Display();
    bool ret = MSSourceLane::isEmissionSuccess(aVehicle);
    _lock.unlock();//Display();
    return ret;
}


bool
GUISourceLane::push( MSVehicle*  )
{
    throw 1;
}


void
GUISourceLane::releaseVehicles()
{
    _lock.unlock();
}



const MSSourceLane::VehCont &
GUISourceLane::getVehiclesSecure()
{
    _lock.lock();
    return myVehicles;
}


void
GUISourceLane::swapAfterLaneChange()
{
    _lock.lock();
    MSLane::swapAfterLaneChange();
    _lock.unlock();
}


void
GUISourceLane::integrateNewVehicle()
{
    _lock.lock();
    MSLane::integrateNewVehicle();
    _lock.unlock();
}


GUILaneWrapper *
GUISourceLane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


