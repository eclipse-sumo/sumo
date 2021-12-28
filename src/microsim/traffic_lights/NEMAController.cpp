/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    NEMAController.cpp
/// @author  Tianxin Li
/// @author  Qichao Wang
/// @date    August 2020
///
// An actuated NEMA-phase-compliant traffic light logic
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "NEMAController.h"
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <netload/NLDetectorBuilder.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include "microsim/output/MSE2Collector.h"
#include <sstream>
#include <iostream>
// ===========================================================================
// parameter defaults definitions
// ===========================================================================


#define INVALID_POSITION std::numeric_limits<double>::max() // tl added

// #define DEBUG_NEMA

// ===========================================================================
// method definitions
// ===========================================================================
NEMALogic::NEMALogic(MSTLLogicControl& tlcontrol,
                     const std::string& id, const std::string& programID,
                     const SUMOTime _offset,
                     const Phases& phases,
                     int /*step*/, SUMOTime /*delay*/,
                     const std::map<std::string, std::string>& parameter,
                     const std::string& basePath) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, _offset, TrafficLightType::NEMA, phases, 0, phases.front()->minDuration + SIMSTEP, parameter),
    myPhase(phases[0]->duration, phases[0]->getState()) {
    myDetectorLength = StringUtils::toDouble(getParameter("detector-length", "20"));
    myDetectorLengthLeftTurnLane = StringUtils::toDouble(getParameter("detector-length-leftTurnLane", "20"));
    myCycleLength = (StringUtils::toDouble(getParameter("total-cycle-length", getParameter("cycle-length", "60"))));
    myNextCycleLength = myCycleLength;
    myDefaultCycleTime = TIME2STEPS(myCycleLength);
    myShowDetectors = StringUtils::toBool(getParameter("show-detectors", toString(OptionsCont::getOptions().getBool("tls.actuated.show-detectors"))));
    myFile = FileHelpers::checkForRelativity(getParameter("file", "NUL"), basePath);
    myFreq = TIME2STEPS(StringUtils::toDouble(getParameter("freq", "300")));
    myVehicleTypes = getParameter("vTypes", "");
    ring1 = getParameter("ring1", "");
    ring2 = getParameter("ring2", "");
    barriers = getParameter("barrierPhases", "");
    coordinates = getParameter("coordinatePhases", getParameter("barrier2Phases",""));
    offset = STEPS2TIME(_offset);
    myNextOffset = offset;
    whetherOutputState = StringUtils::toBool(getParameter("whetherOutputState", "false"));
    coordinateMode = StringUtils::toBool(getParameter("coordinate-mode", "false"));
    //print to check
#ifdef DEBUG_NEMA
    std::cout << "JunctionID = " << myID << std::endl;
    std::cout << "All parameters after calling constructor are: " << std::endl;
    std::cout << "myDetectorLength = " << myDetectorLength << std::endl;
    std::cout << "cycleLength = " << myCycleLength << std::endl;
    std::cout << "ring1 = " << ring1 << std::endl;
    std::cout << "ring2 = " << ring2 << std::endl;
    std::cout << "barriers = " << barriers << std::endl;
    std::cout << "coordinates = " << coordinates << std::endl;
    std::cout << "offset = " << offset << std::endl;
    std::cout << "whetherOutputState = " << whetherOutputState << std::endl;
    std::cout << "coordinateMode = " << coordinateMode << std::endl;
    std::cout << "You reach the end of constructor" << std::endl;
    std::cout << "****************************************\n";
#endif
}

NEMALogic::~NEMALogic() { }

void
NEMALogic::init(NLDetectorBuilder& nb) {
    //init the base path for output state
    outputStateFilePath = outputStateFilePath + "/" + myID + "_state_output";
    // std::cout << "outputStaetFilePath = " << outputStateFilePath << std::endl;
    //init cycleRefPoint
    cycleRefPoint = 0;

    //init outputStateFile
    if (whetherOutputState) {
        outputStateFile.open(outputStateFilePath);
        outputStateFile << "Output state changes:\n";
        outputStateFile.close();
    }
    //init phaseStartTime and phaseExpectedDuration
    int phaseNumber = 8;
    for (int i = 0; i < phaseNumber; i++) {
        phaseStartTime[i] = 0;
        phaseExpectedDuration[i] = 0;
    }

    //print to check
    //init minGreen, maxGreen, vehExt, red, and yellow
    for (MSPhaseDefinition* phase : myPhases) {
        int NEMAPhase = string2int(phase->getName());
        int i = NEMAPhase - 1;
        // std::string indexFromName = phase->getName();
        // std::stringstream ss(indexFromName);
        // int NEMAPhase = 0;
        // ss << NEMAPhase;
#ifdef DEBUG_NEMA
        std::cout << "NEMAIndex = " << NEMAPhase << ": ";
#endif
        minGreen[i] = STEPS2TIME(phase->minDuration);
        maxGreen[i] = STEPS2TIME(phase->maxDuration);
        nextMaxGreen[i] = maxGreen[i];
        vehExt[i] = STEPS2TIME(phase->vehext);
        yellowTime[i] = STEPS2TIME(phase->yellow);
        redTime[i] = STEPS2TIME(phase->red);
        //map state G index to laneIDs
        std::string state = phase->getState();
        std::set<std::string> laneIDs = getLaneIDsFromNEMAState(state);
        std::vector<std::string> laneIDs_vector;
        for (std::string laneID : laneIDs) {
            laneIDs_vector.push_back(laneID);
        }
        phase2ControllerLanesMap[NEMAPhase] = laneIDs_vector;
#ifdef DEBUG_NEMA
        std::cout << "minGreen = " << minGreen[i] << "; maxGreen = " << maxGreen[i] << "; vehext = " << vehExt[i] << "; yellow = " << yellowTime[i] << "; redTime = " << redTime[i] << std::endl;
#endif
    }

#ifdef DEBUG_NEMA
    //print to check the phase2ControllerLanesMap
    for (auto item : phase2ControllerLanesMap) {
        std::cout << "NEMA phase index = " << item.first << " have lanes: ";
        for (auto id : item.second) {
            std::cout << id << " ";
        }
        std::cout << std::endl;
    }
#endif


    //init rings
    rings.push_back(readParaFromString(ring1));
    rings.push_back(readParaFromString(ring2));


#ifdef DEBUG_NEMA
    //print to check
    for (int i = 0; i < (int)rings.size(); i++) {
        int count = 0;
        std::cout << "Ring" << i + 1 << " includes phases: \t";
        for (auto j : rings[i]) {
            count++;
            std::cout << j << " ";
            if (count == 2 || count == 4) {
                std::cout << " | ";
            }
        }
        std::cout << std::endl;
    }
#endif

    //init barriers
    barrierPhaseIndecies = readParaFromString(barriers);
    coordinatePhaseIndecies = readParaFromString(coordinates);

    //init the active index for rings and barriers
    activeRing1Index = 0;
    activeRing2Index = 0;
    activeRing1Phase = 0;
    activeRing2Phase = 0;

    for (int i = 0; (int)rings[0].size(); i++) {
        if (rings[0][i] != 0) {
            // std::cout << i << std::endl;
            // std::cout << "rings[0][i] = " << rings[0][i] << std::endl;
            activeRing1Index = i;
            activeRing1Phase = rings[0][activeRing1Index];
            break;
        }
    }
    for (int i = 0; (int)rings[1].size(); i++) {
        if (rings[1][i] != 0) {
            // std::cout << i << std::endl;
            // std::cout << "rings[0][i] = " << rings[1][i] << std::endl;
            activeRing2Index = i;
            activeRing2Phase = rings[1][activeRing2Index];
            break;
        }
    }

#ifdef DEBUG_NEMA
    //print to check the rings and barriers active phase
    std::cout << "After init, active ring1 phase is " << activeRing1Phase << std::endl;
    std::cout << "After init, active ring2 phase is " << activeRing2Phase << std::endl;


    //print to check the phase definition is correct
    std::cout << "Print to check NEMA phase definitions\n";
    for (auto p : myPhases) {
        std::cout << "index = " << p->getName() << "; ";
        std::cout << "duration (useless) = " << time2string(p->duration) << "; ";
        std::cout << "minDur = " << time2string(p->minDuration) << "; ";
        std::cout << "maxDur = " << time2string(p->maxDuration) << "; ";
        std::cout << "vehext = " << time2string(p->vehext) << "; ";
        std::cout << "yellow = " << time2string(p->yellow) << "; ";
        std::cout << "red = " << time2string(p->red) << "; ";
        std::cout << "state = " << p->getState() << std::endl;
    }
#endif

    //init the traffic light
    MSTrafficLightLogic::init(nb);
    assert(myLanes.size() > 0);
    //iterate through the lanes and build one E2 detector for each lane associated with the traffic light control junction
    for (const LaneVector& lanes : myLanes) {
        for (MSLane* const lane : lanes) {
            //decide the detector length
            double detector_length = 0;
            if (isLeftTurnLane(lane)) {
                detector_length = myDetectorLengthLeftTurnLane;
            } else {
                detector_length = myDetectorLength;
            }
            if (noVehicles(lane->getPermissions())) {
                // do not build detectors on green verges or sidewalks
                continue;
            }
            // Build detector and register them in the detector control
            if (myLaneDetectorMap.find(lane) == myLaneDetectorMap.end()) {
                MSE2Collector* det = nullptr;
                const std::string customID = getParameter(lane->getID());
                if (customID != "") {
                    det = dynamic_cast<MSE2Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).get(customID));
                    if (det == nullptr) {
                        WRITE_ERROR("Unknown laneAreaDetector '" + customID + "' given as custom detector for NEMA tlLogic '" + getID() + "', program '" + getProgramID() + ".");
                        continue;
                    }
                    //set the detector to be visible in gui
                    det->setVisible(myShowDetectors);
                } else {
                    std::string id = "TLS_" + myID + "_" + myProgramID + "_E2DetectorOnLane_" + lane->getID();
                    // std::cout << "The detectorID = " << id << std::endl;
                    //createE2Detector() method will lead to bad detector showing in sumo-gui
                    //so it is better to use build2Detector() rather than createE2Detector()
                    // det = nb.createE2Detector(id, DU_TL_CONTROL, lane, INVALID_POSITION, lane->getLength(), myDetectorLength, 0, 0, 0, myVehicleTypes, myShowDetectors);
                    // MSNet::getInstance()->getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, det, myFile, myFreq);
                    nb.buildE2Detector(id, //detectorID
                                       lane, //lane to build this detector
                                       INVALID_POSITION, // set the detector location by end point and length, so this one is set to invalue value so this parameter can be passed
                                       lane->getLength(), // set the end position of the detector at the end of the lane, which is right at the position of stop bar of a junction
                                       detector_length, //detector length
                                       myFile, // detector information output file
                                       myFreq, // detector reading interval
                                       0, // time-based threshold that decribes how much time has to pass until a vehicle is considerred as halting
                                       0, // speed threshold as halting
                                       0, // minimum dist to the next standing vehicle to make this vehicle count as a participant to the jam
                                       myVehicleTypes, //vehicle types to consider, if it is empty, meaning consider all types of vehicles
                                       false, // detector position check. More details could be found on SUMO web
                                       myShowDetectors, // whether to show detectors in sumo-gui
                                       0, //traffic light that triggers aggregation when swithing
                                       0); // outgoing lane that associated with the traffic light

                    //get the detector to be used in the lane detector map loading
                    det = dynamic_cast<MSE2Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).get(id));
                }
                // print to check
                // std::cout << "E2Detector " << det->getID() << " is built on laneID = " << lane->getID() << std::endl;

                //map the detector to lane and lane to detector
                myLaneDetectorMap[lane] = det;
                myDetectorLaneMap[det] = lane;
                myDetectorInfoVector.push_back(DetectorInfo(det, (int)myPhases.size()));

            }
        }
    }
    //map NEMA phase to detectors
    // std::cout << "start of NEMA phase to detector map building " << std::endl;
    for (auto item : phase2ControllerLanesMap) {
        int NEMAPhaseIndex = item.first;
        std::vector<std::string> laneIDs = item.second;
        std::vector<MSE2Collector*> detectors;
        MSE2Collector* detector = nullptr;
        for (std::string laneID : laneIDs) {
            MSLane* lane = MSLane::dictionary(laneID);
            detector = myLaneDetectorMap[lane];
            detectors.push_back(detector);
        }
        phase2DetectorMap[NEMAPhaseIndex] = detectors;
    }
#ifdef DEBUG_NEMA
    // print to check phase2DetectorMap
    std::cout << "Print to check phase2DetectorMap" << std::endl;
    for (auto item : phase2DetectorMap) {
        std::cout << "The NEMA phase index = " << item.first << " has detectors: \n";
        for (auto det : item.second) {
            std::cout << '\t' << det->getID() << std::endl;
        }
    }
#endif

    R1State = activeRing1Phase;
    R2State = activeRing2Phase;
    // std::cout << "After init, R1State = " << R1State << std::endl;
    // std::cout << "After init, R2State = " << R2State << std::endl;

    R1RYG = 1;
    R2RYG = 1;

    wait4R1Green = false;
    wait4R2Green = false;

    r1barrier = barrierPhaseIndecies[0];
    r2barrier = barrierPhaseIndecies[1];

    r1coordinatePhase = coordinatePhaseIndecies[0];
    r2coordinatePhase = coordinatePhaseIndecies[1];

#ifdef DEBUG_NEMA
    std::cout << "After init, r1/r2 barrier phase = " << r1barrier << " and " << r2barrier << std::endl;
    std::cout << "After init, r1/r2 coordinate phase = " << r1coordinatePhase << " and " << r2coordinatePhase << std::endl;
#endif

    currentState = "";
    // currentR1State = myPhases[R1State - 1]->getState();
    // currentR2State = myPhases[R2State - 1]->getState();
    for (const MSPhaseDefinition* const p : myPhases) {
        if (R1State == string2int(p->getName())) {
            currentR1State = p->getState();
        }
        if (R2State == string2int(p->getName())) {
            currentR2State = p->getState();
        }
    }
#ifdef DEBUG_NEMA
    std::cout << "R1State = " << R1State << " and its state = " << currentR1State << std::endl;
    std::cout << "R2State = " << R2State << " and its state = " << currentR2State << std::endl;
#endif

    //Do not delete. SUMO traffic logic check.
    //SUMO check begin
    const SVCPermissions motorized = ~(SVC_PEDESTRIAN | SVC_BICYCLE);
    std::map<int, std::set<MSE2Collector*>> linkToDetectors;
    std::set<int> actuatedLinks;

    const int numLinks = (int)myLinks.size();
    std::vector<bool> neverMajor(numLinks, true);
    for (const MSPhaseDefinition* phase : myPhases) {
        const std::string& state = phase->getState();
        for (int i = 0; i < numLinks; i++) {
            if (state[i] == LINKSTATE_TL_GREEN_MAJOR) {
                neverMajor[i] = false;
            }
        }
    }
    std::vector<bool> oneLane(numLinks, false);
    for (int i = 0; i < numLinks; i++) {
        for (MSLane* lane : getLanesAt(i)) {
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
    }

    for (const MSPhaseDefinition* phase : myPhases) {
        const int phaseIndex = (int)myDetectorForPhase.size();
        std::set<MSE2Collector*> detectors;
        if (phase->isActuted()) {
            const std::string& state = phase->getState();
            std::set<int> greenLinks;
            std::map<MSE2Collector*, std::set<int>> detectorLinks;

            for (int i = 0; i < numLinks; i++)  {
                if (state[i] == LINKSTATE_TL_GREEN_MAJOR
                        || (state[i] == LINKSTATE_TL_GREEN_MINOR
                            && ((neverMajor[i]  // check1a
                                 && hasMajor(state, getLanesAt(i))) // check1b
                                || oneLane[i])) // check1c
                   ) {
                    greenLinks.insert(i);
                    actuatedLinks.insert(i);
                }

                for (MSLane* lane : getLanesAt(i)) {
                    if (myLaneDetectorMap.count(lane) != 0) {
                        detectorLinks[myLaneDetectorMap[lane]].insert(i);
                    }
                }
            }
            for (auto& item : detectorLinks) {
                MSE2Collector* det = item.first;
                MSLane* detectorLane = myDetectorLaneMap[det];
                bool usable = true;
                // check 1
                for (int j : item.second) {
                    if (greenLinks.count(j) == 0) {
                        usable = false;
                    }
                }

                //check 2
                if (usable) {
                    for (MSLink* link : detectorLane->getLinkCont()) {
                        MSLane* next = link->getLane();
                        if (myLaneDetectorMap.count(next) != 0) {
                            MSE2Collector* nextDet = myLaneDetectorMap[next];
                            for (int j : detectorLinks[nextDet]) {
                                if (greenLinks.count(j) == 0) {
                                    usable = false;
                                    break;
                                }
                            }
                        }
                    }
                }

                if (usable) {
                    detectors.insert(item.first);
                    for (int j : item.second) {
                        linkToDetectors[j].insert(item.first);
                    }
                }
            }
            if (detectors.size() == 0) {
                WRITE_WARNINGF("At NEMA tlLogic '%', actuated phase % has no controlling detector", getID(), toString(phaseIndex));
            }
        }
        std::vector<DetectorInfo*> detectorInfos;
        myDetectorForPhase.push_back(detectorInfos);
        for (MSE2Collector* det : detectors) {
            for (DetectorInfo& detInfo : myDetectorInfoVector) {
                if (detInfo.det == det) {
                    myDetectorForPhase.back().push_back(&detInfo);
                    detInfo.servedPhase[phaseIndex] = true;
                }
            }
        }
    }

    for (int i : actuatedLinks) {
        if (linkToDetectors[i].size() == 0 && myLinks[i].size() > 0
                && (myLinks[i].front()->getLaneBefore()->getPermissions() & motorized) != 0) {
            WRITE_WARNINGF("At NEMA tlLogic '%, linkIndex % has no controlling detector", getID(), toString(i));
        }
    }
    myPhase.setName(toString(activeRing1Phase) + "+" + toString(activeRing2Phase));

#ifdef DEBUG_NEMA
    //std::cout << "reach the end of init()\n";
#endif
}


void 
NEMALogic::setNewSplits(std::vector<double> newSplits) {
    assert(newSplits.size() == 8);
    for (int i = 0; i < 8; i++) {
        nextMaxGreen[i] = newSplits[i] - yellowTime[i] - redTime[i];
    }
}


void 
NEMALogic::setNewMaxGreens(std::vector<double> newMaxGreens) {
    for (int i = 0; i < 8; i++) {
        nextMaxGreen[i] = newMaxGreens[i];
    }
}


void 
NEMALogic::setNewCycleLength(double newCycleLength) {
    myNextCycleLength = newCycleLength;
}


void
NEMALogic::setNewOffset(double newOffset) {
    myNextOffset = newOffset;
}

//helper methods

std::vector<int> NEMALogic::readParaFromString(std::string s) {
    std::vector<int> output;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            int temp = c - '0';
            output.push_back(temp);
        }
    }
    return output;
}

std::vector<std::string> NEMALogic::string2vector(std::string s) {
    std::vector<std::string> output;
    std::stringstream ss(s);
    while (ss.good()) {
        std::string substr;
        std::getline(ss, substr, ',');
        output.push_back(substr);
    }
#ifdef DEBUG_NEMA
    //print to check
    for (auto i : output) {
        std::cout << i << std::endl;
    }
#endif
    return output;
}

std::string NEMALogic::combineStates(std::string state1, std::string state2) {
    std::string output = "";
    if (state1.size() != state2.size()) {
        throw ProcessError("Different sizes of NEMA phase states. Please check the NEMA XML");
    }
    for (int i = 0; i < (int)state1.size(); i++) {
        char ch1 = state1[i];
        char ch2 = state2[i];

        // check through this order. 'G' overwrite 'g'.
        if (ch1 == 'G' || ch2 == 'G') {
            output += 'G';
        } else if (ch1 == 'g' || ch2 == 'g') {
            output += 'g';
        } else if (ch1 == 's' || ch2 == 's') {
            output += 's';
        } else if (ch1 == 'y' || ch2 == 'y') {
            output += 'y';
        } else if (ch1 == 'u' || ch2 == 'u') {
            output += 'u';
        } else if (ch1 == 'O' || ch2 == 'O') {
            output += 'O';
        } else if (ch1 == 'o' || ch2 == 'o') {
            output += 'o';
        } else {
            output += 'r';
        }
    }
    return output;
}

bool NEMALogic::isDetectorActivated(int phaseIndex) {

    for (auto det : phase2DetectorMap.find(phaseIndex)->second) {
        if (det->getCurrentVehicleNumber() > 0) {
            return true;
        }
    }

    return false;
}

const MSPhaseDefinition&
NEMALogic::getCurrentPhaseDef() const {
    return myPhase;
}

SUMOTime
NEMALogic::trySwitch() {
    const std::string newState = NEMA_control();
    if (newState != myPhase.getState()) {
        myPhase.setState(newState);
        // ensure that SwitchCommand::execute notices a change
        myStep = 1 - myStep;
    }
    //std::cout << SIMTIME << " " << myPhase.getState() << "\n";
    return TIME2STEPS(1);
}


std::string
NEMALogic::NEMA_control() {
    std::string outputState = "";
    //controller starts
    SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    double currentTimeInSecond = STEPS2TIME(now);
    //int R1Phase = activeRing1Phase;
    int R1Phase = R1State;
    int R1Index = R1Phase - 1;

    double durationR1 = currentTimeInSecond - phaseStartTime[R1Index];
    //ensure minGreen for each phase
    phaseExpectedDuration[R1Index] = MAX2(phaseExpectedDuration[R1Index], minGreen[R1Index]);
    if (R1Phase != r1coordinatePhase) {
        if (isDetectorActivated(R1Phase)) {
            phaseExpectedDuration[R1Index] = MAX2(phaseExpectedDuration[R1Index], durationR1 + vehExt[R1Index]);
            phaseExpectedDuration[R1Index] = MIN2(phaseExpectedDuration[R1Index], maxGreen[R1Index]);
        }
    }

    int R2Phase = R2State;
    int R2Index = R2Phase - 1;
    double durationR2 = currentTimeInSecond - phaseStartTime[R2Index];
    phaseExpectedDuration[R2Index] = MAX2(phaseExpectedDuration[R2Index], minGreen[R2Index]);
    if (R2Phase != r2coordinatePhase && R2Phase >= 5) {
        if (isDetectorActivated(R2Phase)) {
            phaseExpectedDuration[R2Index] = MAX2(phaseExpectedDuration[R2Index], durationR2 + vehExt[R2Index]);
            phaseExpectedDuration[R2Index] = MIN2(phaseExpectedDuration[R2Index], maxGreen[R2Index]);
        }
    }

    bool EndCurrentPhaseR1 = false;
    bool EndCurrentPhaseR2 = false;
    if (durationR1 >= phaseExpectedDuration[R1Index]) {
        EndCurrentPhaseR1 = true;
    }
    if (durationR2 >= phaseExpectedDuration[R2Index]) {
        EndCurrentPhaseR2 = true;
    }
    if (EndCurrentPhaseR1 && (R1Phase == r1barrier)) {
        if (!(EndCurrentPhaseR2 && R2Phase == r2barrier)) {
            //update expectedDuration
            EndCurrentPhaseR1 = false;
        }
    }
    if (EndCurrentPhaseR1 && (R1Phase == r1coordinatePhase)) {
        if (!EndCurrentPhaseR2 || R2Phase != r2coordinatePhase) {
            EndCurrentPhaseR1 = false;
        }
    }
    if (EndCurrentPhaseR2 && (R2Phase == r2barrier)) {
        if (!EndCurrentPhaseR1 || R1Phase != r1barrier) {
            EndCurrentPhaseR2 = false;
        }
    }
    if (EndCurrentPhaseR2 && (R2Phase == r2coordinatePhase)) {
        if (!EndCurrentPhaseR1 || R1Phase != r1coordinatePhase) {
            EndCurrentPhaseR2 = false;
        }
    }
    if (EndCurrentPhaseR1 && (!wait4R1Green)) {
        phaseEndTimeR1 = currentTimeInSecond;
        phaseExpectedDuration[R1Index] = 0;
        wait4R1Green = true;
    }
    //enter transtion phase for Ring1
    if (wait4R1Green) {
        if (currentTimeInSecond - phaseEndTimeR1 < yellowTime[R1Index]) {
            R1RYG = 0; //yellow
        } else if (currentTimeInSecond - phaseEndTimeR1 < (yellowTime[R1Index] + redTime[R1Index])) {
            R1RYG = -1; //red
            bool toUpdate = (currentTimeInSecond - phaseEndTimeR1) < yellowTime[R1Index] + 0.5;
            if (R1Phase == r1coordinatePhase && toUpdate) {
                for (int i = 0; i < 8; i++) {
                    maxGreen[i] = nextMaxGreen[i];
                }
                offset = myNextOffset;
                myCycleLength = myNextCycleLength;
            }
        } else {
            //next phase
            //time 10 R1Phase = 4. Checked
            R1Phase = nextPhase(rings[0], R1Phase);
            //offset control not included for now
            R1RYG = 1; //green
            //update phaseStartTime
            phaseStartTime[R1Phase - 1] = currentTimeInSecond;

            R1State = R1Phase;
            if (R1Phase == r1coordinatePhase) {
                if (coordinateMode) {
                    phaseExpectedDuration[R1Phase - 1] = ModeCycle(myCycleLength - (currentTimeInSecond - cycleRefPoint - offset) - yellowTime[R1Phase - 1] - redTime[R1Phase - 1], myCycleLength);
                }
            }
            wait4R1Green = false;
        }
    }
    //Enter transition phase in Ring2
    if (EndCurrentPhaseR2 && (!wait4R2Green)) {
        phaseEndTimeR2 = currentTimeInSecond;
        phaseExpectedDuration[R2Index] = 0;
        wait4R2Green = true;
    }
    if (wait4R2Green) {
        if ((currentTimeInSecond - phaseEndTimeR2) < yellowTime[R2Index]) {
            R2RYG = 0;
        } else if ((currentTimeInSecond - phaseEndTimeR2) < (yellowTime[R2Index] + redTime[R2Index])) {
            R2RYG = -1;
        } else {
            R2Phase = nextPhase(rings[1], R2Phase);
            R2RYG = 1;
            //update phaseStartTime
            phaseStartTime[R2Phase - 1] = currentTimeInSecond;
            R2State = R2Phase;
            if (R2Phase == r2coordinatePhase) {
                if (coordinateMode) {
                    phaseExpectedDuration[R2Phase - 1] = ModeCycle(myCycleLength - (currentTimeInSecond - cycleRefPoint - offset) - yellowTime[R2Phase - 1] - redTime[R2Phase - 1], myCycleLength);
                }

            }
            wait4R2Green = false;
        }
    }


    std::string state1 = "";
    for (auto p : myPhases) {
        if (R1State == string2int(p->getName())) {
            state1 = p->getState();
        }
    }
    state1 = transitionState(state1, R1RYG);
    currentR1State = state1;

    std::string state2 = "";
    for (auto p : myPhases) {
        if (R2State == string2int(p->getName())) {
            state2 = p->getState();
        }
    }
    state2 = transitionState(state2, R2RYG);
    currentR2State = state2;

    outputState = combineStates(state1, state2);

    if (currentState != outputState) {
        currentState = outputState;
        if (whetherOutputState) {
            outputStateFile.open(outputStateFilePath, std::ios_base::app);
            outputStateFile << currentTimeInSecond << "\t" << currentState << std::endl;
            outputStateFile.close();
        }

    }
    myPhase.setName(toString(R1Phase) + "+" + toString(R2Phase));
    return outputState;
}

int NEMALogic::nextPhase(std::vector<int> ring, int currentPhase) {

    int length = (int)ring.size();
    int flag = 0;
    int nphase = 0; // next phase
    for (int i = 0; i < length * 2; i++) {
        if (flag == 1) {
            if (ring[i % length] != 0) {
                nphase = ring[i % length];
                break;
            }
        }
        if (ring[i % length] == currentPhase) {
            flag = 1;
        }
    }
    return nphase;

}

//b should be the base of mode
double NEMALogic::ModeCycle(double a, double b) {
    double c = a - b;
    while (c > b) {
        c = c - b;
    }
    while (c < 0) { //should be minimum green (or may be  not)
        c += b;
    }
    return c;
}

std::string NEMALogic::transitionState(std::string curState, int RYG) {
    std::string newState = "";
    if (RYG == 1) {
        //Green
        newState = curState;

    } else if (RYG == -1) {
        // red
        for (char ch : curState) {
            UNUSED_PARAMETER(ch);
            newState += 'r';
        }
    } else {
        // yellow
        for (char ch : curState) {
            if (ch == 'G' || ch == 'g') {
                newState += 'y';
            } else {
                newState += ch;
            }
        }
    }
    return newState;

}


std::set<std::string> NEMALogic::getLaneIDsFromNEMAState(std::string state) {
    std::set<std::string> output;
    const MSTrafficLightLogic::LinkVectorVector& linkV = MSNet::getInstance()->getTLSControl().get(myID).getActive()->getLinks();
    for (int i = 0; i < (int)state.size(); i++) {
        char ch = state[i];
        if (ch == 'G') {
            for (auto link : linkV[i]) {
                output.insert(link->getLaneBefore()->getID());
            }
        }
    }
    return output;
}

bool NEMALogic::isLeftTurnLane(const MSLane* const lane) const {
    const std::vector<MSLink*> links = lane->getLinkCont();
    if (links.size() == 1 && links.front()->getDirection() == LinkDirection::LEFT) {
        return true;;
    }
    return false;
}

bool
NEMALogic::hasMajor(const std::string& state, const LaneVector& lanes) const {
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


void
NEMALogic::activateProgram() {
    MSTrafficLightLogic::activateProgram();
    for (auto& item : myLaneDetectorMap) {
        item.second->setVisible(true);
    }
}

void
NEMALogic::deactivateProgram() {
    MSTrafficLightLogic::deactivateProgram();
    for (auto& item : myLaneDetectorMap) {
        item.second->setVisible(false);
    }
}

void
NEMALogic::setShowDetectors(bool show) {
    myShowDetectors = show;
    for (auto& item : myLaneDetectorMap) {
        item.second->setVisible(myShowDetectors);
    }
}

int NEMALogic::string2int(std::string s) {
    std::stringstream ss(s);
    int ret = 0;
    ss >> ret;
    return ret;
}

void
NEMALogic::setParameter(const std::string& key, const std::string& value) {
    if (StringUtils::startsWith(key, "NEMA.")) {
        if (key == "NEMA.splits" || key == "NEMA.maxGreens") {
            //splits="2.0 3.0 4.0 5.0 2.0 3.0 4.0 5.0"
            const std::vector<std::string>& tmp = StringTokenizer(value).getVector();
            if (tmp.size() != 8) {
                throw InvalidArgument("Parameter '" + key + "' for NEMA controller '" + getID() + "' requires 8 space-separated values");
            }
            std::vector<double> timing;
            for (const std::string& s : tmp) {
                timing.push_back(StringUtils::toDouble(s));
            }
            if (key == "NEMA.maxGreens") {
                setNewMaxGreens(timing);
            } else {
                setNewSplits(timing);
            }
        } else if (key == "NEMA.cycleLength") {
            setNewCycleLength(StringUtils::toDouble(value));
        } else if (key == "NEMA.offset") {
            setNewOffset(StringUtils::toDouble(value));
        } else {
            throw InvalidArgument("Unsupported parameter '" + key + "' for NEMA controller '" + getID() + "'");
        }
    }
    Parameterised::setParameter(key, value);
}
