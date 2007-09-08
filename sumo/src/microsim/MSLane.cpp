/****************************************************************************/
/// @file    MSLane.cpp
/// @author  Christian Roessel
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// operate.
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSLink.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include "MSVehicleControl.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>

#ifdef GUI_DEBUG
#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>
#endif

#ifdef ABS_DEBUG
#include "MSDebugHelper.h"
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// static member definitions
// ===========================================================================
MSLane::DictType MSLane::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods of MSLane::PosGreater
 * ----------------------------------------------------------------------- */
MSLane::PosGreater::result_type
MSLane::PosGreater::operator()(first_argument_type veh1,
                               second_argument_type veh2) const
{
    return veh1->getPositionOnLane() > veh2->getPositionOnLane();
}


/* -------------------------------------------------------------------------
 * methods of MSLane
 * ----------------------------------------------------------------------- */
MSLane::~MSLane()
{
    for (MSLinkCont::iterator i=myLinks.begin(); i!=myLinks.end(); i++) {
        delete *i;
    }
    // TODO
}

/////////////////////////////////////////////////////////////////////////////

MSLane::MSLane(string id, SUMOReal maxSpeed, SUMOReal length, MSEdge* edge,
               size_t numericalID, const Position2DVector &shape,
               const std::vector<SUMOVehicleClass> &allowed,
               const std::vector<SUMOVehicleClass> &disallowed)  :
        myLastState(10000, 10000),
        myApproaching(0),
        myShape(shape),
        myID(id),
        myNumericalID(numericalID),
        myVehicles(),
        myLength(length),
        myEdge(edge),
        myMaxSpeed(maxSpeed),
        myVehBuffer(0),
        myAllowedClasses(allowed),
        myNotAllowedClasses(disallowed),
        myFirstUnsafe(0)
{
    assert(myMaxSpeed>0);
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::initialize(MSLinkCont* links)
{
    myLinks = *links;
    delete links;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::resetApproacherDistance()
{
    myBackDistance = 100000;
    myApproaching = 0;
}

void
MSLane::resetApproacherDistance(MSVehicle *v)
{
    if (myApproaching!=v) {
        return;
    }
    myBackDistance = 100000;
    myApproaching = 0;
}

void
MSLane::moveNonCritical()
{
    assert(myVehicles.size()!=0);
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    // Set the information about the last vehicle
    myLastState = (*myVehicles.begin())->getState();
    myFirstUnsafe = 0;

    // Move vehicles except first and all vehicles that may reach the end of the lane
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;

    VehCont::iterator neighIt, neighEnd;
    bool useNeigh = false;
    if (myUseDefinition->firstNeigh!=myUseDefinition->lastNeigh) {
        if ((*(myUseDefinition->firstNeigh))->myVehicles.size()!=0) {
            useNeigh = true;
            neighIt = (*(myUseDefinition->firstNeigh))->myVehicles.begin();
            neighEnd = (*(myUseDefinition->firstNeigh))->myVehicles.end();
        }
    }

    for (veh = myVehicles.begin();
            !(*veh)->reachingCritical(myLength) && veh != lastBeforeEnd;
            ++veh,++myFirstUnsafe) {

        // get the leader
        //  ... there must be one, because the first vehicle is moved
        //   using moveCritical ...
        VehCont::const_iterator pred(veh + 1);
        // do we have to regard the neighbor?
        if (useNeigh) {
            // get the neighbor
            while (neighIt!=neighEnd&&(*neighIt)->getPositionOnLane()<(*veh)->getPositionOnLane())  {
                ++neighIt;
            }
            if (neighIt!=neighEnd) {
                // move vehicle regarding the neighbor
                (*veh)->move(this, *pred, *neighIt);
            } else {
                // move vehicle without regarding the neighbor
                (*veh)->move(this, *pred, 0);
                // no further neighbors
                useNeigh = false;
            }
        } else {
            // move vehicle without regarding the neighbor
            (*veh)->move(this, *pred, 0);
        }
        MSVehicle *vehicle = (*veh);
        MSVehicle *predec = (*pred);
        assert(&vehicle->getLane()==this);
        assert(&predec->getLane()==this);
    }
    assert(myVehicles.size()==myUseDefinition->noVehicles);
}


void
MSLane::moveCritical()
{
    assert(myVehicles.size()!=0);
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;
    // Move all next vehicles beside the first
    for (veh=myVehicles.begin()+myFirstUnsafe;veh != lastBeforeEnd; ++veh) {
        VehCont::const_iterator pred(veh + 1);
        (*veh)->moveRegardingCritical(this, *pred, 0);
//        ( *veh )->meanDataMove();
        // Check for timeheadway < deltaT
        // Check for timeheadway < deltaT
        MSVehicle *vehicle = (*veh);
        MSVehicle *predec = (*pred);
        assert(&vehicle->getLane()==this);
        assert(&predec->getLane()==this);
        assert((*veh)->getPositionOnLane() < (*pred)->getPositionOnLane());
        assert((*veh)->getPositionOnLane() <= myLength);
    }
    (*veh)->moveRegardingCritical(this, 0, 0);
//    ( *veh )->meanDataMove();
    assert((*veh)->getPositionOnLane() <= myLength);
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    assert(&(*veh)->getLane()==this);
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::detectCollisions(SUMOTime timestep)
{
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    if (myVehicles.size() < 2) {
        return;
    }

    VehCont::iterator lastVeh = myVehicles.end() - 1;
    for (VehCont::iterator veh = myVehicles.begin();
            veh != lastVeh;) {

        VehCont::iterator pred = veh + 1;
        SUMOReal gap = (*pred)->getPositionOnLane() - (*pred)->getLength() - (*veh)->getPositionOnLane();
#ifdef ABS_DEBUG
        if (debug_globaltime>=21868 && ((*veh)->getID()==debug_searched1||(*veh)->getID()==debug_searched2)) {
            DEBUG_OUT << gap << "\n";
        }
#endif
        if (gap < 0) {
#ifdef ABS_DEBUG
            if (debug_globaltime>debug_searchedtime-5 && ((*veh)->getID()==debug_searched1||(*veh)->getID()==debug_searched2)) {
                int blb = 0;
            }
#endif
            MSVehicle *predV = *pred;
            MSVehicle *vehV = *veh;
            MsgHandler *handler = 0;
            if (!OptionsCont::getOptions().getBool("quit-on-accident")) {
                handler = MsgHandler::getWarningInstance();
            } else {
                handler = MsgHandler::getErrorInstance();
            }
            handler->inform(
                "MSLane::detectCollision: Collision of " + vehV->getID() + " with " + predV->getID() + " on MSLane " + myID +" during timestep " + toString<int>(timestep));
//            DEBUG_OUT << ( *veh )->getID() << ":" << ( *veh )->pos() << ", " << ( *veh )->speed() << "\n";
//            DEBUG_OUT << ( *pred )->getID() << ":" << ( *pred )->pos() << ", " << ( *pred )->speed() << "\n";
            if (OptionsCont::getOptions().getBool("quit-on-accident")) {
                throw ProcessError();
            } else {
                vehV->leaveLaneAtLaneChange();
                vehV->onTripEnd(/* *this*/);
                resetApproacherDistance(); // !!! correct? is it (both lines) really necessary during this simulation part?
                vehV->removeApproachingInformationOnKill(/*this*/);
                MSVehicleTransfer::getInstance()->addVeh(vehV);
            }
            veh = myVehicles.erase(veh); // remove current vehicle
            lastVeh = myVehicles.end() - 1;
            myUseDefinition->noVehicles--;
            myUseDefinition->vehLenSum -= (*veh)->getLength();
            if (veh==myVehicles.end()) {
                break;
            }
        } else {
            ++veh;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emit(MSVehicle& veh)
{
    // Here the emission starts
    if (empty()) {

        return emitTry(veh);
    }

    // Try to emit as last veh. (in driving direction)
    VehCont::iterator leaderIt = myVehicles.begin();
    if (emitTry(veh, leaderIt)) {

        return true;
    }

    // if there is only one veh on this lane, try to
    // emit in front of this veh. (leader becomes follower)
    if (leaderIt + 1 ==  myVehicles.end()) {

        return emitTry(leaderIt, veh);
    }

    // At least two vehicles on lane.
    // iterate over follow/leader -pairs
    VehCont::iterator followIt = leaderIt;
    ++leaderIt;
    for (;;) {

        // try to emit between follower and leader
        if (emitTry(followIt, veh, leaderIt)) {

            return true;
        }

        // if leader is the first veh on this lane, try
        // to emit in front of it.
        if (leaderIt + 1 == myVehicles.end()) {

            return emitTry(leaderIt, veh);
        }

        // iterate
        ++leaderIt;
        ++followIt;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::isEmissionSuccess(MSVehicle* aVehicle,
                          const MSVehicle::State &vstate)
{
//    aVehicle->departLane();
    MSLane::VehCont::iterator predIt =
        find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), aVehicle->getPositionOnLane()));
    if (predIt != myVehicles.end()) {
        MSVehicle* pred = *predIt;
        SUMOReal headWay = aVehicle->timeHeadWayGap(pred->getSpeed());   // !!!??
        SUMOReal gap = MSVehicle::gap(pred->getPositionOnLane(), pred->getLength(), vstate.pos());
        if (gap<headWay) {
            return false;
        }
        SUMOReal vsafe = aVehicle->ffeV(vstate.speed(), gap, pred->getSpeed());
        SUMOReal brakeWay = SPEED2DIST(aVehicle->getSpeedAfterMaxDecel(vsafe));//vstate.speed() - aVehicle->decelSpeed();
        if (vsafe<brakeWay) {
            return false;
        }

        // emit
        myVehicles.insert(predIt, aVehicle);
        myUseDefinition->noVehicles++;
        myUseDefinition->vehLenSum += aVehicle->getLength();
        assert(myVehicles.size()==myUseDefinition->noVehicles);
        return true;
    }
    // emit
    myVehicles.push_back(aVehicle);
    myUseDefinition->vehLenSum += aVehicle->getLength();
    myUseDefinition->noVehicles++;
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry(MSVehicle& veh)
{
    SUMOReal safeSpace =
        myApproaching==0
        ? 0
        : myApproaching->getSecureGap(myApproaching->getSpeed(), veh.getSpeed(), veh.getLength());
    safeSpace = MAX2(safeSpace, veh.getLength());
    if (safeSpace<length()) {
        MSVehicle::State state(safeSpace, 0);
        veh.enterLaneAtEmit(this, state);
        myVehicles.push_front(&veh);
        myUseDefinition->noVehicles++;
        myUseDefinition->vehLenSum += veh.getLength();
        assert(myUseDefinition->noVehicles==myVehicles.size());

#ifdef ABS_DEBUG
        if (debug_searched1==veh.getID()||debug_searched2==veh.getID()) {
            DEBUG_OUT << "Using emitTry( MSVehicle& veh )/2:" << debug_globaltime << "\n";
        }
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry(MSVehicle& veh, VehCont::iterator leaderIt)
{
    if (myApproaching==0) {
        // emission as last car (in driving direction)
        MSVehicle *leader = *leaderIt;
        // get invoked vehicles' positions
        SUMOReal leaderPos = (*leaderIt)->getPositionOnLane() - (*leaderIt)->getLength();
        // get secure gaps
        SUMOReal frontGapNeeded = veh.getSecureGap(veh.getSpeed(), leader->getSpeed(), leader->getLength());
        // compute needed room
        SUMOReal frontMax = leaderPos - frontGapNeeded;
        // check whether there is enough room
        if (frontMax>0) {
            // emit vehicle if so
            MSVehicle::State state(frontMax, 0);
            veh.enterLaneAtEmit(this, state);
            myVehicles.push_front(&veh);
            myUseDefinition->noVehicles++;
            myUseDefinition->vehLenSum += veh.getLength();
            assert(myUseDefinition->noVehicles==myVehicles.size());

#ifdef ABS_DEBUG
            if (debug_searched1==veh.getID()||debug_searched2==veh.getID()) {
                DEBUG_OUT << "Using emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/1:" << debug_globaltime << "\n";
            }
#endif

            return true;
        }
        return false;
    } else {
        // another vehicle is approaching this lane
        MSVehicle *leader = *leaderIt;
        MSVehicle *follow = myApproaching;
        // get invoked vehicles' positions
        SUMOReal followPos = follow->getPositionOnLane();
        SUMOReal leaderPos = leader->getPositionOnLane() - leader->getLength();
        // get secure gaps
        SUMOReal frontGapNeeded = veh.getSecureGap(veh.getSpeed(), leader->getSpeed(), leader->getLength());
        SUMOReal backGapNeeded = follow->getSecureGap(follow->getSpeed(), veh.getSpeed(), veh.getLength());
        // compute needed room
        SUMOReal frontMax = leaderPos - frontGapNeeded;
        SUMOReal backMin = followPos + backGapNeeded + veh.getLength();
        // check whether there is enough room
        if (frontMax>0 && backMin<frontMax) {
            // emit vehicle if so
            MSVehicle::State state((frontMax+backMin)/(SUMOReal) 2.0, 0);
            veh.enterLaneAtEmit(this, state);
            myVehicles.insert(leaderIt, &veh);
            myUseDefinition->noVehicles++;
            myUseDefinition->vehLenSum += veh.getLength();
            assert(myUseDefinition->noVehicles==myVehicles.size());
#ifdef ABS_DEBUG
            if (debug_searched1==veh.getID()||debug_searched2==veh.getID()) {
                DEBUG_OUT << "Using emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/2:" << debug_globaltime << "\n";
            }
#endif

            return true;
        }
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry(VehCont::iterator followIt, MSVehicle& veh)
{
    // emission as first car (in driving direction)
    MSVehicle *follow = *followIt;
    // get invoked vehicles' positions
    SUMOReal followPos = follow->getPositionOnLane();
    // get secure gaps
    SUMOReal backGapNeeded = follow->getSecureGap(follow->getSpeed(), veh.getSpeed(), veh.getLength());
    // compute needed room
    SUMOReal backMin = followPos + backGapNeeded + veh.getLength();
    // check whether there is enough room
    if (backMin<length()) {
        // emit vehicle if so
        MSVehicle::State state(backMin, 0);
        veh.enterLaneAtEmit(this, state);
        myVehicles.push_back(&veh);
        myUseDefinition->noVehicles++;
        myUseDefinition->vehLenSum += veh.getLength();
        assert(myUseDefinition->noVehicles==myVehicles.size());
#ifdef ABS_DEBUG
        if (debug_searched1==veh.getID()||debug_searched2==veh.getID()) {
            DEBUG_OUT << "Using emitTry( VehCont::iterator followIt, MSVehicle& veh )/1:" << debug_globaltime << "\n";
        }
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry(VehCont::iterator followIt, MSVehicle& veh,
                VehCont::iterator leaderIt)
{
    MSVehicle *leader = *leaderIt;
    MSVehicle *follow = *followIt;
    // get invoked vehicles' positions
    SUMOReal followPos = follow->getPositionOnLane();
    SUMOReal leaderPos = leader->getPositionOnLane() - leader->getLength();
    // get secure gaps
    SUMOReal frontGapNeeded = veh.getSecureGap(veh.getSpeed(), leader->getSpeed(), leader->getLength());
    SUMOReal backGapNeeded = follow->getSecureGap(follow->getSpeed(), veh.getSpeed(), veh.getLength());
    // compute needed room
    SUMOReal frontMax = leaderPos - frontGapNeeded;
    SUMOReal backMin = followPos + backGapNeeded + veh.getLength();
    // check whether there is enough room
    if (frontMax>0 && backMin<frontMax) {
        // emit vehicle if so
        MSVehicle::State state((frontMax + backMin) / (SUMOReal) 2.0, 0);
        veh.enterLaneAtEmit(this, state);
        myVehicles.insert(leaderIt, &veh);
        myUseDefinition->noVehicles++;
        myUseDefinition->vehLenSum += veh.getLength();
        assert(myUseDefinition->noVehicles==myVehicles.size());
#ifdef ABS_DEBUG
        if (debug_searched1==veh.getID()||debug_searched2==veh.getID()) {
            DEBUG_OUT << "Using emitTry( followIt, veh, leaderIt )/1:" << debug_globaltime << "\n";
        }
#endif

        return true;
    }
    return false;
}

void
MSLane::setCritical()
{
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    // move critical vehicles
    int to_pop = 0;
    bool lastPopped = false;
    VehCont::iterator i;
    for (i=myVehicles.begin() + myFirstUnsafe; i!=myVehicles.end(); i++) {
        (*i)->moveFirstChecked();
        MSLane *target = (*i)->getTargetLane();
        if (target!=this) {
            assert(to_pop==0||lastPopped);
            lastPopped = true;
            to_pop++;
        } else {
            lastPopped = false;
        }
    }
    for (int j = 0; j<to_pop; j++) {
        MSVehicle *v = *(myVehicles.end() - 1);
        MSVehicle *p = pop();
        assert(v==p);
        MSLane *target = p->getTargetLane();
#ifdef RAKNET_DEMO
        int oc = v->intOC;
        if (target->push(p)) {
            Vehicle::removeFromClient(oc);
        } else {
            v->setPosition(v->position().x(), 0, v->position().y());
        }
#else
        target->push(p);
#endif
    }
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    // check whether the lane is free
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;//myVehicles.size();
    }
    if (myVehicles.size()>0) {
        if (MSGlobals::gTimeToGridlock>0
                &&
                (*(myVehicles.end()-1))->getWaitingTime()>MSGlobals::gTimeToGridlock) {

            MSVehicleTransfer *vt = MSVehicleTransfer::getInstance();
            MSVehicle *veh = removeFirstVehicle();
            veh->removeApproachingInformationOnKill();
            if (veh->isEquipped()) {
                veh->getEdge()->removeEquippedVehicle(veh->getID());
            }
            vt->addVeh(veh);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::dictionary(string id, MSLane* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

MSLane*
MSLane::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::clear()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::push(MSVehicle* veh)
{
#ifdef ABS_DEBUG
    if (myVehBuffer!=0) {
        DEBUG_OUT << "Push Failed on Lane:" << myID << "\n";
        DEBUG_OUT << myVehBuffer->getID() << ", " << myVehBuffer->getPositionOnLane() << ", " << myVehBuffer->getSpeed() << "\n";
        DEBUG_OUT << veh->getID() << ", " << veh->getPositionOnLane() << ", " << veh->getSpeed() << "\n";
    }
#endif
    MSVehicle *last = myVehicles.size()!=0
                      ? myVehicles.front()
                      : 0;

    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    if (myVehBuffer != 0 || (last!=0 && last->getPositionOnLane() < veh->getPositionOnLane())) {
        MSVehicle *prev = myVehBuffer!=0
                          ? myVehBuffer : last;
        WRITE_WARNING("Vehicle '" + veh->getID()+ "' beamed due to a collision on push!\n" + "  Lane: '" + myID + "', previous vehicle: '" + prev->getID() +"', time: " + toString<SUMOTime>(MSNet::getInstance()->getCurrentTimeStep())+ ".");
        veh->onTripEnd(/* *this*/);
        resetApproacherDistance();
        veh->removeApproachingInformationOnKill(/*this*/);
        MSVehicleTransfer::getInstance()->addVeh(veh);
//        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
    // check whether the vehicle has ended his route
    if (! veh->destReached(myEdge)) {     // adjusts vehicles routeIterator
        myVehBuffer = veh;
        veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
        SUMOReal pspeed = veh->getSpeed();
        SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
        veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
        veh->_assertPos();
//        setApproaching(veh->pos(), veh);
        return false;
    } else {
        veh->onTripEnd(/* *this*/);
        resetApproacherDistance();
        veh->removeApproachingInformationOnKill(/*this*/);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle*
MSLane::pop()
{
    assert(! myVehicles.empty());
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    MSVehicle* first = myVehicles.back();
    first->leaveLaneAtMove(SPEED2DIST(first->getSpeed())/* - first->pos()*/);
    myVehicles.pop_back();
    myUseDefinition->noVehicles--;
    myUseDefinition->vehLenSum -= first->getLength();
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
    }
    return first;
}


bool
MSLane::appropriate(const MSVehicle *veh)
{
    if (myEdge->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }

    MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this);
    return (link != myLinks.end());
}

//////////////////////////////////////////////////////////////////////////

void
MSLane::integrateNewVehicle()
{
    if (myVehBuffer) {
        assert(myUseDefinition->noVehicles==myVehicles.size());
        myVehicles.push_front(myVehBuffer);
        myUseDefinition->vehLenSum += myVehBuffer->getLength();
        myVehBuffer = 0;
        myUseDefinition->noVehicles++;
        assert(myUseDefinition->noVehicles==myVehicles.size());
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::isLinkEnd(MSLinkCont::const_iterator &i) const
{
    return i==myLinks.end();
}

bool
MSLane::isLinkEnd(MSLinkCont::iterator &i)
{
    return i==myLinks.end();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::inEdge(const MSEdge *edge) const
{
    return myEdge==edge;
}

/////////////////////////////////////////////////////////////////////////////

const MSVehicle * const
MSLane::getLastVehicle() const
{
    if (myVehicles.size()==0) {
        return 0;
    }
    return *myVehicles.begin();
}

/////////////////////////////////////////////////////////////////////////////

const MSVehicle * const
MSLane::getFirstVehicle() const
{
    if (myVehicles.size()==0) {
        return 0;
    }
    return *(myVehicles.end()-1);
}

/////////////////////////////////////////////////////////////////////////////

MSLinkCont::const_iterator
MSLane::succLinkSec(const MSVehicle& veh, unsigned int nRouteSuccs,
                    const MSLane& succLinkSource) const
{
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle&>(veh).getGlID())) {
        int blb = 0;
    }
#endif
    const MSEdge* nRouteEdge = veh.succEdge(nRouteSuccs);
    // check whether the vehicle tried to look beyond its route
    if (nRouteEdge==0) {
        return succLinkSource.myLinks.end();
    }

    std::vector<MSLinkCont::const_iterator> valid;
    // the link must be from a lane to the right or left from the current lane
    //  we have to do it via the edge
    MSLinkCont::const_iterator link;
    for (link=succLinkSource.myLinks.begin(); link!=succLinkSource.myLinks.end() ; ++link) {
        if ((*link)->getLane()!=0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleClass())) {
            valid.push_back(link);
        }
    }
    if (valid.size()==0) {
        return succLinkSource.myLinks.end();
    }
    const MSEdge* nRouteEdge2 = veh.succEdge(nRouteSuccs+1);
    const MSEdge::LaneCont *next_allowed = nRouteEdge->allowedLanes(*nRouteEdge2, veh.getVehicleClass());
    if (nRouteEdge2==0||next_allowed==0) {
        return *(valid.begin());
    }
    /*
    size_t best = 0;
    SUMOReal bestdist = 0;
    */
    for (std::vector<MSLinkCont::const_iterator>::iterator i=valid.begin(); i!=valid.end(); ++i) {
        if (find(next_allowed->begin(), next_allowed->end(), (**i)->getLane())!=next_allowed->end()) {
            return *i;
        }
    }
    return *(valid.begin());
//    return succLinkSource.myLinks.end();
}


/////////////////////////////////////////////////////////////////////////////

void
MSLane::resetMeanData(unsigned index)
{
    assert(index<myMeanData.size());
    myMeanData[ index ].reset();
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::addMean2(const MSVehicle &veh, SUMOReal newV, SUMOReal oldV, SUMOReal gap)
{
    // Add to mean data (edge/lane state dump)
    if (myMeanData.size()!=0) {
        SUMOReal l = veh.getLength();
        for (size_t i=0; i<myMeanData.size(); i++) {
            myMeanData[i].nSamples++;
            myMeanData[i].speedSum += newV;
            myMeanData[i].vehLengthSum += l;
            if (newV<0.1) { // !!! swell
                myMeanData[i].haltSum++;
            }
        }
    }
    // Add to phys state
    if (OptionsCont::getOptions().isSet("physical-states-output")) {
        OutputDevice::getDeviceByOption("physical-states-output") << "   <vphys id=\"" << veh.getID()
        << "\" t=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\" v=\"" << newV
        << "\" a=\"" << (newV-oldV)
        << "\" g=\"" << gap
        << "\"/>" << "\n";
    }
}


/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<(ostream& os, const MSLane& lane)
{
    os << "MSLane: Id = " << lane.myID << "\n";
    return os;
}


/////////////////////////////////////////////////////////////////////////////

const MSLinkCont &
MSLane::getLinkCont() const
{
    return myLinks;
}

/////////////////////////////////////////////////////////////////////////////

const std::string &
MSLane::getID() const
{
    return myID;
}


void
MSLane::releaseVehicles()
{}


const MSLane::VehCont &
MSLane::getVehiclesSecure()
{
    throw 1;
}


void
MSLane::swapAfterLaneChange()
{
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
    myUseDefinition->noVehicles = myVehicles.size();
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;
    }
    assert(myUseDefinition->noVehicles==myVehicles.size());
}


void
MSLane::setApproaching(SUMOReal dist, MSVehicle *veh)
{
    myBackDistance = dist;
    myApproaching = veh;
}

/*
MSLane::VehCont::const_iterator
MSLane::findNextVehicleByPosition(SUMOReal pos) const
{
    assert(pos<myLength);
    // returns if no vehicle is available
    if(myVehicles.size()==0) {
        return myVehicles.end();
    }
    // some kind of a binary search
    size_t off1 = 0;
    size_t off2 = myVehicles.size() - 1;
    while(true) {
        size_t middle = (off1+off2)/2;
        MSVehicle *v1 = myVehicles[middle];
        if(v1->getPositionOnLane()>pos) {
            off2 = middle;
        } else if(v1->getPositionOnLane()<pos) {
            off1 = middle;
        }
        if(off1==off2) {
            return myVehicles.begin() + off1;
        }
    }
}
*/
/*
MSLane::VehCont::const_iterator
MSLane::findPrevVehicleByPosition(const VehCont::const_iterator &beginAt,
                                  SUMOReal pos) const
{
    assert(pos<myLength);
    // returns if no vehicle is available
    if(myVehicles.size()==0) {
        return myVehicles.end();
    }
    // some kind of a binary search
    size_t off1 = distance(myVehicles.begin(), beginAt);
    size_t off2 = myVehicles.size() - 1;
    while(true) {
        size_t middle = (off1+off2)/2;
        MSVehicle *v1 = myVehicles[middle];
        if(v1->getPositionOnLane()>pos) {
            off2 = middle;
        } else if(v1->getPositionOnLane()<pos) {
            off1 = middle;
        }
        if(off1==off2) {
            // there may be no vehicle before
            if(off1==0) {
                return myVehicles.end();
            }
            off1--;
            return myVehicles.begin() + off1;
        }
    }
}
*/

void
MSLane::addMoveReminder(MSMoveReminder* rem)
{
    moveRemindersM.push_back(rem);
}


MSLane::MoveReminderCont
MSLane::getMoveReminders(void)
{
    return moveRemindersM;
}


GUILaneWrapper *
MSLane::buildLaneWrapper(GUIGlObjectStorage &)
{
    throw "Only within the gui-version";
}


void
MSLane::init(MSEdgeControl &, MSEdgeControl::LaneUsage *useDefinition)
{
    myUseDefinition = useDefinition;
}


size_t
MSLane::getVehicleNumber() const
{
    return myUseDefinition->noVehicles;
}


MSVehicle *
MSLane::removeFirstVehicle()
{
    MSVehicle *veh = *(myVehicles.end()-1);
    veh->leaveLaneAtLaneChange();
    myVehicles.erase(myVehicles.end()-1);
    myUseDefinition->noVehicles--;
    myUseDefinition->vehLenSum -= veh->getLength();
    return veh;
}


size_t
MSLane::getNumericalID() const
{
    return myNumericalID;
}


void
MSLane::insertMeanData(unsigned int number)
{
    myMeanData.reserve(myMeanData.size() + number);
    myMeanData.insert(
        myMeanData.end(), number, MSLaneMeanDataValues());
}


MSVehicle *
MSLane::getLastVehicle(MSLaneChanger &) const
{
    if (myVehicles.size()==0) {
        return 0;
    }
    return *myVehicles.begin();
}


SUMOReal
MSLane::getDensity() const
{
    /*
    SUMOReal ret = 0;
    for(VehCont::const_iterator i=myVehicles.begin(); i!=myVehicles.end(); ++i) {
        ret += (*i)->getLength();
    }
    if(myUseDefinition->vehLenSum!=ret) {
        cout << myUseDefinition->vehLenSum << " <-> " << ret << "\n";
        throw 1;
    }
    */
    return myUseDefinition->vehLenSum / myLength;
}


SUMOReal
MSLane::getVehLenSum() const
{
    /*
    SUMOReal ret = 0;
    for(VehCont::const_iterator i=myVehicles.begin(); i!=myVehicles.end(); ++i) {
        ret += (*i)->getLength();
    }
    */
    return myUseDefinition->vehLenSum;
}


MSLane * const
MSLane::getLeftLane() const
{
    return myEdge->leftLane(this);
}


MSLane * const
MSLane::getRightLane() const
{
    return myEdge->rightLane(this);
}


const std::vector<SUMOVehicleClass> &
MSLane::getAllowedClasses() const
{
    return myAllowedClasses;
}


const std::vector<SUMOVehicleClass> &
MSLane::getNotAllowedClasses() const
{
    return myNotAllowedClasses;
}


bool
MSLane::allowsVehicleClass(SUMOVehicleClass vclass) const
{
    if (vclass==SVC_UNKNOWN) {
        return true;
    }
    if (myAllowedClasses.size()==0&&myNotAllowedClasses.size()==0) {
        return true;
    }
    if (find(myAllowedClasses.begin(), myAllowedClasses.end(), vclass)!=myAllowedClasses.end()) {
        return true;
    }
    if (myAllowedClasses.size()!=0) {
        return false;
    }
    if (find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), vclass)!=myNotAllowedClasses.end()) {
        return false;
    }
    return true;
}



/****************************************************************************/

