//---------------------------------------------------------------------------//
//                        GUILane.cpp -
//  A grid of edges for faster drawing
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
// Revision 1.6  2003/07/07 08:14:48  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.5  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease the search for further couts which must be redirected to the messaaging subsystem
//
// Revision 1.4  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.3  2003/04/14 08:27:17  dkrajzew
// new globject concept implemented
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
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
#include <gui/GUIGlObjectStorage.h>
#include "GUIVehicle.h"
#include "GUILane.h"
#include "GUINet.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUILane::GUILane(MSNet &net, std::string id, double maxSpeed, double length,
                 MSEdge* edge, const Position2DVector &shape )
    : MSLane(net, id, maxSpeed, length, edge), myShape(shape)
{
}


GUILane::~GUILane()
{
}


void
GUILane::moveNonCriticalSingle()
{
    _lock.lock();//Display();
    MSLane::moveNonCriticalSingle();
    _lock.unlock();//Display();
}


void
GUILane::moveCriticalSingle()
{
    _lock.lock();//Display();
    MSLane::moveCriticalSingle();
    _lock.unlock();//Display();
}


void
GUILane::moveNonCriticalMulti()
{
    _lock.lock();//Display();
    MSLane::moveNonCriticalMulti();
    _lock.unlock();//Display();
}


void
GUILane::moveCriticalMulti()
{
    _lock.lock();//Display();
    MSLane::moveCriticalMulti();
    _lock.unlock();//Display();
}


void
GUILane::moveNonCriticalMulti(MSEdge::LaneCont::const_iterator firstNeighLane,
                               MSEdge::LaneCont::const_iterator lastNeighLane )
{
    _lock.lock();//Display();
    MSLane::moveNonCriticalMulti(firstNeighLane, lastNeighLane);
    _lock.unlock();//Display();
}


void
GUILane::moveCriticalMulti(MSEdge::LaneCont::const_iterator firstNeighLane,
                               MSEdge::LaneCont::const_iterator lastNeighLane )
{
    _lock.lock();//Display();
    MSLane::moveCriticalMulti(firstNeighLane, lastNeighLane);
    _lock.unlock();//Display();
}


void
GUILane::setCritical()
{
    _lock.lock();//Display();
    MSLane::setCritical();
    _lock.unlock();//Display();
}



bool
GUILane::emit( MSVehicle& newVeh )
{
    _lock.lock();//Display();
    bool ret = MSLane::emit(newVeh);
    _lock.unlock();//Display();
    return ret;
}


bool
GUILane::isEmissionSuccess( MSVehicle* aVehicle )
{
    _lock.lock();//Display();
    bool ret = MSLane::isEmissionSuccess(aVehicle);
    _lock.unlock();//Display();
    return ret;
}


bool
GUILane::push( MSVehicle* veh )
{
    _lock.lock();//Display();
#ifdef ABS_DEBUG
    if(myVehBuffer!=0) {
	    DEBUG_OUT << "Push Failed on Lane:" << myID << endl;
	    DEBUG_OUT << myVehBuffer->id() << ", " << myVehBuffer->pos() << ", " << myVehBuffer->speed() << endl;
	    DEBUG_OUT << veh->id() << ", " << veh->pos() << ", " << veh->speed() << endl;
    }
#endif

    // Insert vehicle only if it's destination isn't reached.
    assert( myVehBuffer == 0 );
    if ( ! veh->destReached( myEdge ) ) { // adjusts vehicles routeIterator
        myVehBuffer = veh;
        veh->enterLaneAtMove( this );
        veh->_assertPos();
        _lock.unlock();//Display();
        return false;
    }
    else {
		static_cast<GUIVehicle*>(veh)->setRemoved();
        static_cast<GUINet*>(MSNet::getInstance())->_idStorage.remove(
            static_cast<GUIVehicle*>(veh)->getGlID());
		// maybe the vehicle is being tracked; mark as not within the simulation any longer
        _lock.unlock();//Display();
        return true;
        // TODO
        // This part has to be discussed, quick an dirty solution:
        // Destination reached. Vehicle vanishes.
        // maybe introduce a vehicle state ...
    }
}


void
GUILane::releaseVehicles()
{
    _lock.unlock();
}



const MSLane::VehCont &
GUILane::getVehiclesSecure()
{
    _lock.lock();
    return myVehicles;
}


void
GUILane::swapAfterLaneChange()
{
    _lock.lock();
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
    _lock.unlock();
}


void
GUILane::integrateNewVehicle()
{
    _lock.lock();
    MSLane::integrateNewVehicle();
    _lock.unlock();
}


GUILaneWrapper *
GUILane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILane.icc"
//#endif

// Local Variables:
// mode:C++
// End:


