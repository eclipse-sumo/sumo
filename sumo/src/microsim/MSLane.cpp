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
#include <set>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


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
    for (MSLinkCont::iterator i=myLinks.begin(); i!=myLinks.end(); ++i) {
        delete *i;
    }
    // TODO
}


MSLane::MSLane(string id, SUMOReal maxSpeed, SUMOReal length, MSEdge* edge,
               size_t numericalID, const Position2DVector &shape,
               const std::vector<SUMOVehicleClass> &allowed,
               const std::vector<SUMOVehicleClass> &disallowed)  :
        myLastState(10000, 10000),
        myShape(shape),
        myID(id),
        myNumericalID(numericalID),
        myVehicles(),
        myLength(length),
        myEdge(edge),
        myMaxSpeed(maxSpeed),
        myAllowedClasses(allowed),
        myNotAllowedClasses(disallowed),
        myFirstUnsafe(0),
        myVehicleLengthSum(0)
{
    assert(myMaxSpeed>0);
}


void
MSLane::initialize(MSLinkCont* links)
{
    myLinks = *links;
    delete links;
}



bool
MSLane::moveNonCritical()
{
    assert(myVehicles.size()!=0);
    // Set the information about the last vehicle
    myLastState = (*myVehicles.begin())->getState();
    myFirstUnsafe = 0;

    // Move vehicles except first and all vehicles that may reach the end of the lane
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;

    VehCont::iterator neighIt, neighEnd;
    bool useNeigh = false;
    if (myFirstNeigh!=myLastNeigh) {
        if ((*myFirstNeigh)->myVehicles.size()!=0) {
            useNeigh = true;
            neighIt = (*myFirstNeigh)->myVehicles.begin();
            neighEnd = (*myFirstNeigh)->myVehicles.end();
        }
    }

    for (veh = myVehicles.begin();
            !(*veh)->reachingCritical(myLength) && veh != lastBeforeEnd;
        ) {

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
        ++veh;
        ++myFirstUnsafe;
        assert(&vehicle->getLane()==this);
        assert(&predec->getLane()==this);
    }
    return myVehicles.size()==0;
}


bool
MSLane::moveCritical()
{
    assert(myVehicles.size()!=0);
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;
    // Move all next vehicles beside the first
    for (veh=myVehicles.begin()+myFirstUnsafe;veh != lastBeforeEnd;) {
        VehCont::const_iterator pred(veh + 1);
        (*veh)->moveRegardingCritical(this, *pred, 0);
        // Check for timeheadway < deltaT
        MSVehicle *vehicle = (*veh);
        MSVehicle *predec = (*pred);
        assert(&vehicle->getLane()==this);
        assert(&predec->getLane()==this);
        assert((*veh)->getPositionOnLane() < (*pred)->getPositionOnLane());
        assert((*veh)->getPositionOnLane() <= myLength);
        ++veh;
    }
    (*veh)->moveRegardingCritical(this, 0, 0);
    assert((*veh)->getPositionOnLane() <= myLength);
    assert(&(*veh)->getLane()==this);
    return myVehicles.size()==0;
}


void
MSLane::detectCollisions(SUMOTime timestep)
{
    if (myVehicles.size() < 2) {
        return;
    }

    VehCont::iterator lastVeh = myVehicles.end() - 1;
    for (VehCont::iterator veh = myVehicles.begin();
            veh != lastVeh;) {

        VehCont::iterator pred = veh + 1;
        SUMOReal gap = (*pred)->getPositionOnLane() - (*pred)->getLength() - (*veh)->getPositionOnLane();
        if (gap < 0) {
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
            if (OptionsCont::getOptions().getBool("quit-on-accident")) {
                throw ProcessError();
            } else {
                vehV->leaveLaneAtLaneChange();
                vehV->onTripEnd();
                MSVehicleTransfer::getInstance()->addVeh(vehV);
            }
            veh = myVehicles.erase(veh); // remove current vehicle
            lastVeh = myVehicles.end() - 1;
            myVehicleLengthSum -= (*veh)->getLength();
            if (veh==myVehicles.end()) {
                break;
            }
        } else {
            ++veh;
        }
    }
}


bool
MSLane::freeEmit(MSVehicle& veh, SUMOReal speed) throw()
{
    // check whether we can emit in behind the last vehicle on the lane
    if(isEmissionSuccess(&veh, MSVehicle::State(0, speed))) {
        return true;
    } 
    // go through the lane, look for free positions
    MSLane::VehCont::iterator predIt = myVehicles.begin();
    while(predIt!=myVehicles.end()) {
        MSVehicle *follower = *predIt;
        MSVehicle *leader = predIt!=myVehicles.end()-1
            ? *(predIt+1)
            : 0;
        SUMOReal followPos = follower->getPositionOnLane();
        SUMOReal leaderPos = leader->getPositionOnLane() - leader->getLength();
        // get secure gaps
        SUMOReal frontGapNeeded = leader!=0
            ? veh.getSecureGap(veh.getSpeed(), leader->getSpeed(), leader->getLength())
            : -1;
        SUMOReal backGapNeeded = follower->getSecureGap(follower->getSpeed(), speed, veh.getLength());
        // compute needed room
        SUMOReal frontMax = frontGapNeeded>=0
            ? leaderPos - frontGapNeeded
            : length();
        SUMOReal backMin = followPos + backGapNeeded + veh.getLength();
        // check whether there is enough room
        if (frontMax>0 && backMin<frontMax) {
            // try emit vehicle (should be always ok)
            if(isEmissionSuccess(&veh, MSVehicle::State((frontMax + backMin) / (SUMOReal) 2.0, speed))) {
                return true;
            } else {
                cerr << "Strange behaviour:" << endl;
            }
        }
    }
}


bool
MSLane::emit(MSVehicle& veh, bool isReinsertion) throw()
{
    SUMOReal pos = 0;
    SUMOReal speed = 0;
    if(!isReinsertion) {
        const MSVehicle::DepartArrivalDefinition &pars = veh.getDepartureDefinition();
        switch(pars.speedProcedure) {
        case DEPART_SPEED_GIVEN:
            speed = pars.speed;
            break;
        case DEPART_SPEED_RANDOM:
            speed = RandHelper::rand(MIN2(veh.getMaxSpeed(), maxSpeed()));
            break;
        case DEPART_SPEED_MAX:
            speed = MIN2(veh.getMaxSpeed(), maxSpeed());
            break;
        case DEPART_SPEED_DEFAULT:
        default:
            break;
        }
        switch(pars.posProcedure) {
        case DEPART_POS_GIVEN:
            pos = pars.pos;
            break;
        case DEPART_POS_RANDOM:
            pos = RandHelper::rand(length());
            break;
        case DEPART_POS_FREE:
            return freeEmit(veh, speed);
        case DEPART_POS_DEFAULT:
        default:
            break;
        }
    } else {
        // vehicle reinsertion after teleportation
        speed = MIN2(veh.getMaxSpeed(), maxSpeed());
        return freeEmit(veh, speed);
    }
    return isEmissionSuccess(&veh, MSVehicle::State(pos, speed));
}


void
MSLane::add2MeanDataEmitted()
{
    if (myMeanData.size()!=0) {
        for (size_t i=0; i<myMeanData.size(); ++i) {
            myMeanData[i].emitted++;
        }
    }
}


bool
MSLane::isEmissionSuccess(MSVehicle* aVehicle,
                          const MSVehicle::State &vstate)
{
    MSLane::VehCont::iterator predIt =
        find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), vstate.pos()));
    // check predeccessor vehicle (!!! complte: what about vehicles that are on the next lane?
    if (predIt != myVehicles.end()) {
        MSVehicle* pred = *predIt;
        SUMOReal headWay = aVehicle->timeHeadWayGap(pred->getSpeed());   // !!!??
        SUMOReal gap = MSVehicle::gap(pred->getPositionOnLane(), pred->getLength(), vstate.pos());
        if (gap<headWay) {
            return false;
        }
        SUMOReal vsafe = aVehicle->ffeV(vstate.speed(), gap, pred->getSpeed());
        if (vsafe<vstate.speed()) {
            return false;
        }
        SUMOReal brakeWay = SPEED2DIST(aVehicle->getSpeedAfterMaxDecel(vsafe));//vstate.speed() - aVehicle->decelSpeed();
        if (vsafe<brakeWay) {
            return false;
        }
    } else {
        predIt = myVehicles.begin();
    }
    // check back vehicle
    if(predIt!=myVehicles.begin()) {
        MSVehicle *follower = *(predIt-1);
        SUMOReal headWay = follower->timeHeadWayGap(follower->getSpeed());
        SUMOReal gap = MSVehicle::gap(vstate.pos(), aVehicle->getLength(), follower->getPositionOnLane());
        if (gap<headWay) {
            return false;
        }
        SUMOReal vsafe = follower->ffeV(follower->getSpeed(), gap, vstate.speed());
        SUMOReal brakeWay = SPEED2DIST(follower->getSpeedAfterMaxDecel(vsafe));
        if (vsafe<brakeWay) {
            return false;
        }
    }
    // check approaching vehicle
    SUMOReal tspeed = maxSpeed();
    SUMOReal dist = tspeed * tspeed * SUMOReal(1./2.*4) + tspeed;
    std::pair<MSVehicle *, SUMOReal> approaching = getApproaching(dist, 0, vstate.speed());
    if(approaching.first!=0) {
        MSVehicle *leader = approaching.first;
        SUMOReal headWay = leader->timeHeadWayGap(leader->getSpeed());   // !!!??
        SUMOReal gap = approaching.second - vstate.pos() - aVehicle->getLength();
        if (gap<headWay) {
            return false;
        }
        SUMOReal vsafe = leader->ffeV(leader->getSpeed(), gap, vstate.speed());
        SUMOReal brakeWay = SPEED2DIST(leader->getSpeedAfterMaxDecel(vsafe));//vstate.speed() - aVehicle->decelSpeed();
        if (vsafe<brakeWay) {
            return false;
        }
    }
    // enter
    aVehicle->enterLaneAtEmit(this, vstate);
    bool wasInactive = myVehicles.size()==0;
    myVehicles.insert(predIt, aVehicle);
    myVehicleLengthSum += aVehicle->getLength();
    if (wasInactive) {
        MSNet::getInstance()->getEdgeControl().gotActive(this);
    }
    add2MeanDataEmitted();
    return true;
}


bool
MSLane::setCritical(std::vector<MSLane*> &into)
{
    // move critical vehicles
    int to_pop = 0;
    int to_pop2 = 0;
    int running = 1;
    bool lastPopped = false;
    bool hadProblem = false;
    bool hadPopped = false;
    VehCont::iterator i;
    for (i=myVehicles.begin() + myFirstUnsafe; i!=myVehicles.end();) {
        (*i)->moveFirstChecked();
        MSLane *target = (*i)->getTargetLane();
        if (target!=this) {
            hadPopped = true;
            if (!(to_pop==0||lastPopped)) {
                hadProblem = true;
            }
            lastPopped = true;
            to_pop++;
            to_pop2 = running;//++;
        } else {
            lastPopped = false;
        }
        ++i;
        if (hadPopped) {
            ++running;
        }
    }
    if (to_pop!=to_pop2) {
        hadProblem = true;
    }
    for (int j = 0; j<to_pop2; j++) {
        MSVehicle *v = *(myVehicles.end() - 1);
        MSVehicle *p = pop();
        assert(v==p);
        MSLane *target = p->getTargetLane();
        if (target==this) {
            MsgHandler::getWarningInstance()->inform("Vehicle '" + v->getID() + "' will be teleported due to false leaving order.");
            v->leaveLaneAtLaneChange();
            v->onTripEnd();
            MSVehicleTransfer::getInstance()->addVeh(v);
            hadProblem = true;
            continue;
        }
        if (target!=0&&p->isOnRoad()) {
            target->push(p);
            into.push_back(target);
        }
    }
    // check whether the lane is free
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;
    }
    if (myVehicles.size()>0) {
        if (MSGlobals::gTimeToGridlock>0
                &&
                (*(myVehicles.end()-1))->getWaitingTime()>MSGlobals::gTimeToGridlock) {

            MSVehicleTransfer *vt = MSVehicleTransfer::getInstance();
            MSVehicle *veh = removeFirstVehicle();
#ifdef HAVE_BOYOM_C2C
            if (veh->isEquipped()) {
                veh->getEdge()->removeEquippedVehicle(veh->getID());
            }
#endif
            vt->addVeh(veh);
        }
    }
    return myVehicles.size()==0;
}


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


void
MSLane::clear()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
}


bool
MSLane::push(MSVehicle* veh)
{
    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    // check whether the vehicle has ended his route
    // Add to mean data (edge/lane state dump)
    if (myMeanData.size()!=0) {
        for (size_t i=0; i<myMeanData.size(); ++i) {
            myMeanData[i].entered++;
        }
    }
    if (! veh->destReached(myEdge)) {     // adjusts vehicles routeIterator
        myVehBuffer.push_back(veh);
        veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
        SUMOReal pspeed = veh->getSpeed();
        SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
        veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
        veh->_assertPos();
        return false;
    } else {
        veh->onTripEnd();
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
}


MSVehicle*
MSLane::pop()
{
    assert(! myVehicles.empty());
    MSVehicle* first = myVehicles.back();
    first->leaveLaneAtMove(SPEED2DIST(first->getSpeed())/* - first->pos()*/);
    myVehicles.pop_back();
    myVehicleLengthSum -= first->getLength();
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
    }
    if (myMeanData.size()!=0) {
        for (size_t i=0; i<myMeanData.size(); ++i) {
            myMeanData[i].left++;
        }
    }
    return first;
}


bool
MSLane::appropriate(const MSVehicle *veh)
{
    if (myEdge->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this, veh->getBestLanesContinuation());
    return (link != myLinks.end());
}


bool
MSLane::integrateNewVehicle()
{
    bool wasInactive = myVehicles.size()==0;
    sort(myVehBuffer.begin(), myVehBuffer.end(), vehicle_position_sorter());
    for (std::vector<MSVehicle*>::const_iterator i=myVehBuffer.begin(); i!=myVehBuffer.end(); ++i) {
        MSVehicle *veh = *i;
        myVehicles.push_front(veh);
        myVehicleLengthSum += veh->getLength();
    }
    myVehBuffer.clear();
    return wasInactive&&myVehicles.size()!=0;
}


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


const MSVehicle * const
MSLane::getLastVehicle() const
{
    if (myVehicles.size()==0) {
        return 0;
    }
    return *myVehicles.begin();
}


const MSVehicle * const
MSLane::getFirstVehicle() const
{
    if (myVehicles.size()==0) {
        return 0;
    }
    return *(myVehicles.end()-1);
}


MSLinkCont::const_iterator
MSLane::succLinkSec(const MSVehicle& veh, unsigned int nRouteSuccs,
                    const MSLane& succLinkSource, const std::vector<MSLane*> &conts) const
{
    const MSEdge* nRouteEdge = veh.succEdge(nRouteSuccs);
    // check whether the vehicle tried to look beyond its route
    if (nRouteEdge==0) {
        // return end (no succeeding link) if so
        return succLinkSource.myLinks.end();
    }
    // a link may be used if
    //  1) there is a destination lane ((*link)->getLane()!=0)
    //  2) the destination lane belongs to the next edge in route ((*link)->getLane()->myEdge == nRouteEdge)
    //  3) the destination lane allows the vehicle's class ((*link)->getLane()->allowsVehicleClass(veh.getVehicleClass()))

    // at first, we'll assume we have the continuations of our route in "conts" (built in "getBestLanes")
    //  "conts" stores the best continuations of our current lane
    MSLinkCont::const_iterator link;
    if (nRouteSuccs>0&&conts.size()>=nRouteSuccs&&nRouteSuccs>0) {
        // we go through the links in our list and return the matching one
        for (link=succLinkSource.myLinks.begin(); link!=succLinkSource.myLinks.end() ; ++link) {
            if ((*link)->getLane()!=0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleClass())) {
                // we should use the link if it connects us to the best lane
                if ((*link)->getLane()==conts[nRouteSuccs-1]) {
                    return link;
                }
            }
        }
    }

    // ok, we were not able to use the conts for any reason
    //  we will now collect allowed links, at first
    // collect allowed links
    std::vector<MSLinkCont::const_iterator> valid;
    for (link=succLinkSource.myLinks.begin(); link!=succLinkSource.myLinks.end() ; ++link) {
        if ((*link)->getLane()!=0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleClass())) {
            valid.push_back(link);
        }
    }
    // if no valid link was found...
    if (valid.size()==0) {
        // ... return end (no succeeding link)
        return succLinkSource.myLinks.end();
    }
    // if there is only one valid link, let's use it...
    if (valid.size()==1) {
        return *(valid.begin());
    }
    // if the next edge is the route end, then we may return an arbitary link
    // also, if there is no allowed lane on the edge following the current one (recheck?)
    const MSEdge* nRouteEdge2 = veh.succEdge(nRouteSuccs+1);
    const MSEdge::LaneCont *next_allowed = nRouteEdge->allowedLanes(*nRouteEdge2, veh.getVehicleClass());
    if (nRouteEdge2==0||next_allowed==0) {
        return *(valid.begin());
    }
    // now let's determine which link is the best
    // in fact, we do not know it, here...
    for (std::vector<MSLinkCont::const_iterator>::iterator i=valid.begin(); i!=valid.end(); ++i) {
        if (find(next_allowed->begin(), next_allowed->end(), (**i)->getLane())!=next_allowed->end()) {
            return *i;
        }
    }
    return *(valid.begin());
}



void
MSLane::resetMeanData(unsigned index)
{
    assert(index<myMeanData.size());
    myMeanData[ index ].reset();
}


void
MSLane::addMean2(const MSVehicle &veh, SUMOReal newV, SUMOReal oldV, SUMOReal gap)
{
    // Add to mean data (edge/lane state dump)
    if (myMeanData.size()!=0) {
        SUMOReal l = veh.getLength();
        for (size_t i=0; i<myMeanData.size(); ++i) {
            myMeanData[i].nSamples++;
            myMeanData[i].speedSum += newV;
            myMeanData[i].vehLengthSum += l;
            if (newV<0.1) { // !!! swell
                myMeanData[i].haltSum++;
            }
        }
    }
    /*
    // Add to phys state
    if (OptionsCont::getOptions().isSet("physical-states-output")) {
        OutputDevice::getDeviceByOption("physical-states-output") << "   <vphys id=\"" << veh.getID()
        << "\" t=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\" v=\"" << newV
        << "\" a=\"" << (newV-oldV)
        << "\" g=\"" << gap
        << "\"/>" << "\n";
    }
    */
}



ostream&
operator<<(ostream& os, const MSLane& lane)
{
    os << "MSLane: Id = " << lane.myID << "\n";
    return os;
}



const MSLinkCont &
MSLane::getLinkCont() const
{
    return myLinks;
}


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
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;
    }
}




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
MSLane::init(MSEdgeControl &, MSEdge::LaneCont::const_iterator firstNeigh, MSEdge::LaneCont::const_iterator lastNeigh)
{
    myFirstNeigh = firstNeigh;
    myLastNeigh = lastNeigh;
}


unsigned int
MSLane::getVehicleNumber() const
{
    return (unsigned int) myVehicles.size();
}


MSVehicle *
MSLane::removeFirstVehicle()
{
    MSVehicle *veh = *(myVehicles.end()-1);
    veh->leaveLaneAtLaneChange();
    myVehicles.erase(myVehicles.end()-1);
    myVehicleLengthSum -= veh->getLength();
    return veh;
}


MSVehicle *
MSLane::removeVehicle(MSVehicle * remVehicle)
{
    for (MSLane::VehCont::iterator it = myVehicles.begin();
            it < myVehicles.end();
            it++) {
        if (remVehicle->getID() == (*it)->getID()) {
            remVehicle->leaveLaneAtLaneChange();
            myVehicles.erase(it);
            myVehicleLengthSum -= remVehicle->getLength();
            break;
        }
    }
    return remVehicle;
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
    return myVehicleLengthSum / myLength;
}


SUMOReal
MSLane::getVehLenSum() const
{
    return myVehicleLengthSum;
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


void
MSLane::addIncomingLane(MSLane *lane, MSLink *viaLink)
{
    IncomingLaneInfo ili;
    ili.lane = lane;
    ili.viaLink = viaLink;
    ili.length = lane->length();
    myIncomingLanes.push_back(ili);
}

class by_second_sorter
{
public:
    inline int operator()(const std::pair<MSVehicle * , SUMOReal> &p1, const std::pair<MSVehicle * , SUMOReal> &p2) const {
        return p1.second<p2.second;
    }
};

std::pair<MSVehicle *, SUMOReal>
MSLane::getApproaching(SUMOReal dist, SUMOReal seen, SUMOReal leaderSpeed) const
{
    // ok, a vehicle has not noticed the lane about itself;
    //  iterate as long as necessary to search for an approaching one
    set<MSLane*> visited;
    visited.insert((MSLane*) this);
    std::vector<std::pair<MSVehicle * , SUMOReal> > possible;
    std::vector<MSLane::IncomingLaneInfo> newFound;
    std::vector<MSLane::IncomingLaneInfo> toExamine = myIncomingLanes;
    while (toExamine.size()!=0) {
        for (std::vector<MSLane::IncomingLaneInfo>::iterator i=toExamine.begin(); i!=toExamine.end(); ++i) {
            if ((*i).viaLink->getState()==MSLink::LINKSTATE_TL_RED) {
                continue;
            }
            MSLane *next = (*i).lane;
            if (next->getFirstVehicle()!=0) {
                MSVehicle * v = (MSVehicle*) next->getFirstVehicle();
                SUMOReal igap = v->interactionGap(v->getSpeed(), myMaxSpeed, leaderSpeed);
                if (igap>(*i).length-v->getPositionOnLane()+seen) {
                    possible.push_back(make_pair(v, (*i).length-v->getPositionOnLane()+seen));
                }
            } else {
                if ((*i).length+seen<dist) {
                    const std::vector<MSLane::IncomingLaneInfo> &followers = next->getIncomingLanes();
                    for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j=followers.begin(); j!=followers.end(); ++j) {
                        if (visited.find((*j).lane)==visited.end()) {
                            visited.insert((*j).lane);
                            MSLane::IncomingLaneInfo ili;
                            ili.lane = (*j).lane;
                            ili.length = (*j).length + (*i).length;
                            ili.viaLink = (*j).viaLink;
                            newFound.push_back(ili);
                        }
                    }
                }
            }
        }
        toExamine.clear();
        swap(newFound, toExamine);
    }
    if (possible.size()==0) {
        return std::pair<MSVehicle *, SUMOReal>(0, -1);
    }
    sort(possible.begin(), possible.end(), by_second_sorter());
    return *(possible.begin());
}


void
MSLane::leftByLaneChange(MSVehicle *v)
{
    myVehicleLengthSum -= v->getLength();
}


void
MSLane::enteredByLaneChange(MSVehicle *v)
{
    myVehicleLengthSum += v->getLength();
}


SUMOReal 
MSLane::getMeanSpeed() const
{
    if(myVehicles.size()==0) {
        return myMaxSpeed;
    }
    SUMOReal v = 0;
    for(VehCont::const_iterator i=myVehicles.begin(); i!=myVehicles.end(); ++i) {
        v += (*i)->getSpeed();
    }
    return v / (SUMOReal) myVehicles.size();
}



/****************************************************************************/

