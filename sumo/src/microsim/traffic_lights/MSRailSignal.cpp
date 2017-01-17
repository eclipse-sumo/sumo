/****************************************************************************/
/// @file    MSRailSignal.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Jan 2015
/// @version $Id$
///
// A rail signal logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include "MSTrafficLightLogic.h"
#include "MSRailSignal.h"
#include <microsim/MSLane.h>
#include "MSPhaseDefinition.h"
#include "MSTLLogicControl.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// method definitions
// ===========================================================================
MSRailSignal::MSRailSignal(MSTLLogicControl& tlcontrol,
                           const std::string& id, const std::string& subid,
                           const std::map<std::string, std::string>& parameters) :
    MSTrafficLightLogic(tlcontrol, id, subid, DELTA_T, parameters),
    myCurrentPhase(DELTA_T, std::string(SUMO_MAX_CONNECTIONS, 'X')) { // dummy phase
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
            while (noRailSignal) {
                std::vector<MSLane::IncomingLaneInfo> incomingLanes = currentLane->getIncomingLanes();
                MSLane* precedentLane;
                if (!incomingLanes.empty()) {
                    precedentLane = incomingLanes.front().lane;
                } else {
                    precedentLane = 0;
                }
                if (precedentLane == 0) { //if there is no preceeding lane
                    noRailSignal = false;
                } else {
                    const MSJunction* junction = precedentLane->getEdge().getToJunction();
                    if ((junction != 0) && (junction->getType() == NODETYPE_RAIL_SIGNAL)) { //if this junction exists and if it has a rail signal
                        noRailSignal = false;
                    } else {
                        afferentBlock.push_back(precedentLane);
                        currentLane = precedentLane;
                    }
                }
            }
            myAfferentBlocks[link] = afferentBlock;

            //find all lanes leading from toLane to the next signal if it was not already done
            if (std::find(myOutgoingLanes.begin(), myOutgoingLanes.end(), toLane) == myOutgoingLanes.end()) { //if toLane was not already contained in myOutgoingLanes
                myOutgoingLanes.push_back(toLane);
                std::vector<const MSLane*> succeedingBlock;   //the vector of lanes leading to the next rail signal
                succeedingBlock.push_back(toLane);
                currentLane = toLane;
                bool noRailSignal = true;   //true if the considered lane is not ending at a rail signal
                while (noRailSignal) {
                    //check first if the current lane is ending at a rail signal
                    std::vector<MSLink*> outGoingLinks = currentLane->getLinkCont();
                    std::vector<MSLink*>::const_iterator j;
                    for (j = outGoingLinks.begin(); j != outGoingLinks.end(); j++) {
                        const MSJunction* junction = currentLane->getEdge().getToJunction();
                        if ((junction != 0) && (junction->getType() == NODETYPE_RAIL_SIGNAL)) { //if this junctions exists and if it has a rail signal
                            noRailSignal = false;
                            break;
                        }
                    }
                    if (noRailSignal) { //if currentLane is not ending at a railSignal
                        //get the next lane
                        std::vector<const MSLane*> outGoingLanes = currentLane->getOutgoingLanes();
                        if (outGoingLanes.size() == 0) {    //if the current lane has no outgoing lanes (deadend)
                            noRailSignal = false;
                        } else {
                            if (outGoingLanes.size() > 1) {
                                WRITE_WARNING("Rail lane '" + currentLane->getID() + "' has more than one outgoing lane but does not have a rail signal at its end");
                            }
                            const MSLane* nextLane = outGoingLanes.front();
                            succeedingBlock.push_back(nextLane);
                            currentLane = nextLane;
                        }
                    }
                }
                mySucceedingBlocks[toLane] = succeedingBlock;
            }
        }
    }
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
    std::string state(myLinks.size(), 'G');   //the state of the phase definition (all signal are green)
    std::vector<MSLane*>::const_iterator i;    //the iterator of outgoing lanes of this junction
    for (i = myOutgoingLanes.begin(); i != myOutgoingLanes.end(); i++) {    //for every outgoing lane
        MSLane* lane = (*i);

        //check if the succeeding block is used by a train
        bool succeedingBlockOccupied = false;
        std::vector<const MSLane*>::const_iterator j;
        for (j = mySucceedingBlocks.at(lane).begin(); j != mySucceedingBlocks.at(lane).end(); j++) { //for every lane in the block between the current signal and the next signal
            if (!(*j)->isEmpty()) { //if this lane is not empty
                succeedingBlockOccupied = true;
                break;
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
    myCurrentPhase = MSPhaseDefinition(DELTA_T, getAppropriateState());
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

