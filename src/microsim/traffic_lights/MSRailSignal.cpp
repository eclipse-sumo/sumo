/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSRoutingEngine.h>
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

//#define DEBUG_FORWARD_BLOCK
//#define DEBUG_BIDI_BLOCK
//#define DEBUG_BACKWARD_BLOCK

//#define DEBUG_SIGNALSTATE
#define DEBUG_SIGNALSTATE_PRIORITY
#define DEBUG_COND (isSelected())
//#define DEBUG_COND (getID() == "disabled")
//#define DEBUG_COND (true)

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

    myConflictLanes.resize(myLinks.size());
    myConflictLinks.resize(myLinks.size());
    myRouteConflictLanes.resize(myLinks.size());
    myRouteConflictLinks.resize(myLinks.size());

    myLastRerouteAttempt.resize(myLinks.size(), std::make_pair(nullptr, -1));

    if (OptionsCont::getOptions().isSet("railsignal-block-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("railsignal-block-output");
        od.openTag("railSignal");
        od.writeAttr(SUMO_ATTR_ID, getID());
    }

    for (LinkVector& links : myLinks) { //for every link index
        // collect lanes and links that are relevant for setting this signal
        // for each index we collect
        // - conflictLanes (signal must be red if any conflict lane is occupied)
        // - conflictLinks (signal must be red if any conflict link is approached by a vehicle
        //    - that cannot break in time (arrivalSpeedBraking > 0)
        //    - approached by a vehicle with higher switching priority (see #3941)
        //
        // forwardBlock
        // - search forward recursive from outgoing lane until controlled railSignal link found
        //   -> add all found lanes
        //
        // bidiBlock (if any forwardBlock edge edge has bidi edge)
        // - search bidi backward recursive until first switch
        //   - from switch search backward recursive all other incoming until controlled rail signal link
        //     -> add final links
        //
        // backwardBlock
        // - search backward recursive from incoming lanes (not bidi for current outgoing lane) 
        //   until controlled railSignal link found
        //   -> add all found lanes
        //   -> add final links
        //
        // conditionalBlocks
        // - for each conflict link (always signalized) that enters from a
        // bidirectional track
        //   - search bidi backward recursive until first switch that is
        //   accessible from the bidi-direction
        //     - from switch search bidi backward recursive until controlled rail signal link
        //       -> add all found lanes
        //       -> add final links

        std::vector<MSLane*> conflictLanes;
        std::vector<MSLink*> conflictLinks;
        LaneSet visited;

        std::vector<MSLane*> forwardBlock;
        std::vector<MSLane*> bidiBlock;
        std::vector<MSLane*> backwardBlock;

        // there should be only one link per index for railSignal
        if (links.size() > 1) {
            throw ProcessError("At railSignal '" + getID() + "' found " + toString(links.size()) 
                    + " links controlled by index " + toString(links[0]->getTLIndex()));
        }
        for (MSLink* link : links) {
            MSLane* toLane = link->getViaLaneOrLane();   //the lane this link is leading to
            MSLane* fromBidi = link->getLaneBefore()->getBidiLane();
            if (fromBidi != nullptr) {
                // do not extend to forward block beyond the entering track (in case of a loop)
                visited.insert(fromBidi);
            }

            collectForwardBlock(toLane, 0., forwardBlock, visited);
#ifdef DEBUG_FORWARD_BLOCK
            if (DEBUG_COND)  std::cout << "railSignal=" << getID() << " index=" << link->getTLIndex() << " forwardBlock=" << toString(forwardBlock) << "\n";
#endif

            for (MSLane* forward : forwardBlock) {
                if (forward->getEdge().getBidiEdge() != nullptr) {
                    MSLane* bidi = forward->getBidiLane();
                    for (MSLink* bidiOutLink : bidi->getLinkCont()) {
                        if (bidiOutLink->getViaLane() != nullptr) {
                            bidiBlock.push_back(bidiOutLink->getViaLane());
                        }
                    }
                    collectBidiBlock(bidi, 0., false, bidiBlock, visited);
                    // assume bidirectional patches are continuous
                    break;
                }
            }
#ifdef DEBUG_BIDI_BLOCK
            if (DEBUG_COND)  std::cout << "railSignal=" << getID() << " index=" << link->getTLIndex() << " bidiBlock=" << toString(bidiBlock) << "\n";
#endif

            conflictLanes.insert(conflictLanes.end(), forwardBlock.begin(), forwardBlock.end());
            conflictLanes.insert(conflictLanes.end(), bidiBlock.begin(), bidiBlock.end());

            // compute conflict links
            for (MSLane* cl : conflictLanes) {
                collectConflictLinks(cl, 0, backwardBlock, conflictLinks, visited);
            }
            auto thisLinkIt = std::find(conflictLinks.begin(), conflictLinks.end(), link);
            if (thisLinkIt != conflictLinks.end()) {
                conflictLinks.erase(thisLinkIt);
            } else {
                WRITE_WARNING("At railSignal junction '" + getID() + "' link " + toString(link->getTLIndex()) + " with direction " + toString(link->getDirection()) + " should be uncontrolled");
            }

            conflictLanes.insert(conflictLanes.end(), backwardBlock.begin(), backwardBlock.end());
#ifdef DEBUG_BACKWARD_BLOCK
            if (DEBUG_COND) {
                std::cout << "railSignal=" << getID() << " index=" << link->getTLIndex() << " backwardBlock=" << toString(backwardBlock);
                std::cout << "railSignal=" << getID() << " index=" << link->getTLIndex() << " conflictLinks=";
                for (MSLink* cl : conflictLinks) {
                    std::cout << toString(cl->getViaLaneOrLane()->getID()) << " ";
                }
                std::cout << "\n";
            }
#endif

            // compute conditional conflict lanes and links
            for (MSLink* cl : conflictLinks) {
                std::vector<MSLane*> routeConflictLanes;
                std::vector<MSLink*> routeConflictLinks;
                MSLane* in = const_cast<MSLane*>(cl->getLaneBefore());
                LaneSet rCVisited = visited;
                // only collect if 
                // 1) the in-edge is bidirectional
                // 2) the foe has no alternative track before reach meeting the end of the forwardBlock
                // 3) the forward block has no alternative track between the end of the forward block and the conflict link
                if (in->getEdge().getBidiEdge() != nullptr 
                        && !hasAlternativeTrack(cl) 
                        && !hasAlternativeTrackBetween(forwardBlock, cl)) {
                    collectBidiBlock(in, 0., false, routeConflictLanes, rCVisited);
                    std::vector<MSLane*> rCBackwardBlock;
                    for (MSLane* rCLane : routeConflictLanes) {
                        collectConflictLinks(rCLane, 0, rCBackwardBlock, routeConflictLinks, rCVisited);
                    }
                }
                myRouteConflictLanes[link->getTLIndex()].push_back(routeConflictLanes);
                myRouteConflictLinks[link->getTLIndex()].push_back(routeConflictLinks);
            }
            

            if (OptionsCont::getOptions().isSet("railsignal-block-output")) {
                OutputDevice& od = OutputDevice::getDeviceByOption("railsignal-block-output");
                od.openTag("link");
                od.writeAttr(SUMO_ATTR_TLLINKINDEX, link->getTLIndex());
                od.writeAttr(SUMO_ATTR_FROM, link->getLaneBefore()->getID());
                od.writeAttr(SUMO_ATTR_TO, link->getViaLaneOrLane()->getID());

                od.openTag("forwardBlock");
                od.writeAttr(SUMO_ATTR_LANES, toString(forwardBlock));
                od.closeTag();
                od.openTag("bidiBlock");
                od.writeAttr(SUMO_ATTR_LANES, toString(bidiBlock));
                od.closeTag();
                od.openTag("backwardBlock");
                od.writeAttr(SUMO_ATTR_LANES, toString(backwardBlock));
                od.closeTag();
                od.openTag("conflictLinks");
                std::vector<std::string> conflictLinkIDs; // railSignalID_tlIndex
                for (MSLink* cl : conflictLinks) {
                    conflictLinkIDs.push_back(getTLLinkID(cl));
                }
                od.writeAttr("logicIndex", toString(conflictLinkIDs));
                for (int i = 0; i < (int)conflictLinks.size(); i++) {
                    const std::vector<MSLane*>& rCLanes = myRouteConflictLanes[link->getTLIndex()][i];
                    const std::vector<MSLink*>& rCLinks = myRouteConflictLinks[link->getTLIndex()][i];
                    if (rCLanes.size() > 0 || rCLinks.size() > 0) {
                        od.openTag("conflictLink");
                        od.writeAttr("logicIndex", getTLLinkID(conflictLinks[i]));
                        if (rCLanes.size() > 0) {
                            od.writeAttr("lanes", toString(rCLanes));
                        }
                        if (rCLinks.size() > 0) {
                            std::vector<std::string> rCLinkIDs; 
                            for (MSLink* rcl : rCLinks) {
                                rCLinkIDs.push_back(getTLLinkID(rcl));
                            }
                            od.writeAttr("links", toString(rCLinkIDs));
                        }
                        od.closeTag();
                    }
                }
                od.closeTag(); // conflictLinks
                od.closeTag(); // link
            }

            myConflictLanes[link->getTLIndex()] = conflictLanes;
            myConflictLinks[link->getTLIndex()] = conflictLinks;
        }
    }
    if (OptionsCont::getOptions().isSet("railsignal-block-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("railsignal-block-output");
        od.closeTag();
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


bool
MSRailSignal::conflictLaneOccupied(int index) const {
    for (const MSLane* lane : myConflictLanes[index]) {
        if (!lane->isEmpty()) {
#ifdef DEBUG_SIGNALSTATE
            if (DEBUG_COND) std::cout << SIMTIME << " railSignal=" << getID() << " index=" << index << " conflictLane " << lane->getID() << " occupied\n";
#endif
            return true;
        }
    }
    return false;
}


bool 
MSRailSignal::hasLinkConflict(int index) const {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
    //if (DEBUG_COND) std::cout << SIMTIME << " railSignal=" << getID() << " index=" << index << " hasLinkConflict...\n";
#endif
    MSLink* currentLink = myLinks[index][0];
    double foeMaxSpeed = -1;
    double foeMinDist = std::numeric_limits<double>::max();
    SUMOTime foeMinETA = std::numeric_limits<SUMOTime>::max();
    long long foeMinNumericalID = std::numeric_limits<long long>::max(); // tie braker

    // check for vehicles that enter the (unconditional) conflict area and
    // resolve conflict according to priority
    std::vector<int> checkRouteConflict;
    const auto& cLinks = myConflictLinks[index];
    for (int clIndex = 0; clIndex < (int)cLinks.size(); ++clIndex) {
        if (myRouteConflictLanes[index][clIndex].size() > 0) {
            // record links where the conditional conflict area may be occupied
            checkRouteConflict.push_back(clIndex);
        }
        const MSLink* link = cLinks[clIndex];
        if (link->getApproaching().size() > 0) {
            const MSTrafficLightLogic* foeTLL = link->getTLLogic();
            assert(foeTLL != nullptr);
            const MSRailSignal* foeRS = dynamic_cast<const MSRailSignal*>(foeTLL);
            if (foeRS != nullptr) {
                if (foeRS->conflictLaneOccupied(link->getTLIndex())) {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
                if (DEBUG_COND) std::cout << SIMTIME << " railSignal=" << getID() << " index=" << index 
                    << " foeLink " << link->getViaLaneOrLane()->getID() << " (ignored)\n";
#endif
                    continue;
                }
            } else if (link->getState() == LINKSTATE_TL_RED) {
                // ignore foe vehicles waiting at a regular traffic light
               continue;
            }
        }
        for (auto apprIt : link->getApproaching()) {
            MSLink::ApproachingVehicleInformation info = apprIt.second;
            foeMaxSpeed = MAX2(apprIt.first->getSpeed(), foeMaxSpeed);
            foeMinDist = MIN2(info.dist, foeMinDist);
            if (info.willPass) {
                foeMinETA = MIN2(info.arrivalTime, foeMinETA);
            }
            foeMinNumericalID = MIN2(foeMinNumericalID, apprIt.first->getNumericalID());
        }
    }
    SUMOVehicle* closest = nullptr;
    if (foeMaxSpeed >= 0 || checkRouteConflict.size() > 0) {
        // check against vehicles approaching this link
        double maxSpeed = -1;
        double minDist = std::numeric_limits<double>::max();
        SUMOTime minETA = std::numeric_limits<SUMOTime>::max();
        long long minNumericalID = std::numeric_limits<long long>::max(); // tie braker
        for (auto apprIt : currentLink->getApproaching()) {
            MSLink::ApproachingVehicleInformation info = apprIt.second;
            maxSpeed = MAX2(apprIt.first->getSpeed(), maxSpeed);
            if (info.dist < minDist) {
                minDist = info.dist;
                closest = const_cast<SUMOVehicle*>(apprIt.first);
            }
            if (info.willPass) {
                minETA = MIN2(info.arrivalTime, minETA);
            }
            minNumericalID = MIN2(minNumericalID, apprIt.first->getNumericalID());
        }
#ifdef DEBUG_SIGNALSTATE_PRIORITY
        if (DEBUG_COND) std::cout << SIMTIME << " railSignal=" << getID() << " index=" << index 
            << " fms=" << foeMaxSpeed << " ms=" << maxSpeed
            << " fmd=" << foeMinDist << " md=" << minDist
            << " fmE=" << foeMinETA << " mE=" << minETA
            << " fmI=" << foeMinNumericalID << " mI=" << minNumericalID
            << "\n";
#endif
        if (foeMinETA < minETA) {
            return true;
        } else if (foeMinETA == minETA) {
            if (foeMaxSpeed > maxSpeed) {
                return true;
            } else if (foeMaxSpeed == maxSpeed) {
                if (foeMinDist < minDist) {
                    return true;
                } else if (foeMinDist == minDist) {
                    if (foeMinNumericalID < minNumericalID) {
                        return true;
                    }
                }
            }
        }
#ifdef DEBUG_SIGNALSTATE_PRIORITY
        //if (DEBUG_COND) std::cout << SIMTIME << " railSignal=" << getID() << " index=" << index << " closestVeh=" << Named::getIDSecure(closest) << " routeFoes " << toString(routeFoes) << "\n";
#endif
        if (closest != nullptr) {
            for (int clIndex : checkRouteConflict) {
                const MSEdge& firstBidi = myRouteConflictLanes[index][clIndex].front()->getEdge();
                const MSEdge* first = firstBidi.getBidiEdge();
                assert(first != nullptr);
                if (std::find(closest->getCurrentRouteEdge(), closest->getRoute().end(), first) != closest->getRoute().end()) {
                    // vehicle wants to drive passt this conflict link
                    //const ConstMSEdgeVector& route = closest->getRoute().getEdges();
                    for (const MSLane* cLane : myRouteConflictLanes[index][clIndex]) {
                        if (cLane->getVehicleNumberWithPartials() > 0) {
                            if (DEBUG_COND) std::cout << SIMTIME << " railSignal=" << getID() << " index=" << index 
                                << " closestVeh=" << closest->getID() << " route edge " << cLane->getEdge().getID() << " blocked\n";
                            // trigger rerouting
                            MSDevice_Routing* rDev = static_cast<MSDevice_Routing*>(closest->getDevice(typeid(MSDevice_Routing)));
                            if (rDev != nullptr) {
                                SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
                                if (myLastRerouteAttempt[index].first != closest 
                                        // reroute each vehicle only once if no
                                        // periodic routing is allowed, otherwise
                                        // with the specified period
                                        || (rDev->getPeriod() > 0 &&  myLastRerouteAttempt[index].second + rDev->getPeriod() <= now)) {
                                    MSEdgeVector routeFoes;
                                    routeFoes.push_back(const_cast<MSEdge*>(first));
                                    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSRoutingEngine::getRouterTT(routeFoes);
                                    myLastRerouteAttempt[index] = std::make_pair(closest, now);
                                    try {
                                        closest->reroute(now, "railSignal:" + getID(), router, false, false, true); // silent
                                    } catch (ProcessError& error) {
#ifdef DEBUG_SIGNALSTATE_PRIORITY
                                        if (DEBUG_COND) std::cout << " rerouting failed: " << error.what() << "\n";
#endif
                                    } 
                                }
                            }
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


void
MSRailSignal::updateCurrentPhase() {
    std::string state(myLinks.size(), 'G');   //the state of the phase definition (all signal are green)
    for (int i = 0; i < (int)myLinks.size(); i++) {
        if (conflictLaneOccupied(i) || hasLinkConflict(i)) {
            state[i] = 'r';
        }
    }
    myCurrentPhase.setState(state);
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


void 
MSRailSignal::collectForwardBlock(MSLane* toLane, double length, std::vector<MSLane*>& forwardBlock, LaneSet& visited) 
{
    while (toLane != nullptr) {
        if (visited.count(toLane) != 0) {
            return;
        }
        visited.insert(toLane);
        forwardBlock.push_back(toLane);
        length += toLane->getLength();
        if (length > MAX_BLOCK_LENGTH) {
            if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
                WRITE_WARNING("Block after rail signal junction '" + getID() +
                        "' exceeds maximum length (stopped searching after lane '" + toLane->getID() + "' (length=" + toString(length) + "m).");
            }
            myNumWarnings++;
            return;
        }
        const MSLinkCont& links = toLane->getLinkCont();
        toLane = nullptr;
        for (MSLink* link : links) {
            if (link->getDirection() == LINKDIR_TURN) {
                continue;
            }
            if (link->getTLLogic() != nullptr) {
                if (link->getTLLogic() == this) {
                    WRITE_WARNING("Found circular block at railSignal junction '" + getID() + "' (" + toString(forwardBlock.size()) + " lanes, length " + toString(length) + ")");
                }
                return;
            }
            if (toLane == nullptr) {
                toLane = link->getViaLaneOrLane();
            } else {
                collectForwardBlock(link->getViaLaneOrLane(), length, forwardBlock, visited);
            }
        }
    }
}


void 
MSRailSignal::collectBidiBlock(MSLane* toLane, double length, bool foundSwitch, std::vector<MSLane*>& bidiBlock, LaneSet& visited) 
{
    while (toLane != nullptr) {
        //std::cout << " toLane=" << toLane->getID() << " length=" << length << " foundSwitch=" << foundSwitch << "\n";
        if (visited.count(toLane) != 0) {
            return;
        }
        visited.insert(toLane);
        bidiBlock.push_back(toLane);
        length += toLane->getLength();
        if (length > MAX_BLOCK_LENGTH) {
            if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
                WRITE_WARNING("Bidirectional block after rail signal junction '" + getID() +
                        "' exceeds maximum length (stopped searching after lane '" + toLane->getID() + "' (length=" + toString(length) + "m).");
            }
            myNumWarnings++;
            return;
        }
        const auto& incomingLaneInfos = toLane->getIncomingLanes();
        MSLane* prev = toLane;
        toLane = nullptr;
        for (const auto& ili : incomingLaneInfos) {
            if (ili.viaLink->getDirection() == LINKDIR_TURN) {
                continue;
            }
            if (ili.viaLink->getTLLogic() != nullptr) {
                if (!foundSwitch && bidiBlock.size() > 1) {
                    // check wether this node is switch (first edge doesn't count)
                    for (MSLink* link : ili.lane->getLinkCont()) {
                        if (link->getDirection() == LINKDIR_TURN) {
                            continue;
                        }
                        if (link->getViaLaneOrLane() == prev) {
                            continue;
                        }
                        //std::cout << "   ili.lane=" << ili.lane->getID() 
                        //    << " prev=" << prev->getID()
                        //    << " linkDir=" << ili.viaLink->getDirection()
                        //    << " linkIndex=" << ili.viaLink->getTLIndex()
                        //    << "\n";
                        foundSwitch = true;
                        break;
                    }
                }
                if (foundSwitch) {
                    return;
                }
            }
            if (toLane == nullptr) {
                toLane = ili.lane;
            } else {
                foundSwitch = true;
                collectBidiBlock(ili.lane, length, true, bidiBlock, visited);
            }
        }
        if (toLane != nullptr && !foundSwitch && prev->getEdge().getBidiEdge() != nullptr) {
            // check switch in the other direction
            MSLane* prevBidi = prev->getBidiLane();
            for (MSLink* link : prevBidi->getLinkCont()) {
                if (link->getDirection() == LINKDIR_TURN) {
                    continue;
                }
                if (link->getViaLaneOrLane()->getEdge().getBidiEdge() != &toLane->getEdge()) {
                    //std::cout << "   toLane=" << toLane->getID() 
                    //    << " prev=" << prev->getID()
                    //    << " prevBidi=" << prevBidi->getID() 
                    //    << " linkLane=" << link->getViaLaneOrLane()->getEdge().getID() 
                    //    << " linkLaneBidi=" << Named::getIDSecure(link->getViaLaneOrLane()->getEdge().getBidiEdge())
                    //    << "\n";
                    foundSwitch = true;
                    break;
                }
            }
        }
    }
}


void
MSRailSignal::collectConflictLinks(MSLane* toLane, double length,
            std::vector<MSLane*>& backwardBlock,
            std::vector<MSLink*>& conflictLinks,
            LaneSet& visited)
{
    while (toLane != nullptr) {
        //std::cout << "collectConflictLinks " << getID() << " toLane=" << toLane->getID() << " length=" << length 
        //    << " backward=" << toString(backwardBlock) 
        //    << " conflictLinks=" << conflictLinks.size()
        //    << " visited=" << visited.size()
        //    << "\n";
        const auto& incomingLaneInfos = toLane->getIncomingLanes();
        MSLane* orig = toLane;
        toLane = nullptr;
        for (const auto& ili : incomingLaneInfos) {
            if (ili.viaLink->getDirection() == LINKDIR_TURN) {
                continue;
            }
            if (visited.count(ili.lane) != 0) {
                continue;
            }
            if (ili.viaLink->getTLLogic() != nullptr) {
                conflictLinks.push_back(ili.viaLink);
                continue;
            }
            backwardBlock.push_back(ili.lane);
            visited.insert(ili.lane);
            length += orig->getLength();
            if (length > MAX_BLOCK_LENGTH) {
                if (myNumWarnings < MAX_SIGNAL_WARNINGS) {
                    WRITE_WARNING("incoming conflict block after rail signal junction '" + getID() +
                            "' exceeds maximum length (stopped searching after lane '" + orig->getID() + "' (length=" + toString(length) + "m).");
                }
                myNumWarnings++;
                return;
            }
            if (toLane == nullptr) {
                toLane = ili.lane;
            } else {
                collectConflictLinks(ili.lane, length, backwardBlock, conflictLinks, visited);
            }
        }
    }
}


void
MSRailSignal::addLink(MSLink* link, MSLane* lane, int pos) {
    if (pos >= 0) {
        MSTrafficLightLogic::addLink(link, lane, pos);
    } // ignore uncontrolled link
}


bool 
MSRailSignal::hasAlternativeTrack(MSLink* link) {
    for (MSLink* cand : link->getLaneBefore()->getLinkCont()) {
        if (cand != link && cand->getTLLogic() != nullptr) {
            return true;
        }
    }
    return false;
}

bool 
MSRailSignal::hasAlternativeTrackBetween(const std::vector<MSLane*>& forwardBlock, MSLink* cLink) {
    // check if the forward block ends at a controlled switch
    // note: link::myJunction not yet initialized here
    const MSJunction* cLinkJunction = cLink->getLaneBefore()->getEdge().getToJunction(); 
    //std::cout << " forwardBlock=" << toString(forwardBlock) << " cLink_j=" << cLinkJunction->getID() << "\n";
    if (forwardBlock.back()->getEdge().getToJunction() == cLinkJunction) {
        int numLinks = 0;
        for (MSLink* cand : forwardBlock.back()->getLinkCont()) {
            if (cand->getTLLogic() != nullptr) {
                numLinks++;
                if (numLinks > 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::string 
MSRailSignal::getTLLinkID(MSLink* link) {
    return link->getTLLogic()->getID() + "_" + toString(link->getTLIndex());
}

/****************************************************************************/

