//---------------------------------------------------------------------------//
//                        GUIInternalLane.cpp -
//  Lane within junctions, derived from the normal lane
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 04.09.2003
//  copyright            : (C) 2003 by DLR http://ivf.dlr.de/
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
// Revision 1.5  2003/10/22 15:43:49  dkrajzew
// further work on a correct deletion of vehicles articipating in an accident
//
// Revision 1.4  2003/10/22 11:25:04  dkrajzew
// removeing from lane corrected
//
// Revision 1.3  2003/10/22 07:07:06  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.2  2003/10/06 07:39:44  dkrajzew
// MSLane::push changed due to some inproper Vissim-behaviour; now removes a vehicle and reports an error if push fails
//
// Revision 1.1  2003/09/05 15:02:47  dkrajzew
// first steps for reading of internal lanes
//
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
#include "GUINet.h"
#include "GUIVehicle.h"
#include "GUILaneWrapper.h"
#include "GUIInternalLane.h"


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
GUIInternalLane::GUIInternalLane(MSNet &net, std::string id,
                             double maxSpeed, double length,
                             MSEdge* edge, const Position2DVector &shape )
    : MSInternalLane(net, id, maxSpeed, length, edge), myShape(shape)
{
}


GUIInternalLane::~GUIInternalLane()
{
}

/*
void
GUIInternalLane::moveNonCriticalSingle()
{
    _lock.lock();//Display();
    MSInternalLane::moveNonCriticalSingle();
    _lock.unlock();//Display();
}


void
GUIInternalLane::moveCriticalSingle()
{
    _lock.lock();//Display();
    MSInternalLane::moveCriticalSingle();
    _lock.unlock();//Display();
}


void
GUIInternalLane::moveNonCriticalMulti()
{
    _lock.lock();//Display();
    MSInternalLane::moveNonCriticalMulti();
    _lock.unlock();//Display();
}


void
GUIInternalLane::moveCriticalMulti()
{
    _lock.lock();//Display();
    MSInternalLane::moveCriticalMulti();
    _lock.unlock();//Display();
}

*/
void
GUIInternalLane::moveNonCritical(/*const MSEdge::LaneCont::const_iterator &firstNeighLane,
                               const MSEdge::LaneCont::const_iterator &lastNeighLane */)
{
    _lock.lock();//Display();
    MSInternalLane::moveNonCritical(/*firstNeighLane, lastNeighLane*/);
    _lock.unlock();//Display();
}


void
GUIInternalLane::moveCritical(/*const MSEdge::LaneCont::const_iterator &firstNeighLane,
                            const MSEdge::LaneCont::const_iterator &lastNeighLane */)
{
    _lock.lock();//Display();
    MSInternalLane::moveCritical(/*firstNeighLane, lastNeighLane*/);
    _lock.unlock();//Display();
}


void
GUIInternalLane::setCritical()
{
    _lock.lock();//Display();
    MSInternalLane::setCritical();
    _lock.unlock();//Display();
}





bool
GUIInternalLane::emit( MSVehicle& newVeh )
{
    _lock.lock();//Display();
    bool ret = MSInternalLane::emit(newVeh);
    _lock.unlock();//Display();
    return ret;
}


bool
GUIInternalLane::isEmissionSuccess( MSVehicle* aVehicle )
{
    _lock.lock();//Display();
    bool ret = MSInternalLane::isEmissionSuccess(aVehicle);
    _lock.unlock();//Display();
    return ret;
}


bool
GUIInternalLane::push( MSVehicle* veh )
{
    _lock.lock();//Display();
#ifdef ABS_DEBUG
    if(myVehBuffer!=0) {
        DEBUG_OUT << MSNet::globaltime << ":Push Failed on Lane:" << myID << endl;
	    DEBUG_OUT << myVehBuffer->id() << ", " << myVehBuffer->pos() << ", " << myVehBuffer->speed() << endl;
	    DEBUG_OUT << veh->id() << ", " << veh->pos() << ", " << veh->speed() << endl;
    }
#endif

    // Insert vehicle only if it's destination isn't reached.
    if( myVehBuffer != 0 ) {
        if(myVehBuffer->pos()<veh->pos()) {
            cout << "vehicle '" << myVehBuffer->id() << "' removed!";
            myVehBuffer->leaveLaneAtLaneChange();
    		static_cast<GUIVehicle*>(myVehBuffer)->setRemoved();
            static_cast<GUINet*>(MSNet::getInstance())->_idStorage.remove(
                static_cast<GUIVehicle*>(myVehBuffer)->getGlID());
        } else {
            cout << "vehicle '" << veh->id() << "' removed!";
            veh->leaveLaneAtLaneChange();
    		static_cast<GUIVehicle*>(veh)->setRemoved();
            static_cast<GUINet*>(MSNet::getInstance())->_idStorage.remove(
                static_cast<GUIVehicle*>(veh)->getGlID());
    		// maybe the vehicle is being tracked; mark as not within the simulation any longer
            _lock.unlock();//Display();
            return true;
        }
    }
    myVehBuffer = veh;
    veh->enterLaneAtMove( this );
    veh->_assertPos();
    _lock.unlock();//Display();
    return false;
}


void
GUIInternalLane::releaseVehicles()
{
    _lock.unlock();
}



const MSInternalLane::VehCont &
GUIInternalLane::getVehiclesSecure()
{
    _lock.lock();
    return myVehicles;
}


void
GUIInternalLane::swapAfterLaneChange()
{
    _lock.lock();
    MSLane::swapAfterLaneChange();
    _lock.unlock();
}


void
GUIInternalLane::integrateNewVehicle()
{
    _lock.lock();
    MSLane::integrateNewVehicle();
    _lock.unlock();
}


GUILaneWrapper *
GUIInternalLane::buildLaneWrapper(GUIGlObjectStorage &idStorage,
                                bool allowAggregation)
{
    return new GUILaneWrapper(idStorage, *this, myShape, allowAggregation);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIInternalLane.icc"
//#endif

// Local Variables:
// mode:C++
// End:


