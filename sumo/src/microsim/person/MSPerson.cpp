/***************************************************************************
                          MSPerson.h
			  THe class for modelling person-movements
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.1  2006/09/18 10:04:43  dkrajzew
// first steps towards a person-based multimodal simulation
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:18  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.3  2001/11/15 17:12:13  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.2  2001/11/14 10:49:06  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:14:44  traffic
// new extension
//
// Revision 1.1  2001/10/23 09:32:25  traffic
// person route implementation
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
#include <vector>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/person/MSPerson.h>
#include <microsim/person/MSPersonControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>


using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
MSPerson::MSPersonStage::MSPersonStage(const MSEdge &destination)
    : m_pDestination(destination)
{
}


MSPerson::MSPersonStage::~MSPersonStage()
{
}


const MSEdge &
MSPerson::MSPersonStage::getDestination() const
{
    return m_pDestination;
}




MSPerson::MSPersonStage_Walking::MSPersonStage_Walking(const MSEdge &destination,
                                                       SUMOTime walkingTime)
    : MSPersonStage(destination), m_uiWalkingTime(walkingTime)
{
}


MSPerson::MSPersonStage_Walking::~MSPersonStage_Walking()
{
}


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






MSPerson::MSPersonStage_PublicVehicle::MSPersonStage_PublicVehicle(const MSEdge &destination,
                                                                   const string &lineId)
    : MSPersonStage(destination), m_LineId(lineId)
{
}


MSPerson::MSPersonStage_PublicVehicle::~MSPersonStage_PublicVehicle()
{
}


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





MSPerson::MSPersonStage_PrivateVehicle::MSPersonStage_PrivateVehicle(
        const MSEdge &destination,
        const std::string &routeId, const std::string &vehicleId,
        const std::string &vehicleType)
    : MSPersonStage(destination), m_RouteId(routeId),
    m_VehicleId(vehicleId), m_VehicleType(vehicleType)
{
}


MSPerson::MSPersonStage_PrivateVehicle::~MSPersonStage_PrivateVehicle()
{
}


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






MSPerson::MSPersonStage_Waiting::MSPersonStage_Waiting(const MSEdge &destination,
                                                       SUMOTime waitingTime)
    : MSPersonStage(destination), m_uiWaitingTime(waitingTime)
{
}


MSPerson::MSPersonStage_Waiting::~MSPersonStage_Waiting()
{
}


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


MSPerson::DictType MSPerson::myDict;


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
    return *m_pStep;
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
