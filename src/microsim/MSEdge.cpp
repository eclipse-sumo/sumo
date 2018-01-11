/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSEdge.cpp
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Sascha Krieg
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// A road/street connecting two junctions
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <iostream>
#include <cassert>
#include <utils/common/StringTokenizer.h>
#include <utils/options/OptionsCont.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#include <mesosim/MEVehicle.h>
#include "MSInsertionControl.h"
#include "MSJunction.h"
#include "MSLane.h"
#include "MSLaneChanger.h"
#include "MSLaneChangerSublane.h"
#include "MSGlobals.h"
#include "MSNet.h"
#include "MSVehicle.h"
#include "MSLeaderInfo.h"
#include "MSContainer.h"
#include "MSEdgeWeightsStorage.h"
#include "MSEdge.h"

#define BEST_LANE_LOOKAHEAD 3000.0

// ===========================================================================
// static member definitions
// ===========================================================================
MSEdge::DictType MSEdge::myDict;
MSEdgeVector MSEdge::myEdges;


// ===========================================================================
// member method definitions
// ===========================================================================
MSEdge::MSEdge(const std::string& id, int numericalID,
               const SumoXMLEdgeFunc function,
               const std::string& streetName,
               const std::string& edgeType,
               int priority) :
    Named(id), myNumericalID(numericalID), myLanes(0),
    myLaneChanger(0), myFunction(function), myVaporizationRequests(0),
    myLastFailedInsertionTime(-1),
    myFromJunction(0), myToJunction(0),
    myStreetName(streetName),
    myEdgeType(edgeType),
    myPriority(priority),
    myWidth(0),
    myLength(-1.),
    myEmptyTraveltime(-1.),
    myAmDelayed(false),
    myAmRoundabout(false),
    myAmFringe(true) {
}


MSEdge::~MSEdge() {
    delete myLaneChanger;
    for (AllowedLanesCont::iterator i1 = myAllowed.begin(); i1 != myAllowed.end(); i1++) {
        delete(*i1).second;
    }
    for (ClassedAllowedLanesCont::iterator i2 = myClassedAllowed.begin(); i2 != myClassedAllowed.end(); i2++) {
        for (AllowedLanesCont::iterator i1 = (*i2).second.begin(); i1 != (*i2).second.end(); i1++) {
            delete(*i1).second;
        }
    }
    delete myLanes;
    // Note: Lanes are delete using MSLane::clear();
}


void
MSEdge::initialize(const std::vector<MSLane*>* lanes) {
    assert(lanes != 0);
    myLanes = lanes;
    if (myFunction == EDGEFUNC_CONNECTOR) {
        myCombinedPermissions = SVCAll;
    }
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        myWidth += (*i)->getWidth();
    }
    double widthBefore = 0;
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        (*i)->setRightSideOnEdge(widthBefore, (int)mySublaneSides.size());
        MSLeaderInfo ahead(*i);
        for (int j = 0; j < ahead.numSublanes(); ++j) {
            mySublaneSides.push_back(widthBefore + j * MSGlobals::gLateralResolution);
        }
        widthBefore += (*i)->getWidth();
    }
}


void MSEdge::recalcCache() {
    if (myLanes->empty()) {
        return;
    }
    myLength = myLanes->front()->getLength();
    myEmptyTraveltime = myLength / MAX2(getSpeedLimit(), NUMERICAL_EPS);

    if (MSGlobals::gMesoTLSPenalty > 0 || MSGlobals::gMesoMinorPenalty > 0) {
        // add tls penalties to the minimum travel time
        SUMOTime minPenalty = -1;
        for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
            MSLane* l = *i;
            const MSLinkCont& lc = l->getLinkCont();
            for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); ++j) {
                MSLink* link = *j;
                SUMOTime linkPenalty = link->getMesoTLSPenalty() + (link->havePriority() ? 0 : MSGlobals::gMesoMinorPenalty);
                if (minPenalty == -1) {
                    minPenalty = linkPenalty;
                } else {
                    minPenalty = MIN2(minPenalty, linkPenalty);
                }
            }
        }
        if (minPenalty > 0) {
            myEmptyTraveltime += STEPS2TIME(minPenalty);
        }
    }
}


void
MSEdge::closeBuilding() {
    myAllowed[0] = new std::vector<MSLane*>();
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        myAllowed[0]->push_back(*i);
        const MSLinkCont& lc = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); ++j) {
            (*j)->initParallelLinks();
            MSLane* toL = (*j)->getLane();
            if (toL != 0) {
                MSEdge& to = toL->getEdge();
                //
                if (std::find(mySuccessors.begin(), mySuccessors.end(), &to) == mySuccessors.end()) {
                    mySuccessors.push_back(&to);
                }
                if (std::find(to.myPredecessors.begin(), to.myPredecessors.end(), this) == to.myPredecessors.end()) {
                    to.myPredecessors.push_back(this);
                }
                //
                if (myAllowed.find(&to) == myAllowed.end()) {
                    myAllowed[&to] = new std::vector<MSLane*>();
                }
                myAllowed[&to]->push_back(*i);
                if ((*j)->getDirection() != LINKDIR_TURN) {
                    myAmFringe = false;
                }
            }
            toL = (*j)->getViaLane();
            if (toL != 0) {
                MSEdge& to = toL->getEdge();
                if (std::find(to.myPredecessors.begin(), to.myPredecessors.end(), this) == to.myPredecessors.end()) {
                    to.myPredecessors.push_back(this);
                }
            }
        }
    }
    std::sort(mySuccessors.begin(), mySuccessors.end(), by_id_sorter());
    rebuildAllowedLanes();
    recalcCache();
    // segment building depends on the finished list of successors (for multi-queue)
    if (MSGlobals::gUseMesoSim && !myLanes->empty()) {
        MSGlobals::gMesoNet->buildSegmentsFor(*this, OptionsCont::getOptions());
    }
}


void
MSEdge::buildLaneChanger() {
    if (!myLanes->empty()) {
        const bool allowChanging = allowsLaneChanging();
        if (MSGlobals::gLateralResolution > 0) {
            // may always initiate sublane-change
            myLaneChanger = new MSLaneChangerSublane(myLanes, allowChanging);
        } else {
            if (MSGlobals::gLaneChangeDuration > 0) {
                myLaneChanger = new MSLaneChanger(myLanes, allowChanging);
            } else if (myLanes->size() > 1 || canChangeToOpposite()) {
                myLaneChanger = new MSLaneChanger(myLanes, allowChanging);
            }
        }
    }
}


bool
MSEdge::allowsLaneChanging() {
    if (isInternal()) {
        // allow changing only if all links leading to this internal lane have priority
        // or they are controlled by a traffic light
        for (std::vector<MSLane*>::const_iterator it = myLanes->begin(); it != myLanes->end(); ++it) {
            MSLane* pred = (*it)->getLogicalPredecessorLane();
            MSLink* link = MSLinkContHelper::getConnectingLink(*pred, **it);
            assert(link != 0);
            LinkState state = link->getState();
            if (state == LINKSTATE_MINOR
                    || state == LINKSTATE_EQUAL
                    || state == LINKSTATE_STOP
                    || state == LINKSTATE_ALLWAY_STOP
                    || state == LINKSTATE_DEADEND) {
                return false;
            }
        }
    }
    return true;
}


void
MSEdge::rebuildAllowedLanes() {
    // clear myClassedAllowed.
    // it will be rebuilt on demand
    for (ClassedAllowedLanesCont::iterator i2 = myClassedAllowed.begin(); i2 != myClassedAllowed.end(); i2++) {
        for (AllowedLanesCont::iterator i1 = (*i2).second.begin(); i1 != (*i2).second.end(); i1++) {
            delete(*i1).second;
        }
    }
    myClassedAllowed.clear();
    myClassesSuccessorMap.clear();
    // rebuild myMinimumPermissions and myCombinedPermissions
    myMinimumPermissions = SVCAll;
    myCombinedPermissions = 0;
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        myMinimumPermissions &= (*i)->getPermissions();
        myCombinedPermissions |= (*i)->getPermissions();
    }
}


// ------------ Access to the edge's lanes
MSLane*
MSEdge::leftLane(const MSLane* const lane) const {
    return parallelLane(lane, 1);
}


MSLane*
MSEdge::rightLane(const MSLane* const lane) const {
    return parallelLane(lane, -1);
}


MSLane*
MSEdge::parallelLane(const MSLane* const lane, int offset) const {
    const int index = (int)(find(myLanes->begin(), myLanes->end(), lane) - myLanes->begin());
    if (index == (int)myLanes->size()) {
        return 0;
    }
    const int resultIndex = index + offset;
    if (resultIndex >= (int)myLanes->size() || resultIndex < 0) {
        return 0;
    } else {
        return (*myLanes)[resultIndex];
    }
}


const std::vector<MSLane*>*
MSEdge::allowedLanes(const MSEdge& destination, SUMOVehicleClass vclass) const {
    return allowedLanes(&destination, vclass);
}


const std::vector<MSLane*>*
MSEdge::allowedLanes(SUMOVehicleClass vclass) const {
    return allowedLanes(0, vclass);
}


const std::vector<MSLane*>*
MSEdge::getAllowedLanesWithDefault(const AllowedLanesCont& c, const MSEdge* dest) const {
    AllowedLanesCont::const_iterator it = c.find(dest);
    if (it == c.end()) {
        return 0;
    }
    return it->second;
}


const std::vector<MSLane*>*
MSEdge::allowedLanes(const MSEdge* destination, SUMOVehicleClass vclass) const {
    if (destination == 0 && (myMinimumPermissions & vclass) == vclass) {
        // all lanes allow vclass
        return getAllowedLanesWithDefault(myAllowed, destination);
    }
    // look up cached result in myClassedAllowed
    ClassedAllowedLanesCont::const_iterator i = myClassedAllowed.find(vclass);
    if (i != myClassedAllowed.end()) {
        // can use cached value
        const AllowedLanesCont& c = (*i).second;
        return getAllowedLanesWithDefault(c, destination);
    } else {
        // this vclass is requested for the first time. rebuild all destinations
        // go through connected edges
#ifdef HAVE_FOX
        if (MSDevice_Routing::isParallel()) {
            MSDevice_Routing::lock();
        }
#endif
        for (AllowedLanesCont::const_iterator i1 = myAllowed.begin(); i1 != myAllowed.end(); ++i1) {
            const MSEdge* edge = i1->first;
            const std::vector<MSLane*>* lanes = i1->second;
            myClassedAllowed[vclass][edge] = new std::vector<MSLane*>();
            // go through lanes approaching current edge
            for (std::vector<MSLane*>::const_iterator i2 = lanes->begin(); i2 != lanes->end(); ++i2) {
                // origin lane allows the current vehicle class?
                if ((*i2)->allowsVehicleClass(vclass)) {
                    if (edge == 0) {
                        myClassedAllowed[vclass][edge]->push_back(*i2);
                    } else {
                        // target lane allows the current vehicle class?
                        const MSLinkCont& lc = (*i2)->getLinkCont();
                        for (MSLinkCont::const_iterator it_link = lc.begin(); it_link != lc.end(); ++it_link) {
                            const MSLane* targetLane = (*it_link)->getLane();
                            if ((&(targetLane->getEdge()) == edge) && targetLane->allowsVehicleClass(vclass)) {
                                // -> may be used
                                myClassedAllowed[vclass][edge]->push_back(*i2);
                                break;
                            }
                        }
                    }
                }
            }
            // assert that 0 is returned if no connection is allowed for a class
            if (myClassedAllowed[vclass][edge]->size() == 0) {
                delete myClassedAllowed[vclass][edge];
                myClassedAllowed[vclass][edge] = 0;
            }
        }
#ifdef HAVE_FOX
        if (MSDevice_Routing::isParallel()) {
            MSDevice_Routing::unlock();
        }
#endif
        return myClassedAllowed[vclass][destination];
    }
}


// ------------
SUMOTime
MSEdge::incVaporization(SUMOTime) {
    ++myVaporizationRequests;
    return 0;
}


SUMOTime
MSEdge::decVaporization(SUMOTime) {
    --myVaporizationRequests;
    return 0;
}


MSLane*
MSEdge::getFreeLane(const std::vector<MSLane*>* allowed, const SUMOVehicleClass vclass, double departPos) const {
    if (allowed == 0) {
        allowed = allowedLanes(vclass);
    }
    MSLane* res = 0;
    if (allowed != 0) {
        double largestGap = 0;
        MSLane* resByGap = 0;
        double leastOccupancy = std::numeric_limits<double>::max();;
        for (std::vector<MSLane*>::const_iterator i = allowed->begin(); i != allowed->end(); ++i) {
            const double occupancy = (*i)->getBruttoOccupancy();
            if (occupancy < leastOccupancy) {
                res = (*i);
                leastOccupancy = occupancy;
            }
            const MSVehicle* last = (*i)->getLastFullVehicle();
            const double lastGap = (last != 0 ? last->getPositionOnLane() : myLength) - departPos;
            if (lastGap > largestGap) {
                largestGap = lastGap;
                resByGap = (*i);
            }
        }
        if (resByGap != 0) {
            //if (res != resByGap) std::cout << SIMTIME << " edge=" << getID() << " departPos=" << departPos << " res=" << Named::getIDSecure(res) << " resByGap=" << Named::getIDSecure(resByGap) << " largestGap=" << largestGap << "\n";
            res = resByGap;
        }
    }
    return res;
}

double
MSEdge::getDepartPosBound(const MSVehicle& veh, bool upper) const {
    const SUMOVehicleParameter& pars = veh.getParameter();
    double pos = getLength();
    // determine the position
    switch (pars.departPosProcedure) {
        case DEPART_POS_GIVEN:
            pos = pars.departPos;
            if (pos < 0.) {
                pos += myLength;
            }
            break;
        case DEPART_POS_RANDOM:
            // could be any position on the edge
            break;
        case DEPART_POS_RANDOM_FREE:
            // could be any position on the edge due to multiple random attempts
            break;
        case DEPART_POS_FREE:
            // many candidate positions, upper bound could be computed exactly
            // with much effort
            break;
        case DEPART_POS_LAST:
            if (upper) {
                for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
                    MSVehicle* last = (*i)->getLastFullVehicle();
                    if (last != 0) {
                        pos = MIN2(pos, last->getPositionOnLane());
                    }
                }
            } else {
                pos = 0;
            }
        case DEPART_POS_BASE:
        case DEPART_POS_DEFAULT:
            break;
        default:
            pos = MIN2(pos, veh.getVehicleType().getLength());
            break;
    }
    return pos;
}


MSLane*
MSEdge::getDepartLane(MSVehicle& veh) const {
    switch (veh.getParameter().departLaneProcedure) {
        case DEPART_LANE_GIVEN:
            if ((int) myLanes->size() <= veh.getParameter().departLane || !(*myLanes)[veh.getParameter().departLane]->allowsVehicleClass(veh.getVehicleType().getVehicleClass())) {
                return 0;
            }
            return (*myLanes)[veh.getParameter().departLane];
        case DEPART_LANE_RANDOM:
            return RandHelper::getRandomFrom(*allowedLanes(veh.getVehicleType().getVehicleClass()));
        case DEPART_LANE_FREE:
            return getFreeLane(0, veh.getVehicleType().getVehicleClass(), getDepartPosBound(veh, false));
        case DEPART_LANE_ALLOWED_FREE:
            if (veh.getRoute().size() == 1) {
                return getFreeLane(0, veh.getVehicleType().getVehicleClass(), getDepartPosBound(veh, false));
            } else {
                return getFreeLane(allowedLanes(**(veh.getRoute().begin() + 1), veh.getVehicleType().getVehicleClass()), veh.getVehicleType().getVehicleClass(), getDepartPosBound(veh, false));
            }
        case DEPART_LANE_BEST_FREE: {
            veh.updateBestLanes(false, myLanes->front());
            const std::vector<MSVehicle::LaneQ>& bl = veh.getBestLanes();
            double bestLength = -1;
            for (std::vector<MSVehicle::LaneQ>::const_iterator i = bl.begin(); i != bl.end(); ++i) {
                if ((*i).length > bestLength) {
                    bestLength = (*i).length;
                }
            }
            // beyond a certain length, all lanes are suitable
            // however, we still need to check departPos to avoid unsuitable insertion
            // (this is only possible in some cases)
            double departPos = 0;
            if (bestLength > BEST_LANE_LOOKAHEAD) {
                departPos = getDepartPosBound(veh);
                bestLength = MIN2(bestLength - departPos, BEST_LANE_LOOKAHEAD);
            }
            std::vector<MSLane*>* bestLanes = new std::vector<MSLane*>();
            for (std::vector<MSVehicle::LaneQ>::const_iterator i = bl.begin(); i != bl.end(); ++i) {
                if (((*i).length - departPos) >= bestLength) {
                    bestLanes->push_back((*i).lane);
                }
            }
            MSLane* ret = getFreeLane(bestLanes, veh.getVehicleType().getVehicleClass(), getDepartPosBound(veh, false));
            delete bestLanes;
            return ret;
        }
        case DEPART_LANE_DEFAULT:
        case DEPART_LANE_FIRST_ALLOWED:
            for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
                if ((*i)->allowsVehicleClass(veh.getVehicleType().getVehicleClass())) {
                    return *i;
                }
            }
            return 0;
        default:
            break;
    }
    if (!(*myLanes)[0]->allowsVehicleClass(veh.getVehicleType().getVehicleClass())) {
        return 0;
    }
    return (*myLanes)[0];
}


bool
MSEdge::insertVehicle(SUMOVehicle& v, SUMOTime time, const bool checkOnly, const bool forceCheck) const {
    // when vaporizing, no vehicles are inserted, but checking needs to be successful to trigger removal
    if (isVaporizing() || isTazConnector()) {
        return checkOnly;
    }
    const SUMOVehicleParameter& pars = v.getParameter();
    const MSVehicleType& type = v.getVehicleType();
    if (pars.departSpeedProcedure == DEPART_SPEED_GIVEN && pars.departSpeed > getVehicleMaxSpeed(&v)) {
        const std::vector<double>& speedFactorParams = type.getSpeedFactor().getParameter();
        if (speedFactorParams[1] > 0.) {
            v.setChosenSpeedFactor(type.computeChosenSpeedDeviation(0, pars.departSpeed / getSpeedLimit()));
            if (v.getChosenSpeedFactor() > speedFactorParams[0] + 2 * speedFactorParams[1]) {
                // only warn for significant deviation
                WRITE_WARNING("Choosing new speed factor " + toString(v.getChosenSpeedFactor()) + " for vehicle '" + pars.id + "' to match departure speed.");
            }
        } else {
            throw ProcessError("Departure speed for vehicle '" + pars.id +
                               "' is too high for the departure edge '" + getID() + "'.");
        }
    }
    if (MSGlobals::gUseMesoSim) {
        if (!forceCheck && myLastFailedInsertionTime == time) {
            return false;
        }
        double pos = 0.0;
        switch (pars.departPosProcedure) {
            case DEPART_POS_GIVEN:
                if (pars.departPos >= 0.) {
                    pos = pars.departPos;
                } else {
                    pos = pars.departPos + getLength();
                }
                if (pos < 0 || pos > getLength()) {
                    WRITE_WARNING("Invalid departPos " + toString(pos) + " given for vehicle '" +
                                  v.getID() + "'. Inserting at lane end instead.");
                    pos = getLength();
                }
                break;
            case DEPART_POS_RANDOM:
            case DEPART_POS_RANDOM_FREE:
                pos = RandHelper::rand(getLength());
                break;
            default:
                break;
        }
        bool result = false;
        MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this, pos);
        MEVehicle* veh = static_cast<MEVehicle*>(&v);
        if (pars.departPosProcedure == DEPART_POS_FREE) {
            while (segment != 0 && !result) {
                if (checkOnly) {
                    result = segment->hasSpaceFor(veh, time, true);
                } else {
                    result = segment->initialise(veh, time);
                }
                segment = segment->getNextSegment();
            }
        } else {
            if (checkOnly) {
                result = segment->hasSpaceFor(veh, time, true);
            } else {
                result = segment->initialise(veh, time);
            }
        }
        return result;
    }
    if (checkOnly) {
        switch (v.getParameter().departLaneProcedure) {
            case DEPART_LANE_GIVEN:
            case DEPART_LANE_DEFAULT:
            case DEPART_LANE_FIRST_ALLOWED: {
                MSLane* insertionLane = getDepartLane(static_cast<MSVehicle&>(v));
                if (insertionLane == 0) {
                    WRITE_WARNING("could not insert vehicle '" + v.getID() + "' on any lane of edge '" + getID() + "', time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()));
                    return false;
                }
                const double occupancy = insertionLane->getBruttoOccupancy();
                return occupancy == 0 || occupancy * myLength + v.getVehicleType().getLengthWithGap() <= myLength;
            }
            default:
                for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
                    const double occupancy = (*i)->getBruttoOccupancy();
                    if (occupancy == 0 || occupancy * myLength + v.getVehicleType().getLengthWithGap() <= myLength) {
                        return true;
                    }
                }
        }
        return false;
    }
    MSLane* insertionLane = getDepartLane(static_cast<MSVehicle&>(v));
    if (insertionLane == 0) {
        return false;
    }

    if (!forceCheck) {
        if (myLastFailedInsertionTime == time) {
            if (myFailedInsertionMemory.count(insertionLane->getIndex())) {
                // A vehicle was already rejected for the proposed insertionLane in this timestep
                return false;
            }
        } else {
            // last rejection occured in a previous timestep, clear cache
            myFailedInsertionMemory.clear();
        }
    }

    bool success = insertionLane->insertVehicle(static_cast<MSVehicle&>(v));

    if (!success) {
        myFailedInsertionMemory.insert(insertionLane->getIndex());
    }
    return success;
}


void
MSEdge::changeLanes(SUMOTime t) {
    if (myLaneChanger == 0) {
        return;
    }
    myLaneChanger->laneChange(t);
}



const MSEdge*
MSEdge::getInternalFollowingEdge(const MSEdge* followerAfterInternal) const {
    //@todo to be optimized
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        MSLane* l = *i;
        const MSLinkCont& lc = l->getLinkCont();
        for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); ++j) {
            MSLink* link = *j;
            if (&link->getLane()->getEdge() == followerAfterInternal) {
                if (link->getViaLane() != 0) {
                    return &link->getViaLane()->getEdge();
                } else {
                    return 0; // network without internal links
                }
            }
        }
    }
    return 0;
}

double
MSEdge::getInternalFollowingLengthTo(const MSEdge* followerAfterInternal) const {
    assert(followerAfterInternal != 0);
    assert(!followerAfterInternal->isInternal());
    double dist = 0.;
    const MSEdge* edge = getInternalFollowingEdge(followerAfterInternal);
    // Take into account non-internal lengths until next non-internal edge
    while (edge != 0 && edge->isInternal()) {
        dist += edge->getLength();
        edge = edge->getInternalFollowingEdge(followerAfterInternal);
    }
    return dist;
}

const MSEdge*
MSEdge::getNormalBefore() const {
    const MSEdge* result = this;
    while (result->isInternal()) {
        assert(myPredecessors.size() == 1);
        result = myPredecessors.front();
    }
    return result;
}

double
MSEdge::getMeanSpeed() const {
    double v = 0;
    double no = 0;
    if (MSGlobals::gUseMesoSim) {
        for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
            const double vehNo = (double) segment->getCarNumber();
            v += vehNo * segment->getMeanSpeed();
            no += vehNo;
        }
        if (no == 0) {
            return getLength() / myEmptyTraveltime; // may include tls-penalty
        }
    } else {
        for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
            const double vehNo = (double)(*i)->getVehicleNumber();
            v += vehNo * (*i)->getMeanSpeed();
            no += vehNo;
        }
        if (no == 0) {
            return getSpeedLimit();
        }
    }
    return v / no;
}


double
MSEdge::getCurrentTravelTime(double minSpeed) const {
    assert(minSpeed > 0);
    if (!myAmDelayed) {
        return myEmptyTraveltime;
    }
    return getLength() / MAX2(minSpeed, getMeanSpeed());
}


double
MSEdge::getRoutingSpeed() const {
    return MSDevice_Routing::getAssumedSpeed(this);
}



bool
MSEdge::dictionary(const std::string& id, MSEdge* ptr) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict[id] = ptr;
        while ((int)myEdges.size() < ptr->getNumericalID() + 1) {
            myEdges.push_back(0);
        }
        myEdges[ptr->getNumericalID()] = ptr;
        return true;
    }
    return false;
}


MSEdge*
MSEdge::dictionary(const std::string& id) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


int
MSEdge::dictSize() {
    return (int)myDict.size();
}


const MSEdgeVector&
MSEdge::getAllEdges() {
    return myEdges;
}


void
MSEdge::clear() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
    myEdges.clear();
}


void
MSEdge::insertIDs(std::vector<std::string>& into) {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


void
MSEdge::parseEdgesList(const std::string& desc, ConstMSEdgeVector& into,
                       const std::string& rid) {
    if (desc[0] == BinaryFormatter::BF_ROUTE) {
        std::istringstream in(desc, std::ios::binary);
        char c;
        in >> c;
        FileHelpers::readEdgeVector(in, into, rid);
    } else {
        StringTokenizer st(desc);
        parseEdgesList(st.getVector(), into, rid);
    }
}


void
MSEdge::parseEdgesList(const std::vector<std::string>& desc, ConstMSEdgeVector& into,
                       const std::string& rid) {
    for (std::vector<std::string>::const_iterator i = desc.begin(); i != desc.end(); ++i) {
        const MSEdge* edge = MSEdge::dictionary(*i);
        // check whether the edge exists
        if (edge == 0) {
            throw ProcessError("The edge '" + *i + "' within the route " + rid + " is not known."
                               + "\n The route can not be build.");
        }
        into.push_back(edge);
    }
}


double
MSEdge::getDistanceTo(const MSEdge* other) const {
    if (isTazConnector()) {
        if (other->isTazConnector()) {
            return myTazBoundary.distanceTo2D(other->myTazBoundary);
        }
        return myTazBoundary.distanceTo2D(other->getFromJunction()->getPosition());
    }
    if (other->isTazConnector()) {
        return other->myTazBoundary.distanceTo2D(getToJunction()->getPosition());
    }
    return getToJunction()->getPosition().distanceTo2D(other->getFromJunction()->getPosition());
}


double
MSEdge::getSpeedLimit() const {
    // @note lanes might have different maximum speeds in theory
    return myLanes->empty() ? 1 : getLanes()[0]->getSpeedLimit();
}


double
MSEdge::getLengthGeometryFactor() const {
    return myLanes->empty() ? 1 : getLanes()[0]->getLengthGeometryFactor();
}

double
MSEdge::getVehicleMaxSpeed(const SUMOVehicle* const veh) const {
    // @note lanes might have different maximum speeds in theory
    return myLanes->empty() ? 1 : getLanes()[0]->getVehicleMaxSpeed(veh);
}


void
MSEdge::setMaxSpeed(double val) const {
    if (myLanes != 0) {
        for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
            (*i)->setMaxSpeed(val);
        }
    }
}



std::vector<MSTransportable*>
MSEdge::getSortedPersons(SUMOTime timestep, bool includeRiding) const {
    std::vector<MSTransportable*> result(myPersons.begin(), myPersons.end());
    if (includeRiding) {
        for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
            const MSLane::VehCont& vehs = (*i)->getVehiclesSecure();
            for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
                const std::vector<MSTransportable*>& persons = (*j)->getPersons();
                result.insert(result.end(), persons.begin(), persons.end());
            }
            (*i)->releaseVehicles();
        }
    }
    sort(result.begin(), result.end(), transportable_by_position_sorter(timestep));
    return result;
}


std::vector<MSTransportable*>
MSEdge::getSortedContainers(SUMOTime timestep, bool /* includeRiding */) const {
    std::vector<MSTransportable*> result(myContainers.begin(), myContainers.end());
    sort(result.begin(), result.end(), transportable_by_position_sorter(timestep));
    return result;
}


int
MSEdge::transportable_by_position_sorter::operator()(const MSTransportable* const c1, const MSTransportable* const c2) const {
    const double pos1 = c1->getCurrentStage()->getEdgePos(myTime);
    const double pos2 = c2->getCurrentStage()->getEdgePos(myTime);
    if (pos1 != pos2) {
        return pos1 < pos2;
    }
    return c1->getID() < c2->getID();
}


void
MSEdge::addSuccessor(MSEdge* edge) {
    mySuccessors.push_back(edge);
    if (isTazConnector() && edge->getFromJunction() != 0) {
        myTazBoundary.add(edge->getFromJunction()->getPosition());
    }

    edge->myPredecessors.push_back(this);
    if (edge->isTazConnector() && getToJunction() != 0) {
        edge->myTazBoundary.add(getToJunction()->getPosition());
    }
}


const MSEdgeVector&
MSEdge::getSuccessors(SUMOVehicleClass vClass) const {
    if (vClass == SVC_IGNORING || !MSNet::getInstance()->hasPermissions() || myFunction == EDGEFUNC_CONNECTOR) {
        return mySuccessors;
    }
#ifdef HAVE_FOX
    if (MSDevice_Routing::isParallel()) {
        MSDevice_Routing::lock();
    }
#endif
    std::map<SUMOVehicleClass, MSEdgeVector>::iterator i = myClassesSuccessorMap.find(vClass);
    if (i == myClassesSuccessorMap.end()) {
        // instantiate vector
        myClassesSuccessorMap[vClass];
        i = myClassesSuccessorMap.find(vClass);
        // this vClass is requested for the first time. rebuild all successors
        for (MSEdgeVector::const_iterator it = mySuccessors.begin(); it != mySuccessors.end(); ++it) {
            if ((*it)->isTazConnector()) {
                i->second.push_back(*it);
            } else {
                const std::vector<MSLane*>* allowed = allowedLanes(*it, vClass);
                if (allowed != 0 && allowed->size() > 0) {
                    i->second.push_back(*it);
                }
            }
        }
    }
    // can use cached value
#ifdef HAVE_FOX
    if (MSDevice_Routing::isParallel()) {
        MSDevice_Routing::unlock();
    }
#endif
    return i->second;
}


bool
MSEdge::canChangeToOpposite() {
    return (!myLanes->empty() && myLanes->back()->getOpposite() != 0 &&
            // do not change on curved internal lanes
            (!isInternal() || myLanes->back()->getIncomingLanes()[0].viaLink->getDirection() == LINKDIR_STRAIGHT));
}


bool
MSEdge::hasMinorLink() const {
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        const MSLinkCont& lc = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); ++j) {
            if (!(*j)->havePriority()) {
                return true;
            }
        }
    }
    return false;
}


void MSEdge::checkAndRegisterBiDirEdge() {
    myOppositingSuperposableEdge = 0;
    if (getFunction() != EDGEFUNC_NORMAL) {
        return;
    }
    ConstMSEdgeVector candidates = myToJunction->getOutgoing();
    for (ConstMSEdgeVector::const_iterator it = candidates.begin(); it != candidates.end(); it++) {
        if ((*it)->getToJunction() == myFromJunction) { //reverse edge
            if (myOppositingSuperposableEdge != 0 && isSuperposable(*it)) {
                WRITE_WARNING("Ambiguous superposable edges between junction '" + myToJunction->getID() + "' and '" + myFromJunction->getID() + "'.");
                break;
            }
            myOppositingSuperposableEdge = isSuperposable(*it) ? *it : 0;
        }
    }
}


bool MSEdge::isSuperposable(const MSEdge* other) {
    if (other == 0 || other->getLanes().size() != myLanes->size()) {
        return false;
    }
    std::vector<MSLane*>::const_iterator it1 = myLanes->begin();
    std::vector<MSLane*>::const_reverse_iterator it2 = other->getLanes().rbegin();
    do {
        if ((*it1)->getShape().reverse() != (*it2)->getShape()) {
            return false;
        }
        it1++;
        it2++;
    } while (it1 != myLanes->end());

    return true;
}

/****************************************************************************/

