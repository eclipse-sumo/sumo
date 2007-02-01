/****************************************************************************/
/// @file    MSPerson.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id: $
///
// missing_desc
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/person/MSPerson.h>
#include <microsim/person/MSPersonControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>

// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage::MSPersonStage(const MSEdge &destination)
        : m_pDestination(destination)
{}


MSPerson::MSPersonStage::~MSPersonStage()
{}


const MSEdge &
MSPerson::MSPersonStage::getDestination() const
{
    return m_pDestination;
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Walking - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Walking::MSPersonStage_Walking(const MSEdge &destination,
        SUMOTime walkingTime)
        : MSPersonStage(destination), m_uiWalkingTime(walkingTime)
{}


MSPerson::MSPersonStage_Walking::~MSPersonStage_Walking()
{}


SUMOTime
MSPerson::MSPersonStage_Walking::getWalkingTime()
{
    return m_uiWalkingTime;
}


void
MSPerson::MSPersonStage_Walking::proceed(MSNet *net,
        MSPerson *person, SUMOTime now,
        MSEdge *previousEdge)
{
//!!!    myWalking.add(now + m_uiWalkingTime, person);
}



/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_PublicVehicle - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_PublicVehicle::MSPersonStage_PublicVehicle(const MSEdge &destination,
        const string &lineId)
        : MSPersonStage(destination), m_LineId(lineId)
{}


MSPerson::MSPersonStage_PublicVehicle::~MSPersonStage_PublicVehicle()
{}


const string &
MSPerson::MSPersonStage_PublicVehicle::getLineId() const
{
    return m_LineId;
}

void
MSPerson::MSPersonStage_PublicVehicle::proceed(MSNet *net, MSPerson *person,
        SUMOTime now, MSEdge *previousEdge)
{
//!!!!    previousEdge->addWaitingForPublicVehicle(person, m_LineId);
}



/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_PrivateVehicle - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_PrivateVehicle::MSPersonStage_PrivateVehicle(
    const MSEdge &destination,
    const std::string &routeId, const std::string &vehicleId,
    const std::string &vehicleType)
        : MSPersonStage(destination), m_RouteId(routeId),
        m_VehicleId(vehicleId), m_VehicleType(vehicleType)
{}


MSPerson::MSPersonStage_PrivateVehicle::~MSPersonStage_PrivateVehicle()
{}


const std::string &
MSPerson::MSPersonStage_PrivateVehicle::getRouteId() const
{
    return m_RouteId;
}


const std::string &
MSPerson::MSPersonStage_PrivateVehicle::getVehicleId() const
{
    return m_VehicleId;
}


const std::string &
MSPerson::MSPersonStage_PrivateVehicle::getVehicleType() const
{
    return m_VehicleType;
}


void
MSPerson::MSPersonStage_PrivateVehicle::proceed(MSNet *net, MSPerson *person,
        SUMOTime now, MSEdge *previousEdge)
{
    /*!!!
    MSVehicle *vehicle = new MSVehicle(m_VehicleId,  (MSNet::Route*) MSNet::routeDict(m_RouteId), now, MSVehicleType::dictionary(m_VehicleType));
    vehicle->addPerson(person, m_pDestination);
    net->myEmitter->addStarting(vehicle);
    */
}


/* -------------------------------------------------------------------------
 * MSPerson::MSPersonStage_Waiting - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Waiting::MSPersonStage_Waiting(const MSEdge &destination,
        SUMOTime waitingTime)
        : MSPersonStage(destination), m_uiWaitingTime(waitingTime)
{}


MSPerson::MSPersonStage_Waiting::~MSPersonStage_Waiting()
{}


SUMOTime
MSPerson::MSPersonStage_Waiting::getWaitingTime() const
{
    return m_uiWaitingTime;
}


void
MSPerson::MSPersonStage_Waiting::proceed(MSNet *net, MSPerson *person,
        SUMOTime now, MSEdge *previousEdge)
{
//!!!!    net->myPersons->add(now + m_uiWaitingTime, person);
}


/* -------------------------------------------------------------------------
 * MSPerson - static variables
 * ----------------------------------------------------------------------- */
MSPerson::DictType MSPerson::myDict;


/* -------------------------------------------------------------------------
 * MSPerson - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPerson(const std::string &id, MSPersonRoute *route)
        : m_uiTravelTime(0),
        m_Id(id),
        m_pRoute(route)
{
    m_pStep = m_pRoute->begin();
}


MSPerson::~MSPerson()
{
    delete m_pRoute;
}


const MSPerson::MSPersonStage &
MSPerson::getCurrentStage() const
{
    return **m_pStep;
}


void
MSPerson::proceed(MSNet *net, SUMOTime time)
{
    /*!!!
    MSEdge *arrivedAt = m_pStep->getDestination();
    m_pStep++;
    if(m_pStep==m_pRoute->end()) return;
    (*m_pStep).proceed(net, this, time, arrivedAt);
    */
}


bool
MSPerson::endReached() const
{
    return m_pStep == m_pRoute->end();
}


bool
MSPerson::dictionary(const std::string &id, MSPerson* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSPerson*
MSPerson::dictionary(const std::string &id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}



/****************************************************************************/

