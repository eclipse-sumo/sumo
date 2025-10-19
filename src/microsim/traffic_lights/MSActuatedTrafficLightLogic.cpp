/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSActuatedTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @author  Julia Ringel
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
///
// An actuated (adaptive) traffic light logic
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunctionLogic.h>
#include <netload/NLDetectorBuilder.h>
#include "MSActuatedTrafficLightLogic.h"

//#define DEBUG_DETECTORS
//#define DEBUG_PHASE_SELECTION
//#define DEBUG_PHASE_SELECTION_CUSTOM
#define DEBUG_COND (getID()=="C")

// ===========================================================================
// static members
// ===========================================================================
const std::vector<std::string> MSActuatedTrafficLightLogic::OPERATOR_PRECEDENCE({
    "**", "^", "*", "/", "+", "-", "%",
    "=", "==", "!=", "<", ">", "<=", ">=",
    "and", "&&", "or", "||",
});

// ===========================================================================
// parameter defaults definitions
// ===========================================================================
#define DEFAULT_MAX_GAP "3.0"
#define DEFAULT_PASSING_TIME "1.9"
#define DEFAULT_DETECTOR_GAP "2.0"
#define DEFAULT_INACTIVE_THRESHOLD "180"
#define DEFAULT_CURRENT_PRIORITY 10
#define DEFAULT_CROSSING_PRIORITY 100

#define DEFAULT_LENGTH_WITH_GAP 7.5
#define DEFAULT_BIKE_LENGTH_WITH_GAP (getDefaultVehicleLength(SVC_BICYCLE) + 0.5)
#define DEFAULT_STATIC_MINDUR TIME2STEPS(0) // loop position for non-stretchable phases

#define NO_DETECTOR "NO_DETECTOR"
#define DEFAULT_CONDITION "DEFAULT"

// ===========================================================================
// method definitions
// ===========================================================================
MSActuatedTrafficLightLogic::MSActuatedTrafficLightLogic(MSTLLogicControl& tlcontrol,
        const std::string& id, const std::string& programID,
        const SUMOTime offset,
        const Phases& phases,
        int step, SUMOTime delay,
        const Parameterised::Map& parameter,
        const std::string& basePath,
        const ConditionMap& conditions,
        const AssignmentMap& assignments,
        const FunctionMap& functions) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, offset, TrafficLightType::ACTUATED, phases, step, delay, parameter),
    myHasMultiTarget(false),
    myLastTrySwitchTime(0),
    myConditions(conditions),
    myAssignments(assignments),
    myFunctions(functions),
    myTraCISwitch(false),
    myDetectorPrefix(id + "_" + programID + "_") {
    myMaxGap = StringUtils::toDouble(getParameter("max-gap", DEFAULT_MAX_GAP));
    myJamThreshold = StringUtils::toDouble(getParameter("jam-threshold", OptionsCont::getOptions().getValueString("tls.actuated.jam-threshold")));
    myPassingTime = StringUtils::toDouble(getParameter("passing-time", DEFAULT_PASSING_TIME));
    myDetectorGap = StringUtils::toDouble(getParameter("detector-gap", DEFAULT_DETECTOR_GAP));
    myInactiveThreshold = string2time(getParameter("inactive-threshold", DEFAULT_INACTIVE_THRESHOLD));
    myShowDetectors = StringUtils::toBool(getParameter("show-detectors", toString(OptionsCont::getOptions().getBool("tls.actuated.show-detectors"))));
    myBuildAllDetectors = StringUtils::toBool(getParameter("build-all-detectors", "false"));
    myFile = FileHelpers::checkForRelativity(getParameter("file", "NUL"), basePath);
    myFreq = TIME2STEPS(StringUtils::toDouble(getParameter("freq", "300")));
    myVehicleTypes = getParameter("vTypes", "");

    if (hasParameter("hide-conditions")) {
        std::vector<std::string> hidden = StringTokenizer(getParameter("hide-conditions", "")).getVector();
        std::set<std::string> hiddenSet(hidden.begin(), hidden.end());
        for (auto item : myConditions) {
            if (hiddenSet.count(item.first) == 0) {
                myListedConditions.insert(item.first);
            }
        }
    } else {
        const bool showAll = getParameter("show-conditions", "") == "";
        std::vector<std::string> shown = StringTokenizer(getParameter("show-conditions", "")).getVector();
        std::set<std::string> shownSet(shown.begin(), shown.end());
        for (auto item : myConditions) {
            if (showAll || shownSet.count(item.first) != 0) {
                myListedConditions.insert(item.first);
            }
        }
    }
    if (hasParameter("extra-detectors")) {
        const std::string extraIDs = getParameter("extra-detectors", "");
        for (std::string customID : StringTokenizer(extraIDs).getVector()) {
            try {
                myExtraLoops.push_back(retrieveDetExpression<MSInductLoop, SUMO_TAG_INDUCTION_LOOP>(customID, extraIDs, true));
            } catch (ProcessError&) {
                myExtraE2.push_back(retrieveDetExpression<MSE2Collector, SUMO_TAG_LANE_AREA_DETECTOR>(customID, extraIDs, true));
            }
        }
    }
    myStack.push_back(std::map<std::string, double>());
}


MSActuatedTrafficLightLogic::~MSActuatedTrafficLightLogic() { }

void
MSActuatedTrafficLightLogic::init(NLDetectorBuilder& nb) {
    MSTrafficLightLogic::init(nb);
    initAttributeOverride();
    initSwitchingRules();
    for (int i = 0; i < (int)myPhases.size(); i++) {
        initTargets(i);
    }
    if (myLanes.size() == 0) {
        // must be an older network
        WRITE_WARNINGF(TL("Traffic light '%' does not control any links"), getID());
    }
    bool warn = true; // warn only once
    const int numLinks = (int)myLinks.size();

    // Detector position should be computed based on road speed. If the position
    // is quite far away and the minDur is short this may cause the following
    // problems:
    //
    // 1)  high flow failure:
    // In a standing queue, no vehicle touches the detector.
    // By the time the queue advances, the detector gap has been exceeded and the phase terminates prematurely
    //
    // 2) low flow failure
    // The standing queue is fully between stop line and detector and there are no further vehicles.
    // The minDur is too short to let all vehicles pass
    //
    // Problem 2) is not so critical because there is less potential for
    // jamming in a low-flow situation. In contrast, problem 1) should be
    // avoided as it has big jamming potential. We compute an upper bound for the
    // detector distance to avoid it


    std::set<int> multiNextTargets = getMultiNextTargets();
    // change values for setting the loops and lanestate-detectors, here
    //SUMOTime inductLoopInterval = 1; //
    // build the induct loops
    std::map<const MSLane*, MSInductLoop*> laneInductLoopMap;
    std::map<MSInductLoop*, int> inductLoopInfoMap; // retrieve junction entry lane in case loops are placed further upstream (and other properties)
    int detEdgeIndex = -1;
    int detLaneIndex = 0;
    const double detDefaultLength = StringUtils::toDouble(getParameter("detector-length",
                                    OptionsCont::getOptions().getValueString("tls.actuated.detector-length")));
    MSEdge* prevDetEdge = nullptr;
    for (LaneVector& lanes : myLanes) {
        for (MSLane* lane : lanes) {
            const std::string customID = getParameter(lane->getID());
            if (noVehicles(lane->getPermissions()) && customID == "") {
                // do not build detectors on green verges or sidewalks
                continue;
            }
            if (laneInductLoopMap.find(lane) != laneInductLoopMap.end()) {
                // only build one detector per lane
                continue;
            }
            const SUMOTime minDur = getMinimumMinDuration(lane, multiNextTargets);
            if (minDur == std::numeric_limits<SUMOTime>::max() && customID == "" && !myBuildAllDetectors) {
                // only build detector if this lane is relevant for an actuated phase
                continue;
            }
            double length = lane->getLength();
            double ilpos;
            double inductLoopPosition;
            MSInductLoop* loop = nullptr;
            if (&lane->getEdge() != prevDetEdge) {
                detEdgeIndex++;
                detLaneIndex = 0;
                prevDetEdge = &lane->getEdge();
            } else {
                detLaneIndex++;
            }
            const bool isBikeLane = (lane->getPermissions() & ~SVC_PEDESTRIAN) == SVC_BICYCLE;
            const double defaultLength = isBikeLane ? DEFAULT_BIKE_LENGTH_WITH_GAP : DEFAULT_LENGTH_WITH_GAP;
            if (customID == "") {
                const double speed = isBikeLane ? DEFAULT_BICYCLE_SPEED : lane->getSpeedLimit();
                inductLoopPosition = MIN2(
                                         myDetectorGap * speed,
                                         (STEPS2TIME(minDur) / myPassingTime + 0.5) * defaultLength);

                // check whether the lane is long enough
                ilpos = length - inductLoopPosition;
                MSLane* placementLane = lane;
                while (ilpos < 0 && placementLane->getIncomingLanes().size() == 1
                        && placementLane->getIncomingLanes().front().viaLink->getCorrespondingEntryLink()->getTLLogic() == nullptr) {
                    placementLane = placementLane->getLogicalPredecessorLane();
                    ilpos += placementLane->getLength();
                }
                if (ilpos < 0) {
                    ilpos = 0;
                }
                // Build the induct loop and set it into the container
                const double detLength = getDouble("detector-length:" + lane->getID(), detDefaultLength);
                std::string id = myDetectorPrefix + "D" + toString(detEdgeIndex) + "." + toString(detLaneIndex);
                loop = static_cast<MSInductLoop*>(nb.createInductLoop(id, placementLane, ilpos, detLength, "", myVehicleTypes, "", (int)PersonMode::NONE, myShowDetectors));
                MSNet::getInstance()->getDetectorControl().add(SUMO_TAG_INDUCTION_LOOP, loop, myFile, myFreq);
            } else if (customID == NO_DETECTOR) {
                continue;
            } else {
                loop = dynamic_cast<MSInductLoop*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).get(customID));
                if (loop == nullptr) {
                    throw ProcessError(TLF("Unknown inductionLoop '%' given as custom detector for actuated tlLogic '%', program '%.", customID, getID(), getProgramID()));
                }
                ilpos = loop->getPosition();
                inductLoopPosition = length - ilpos;
            }
            const double maxGap = getDouble("max-gap:" + lane->getID(), myMaxGap);
            const double jamThreshold = getDouble("jam-threshold:" + lane->getID(), myJamThreshold);
            laneInductLoopMap[lane] = loop;
            inductLoopInfoMap[loop] = (int)myInductLoops.size();
            myInductLoops.push_back(InductLoopInfo(loop, lane, (int)myPhases.size(), maxGap, jamThreshold));

            if (warn && floor(floor(inductLoopPosition / defaultLength) * myPassingTime) > STEPS2TIME(minDur)) {
                // warn if the minGap is insufficient to clear vehicles between stop line and detector
                WRITE_WARNINGF(TL("At actuated tlLogic '%', minDur % is too short for a detector gap of %m."), getID(), time2string(minDur), toString(inductLoopPosition));
                warn = false;
            }
        }
    }
    // assign loops to phase index (myInductLoopsForPhase)
    //  check1: loops may not be used for a phase if there are other connections from the same lane that may not drive in that phase
    //            greenMinor is ambiguous as vehicles may not be able to drive
    //            Under the following condition we allow actuation from minor link:
    //              check1a : the minor link is minor in all phases
    //              check1b : there is another major link from the same lane in the current phase
    //              check1e : the conflict is only with bikes/pedestrians (i.e. for a right turn, also left turn with no oncoming traffic)
    //              check1f : the conflict is only with a link from the same edge
    //            (Under these conditions we assume that the minor link is unimportant and traffic is mostly for the major link)
    //
    //              check1c: when the edge has only one lane, we treat greenMinor as green as there would be no actuation otherwise
    //              check1d: for turnarounds 1b is sufficient and we do not require 1a
    //
    //  check2: if there are two loops on subsequent lanes (joined tls) and the second one has a red link, the first loop may not be used
    //
    //  if a jamThreshold is specificed for the loop, all checks are ignored

    // also assign loops to link index for validation:
    // check if all links from actuated phases (minDur != maxDur) have an inductionloop in at least one phase
    const SVCPermissions motorized = ~(SVC_PEDESTRIAN | SVC_BICYCLE);
    std::map<int, std::set<MSInductLoop*> > linkToLoops;
    std::set<int> actuatedLinks;

    std::vector<bool> neverMajor(numLinks, true);
    for (const MSPhaseDefinition* phase : myPhases) {
        const std::string& state = phase->getState();
        for (int i = 0; i < numLinks; i++)  {
            if (state[i] == LINKSTATE_TL_GREEN_MAJOR) {
                neverMajor[i] = false;
            }
        }
    }
    std::vector<bool> oneLane(numLinks, false);
    std::vector<bool> turnaround(numLinks, true);
    for (int i = 0; i < numLinks; i++)  {
        for (MSLane* lane : getLanesAt(i)) {
            // only count motorized vehicle lanes
            int numMotorized = 0;
            for (MSLane* l : lane->getEdge().getLanes()) {
                if ((l->getPermissions() & motorized) != 0) {
                    numMotorized++;
                }
            }
            if (numMotorized == 1) {
                oneLane[i] = true;
                break;
            }
        }
        for (MSLink* link : getLinksAt(i)) {
            if (!link->isTurnaround()) {
                turnaround[i] = false;
                break;
            }
        }
    }

    for (const MSPhaseDefinition* phase : myPhases) {
        const int phaseIndex = (int)myInductLoopsForPhase.size();
        std::set<MSInductLoop*> loops;
        if (phase->isActuated() || multiNextTargets.count(phaseIndex) != 0) {
            const std::string& state = phase->getState();
            // collect indices of all green links for the phase
            std::set<int> greenLinks;
            // green links that could jam
            std::set<int> greenLinksPermissive;
            // collect green links for each induction loops (in this phase)
            std::map<MSInductLoop*, std::set<int> > loopLinks;

            for (int i = 0; i < numLinks; i++)  {
                if (state[i] == LINKSTATE_TL_GREEN_MAJOR) {
                    greenLinks.insert(i);
                    if (phase->isActuated()) {
                        actuatedLinks.insert(i);
                    }

                    for (MSLink* link : getLinksAt(i)) {
                        if (link->getLane()->isCrossing()) {
                            while (myCrossingsForPhase.size() < myPhases.size()) {
                                myCrossingsForPhase.push_back(std::vector<const MSLink*>());
                            }
                            myCrossingsForPhase[phaseIndex].push_back(link);
                        }
                    }

                } else if (state[i] == LINKSTATE_TL_GREEN_MINOR) {
                    if (((neverMajor[i] || turnaround[i])  // check1a, 1d
                            && hasMajor(state, getLanesAt(i))) // check1b
                            || oneLane[i] // check1c
                            || weakConflict(i, state)) { // check1e, check1f
                        greenLinks.insert(i);
                        if (!turnaround[i]) {
                            if (phase->isActuated()) {
                                actuatedLinks.insert(i);
                            }
                        }
                    } else {
                        greenLinksPermissive.insert(i);
                    }
                }
#ifdef DEBUG_DETECTORS
                if (DEBUG_COND) {
                    std::cout << " phase=" << phaseIndex << " i=" << i << " state=" << state[i] << " green=" << greenLinks.count(i) << " oneLane=" << oneLane[i]
                              << " turn=" << turnaround[i] << " loopLanes=";
                    for (MSLane* lane : getLanesAt(i)) {
                        if (laneInductLoopMap.count(lane) != 0) {
                            std::cout << lane->getID() << " ";
                        }
                    }
                    std::cout << "\n";
                }
#endif
                for (MSLane* lane : getLanesAt(i)) {
                    if (laneInductLoopMap.count(lane) != 0) {
                        loopLinks[laneInductLoopMap[lane]].insert(i);
                    }
                }
            }
            for (auto& item : loopLinks) {
                MSInductLoop* loop = item.first;
                const InductLoopInfo& info = myInductLoops[inductLoopInfoMap[loop]];
                const MSLane* loopLane = info.lane;
                bool usable = true;
                bool foundUsable = false;
                // check1
                for (int j : item.second) {
                    if (greenLinks.count(j) == 0 && (info.jamThreshold <= 0 || greenLinksPermissive.count(j) == 0)) {
                        usable = false;
#ifdef DEBUG_DETECTORS
                        if (DEBUG_COND) {
                            std::cout << " phase=" << phaseIndex << " check1: loopLane=" << loopLane->getID() << " notGreen=" << j << " oneLane[j]=" << oneLane[j] << "\n";
                        }
#endif
                    } else {
                        foundUsable = true;
                    }
                }
                if (!usable && foundUsable && info.jamThreshold > 0) {
                    // permit green even when the same lane has green and red links (if we have jamDetection)
                    usable = true;
                }
                // check2 (skip if we have jam detection)
                if (usable && info.jamThreshold <= 0) {
                    for (MSLink* link : loopLane->getLinkCont()) {
                        if (link->isTurnaround()) {
                            continue;
                        }
                        const MSLane* next = link->getLane();
                        if (laneInductLoopMap.count(next) != 0) {
                            MSInductLoop* nextLoop = laneInductLoopMap[next];
                            for (int j : loopLinks[nextLoop]) {
                                if (greenLinks.count(j) == 0) {
                                    usable = false;
#ifdef DEBUG_DETECTORS
                                    if (DEBUG_COND) std::cout << " phase=" << phaseIndex << " check2: loopLane=" << loopLane->getID()
                                                                  << " nextLane=" << next->getID() << " nextLink=" << j << " nextState=" << state[j] << "\n";
#endif
                                    break;
                                }
                            }
                        }
                    }
                }

                if (usable) {
                    loops.insert(item.first);
#ifdef DEBUG_DETECTORS
                    if (DEBUG_COND) {
                        std::cout << " phase=" << phaseIndex << " usableLoops=" << item.first->getID() << " links=" << joinToString(item.second, " ") << "\n";
                    }
#endif
                    for (int j : item.second) {
                        linkToLoops[j].insert(item.first);
                    }
                }
            }
            if (loops.size() == 0 && phase->isActuated()) {
                WRITE_WARNINGF(TL("At actuated tlLogic '%', actuated phase % has no controlling detector."), getID(), toString(phaseIndex));
            }
        }
#ifdef DEBUG_DETECTORS
        if (DEBUG_COND) {
            std::cout << " phase=" << phaseIndex << " loops=" << joinNamedToString(loops, " ") << "\n";
        }
        if (DEBUG_COND) {
            std::cout << " linkToLoops:\n";
            for (auto item : linkToLoops) {
                std::cout << "   link=" << item.first << " loops=" << joinNamedToString(item.second, " ") << "\n";
            }
        }
#endif
        std::vector<InductLoopInfo*> loopInfos;
        myInductLoopsForPhase.push_back(loopInfos);
        for (MSInductLoop* loop : loops) {
            for (InductLoopInfo& loopInfo : myInductLoops) {
                if (loopInfo.loop == loop) {
                    myInductLoopsForPhase.back().push_back(&loopInfo);
                    loopInfo.servedPhase[phaseIndex] = true;
                }
            }
        }
    }
#ifdef DEBUG_DETECTORS
    if (DEBUG_COND) {
        std::cout << "final linkToLoops:\n";
        for (auto item : linkToLoops) {
            std::cout << "   link=" << item.first << " loops=" << joinNamedToString(item.second, " ") << "\n";
        }
    }
#endif
    std::vector<int> warnLinks;
    for (int i : actuatedLinks) {
        if (linkToLoops[i].size() == 0 && myLinks[i].size() > 0
                && (myLinks[i].front()->getLaneBefore()->getPermissions() & motorized) != 0) {
            if (getParameter(myLinks[i].front()->getLaneBefore()->getID()) != NO_DETECTOR) {
                warnLinks.push_back(i);
            }
        }
    }
    if (warnLinks.size() > 0) {
        WRITE_WARNINGF(TL("At actuated tlLogic '%', linkIndex % has no controlling detector."), getID(), joinToString(warnLinks, ","));
    }
    // parse maximum green times for each link (optional)
    for (const auto& kv : getParametersMap()) {
        if (StringUtils::startsWith(kv.first, "linkMaxDur:")) {
            int link = StringUtils::toInt(kv.first.substr(11));
            if (link < 0 || link >= myNumLinks) {
                WRITE_ERRORF(TL("Invalid link '%' given as linkMaxDur parameter for actuated tlLogic '%', program '%."), kv.first.substr(11), getID(), getProgramID());
                continue;
            }
            if (myLinkMaxGreenTimes.empty()) {
                myLinkMaxGreenTimes = std::vector<SUMOTime>(myNumLinks, std::numeric_limits<SUMOTime>::max());
            }
            myLinkMaxGreenTimes[link] = string2time(kv.second);
        } else if (StringUtils::startsWith(kv.first, "linkMinDur:")) {
            int link = StringUtils::toInt(kv.first.substr(11));
            if (link < 0 || link >= myNumLinks) {
                WRITE_ERRORF(TL("Invalid link '%' given as linkMinDur parameter for actuated tlLogic '%', program '%."), kv.first.substr(11), getID(), getProgramID());
                continue;
            }
            if (myLinkMinGreenTimes.empty()) {
                myLinkMinGreenTimes = std::vector<SUMOTime>(myNumLinks, 0);
            }
            myLinkMinGreenTimes[link] = string2time(kv.second);
        }
    }
    bool haveSwitchingRules = myConditions.size() > 0 || myAssignments.size() > 0 || myFunctions.size() > 0;
    for (auto sr : mySwitchingRules) {
        if (sr.enabled) {
            haveSwitchingRules = true;
            break;
        }
    }
    if (myLinkMaxGreenTimes.size() > 0 || myLinkMinGreenTimes.size() > 0 || haveSwitchingRules) {
        myLinkGreenTimes = std::vector<SUMOTime>(myNumLinks, 0);
        myLinkRedTimes = std::vector<SUMOTime>(myNumLinks, 0);
    }
    //std::cout << SIMTIME << " linkMaxGreenTimes=" << toString(myLinkMaxGreenTimes) << "\n";
}


bool
MSActuatedTrafficLightLogic::weakConflict(int tlIndex, const std::string& state) const {
    for (MSLink* link : getLinksAt(tlIndex)) {
        int linkIndex = link->getIndex();
        const MSJunction* junction = link->getJunction();
        for (int i = 0; i < (int)myLinks.size(); i++) {
            if (i == tlIndex) {
                continue;
            }
            if (state[i] == LINKSTATE_TL_GREEN_MAJOR || state[i] == LINKSTATE_TL_GREEN_MINOR) {
                for (MSLink* foe : getLinksAt(i)) {
                    // junction logic is based on junction link index rather than tl index
                    int foeIndex = foe->getIndex();
                    const MSJunction* junction2 = foe->getJunction();
                    if (junction == junction2) {
                        const MSJunctionLogic* logic = junction->getLogic();
                        //std::cout << " greenLink=" << i << " isFoe=" << logic->getFoesFor(linkIndex).test(foeIndex) << "\n";
                        if (logic->getFoesFor(linkIndex).test(foeIndex)
                                && (foe->getPermissions() & ~SVC_VULNERABLE) != 0 // check1e
                                && &foe->getLaneBefore()->getEdge() != &link->getLaneBefore()->getEdge()) { // check1f
                            //std::cout << " strongConflict " << tlIndex << " in phase " << state << " with link " << foe->getTLIndex() << "\n";
                            return false;
                        }
                    }
                }

            }
        }
    }
    //std::cout << " weakConflict " << tlIndex << " in phase " << state << "\n";
    return true;
}


SUMOTime
MSActuatedTrafficLightLogic::getMinDur(int step) const {
    step = step < 0 ? myStep : step;
    const MSPhaseDefinition* p = myPhases[step];
    return p->minDuration != MSPhaseDefinition::OVERRIDE_DURATION
           ? p->minDuration
           : TIME2STEPS(evalExpression(myConditions.find("minDur:" + toString(step))->second));
}

SUMOTime
MSActuatedTrafficLightLogic::getMaxDur(int step) const {
    step = step < 0 ? myStep : step;
    const MSPhaseDefinition* p = myPhases[step];
    return p->maxDuration != MSPhaseDefinition::OVERRIDE_DURATION
           ? p->maxDuration
           : TIME2STEPS(evalExpression(myConditions.find("maxDur:" + toString(step))->second));
}

SUMOTime
MSActuatedTrafficLightLogic::getEarliestEnd(int step) const {
    step = step < 0 ? myStep : step;
    const MSPhaseDefinition* p = myPhases[step];
    return p->earliestEnd != MSPhaseDefinition::OVERRIDE_DURATION
           ? p->earliestEnd
           : TIME2STEPS(evalExpression(myConditions.find("earliestEnd:" + toString(step))->second));
}

SUMOTime
MSActuatedTrafficLightLogic::getLatestEnd(int step) const {
    step = step < 0 ? myStep : step;
    const MSPhaseDefinition* p = myPhases[step];
    return p->latestEnd != MSPhaseDefinition::OVERRIDE_DURATION
           ? p->latestEnd
           : TIME2STEPS(evalExpression(myConditions.find("latestEnd:" + toString(step))->second));
}


void
MSActuatedTrafficLightLogic::initAttributeOverride() {
    const SUMOTime ovrd = MSPhaseDefinition::OVERRIDE_DURATION;
    for (int i = 0; i < (int)myPhases.size(); i++) {
        MSPhaseDefinition* phase = myPhases[i];
        const std::string errorSuffix = "' for overriding attribute in phase " + toString(i) + " of tlLogic '" + getID() + "' in program '" + getProgramID() + "'.";
        if (phase->minDuration == ovrd) {
            const std::string cond = "minDur:" + toString(i);
            if (myConditions.count(cond) == 0) {
                throw ProcessError("Missing condition '" + cond + errorSuffix);
            }
        }
        if (phase->maxDuration == ovrd) {
            const std::string cond = "maxDur:" + toString(i);
            if (myConditions.count(cond) == 0) {
                throw ProcessError("Missing condition '" + cond + errorSuffix);
            }
        }
        if (phase->earliestEnd == ovrd) {
            const std::string cond = "earliestEnd:" + toString(i);
            if (myConditions.count(cond) == 0) {
                throw ProcessError("Missing condition '" + cond + errorSuffix);
            }
        }
        if (phase->latestEnd == ovrd) {
            const std::string cond = "latestEnd:" + toString(i);
            if (myConditions.count(cond) == 0) {
                throw ProcessError("Missing condition '" + cond + errorSuffix);
            }
        }
    }
}


void
MSActuatedTrafficLightLogic::initSwitchingRules() {
    for (int i = 0; i < (int)myPhases.size(); i++) {
        SwitchingRules sr;
        MSPhaseDefinition* phase = myPhases[i];
        std::vector<int> nextPhases = phase->nextPhases;
        if (nextPhases.size() == 0) {
            nextPhases.push_back((i + 1) % (int)myPhases.size());
        } else if (nextPhases.size() > 1) {
            myHasMultiTarget = true;
        }
        for (int next : nextPhases) {
            if (next >= 0 && next < (int)myPhases.size()) {
                const MSPhaseDefinition* nextPhase = myPhases[next];
                if (nextPhase->earlyTarget != "" || nextPhase->finalTarget != "") {
                    sr.enabled = true;
                }
            }
        }
        // simplifies later code
        phase->nextPhases = nextPhases;
        mySwitchingRules.push_back(sr);
    }
}


void
MSActuatedTrafficLightLogic::initTargets(int step) {
    // next -> target -> transitionTime starting from step
    std::map<int, std::map<int, SUMOTime> > reached;
    const std::vector<int>& next = myPhases[step]->nextPhases;
    for (int n : next) {
        findTargets(step, n, 0, reached[n]);
    }
    for (int target = 0; target < (int)myPhases.size(); target++) {
        int bestNext = next[0];
        SUMOTime bestTime = SUMOTime_MAX;
        for (auto item : reached) {
            auto it = item.second.find(target);
            if (it != item.second.end()) {
                SUMOTime transitionTime = it->second;
                if (transitionTime < bestTime) {
                    bestTime = transitionTime;
                    bestNext = item.first;
                }
            }
        }
        if (bestTime != SUMOTime_MAX) {
            myTargets[step][bestNext].push_back(target);
            //std::cout << " myTargets step=" << step << " bestNext=" << bestNext << " target=" << target << "\n";
        }
    }
}


void
MSActuatedTrafficLightLogic::findTargets(int origStep, int n, SUMOTime priorTransition, std::map<int, SUMOTime>& found) {
    std::pair<int, SUMOTime> tDur = getTarget(n);
    int target = tDur.first;
    SUMOTime transitionTime = tDur.second + priorTransition;
    //std::cout << "   findTargets origStep=" << origStep << " n=" << n << " ptt=" << priorTransition << " target=" << target << " tt=" << transitionTime << "\n";
    if (target == origStep) {
        // full circle
        //std::cout << "     foundCircle\n";
        return;
    }
    auto it = found.find(target);
    if (it != found.end()) {
        if (it->second <= transitionTime) {
            //std::cout << "     oldShorterTime=" << it->second << "\n";
            // found the same target again
            return;
        } else {
            //std::cout << "     newShorterTime=" << it->second << "\n";
        }
    } else {
        //std::cout << "     newTarget\n";
    }
    found[target] = transitionTime;
    //std::cout << "    targetNext=" << toString(myPhases[target]->nextPhases) << "\n";
    for (int n2 : myPhases[target]->nextPhases) {
        findTargets(origStep, n2, transitionTime, found);
    }
}


std::set<int>
MSActuatedTrafficLightLogic::getMultiNextTargets() const {
    std::set<int> result;
    if (myHasMultiTarget) {
        // find all phase that are the target green phase of a 'next' attribute
        for (const MSPhaseDefinition* p : myPhases) {
            for (int next : p->nextPhases) {
                result.insert(getTarget(next).first);
            }
        }
    }
    return result;
}


SUMOTime
MSActuatedTrafficLightLogic::getMinimumMinDuration(MSLane* lane, const std::set<int>& multiNextTargets) const {
    SUMOTime result = std::numeric_limits<SUMOTime>::max();
    for (int pI = 0; pI < (int)myPhases.size(); pI++) {
        const MSPhaseDefinition* phase = myPhases[pI];
        const std::string& state = phase->getState();
        for (int i = 0; i < (int)state.size(); i++)  {
            if (state[i] == LINKSTATE_TL_GREEN_MAJOR || state[i] == LINKSTATE_TL_GREEN_MINOR) {
                for (MSLane* cand : getLanesAt(i)) {
                    if (lane == cand) {
                        if (phase->isActuated()) {
                            result = MIN2(result, getMinDur(pI));
                        } else if (multiNextTargets.count(pI) != 0) {
                            result = MIN2(result, DEFAULT_STATIC_MINDUR);
                        }
                    }
                }
            }
        }
    }
    return result;
}

bool
MSActuatedTrafficLightLogic::hasMajor(const std::string& state, const LaneVector& lanes) const {
    for (int i = 0; i < (int)state.size(); i++) {
        if (state[i] == LINKSTATE_TL_GREEN_MAJOR) {
            for (MSLane* cand : getLanesAt(i)) {
                for (MSLane* lane : lanes) {
                    if (lane == cand) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


// ------------ Switching and setting current rows
void
MSActuatedTrafficLightLogic::activateProgram() {
    MSTrafficLightLogic::activateProgram();
    for (InductLoopInfo& loopInfo : myInductLoops) {
        loopInfo.loop->setVisible(myShowDetectors);
    }
}


void
MSActuatedTrafficLightLogic::deactivateProgram() {
    MSTrafficLightLogic::deactivateProgram();
    for (InductLoopInfo& loopInfo : myInductLoops) {
        loopInfo.loop->setVisible(false);
    }
}

void
MSActuatedTrafficLightLogic::changeStepAndDuration(MSTLLogicControl& tlcontrol,
        SUMOTime simStep, int step, SUMOTime stepDuration) {
    // do not change timing if the phase changes
    if (step >= 0 && step != myStep) {
        myStep = step;
        myPhases[myStep]->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
        setTrafficLightSignals(simStep);
        tlcontrol.get(getID()).executeOnSwitchActions();
    } else if (step < 0) {
        // TraCI requested new timing
        mySwitchCommand->deschedule(this);
        mySwitchCommand = new SwitchCommand(tlcontrol, this, stepDuration + simStep);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
            mySwitchCommand, stepDuration + simStep);
        myTraCISwitch = true;
    }
}


void
MSActuatedTrafficLightLogic::loadState(MSTLLogicControl& tlcontrol, SUMOTime t, int step, SUMOTime spentDuration) {
    const SUMOTime lastSwitch = t - spentDuration;
    myStep = step;
    myPhases[myStep]->myLastSwitch = lastSwitch;
    const SUMOTime nextSwitch = t + getPhase(step).minDuration - spentDuration;
    mySwitchCommand->deschedule(this);
    mySwitchCommand = new SwitchCommand(tlcontrol, this, nextSwitch);
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(mySwitchCommand, nextSwitch);
    setTrafficLightSignals(lastSwitch);
    tlcontrol.get(getID()).executeOnSwitchActions();
}


SUMOTime
MSActuatedTrafficLightLogic::trySwitch() {
    // checks if the actual phase should be continued
    // @note any vehicles which arrived during the previous phases which are now waiting between the detector and the stop line are not
    // considere here. RiLSA recommends to set minDuration in a way that lets all vehicles pass the detector
    SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    executeAssignments(myAssignments, myConditions);

    if (myLinkGreenTimes.size() > 0) {
        // constraints exist, record green time durations for each link
        const std::string& state = getCurrentPhaseDef().getState();
        SUMOTime lastDuration = SIMSTEP - myLastTrySwitchTime;
        for (int i = 0; i < myNumLinks; i++) {
            if (state[i] == 'G' || state[i] == 'g') {
                myLinkGreenTimes[i] += lastDuration;
            } else {
                myLinkGreenTimes[i] = 0;
            }
            if (state[i] == 'r' || state[i] == 'u') {
                myLinkRedTimes[i] += lastDuration;
            } else {
                myLinkRedTimes[i] = 0;
            }
        }
    }
    myLastTrySwitchTime = now;
    // decide the next phase
    const bool multiTarget = myPhases[myStep]->nextPhases.size() > 1 && myPhases[myStep]->nextPhases.front() >= 0;
    const int origStep = myStep;
    int nextStep = myStep;
    SUMOTime actDuration = now - myPhases[myStep]->myLastSwitch;

    if (mySwitchingRules[myStep].enabled) {
        const bool mustSwitch = MIN2(getMaxDur() - actDuration, getLatest()) <= 0;
        nextStep = decideNextPhaseCustom(mustSwitch);
    } else {
        // default algorithm
        const double detectionGap = gapControl();
#ifdef DEBUG_PHASE_SELECTION
        if (DEBUG_COND) {
            std::cout << SIMTIME << " p=" << myStep
                      << " trySwitch dGap=" << (detectionGap == std::numeric_limits<double>::max() ? "inf" : toString(detectionGap))
                      << " multi=" << multiTarget << "\n";
        }
#endif
        if (detectionGap < std::numeric_limits<double>::max() && !multiTarget && !myTraCISwitch) {
            return duration(detectionGap);
        }
        if (multiTarget) {
            nextStep = decideNextPhase();
        } else {
            if (myPhases[myStep]->nextPhases.size() == 1 && myPhases[myStep]->nextPhases.front() >= 0) {
                nextStep = myPhases[myStep]->nextPhases.front();
            } else {
                nextStep = (myStep + 1) % (int)myPhases.size();
            }
        }
    }

    myTraCISwitch = false;
    if (myLinkMinGreenTimes.size() > 0) {
        SUMOTime linkMinDur = getLinkMinDuration(getTarget(nextStep).first);
        if (linkMinDur > 0) {
            // for multiTarget, the current phase must be extended but if another
            // targer is chosen, earlier switching than linkMinDur is possible
            return multiTarget ? TIME2STEPS(1) : linkMinDur;
        }
    }
    myStep = nextStep;
    assert(myStep <= (int)myPhases.size());
    assert(myStep >= 0);
    //stores the time the phase started
    const SUMOTime prevStart = myPhases[myStep]->myLastSwitch;
    if (myStep != origStep) {
        myPhases[origStep]->myLastEnd = now;
        myPhases[myStep]->myLastSwitch = now;
        actDuration = 0;
    }
    // activate coloring
    if ((myShowDetectors || myHasMultiTarget) && getCurrentPhaseDef().isGreenPhase()) {
        for (InductLoopInfo* loopInfo : myInductLoopsForPhase[myStep]) {
            //std::cout << SIMTIME << " p=" << myStep << " loopinfo=" << loopInfo->loop->getID() << " set lastGreen=" << STEPS2TIME(now) << "\n";
            if (loopInfo->isJammed()) {
                loopInfo->loop->setSpecialColor(&RGBColor::ORANGE);
            } else {
                loopInfo->loop->setSpecialColor(&RGBColor::GREEN);
            }
            loopInfo->lastGreenTime = now;
        }
    }
    // set the next event
#ifdef DEBUG_PHASE_SELECTION
    if (DEBUG_COND) {
        std::cout << SIMTIME << " tl=" << getID() << " p=" << myStep
                  << " nextTryMinDur=" << STEPS2TIME(getMinDur() - actDuration)
                  << " nextTryEarliest=" << STEPS2TIME(getEarliest(prevStart)) << "\n";
    }
#endif
    SUMOTime minRetry = myStep != origStep ? 0 : TIME2STEPS(1);
    return MAX3(minRetry, getMinDur() - actDuration, getEarliest(prevStart));
}


// ------------ "actuated" algorithm methods
SUMOTime
MSActuatedTrafficLightLogic::duration(const double detectionGap) const {
    assert(getCurrentPhaseDef().isGreenPhase());
    assert((int)myPhases.size() > myStep);
    const SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - myPhases[myStep]->myLastSwitch;
    // ensure that minimum duration is kept
    SUMOTime newDuration = getMinDur() - actDuration;
    // try to let the last detected vehicle pass the intersection (duration must be positive)
    newDuration = MAX3(newDuration, TIME2STEPS(myDetectorGap - detectionGap), SUMOTime(1));
    // cut the decimal places to ensure that phases always have integer duration
    if (newDuration % 1000 != 0) {
        const SUMOTime totalDur = newDuration + actDuration;
        newDuration = (totalDur / 1000 + 1) * 1000 - actDuration;
    }
    // ensure that the maximum duration is not exceeded
    newDuration = MIN3(newDuration, getMaxDur() - actDuration, getLatest());
    return newDuration;
}


double
MSActuatedTrafficLightLogic::gapControl() {
    //intergreen times should not be lengthend
    assert((int)myPhases.size() > myStep);
    double result = std::numeric_limits<double>::max();
    if (MSGlobals::gUseMesoSim) {
        return result;
    }
    // switch off active colors
    if (myShowDetectors) {
        for (InductLoopInfo& loopInfo : myInductLoops) {
            if (loopInfo.lastGreenTime < loopInfo.loop->getLastDetectionTime()) {
                loopInfo.loop->setSpecialColor(&RGBColor::RED);
            } else {
                loopInfo.loop->setSpecialColor(nullptr);
            }
        }
    }
    if (!getCurrentPhaseDef().isGreenPhase()) {
        return result; // end current phase
    }

    // Checks, if the maxDuration is kept. No phase should last longer than maxDuration.
    SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - myPhases[myStep]->myLastSwitch;
    if (actDuration >= getCurrentPhaseDef().maxDuration || maxLinkDurationReached() || getLatest() == 0) {
#ifdef DEBUG_PHASE_SELECTION
        if (DEBUG_COND) {
            std::cout << SIMTIME << " actDuration=" << STEPS2TIME(actDuration) << " maxDur=" << STEPS2TIME(getCurrentPhaseDef().maxDuration)
                      << " maxLinkDurationReached=" << maxLinkDurationReached() << " latest=" << STEPS2TIME(getLatest()) << "\n";
        }
#endif
        return result; // end current phase
    }

    // now the gapcontrol starts
    for (InductLoopInfo* loopInfo : myInductLoopsForPhase[myStep]) {
        MSInductLoop* loop = loopInfo->loop;
        if (loopInfo->isJammed()) {
            loopInfo->loop->setSpecialColor(&RGBColor::ORANGE);
        } else {
            loopInfo->loop->setSpecialColor(&RGBColor::GREEN);
        }
        const double actualGap = loop->getTimeSinceLastDetection();
        if (actualGap < loopInfo->maxGap && !loopInfo->isJammed()) {
            result = MIN2(result, actualGap);
        }
    }
    return result;
}

int
MSActuatedTrafficLightLogic::decideNextPhase() {
    const auto& cands = myPhases[myStep]->nextPhases;
    // decide by priority
    // first target is the default when there is no traffic
    // @note: to keep the current phase, even when there is no traffic, it must be added to 'next' explicitly
    int result = cands.front();
    int maxPrio = 0;
    SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - myPhases[myStep]->myLastSwitch;
    const bool canExtend = actDuration < getCurrentPhaseDef().maxDuration && !maxLinkDurationReached() && getLatest() > 0;
    if (canExtend) {
        // consider keeping the current phase until maxDur is reached
        // (only when there is still traffic in that phase)
        int currentPrio = getPhasePriority(myStep);
#ifdef DEBUG_PHASE_SELECTION
        std::cout << SIMTIME << " p=" << myStep << " loops=" << myInductLoopsForPhase[myStep].size() << " currentPrio=" << currentPrio << "\n";
#endif
        if (currentPrio > maxPrio) {
            result = myStep;
            maxPrio = currentPrio;
        }
    }
    for (int step : cands) {
        int prio = 0;
        for (int target : myTargets[myStep][step]) {
            prio += getPhasePriority(target);
#ifdef DEBUG_PHASE_SELECTION
            if (DEBUG_COND) {
                std::cout << SIMTIME << " p=" << myStep << " step=" << step << " target=" << target << " loops=" << myInductLoopsForPhase[target].size() << " prio=" << prio << "\n";
            }
#endif
        }
        if (prio > maxPrio && canExtendLinkGreen(getTarget(step).first)) {
            maxPrio = prio;
            result = step;
        }
    }
    return result;
}


std::pair<int, SUMOTime>
MSActuatedTrafficLightLogic::getTarget(int step) const {
    int seen = 0;
    int origStep = step;
    SUMOTime dur = 0;

    // if step is a transition, find the upcoming green phase
    while (!myPhases[step]->isGreenPhase()) {
        seen += 1;
        dur += myPhases[step]->duration;
        if (myPhases[step]->nextPhases.size() > 0 && myPhases[step]->nextPhases.front() >= 0) {
            for (int next : myPhases[step]->nextPhases) {
                if (next != step) {
                    step = next;
                    break;
                }
            }
        } else {
            step = (step + 1) % (int)myPhases.size();
        }
        if (step == origStep || seen > (int)myPhases.size()) {
            WRITE_WARNING("At actuated tlLogic '" + getID() + "', infinite transition loop from phase " + toString(origStep));
            return std::make_pair(0, 0);
        }
    }
    return std::make_pair(step, dur);
}

int
MSActuatedTrafficLightLogic::getDetectorPriority(const InductLoopInfo& loopInfo) const {
    MSInductLoop* loop = loopInfo.loop;
    const double actualGap = loop->getTimeSinceLastDetection();
    if ((actualGap < loopInfo.maxGap && !loopInfo.isJammed())
            || loopInfo.lastGreenTime < loop->getLastDetectionTime()) {
        SUMOTime inactiveTime = MSNet::getInstance()->getCurrentTimeStep() - loopInfo.lastGreenTime;
        // @note. Inactive time could also be tracked regardless of current activity (to increase robustness in case of detection failure
        if (inactiveTime > myInactiveThreshold) {
#ifdef DEBUG_PHASE_SELECTION
            if (DEBUG_COND) {
                std::cout << "    loop=" << loop->getID() << " gap=" << loop->getTimeSinceLastDetection() << " lastGreen=" << STEPS2TIME(loopInfo.lastGreenTime)
                          << " lastDetection=" << STEPS2TIME(loop->getLastDetectionTime()) << " inactive=" << STEPS2TIME(inactiveTime) << "\n";
            }
#endif
            return (int)STEPS2TIME(inactiveTime);
        } else {
            // give bonus to detectors that are currently served (if that phase can stil be extended)
            if (loopInfo.servedPhase[myStep]) {
                SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - myPhases[myStep]->myLastSwitch;
                const bool canExtend = actDuration < getCurrentPhaseDef().maxDuration && getLatest() > 0;
#ifdef DEBUG_PHASE_SELECTION
                if (DEBUG_COND) {
                    std::cout << "    loop=" << loop->getID()
                              << " actDuration=" << STEPS2TIME(actDuration)
                              << " maxDur=" << STEPS2TIME(getCurrentPhaseDef().maxDuration)
                              << " getLatest=" << STEPS2TIME(getLatest())
                              << " canExtend=" << canExtend
                              << "\n";
                }
#endif
                if (canExtend) {
                    return DEFAULT_CURRENT_PRIORITY;
                } else {
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

int
MSActuatedTrafficLightLogic::getPhasePriority(int step) const {
    int result = 0;
    for (const InductLoopInfo* loopInfo : myInductLoopsForPhase[step]) {
        result += getDetectorPriority(*loopInfo);
    }
    if (myCrossingsForPhase.size() > 0) {
        for (const MSLink* crossingEntry : myCrossingsForPhase[step]) {
            auto* aPersons = crossingEntry->getApproachingPersons();
            if (aPersons != nullptr && aPersons->size() > 0) {
                result += DEFAULT_CROSSING_PRIORITY;
            }
        }
    }
    return result;
}


void
MSActuatedTrafficLightLogic::setShowDetectors(bool show) {
    myShowDetectors = show;
    for (InductLoopInfo& loopInfo : myInductLoops) {
        loopInfo.loop->setVisible(myShowDetectors);
    }
}


bool
MSActuatedTrafficLightLogic::maxLinkDurationReached() {
    if (myLinkMaxGreenTimes.empty()) {
        return false;
    }
    for (int i = 0; i < myNumLinks; i++) {
        if (myLinkGreenTimes[i] >= myLinkMaxGreenTimes[i]) {
            //std::cout << SIMTIME << " maxLinkDurationReached i=" << i << "\n";
            return true;
        }
    }
    return false;
}

bool
MSActuatedTrafficLightLogic::canExtendLinkGreen(int target) {
    if (myLinkMaxGreenTimes.empty()) {
        return true;
    }
    const std::string& targetState = myPhases[target]->getState();
    for (int i = 0; i < myNumLinks; i++) {
        if (myLinkGreenTimes[i] >= myLinkMaxGreenTimes[i] && (
                    targetState[i] == 'G' || targetState[i] == 'g')) {
            //std::cout << SIMTIME << " cannotExtendLinkGreen target=" << target << " i=" << i << "\n";
            return false;
        }
    }
    return true;
}

SUMOTime
MSActuatedTrafficLightLogic::getLinkMinDuration(int target) const {
    SUMOTime result = 0;
    if (target != myStep && myLinkMinGreenTimes.size() > 0) {
        const std::string& state = myPhases[myStep]->getState();
        const std::string& targetState = myPhases[target]->getState();
        for (int i = 0; i < myNumLinks; i++) {
            if (myLinkGreenTimes[i] < myLinkMinGreenTimes[i]
                    && (state[i] == 'G' || state[i] == 'g')
                    && !(targetState[i] == 'G' || targetState[i] == 'g')) {
                result = MAX2(result, myLinkMinGreenTimes[i] - myLinkGreenTimes[i]);
                //std::cout << SIMTIME << " getLinkMinDuration myStep=" << myStep << " target=" << target << " i=" << i
                //    << " greenTime=" << STEPS2TIME(myLinkGreenTimes[i]) << " min=" << STEPS2TIME(myLinkMinGreenTimes[i]) << " result=" << STEPS2TIME(result) << "\n";
            }
        }
    }
    return result;
}

int
MSActuatedTrafficLightLogic::decideNextPhaseCustom(bool mustSwitch) {
    for (int next : getCurrentPhaseDef().nextPhases) {
        const MSPhaseDefinition* phase = myPhases[next];
        const std::string& condition = mustSwitch ? phase->finalTarget : phase->earlyTarget;
#ifdef DEBUG_PHASE_SELECTION_CUSTOM
        if (DEBUG_COND) {
            std::cout << SIMTIME << " mustSwitch=" << mustSwitch << " cur=" << myStep << " next=" << next << " condition=" << condition
                      << " eval=" << (condition == "" ? NAN : evalExpression(condition)) << "\n";
        }
#endif
        if (condition != "") {
            // backward compatibility if a user redefined DEFAULT_CONDITION
            if (condition == DEFAULT_CONDITION && myConditions.count(DEFAULT_CONDITION) == 0) {
                if (gapControl() == std::numeric_limits<double>::max()) {
                    return next;
                }
            } else if (evalExpression(condition)) {
                return next;
            }
        }
    }
    return mustSwitch ? getCurrentPhaseDef().nextPhases.back() : myStep;
}


double
MSActuatedTrafficLightLogic::evalExpression(const std::string& condition) const {
    const size_t bracketOpen = condition.find('(');
    if (bracketOpen != std::string::npos) {
        // find matching closing bracket
        size_t bracketClose = std::string::npos;
        int open = 1;
        for (size_t i = bracketOpen + 1; i < condition.size(); i++) {
            if (condition[i] == '(') {
                open++;
            } else if (condition[i] == ')') {
                open--;
                if (open == 0) {
                    bracketClose = i;
                    break;
                }
            }
        }
        if (bracketClose == std::string::npos) {
            throw ProcessError(TLF("Unmatched parentheses in condition %'", condition));
        }
        std::string cond2 = condition;
        const std::string inBracket = condition.substr(bracketOpen + 1, bracketClose - bracketOpen - 1);
        double bracketVal = evalExpression(inBracket);
        cond2.replace(bracketOpen, bracketClose - bracketOpen + 1, toString(bracketVal));
        try {
            return evalExpression(cond2);
        } catch (ProcessError& e) {
            throw ProcessError(TLF("Error when evaluating expression '%':\n  %", condition, e.what()));
        }
    }
    std::vector<std::string> tokens = StringTokenizer(condition).getVector();
    //std::cout << SIMTIME << " tokens(" << tokens.size() << ")=" << toString(tokens) << "\n";
    if (tokens.size() == 0) {
        throw ProcessError(TLF("Invalid empty condition '%'", condition));
    } else if (tokens.size() == 1) {
        try {
            return evalAtomicExpression(tokens[0]);
        } catch (ProcessError& e) {
            throw ProcessError(TLF("Error when evaluating expression '%':\n  %", condition, e.what()));
        }
    } else if (tokens.size() == 2) {
        if (tokens[0] == "not") {
            try {
                return evalAtomicExpression(tokens[1]) == 0. ? 1. : 0.;
            } catch (ProcessError& e) {
                throw ProcessError(TLF("Error when evaluating expression '%':\n  %", condition, e.what()));
            }
        } else {
            throw ProcessError(TLF("Unsupported condition '%'", condition));
        }
    } else if (tokens.size() == 3) {
        // infix expression
        const double a = evalAtomicExpression(tokens[0]);
        const double b = evalAtomicExpression(tokens[2]);
        const std::string& o = tokens[1];
        //std::cout << SIMTIME << " o=" << o << " a=" << a << " b=" << b << "\n";
        try {
            return evalTernaryExpression(a, o, b, condition);
        } catch (ProcessError& e) {
            throw ProcessError(TLF("Error when evaluating expression '%':\n  %", condition, e.what()));
        }
    } else {
        const int iEnd = (int)tokens.size() - 1;
        for (const std::string& o : OPERATOR_PRECEDENCE) {
            for (int i = 1; i < iEnd; i++) {
                if (tokens[i] == o) {
                    try {
                        const double val = evalTernaryExpression(
                                               evalAtomicExpression(tokens[i - 1]), o,
                                               evalAtomicExpression(tokens[i + 1]), condition);
                        std::vector<std::string> newTokens(tokens.begin(), tokens.begin() + (i - 1));
                        newTokens.push_back(toString(val));
                        newTokens.insert(newTokens.end(), tokens.begin() + (i + 2), tokens.end());
                        return evalExpression(toString(newTokens));
                    } catch (ProcessError& e) {
                        throw ProcessError(TLF("Error when evaluating expression '%':\n  %", condition, e.what()));
                    }
                }
            }
        }
        throw ProcessError(TLF("Parsing expressions with % elements ('%') is not supported", toString(tokens.size()), condition));
    }
    return true;
}

double
MSActuatedTrafficLightLogic::evalTernaryExpression(double a, const std::string& o, double b, const std::string& condition) const {
    if (o == "=" || o == "==") {
        return (double)(a == b);
    } else if (o == "<") {
        return (double)(a < b);
    } else if (o == ">") {
        return (double)(a > b);
    } else if (o == "<=") {
        return (double)(a <= b);
    } else if (o == ">=") {
        return (double)(a >= b);
    } else if (o == "!=") {
        return (double)(a != b);
    } else if (o == "or" || o == "||") {
        return (double)(a || b);
    } else if (o == "and" || o == "&&") {
        return (double)(a && b);
    } else if (o == "+") {
        return a + b;
    } else if (o == "-") {
        return a - b;
    } else if (o == "*") {
        return a * b;
    } else if (o == "/") {
        if (b == 0) {
            WRITE_ERRORF(TL("Division by 0 in condition '%'"), condition);
            return 0;
        }
        return a / b;
    } else if (o == "%") {
        return fmod(a, b);
    } else if (o == "**" || o == "^") {
        return pow(a, b);
    } else  {
        throw ProcessError(TLF("Unsupported operator '%' in condition '%'", o, condition));
    }
}

double
MSActuatedTrafficLightLogic::evalCustomFunction(const std::string& fun, const std::string& arg) const {
    std::vector<std::string> args = StringTokenizer(arg, ",").getVector();
    const Function& f = myFunctions.find(fun)->second;
    if ((int)args.size() != f.nArgs) {
        throw ProcessError(TLF("Function '%' requires % arguments but % were given", fun, toString(f.nArgs), toString(args.size())));
    }
    std::vector<double> args2;
    for (auto a : args) {
        args2.push_back(evalExpression(a));
    }
    myStack.push_back(myStack.back());
    myStack.back()["$0"] = 0;
    for (int i = 0; i < (int)args2.size(); i++) {
        myStack.back()["$" + toString(i + 1)] = args2[i];
    }
    try {
        ConditionMap empty;
        executeAssignments(f.assignments, empty, myConditions);
    } catch (ProcessError& e) {
        throw ProcessError(TLF("Error when evaluating function '%' with args '%' (%)", fun, joinToString(args2, ","), e.what()));
    }
    double result = myStack.back()["$0"];
    myStack.pop_back();
    return result;
}


void
MSActuatedTrafficLightLogic::executeAssignments(const AssignmentMap& assignments, ConditionMap& conditions, const ConditionMap& forbidden) const {
    for (const auto& assignment : assignments) {
        if (evalExpression(std::get<1>(assignment))) {
            const std::string& id = std::get<0>(assignment);
            const double val = evalExpression(std::get<2>(assignment));
            ConditionMap::iterator it = conditions.find(id);
            if (it != conditions.end()) {
                it->second = toString(val);
            } else if (forbidden.find(id) != forbidden.end()) {
                throw ProcessError(TLF("Modifying global condition '%' is forbidden", id));
            } else {
                myStack.back()[id] = val;
            }
        }
    }
}


double
MSActuatedTrafficLightLogic::evalAtomicExpression(const std::string& expr) const {
    if (expr.size() == 0) {
        throw ProcessError(TL("Invalid empty expression"));
    } else if (expr[0] == '!') {
        return evalAtomicExpression(expr.substr(1)) == 0. ? 1. : 0.;
    } else if (expr[0] == '-') {
        return -evalAtomicExpression(expr.substr(1));
    } else {
        // check for 'operator:'
        const size_t pos = expr.find(':');
        if (pos == std::string::npos) {
            auto it = myConditions.find(expr);
            if (it != myConditions.end()) {
                // symbol lookup
                return evalExpression(it->second);
            } else {
                // look at stack
                auto it2 = myStack.back().find(expr);
                if (it2 != myStack.back().end()) {
                    return it2->second;
                }
                // must be a number
                return StringUtils::toDouble(expr);
            }
        } else {
            const std::string fun = expr.substr(0, pos);
            const std::string arg = expr.substr(pos + 1);
            if (fun == "z") {
                return retrieveDetExpression<MSInductLoop, SUMO_TAG_INDUCTION_LOOP>(arg, expr, true)->getTimeSinceLastDetection();
            } else if (fun == "a") {
                try {
                    return retrieveDetExpression<MSInductLoop, SUMO_TAG_INDUCTION_LOOP>(arg, expr, true)->getTimeSinceLastDetection() == 0;
                } catch (ProcessError&) {
                    return retrieveDetExpression<MSE2Collector, SUMO_TAG_LANE_AREA_DETECTOR>(arg, expr, true)->getCurrentVehicleNumber();
                }
            } else if (fun == "w") {
                try {
                    return retrieveDetExpression<MSInductLoop, SUMO_TAG_INDUCTION_LOOP>(arg, expr, true)->getOccupancyTime();
                } catch (ProcessError&) {
                    return retrieveDetExpression<MSE2Collector, SUMO_TAG_LANE_AREA_DETECTOR>(arg, expr, true)->getCurrentJamDuration();
                }
            } else if (fun == "g" || fun == "r") {
                try {
                    int linkIndex = StringUtils::toInt(arg);
                    if (linkIndex >= 0 && linkIndex < myNumLinks) {
                        const std::vector<SUMOTime>& times = fun == "g" ? myLinkGreenTimes : myLinkRedTimes;
                        if (times.empty()) {
                            return 0;
                        }
                        if (myLastTrySwitchTime < SIMSTEP) {
                            // times are only updated at the start of a phase where
                            // switching is possible (i.e. not during minDur).
                            // If somebody is looking at those values in the tracker
                            // this would be confusing
                            const LinkState ls = getCurrentPhaseDef().getSignalState(linkIndex);
                            if ((fun == "g" && (ls == LINKSTATE_TL_GREEN_MAJOR || ls == LINKSTATE_TL_GREEN_MINOR))
                                    || (fun == "r" && (ls == LINKSTATE_TL_RED || ls == LINKSTATE_TL_REDYELLOW))) {
                                const SUMOTime currentGreen = SIMSTEP - myLastTrySwitchTime;
                                return STEPS2TIME(times[linkIndex] + currentGreen);
                            } else {
                                return 0;
                            }
                        } else {
                            return STEPS2TIME(times[linkIndex]);
                        }
                    }
                } catch (NumberFormatException&) { }
                throw ProcessError(TLF("Invalid link index '%' in expression '%'", arg, expr));
            } else if (fun == "p") {
                try {
                    int linkIndex = StringUtils::toInt(arg);
                    if (linkIndex >= 0 && linkIndex < myNumLinks) {
                        double approachingPersons = 0;
                        for (const MSLink* link : getLinksAt(linkIndex)) {
                            auto* aPersons = link->getApproachingPersons();
                            if (aPersons != nullptr) {
                                approachingPersons += (double)aPersons->size();
                            }
                        }
                        return approachingPersons;
                    }
                } catch (NumberFormatException&) { }
                throw ProcessError(TLF("Invalid link index '%' in expression '%'", arg, expr));
            } else if (fun == "c") {
                return STEPS2TIME(getTimeInCycle());
            } else {
                if (myFunctions.find(fun) == myFunctions.end()) {
                    throw ProcessError(TLF("Unsupported function '%' in expression '%'", fun, expr));
                }
                return evalCustomFunction(fun, arg);
            }
        }
    }
}


std::map<std::string, double>
MSActuatedTrafficLightLogic::getDetectorStates() const {
    std::map<std::string, double> result;
    for (auto li : myInductLoops) {
        result[li.loop->getID()] = li.loop->getOccupancy() > 0 ? 1 : 0;
    }
    for (auto loop : myExtraLoops) {
        result[loop->getID()] = loop->getOccupancy() > 0 ? 1 : 0;
    }
    for (auto loop : myExtraE2) {
        result[loop->getID()] = loop->getCurrentVehicleNumber();
    }
    return result;
}

double
MSActuatedTrafficLightLogic::getDetectorState(const std::string laneID) const {
    double result = 0.0;
    for (auto li : myInductLoops) {
        if (li.lane->getID() == laneID) {
            result = li.loop->getOccupancy() > 0 ? 1 : 0;
            break;
        }
    }
    return result;
}

std::map<std::string, double>
MSActuatedTrafficLightLogic::getConditions() const {
    std::map<std::string, double> result;
    for (auto item : myConditions) {
        if (myListedConditions.count(item.first) != 0) {
            try {
                result[item.first] = evalExpression(item.second);
            } catch (ProcessError& e) {
                WRITE_ERRORF(TL("Error when retrieving conditions '%' for tlLogic '%' (%)"), item.first, getID(), e.what());
            }
        }
    }
    return result;
}

const std::string
MSActuatedTrafficLightLogic::getParameter(const std::string& key, const std::string defaultValue) const {
    if (StringUtils::startsWith(key, "condition.")) {
        const std::string cond = key.substr(10);
        auto it = myConditions.find(cond);
        if (it != myConditions.end()) {
            return toString(evalExpression(it->second));
        } else {
            throw InvalidArgument(TLF("Unknown condition '%' for actuated traffic light '%'", cond, getID()));
        }
    } else {
        return MSSimpleTrafficLightLogic::getParameter(key, defaultValue);
    }
}

void
MSActuatedTrafficLightLogic::setParameter(const std::string& key, const std::string& value) {
    // some pre-defined parameters can be updated at runtime
    if (key == "detector-gap" || key == "passing-time" || key == "file" || key == "freq" || key == "vTypes"
            || key == "build-all-detectors"
            || StringUtils::startsWith(key, "linkMaxDur")
            || StringUtils::startsWith(key, "linkMinDur")) {
        throw InvalidArgument(key + " cannot be changed dynamically for actuated traffic light '" + getID() + "'");
    } else if (key == "max-gap") {
        myMaxGap = StringUtils::toDouble(value);
        // overwrite custom values
        for (InductLoopInfo& loopInfo : myInductLoops) {
            loopInfo.maxGap = myMaxGap;
        }
        Parameterised::setParameter(key, value);
    } else if (StringUtils::startsWith(key, "max-gap:")) {
        const std::string laneID = key.substr(8);
        for (InductLoopInfo& loopInfo : myInductLoops) {
            if (loopInfo.lane->getID() == laneID) {
                loopInfo.maxGap = StringUtils::toDouble(value);
                Parameterised::setParameter(key, value);
                return;
            }
        }
        throw InvalidArgument(TLF("Invalid lane '%' in key '%' for actuated traffic light '%'", laneID, key, getID()));
    } else if (key == "jam-threshold") {
        myJamThreshold = StringUtils::toDouble(value);
        // overwrite custom values
        for (InductLoopInfo& loopInfo : myInductLoops) {
            loopInfo.jamThreshold = myJamThreshold;
        }
        Parameterised::setParameter(key, value);
    } else if (StringUtils::startsWith(key, "jam-threshold:")) {
        const std::string laneID = key.substr(14);
        for (InductLoopInfo& loopInfo : myInductLoops) {
            if (loopInfo.lane->getID() == laneID) {
                loopInfo.jamThreshold = StringUtils::toDouble(value);
                Parameterised::setParameter(key, value);
                return;
            }
        }
        throw InvalidArgument(TLF("Invalid lane '%' in key '%' for actuated traffic light '%'", laneID, key, getID()));
    } else if (key == "show-detectors") {
        myShowDetectors = StringUtils::toBool(value);
        Parameterised::setParameter(key, value);
        for (InductLoopInfo& loopInfo : myInductLoops) {
            loopInfo.loop->setVisible(myShowDetectors);
        }
    } else if (key == "inactive-threshold") {
        myInactiveThreshold = string2time(value);
        Parameterised::setParameter(key, value);
    } else {
        MSSimpleTrafficLightLogic::setParameter(key, value);
    }
}


/****************************************************************************/
