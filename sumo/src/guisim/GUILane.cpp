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
// Revision 1.21  2003/11/26 10:58:30  dkrajzew
// messages from the simulation are now also passed to the message handler
//
// Revision 1.20  2003/11/20 14:40:26  dkrajzew
// push() debugged; dead code removed
//
// Revision 1.19  2003/11/20 13:23:43  dkrajzew
// detector-related debugging
//
// Revision 1.18  2003/11/18 14:31:00  dkrajzew
// usage of a colon instead of a dot patched
//
// Revision 1.17  2003/11/12 14:01:08  dkrajzew
// MSLink-members are now secured from the outer world
//
// Revision 1.16  2003/10/27 10:48:52  dkrajzew
// keeping the pointer to a deleted vehicle - bug patched
//
// Revision 1.15  2003/10/22 15:43:49  dkrajzew
// further work on a correct deletion of vehicles articipating in an accident
//
// Revision 1.14  2003/10/22 07:07:06  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.13  2003/10/14 14:18:43  dkrajzew
// false order of deletion and reading from an object patched
//
// Revision 1.12  2003/10/06 07:39:44  dkrajzew
// MSLane::push changed due to some inproper Vissim-behaviour; now removes a vehicle and reports an error if push fails
//
// Revision 1.11  2003/09/22 12:38:24  dkrajzew
// more verbose output to non-empty-vehBuffer - exception added
//
// Revision 1.10  2003/09/05 14:59:54  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.9  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.8  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.7  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
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
#include <utils/common/MsgHandler.h>
#include <microsim/MSNet.h>
#include "GUILane.h"
#include "GUIVehicle.h"
#include <gui/GUIGlObjectStorage.h>
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
GUILane::moveNonCritical()
{
    _lock.lock();//Display();
    MSLane::moveNonCritical();
    _lock.unlock();//Display();
}


void
GUILane::moveCritical()
{
    _lock.lock();//Display();
    MSLane::moveCritical();
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
        DEBUG_OUT << MSNet::globaltime << ":Push Failed on Lane:" << myID << endl;
	    DEBUG_OUT << myVehBuffer->id() << ", " << myVehBuffer->pos() << ", " << myVehBuffer->speed() << endl;
	    DEBUG_OUT << veh->id() << ", " << veh->pos() << ", " << veh->speed() << endl;
    }
#endif

    // Insert vehicle only if it's destination isn't reached.
    if( myVehBuffer != 0 ) {
        MsgHandler::getWarningInstance()->inform(
            string("Vehicle '") + veh->id()
            + string("' removed due to a collision on push!\n")
            + string("  Lane: '") + id() + string("' Previous vehicle: '")
            + myVehBuffer->id() + string("'."));
        veh->onTripEnd(*this);
        static_cast<GUIVehicle*>(veh)->setRemoved();
        static_cast<GUINet*>(MSNet::getInstance())->getIDStorage().remove(
            static_cast<GUIVehicle*>(veh)->getGlID());
        // maybe the vehicle is being tracked; mark as not within the simulation any longer
        _lock.unlock();//Display();
        return true;
    }
    // check whether the vehicle has ended his route
    if ( ! veh->destReached( myEdge ) ) { // adjusts vehicles routeIterator
        myVehBuffer = veh;
        veh->enterLaneAtMove( this );
        double pspeed = veh->speed();
        double oldPos = veh->pos() - veh->speed() * MSNet::deltaT();
        veh->workOnMoveReminders( oldPos, veh->pos(), pspeed );
        veh->_assertPos();
        _lock.unlock();//Display();
        setApproaching(veh->pos(), veh);
        return false;
    } else {
        veh->onTripEnd(*this);
		static_cast<GUIVehicle*>(veh)->setRemoved();
        static_cast<GUINet*>(MSNet::getInstance())->getIDStorage().remove(
            static_cast<GUIVehicle*>(veh)->getGlID());
        resetApproacherDistance();
        _lock.unlock();//Display();
        return true;
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
    MSLane::swapAfterLaneChange();
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
GUILane::buildLaneWrapper(GUIGlObjectStorage &idStorage, bool allowAggregation)
{
    return new GUILaneWrapper(idStorage, *this, myShape, allowAggregation);
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILane.icc"
//#endif

// Local Variables:
// mode:C++
// End:


