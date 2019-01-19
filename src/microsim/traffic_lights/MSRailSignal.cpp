/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSRailSignal.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Jan 2015
/// @version $Id$
///
// A rail signal logic
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <microsim/MSVehicle.h>
#include "MSTrafficLightLogic.h"
#include "MSRailSignal.h"
#include <microsim/MSLane.h>
#include "MSPhaseDefinition.h"
#include "MSTLLogicControl.h"

// typical block length in germany on main lines is 3-5km on branch lines up to 7km
// special branches that are used by one train exclusively could also be up to 20km in length
// minimum block size in germany is 37.5m (LZB)
// larger countries (USA, Russia) might see blocks beyond 20km)
#define MAX_BLOCK_LENGTH 20000
#define MAX_SIGNAL_WARNINGS 10

//#define DEBUG_SUCCEEDINGBLOCKS
//#define DEBUG_SIGNALSTATE
#define DEBUG_COND (getID() == "disabled")

// ===========================================================================
// static value definitions
// ===========================================================================
int MSRailSignal::myNumWarnings(0);

class ApproachingVehicleInformation;
// ===========================================================================
// method definitions
// ===========================================================================
MSRailSignal::MSRailSignal(MSTLLogicControl& tlcontrol,
                           const std::string& id, const std::string& programID,
                           const std::map<std::string, std::string>& parameters) :
    MSTrafficLightLogic(tlcontrol, id, programID, TLTYPE_RAIL_SIGNAL, DELTA_T, parameters),
    myCurrentPhase(DELTA_T, std::string(SUMO_MAX_CONNECTIONS, 'X'), -1) { // dummy phase
    myDefaultCycleTime = DELTA_T;
}

void
MSRailSignal::init(NLDetectorBuilder&) {
    assert(myLanes.size() > 0);
    LinkVectorVector::iterator i2;    //iterator of the link indices of this junction (most likely there is just one link per index)
    // find all outgoing lanes from the junction and its succeeding lanes leading to the next rail signal
    // and find for every link at the junction all lanes leading from a previous signal to this link
    for (i2 = myLinks.begin(); i2 != myLinks.end(); ++i2) { //for every link index
        const LinkVector& links = *i2;
        LinkVector::const_iterator i;   //iterator of the links that belong to the same link index
        for (i = links.begin(); i != links.end(); i++) { //for every link that belongs to the current index
            MSLink* link = (*i);
            MSLane* toLane = link->getLane();   //the lane this link is leading to
            myLinksToLane[toLane].push_back(link);
            myLinkIndices[link] = (int)std::distance(myLinks.begin(), i2); //assign the index of link to link

            //find all lanes leading from a previous signal to link (we presume that there exists only one path from a previous signal to link)
            std::vector<const MSLane*> afferentBlock; //the vector of lanes leading from a previous signal to link
            bool noRailSignal = true;   //true if the considered lane is not outgoing from a rail signal
            //get the approaching lane of the link
            const MSLane* approachingLane = link->getLaneBefore();   //the lane this link is coming from
            afferentBlock.push_back(approachingLane);
            const MSLane* currentLane = approachingLane;
            //look recursively for all lanes that lie before approachingLane and add them to afferentBlock until a rail signal is found
            double blockLength = approachingLane->getLength();
            while (noRailSignal) {
                std::vector<MSLane::IncomingLaneInfo> incomingLanes = currentLane->getIncomingLanes();
                // ignore incoming lanes for non-rail classes
                for (auto it = incomingLanes.begin(); it != incomingLanes.end();) {
                    if (((*it).lane->getPermissions() & SVC_RAIL_CLASSES) == 0) {
                        it = incomingLanes.erase(it);
                    } else {
                        it++;
                    }
                }
                MSLane* precedentLane;
                if (!incomingLanes.empty()) {
                    precedentLane = incomingLanes.front().lane;
                } else {
                    precedentLane = nullptr;
                }
                if (precedentLane == nullptr) { //if there is no preceeding lane
                    noRailSignal = false;
                } else if (blockLength >= MAX_BLOCK_LENGTH) { // avoid huge blocks
                    WRITE_WARNING("Block before rail signal junction '" + getID() +
                                  "' exceeds maximum length (stopped searching at lane '" + precedentLane->getID() + "' after " + toString(blockLength) + "m).");
                    noRailSignal = false;
                } else {
                    const MSJunction* junction = precedentLane->getEdge().getToJunction();
                    if ((junction != nullptr) && (junction->getType() == NODETYPE_RAIL_SIGNAL || junction->getType() == NODETYPE_TRAFFIC_LIGHT)) { //if this junction exists and if it has a rail signal
                        noRailSignal = false;
                    } else {
                        afferentBlock.push_back(precedentLane);
                        blockLength += precedentLane->getLength();
                        currentLane = precedentLane;
                    }
                }
            }
            myAfferentBlocks[link] = afferentBlock;

            //find all lanes leading from toLane to the next signal if it was not already done
            if (std::find(myOutgoingLanes.begin(), myOutgoingLanes.end(), toLane) == myOutgoingLanes.end()) { //if toLane was not already contained in myOutgoingLanes
                myOutgoingLanes.push_back(toLane);
                std::vector<const MSLane*> succeedingBlock;   //the vector of lanes leading to the next rail signal
                for (const auto& ili : toLane->getIncomingLanes()) {
                    if (ili.lane->isInternal()) {
                        succeedingBlock.push_back(ili.lane);
                    }
                }
                succeedingBlock.push_back(toLane);
                currentLane = toLane;
                bool noRailSignalLocal = true;   //true if the considered lane is not ending at a rail signal
                double blockLength = toLane->getLength();
                while (noRailSignalLocal) {
                    //check first if the current lane is ending at a rail signal
                    std::vector<MSLink*> outGoingLinks = currentLane->getLinkCont();
                    std::vector<MSLink*>::const_iterator j;
                    for (j = outGoingLinks.begin(); j != outGoingLinks.end(); j++) {
                        const MSJunction* junction = currentLane->getEdge().getToJunction();
                        if ((junction != nullptr) && (junction->getType() == NODETYPE_RAIL_SIGNAL || junction->getType() == NODETYPE_TRAFFIC_LIGHT)) { //if this junctions exists and if it has a rail signal
                            noRailSignalLocal = false;
                            break;
                        }
                    }
                    if (noRailSignalLocal) { //if currentLane is not ending at a railSignal
                        //get the next lane
                        std::vector<const MSLane*> outGoingLanes;
                        // ignore outgoing lanes for non-rail classes
                        for (MSLink* link : currentLane->getLinkCont()) {
                            if ((link->getLane()->getPermissions() & SVC_RAIL_CLASSES) != 0 && link->getDirection() != LINKDIR_TURN) {
                                outGoingLanes.push_back(link->getLane());
                            }
                        }
                        if (outGoingLanes.size() == 0) {    //if the current lane has no outgoing lanes (deadend)
                            noRailSignalLocal = false;
                        } else if (blockLength > MAX_BLOCK_LENGTH) {
                            WRITE_WARNING("Block after rail signal junction '" + getID() +
                                          "' exceeds maximum length (stopped searching at lane '" + currentLane->getID() + "' after " + toString(blockLength) + "m).");
                            noRailSignalLocal = false;
                        } else {
                            if (outGoingLanes.size() > 1) {
                                if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
                                    WRITE_WARNING("Rail lane '" + currentLane->getID() + "' has more than one outgoing lane but does not have a rail signal at its end");
                                } else if (myNumWarnings == MAX_SIGNAL_WARNINGS) {
                                    WRITE_WARNING("Suppressing further signal warnings ...");
                                }
                                myNumWarnings++;
                            }
                            const MSLane* nextLane = outGoingLanes.front();
                            succeedingBlock.push_back(nextLane);
                            blockLength += nextLane->getLength();
                            currentLane = nextLane;
                        }
                    }
                }
                mySucceedingBlocks[toLane] = succeedingBlock;
            }
        }
    }
#ifdef DEBUG_SUCCEEDINGBLOCKS
    if (DEBUG_COND) {
        std::cout << "railSignal=" << getID() << " mySucceedingBlocks:\n";
        for (auto item : mySucceedingBlocks) {
            std::cout << "   toLane=" << item.first->getID() << " succ=" << toString(item.second) << "\n";
        }
    }
#endif


    for (auto& item : mySucceedingBlocks) {
        MSLane* out = item.first; 
        std::vector<const MSLane*> forwardSuccessors = item.second;
        std::vector<const MSLane*>& succeedingBlock = item.second;
        for (const MSLane* lane : forwardSuccessors) {
            const MSEdge* reverseEdge = lane->getEdge().getBidiEdge();
            if (reverseEdge != nullptr) {
                const MSLane* revLane = reverseEdge->getLanes()[0];
                succeedingBlock.push_back(revLane);
                for (const auto& ili : revLane->getIncomingLanes()) {
                    succeedingBlock.push_back(ili.lane);
                }
                const MSJunction* to = lane->getEdge().getToJunction();
                if (to != out->getEdge().getFromJunction()) {
                    // no loops
                    if (to->getType() == NODETYPE_RAIL_SIGNAL || to->getType() == NODETYPE_TRAFFIC_LIGHT) {
                        // find all entry links that appproach revLane
                        for (MSLane* internal : to->getInternalLanes()) {
                            for (const auto& ili : internal->getIncomingLanes()) {
                                if (ili.viaLink->getLane() == revLane) {
                                    mySucceedingBlocksIncomingLinks[out].push_back(ili.viaLink);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#ifdef DEBUG_SUCCEEDINGBLOCKS
    if (DEBUG_COND) {
        std::cout << " mySucceedingBlocks with bidi:\n";
        for (auto item : mySucceedingBlocks) {
            std::cout << "   toLane=" << item.first->getID() << " succ=" << toString(item.second) << "\n";
        }
        std::cout << " blockIncomingLinks:\n";
        for (auto item : mySucceedingBlocksIncomingLinks) {
            std::cout << "   lane=" << item.first->getID() << " links=";
            for (const MSLink* link : item.second) {
                std::cout << link->getViaLaneOrLane()->getID() << " ";
            }
            std::cout << "\n";
        }
    }
#endif

    updateCurrentPhase();   //check if this is necessary or if will be done already at another stage
    setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
}


MSRailSignal::~MSRailSignal() {}


// ----------- Handling of controlled links
void
MSRailSignal::adaptLinkInformationFrom(const MSTrafficLightLogic& logic) {
    MSTrafficLightLogic::adaptLinkInformationFrom(logic);
    updateCurrentPhase();
}


// ------------ Switching and setting current rows
SUMOTime
MSRailSignal::trySwitch() {
    updateCurrentPhase();
    setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
    return DELTA_T;
}

std::string
MSRailSignal::getAppropriateState() {
#ifdef DEBUG_SIGNALSTATE
    if (DEBUG_COND) std::cout << SIMTIME << " getAppropriateState railSignal=" << getID() << "\n";
#endif
    std::string state(myLinks.size(), 'G');   //the state of the phase definition (all signal are green)
    for (MSLane* lane : myOutgoingLanes) {
        //check if the succeeding block is used by a train
        bool succeedingBlockOccupied = false;
        const std::vector<const MSLane*>& block = mySucceedingBlocks.at(lane);
        for (const MSLane* l : block) {
            if (!l->isEmpty()) { //if this lane is not empty
                succeedingBlockOccupied = true;
                break;
            }
        }
#ifdef DEBUG_SIGNALSTATE
        if (DEBUG_COND) {
            std::cout << " out=" << lane->getID() << "\n";
            for (const MSLane* l : mySucceedingBlocks.at(lane)) {
                std::cout << "   succ=" << l->getID() << " occ=" << !l->isEmpty() << "\n";
            }
            auto itLinks = mySucceedingBlocksIncomingLinks.find(lane);
            if (itLinks != mySucceedingBlocksIncomingLinks.end()) {
                for (const MSLink* l : itLinks->second) {
                    std::cout << "   incomingLink=" << l->getLaneBefore()->getID() << "->" << l->getLane()->getID() << "\n";
                    for (const auto& ap : l->getApproaching()) {
                        std::cout << "     ap=" << ap.first->getID() << " asb=" << ap.second.arrivalSpeedBraking << "\n";
                    }
                }
            }

        }
#endif
        if (!succeedingBlockOccupied) {
            // check whether approaching vehicles reserve the block
            const auto itLinks = mySucceedingBlocksIncomingLinks.find(lane);
            if (itLinks != mySucceedingBlocksIncomingLinks.end()) {
                for (const MSLink* l : itLinks->second) {
                    for (auto apprIt : l->getApproaching()) {
                        MSLink::ApproachingVehicleInformation info = apprIt.second;
                        if (info.arrivalSpeedBraking > 0) {
                            succeedingBlockOccupied = true;
                            break;
                        }
                    }
                }
            }
        }

        /*-if the succeeding block is occupied the signals for all links leading to lane will be set to red.
          -if the succeeding block is not occupied and all blocks leading to lane are not occupied all signal
          will keep green.
          -if the succeeding block is not occupied and there is only one block leading to lane its signal will
          keep green (no matter if this block is occupied or not).
          -if the succeeding block is not occupied and there is more than one block leading to lane and some
          of them are occupied the signals for all links leading to lane, except one whose corresponding block
          is occupied, will be set to red. the signal for the remaining block will keep green*/
        if (succeedingBlockOccupied) {      //if the succeeding block is used by a train
            std::vector<MSLink*>::const_iterator k;
            for (k = myLinksToLane[lane].begin(); k != myLinksToLane[lane].end(); k++) { //for every link leading to this lane
                state.replace(myLinkIndices[*k], 1, "r"); //set the signal of the link (*k) to red
            }
        } else {
            if (myLinksToLane[lane].size() > 1) {   //if there is more than one link leading to lane
                bool hasOccupiedBlock = false;
                std::vector<MSLink*>::const_iterator k;
                for (k = myLinksToLane[lane].begin(); k != myLinksToLane[lane].end(); k++) { //for every link leading to lane
                    std::vector<const MSLane*>::const_iterator l;
                    for (l = myAfferentBlocks[(*k)].begin(); l != myAfferentBlocks[(*k)].end(); l++) {    //for every lane of the block leading from a previous signal to the link (*k)
                        if (!(*l)->isEmpty()) { //if this lane is not empty
                            hasOccupiedBlock = true;
                            //set the signals for all links leading to lane, except for (*k), to red; the signal for (*k) will remain green
                            std::vector<MSLink*>::const_iterator m;
                            for (m = myLinksToLane[lane].begin(); m != myLinksToLane[lane].end(); m++) { //for every link leading to lane
                                if (*m != *k) { //if this link is not the one corresponding to occupiedBlock
                                    state.replace(myLinkIndices[*m], 1, "r");   //set the signal of this link to red
                                }
                            }
                            break;  // we don't have to check the other lanes of this block anymore
                        }
                    }
                    if (hasOccupiedBlock) { //we don't have to check the other blocks anymore
                        break;
                    }
                }
            }
        }
    }
    return state;
}


void
MSRailSignal::updateCurrentPhase() {
    myCurrentPhase.setState(getAppropriateState());
}


// ------------ Static Information Retrieval
int
MSRailSignal::getPhaseNumber() const {
    return 0;
}

const MSTrafficLightLogic::Phases&
MSRailSignal::getPhases() const {
    return myPhases;
}

const MSPhaseDefinition&
MSRailSignal::getPhase(int) const {
    return myCurrentPhase;
}

// ------------ Dynamic Information Retrieval
int
MSRailSignal::getCurrentPhaseIndex() const {
    return 0;
}

const MSPhaseDefinition&
MSRailSignal::getCurrentPhaseDef() const {
    return myCurrentPhase;
}

// ------------ Conversion between time and phase
SUMOTime
MSRailSignal::getPhaseIndexAtTime(SUMOTime) const {
    return 0;
}

SUMOTime
MSRailSignal::getOffsetFromIndex(int) const {
    return 0;
}

int
MSRailSignal::getIndexFromOffset(SUMOTime) const {
    return 0;
}


/****************************************************************************/

