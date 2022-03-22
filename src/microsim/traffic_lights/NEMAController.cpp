/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
#include <sstream>
#include <iostream>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/output/MSInductLoop.h>
#include <netload/NLDetectorBuilder.h>
#include "NEMAController.h"


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
                     int step, SUMOTime delay,
                     const std::map<std::string, std::string>& parameter,
                     const std::string& basePath) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, _offset, TrafficLightType::NEMA, phases, step, delay, parameter),
    myPhase(phases[0]->duration, phases[0]->getState()) {
    myDetectorLength = StringUtils::toDouble(getParameter("detector-length", "20"));
    myDetectorLengthLeftTurnLane = StringUtils::toDouble(getParameter("detector-length-leftTurnLane", "20"));
    myCycleLength = (StringUtils::toDouble(getParameter("total-cycle-length", getParameter("cycle-length", getParameter(toString(SUMO_ATTR_CYCLETIME), "60")))));
    myNextCycleLength = myCycleLength;
    myDefaultCycleTime = TIME2STEPS(myCycleLength);
    myShowDetectors = StringUtils::toBool(getParameter("show-detectors", toString(OptionsCont::getOptions().getBool("tls.actuated.show-detectors"))));
    myFile = FileHelpers::checkForRelativity(getParameter("file", "NUL"), basePath);
    myFreq = TIME2STEPS(StringUtils::toDouble(getParameter("freq", "300")));
    myVehicleTypes = getParameter("vTypes", "");
    ring1 = getParameter("ring1", "");
    ring2 = getParameter("ring2", "");
    myCabinetType = parseCabinetType(getParameter("cabinetType", "Type170"));
    ignoreErrors = StringUtils::toBool(getParameter("ignore-errors", "false"));
    // TODO: Create a parameter for this
    cycleRefPoint = 0;
    
    std::vector<int> VecMinRecall = readParaFromString(getParameter("minRecall", "1,2,3,4,5,6,7,8"));
    for (int i = 0; i < (int)VecMinRecall.size(); i++) {
        minRecalls[VecMinRecall[i] - 1] = true;
        recall[VecMinRecall[i] - 1] = true;
    }

    std::vector<int> VecMaxRecall = readParaFromString(getParameter("maxRecall", ""));
    for (int i = 0; i < (int)VecMaxRecall.size(); i++) {
        maxRecalls[VecMaxRecall[i] - 1] = true;
        recall[VecMaxRecall[i] - 1] = true;
    }

#ifdef DEBUG_NEMA
    std::cout << "minRecall: ";
    for (int i = 0; i < 8; i++) {
        std::cout << minRecalls[i] << '\t';
    }
    std::cout << std::endl;

    std::cout << "maxRecall: ";
    for (int i = 0; i < 8; i++) {
        std::cout << maxRecalls[i] << '\t';
    }
    std::cout << std::endl;
#endif
    barriers = getParameter("barrierPhases", "");
    coordinates = getParameter("coordinatePhases", getParameter("barrier2Phases", ""));
    fixForceOff = StringUtils::toBool(getParameter("fixForceOff", "false"));
    offset = STEPS2TIME(_offset);
    myNextOffset = offset;
    whetherOutputState = StringUtils::toBool(getParameter("whetherOutputState", "false"));
    coordinateMode = StringUtils::toBool(getParameter("coordinate-mode", "false"));
    greenTransfer = StringUtils::toBool(getParameter("greenTransfer", "true"));

    //missing parameter error
    error_handle_not_set(ring1, "ring1");
    error_handle_not_set(ring2, "ring2");
    error_handle_not_set(barriers, "barrierPhases");
    error_handle_not_set(coordinates, "barrier2Phases or coordinatePhases");

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
    std::cout << "myShowDetectors = " << myShowDetectors << std::endl;
    std::cout << "coordinateMode = " << coordinateMode << std::endl;
    std::cout << "fixForceOff = " << fixForceOff << std::endl;
    std::cout << "greenTransfer = " << greenTransfer << std::endl;
    std::cout << "You reach the end of constructor" << std::endl;
    std::cout << "****************************************\n";
#endif

    // Create vector of latching detectors
    IntVector latchingDetectors = readParaFromString(getParameter("latchingDetectors", ""));

    // construct the phaseDetectorMapping. In the future this could hold more parameters, such as lock in time or delay
    for (int i = 0; i < 2; i++){
        auto local_ring = i < 1? ring1: ring2;
        for (auto p: readParaFromString(local_ring)){
            if (p > 0){
                bool latching = false;
                std::string cps = "crossPhaseSwitching:";
                int crossPhase = StringUtils::toInt(getParameter(cps.append(std::to_string(p)), "0"));
                if (std::find(latchingDetectors.begin(), latchingDetectors.end(), p) != latchingDetectors.end()) {
                    latching = true;
                }
                phase2DetectorMap[p] = phaseDetectorInfo(crossPhase, latching);
            }
        }
    }

    // Construct the Cross Mapping
    for (auto &phaseDetectInfo : phase2DetectorMap) {
        if (phaseDetectInfo.second.cpdSource > 0) {
            // WRITE_WARNING(error);
            // TODO: Handle 
            if (phase2DetectorMap.find(phaseDetectInfo.second.cpdSource) != phase2DetectorMap.end()){    
                phase2DetectorMap.find(phaseDetectInfo.second.cpdSource) -> second.cpdTarget = phaseDetectInfo.first;
            } else {
                phaseDetectInfo.second.cpdSource = 0;
                std::string msg = "At NEMA tlLogic '" + getID() + "', the cross phase switching for phase " + toString(phaseDetectInfo.first) 
                                    + " is not enabled because phase " + toString(phaseDetectInfo.second.cpdSource) + " does not exist"; 
                if (!ignoreErrors){
                    throw ProcessError(msg);
                } else {
                    WRITE_WARNING(msg)
                }
            }
        }
    }

    // Construct the NEMA specific timing data types and initial phases
    constructTimingAndPhaseDefs();

}

NEMALogic::~NEMALogic() { }

void
NEMALogic::constructTimingAndPhaseDefs(){
    //init phaseStartTime and phaseExpectedDuration
    int phaseNumber = 8;
    for (int i = 0; i < phaseNumber; i++) {
        phaseStartTime[i] = 0;
        phaseExpectedDuration[i] = 0;
    }

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
        maxGreenMaster[i] = maxGreen[i];
        vehExt[i] = STEPS2TIME(phase->vehext);
        yellowTime[i] = STEPS2TIME(phase->yellow);
        redTime[i] = STEPS2TIME(phase->red);
        //map state G index to laneIDs
        std::string state = phase->getState();
        std::set<std::string> laneIDs = getLaneIDsFromNEMAState(state);
        std::vector<std::string> laneIDs_vector;
        for (std::string laneID : laneIDs) {
            laneIDs_vector.push_back(laneID);
            myLanePhaseMap[laneID] = NEMAPhase;
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
    
    // unpack barriers
    r1barrier = barrierPhaseIndecies[0];
    r2barrier = barrierPhaseIndecies[1];
    r1coordinatePhase = coordinatePhaseIndecies[0];
    r2coordinatePhase = coordinatePhaseIndecies[1];

    //init the active index for rings and barriers
    activeRing1Index = 0;
    activeRing2Index = 0;
    activeRing1Phase = 0;
    activeRing2Phase = 0;

    // Create the barrier to phase mapping
    constructBarrierMap(0, myRingBarrierMapping[0]);
    constructBarrierMap(1, myRingBarrierMapping[1]);


    // Set the active ring1 and ring2 phases to be on the right side of the barrier
    activeRing1Phase = myRingBarrierMapping[0][0].back() != 0? myRingBarrierMapping[0][0].back() : myRingBarrierMapping[0][0].front() ;
    activeRing1Index = activeRing1Phase - 1;

    activeRing2Phase = myRingBarrierMapping[1][0].back() != 0? myRingBarrierMapping[1][0].back() : myRingBarrierMapping[1][0].front() ;
    activeRing2Index = activeRing2Phase - 1;
    
    
    // Calculate Force offs Based on Timing
    calculateForceOffs();

    if (coordinateMode){
        // Calculate the Initial Phases in coordinated operation only.
        // Otherwise they have already been calculated above
        calculateInitialPhases();
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

    R1State = activeRing1Phase;
    R2State = activeRing2Phase;

    // set the next phase to current for initialization
    myNextPhaseR1 = R1State;
    myNextPhaseR2 = R2State;
    myNextPhaseR1Distance = 0;
    myNextPhaseR2Distance = 0;

    // std::cout << "After init, R1State = " << R1State << std::endl;
    // std::cout << "After init, R2State = " << R2State << std::endl;

    R1RYG = GREEN;
    R2RYG = GREEN;
    
    wait4R1Green = false;
    wait4R2Green = false;


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
        // Initial Phases
    std::string state1 = transitionState(currentR1State, GREEN);
    std::string state2 = transitionState(currentR2State, GREEN);
    myPhase.setState(combineStates(state1, state2));
    myPhase.setName(toString(activeRing1Phase) + "+" + toString(activeRing2Phase));
    myNextPhaseR1 = 0;
    myNextPhaseR2 = 0;

    // myStep Should Start at 0
    myStep = 0;

    //validating timing
    validate_timing();
}

void
NEMALogic::init(NLDetectorBuilder& nb) {
    //init the base path for output state
    outputStateFilePath = outputStateFilePath + "/" + myID + "_state_output";
    // std::cout << "outputStaetFilePath = " << outputStateFilePath << std::endl;
    //init cycleRefPoint

    //init outputStateFile
    if (whetherOutputState) {
        outputStateFile.open(outputStateFilePath);
        outputStateFile << "Output state changes:\n";
        outputStateFile.close();
    }



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
                    int phaseNumber = 0;
                    if (myLanePhaseMap.find(lane->getID()) != myLanePhaseMap.end()){    
                         phaseNumber = myLanePhaseMap.find(lane->getID()) -> second;
                    }
                    std::string id = myID + "_" + myProgramID + "_D" + toString(phaseNumber) + "." + toString(lane->getIndex());
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
        phase2DetectorMap.find(NEMAPhaseIndex) -> second.detectors = detectors;
    }
#ifdef DEBUG_NEMA
    // print to check phase2DetectorMap
    std::cout << "Print to check phase2DetectorMap" << std::endl;
    for (auto item : phase2DetectorMap) {
        std::cout << "The NEMA phase index = " << item.first << " has detectors: \n";
        for (auto det : item.second.detectors) {
            std::cout << '\t' << det->getID() << std::endl;
        }
    }
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
        if (phase->isActuated()) {
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

#ifdef DEBUG_NEMA
    //std::cout << "reach the end of init()\n";
#endif
}

void
NEMALogic::validate_timing() {
    //check cycle length
    for (int ringIndex = 0; ringIndex <= 1; ringIndex++){
        // TS2 Force Offs don't go in order, so using a different method to check cycle time
        double cycleLengthCalculated = 0;
        for (int p : rings[ringIndex]){
            if (p > 0){
                cycleLengthCalculated += (maxGreen[p - 1] + yellowTime[p - 1] + redTime[p - 1]);
            }
        }
        if (coordinateMode && cycleLengthCalculated != myCycleLength){
            int ringNumber = ringIndex + 1;
            const std::string error = "At NEMA tlLogic '" + getID() + "', Ring " + toString(ringNumber) + " does not add to cycle length.";
            if (ignoreErrors) {
                WRITE_WARNING(error);
            } else {
                throw  ProcessError(error);
            }
        }
    }
    // check barriers
    double ring1barrier1_length = forceOffs[r1barrier - 1] + yellowTime[r1barrier - 1] + redTime[r1barrier - 1];
    double ring2barrier1_length = forceOffs[r2barrier - 1] + yellowTime[r2barrier - 1] + redTime[r2barrier - 1];
    if (ring1barrier1_length != ring2barrier1_length) {
        const std::string error = "At NEMA tlLogic '" + getID() + "', the phases before barrier 1 from both rings do not add up. (ring1="
                                  + toString(ring1barrier1_length) + ", ring2=" + toString(ring2barrier1_length) + ")";
        if (coordinateMode && !ignoreErrors) {
            throw  ProcessError(error);
        } else {
            WRITE_WARNING(error);
        }
    }
    double ring1barrier2_length = forceOffs[r2coordinatePhase - 1] + yellowTime[r2coordinatePhase - 1] + redTime[r2coordinatePhase - 1];
    double ring2barrier2_length = forceOffs[r1coordinatePhase - 1] + yellowTime[r1coordinatePhase - 1] + redTime[r1coordinatePhase - 1];
    if (ring1barrier2_length != ring2barrier2_length) {
        const std::string error = "At NEMA tlLogic '" + getID() + "', the phases before barrier 2 from both rings do not add up. (ring1="
                                  + toString(ring1barrier2_length) + ", ring2=" + toString(ring2barrier2_length) + ")";
        if (coordinateMode && !ignoreErrors) {
            throw  ProcessError(error);
        } else {
            WRITE_WARNING(error);
        }
    }
    // no offset for non coordinated
    if (!coordinateMode && offset != 0) {
        WRITE_WARNINGF("NEMA tlLogic '%' is not coordinated but an offset was set.", getID());
    }
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
        throw ProcessError("At NEMA tlLogic '" + getID() + "', different sizes of NEMA phase states. Please check the NEMA XML");
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

bool NEMALogic::isDetectorActivated(int phaseNumber, const phaseDetectorInfo &detectInfo, int depth = 0) const{
        // Handle the non-reporting conditions
        // if I am checking for myself, and my target is active while I am not, don't report as active.
        if ((detectInfo.cpdTarget == R1State && R1RYG >= GREEN) 
            || (detectInfo.cpdTarget == R2State && R2RYG >= GREEN)){
            if ((phaseNumber != R1State) && (phaseNumber != R2State) && (depth < 1)){
                return false;
            } 
        }
        // Normal Detector Check
        for (auto det : detectInfo.detectors) {
            if (det->getCurrentVehicleNumber() > 0) {
                return true;
            }
        }
        // If I haven't been reported as false or true yet, check my cross-phase detector, but only if it isn't currently active
        if ((detectInfo.cpdSource > 0 && depth < 1) 
            && ((detectInfo.cpdSource != R1State) && (detectInfo.cpdSource != R2State))
            && ((phaseNumber == R1State && R1RYG >= GREEN) || (phaseNumber == R2State && R2RYG >= GREEN))
            ){
            return isDetectorActivated(detectInfo.cpdSource, phase2DetectorMap.find(detectInfo.cpdSource) -> second, depth + 1);
        }
        // Catch All False Return
        return false;
}


std::map<std::string, double>
NEMALogic::getDetectorStates() const {
    std::map<std::string, double> result;
    for (auto item : myDetectorLaneMap) {
        result[item.first->getID()] = item.first->getCurrentVehicleNumber();
    }
    return result;
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
    return TIME2STEPS(TS);
}


std::string
NEMALogic::NEMA_control() {
    std::string outputState = "";
    //controller starts
    SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    double currentTimeInSecond = STEPS2TIME(now);
    double currentInCycleTime = ModeCycle(currentTimeInSecond - cycleRefPoint - offset, myCycleLength);
    // Check the detectors
    checkDetectors();

    #ifdef DEBUG_NEMA
    //print to check
    //I didn't use getTimeInCycle(). This is because the cycle reference point may change in the future.
    std::cout << "current time in cycle:\t" << currentInCycleTime << "\t" << "phases: " << R1State << '\t' << R2State << std::endl;
#endif
    //int R1Phase = activeRing1Phase;
    int R1Phase = R1State;
    int R1Index = R1Phase - 1;

    double durationR1 = currentTimeInSecond - phaseStartTime[R1Index];
    double phaseStartTimeInCycleR1 = ModeCycle(phaseStartTime[R1Index] - cycleRefPoint - offset, myCycleLength);
    //ensure minGreen for each phase
    if (maxRecalls[R1Index]) {
        phaseExpectedDuration[R1Index] = maxGreen[R1Index];
    } else {
        phaseExpectedDuration[R1Index] = MAX2(phaseExpectedDuration[R1Index], minGreen[R1Index]);
    }
    if (((R1Phase != r1coordinatePhase) || (vehExt[R1Index] > 0 && !coordinateMode)) && (R1RYG == GREEN || R1RYG == GREENREST)) {
        if (readDetector(R1Phase)) {
            phaseExpectedDuration[R1Index] = MAX2(phaseExpectedDuration[R1Index], durationR1 + vehExt[R1Index]);
            if (fixForceOff) {
                phaseExpectedDuration[R1Index] = MIN2(phaseExpectedDuration[R1Index], ModeCycle(forceOffs[R1Index] - phaseStartTimeInCycleR1, myCycleLength));
#ifdef DEBUG_NEMA
                std::cout << "R1 phase " << R1State << " forceOff " << forceOffs[R1Index] << "\tphase start " << phaseStartTimeInCycleR1 << std::endl;
#endif
            } else {
                phaseExpectedDuration[R1Index] = MIN2(phaseExpectedDuration[R1Index], maxGreen[R1Index]);
            }
        }
    }

    int R2Phase = R2State;
    int R2Index = R2Phase - 1;
    double durationR2 = currentTimeInSecond - phaseStartTime[R2Index];
    double phaseStartTimeInCycleR2 = ModeCycle(phaseStartTime[R2Index] - cycleRefPoint - offset, myCycleLength);

    if (maxRecalls[R2Index]) {
        phaseExpectedDuration[R2Index] = maxGreen[R2Index];
    } else {
        phaseExpectedDuration[R2Index] = MAX2(phaseExpectedDuration[R2Index], minGreen[R2Index]);
    }
    if ((((R2Phase != r2coordinatePhase && R2Phase >= 5) || ((R2Phase >= 5 && vehExt[R2Index] > 0) && !coordinateMode)) && (R2RYG == GREEN || R2RYG == GREENREST))) {
        if (readDetector(R2Phase)) {
            phaseExpectedDuration[R2Index] = MAX2(phaseExpectedDuration[R2Index], durationR2 + vehExt[R2Index]);
            if (fixForceOff) {
                phaseExpectedDuration[R2Index] = MIN2(phaseExpectedDuration[R2Index], ModeCycle(forceOffs[R2Index] - phaseStartTimeInCycleR2, myCycleLength));
#ifdef DEBUG_NEMA
                std::cout << "R2 phase " << R1State << " forceOff " << forceOffs[R2Index] << "\tphase start " << phaseStartTimeInCycleR2 << std::endl;
#endif
            } else {
                phaseExpectedDuration[R2Index] = MIN2(phaseExpectedDuration[R2Index], maxGreen[R2Index]);
            }
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
    // Green rest can always transition, even if it is at the barrier
    if (EndCurrentPhaseR1 && (R1Phase == r1barrier)) {
        if ((!EndCurrentPhaseR2  || R2RYG < GREEN) && R1RYG != GREENREST) {
            EndCurrentPhaseR1 = false;
        }
    }
    if (EndCurrentPhaseR1 && (R1Phase == r1coordinatePhase)) {
        if ((!EndCurrentPhaseR2 || R2RYG < GREEN) && R1RYG != GREENREST) {
            EndCurrentPhaseR1 = false;
        }
    }
    if (EndCurrentPhaseR2 && (R2Phase == r2barrier)) {
        if ((!EndCurrentPhaseR1 || R1RYG < GREEN) && R2RYG != GREENREST) {
            EndCurrentPhaseR2 = false;
        }
    }
    if (EndCurrentPhaseR2 && (R2Phase == r2coordinatePhase)) {
        if ((!EndCurrentPhaseR1 || R1RYG < GREEN) && R2RYG != GREENREST) {
            EndCurrentPhaseR2 = false;
        }
    }

    // Falling Edge of Green
    if (EndCurrentPhaseR1 && (!wait4R1Green)) {
        phaseEndTimeR1 = currentTimeInSecond;
        phaseExpectedDuration[R1Index] = 0;
        wait4R1Green = true;
    }
    if (EndCurrentPhaseR2 && (!wait4R2Green)) {
        phaseEndTimeR2 = currentTimeInSecond;
        phaseExpectedDuration[R2Index] = 0;
        wait4R2Green = true;
    }

    // catch the falling edge of green rest
    // Logic for green rest. 
    // The maximum timer only starts when there is a car in another lane even if it is less than the minimum timer.
    // **This could be specific to Econolite Controllers** 
    // TODO: Remove the hardcoded phases
    if ((R1RYG >= GREEN && R1Phase == r1coordinatePhase) && (R2RYG >= GREEN && R2Phase == r2coordinatePhase) && !coordinateMode){
        // will still allow the phase to be extended with vehicle detection
        bool flag = false;
        for (auto &p: phase2DetectorMap){
            if (p.first != R1State && p.first != R2State && p.second.detectActive){
                // The ring with the active detector should be set to green transfer. 
                // The other ring is set to perpetually past the minimum timer.
                if (!wait4R1Green || !wait4R2Green){
                    // I am using the altered max green timer as the latching state here. 
                    // This if statement should only be entered on a rising edge of vehicle arrival 
                    if (!wait4R2Green && maxGreen[R2Index] <= (maxGreenMaster[R2Index]) && R2RYG != GREENTRANSFER){
                    // R2RYG = GREEN;
                        if (durationR2 > minGreen[R2Index]){
                            // If I am past my minimum timer, the behavior becomes acting like I am perpetually one step past my min
                            // BUT! the maximum timer restarts from when there is a detection. So even if I am 20 seconds past my minimum timer, 
                            // The maximum timer only starts to take effect once a detection on another phase
                            phaseStartTime[R2Index] = currentTimeInSecond - minGreen[R2Index];
                            phaseExpectedDuration[R2Index] = minGreen[R2Index] + MAX2(0.0, phaseExpectedDuration[R2Index] - durationR2);
                        }    
                        maxGreen[R2Index] = maxGreenMaster[R2Index] + MIN2(durationR2, minGreen[R2Index]);
                        } 
                    if (!wait4R1Green && maxGreen[R1Index] <= (maxGreenMaster[R1Index]) && R1RYG != GREENTRANSFER){
                    // If the phase is in Green Transfer or Green Rest, the behavior should mimic 
                        if (durationR1 > minGreen[R1Index]){
                            phaseStartTime[R1Index] = currentTimeInSecond - minGreen[R1Index];
                            phaseExpectedDuration[R1Index] = minGreen[R1Index] + MAX2(0.0, phaseExpectedDuration[R1Index] - durationR1);
                        }        
                        maxGreen[R1Index] = maxGreenMaster[R1Index] + MIN2(durationR1, minGreen[R1Index]);
                        }
                    // The opposite phase goes into GREENTRANSFER mode if it is ready to transfer at this point.
                    // This is important because GREENTRANSFER can't be extended with vehicle extension timers
                    R2RYG = wait4R2Green? GREENTRANSFER : R2RYG;
                    R1RYG = wait4R1Green? GREENTRANSFER : R1RYG;
               }
               flag = true;
               break;
            }
        }
        // If a car left the detector on the side street, reset the max green to the default.
        // This resets the timer from above. The state will reset to green rest in the algorithm below.
        if (!flag){
            maxGreen[R1Index] = maxGreen[R1Index] > maxGreenMaster[R1Index]? maxGreenMaster[R1Index]: maxGreen[R1Index];
            maxGreen[R2Index] = maxGreen[R2Index] > maxGreenMaster[R2Index]? maxGreenMaster[R2Index]: maxGreen[R2Index];
            // Green Transfer is cleared and reverted back to green rest here
            R1RYG = R1RYG == GREENTRANSFER? GREENREST : R1RYG;
            R2RYG = R2RYG == GREENTRANSFER? GREENREST : R2RYG;
            if (R1RYG == GREEN && durationR1 > minGreen[R1Index]){
                R1RYG = GREENREST;
                // Carry over the remaining vehicle extension timer
                phaseExpectedDuration[R1Index] = minGreen[R1Index] + MAX2(0.0, phaseExpectedDuration[R1Index] - durationR1);
                phaseStartTime[R1Index] = currentTimeInSecond - minGreen[R1Index];
            }
            if (R2RYG == GREEN && durationR2 > minGreen[R2Index]){
                R2RYG = GREENREST;
                // Carry over the remaining vehicle extension timer
                phaseExpectedDuration[R2Index] = minGreen[R2Index] + MAX2(0.0, phaseExpectedDuration[R2Index] - durationR2);
                phaseStartTime[R2Index] = currentTimeInSecond - minGreen[R2Index];
            } 
        }
    }

    // Reset Green Rest to Green after arriving back at cycle beginning
    if (coordinateMode && (R1RYG == GREENREST) && (R2RYG == GREENREST) && (myCabinetType == TS2)){
        double cycleTime = ModeCycle(currentTimeInSecond - cycleRefPoint - offset, myCycleLength);
        for (auto& p: {R1Phase, R2Phase}){
            if (cycleTime <= (forceOffs[p - 1] - maxGreen[p - 1] + TS / 2)){
                phaseExpectedDuration[p - 1] = coordModeCycle(currentTimeInSecond, p);
                phaseStartTime[p - 1] = currentTimeInSecond;
                if (p == R1Phase){
                    R1RYG = GREEN;
                    wait4R1Green = false;
                    EndCurrentPhaseR1 = false;
                }else{
                    R2RYG = GREEN;
                    wait4R2Green = false;
                    EndCurrentPhaseR2 = false;
                }
            }
        }
    }

    // Logic for Green Rest & Green Transfer
    // This requires a detector check. It should only be entered when the lights are green
    // This logic doesn't need to enter at all if in coordinated mode and greenTransfer is disabled
    if (((EndCurrentPhaseR1 && R1RYG >= GREEN) || (EndCurrentPhaseR2 && R2RYG >= GREEN)) && (!coordinateMode || greenTransfer)) {
        // Calculate the potential next phases.
        // Have to do it here and below because the "final" traffic light check is at the end of yellow
        int tempR1Phase; 
        int tempR2Phase;
        int tempR1Distance = 0;
        int tempR2Distance = 0;
        // Get the next phases, with the first option being staying in the current phase. If the next phase has already been determined, use that as the reference
        std::tie(tempR1Phase, tempR2Phase) = getNextPhases(
            myNextPhaseR1 == 0 ? R1Phase : myNextPhaseR1, 
            myNextPhaseR2 == 0 ? R2Phase : myNextPhaseR2,
            tempR1Distance,
            tempR2Distance,
            myNextPhaseR1 == 0 ? wait4R1Green : false, 
            myNextPhaseR2 == 0 ? wait4R2Green : false, 
            true);
        // entry point to green rest. First check detector status, then determine if this should be up next.
        // Green rest is effectively the same as being perpetually past the minimum green timer but not changing
        // Green Rest exists in Coordinate Mode too. TS2 allows Green Rest
        if ((tempR1Phase == R1Phase && EndCurrentPhaseR1) && (tempR2Phase == R2Phase && EndCurrentPhaseR2) && (!coordinateMode || myCabinetType == TS2)){
            // mark that the phases are not desired to end
            EndCurrentPhaseR1 = false;
            EndCurrentPhaseR2 = false;
            wait4R1Green = false;
            wait4R2Green = false;
            // Timing update. This logic should be checked the next step, so only add the simulation timestep.
            // Potential that this needs to be extended in the future.
            phaseEndTimeR1 += TS;
            phaseEndTimeR2 += TS;
            // setting the phase start time to current time - the minimum timer
            // will still allow the phase to be extended with vehicle detection
            phaseStartTime[R1Index] = currentTimeInSecond - minGreen[R1Index];
            phaseStartTime[R2Index] = currentTimeInSecond - minGreen[R2Index];

            // Set my state to Green Rest
            R1RYG = GREENREST;
            R2RYG = GREENREST;
        } else if (tempR1Phase == R1Phase && EndCurrentPhaseR1 && greenTransfer) {
            // This is the logic for green transfer on Ring 1
            // Green transfer occurs when current phase should end but there isn't a better one to go to,
            // even though the other phase might be transitioning
            if (!EndCurrentPhaseR2 || (tempR2Phase != R2Phase)) {
                EndCurrentPhaseR1 = false;
                wait4R1Green = false;
                phaseEndTimeR1 += TS;
                if ((R1Phase == r1barrier || R1Phase == r1coordinatePhase) && R1RYG != GREENREST) {
                    // If the "green transfer" is at the barrier, it can't actually move until the other phase is done
                    phaseEndTimeR1 = currentTimeInSecond + phaseExpectedDuration[tempR2Phase - 1];
                    phaseExpectedDuration[R1Index] = phaseExpectedDuration[tempR2Phase - 1];
                }
                R1RYG = R1RYG == GREENREST ? GREENREST : GREENTRANSFER;
                // Lock in the next R2 Phase
                if (EndCurrentPhaseR2 && (tempR2Phase != R2Phase) && (myNextPhaseR2 == 0)){
                    myNextPhaseR2 = tempR2Phase;
                    myNextPhaseR2Distance = tempR2Distance;
                } 
            }
        } else if (tempR2Phase == R2Phase && EndCurrentPhaseR2 && greenTransfer) {
            if (!EndCurrentPhaseR1 || (tempR1Phase != R1Phase)) {
                // This is the logic for green transfer on Ring 2
                EndCurrentPhaseR2 = false;
                wait4R2Green = false;
                phaseEndTimeR2 += TS;
                if ((R2Phase == r2barrier || R2Phase == r2coordinatePhase) && R2RYG != GREENREST) {
                    // If the "green transfer" is at the barrier, it can't actually move until the other phase is done
                    phaseEndTimeR2 = currentTimeInSecond + phaseExpectedDuration[tempR1Phase - 1];
                    phaseExpectedDuration[R2Index] = phaseExpectedDuration[tempR1Phase - 1];
                }
                R2RYG = R2RYG == GREENREST ? GREENREST : GREENTRANSFER;
                // Lock in the next R1 Phase
                if (EndCurrentPhaseR1 && (tempR1Phase != R1Phase) && (myNextPhaseR1 == 0)){
                    myNextPhaseR1 = tempR1Phase;
                    myNextPhaseR1Distance = tempR1Distance;
                }
            }
        }
        // The unique cases have been caught by this point. If it makes it through the filter above, let the phase transition 
        else if ((tempR1Phase > 0 && R1RYG >= GREEN && wait4R1Green) || (tempR2Phase > 0 && R2RYG >= GREEN && wait4R2Green)){
            myNextPhaseR1 = tempR1Phase > 0? tempR1Phase : myNextPhaseR1;
            myNextPhaseR1Distance = tempR1Phase > 0? tempR1Distance : myNextPhaseR1Distance;
            myNextPhaseR2 = tempR2Phase > 0? tempR2Phase : myNextPhaseR2;
            myNextPhaseR2Distance = tempR2Phase > 0? tempR2Distance : myNextPhaseR2Distance;
        } 
    }


    // Calculate the next phase with knowledge of both rings
    // Next Phase should be calculated throughout the red phase. 
    bool calculate = false;
    if (wait4R1Green || wait4R2Green) {
        if (wait4R1Green && ((currentTimeInSecond - phaseEndTimeR1) < (yellowTime[R1Index] + redTime[R1Index]))) {
            calculate = true;
        }
        if (wait4R2Green && ((currentTimeInSecond - phaseEndTimeR2) < (yellowTime[R2Index] + redTime[R2Index]))) {
            calculate = true;
        }
        if (calculate) {
            // Try to calculate the next phase again. If the new distance is shorter than the previously calculated next phase, 
            // or if the new phase is further away but the previously calculate phase isn't active, transition to that phase. 
            int tempR1Phase; 
            int tempR2Phase;
            int tempR1Distance = 0;
            int tempR2Distance = 0;
            std::tie(tempR1Phase, tempR2Phase) = getNextPhases(R1Phase, R2Phase, tempR1Distance, tempR2Distance, wait4R1Green, wait4R2Green);
            if (!wait4R1Green){
                if (((tempR2Phase != myNextPhaseR2) || (myNextPhaseR2 == 0)) && (tempR2Phase != R2Phase)){
                    assert(findBarrier(tempR2Phase, 1) == findBarrier(R1Phase, 0));
                    myNextPhaseR2 = tempR2Phase;
                    myNextPhaseR2Distance = tempR2Distance;
                }
            } else if (!wait4R2Green) {
               if (((tempR1Phase != myNextPhaseR1) || (myNextPhaseR1 == 0)) && (tempR1Phase != R1Phase)){
                    assert(findBarrier(tempR1Phase, 0) == findBarrier(R2Phase, 1));
                    myNextPhaseR1 = tempR1Phase;
                    myNextPhaseR1Distance = tempR1Distance;
                }
            }
            // Both phases are waiting a change 
            else {
                // If the next phase hasn't been calculated, or if it has and checking the detectors at the beginning of red yields a different result
                // Then as long as that nextPhase is on the the same side of the barrier (only matters in coordinated mode) as the previously calculated next phase, we can transition to it 
                
                // Only allowed to barrier cross if they ended at the same exact time! 
                bool okToBarrierCross = TIME2STEPS(phaseEndTimeR1) == TIME2STEPS(phaseEndTimeR2);
                int tmpBarrier1 = findBarrier(tempR1Phase, 0);
                int tmpBarrier2 = findBarrier(tempR2Phase, 1);;
                // if ((tempR1Phase != myNextPhaseR1) && (tempR2Phase != myNextPhaseR2)){
                if ((myNextPhaseR1 == 0) && (myNextPhaseR2 == 0)){
                    myNextPhaseR1 = tempR1Phase;
                    myNextPhaseR1Distance = tempR1Distance;
                    myNextPhaseR2 = tempR2Phase;
                    myNextPhaseR2Distance = tempR2Distance;
                } else {
                    if ((tmpBarrier1 != findBarrier(R1Phase, 0)) && (tmpBarrier2 != findBarrier(R2Phase, 1)) && okToBarrierCross){
                        // Handle Ring 1
                        bool tryR1Switch = false;
                        bool tryR2Switch = false;
                        if (myNextPhaseR1 == 0 && (tempR1Phase != R1Phase)){
                            myNextPhaseR1 = tempR1Phase;
                            myNextPhaseR1Distance = tempR1Distance;
                        }
                        else if ((tempR1Phase != R1Phase) && (tempR1Distance == 0 || (tempR1Distance < myNextPhaseR1Distance || (recall[tempR1Phase - 1] || readDetector(tempR1Phase))))){
                            tryR1Switch = true;
                        }
                        if (myNextPhaseR2 == 0 && (tempR2Phase != R2Phase)){
                            myNextPhaseR2 = tempR2Phase;
                            myNextPhaseR2Distance = tempR2Distance;
                        }
                        else if ((tempR2Phase != R2Phase) && (tempR2Distance == 0 || (tempR2Distance < myNextPhaseR2Distance || (recall[tempR2Phase - 1] || readDetector(tempR2Phase))))){
                            tryR2Switch = true;
                        }

                        if (tryR1Switch && !tryR2Switch && tmpBarrier1 == findBarrier(myNextPhaseR2, 1)){
                            myNextPhaseR1 = tempR1Phase;
                            myNextPhaseR1Distance = tempR1Distance;
                        } 
                        else if (!tryR1Switch && tryR2Switch && tmpBarrier2 == findBarrier(myNextPhaseR1, 0)){
                            myNextPhaseR2 = tempR2Phase;
                            myNextPhaseR2Distance = tempR2Distance;
                        } else if (tryR1Switch && tryR2Switch){
                            myNextPhaseR1 = tempR1Phase;
                            myNextPhaseR1Distance = tempR1Distance;
                            myNextPhaseR2 = tempR2Phase;
                            myNextPhaseR2Distance = tempR2Distance;
                        }
                    }
                    // Just the R1 Phase is different 
                    else if ((tempR1Phase != myNextPhaseR1) || (myNextPhaseR1 == 0)){
                        if ((tempR1Phase != R1Phase) && ((tmpBarrier1 == findBarrier(myNextPhaseR1, 0)) || myNextPhaseR2 == 0)){
                            if (tempR1Distance == 0 || (tempR1Distance < myNextPhaseR1Distance || (recall[tempR1Phase - 1] || readDetector(tempR1Phase)))){
                                myNextPhaseR1 = tempR1Phase;
                                myNextPhaseR1Distance = tempR1Distance;
                            } 
                        }
                    }
                    // Just the R2Phase is different  
                    else if ((tempR2Phase != myNextPhaseR2) || (myNextPhaseR2 == 0)) {
                        if ((tempR2Phase != R2Phase) && (tmpBarrier2 == findBarrier(myNextPhaseR2, 1)) || myNextPhaseR2 == 0){
                            if (tempR2Distance == 0 || (tempR2Distance < myNextPhaseR2Distance || (recall[tempR2Phase - 1] || readDetector(tempR2Phase)))){
                                myNextPhaseR1 = tempR1Phase;
                                myNextPhaseR1Distance = tempR1Distance;
                            } 
                        }
                    }
                }


                // if ((myNextPhaseR1 == 0) || ((tempR1Phase != myNextPhaseR1))){
                //     if (!coordinateMode || (myNextPhaseR1 > 0 && (findBarrier(myNextPhaseR1, 0) == findBarrier(tempR1Phase, 0)))){
                //         // This captures a behaviour where the next phase was calculate as 3+8. Then at the start of yellow, the 3 vehicle leaves the detector. 
                //         // If 4 is active, it should go to 4. If 4 is not active, even though getNextPhases will return 4, it should not go to 4 but instead serve 3 first.
                //         if (!coordinateMode || (tempR1Distance == 0 || tempR1Distance < myNextPhaseR1Distance) 
                //         || ((tempR1Distance > myNextPhaseR1Distance) && (recall[tempR1Phase - 1] || readDetector(tempR1Phase)))){
                //             takeNewR1 = true;
                //             // myNextPhaseR1 = tempR1Phase;
                //             // myNextPhaseR1Distance = tempR1Distance;
                //         } 
                //     }
                // } 
                // if ((myNextPhaseR2 == 0) || ((tempR2Phase != myNextPhaseR2))){
                //     if (!coordinateMode || (myNextPhaseR2 > 0 && (findBarrier(myNextPhaseR2, 1) == findBarrier(tempR2Phase, 1)))){
                //         if (!coordinateMode || (tempR2Distance == 0 || tempR2Distance < myNextPhaseR2Distance) 
                //         || ((tempR2Distance > myNextPhaseR2Distance) && (recall[tempR2Phase - 1] || readDetector(tempR2Phase)))){
                //             takeNewR1 = true;
                //             // myNextPhaseR2 = tempR2Phase;
                //             // myNextPhaseR2Distance = tempR2Distance;
                //         } 
                //     }
                // }

                assert(findBarrier(myNextPhaseR2, 1) == findBarrier(myNextPhaseR1, 0));
            }
        }
    }

    //enter transtion phase for Ring1
    if (wait4R1Green) {
        if (currentTimeInSecond - phaseEndTimeR1 < yellowTime[R1Index]) {
            // Debugging assertion. A phase should NEVER be served for shorter than it's minimum time
            assert(durationR1 >= minGreen[R1Index]);
            // Not removing this if statement for clarity on the transition timing
            R1RYG = YELLOW; //yellow
        } else if (currentTimeInSecond - phaseEndTimeR1 < (yellowTime[R1Index] + redTime[R1Index])) {
            R1RYG = RED; //red
            // TODO: remove the 0.5 (it has timing issues with <1 timesteps)
            bool toUpdate = (currentTimeInSecond - phaseEndTimeR1) < (yellowTime[R1Index] + TS / 2) ;
            if (R1Phase == r1coordinatePhase && toUpdate) {
                for (int i = 0; i < 8; i++) {
                    maxGreenMaster[i] = nextMaxGreen[i];
                }
                offset = myNextOffset;
                myCycleLength = myNextCycleLength;
            }
            // Reset the closing phase's maxGreen timer to the Master
            maxGreen[R1Phase - 1] = maxGreenMaster[R1Phase - 1]; 
        } else {
            
            if (myNextPhaseR1 == r1coordinatePhase) {
                if (coordinateMode) {
                    phaseExpectedDuration[myNextPhaseR1 - 1] = coordModeCycle(currentTimeInSecond, myNextPhaseR1);
                    // Assert that the coordinated phase starts at the correct time.
                    if (myCabinetType == TS2){
                        assert(ModeCycle(forceOffs[myNextPhaseR1 - 1] - currentInCycleTime, myCycleLength) >= maxGreen[myNextPhaseR1 - 1]); 
                    } else {
                        if (currentTimeInSecond > (myCycleLength + offset)){
                            // The intial phases cause 
                            assert(phaseExpectedDuration[myNextPhaseR1 - 1] >= maxGreen[myNextPhaseR1 - 1]);
                        }
                    }
               }
            } else {
                if (coordinateMode && myCabinetType == TS2){
                    // TS2 in coordinated mode allows phases to be served as long as they can achieve their minimum time.
                    // We force them to make the split (doesn't affect fix force off) by ending the phase at it's forceOff.
                    if (fixForceOff){
                        // Let it use the remaining time in 
                        maxGreen[myNextPhaseR1 - 1] = ModeCycle(forceOffs[myNextPhaseR1 - 1] - currentInCycleTime, myCycleLength);
                        // Add the last phase's remaining time onto my max time. 
                        maxGreen[myNextPhaseR1 - 1] += MAX2(0.0, maxGreen[R1Phase - 1] - (currentInCycleTime - ModeCycle(phaseStartTime[R1Phase - 1], myCycleLength)));
                    } else {
                        // I have to be off by my fix force off
                        maxGreen[myNextPhaseR1 - 1] = MIN2(maxGreenMaster[myNextPhaseR1 - 1], ModeCycle(forceOffs[myNextPhaseR1 - 1] - currentInCycleTime, myCycleLength));
                    }
                }
            }
            R1Phase = myNextPhaseR1;
            R1RYG = GREEN;
            phaseStartTime[R1Phase - 1] = currentTimeInSecond;
            R1State = R1Phase;
            wait4R1Green = false;
            myNextPhaseR1 = 0;
            myNextPhaseR1Distance = 0;
        }
    }

    if (wait4R2Green) {
        if ((currentTimeInSecond - phaseEndTimeR2) < yellowTime[R2Index]) {
            // Debugging assertion. A phase should NEVER be served for shorter than it's minimum time
            assert(durationR2 >= minGreen[R2Index]);
            R2RYG = YELLOW;
        } else if ((currentTimeInSecond - phaseEndTimeR2) < (yellowTime[R2Index] + redTime[R2Index])) {
            R2RYG = RED;
            // Reset the maxGreen to the master maxGreen time
            maxGreen[R2Phase - 1] = maxGreenMaster[R2Phase - 1];
        } else {
            if (myNextPhaseR2 == r2coordinatePhase) {
                if (coordinateMode) {
                    phaseExpectedDuration[myNextPhaseR2 - 1] = coordModeCycle(currentTimeInSecond, myNextPhaseR2);
                    if (myCabinetType == TS2){
                        assert(ModeCycle(forceOffs[myNextPhaseR2 - 1] - currentInCycleTime, myCycleLength) >= maxGreen[myNextPhaseR2 - 1]); 
                    } else {
                        if (currentTimeInSecond > (myCycleLength + offset)){
                            // The intial phases cause 
                            assert(phaseExpectedDuration[myNextPhaseR2 - 1] >= maxGreen[myNextPhaseR2 - 1]);
                        }
                    }
                }
            } else {
                if (coordinateMode && myCabinetType == TS2){
                    // TS2 in coordinated mode allows phases to be served as long as they can achieve their minimum time.
                    // We force them to make the split (doesn't affect fix force off) by ending the phase at it's forceOff.
                    if (fixForceOff){
                        // Let it use the remaining time in 
                        maxGreen[myNextPhaseR2 - 1] = ModeCycle(forceOffs[myNextPhaseR2 - 1] - currentInCycleTime, myCycleLength);
                        // Add the last phase's remaining time onto my max time. 
                        maxGreen[myNextPhaseR2 - 1] += MAX2(0.0, maxGreen[R2Phase - 1] - (currentInCycleTime - ModeCycle(phaseStartTime[R2Phase - 1], myCycleLength)));
                    } else {
                        // I have to be off by my fix force off
                        maxGreen[myNextPhaseR2 - 1] = MIN2(maxGreenMaster[myNextPhaseR2 - 1], ModeCycle(forceOffs[myNextPhaseR2 - 1] - currentInCycleTime, myCycleLength));
                    }
                }
            }
            R2Phase = myNextPhaseR2;
            R2RYG = GREEN;
            phaseStartTime[R2Phase - 1] = currentTimeInSecond;
            R2State = R2Phase;
            wait4R2Green = false;
            myNextPhaseR2 = 0;
            myNextPhaseR2Distance = 0;
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

    // Clear the Detectors
    clearDetectors();
    assert(findBarrier(R1Phase, 0) == findBarrier(R2Phase, 1));
    myPhase.setName(toString(R1Phase) + "+" + toString(R2Phase));
    return outputState;
}

int NEMALogic::nextPhase(std::vector<int> ring, int currentPhase, int& distance, bool sameAllowed, int ringNum) {
    int length = (int)ring.size();
    int flag = 0;
    int nphase = 0; // next phase
    int i = 0; // i represents the distance
    int matching_i = 0;
    for (i = 0; i < length * 2; i++) {
        if (flag == 1) {
            distance ++;
            if (ring[i % length] != 0) {
                int tempPhase = ring[i % length];
                if (recall[tempPhase-1] || readDetector(tempPhase)){
                    if (fitInCycle(tempPhase, ringNum)){
                        nphase=tempPhase;
                        break;
                    }
                }

#ifdef DEBUG_NEMA
                else {
                    std::cout << "phase " << tempPhase << " was skipped" << std::endl;
                }
#endif
            }
        }
        if (ring[i % length] == currentPhase) {
            flag = 1;
            matching_i = i;
        }
    }
    if (nphase != 0) {
        if ((nphase != currentPhase) || sameAllowed){
            return nphase;
        } else {
            // Get the next sequential phase.
            // At this point, i the index for currentPhase, so start there and look for the next sequential
            // Handle 0 in the ring with the for loop
            int nPhaseTemp = 0;
            for (i; i < length * 4; i++){
                distance ++;
                if (ring[i % length] != 0){
                    if (ring[i % length] != currentPhase && fitInCycle(ring[matching_i % length], ringNum)){
                        nPhaseTemp = ring[i % length];
                        break;
                    }
                }
            }
            // There is a case where there is no other non-zero phase found
            // This occurs when a ring is [ 0 2 | 0 4 ] for example  
            return nPhaseTemp != 0 ? nPhaseTemp : nphase;
        }
    } else {
        // this should only occur in the subset
        if (sameAllowed && (fitInCycle(ring[matching_i % length], ringNum))) {
            return ring[matching_i % length];
        } else {
            matching_i++; 
            // Handle 0 phases
            for (matching_i; matching_i < length * 4; matching_i++){
                if (ring[matching_i % length] != 0 && fitInCycle(ring[matching_i % length], ringNum)){
                    distance++;
                    break;       
                }
            }
            return ring[matching_i % length];
        }
    }
}


void NEMALogic::constructBarrierMap(int ring, std::vector<std::vector<int>>& barrierMap) {
    int flag = 0;
    std::vector<int> barrierOne;
    std::vector<int> barrierTwo;
    for (int localPhase : rings[ring]) {
        if (!flag) {
            barrierOne.push_back(localPhase);
            if (((localPhase == r1coordinatePhase || localPhase == r1barrier) && ring == 0) || ((localPhase == r2coordinatePhase || localPhase == r2barrier) && ring == 1)) {
                flag = 1;
            }
        } else {
            barrierTwo.push_back(localPhase);
        }
    }
    barrierMap.push_back(barrierOne);
    barrierMap.push_back(barrierTwo);
}

int NEMALogic::findBarrier(int phase, int ring) {
    int barrier = 0;
    for (int localPhase : myRingBarrierMapping[ring][1]) {
        if (phase == localPhase) {
            barrier = 1;
            break;
        }
    }
    return barrier;
}


std::tuple<int, int> NEMALogic::getNextPhases(int R1Phase, int R2Phase, int& r1Distance, int& r2Distance, bool toUpdateR1, bool toUpdateR2, bool stayOk) {
    // If myNextPhase has already been set, pass that to the next phase logic. 
    // If it hasn't (=0) then pass in the current phase
    // R1Phase = myNextPhaseR1 == 0 ? R1Phase : myNextPhaseR1; 
    // R2Phase = myNextPhaseR2 == 0 ? R2Phase : myNextPhaseR2;                
    // If myNextPhase has already been set, tell the next phase algo that it CANNOT be changed
    // Aka that wait4Green is false
    // toUpdateR1 = myNextPhaseR1 == 0 ? toUpdateR1 : false; 
    // toUpdateR2 = myNextPhaseR2 == 0 ? toUpdateR2 : false; 
    int nextR1Phase = R1Phase;
    int nextR2Phase = R2Phase;
    int currentR1Barrier = findBarrier(R1Phase, 0);
    int currentR2Barrier = findBarrier(R2Phase, 1);
    // Only 1 or both can be !toUpdate (otherwise we wouldn't be in this situation)
    if (!toUpdateR1) {
        int d = 0;
        // this adds handling for faulty initial conditions
        int defaultPhase = (currentR2Barrier == currentR1Barrier) ? R2Phase : myRingBarrierMapping[1][currentR1Barrier].back();
        nextR2Phase = nextPhase(myRingBarrierMapping[1][currentR1Barrier], defaultPhase, d, stayOk, 1);
        // If we aren't updating both, the search range is only the subset of values on the same side of the barrier;
    } else if (!toUpdateR2) {
        int d = 0;
        // this adds handling for faulty initial conditions
        int defaultPhase = (currentR1Barrier == currentR2Barrier) ? R1Phase : myRingBarrierMapping[0][currentR2Barrier].back();
        nextR1Phase = nextPhase(myRingBarrierMapping[0][currentR2Barrier], defaultPhase, d, stayOk, 0);
    } else {
        // Both can be updated. We should take the change requiring the least distance travelled around the loop,
        // and then recalculate the other ring if it is not in the same barrier
        int tempR1Distance = 0;
        int tempR2Distance = 0;
        // TODO: Changing this stayOk to true could be the cause of issues down the line
        nextR1Phase = nextPhase(rings[0], R1Phase, tempR1Distance, true, 0);
        nextR2Phase = nextPhase(rings[1], R2Phase, tempR2Distance, true, 1);
        int localR1Barrier = findBarrier(nextR1Phase, 0);
        int localR2Barrier = findBarrier(nextR2Phase, 1);

        bool atR1Barrier = (R1Phase == r1coordinatePhase || R1Phase == r1barrier);
        bool atR2Barrier = (R2Phase == r2coordinatePhase || R2Phase == r2barrier);

        // Simple Case. 1 + 6 cannot go to 2 + 5 unless in Green Rest. 
        if (!coordinateMode && (localR1Barrier == localR2Barrier) && (localR2Barrier == currentR2Barrier)){
            if (atR2Barrier && (R2RYG == GREEN || R2RYG == GREENTRANSFER)){
                nextR2Phase = R2Phase; 
            }
            if (atR1Barrier && (R1RYG == GREEN || R1RYG == GREENTRANSFER)){
                nextR1Phase = R1Phase; 
            }
        }

        // Barrier Check. The only time that a phase that is at the barrier can move "reverse" away from the barrier (3+8 -> 4+7)
        // Is when in "Green Rest". That has already been calculated by this point, so if 8 has gone yellow -> red, we have to go to the other barrier
        // Check this be asserting that if the R1Phase is at a barrier and it is < GREEN (YELLOW || RED) AND it's desired phase 
        // is on the same side of the barrier that it is currently, then we need to recalculate on the other side of the barrier
        if (((coordinateMode && (atR1Barrier || atR2Barrier)) 
            || (!coordinateMode && (atR1Barrier && atR2Barrier))) 
            && (R1RYG < GREEN && R2RYG < GREEN)){
            // If either of my phase are at the barrier and have gone yellow, then both phases have to cross the barrier
            if (localR1Barrier == currentR1Barrier){
                nextR1Phase = nextPhase(myRingBarrierMapping[0][!currentR1Barrier], myRingBarrierMapping[0][!currentR1Barrier].back(), tempR1Distance, true, 0);
            } 
            if (localR2Barrier == currentR2Barrier){
                nextR2Phase = nextPhase(myRingBarrierMapping[1][!currentR2Barrier], myRingBarrierMapping[1][!currentR2Barrier].back(), tempR2Distance, true, 1);
            }
        }
        // Handle the case where the distance is the same and in coordinated mode.
        // TODO: Refactor all of this logic. 
        else if (coordinateMode && (tempR1Distance == tempR2Distance) && (localR1Barrier != localR2Barrier)){
            // Give preference to result on the side of the current barrier.
            if (localR2Barrier == currentR2Barrier){
                // recalculate phase 1
                if (R1RYG >= GREEN && atR1Barrier){
                    nextR1Phase = R1Phase;
                } else {
                    int defaultPhase = (R1RYG >= GREEN && currentR1Barrier == localR2Barrier) ? R1Phase : myRingBarrierMapping[0][localR2Barrier].back();
                    nextR1Phase = nextPhase(myRingBarrierMapping[0][localR2Barrier], defaultPhase, tempR1Distance, true, 0);
                }
            } else {
                if (R2RYG >= GREEN && atR2Barrier){
                    nextR2Phase = R2Phase;
                } else {
                    int defaultPhase = (R2RYG >= GREEN && currentR2Barrier == localR1Barrier) ? R2Phase : myRingBarrierMapping[1][localR1Barrier].back();
                    nextR2Phase = nextPhase(myRingBarrierMapping[1][localR1Barrier], defaultPhase, tempR2Distance, true, 0);
                }
            }
        }
        // If the initially calculated next phases end up on opposite sides of a barrier, regardless of whether I am at a barrier phase or not,  
        // recalculated the one with a longer travelled distance 
        else if ((tempR1Distance <= tempR2Distance) && (localR1Barrier != localR2Barrier)) {
            // If I am still green and already at a barrier on the side of the new R1Phase, I cannot transition away from that barrier.
             if (!coordinateMode && localR2Barrier == currentR2Barrier && tempR2Distance <= tempR1Distance){
                // Actually give preference to ring 2 here. If R1 is already at the barrier, it cannot move away from the barrier.
                if (atR1Barrier && (R1RYG == GREEN || R1RYG == GREENTRANSFER)){
                    nextR1Phase = R1Phase; 
                } else {
                    int tmpStartPhase  = (R1Phase == r1coordinatePhase || R1Phase == r1barrier || R1RYG > GREEN || (!stayOk && R1RYG < GREEN))? 
                                        myRingBarrierMapping[0][localR2Barrier].front(): myRingBarrierMapping[0][localR2Barrier].back();
                    nextR1Phase = nextPhase(myRingBarrierMapping[0][localR2Barrier], tmpStartPhase, tempR1Distance, stayOk, 0);
                }
            } 
            else if (!coordinateMode && localR1Barrier == currentR1Barrier && tempR1Distance <= tempR2Distance){
                // Actually give preference to ring 2 here. If R1 is already at the barrier, it cannot move away from the barrier.
                if (atR2Barrier && (R2RYG == GREEN || R2RYG == GREENTRANSFER) ){
                    nextR2Phase = R2Phase;
                } else {
                    int tmpStartPhase  = (R2Phase == r2coordinatePhase || R2Phase == r2barrier || R2RYG > GREEN || (!stayOk && R2RYG < GREEN))? 
                                        myRingBarrierMapping[1][localR1Barrier].front(): myRingBarrierMapping[1][localR1Barrier].back();
                    nextR2Phase = nextPhase(myRingBarrierMapping[1][localR1Barrier], tmpStartPhase, tempR2Distance, stayOk, 1);
                }
            }
            else{
                if (R2RYG >= GREEN && ((R2Phase == r2barrier && localR1Barrier == 1) || (R2Phase == r2coordinatePhase && localR1Barrier == 0))){
                    nextR2Phase = R2Phase;
                } else {
                    int defaultPhase = ((R2RYG >= GREEN && currentR2Barrier == localR1Barrier) || (!stayOk && R2RYG < GREEN)) ? R2Phase : myRingBarrierMapping[1][localR1Barrier].back();
                    nextR2Phase = nextPhase(myRingBarrierMapping[1][localR1Barrier], defaultPhase, tempR2Distance, stayOk, 1);
                }
            } 
        } else if ((tempR1Distance > tempR2Distance) && (localR1Barrier != localR2Barrier)) {
            if (R1RYG >= GREEN && ((R1Phase == r1barrier && localR2Barrier == 1) || (R1Phase == r1coordinatePhase && localR2Barrier == 0))){
                nextR1Phase = R1Phase;
            } else {
                int defaultPhase = ((R1RYG >= GREEN && currentR1Barrier == localR2Barrier) || (!stayOk && R1RYG < GREEN)) ? R1Phase : myRingBarrierMapping[0][localR2Barrier].back();
                nextR1Phase = nextPhase(myRingBarrierMapping[0][localR2Barrier], defaultPhase, tempR1Distance, stayOk, 0);
            }
        }
    }
    // Update the straightline distance
    r1Distance = measureRingDistance(R1Phase, nextR1Phase, 0);
    r2Distance = measureRingDistance(R2Phase, nextR2Phase, 1);
    // Only actually keep the changes if the controller wants to transition the state
    // We must do this because myNextPhaseR<1,2>Phase = 0 is special and indicates that 
    // the next phase should be calculated the next time the phase is up to be checked
    return std::make_tuple(toUpdateR1 ? nextR1Phase : myNextPhaseR1, toUpdateR2 ? nextR2Phase : myNextPhaseR2);
}


int NEMALogic::measureRingDistance(int currentPhase, int nextPhase, int ringNum){
        int length = (int)rings[ringNum].size();
        int d = 0;
        bool found = false;        
        for (int i = 0; i < (length * 2); i++){
            if (rings[ringNum][i % length] != 0){ 
                if (found){
                   d++;
                   if (rings[ringNum][i % length] == nextPhase){
                       break;
                   } 
                }
                else if (rings[ringNum][i % length] == currentPhase){
                   found = true;
                } 
            }
        }
        assert(d > 0);
        return d;
}


//b should be the base of mode
double NEMALogic::ModeCycle(double a, double b) {
    double c = a - b;
    while (c >= b) {
        c = c - b;
    }
    while (c < 0) { //should be minimum green (or may be  not)
        c += b;
    }
    return c;
}

std::string NEMALogic::transitionState(std::string curState, int RYG) {
    std::string newState = "";
    if (RYG >= GREEN) {
        //Green
        newState = curState;

    } else if (RYG == RED) {
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
        return true;
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


const std::string
NEMALogic::getParameter(const std::string& key, const std::string defaultValue) const {
    if (StringUtils::startsWith(key, "NEMA.")) {
        if (key == "NEMA.phaseCall") {
            std::string out_str=std::to_string(isDetectorActivated(1, phase2DetectorMap.find(1) -> second));
            for (int i = 2; i<=8; i++)
            {
                out_str+=",";
                if (phase2DetectorMap.find(i) != phase2DetectorMap.end()) {
                    out_str+=std::to_string(isDetectorActivated(i, phase2DetectorMap.find(i) -> second));
                } else {
                    out_str+=std::to_string(false);
                }
            }
            return out_str;
        } else {
            throw InvalidArgument("Unsupported parameter '" + key + "' for NEMA controller '" + getID() + "'");
        }
    } else {
        return Parameterised::getParameter(key, defaultValue);
    }
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

void
NEMALogic::error_handle_not_set(std::string param_variable, std::string param_name) {
    if (param_variable == "") {
        throw InvalidArgument("Please set " + param_name + " for NEMA tlLogic '" + getID() + "'");
    }
}

void
NEMALogic::checkDetectors(){
    for (auto &p: phase2DetectorMap){
        // I don't need to check again if the detector is already active
        if (!p.second.detectActive){
            p.second.detectActive = isDetectorActivated(p.first, p.second, 0);
            #ifdef DEBUG_NEMA
            std::cout << "TL " << myID <<" Detector  "<< p.first <<":  "<< p.second.detectActive <<std::endl;
            #endif
        }
    }
}

void
NEMALogic::calculateForceOffs170(int r1StartIndex, int r2StartIndex){

    int initialIndexRing[2] = {r1StartIndex, r2StartIndex};

    // calculate force offs with the rings in order
    for (int ringNumber = 0; ringNumber<2;ringNumber++){
        int length = (int)rings[ringNumber].size();
        int aPhaseNumber = rings[ringNumber][initialIndexRing[ringNumber]];
        int aPhaseIndex = aPhaseNumber - 1;
        int nPhaseIndex = aPhaseIndex; //next phase
        int nPhaseNumber = aPhaseNumber;
        forceOffs[aPhaseNumber-1]=maxGreen[aPhaseNumber-1];
        #ifdef DEBUG_NEMA
        std::cout << "Phase  "<<aPhaseNumber <<": force off "<<forceOffs[aPhaseNumber-1]<<std::endl;
        #endif
        for (int i = initialIndexRing[ringNumber]+1; i < length; i++) {
            nPhaseNumber = rings[ringNumber][i];
            nPhaseIndex = nPhaseNumber -1;
            // std::cout <<" ring "<<ringNumber <<" i: "<<i<< " phase: "<<nPhaseNumber<< std::endl;
            if (nPhaseNumber != 0){
                forceOffs[nPhaseIndex] = forceOffs[aPhaseIndex] + maxGreen[nPhaseIndex] + yellowTime[aPhaseIndex]+redTime[aPhaseIndex];
                aPhaseNumber = nPhaseNumber;
                aPhaseIndex = nPhaseIndex;

                #ifdef DEBUG_NEMA
                std::cout << "- Phase "<<aPhaseNumber <<": force off "<<forceOffs[aPhaseIndex]<<std::endl;
                #endif
            }
        }
    }
}

void
NEMALogic::clearDetectors(){
    
    for (auto &p: phase2DetectorMap){
        // If the detector isn't latching then it is marked as off.
        // If it is latching and the current green phase is the latching detectors phase, we can mark it as off as well
        if ((!p.second.latching) || ((p.first == R1State) || (p.first == R2State))){
            p.second.detectActive = false;
        }
    }
}

void
NEMALogic::calculateForceOffsTS2(){
    // TS2 "0" cycle time is the start of the "first" coordinated phases.
    // We can find this "0" point by first constructing the forceOffs in sequential order via the 170 method 
    calculateForceOffs170(0, 0);

    // Switch the Force Off Times to align with TS2 Cycle. 
    double minCoordTime = MIN2(forceOffs[r1coordinatePhase - 1] - maxGreen[r1coordinatePhase - 1], forceOffs[r2coordinatePhase - 1] - maxGreen[r2coordinatePhase - 1]);

    // loop rings individually
    for (int i = 0; i < 2; i++){
        for (int p : rings[i]){
            if (p > 0){
                if ((forceOffs[p - 1] - minCoordTime) >= 0){
                    forceOffs[p - 1] -= minCoordTime;
                }else {
                    forceOffs[p - 1] = (myCycleLength + (forceOffs[p - 1] - minCoordTime));
                }
            }
        }
    } 
}

void
NEMALogic::calculateInitialPhases170(){
    int initialIndexRing [2] = {0, 0};
    // calculate initial phases based on in cycle clock
    for (int ringNumber = 0; ringNumber<2;ringNumber++){
        int length = (int)rings[ringNumber].size();
        for (int i = initialIndexRing[ringNumber]; i < length; i++) {
            int aPhaseIndex = rings[ringNumber][i]-1;
            if (aPhaseIndex != -1){
                if (forceOffs[aPhaseIndex] - minGreen[aPhaseIndex] > 0){
                    phaseCutOffs[aPhaseIndex] = forceOffs[aPhaseIndex] - minGreen[aPhaseIndex];
                } else {
                    phaseCutOffs[aPhaseIndex] = myCycleLength - forceOffs[aPhaseIndex] - minGreen[aPhaseIndex];
                }
                #ifdef DEBUG_NEMA
                std::cout << "Phase "<<aPhaseIndex+1<<" cut off is "<<phaseCutOffs[aPhaseIndex]<<std::endl;
                #endif
            }
        }
    }

    // sort phaseCutOffs in order, this is to adapt it to the TS2 algorithm. 
    // Type 170 should already be sorted.
    // Slice Phase Cutoffs into Ring1 & Ring 2
    std::vector<IntVector> localRings = rings;
    for (int ringNumber = 0; ringNumber < 2; ringNumber++){
        std::sort(localRings[ringNumber].begin(), localRings[ringNumber].end(), [&](int i, int j) 
        { return phaseCutOffs[i - 1] < phaseCutOffs[j - 1]; });
    }

    // find the current in cycle time.
    SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    double currentTimeInSecond = STEPS2TIME(now);
    double currentInCycleTime = ModeCycle(currentTimeInSecond - cycleRefPoint - offset, myCycleLength);

    // find the initial phases
    bool found[2] = {false, false};
    for (int ringNumber = 0; ringNumber < 2; ringNumber++){
        int aPhaseIndex = -1;
        // This searches sorted
        for (int p: localRings[ringNumber]) {
            if (p > 0){
                aPhaseIndex = p - 1;
                // #TODO: Fix this logic intelligently.
                if ((myCabinetType == Type170 && (currentInCycleTime + minGreen[p - 1] < phaseCutOffs[p - 1]))
                    || (myCabinetType == TS2 && fitInCycle(p, ringNumber)))
                    {   
                    #ifdef DEBUG_NEMA
                    std::cout<<"current in cycle time="<<currentInCycleTime<<" phase: "<<aPhaseIndex<<std::endl;
                    #endif
                    found[ringNumber] = true;
                    break;
                }
            }
        }
        if (ringNumber == 0){
            if (found[ringNumber]){
                activeRing1Index = aPhaseIndex;
                activeRing1Phase = activeRing1Index + 1;
            }
        }
        else{
            if (found[ringNumber]){
                activeRing2Index = aPhaseIndex;
                activeRing2Phase = activeRing2Index + 1;
            }
        }
    }
    if (found[0] * found[1] < 1){
        // If one or the other phases weren't found, default to the coordinated phase.
        // This ensures that no barriers are crossed
        activeRing2Phase = r2coordinatePhase;
        activeRing2Index = r2coordinatePhase - 1;
        activeRing1Phase = r1coordinatePhase;
        activeRing1Index = r1coordinatePhase - 1;
    }
}

bool
NEMALogic::readDetector(int phase){
    return phase2DetectorMap.find(phase) -> second.detectActive;
}
void
NEMALogic::calculateInitialPhasesTS2(){
    // Modifications where made to 170 algorithm so that it works with both.
    calculateInitialPhases170();

    // Set the phase expected duration to initialize correctly
    phaseExpectedDuration[activeRing1Phase - 1] = activeRing1Phase == r1coordinatePhase? coordModeCycleTS2(0, activeRing1Phase) : minGreen[activeRing1Phase - 1];
    phaseExpectedDuration[activeRing2Phase - 1] = activeRing2Phase == r2coordinatePhase? coordModeCycleTS2(0, activeRing2Phase) : minGreen[activeRing2Phase - 1];
}

double
NEMALogic::coordModeCycle170(double currentTime, int phase){
    return ModeCycle(myCycleLength - (currentTime - cycleRefPoint - offset) - yellowTime[phase - 1] - redTime[phase - 1], myCycleLength);  
}

double
NEMALogic::coordModeCycleTS2(double currentTime, int phase){
    // This puts the phase green for the rest of the cycle, plus the first bit in which it must be green
    // We don't need the yellow and red here because the force off already incorporates that.
    return ModeCycle((myCycleLength + forceOffs[phase - 1]) - (currentTime - cycleRefPoint - offset), myCycleLength);
}

bool
NEMALogic::fitInCycleTS2(int phase, int ringNum){
    if (!coordinateMode   
        || ((phase == r2coordinatePhase) || (phase == r1coordinatePhase))
        // || ((ringNum == 0 && (R1State != r1coordinatePhase)) || (ringNum == 1 && (R2State != r2coordinatePhase))))
        ){
        return true;
    } else {
        bool iFit = true;
        double currentTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
        double timeInCycle = ModeCycle(currentTime - cycleRefPoint - offset, myCycleLength);
        int length = (int)rings[ringNum].size();

        // Find the path to the coordinate phase
        // If the proceeding phase can fit, then I should not mark myself as "fitting".
        int proceedingPhase = 0;
        for (int j = 0; j < (length * 2); j++){
            if (rings[ringNum][j % length] != 0){
                if ((rings[ringNum][j % length] == phase) && (proceedingPhase != 0)){
                    break;
                } else {
                    proceedingPhase = rings[ringNum][j % length];
                }
            }
        }
        
        if (proceedingPhase != 0){
            iFit = false;
            // Calculate the transition time of the proceeding phase:
            // double transitionTimes[2] = {0.0, 0.0};
            double transitionTime = 0.0;
            if (ringNum < 1){
                if (R1RYG < GREEN){
                    transitionTime = MAX2(0.0, (yellowTime[R1State - 1] + redTime[R1State - 1]) - (currentTime - phaseEndTimeR1));
                } else {
                    transitionTime = yellowTime[R1State - 1] + redTime[R1State - 1];
                }
            } else {
                if (R2RYG < GREEN){
                    transitionTime = MAX2(0.0, (yellowTime[R2State - 1] + redTime[R2State - 1]) - (currentTime - phaseEndTimeR2));
                } else {
                    transitionTime = yellowTime[R2State - 1] + redTime[R2State - 1];
                }
            }

            // Red is given as lee-way. This is surprising but proven in tests
            // double endTime = ModeCycle(maxGreen[phase - 1] - redTime[phase - 1] + timeInCycle,  myCycleLength);
            double d = forceOffs[phase - 1] - timeInCycle;
            d = d >= 0? d :  d + myCycleLength;
            double priorD = forceOffs[proceedingPhase - 1] - timeInCycle;
            priorD = priorD >= 0? priorD :  priorD + myCycleLength;
            // only consider the fit cases if I fit
            // At the coordinated phases?
            // The logic is slightly different for coordinated phases, so the flag below is set.
            bool coordPhasesActive = (R1State == r1coordinatePhase) && (R2State == r2coordinatePhase);
            if (d >= (minGreen[phase - 1] + transitionTime)){
                // If I am inside of my time chunk in the cycle, say that I fit. 
                if (coordPhasesActive){
                    if (d <= priorD){
                        iFit = true;
                    }
                    // if the prior phase is closer to it's cutoff than I am to mine, but the prior phase doesn't fit, return that I do fit.
                    // This al 
                    else if (priorD <= (minGreen[proceedingPhase - 1] + transitionTime)){
                        iFit = true;
                    }
                    // If the proceeding phase is already active, I can automatically go next.
                    else if ((proceedingPhase == R1State && proceedingPhase != r1coordinatePhase) || (proceedingPhase == R2State && proceedingPhase != r2coordinatePhase)){
                        iFit = true;
                    }  
                    // If the current phase is in a transition, then say that I can myself fit, even if a proceeding phase can also fit.
                    else if ((R1RYG < GREEN) && (R2RYG < GREEN)){
                        // Have to do an extra check here to make sure that my "distance" is not wrapped around the cycle
                        int nextPhase = phase;
                        for (int i = 0; i < (length * 2); i++){
                            if (rings[ringNum][i % length] != 0){ 
                                // Find the next phase
                                double nextPhaseD = rings[ringNum][i % length] - timeInCycle;
                                nextPhaseD = nextPhaseD >= 0? nextPhaseD :  nextPhaseD + myCycleLength;
                                if (nextPhaseD < d){
                                    nextPhase = rings[ringNum][i % length];
                                    break;
                                }
                            }
                        }
                        if (nextPhase == phase){
                            iFit = true;
                        }
                    }   
                }
                else {
                    // the current phases are not the coordinated phases but I can fit, return true. Otherwise return False! 
                    // This can happen with green transfers etc.
                    iFit = true;
                }
            }
        }
        return iFit;
    }
}
