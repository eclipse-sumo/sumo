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
// Revision 1.23  2006/01/31 10:55:27  dkrajzew
// unneeded inclusions removed
//
// Revision 1.22  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.21  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.20  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.19  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.18  2005/02/01 10:10:39  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.17  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.16  2004/08/02 11:57:34  dkrajzew
// debugging
//
// Revision 1.15  2004/07/02 08:52:49  dkrajzew
// numerical id added (for online-routing)
//
// Revision 1.14  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.13  2003/12/12 12:36:00  dkrajzew
// proper usage of lane states applied; scheduling of vehicles into the beamer on push failures added
//
// Revision 1.12  2003/12/11 06:24:55  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.11  2003/12/04 13:35:02  dkrajzew
// correct usage of detectors when moving over more than a single edge applied
//
// Revision 1.10  2003/11/26 10:58:30  dkrajzew
// messages from the simulation are now also passed to the message handler
//
// Revision 1.9  2003/11/20 14:40:26  dkrajzew
// push() debugged; dead code removed
//
// Revision 1.8  2003/11/20 13:23:43  dkrajzew
// detector-related debugging
//
// Revision 1.7  2003/11/18 14:31:00  dkrajzew
// usage of a colon instead of a dot patched
//
// Revision 1.6  2003/11/12 14:01:08  dkrajzew
// MSLink-members are now secured from the outer world
//
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

#include <string>
#include <utility>
#include <utils/foxtools/FXMutex.h>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position2D.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSNet.h>
#include "GUINet.h"
#include "GUIVehicle.h"
#include "GUILaneWrapper.h"
#include "GUIInternalLane.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
GUIInternalLane::GUIInternalLane(/*MSNet &net, */std::string id,
                             SUMOReal maxSpeed, SUMOReal length,
                             MSEdge* edge, size_t numericalID,
                             const Position2DVector &shape )
    : MSInternalLane(/*net, */id, maxSpeed, length, edge, numericalID, shape)
{
}


GUIInternalLane::~GUIInternalLane()
{
    // just to quit cleanly on a failure
    if(_lock.locked()) {
        _lock.unlock();
    }
}


void
GUIInternalLane::moveNonCritical()
{
    _lock.lock();//Display();
    MSInternalLane::moveNonCritical();
    _lock.unlock();//Display();
}


void
GUIInternalLane::moveCritical()
{
    _lock.lock();//Display();
    MSInternalLane::moveCritical();
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
    MSVehicle *last = myVehicles.size()!=0
        ? myVehicles.front()
        : 0;

    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    if( myVehBuffer != 0 || (last!=0 && last->pos() < veh->pos()) ) {
        MSVehicle *prev = myVehBuffer!=0
            ? myVehBuffer : last;
        WRITE_WARNING(string("Vehicle '") + veh->id()+ string("' beamed due to a collision on push!\n")+ string("  Lane: '") + id() + string("', previous vehicle: '")+ prev->id() + string("', time: ")+ toString<SUMOTime>(MSNet::getInstance()->getCurrentTimeStep())+ string("."));
        veh->onTripEnd(/* *this*/);
        veh->removeApproachingInformationOnKill(/*this*/);
        MSVehicleTransfer::getInstance()->addVeh(veh);
//        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        // maybe the vehicle is being tracked; mark as not within the simulation any longer
        _lock.unlock();//Display();
        return true;
    }
    // check whether the vehicle has ended his route
    veh->destReached( myEdge );
    myVehBuffer = veh;
    veh->enterLaneAtMove( this, SPEED2DIST(veh->speed()) - veh->pos() );
    SUMOReal pspeed = veh->speed();
    SUMOReal oldPos = veh->pos() - SPEED2DIST(veh->speed());
    veh->workOnMoveReminders( oldPos, veh->pos(), pspeed );
    veh->_assertPos();
    _lock.unlock();//Display();
//    setApproaching(veh->pos(), veh);
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
GUIInternalLane::buildLaneWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUILaneWrapper(idStorage, *this, myShape);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


