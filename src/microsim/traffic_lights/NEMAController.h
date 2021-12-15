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
/// @file    NEMAController.h
/// @author  Tianxin Li
/// @author  Qichao Wang
/// @date    August 2020
///
// An actuated NEMA-phase-compliant traffic light logic
/****************************************************************************/
#pragma once
#include <config.h>

#include <utility>
#include <vector>
#include <bitset>
#include <map>
#include <microsim/MSEventControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include "MSSimpleTrafficLightLogic.h"
#include "microsim/output/MSE2Collector.h"
#include <set>


// ===========================================================================
// class declarations
// ===========================================================================
class NLDetectorBuilder;
class MSE2Collector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NEMALogic
 * @brief A NEMA (adaptive) traffic light logic based on E2Detector
 */
class NEMALogic : public MSSimpleTrafficLightLogic {
public:

    /// @brief Definition of a map from lanes to corresponding area detectors
    typedef std::map<MSLane*, MSE2Collector*> LaneDetectorMap;

    /// @brief Definition of a map from detectors to corresponding lanes
    typedef std::map<MSE2Collector*, MSLane*> DetectorLaneMap;

    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] programID This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] parameter The parameter to use for tls set-up
     */
    NEMALogic(MSTLLogicControl& tlcontrol,
              const std::string& id, const std::string& programID,
              const SUMOTime offset,
              const MSSimpleTrafficLightLogic::Phases& phases,
              int step, SUMOTime delay,
              const std::map<std::string, std::string>& parameter,
              const std::string& basePath);


    /** @brief Initialises the tls with information about incoming lanes
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder& nb) override;

    /// @brief Destructor
    ~NEMALogic();


    SUMOTime trySwitch() override;

    /// @name Dynamic Information Retrieval
    /// @{

    /** @brief Returns the definition of the current phase
     * @return The current phase
     */
    const MSPhaseDefinition& getCurrentPhaseDef() const override;
    /// @}

    /// @brief called when switching programs
    void activateProgram() override;
    void deactivateProgram() override;

    bool showDetectors() const {
        return myShowDetectors;
    }

    void setShowDetectors(bool show);

    std::map<int, std::vector<MSE2Collector*>>* getPhase2DetectorMap() {
        return &phase2DetectorMap;
    }

    // control logic
    std::string NEMA_control();

    std::string combineStates(std::string state1, std::string state2);

    bool isDetectorActivated(int phaseIndex);

    int nextPhase(std::vector<int> ring, int currentPhase);

    double ModeCycle(double a, double b);

    std::string transitionState(std::string curState, int RYG);

    std::set<std::string> getLaneIDsFromNEMAState(std::string state);

    void setNewMaxGreens(std::vector<double> newMaxGreens);
    void setNewSplits(std::vector<double> newSplits);
    void setNewCycleLength(double newCycleLength);
    void setNewOffset(double newOffset);

    // not using for now, but could be helpful for cycle change controller
    double getCurrentCycleLength() {
        return myCycleLength;
    }

    void setCycleLength(double newCycleLength) {
        myCycleLength = newCycleLength;
    }

    bool isGreenPhase(std::string state) {
        for (char ch : state) {
            if (ch == 'g' || ch == 'G') {
                return true;
            }
        }
        return false;
    }

    /// @brief try to set the given parameter. Parameters prefixed with 'NEMA.' control functionality
    void setParameter(const std::string& key, const std::string& value) override; 

protected:
    // create a small datatype for mapping detector to phase index
    // This is the one copied from MSActuatedTrafficLightLogic
    // not used in our controller, but it is here for meeting the SUMO default traffic logic light check
    // this one and related could be removed with extra efforts
    struct DetectorInfo {
        DetectorInfo(MSE2Collector* _det, int numPhases):
            det(_det),
            servedPhase(numPhases, false)
        {}
        MSE2Collector* det;
        SUMOTime lastGreenTime = 0;
        std::vector<bool> servedPhase;
    };

    typedef std::vector<std::vector<DetectorInfo*>> detectorMap;

    /// @brief return whether there is a major link from the given lane in the given phase
    bool hasMajor(const std::string& state, const LaneVector& lanes) const;
    /// @}
    std::vector<int> readParaFromString(std::string s);

    //convert laneIDs string to string vector
    std::vector<std::string> string2vector(std::string s);

    //decide whether the detector is for left turn lane
    //if it is, use the detector length for left turn lane
    bool isLeftTurnLane(const MSLane* const lane) const;

    //convert "1" to int 1
    int string2int(std::string s);

// protected:
    detectorMap myDetectorForPhase;

    std::vector<DetectorInfo> myDetectorInfoVector;

    /// @brief A map from lanes to detectors
    LaneDetectorMap myLaneDetectorMap;

    /// @brief A map from detectors to lanes
    DetectorLaneMap myDetectorLaneMap;

    // detector length
    double myDetectorLength;

    // detector length for left turn lane
    double myDetectorLengthLeftTurnLane;

    // total cycle length
    double myCycleLength;

    // total cycle length in the next cycle
    double myNextCycleLength;

    /// Whether the detectors shall be shown in the GUI
    bool myShowDetectors;

    /// The output file for generated detectors
    std::string myFile;

    /// The frequency for aggregating detector output
    SUMOTime myFreq;

    /// Whether detector output separates by vType
    std::string myVehicleTypes;

    //"3,4,1,2"
    std::string ring1;
    //"7,8,5,6"
    std::string ring2;
    /*
    {
        {3,4,1,2},
        {7,8,5,6}
    }
    */
    std::vector<std::vector<int>> rings;
    // "4,8"
    std::string barriers;
    // "2,6"
    std::string coordinates;

    //size = 2. 0->ring1 1->ring2
    std::vector<int> barrierPhaseIndecies;
    std::vector<int> coordinatePhaseIndecies;

    // std::vector<std::vector<int>> barriers;
    //init 0 and then 1,2,3,0
    //redundant need to remove
    int activeRing1Index;
    int activeRing2Index;
    //init 0. 0->barrierPhases; 1->coordinatePhases
    // bool coordinatePhases;

    //rings[0][activeRing1Index]
    int activeRing1Phase;
    //rings[1][activeRing1Index]
    int activeRing2Phase;

    /*
    {
        {1 : {det1, det2 ...}},
        {2 : {det3}},
        ...
        {8 : {...}}
    }
    */
    std::map<int, std::vector<MSE2Collector*>> phase2DetectorMap;

    double minGreen[8] {};
    double maxGreen[8] {};
    double nextMaxGreen[8] {};
    double vehExt[8] {};
    double yellowTime[8] {};
    double redTime[8] {};
    double phaseStartTime[8] {};
    double phaseExpectedDuration[8] {};

    double phaseEndTimeR1, phaseEndTimeR2;
    bool wait4R1Green, wait4R2Green;
    int R1RYG, R2RYG;
    double cycleRefPoint;// missing update
    //activeR1phase
    int R1State, R2State;
    double offset;
    double myNextOffset;
    int r1barrier, r2barrier;
    int r1coordinatePhase, r2coordinatePhase;

    std::map<int, std::vector<std::string>> phase2ControllerLanesMap;

    bool whetherOutputState;

    std::string currentState;
    std::string currentR1State;
    std::string currentR2State;

    std::string outputStateFilePath;
    std::ofstream outputStateFile;
    bool coordinateMode;

    /// @brief virtual phase that holds the current state
    MSPhaseDefinition myPhase;

};
