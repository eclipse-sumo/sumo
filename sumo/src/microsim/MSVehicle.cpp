/****************************************************************************/
/// @file    MSVehicle.cpp
/// @author  Christian Roessel
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// micro-simulation Vehicles.
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

#include "MSLane.h"
#include "MSVehicle.h"
#include "MSEdge.h"
#include "MSVehicleType.h"
#include "MSNet.h"
#include "MSRoute.h"
#include "MSLinkCont.h"
#include "MSVehicleQuitReminded.h"
#include "MSDebugHelper.h"
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <utils/common/RGBColor.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSGlobals.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <map>
#include "MSMoveReminder.h"
#include <utils/options/OptionsCont.h>
#include "MSLCM_DK2004.h"
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "trigger/MSBusStop.h"
#include <utils/helpers/SUMODijkstraRouter.h>
#include "MSPerson.h"
#include <utils/common/RandHelper.h>


#include "devices/MSDevice_CPhone.h"

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


#define BUS_STOP_OFFSET 0.5


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods of MSVehicle::State
 * ----------------------------------------------------------------------- */
MSVehicle::State::State(const State& state)
{
    myPos = state.myPos;
    mySpeed = state.mySpeed;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State&
MSVehicle::State::operator=(const State& state)
{
    myPos   = state.myPos;
    mySpeed = state.mySpeed;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::State::operator!=(const State& state)
{
    return (myPos   != state.myPos ||
            mySpeed != state.mySpeed);
}

/////////////////////////////////////////////////////////////////////////////

SUMOReal
MSVehicle::State::pos() const
{
    return myPos;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State::State(SUMOReal pos, SUMOReal speed) :
        myPos(pos), mySpeed(speed)
{}


/* -------------------------------------------------------------------------
 * MSVehicle-methods
 * ----------------------------------------------------------------------- */
MSVehicle::~MSVehicle()
{
    // remove move reminder
    for (QuitRemindedVector::iterator i=myQuitReminded.begin(); i!=myQuitReminded.end(); ++i) {
        (*i)->removeOnTripEnd(this);
    }
    // delete the route
    if (!myRoute->inFurtherUse() && myRepetitionNumber <= 0) {
        MSRoute::erase(myRoute->getID());
    }
    // delete values in CORN
    // prior routes
    if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE)!=myPointerCORNMap.end()) {
        ReplacedRoutesVector *v = (ReplacedRoutesVector*) myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE];
        for (ReplacedRoutesVector::iterator i=v->begin(); i!=v->end(); ++i) {
            delete(*i).route;
        }
        delete v;
    }
    // devices
    {
        // cell phones
        if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_DEV_CPHONE)!=myPointerCORNMap.end()) {
            vector<MSDevice_CPhone*> *v = (vector<MSDevice_CPhone*>*) myPointerCORNMap[MSCORN::CORN_P_VEH_DEV_CPHONE];
            for (vector<MSDevice_CPhone*>::iterator i=v->begin(); i!=v->end(); ++i) {
                delete(*i);
            }
            delete v;
        }
        if (MSNet::getInstance()->getMSPhoneNet()!=0) {
            MSNet::getInstance()->getMSPhoneNet()->removeVehicle(*this, MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    delete myLaneChangeModel;
    delete akt;
    {
        for (VehCont::iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            delete(*i).second;
        }
        myNeighbors.clear();
    }
    {
        for (ClusterCont::iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
            delete(*i);
        }
        clusterCont.clear();
    }
    {
        for (InfoCont::iterator i=infoCont.begin(); i!=infoCont.end(); ++i) {
            delete(*i).second;
        }
        infoCont.clear();
    }
    if (hasCORNPointerValue(MSCORN::CORN_P_VEH_OWNCOL)) {
        delete(RGBColor *) myPointerCORNMap[MSCORN::CORN_P_VEH_OWNCOL];
    }
    // persons
    if (hasCORNPointerValue(MSCORN::CORN_VEH_PASSENGER)) {
        std::vector<MSPerson*> *persons = (std::vector<MSPerson*>*) myPointerCORNMap[MSCORN::CORN_VEH_PASSENGER];
        for (std::vector<MSPerson*>::iterator i=persons->begin(); i!=persons->end(); ++i) {
            (*i)->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep());
        }
        delete persons;
    }
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::MSVehicle(string id,
                     MSRoute* route,
                     SUMOTime departTime,
                     const MSVehicleType* type,
                     int repNo, int repOffset,
                     int vehicleIndex) :
#ifdef HAVE_MESOSIM
        MEVehicle(this, 0, 0),
#endif
        myLastLaneChangeOffset(0),
        myTarget(0),
        myWaitingTime(0),
        myRepetitionNumber(repNo),
        myPeriod(repOffset),
        myID(id),
        myRoute(route),
        myDesiredDepart(departTime),
        myState(0, 0), //
        myIndividualMaxSpeed(0.0),
        myIsIndividualMaxSpeedSet(false),
        equipped(false),
        lastUp(0),
        clusterId(-1),
        totalNrOfSavedInfos(0),
        timeSinceStop(0),
        akt(0),
        myLastInfoTime(0),
        myHaveRouteInfo(false),
        myLane(0),
        myType(type),
        myLastBestLanesEdge(0),
        myCurrEdge(myRoute->begin()),
        myAllowedLanes(0),
        myMoveReminders(0),
        myOldLaneMoveReminders(0),
        myOldLaneMoveReminderOffsets(0),
        myNoGot(0), myNoSent(0), myNoGotRelevant(0)
{
    rebuildAllowedLanes();
    myLaneChangeModel = new MSLCM_DK2004(*this);
    // init cell phones
    initDevices(vehicleIndex);
}


void
MSVehicle::initDevices(int vehicleIndex)
{
    OptionsCont &oc = OptionsCont::getOptions();
    /*
        // cell phones
        if (myType->getID().compare("SBahn")== 0) {
            int noCellPhones = 1;
            if ((28800 <= myDesiredDepart && 32400 >= myDesiredDepart) || (61200 <= myDesiredDepart && 64800 >= myDesiredDepart))//40% 8 -9;17-18
                noCellPhones = 154;
            else if ((46800 <= myDesiredDepart && 61200 >= myDesiredDepart) || (64800 <= myDesiredDepart && 68400 >= myDesiredDepart)) //35% 13-17;18-19
                noCellPhones = 134;
            else if ((21600 <= myDesiredDepart && 28800 >= myDesiredDepart) || (32400 <= myDesiredDepart && 46800 >= myDesiredDepart) //25% 6-8;9-13;19-24
                     || (68400 <= myDesiredDepart && 86400 >= myDesiredDepart))
                noCellPhones = 96;
            else if ((0 <= myDesiredDepart && 5400 >= myDesiredDepart) || (14400 <= myDesiredDepart && 21600 >= myDesiredDepart)) //10% 0-1:30;4-6
                noCellPhones = 38;
            vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
            for (int np=0; np<noCellPhones; np++) {
                string phoneid = getID() + "_cphone#" + toString(np);
                v->push_back(new MSDevice_CPhone(*this, phoneid));
            }
            myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
        } else if (myType->getID().substr(0, 3)=="PKW") {
            int noCellPhones = 1;
            vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
            for (int np=0; np<noCellPhones; np++) {
                string phoneid = getID() + "_cphone#" + toString(np);
                v->push_back(new MSDevice_CPhone(*this, phoneid));
            }
            myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
        }
        else if (oc.getBool("device.cell-phone.percent-of-activity")) {
            /*myIntCORNMap[MSCORN::CORN_VEH_DEV_NO_CPHONE] = 1;
            string phoneid = getID() + "_cphone#0";
            MSDevice_CPhone* pdcp  = new MSDevice_CPhone(*this, phoneid);
            myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*)pdcp;/
            if (RandHelper::rand()<=oc.getFloat("device.cell-phone.probability")) {
                vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
                string phoneid = getID() + "_cphone#0";
                v->push_back(new MSDevice_CPhone(*this, phoneid));
                myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
            }
        } else if (oc.getFloat("device.cell-phone.probability")!=0||oc.isSet("device.cell-phone.knownveh")) {
            bool t1 = RandHelper::rand()<=oc.getFloat("device.cell-phone.probability");
            bool t2 = oc.isSet("device.cell-phone.knownveh") && OptionsCont::getOptions().isInStringVector("device.cell-phone.knownveh", myID);
            if (t1||t2) {
                int noCellPhones = (int)RandHelper::rand(oc.getFloat("device.cell-phone.amount.min"),
                                                         oc.getFloat("device.cell-phone.amount.max"));
                vector<MSDevice_CPhone*> *v = new vector<MSDevice_CPhone*>();
                for (int np=0; np<noCellPhones; np++) {
                    string phoneid = getID() + "_cphone#" + toString(np);
                    v->push_back(new MSDevice_CPhone(*this, phoneid));
                }
                myPointerCORNMap[(MSCORN::Pointer)(MSCORN::CORN_P_VEH_DEV_CPHONE)] = (void*) v;
            }
        }
    */
    // c2c communication
    if (oc.getFloat("device.c2x.probability")!=0||oc.isSet("device.c2x.knownveh")) {
        bool t1 = false;
        if (!oc.getBool("device.c2x.deterministic")) {
            t1 = RandHelper::rand()<=oc.getFloat("device.c2x.probability");
        } else {
            t1 = !((vehicleIndex%1000)>=(int)(oc.getFloat("device.c2x.probability")*1000.));
        }
        bool t2 = oc.isSet("device.c2x.knownveh") && OptionsCont::getOptions().isInStringVector("device.c2x.knownveh", myID);
        if (t1||t2) {
            equipped = true;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge &
MSVehicle::departEdge()
{
    return **myCurrEdge;
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge::LaneCont &
MSVehicle::departLanes()
{
    return *(myAllowedLanes[0]);
}

/////////////////////////////////////////////////////////////////////////////

SUMOTime
MSVehicle::desiredDepart() const
{
    return myDesiredDepart;
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge*
MSVehicle::succEdge(unsigned int nSuccs) const
{
    if (hasSuccEdge(nSuccs)) {
        return *(myCurrEdge + nSuccs);
    } else {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::hasSuccEdge(unsigned int nSuccs) const
{
    if (myCurrEdge + nSuccs >= myRoute->end()) {
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::destReached(const MSEdge* targetEdge)
{
    if (targetEdge->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return false;
    }
    // search for the target in the vehicle's route. Usually there is
    // only one iteration. Only for very short edges a vehicle can
    // "jump" over one ore more edges in one timestep.
    MSRouteIterator edgeIt = myCurrEdge;
    while (*edgeIt != targetEdge) {
        ++edgeIt;
        assert(edgeIt != myRoute->end());
    }
    myCurrEdge = edgeIt;
    // Check if destination-edge is reached. Update allowedLanes makes
    // only sense if destination isn't reached.
    MSRouteIterator destination = myRoute->end() - 1;
    if (myCurrEdge == destination) {
        return true;
    } else {
        rebuildAllowedLanes();
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::endsOn(const MSLane &lane) const
{
    return lane.inEdge(myRoute->getLastEdge());
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::move(MSLane* lane, const MSVehicle* pred, const MSVehicle* neigh)
{
    // reset move information
    myTarget = 0;
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "movea/1:" << debug_globaltime << ": " << myID << " at " << myLane->getID() << ": " << getPositionOnLane() << ", " << getSpeed() << "\n";
    }
#endif
    // save old v for optional acceleration computation
    SUMOReal oldV = myState.mySpeed;
    // compute gap to use
    SUMOReal gap = gap2pred(*pred);
    if (gap<0.1) {
        assert(gap>-0.1);
        gap = 0;
    }
    SUMOReal vSafe  = myType->ffeV(myState.mySpeed, gap, pred->getSpeed());
    if (neigh!=0&&neigh->getSpeed()>60./3.6) {
        SUMOReal mgap = MAX2((SUMOReal) 0, neigh->getPositionOnLane()-neigh->getLength()-getPositionOnLane());
        SUMOReal nVSafe = myType->ffeV(myState.mySpeed, mgap, neigh->getSpeed());
        vSafe = MIN2(vSafe, nVSafe);
    }


    if (!myStops.empty()) {
        if (myStops.begin()->reached) {
            if (myStops.begin()->duration==0) {
                if (myStops.begin()->busstop!=0) {
                    myStops.begin()->busstop->leaveFrom(this);
                }
                myStops.pop_front();
            } else {
                myStops.begin()->duration--;
                myTarget = myLane;
                myState.mySpeed = 0;
                myLane->addMean2(*this, 0, oldV, gap);
                return; // !!!detectore etc?
            }
        } else {
            if (myStops.begin()->lane==myLane) {
                Stop &bstop = *myStops.begin();
                SUMOReal endPos = bstop.pos;
                bool busStopsMustHaveSpace = true;
                if (bstop.busstop!=0) {
                    endPos = bstop.busstop->getLastFreePos();
                    if (endPos-5.<bstop.busstop->getBeginLanePosition()) { // !!! explicite offset
                        busStopsMustHaveSpace = false;
                    }
                }
                if (myState.pos()>=endPos-BUS_STOP_OFFSET&&busStopsMustHaveSpace) {
                    bstop.reached = true;
                    if (bstop.duration==-1) {
                        assert(bstop.until>=0);
                        bstop.duration = bstop.until - MSNet::getInstance()->getCurrentTimeStep();
                    }
                    if (bstop.busstop!=0) {
                        bstop.busstop->enter(this, myState.pos(), myState.pos()-myType->getLength());
                    }
                }

                vSafe = MIN2(vSafe, myType->ffeS(myState.mySpeed, endPos-myState.pos()));
            }
        }
    }

    SUMOReal maxNextSpeed = MIN2(myType->maxNextSpeed(myState.mySpeed), getMaxSpeed());

    SUMOReal vNext = myType->dawdle(MIN3(lane->maxSpeed(), myType->maxNextSpeed(myState.mySpeed), vSafe));
    vNext =
        myLaneChangeModel->patchSpeed(
            MAX2((SUMOReal) 0, myState.mySpeed-ACCEL2SPEED(myType->getMaxDecel())), //!!! reverify
            vNext,
            MIN3(vSafe, myLane->maxSpeed(), maxNextSpeed),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);
    vNext = MIN4(vNext, vSafe, myLane->maxSpeed(), maxNextSpeed);//vaccel(myState.mySpeed, myLane->maxSpeed()));

    SUMOReal predDec = pred->getSpeedAfterMaxDecel(pred->getSpeed()); //!!!!q//-decelAbility() /* !!! decelAbility of leader! */);
    if (myType->brakeGap(vNext)+vNext*myType->getTau() > myType->brakeGap(predDec) + gap) {

        vNext = MIN2(vNext, DIST2SPEED(gap));
    }

    vNext = MAX2((SUMOReal) 0, vNext);
    if (vNext<=0.1) {
        myWaitingTime += DELTA_T;
    } else {
        myWaitingTime = 0;
    }

    // call reminders after vNext is set
    workOnMoveReminders(myState.myPos,
                        myState.myPos + SPEED2DIST(vNext), vNext);
    // update position and speed
    myState.myPos  += SPEED2DIST(vNext);
    assert(myState.myPos < lane->length());
    myState.mySpeed = vNext;
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "movea/2:" << debug_globaltime << ": " << myID << " at " << myLane->getID() << ": " << getPositionOnLane() << ", " << getSpeed() << "\n";
    }
#endif
    //@ to be optimized (move to somewhere else)
    if (hasCORNIntValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
    //@ to be optimized (move to somewhere else)
    myLane->addMean2(*this, vNext, oldV, gap);
}


void
MSVehicle::moveRegardingCritical(MSLane* lane,
                                 const MSVehicle* pred,
                                 const MSVehicle* /*neigh*/)
{
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "moveb/1:" << debug_globaltime << ": " << myID << " at " << myLane->getID() << ": " << getPositionOnLane() << ", " << getSpeed() << "\n";
    }
#endif
    myLFLinkLanes.clear();
    // check whether the vehicle is not on an appropriate lane
    if (!myLane->appropriate(this)) {
        // decelerate to lane end when yes
        SUMOReal vWish = myType->ffeS(myState.mySpeed, myLane->length()-myState.myPos);
        if (pred!=0) {
            vWish = MIN2(vWish, myType->ffeV(myState.mySpeed, gap2pred(*pred), pred->getSpeed()));
        }
        // !!! check whether the vehicle wants to stop somewhere
        if (!myStops.empty()&&myStops.begin()->lane->getEdge()==lane->getEdge()) {
            SUMOReal seen = lane->length() - myState.pos();
            SUMOReal vsafeStop = myType->ffeS(myState.mySpeed, seen-(lane->length()-myStops.begin()->pos));
            vWish = MIN2(vWish, vsafeStop);
        }
        vWish = MAX2((SUMOReal) 0, vWish);
        myLFLinkLanes.push_back(
            DriveProcessItem(0, vWish, vWish));
    } else {
        // compute other values as in move
        SUMOReal vBeg = MIN2(myType->maxNextSpeed(myState.mySpeed), lane->maxSpeed());//vaccel( myState.mySpeed, lane->maxSpeed() );
        if (pred!=0) {
            SUMOReal vSafe = myType->ffeV(myState.mySpeed, gap2pred(*pred), pred->getSpeed());
            //  the vehicle is bound by the lane speed and must not drive faster
            //  than vsafe to the next vehicle
            vBeg = MIN2(vBeg, vSafe);
        }
        // check whether the driver wants to let someone in
        // set next links, computing possible speeds
        vsafeCriticalCont(vBeg);
    }
    //@ to be optimized (move to somewhere else)
    if (hasCORNIntValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
    //@ to be optimized (move to somewhere else)
}


void
MSVehicle::moveFirstChecked()
{
    myTarget = 0;
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
    }
#endif
    // save old v for optional acceleration computation
    SUMOReal oldV = myState.mySpeed;
    // get vsafe
    SUMOReal vSafe = 0;

    assert(myLFLinkLanes.size()!=0);
    DriveItemVector::iterator i;
    MSLane *currentLane = myLane;
    bool cont = true;
    for (i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end()&&cont; i++) {
        MSLink *link = (*i).myLink;
        bool onLinkEnd = link==0;
        // the vehicle must change the lane on one of the next lanes
        if (!onLinkEnd) {
            if (link->havePriority()&&link->opened()) {
                vSafe = (*i).myVLinkPass;
            } else {
                if (link->opened()) {
                    vSafe = (*i).myVLinkPass;
                } else {
                    if (vSafe<getSpeedAfterMaxDecel(myState.mySpeed)&&link->getState()==MSLink::LINKSTATE_TL_YELLOW) {
                        vSafe = (*i).myVLinkPass;
                    } else {
                        vSafe = (*i).myVLinkWait;
                        cont = false;
                    }
                }
            }
        } else {
            vSafe = (*i).myVLinkWait;
            cont = false;
            break;
        }
        currentLane = link->getLane();
    }


    if (!myStops.empty()) {
        if (myStops.begin()->reached) {
            if (myStops.begin()->duration==0) {
                if (myStops.begin()->busstop!=0) {
                    myStops.begin()->busstop->leaveFrom(this);
                }
                myStops.pop_front();
            } else {
                myStops.begin()->duration--;
                myTarget = myLane;
                myState.mySpeed = 0;
                myLane->addMean2(*this, 0, oldV, -1);
                return; // !!!detectore etc?
            }
        } else {
            if (myStops.begin()->lane==myLane) {
                Stop &bstop = *myStops.begin();
                SUMOReal endPos = bstop.pos;
                bool busStopsMustHaveSpace = true;
                if (bstop.busstop!=0) {
                    endPos = bstop.busstop->getLastFreePos();
                    if (endPos-5.<bstop.busstop->getBeginLanePosition()) { // !!! explicite offset
                        busStopsMustHaveSpace = false;
                    }
                }
                if (myState.pos()>=endPos-BUS_STOP_OFFSET&&busStopsMustHaveSpace) {
                    bstop.reached = true;
                    if (bstop.duration==-1) {
                        assert(bstop.until>=0);
                        bstop.duration = bstop.until - MSNet::getInstance()->getCurrentTimeStep();
                    }
                    if (bstop.busstop!=0) {
                        bstop.busstop->enter(this, myState.pos(), myState.pos()-myType->getLength());
                    }
                }
                vSafe = MIN2(vSafe, myType->ffeS(myState.mySpeed, endPos-myState.pos()));
            }
        }
    }

    // compute vNext in considering dawdling
    SUMOReal vNext = myType->dawdle(vSafe);
    vNext =
        myLaneChangeModel->patchSpeed(
            getSpeedAfterMaxDecel(myState.mySpeed),
            vNext,
            MIN3(vSafe, myType->maxNextSpeed(myState.mySpeed), myLane->maxSpeed()), //vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);

    // visit waiting time
    if (vNext<=0.1) {
        myWaitingTime += DELTA_T;
    } else {
        myWaitingTime = 0;
    }
    // call reminders after vNext is set
    SUMOReal pos = myState.myPos;

    vNext = MIN2(vNext, getMaxSpeed());

    // update position
    myState.myPos += SPEED2DIST(vNext);
    // update speed
    myState.mySpeed = vNext;
    MSLane *approachedLane = myLane;
    approachedLane->addMean2(*this, vNext, oldV, -1);


    // move the vehicle forward
    size_t no = 0;
    SUMOReal driven = approachedLane->length() - pos;
    for (i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end()
            &&
            myState.myPos>approachedLane->length();
            ++i) {

        if (approachedLane!=myLane) {
            leaveLaneAtMove(driven);
        }
        MSLink *link = (*i).myLink;
        // check whether the vehicle was allowed to enter lane
        //  otherwise it is decelareted and we do not need to test for it's
        //  approach on the following lanes when a lane changing is performed
        assert(approachedLane!=0);
        myState.myPos -= approachedLane->length();
        assert(myState.myPos>0);
        if (approachedLane!=myLane) {
            enterLaneAtMove(approachedLane, driven, true);
            driven += approachedLane->length();
        }
        // proceed to the next lane
        if (link!=0/*approachedLane->isLinkEnd(link)*/) {
#ifdef HAVE_INTERNAL_LANES
            approachedLane = link->getViaLane();
            if (approachedLane==0) {
                approachedLane = link->getLane();
            }
#else
            approachedLane = link->getLane();
#endif
        }
        // set information about approaching
        approachedLane->setApproaching(myState.pos(), this);
        approachedLane->addMean2(*this, vNext, oldV, -1);
        no++;
    }
    // set approaching information for consecutive lanes the vehicle may reach in the
    //  next steps
    MSLane *tmpApproached = approachedLane;
    SUMOReal dist = myType->brakeGap(myState.mySpeed) - driven;
    SUMOReal tmpPos = myState.myPos + dist;
    for (; dist>0&&tmpApproached->length()<tmpPos&&i!=myLFLinkLanes.end(); i++) {
        MSLink *link = (*i).myLink;
        if (link==0) {
            break;
        }
        tmpPos -= tmpApproached->length();//approachedLane->length();
#ifdef HAVE_INTERNAL_LANES
        tmpApproached = link->getViaLane();
        if (tmpApproached==0) {
            tmpApproached = link->getLane();
        }
#else
        tmpApproached = link->getLane();
#endif
        tmpApproached->setApproaching(tmpPos, this);
    }

    // enter lane herein if no push occures (otherwise, do it there)
    if (no==0) {
        workOnMoveReminders(pos, pos + SPEED2DIST(vNext), vNext);
    }
    myTarget = approachedLane;
    assert(myTarget!=0);
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "moveb/1:" << debug_globaltime << ": " << myID << " at " << getLane().getID() << ": " << myState.myPos << ", " << myState.mySpeed << "\n";
    }
#endif
    assert(myTarget->length()>=myState.myPos);
}


void
MSVehicle::_assertPos() const
{
    assert(myState.myPos<=myLane->length());
}


void
MSVehicle::vsafeCriticalCont(SUMOReal boundVSafe)
{
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "vsafeCriticalCont/" << debug_globaltime << ":" << myID << "\n";
    }
#endif
    // the vehicle may have just to look into the next lane
    //  compute this information and use it only once in the next loop
    SUMOReal seen = myLane->length() - myState.myPos;
    MSLane *nextLane = myLane;
    // compute the way the vehicle would drive if it would use the current speed and then
    //  decelerate
    SUMOReal dist = SPEED2DIST(boundVSafe) + myType->brakeGap(myState.mySpeed);
    SUMOReal vLinkPass = boundVSafe;
    SUMOReal vLinkWait = vLinkPass;
    if (seen>dist) {
        // just for the case the vehicle is still very far away from the lane end
        myLFLinkLanes.push_back(DriveProcessItem(0, vLinkPass, vLinkPass));
        return;
    }

    size_t view = 1;
    // loop over following lanes
    while (true) {
        SUMOReal laneLength = nextLane->length();
        if (!myStops.empty()&&myStops.begin()->lane->getEdge()==nextLane->getEdge()) {
            SUMOReal vsafeStop = myType->ffeS(myState.mySpeed, seen-(nextLane->length()-myStops.begin()->pos));
            vLinkPass = MIN2(vLinkPass, vsafeStop);
            vLinkWait = MIN2(vLinkWait, vsafeStop);
        }

        // get the next link used
        MSLinkCont::const_iterator link =
            myLane->succLinkSec(*this, view, *nextLane);

        // check whether the lane is a dead end
        //  (should be valid only on further loop iterations
        if (nextLane->isLinkEnd(link)) {
            SUMOReal laneEndVSafe = myType->ffeS(myState.mySpeed, seen);
            myLFLinkLanes.push_back(
                DriveProcessItem(0, MIN2(vLinkPass, laneEndVSafe), MIN2(vLinkPass, laneEndVSafe)));
            // the vehicle will not drive further
            return;
        }
        // the link was passed
        vLinkWait = vLinkPass;


        // !!! optimize this - make this optional
        //  needed to let vehicles wait for all overlapping vehicles in front
        const MSLinkCont &lc = nextLane->getLinkCont();

        // get the following lane
#ifdef HAVE_INTERNAL_LANES
        bool nextInternal = false;
        nextLane = (*link)->getViaLane();
        if (nextLane==0) {
            nextLane = (*link)->getLane();
        } else {
            nextInternal = true;
        }
#else
        nextLane = (*link)->getLane();
#endif

        // compute the velocity to use when the link is not blocked by oter vehicles
        // the vehicle shall be not fastern when reaching the next lane than allowed
        SUMOReal vmaxNextLane =
            myType->ffeV(myState.mySpeed, seen, nextLane->maxSpeed());

        // the vehicle shall keep a secure distance to its predecessor
        //  (or approach the lane end if the predeccessor is too near)
        SUMOReal vsafePredNextLane = 1000;

        // !!! optimize this - make this optional
        SUMOReal r_dist2Pred = seen;
        if (nextLane->getLastVehicle()!=0) {
            r_dist2Pred = r_dist2Pred + nextLane->myLastState.pos() - nextLane->getLastVehicle()->getLength();
        } else {
            r_dist2Pred = r_dist2Pred + nextLane->length();
        }
//        +nextLane->myLastState.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! the real length of the car

#ifdef HAVE_INTERNAL_LANES
        if (MSGlobals::gUsingInternalLanes) {
            for (size_t j=0; j<lc.size(); ++j) {
                MSLane *nl = lc[j]->getViaLane();
                if (nl==0) {
                    nl = lc[j]->getLane();
                }
                if (nl==0) {
                    continue;
                }

                const State &nextLanePred = nl->myLastState;
                SUMOReal dist2Pred = seen;
                if (nl->getLastVehicle()!=0) {
                    dist2Pred = dist2Pred + nextLanePred.pos() - nl->getLastVehicle()->getLength();
                } else {
                    dist2Pred = dist2Pred + nl->length();
                }
//                seen+nextLanePred.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! the real length of the car
//            if(nl->length()<dist2Pred&&nl->length()<MSVehicleType::getMaxVehicleLength()) { // @!!! the real length of the car


                if (dist2Pred>=0) {
                    // leading vehicle is not overlapping
                    vsafePredNextLane =
                        MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
                    SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-ACCEL2SPEED(myType->decelAbility()) /* !!! decelAbility of leader! */);
                    if (myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                        vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred));
                    }
                } else {
                    // leading vehicle is overlapping (stands within the junction)
                    vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/*MAX2((SUMOReal) 0, seen-dist2Pred, 0);
                    // we have to wait in any case
                    break;
                }

//                if(MSGlobals::gUsingInternalLanes) {
                if (nextLanePred.pos()>9000) {
                    dist2Pred = seen + nl->length();
                }

                const MSLinkCont &lc2 = nl->getLinkCont();
                for (size_t j2=0; j2<lc2.size(); ++j2) {
                    MSLane *nl2 = lc2[j2]->getViaLane();
                    if (nl2==0) {
                        nl2 = lc2[j2]->getLane();
                    }
                    if (nl2==0) {
                        continue;
                    }
                    const State &nextLanePred2 = nl2->myLastState;
                    SUMOReal dist2Pred2 = dist2Pred;//dist2Pred+nextLanePred2.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! the real length of the car
                    if (nl2->getLastVehicle()!=0) {
                        dist2Pred2 = dist2Pred2 + nextLanePred2.pos() - nl2->getLastVehicle()->getLength();
                    } else {
                        dist2Pred2 = dist2Pred2 + nl2->length();
                    }
                    if (dist2Pred2>=0) {
                        // leading vehicle is not overlapping
                        vsafePredNextLane =
                            MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred2, nextLanePred2.speed()));
                        SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred2.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
                        if (myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred2) {

                            vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred2));
                        }
                    } else {
                        // leading vehicle is overlapping (stands within the junction)
                        vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/*MAX2((SUMOReal) 0, seen-dist2Pred, 0);
                        break;
                    }
                }
            }
        } else {
#endif
            const State &nextLanePred = nextLane->myLastState;
            SUMOReal dist2Pred = seen;
            if (nextLane->getLastVehicle()!=0) {
                dist2Pred = dist2Pred + nextLanePred.pos() - nextLane->getLastVehicle()->getLength();
            } else {
                dist2Pred = dist2Pred + nextLane->length();
            }
//                seen+nextLanePred.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! the real length of the car
//            if(nl->length()<dist2Pred&&nl->length()<MSVehicleType::getMaxVehicleLength()) { // @!!! the real length of the car


            if (dist2Pred>=0) {
                // leading vehicle is not overlapping
                vsafePredNextLane =
                    MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
                SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
                if (myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                    vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred));
                }
            } else {
                // leading vehicle is overlapping (stands within the junction)
                vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/*MAX2((SUMOReal) 0, seen-dist2Pred, 0);
            }
#ifdef HAVE_INTERNAL_LANES
        }
#endif
        /*
        } else {
        if(dist2Pred>=0) {
            // leading vehicle is not overlapping
            vsafePredNextLane =
                MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
            SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-myType->decelAbility() / !!! decelAbility of leader! /);
            if(myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred));
            }
        } else {
            // leading vehicle is overlapping (stands within the junction)
            vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/MAX2((SUMOReal) 0, seen-dist2Pred, 0);
        }
        }
        */

        // compute the velocity to use when the link may be used
        vLinkPass =
            MIN3(vLinkPass, vmaxNextLane, vsafePredNextLane/*, vsafeNextLaneEnd*/);

        // if the link may not be used (is blocked by another vehicle) then let the
        //  vehicle decelerate until the end of the street
        vLinkWait =
            MIN3(vLinkPass, vLinkWait, myType->ffeS(myState.mySpeed, seen));
        vLinkWait = MAX2(vLinkWait, myType->getSpeedAfterMaxDecel(myState.mySpeed));

        if ((*link)->getState()==MSLink::LINKSTATE_TL_YELLOW&&SPEED2DIST(vLinkWait)+myState.myPos<laneLength) {
            myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkWait, vLinkWait));
            return;
        }
        // valid, when a vehicle is not on a priorised lane
        if (!(*link)->havePriority()) {
            // if it has already decelerated to let priorised vehicles pass
            //  and when the distance to the vehicle on the next lane allows moving
            //  (the check whether other incoming vehicles may stop this one is done later)
            // then let it pass
            if (seen>=myType->approachingBrakeGap(myState.mySpeed)&&r_dist2Pred>0) {
                vLinkPass = MIN3(vLinkPass, myType->maxNextSpeed(myState.mySpeed), myLane->maxSpeed());
                (*link)->setApproaching(this);
            } else {
                // let it wait in the other cases
                vLinkPass = vLinkWait;
            }
        }
        myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkPass, vLinkWait));
        if (vsafePredNextLane>0&&dist-seen>0) {
            (*link)->setApproaching(this);
        } else {
            return;
        }
        seen += nextLane->length();
        if (seen>dist) {
            return;
        }
#ifdef HAVE_INTERNAL_LANES
        if (!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    }
}

/////////////////////////////////////////////////////////////////////////////

Position2D
MSVehicle::getPosition() const
{
    if (myLane==0) {
        return Position2D(-1000, -1000);
    }
    return myLane->getShape().positionAtLengthPosition(myState.pos());
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::getInTransit() const
{
    return (myLane!=0);
}

/////////////////////////////////////////////////////////////////////////////

const string &
MSVehicle::getID() const
{
    return myID;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isEquipped() const
{
    return equipped;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::onAllowed(const MSLane* lane) const
{
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
    if (lane->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if (myAllowedLanes.size()==0) {
        return false; // check (was assertion only)!!!
    }
    if (!lane->allowsVehicleClass(myType->getVehicleClass())) {
        return false;
    }
    assert(myAllowedLanes.size()!=0);
    MSEdge::LaneCont::const_iterator compare =
        find(myAllowedLanes[0]->begin(), myAllowedLanes[0]->end(), lane);
    return (compare != myAllowedLanes[0]->end());
}


bool
MSVehicle::onAllowed() const
{
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
    if (myLane->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if (myAllowedLanes.size()==0) {
        return false; // check (was assertion only)!!!
    }
    if (!myLane->allowsVehicleClass(myType->getVehicleClass())) {
        return false;
    }
    assert(myAllowedLanes.size()!=0);
    MSEdge::LaneCont::const_iterator compare =
        find(myAllowedLanes[0]->begin(), myAllowedLanes[0]->end(), myLane);
    return (compare != myAllowedLanes[0]->end());
}


/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtMove(MSLane* enteredLane, SUMOReal driven, bool inBetweenJump)
{
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
    }
#endif
    // save the old work reminders, patching the position information
    // add the information about the new offset to the old lane reminders
    SUMOReal oldLaneLength = myLane->length();
    OffsetVector::iterator i;
    for (i=myOldLaneMoveReminderOffsets.begin(); i!=myOldLaneMoveReminderOffsets.end(); i++) {
        (*i) += oldLaneLength;
    }
    for (size_t j=0; j<myMoveReminders.size(); j++) {
        myOldLaneMoveReminderOffsets.push_back(oldLaneLength);
    }

    copy(myMoveReminders.begin(), myMoveReminders.end(),
         back_inserter(myOldLaneMoveReminders));
    assert(myOldLaneMoveReminders.size()==myOldLaneMoveReminderOffsets.size());
    // set the entered lane as the current lane
    myLane = enteredLane;
    myTarget = enteredLane;
    // and update the mean data
    SUMOReal entryTimestep =
        static_cast< SUMOReal >(MSNet::getInstance()->getCurrentTimeStep()) - 1 +
        driven / myState.mySpeed;
    assert(entryTimestep<=debug_globaltime);
    // get new move reminder
    myMoveReminders = enteredLane->getMoveReminders();
    // proceed in route
    const MSEdge * const enteredEdge = enteredLane->getEdge();
    if (enteredEdge->getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
        MSRouteIterator edgeIt = myCurrEdge;
        while (*edgeIt != enteredEdge) {
            ++edgeIt;
            assert(edgeIt != myRoute->end());
        }
        myCurrEdge = edgeIt;
    }

    if (isEquipped()) {
        (*myCurrEdge)->addEquippedVehicle(getID(), this);
        delete akt;
        akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
    }
    if (MSCORN::wished(MSCORN::CORN_VEHCONTROL_WANTS_DEPARTURE_INFO)) {
        MSNet::getInstance()->getVehicleControl().vehicleMoves(this);
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtLaneChange(MSLane* enteredLane)
{
    myLane = enteredLane;
    // switch to and activate the new lane's reminders
    // keep OldLaneReminders
    myMoveReminders = enteredLane->getMoveReminders();
    myAllowedLanes.clear();
    rebuildAllowedLanes();
    activateRemindersByEmitOrLaneChange();
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtEmit(MSLane* enteredLane, const State &state)
{
    myState = state;
    assert(myState.myPos >= 0);
    assert(myState.mySpeed >= 0);
    myWaitingTime = 0;
    myLane = enteredLane;
    // set and activate the new lane's reminders
    myMoveReminders = enteredLane->getMoveReminders();
    activateRemindersByEmitOrLaneChange();

    // for Car2Car
    if (isEquipped()) {
        delete akt;
        (*myCurrEdge)->addEquippedVehicle(getID(), this);
        akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtMove(SUMOReal /*driven*/)
{
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, ((GUIVehicle*) this)->getGlID())) {
        int blb = 0;
    }
#endif
    if (isEquipped()) {
        // leave the c2c-edge
        (*myCurrEdge)->removeEquippedVehicle(getID());
        // checke whether the vehicle needed longer than expected
        float factor = (*myCurrEdge)->getEffort(this, MSNet::getInstance()->getCurrentTimeStep());
//        std::map<const MSEdge * const, Information *>::iterator i = infoCont.find(*myCurrEdge);
        float nt = (float)(MSNet::getInstance()->getCurrentTimeStep() - akt->time);
        if (nt>10&&nt>factor*MSGlobals::gAddInfoFactor) { // !!! explicite
            // if so, check whether an information about the edge was already existing
            std::map<const MSEdge * const, Information *>::iterator i = infoCont.find(*myCurrEdge);
            if (i==infoCont.end()) {
                // no, add the new information
                Information *info = new Information(*akt);
                info->neededTime = nt;
                infoCont[*myCurrEdge] = info;
                i = infoCont.find(*myCurrEdge);
            } else {
                // yes, update the existing information
                (*i).second->neededTime = nt;
            }
            // save the information
            MSCORN::saveSavedInformationData(MSNet::getInstance()->getCurrentTimeStep(),
                                             getID(),(*myCurrEdge)->getID(),"congestion",(*i).second->time,nt);
            totalNrOfSavedInfos++;
        } else if (infoCont.find(*myCurrEdge)!=infoCont.end()) {
            // ok, we could pass the edge faster than assumed; remove the information
            infoCont.erase(*myCurrEdge);
        }
        delete akt;
        akt = 0;
    }

    if (!myAllowedLanes.empty()) {
        myAllowedLanes.pop_front();
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtLaneChange(void)
{
    // dismiss the old lane's reminders
    SUMOReal savePos = myState.myPos; // have to do this due to SUMOReal-precision errors
    vector< MSMoveReminder* >::iterator rem;
    for (rem=myMoveReminders.begin(); rem != myMoveReminders.end(); ++rem) {
        (*rem)->dismissByLaneChange(*this);
    }
    std::vector<SUMOReal>::iterator off = myOldLaneMoveReminderOffsets.begin();
    for (rem=myOldLaneMoveReminders.begin(); rem!=myOldLaneMoveReminders.end(); ++rem, ++off) {
        myState.myPos += (*off);
        (*rem)->dismissByLaneChange(*this);
        myState.myPos -= (*off);
    }
    myState.myPos = savePos; // have to do this due to SUMOReal-precision errors
    myMoveReminders.clear();
    myOldLaneMoveReminders.clear();
    myOldLaneMoveReminderOffsets.clear();
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge * const
MSVehicle::getEdge() const
{
    return *myCurrEdge;
}



bool
MSVehicle::reachingCritical(SUMOReal laneLength) const
{
    // check whether the vehicle will run over the lane when accelerating
    return (laneLength - myState.myPos - myType->brakeGap(myState.mySpeed)) <= 0;
}


MSLane *
MSVehicle::getTargetLane() const
{
    return myTarget;
}


const MSLane &
MSVehicle::getLane() const
{
    return *myLane;
}


bool
MSVehicle::periodical() const
{
    return myPeriod>0;
}


MSVehicle *
MSVehicle::getNextPeriodical() const
{
    // check whether another one shall be repated
    if (myRepetitionNumber<=0) {
        return 0;
    }
    MSVehicle *ret = MSNet::getInstance()->getVehicleControl().buildVehicle(
                         StringUtils::version1(myID), myRoute, myDesiredDepart+myPeriod,
                         myType, myRepetitionNumber-1, myPeriod);
    for (std::list<Stop>::const_iterator i=myStops.begin(); i!=myStops.end(); ++i) {
        ret->myStops.push_back(*i);
    }
    return ret;
}


bool
MSVehicle::running() const
{
    return myLane!=0;
}




void
MSVehicle::workOnMoveReminders(SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed,
                               MoveOnReminderMode mode)
{
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
    }
#endif
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (MoveReminderContIt rem=myMoveReminders.begin(); rem!=myMoveReminders.end();) {
        if (!(*rem)->isStillActive(*this, oldPos, newPos, newSpeed)) {
            rem = myMoveReminders.erase(rem);
        } else {
            ++rem;
        }
    }
    if (mode!=CURRENT) {
        OffsetVector::iterator off=myOldLaneMoveReminderOffsets.begin();
        for (MoveReminderContIt rem=myOldLaneMoveReminders.begin(); rem!=myOldLaneMoveReminders.end();) {
            SUMOReal oldLaneLength = *off;
            if (!(*rem)->isStillActive(*this, oldLaneLength+oldPos, oldLaneLength+newPos, newSpeed)) {
                rem = myOldLaneMoveReminders.erase(rem);
                off = myOldLaneMoveReminderOffsets.erase(off);
            } else {
                ++rem;
                ++off;
            }
        }
    }
}


void
MSVehicle::activateRemindersByEmitOrLaneChange()
{
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (MoveReminderContIt rem=myMoveReminders.begin(); rem!=myMoveReminders.end();) {
        if (!(*rem)->isActivatedByEmitOrLaneChange(*this)) {
            rem = myMoveReminders.erase(rem);
        } else {
            ++rem;
        }
    }
}


MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel()
{
    return *myLaneChangeModel;
}


const MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel() const
{
    return *myLaneChangeModel;
}


size_t
MSVehicle::getWaitingTime() const
{
    return myWaitingTime;
}


bool
MSVehicle::proceedVirtualReturnWhetherEnded(const MSEdge *const newEdge)
{
    bool myDestReached = destReached(newEdge);
    myAllowedLanes.clear(); // !!! not really necessary!?
    rebuildAllowedLanes();
    return myDestReached;
}


void
MSVehicle::onTripEnd(bool /*wasAlreadySet*/)
{
    SUMOReal pspeed = myState.mySpeed;
    SUMOReal pos = myState.myPos;
    SUMOReal oldPos = pos - SPEED2DIST(pspeed);
    if (pos - myType->getLength() < 0) {
        SUMOReal pdist = (SUMOReal)(myType->getLength() + 0.01) - oldPos;
        pspeed = DIST2SPEED(pdist);
        pos = (SUMOReal)(myType->getLength() + 0.1);
    }
    pos += myLane->length();
    oldPos += myLane->length();
    // process reminder
    // current
    vector< MSMoveReminder* >::iterator rem;
    for (rem=myMoveReminders.begin(); rem!=myMoveReminders.end(); ++rem) {
        // the vehicle may only be at the entry occupancy correction
        if ((*rem)->isStillActive(*this, oldPos, pos, pspeed)) {
            assert(false);
        }
    }
    // old
    rem = myOldLaneMoveReminders.begin();
    OffsetVector::iterator off = myOldLaneMoveReminderOffsets.begin();
    for (;rem!=myOldLaneMoveReminders.end(); ++rem, ++off) {
        SUMOReal oldLaneLength = *off;
        if ((*rem)->isStillActive(*this, oldPos+oldLaneLength, pos+oldLaneLength, pspeed)) {
            assert(false); // !!!
        }
    }
    // remove from structures to be informed about it
    for (QuitRemindedVector::iterator i=myQuitReminded.begin(); i!=myQuitReminded.end(); ++i) {
        (*i)->removeOnTripEnd(this);
    }
    myQuitReminded.clear();
    // remove c2c connections // !!! delete them ,too!!!
    {
        for (VehCont::iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            delete(*i).second;
        }
        myNeighbors.clear();
    }
    {
        for (ClusterCont::iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
            delete(*i);
        }
        clusterCont.clear();
    }
    {
        for (InfoCont::iterator i=infoCont.begin(); i!=infoCont.end(); ++i) {
            delete(*i).second;
        }
        infoCont.clear();
    }
}


void
MSVehicle::removeApproachingInformationOnKill()
{
    DriveItemVector::iterator i = myLFLinkLanes.begin();
    while (i!=myLFLinkLanes.end()&&(*i).myLink!=0/*&&(*i).myLink->getLane()!=begin&&(*i).myLink->getViaLane()!=begin*/) {
        MSLane *tmp = (*i).myLink->getLane();
        if (tmp!=0) {
            tmp->resetApproacherDistance(this);
        }
#ifdef HAVE_INTERNAL_LANES
        tmp = (*i).myLink->getViaLane();
        if (tmp!=0) {
            tmp->resetApproacherDistance(this);
        }
#endif
        ++i;
    }
}


void
MSVehicle::onDepart()
{
    // check whether the vehicle's departure time shall be saved
    myIntCORNMap[MSCORN::CORN_VEH_REALDEPART] = (int) MSNet::getInstance()->getCurrentTimeStep(); // !!!
    // check whether the vehicle control shall be informed
    if (MSCORN::wished(MSCORN::CORN_VEHCONTROL_WANTS_DEPARTURE_INFO)) {
        MSNet::getInstance()->getVehicleControl().vehicleEmitted(this);
    }
    // initialise devices
    {
        // cell phones
        if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_DEV_CPHONE)!=myPointerCORNMap.end()) {
            vector<MSDevice_CPhone*> *v = (vector<MSDevice_CPhone*>*) myPointerCORNMap[MSCORN::CORN_P_VEH_DEV_CPHONE];
            for (vector<MSDevice_CPhone*>::iterator i=v->begin(); i!=v->end(); ++i) {
                (*i)->onDepart();
            }
        }
    }
}


void
MSVehicle::quitRemindedEntered(MSVehicleQuitReminded *r)
{
    myQuitReminded.push_back(r);
}


void
MSVehicle::quitRemindedLeft(MSVehicleQuitReminded *r)
{
    QuitRemindedVector::iterator i = find(myQuitReminded.begin(), myQuitReminded.end(), r);
    if (i!=myQuitReminded.end()) {
        myQuitReminded.erase(i);
    }
}


int
MSVehicle::getCORNIntValue(MSCORN::Function f) const
{
    return myIntCORNMap.find(f)->second;
}


void *
MSVehicle::getCORNPointerValue(MSCORN::Pointer p) const
{
    return myPointerCORNMap.find(p)->second;
}


bool
MSVehicle::hasCORNIntValue(MSCORN::Function f) const
{
    return myIntCORNMap.find(f)!=myIntCORNMap.end();
}


bool
MSVehicle::hasCORNPointerValue(MSCORN::Pointer p) const
{
    return myPointerCORNMap.find(p)!=myPointerCORNMap.end();
}



const MSRoute &
MSVehicle::getRoute() const
{
    return *myRoute;
}


const MSRoute &
MSVehicle::getRoute(int index) const
{
    if (index==0) {
        return *myRoute;
    }
    std::map<MSCORN::Pointer, void*>::const_iterator i = myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE);
    assert(i!=myPointerCORNMap.end());
    const ReplacedRoutesVector * const v = (const ReplacedRoutesVector * const)(*i).second;
    assert((int) v->size()>index);
    return *((*v)[index].route);
}


bool
MSVehicle::replaceRoute(const MSEdgeVector &edges, SUMOTime simTime)
{
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        for (MSEdgeVector::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << "\n";
        for (MSRouteIterator i2=myRoute->begin(); i2!=myRoute->end(); ++i2) {
            DEBUG_OUT << (*i2)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << "\n";
    }
#endif
    MSRoute *otherr = 0;
    const MSEdge *currentEdge = *myCurrEdge;
    // check whether the information shall be saved
    if (MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
        otherr = new MSRoute(*myRoute);
    }
    // try to replace the current route
    bool replaced = myRoute->replaceBy(edges, myCurrEdge);
    if (replaced) {
        // rebuild in-vehicle route information
        myCurrEdge = myRoute->find(currentEdge);
        myAllowedLanes.clear();
        rebuildAllowedLanes();
        myLastBestLanesEdge = 0;
        // save information that the vehicle was rerouted
        myIntCORNMap[MSCORN::CORN_VEH_WASREROUTED] = 1;
        // ... maybe the route information shall be saved for output?
        if (MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
            RouteReplaceInfo rri(*myCurrEdge, simTime, otherr);
            if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE)==myPointerCORNMap.end()) {
                myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE] = new ReplacedRoutesVector();
            }
            ((ReplacedRoutesVector*) myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE])->push_back(rri);
        }
        myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] = 0;
        myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] =
            myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] + 1;
        myAllowedLanes.clear();
        rebuildAllowedLanes();
    }
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        for (MSRouteIterator i=myRoute->begin(); i!=myRoute->end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << "\n";
    }
#endif
    return replaced;
}


bool
MSVehicle::replaceRoute(MSRoute *newRoute, SUMOTime simTime)
{
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        for (MSEdgeVector::const_iterator i=newRoute->begin(); i!=newRoute->end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << "\n";
        for (MSRouteIterator i2=myRoute->begin(); i2!=myRoute->end(); ++i2) {
            DEBUG_OUT << (*i2)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << "\n";
    }
#endif
    MSRoute *otherr = 0;
    const MSEdge *currentEdge = *myCurrEdge;
    if (newRoute->find(currentEdge)==newRoute->end()) {
        return false;
    }
    // check whether the information shall be saved
    if (MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
        otherr = new MSRoute(*myRoute);
    }
    // try to replace the current route
    myRoute = newRoute;
    // rebuild in-vehicle route information
    myCurrEdge = myRoute->find(currentEdge);
    myAllowedLanes.clear();
    rebuildAllowedLanes();
    myLastBestLanesEdge = 0;
    // save information that the vehicle was rerouted
    myIntCORNMap[MSCORN::CORN_VEH_WASREROUTED] = 1;
    // ... maybe the route information shall be saved for output?
    if (MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
        RouteReplaceInfo rri(*myCurrEdge, simTime, otherr);
        if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE)==myPointerCORNMap.end()) {
            myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE] = new ReplacedRoutesVector();
        }
        ((ReplacedRoutesVector*) myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE])->push_back(rri);
        myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] = 0;
        myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] =
            myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] + 1;
    }
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        for (MSRouteIterator i=myRoute->begin(); i!=myRoute->end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << "\n";
    }
#endif
    assert((MSEdge*)succEdge(1)!=0);
    return true;
}


const MSVehicleType &
MSVehicle::getVehicleType() const
{
    return *myType;
}


void
MSVehicle::rebuildAllowedLanes()
{
    SUMOReal dist = 0;
    // check what was already computed
    for (NextAllowedLanes::const_iterator i=myAllowedLanes.begin(); i!=myAllowedLanes.end(); ++i) {
        dist += ((*(*i))[0])->length();
    }
    // compute next allowed lanes up to 1000m into the future
    SUMOReal MIN_DIST = 1000;
    if (dist<MIN_DIST) {
        size_t pos = distance(myRoute->begin(), myCurrEdge) + myAllowedLanes.size();
        if (pos>=myRoute->size()-1) {
            return;
        }
        const MSEdge::LaneCont *al = (*myRoute)[pos]->allowedLanes(*(*myRoute)[pos+1], myType->getVehicleClass());
        while (al!=0&&dist<MIN_DIST&&pos<myRoute->size()-1) {
            assert(al!=0);
            myAllowedLanes.push_back(al);
            pos++;
            if (pos<myRoute->size()-1) {
                dist += ((*al)[0])->length();
                al = (*myRoute)[pos]->allowedLanes(*(*myRoute)[pos+1], myType->getVehicleClass());
            }
        }
    }
}


const std::vector<std::vector<MSVehicle::LaneQ> > &
MSVehicle::getBestLanes() const
{
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, ((GUIVehicle*) this)->getGlID())) {
        int blb = 0;
    }
#endif
    if (myLastBestLanesEdge==myLane->getEdge()) {
        std::vector<LaneQ> &lanes = *myBestLanes.begin();
        std::vector<LaneQ>::iterator i;
        for (i=lanes.begin(); i!=lanes.end(); ++i) {
            SUMOReal v = 0;
            for (std::vector<MSLane*>::const_iterator j=(*lanes.begin()).joined.begin(); j!=(*lanes.begin()).joined.end(); ++j) {
                v += (*j)->getVehLenSum();
            }
            v += (*lanes.begin()).lane->getVehLenSum();
            (*lanes.begin()).v = v;
        }
        return myBestLanes;
    }

    myBestLanes.clear();
    myLastBestLanesEdge = myLane->getEdge();
    SUMOReal MIN_DIST = 3000;
    MSRouteIterator ce = myCurrEdge;
    int seen = 0;
    float dist = -(*myLastBestLanesEdge->getLanes())[0]->length();//-getPositionOnLane();
    // compute initial list
    // each item in the list is a list of lane descriptions
    while (seen<4&&dist<MIN_DIST&&ce!=myRoute->end()) {
        const MSEdge::LaneCont * const lanes = (*ce)->getLanes();
        myBestLanes.push_back(std::vector<LaneQ>());
        std::vector<LaneQ> &curr = *(myBestLanes.end()-1);
        bool gotOne = false;
        size_t i;
        for (i=0; i<lanes->size(); i++) {
            curr.push_back(LaneQ());
            LaneQ &currQ = *(curr.end()-1);
            if ((ce+1)!=myRoute->end()) {
                const MSEdge::LaneCont *allowed = (*ce)->allowedLanes(**(ce+1), myType->getVehicleClass());
                if (allowed!=0&&find(allowed->begin(), allowed->end(), (*lanes)[i])!=allowed->end()) {
                    currQ.t1 = true;
                    gotOne = true;
                } else {
                    currQ.t1 = false;
                }
            } else {
                currQ.t1 = true;
                gotOne = true;
            }

            currQ.length = (*lanes)[i]->length();
            currQ.alllength = (*lanes)[i]->length();

            if (!myStops.empty()&&myStops.begin()->lane->getEdge()==(*lanes)[i]->getEdge()) {
                if (myStops.begin()->lane!=(*lanes)[i]) {
                    currQ.length = 0;
                    currQ.alllength = 0;
                    currQ.t1 = false;
                }
            }

            currQ.lane = (*lanes)[i];
            currQ.hindernisPos = (*lanes)[i]->getDensity() * currQ.lane->length();
            currQ.v = (*lanes)[i]->getVehLenSum();
            currQ.wish = 1;
            currQ.dir = 0;
        }
        ce++;
        seen++;
        dist += (*lanes)[0]->length();
    }
    // sum up consecutive lengths
    {
        ce = myCurrEdge + myBestLanes.size() - 1;
        std::vector<std::vector<LaneQ> >::reverse_iterator i;
        for (i=myBestLanes.rbegin()+1; i!=myBestLanes.rend(); ++i, --ce) {
            std::vector<LaneQ> &curr = *i;
            size_t j;
            std::vector<int> bestNext;
            SUMOReal bestLength = -1;
            bool gotOne = false;
            for (j=0; j<curr.size(); ++j) {
                if (curr[j].length>bestLength) {
                    bestNext.clear();
                    bestLength = curr[j].length;
                    bestNext.push_back(j);
                } else if (curr[j].length==bestLength) {
                    bestNext.push_back(j);
                }
                if (!curr[j].t1) {
                    continue;
                }
                std::vector<LaneQ> &next = *(i-1);
                const MSLinkCont &lc = curr[j].lane->getLinkCont();
                bool oneFound = false;
                for (MSLinkCont::const_iterator k=lc.begin(); k!=lc.end()&&!oneFound; ++k) {
                    MSLane *c = (*k)->getLane();
                    for (std::vector<LaneQ>::iterator l=next.begin(); l!=next.end()&&!oneFound; ++l) {
                        if ((*l).lane==c/*&&curr[j].t1*/&&(*l).t1) {
                            gotOne = true;
                            /*
                            (*l).length += next[j].length;
                            (*l).v += next[j].v;
                            (*l).wish++;
                            (*l).alllength += next[j].alllength;
                            */
                            oneFound = true;
                            curr[j].length += (*l).length;
                            curr[j].v += (*l).v;
                            curr[j].wish++;// += (*l).length;
                            curr[j].alllength = (*l).alllength;
                            if ((*l).joined.size()!=0) {
                                copy((*l).joined.begin(), (*l).joined.end(), back_inserter(curr[j].joined));
                            } else {
                                (*l).joined.push_back((*l).lane);
                            }
                        }
                    }
                }
            }
            if (!gotOne) {
                // ok, there was no direct matching connection
                // first hack: get the first to the next edge
                for (j=0; j<curr.size(); ++j) {
                    if (!curr[j].t1) {
                        continue;
                    }
                    std::vector<LaneQ> &next = *(i-1);
                    const MSLinkCont &lc = curr[j].lane->getLinkCont();
                    bool oneFound = false;
                    for (MSLinkCont::const_iterator k=lc.begin(); k!=lc.end()&&!oneFound; ++k) {
                        MSLane *c = (*k)->getLane();
                        for (std::vector<LaneQ>::iterator l=next.begin(); l!=next.end(); ++l) {
                            if ((*l).lane==c/*&&curr[j].t1&&(*l).t1*/) {
                                /*
                                cout << "c3111 " << "\n";
                                (*l).length += next[j].lane->length();//.length;
                                cout << "c3112 " << "\n";
                                (*l).v += next[j].lane->getDensity();//;
                                cout << "c3113 " << "\n";
                                (*l).wish++;
                                cout << "c3114 " << "\n";
                                (*l).alllength += next[j].lane->length();//.alllength;
                                cout << "c3115 " << "\n";
                                */
                                curr[j].length += (*l).lane->length();//.length;
                                curr[j].v += (*l).lane->getDensity();//.v;
                                curr[j].wish++;// += (*l).length;
                                curr[j].alllength = (*l).alllength;
                                (*l).joined.push_back((*l).lane);
                            }
                        }
                    }
                }
            }
        }
    }
    // compute moving direction
    {
        // !!! optimize: maybe only for the current edge
        std::vector<std::vector<LaneQ> >::iterator i;
        for (i=myBestLanes.begin(); i!=myBestLanes.end(); ++i) {
            std::vector<LaneQ> &curr = *i;
            int best = 0;
            SUMOReal bestLength = 0;
            size_t j;
            for (j=0; j<curr.size(); ++j) {
                if (curr[j].length>bestLength) {
                    bestLength = curr[j].length;
                    best = j;
                }
            }
            for (j=0; j<curr.size(); ++j) {
                curr[j].dir = best-j;
            }
        }
    }
    return myBestLanes;

}


void
MSVehicle::writeXMLRoute(OutputDevice &os, int index) const
{
    MSRoute *route2Write = myRoute;
    // check if a previous route shall be written
    os << "      <route";
    if (index>=0) {
        std::map<MSCORN::Pointer, void*>::const_iterator i = myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE);
        assert(i!=myPointerCORNMap.end());
        const ReplacedRoutesVector *v = (const ReplacedRoutesVector *)(*i).second;
        assert((int) v->size()>index);
        // write edge on which the vehicle was when the route was valid
        os << " replacedOnEdge=\"" << (*v)[index].edge->getID() << "\" ";
        // write the time at which the route was replaced
        os << " replacedAtTime=\"" << (*v)[index].time << "\"";
        // get the route
        route2Write = (*v)[index].route;
    }
    os << ">";
    // write the route
    route2Write->writeEdgeIDs(os);
    os << "</route>" << "\n";
}


void
MSVehicle::setCORNColor(SUMOReal red, SUMOReal green, SUMOReal blue)
{
    if (hasCORNPointerValue(MSCORN::CORN_P_VEH_OWNCOL)) {
        delete(RGBColor *) myPointerCORNMap[MSCORN::CORN_P_VEH_OWNCOL];
    }
    myPointerCORNMap[MSCORN::CORN_P_VEH_OWNCOL] = new RGBColor(red, green, blue);
}


#ifdef HAVE_MESOSIM
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include "MSGlobals.h"
#endif

void
MSVehicle::saveState(std::ostream &os)
{
    FileHelpers::writeString(os, myID);
    FileHelpers::writeInt(os, myLastLaneChangeOffset);
    FileHelpers::writeUInt(os, myWaitingTime);
    FileHelpers::writeInt(os, myRepetitionNumber);
    FileHelpers::writeInt(os, myPeriod);
    FileHelpers::writeString(os, myRoute->getID());
    FileHelpers::writeUInt(os, myDesiredDepart);
    FileHelpers::writeString(os, myType->getID());
    FileHelpers::writeUInt(os, myRoute->posInRoute(myCurrEdge));
    FileHelpers::writeUInt(os, (unsigned int) getCORNIntValue(MSCORN::CORN_VEH_REALDEPART));
#ifdef HAVE_MESOSIM
    // !!! several things may be missing
    if (seg==0) {
        FileHelpers::writeUInt(os, 0);
        FileHelpers::writeFloat(os, tEvent);
        FileHelpers::writeFloat(os, tLastEntry);
    } else {
        FileHelpers::writeUInt(os, seg->get_index());
        FileHelpers::writeFloat(os, tEvent);
        FileHelpers::writeFloat(os, tLastEntry);
    }
    FileHelpers::writeByte(os, inserted);
#endif
}




void
MSVehicle::addVehNeighbors(MSVehicle *veh, SUMOTime time)
{
    if (&veh->getLane()==0||myLane==0) {
        // obviously, one of the vehicles is being teleported
        return;
    }

    // check whether the other vehicle is in range
    if (computeDistance(this, veh)) {
        Position2D pos1 = getPosition();
        Position2D pos2 = veh->getPosition();
        std::map<MSVehicle * const, C2CConnection*>::iterator i = myNeighbors.find(veh);
        if (i==myNeighbors.end()) {
            // the vehicles will establish a new connection
            C2CConnection *con = new C2CConnection;
            con->connectedVeh = veh;
            con->state = disconnected;
//			con->timeSinceSeen = 1;
//			con->timeSinceConnect = 0;
            con->lastTimeSeen = time;
            myNeighbors[veh] = con;
            // the other car must inform THIS vehicle if it's removed from the network
            veh->quitRemindedEntered(this);
        } else {
            // ok, the vehicles already interact
            //  increment the connection time
            (*i).second->lastTimeSeen = time;
        }
        MSCORN::saveVehicleInRangeData(time, getID(), veh->getID(),
                                       pos1.x(),pos1.y(), pos2.x(),pos2.y());
    }
}


void
MSVehicle::cleanUpConnections(SUMOTime time)
{
    std::vector<MSVehicle *> toErase;
    std::map<MSVehicle * const, C2CConnection*>::iterator i;
    // recheck connections
    for (i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
        MSVehicle * const neigh = (*i).first;
        if ((*i).second->lastTimeSeen != time) {
            // the vehicle is not longer in range
            toErase.push_back(neigh);
            // the other vehicle must no longer inform us about being removed from the network
            neigh->quitRemindedLeft(this);
        }/* !!! else {
                                            // the vehicle is still in range
                                            ((*i).second->timeSinceSeen)++;
                                            if(((*i).second->state!=dialing) && ((*i).second->state!=disconnected)){
                                	            ((*i).second->timeSinceConnect)++;
                                            }
                                        }*/
    }

    // go through the list of invalid connections, erase them
    for (vector<MSVehicle *>::iterator j=toErase.begin(); j!=toErase.end(); ++j) {
        i = myNeighbors.find(*j);
        delete(*i).second;
        myNeighbors.erase(i);
    }
}


bool
MSVehicle::computeDistance(MSVehicle* veh1, MSVehicle* veh2)
{
    bool b = false;
    Position2D pos1 = veh1->getPosition();
    Position2D pos2 = veh2->getPosition();
    if (pos1.x()==-1000||pos2.x()==-1000) {
        return false;
    }
    if (fabs(pos1.x()-pos2.x())<MSGlobals::gLANRange && fabs(pos1.y()-pos2.y())<MSGlobals::gLANRange) {
        SUMOReal distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
        if ((distance>0)&&(distance<=MSGlobals::gLANRange)) {
            b = true;
        }
    }
    return b;
}


void
MSVehicle::updateInfos(SUMOTime time)
{
    // first, count how long the vehicle is waiting at the same position
    if (myState.speed()<1.) {
        timeSinceStop++;
    } else {
        timeSinceStop = 0;
    }

    // second, save the information as "Congestion", if the vehicle is at the same
    // position longer as 2 minutes
    if (timeSinceStop > 120) {
        SUMOReal timeByMeanSpeed1 = (SUMOReal) timeSinceStop;
        if (akt!=0&&myLane!=0) {
            SUMOReal neededTime = (SUMOReal)(MSNet::getInstance()->getCurrentTimeStep() - akt->time);
            timeByMeanSpeed1 = myLane->length() / (myState.pos()/neededTime);
        }
        SUMOReal estimatedTime = timeByMeanSpeed1;
        map<const MSEdge * const, Information *>::iterator i = infoCont.find(*myCurrEdge);
        if (i == infoCont.end()) {
            // this is a new information about an edge where speed is too low
            Information *info = new Information(estimatedTime, time);
            infoCont[*myCurrEdge] = info;
            MSCORN::saveSavedInformationData(time, getID(), (*myCurrEdge)->getID(), "congestion", info->time, 0);
            totalNrOfSavedInfos++;
        } else {
            // this edge is already known as being too slow
            // - replace prior information by own
            (*i).second->neededTime = estimatedTime;
            (*i).second->time = time;
        }
    }

    // remove information older than wished
    std::vector<const MSEdge * > toErase;
    std::map<const MSEdge * const, Information*>::iterator j = infoCont.begin();
    for (; j!= infoCont.end(); ++j) {
        if ((*j).second->time < time - MSGlobals::gLANRefuseOldInfosOffset) {
            toErase.push_back((*j).first);
        }
    }
    // go through the list of invalid information, erase them
    for (vector<const MSEdge *>::iterator k=toErase.begin(); k!=toErase.end(); ++k) {
        infoCont.erase(infoCont.find(*k));
    }
}


void
MSVehicle::removeOnTripEnd(MSVehicle *veh)
{
    assert(myNeighbors.find(veh)!=myNeighbors.end());
    std::map<MSVehicle * const, C2CConnection*>::iterator i = myNeighbors.find(veh);
    delete(*i).second;
    myNeighbors.erase(i);
    quitRemindedLeft(veh);
}


bool
MSVehicle::knowsEdgeTest(MSEdge &edge) const
{
    return infoCont.find(&edge)!=infoCont.end();
}


const MSVehicle::VehCont &
MSVehicle::getConnections() const
{
    return myNeighbors;
}


void
MSVehicle::setClusterId(int Id)
{
    clusterId = Id;
}


int
MSVehicle::getClusterId(void) const
{
    return clusterId;
}


int
MSVehicle::buildMyCluster(int myStep, int clId)
{
    int count = 1;
    // build the cluster
    {
        clusterId = clId;
        std::map<MSVehicle * const, C2CConnection*>::iterator i;
        for (i=myNeighbors.begin(); i!=myNeighbors.end(); i++) {
            if ((*i).first->getClusterId()<0) {
                count++;
                (*i).second->connectedVeh->setClusterId(clId);
                clusterCont.push_back((*i).second);
                std::map<MSVehicle * const, C2CConnection*>::iterator j;
                for (j=(*i).first->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++) {
                    if ((*i).first->getClusterId()<0) {
                        count++;
                        (*i).first->setClusterId(clId);
                        clusterCont.push_back((*j).second);
                    }
                }
            } else if ((*i).second->connectedVeh->getClusterId()==clusterId) {
                // du bist zwar mein Nachbarn, aber du wrdest von einem anderen Nachbarn von mir schon eingeladen,
                // dann werde ich deine nachbarn einladen.
                std::map<MSVehicle * const, C2CConnection*>::iterator j;
                for (j=(*i).first->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++) {
                    if ((*i).first->getClusterId()<0) {
                        count++;
                        (*i).first->setClusterId(clId);
                        clusterCont.push_back((*j).second);
                    }
                }
            }
        }
    }

    // write output
    {
        ostringstream vehs;
        for (std::vector<C2CConnection*>::const_iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
            if (i!=clusterCont.begin()) {
                vehs << ' ';
            }
            vehs << (*i)->connectedVeh->getID();
        }
        MSCORN::saveClusterInfoData(myStep, clId, getID(), vehs.str(), count);
    }
    return count;
}


void
MSVehicle::sendInfos(SUMOTime time)
{
    // the number of possible packets
    size_t numberOfSendingPos = (size_t) MSGlobals::gNumberOfSendingPos; // 732
    // the number of information per packet
    size_t infoPerPaket = (size_t) MSGlobals::gInfoPerPaket; // 14
    size_t numberOfInfo = numberOfSendingPos*infoPerPaket; // 10248

    if (infoCont.size()>0 && numberOfSendingPos>0) {
        // send information to direct neighbors
        for (VehCont::const_iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            // compute the number of netto-sendable packets
            // !!! maybe it would be prettier to have computed this by the retriever
            size_t nofP = numOfInfos(this, (*i).first);
            if (nofP>numberOfInfo) {
                nofP = (int) numberOfInfo;
            }
            // send the computed number of information to the neighbor
            (*i).first->transferInformation(getID(), infoCont, nofP, time);
        }
        // reduce the number of packets that still may be sent
        size_t sentBruttoP = MIN2((size_t) ceil((SUMOReal)(infoCont.size())/(SUMOReal) infoPerPaket), (size_t) numberOfSendingPos);
        myNoSent += sentBruttoP;
        numberOfSendingPos = numberOfSendingPos - sentBruttoP;
        numberOfInfo = numberOfInfo - infoCont.size();
    }

    if (numberOfInfo<=0) {
        return;
    }

    // now let all neighbors send
    for (ClusterCont::const_iterator o=clusterCont.begin(); o!=clusterCont.end()&&numberOfInfo>0&&numberOfSendingPos>0; ++o) {
        if ((*o)->connectedVeh->infoCont.size()>0 && numberOfSendingPos>0) {
            for (VehCont::const_iterator j=(*o)->connectedVeh->myNeighbors.begin(); j!=(*o)->connectedVeh->myNeighbors.end(); ++j) {
                // compute the number of netto-sendable packets
                // !!! maybe it would be prettier to have computed this by the retriever
                size_t nofP = numOfInfos((*j).second->connectedVeh,(*o)->connectedVeh);
                if (nofP>numberOfInfo) {
                    nofP = (int) numberOfInfo;
                }
                // send the computed number of information to the neighbor
                (*j).second->connectedVeh->transferInformation((*o)->connectedVeh->getID(),(*o)->connectedVeh->infoCont, nofP, time);
            }
            size_t sentBruttoP = MIN2((size_t) ceil((SUMOReal)((*o)->connectedVeh->infoCont.size())/(SUMOReal) infoPerPaket), (size_t) numberOfSendingPos);
            numberOfSendingPos = numberOfSendingPos - sentBruttoP;
            numberOfInfo = numberOfInfo - (*o)->connectedVeh->infoCont.size();
        }
    }
    clusterCont.clear();
}


size_t
MSVehicle::numOfInfos(MSVehicle *veh1, MSVehicle* veh2)
{
    Position2D pos1 = veh1->getPosition();
    Position2D pos2 = veh2->getPosition();
    SUMOReal distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
    SUMOReal x = (SUMOReal)(((-2.3*distance + 1650.)*MSGlobals::gNumberOfSendingPos)/1500.);  //approximation function
    return (int)(x*MSGlobals::gInfoPerPaket);
}


bool
MSVehicle::willPass(const MSEdge * const edge) const
{
    return find(myCurrEdge, myRoute->end(), edge)!=myRoute->end();
}


void
MSVehicle::transferInformation(const std::string &senderID, const InfoCont &infos,
                               int NofP, SUMOTime currentTime)
{
    if (NofP>0&&infos.size()>0) {
        myLastInfoTime = currentTime;
    }
    int count = 0;
    std::map<const MSEdge * const, Information *>::const_iterator i;
    // go through the saved information
    for (i=infos.begin(); i!=infos.end() && count<NofP; ++i) {
        std::map<const MSEdge * const, Information *>::iterator j = infoCont.find((*i).first);
        if (j==infoCont.end()) {
            if ((*i).second->neededTime > 0) {
                infoCont[(*i).first] = new Information(*(*i).second);
                ++myNoGot;
            }
        } else if (((*i).second->time > (*j).second->time) && (*i).second->neededTime > 0) {
            // save the information about a previously known edge
            //  (it is newer than the stored)
            delete infoCont[(*i).first];
            infoCont[(*i).first] = new Information(*(*i).second);
            ++myNoGot;
        }
        count++;
        MSCORN::saveTransmittedInformationData(MSNet::getInstance()->getCurrentTimeStep(),senderID,getID(),(*i).first->getID(),(*i).second->time,(*i).second->neededTime);
        // if the edge is on the route, mark that a relevant information has been added
        bool bWillPass = willPass((*i).first);
        if (bWillPass) {
            myHaveRouteInfo = true;
            ++myNoGotRelevant;
        }
    }
}


SUMOReal
MSVehicle::getC2CEffort(const MSEdge * const e, SUMOTime /*t*/) const
{
    if (infoCont.find(e)==infoCont.end()) {
        return -1;
    }
    return infoCont.find(e)->second->neededTime;
}


void
MSVehicle::checkReroute(SUMOTime t)
{
    // do not try to reroute when no new information is available
    if (myLastInfoTime!=t) {
        return;
    }
    // do not try to reroute when no information about the own route is available
    if (!myHaveRouteInfo) {
        return;
    }
    // try to reroute
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, ((GUIVehicle*) this)->getGlID())) {
        int blb = 0;
    }
#endif

    if (myStops.size()==0) {
        myHaveRouteInfo = false;
        // check whether to reroute
        SUMODijkstraRouter<MSEdge, MSVehicle, prohibited_withRestrictions<MSEdge, MSVehicle>, MSEdge> router(MSEdge::dictSize(), true, &MSEdge::getC2CEffort);
        std::vector<const MSEdge*> edges;
        router.compute(*myCurrEdge, myRoute->getLastEdge(), (const MSVehicle * const) this,
                       MSNet::getInstance()->getCurrentTimeStep(), edges);
        // check whether the new route is the same as the prior
        MSRouteIterator ri = myCurrEdge;
        std::vector<const MSEdge*>::iterator ri2 = edges.begin();
        while (ri!=myRoute->end()&&ri2!=edges.end()&&*ri==*ri2) {
            ri++;
            ri2++;
        }
        if (ri!=myRoute->end()||ri2!=edges.end()) {
            int rerouteIndex = 0;
            if (myIntCORNMap.find(MSCORN::CORN_VEH_NUMBERROUTE)!=myIntCORNMap.end()) {
                rerouteIndex = (int) myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
            }
            string nid = myRoute->getID() + "#" + toString(rerouteIndex);
            MSRoute *rep = new MSRoute(nid, edges, true);
            if (!MSRoute::dictionary(nid, rep)) {
                //cout << "Error: Could not insert route ''" << "\n";
            } else {
                MSCORN::setWished(MSCORN::CORN_VEH_SAVEREROUTING);
                replaceRoute(rep, MSNet::getInstance()->getCurrentTimeStep());
            }
        }
    }
}


size_t
MSVehicle::getNoGot() const
{
    return myNoGot;
}


size_t
MSVehicle::getNoSent() const
{
    return myNoSent;
}


size_t
MSVehicle::getNoGotRelevant() const
{
    return myNoGotRelevant;
}



/****************************************************************************/

