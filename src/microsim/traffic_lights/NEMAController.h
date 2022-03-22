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
    typedef std::map<MSE2Collector*, MSLane*, ComparatorIdLess> DetectorLaneMap;

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

    std::map<int, std::vector<MSE2Collector*>> getPhase2DetectorMap() {
        std::map<int, std::vector<MSE2Collector*>> temp;
        for (auto const& detectInfo : phase2DetectorMap) {
            temp[detectInfo.first] = detectInfo.second.detectors;
        }
        return temp;
    }

    /// @brief retrieve all detectors used by this program
    std::map<std::string, double> getDetectorStates() const override;

    // control logic
    std::string NEMA_control();

    std::string combineStates(std::string state1, std::string state2);

    int nextPhase(std::vector<int> ring, int phaseNum, int& distance,  bool sameAllowed, int ringNum);

    std::tuple<int, int> getNextPhases(int currentR1Index, int currentR2Index, int& r1Distance, int& r2Distance, bool toUpdateR1, bool toUpdateR2, bool stayOk = false);

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

    /// @brief try to get the value of the given parameter. Parameters prefixed with 'NEMA.' control functionality
    const std::string getParameter(const std::string& key, const std::string defaultValue = "") const override;

protected:
    /// @brief Initializes timing parameters and calculate initial phases
    void constructTimingAndPhaseDefs();

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

    // Light Head State
    enum lightState {
        YELLOW = 0,
        RED = 1,
        GREEN = 2,
        GREENTRANSFER = 3,
        GREENREST = 4,
    };

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

    /// @brief A map from lanes names to phases
    std::map<std::string, int> myLanePhaseMap;

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

    /*
    This serves as a mapping to speed up phaseSelection
    {
      {{3, 4}, {1, 2}},
      {{7, 8}, {5, 6}}
    }
    */
    std::vector<std::vector<int>> myRingBarrierMapping[2];

    // Creating a small extensible datatype for including information about the phase's detectors
    // this is different than DetectorInfo, as it is per-phase not per-detector.
    // Purpose is that when we check detectors, we may need to have per-detector settings handy
    struct phaseDetectorInfo {
        phaseDetectorInfo():
            detectors(),
            cpdTarget(),
            cpdSource(),
            detectActive(),
            latching()
        {}
         phaseDetectorInfo(int _cross_phase_source, bool _latching):
            cpdSource(_cross_phase_source),
            latching(_latching)
        {}
        std::vector<MSE2Collector*> detectors = {nullptr};
        int cpdTarget = 0;
        int cpdSource = 0;
        bool detectActive = false;
        bool latching = false;
    };

    bool isDetectorActivated(int phaseNumber, const phaseDetectorInfo &phaseInfo, int depth) const;
    // myNextPhase needs to be presevered in memory because the phase is calculated at start of yellow 
    // but not implementend until the end of red 
    int myNextPhaseR1;
    int myNextPhaseR1Distance;
    int myNextPhaseR2;
    int myNextPhaseR2Distance;

    bool minRecalls[8] {};
    bool maxRecalls[8] {};
    bool recall[8] {};

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
        {1 : phaseDetectorInfo{
                detectors: {det1, det2, ...},
                crossPhaseDetector: 6
            },
        },
        {2 : ...
    }
    */
    std::map<int, phaseDetectorInfo> phase2DetectorMap;

    double minGreen[8] {};
    double maxGreen[8] {};
    double maxGreenMaster[8] {};
    double nextMaxGreen[8] {};
    double vehExt[8] {};
    double yellowTime[8] {};
    double redTime[8] {};
    double phaseStartTime[8] {};
    double forceOffs[8] {};
    double phaseCutOffs[8] {};
    double phaseExpectedDuration[8] {};

    bool fixForceOff;

    double phaseEndTimeR1, phaseEndTimeR2;
    bool wait4R1Green, wait4R2Green;
    lightState R1RYG, R2RYG;
    double cycleRefPoint;// missing update
    //activeR1phase
    int R1State, R2State;
    double offset;
    double myNextOffset;
    int r1barrier, r2barrier;
    int r1coordinatePhase, r2coordinatePhase;

    std::map<int, std::vector<std::string>> phase2ControllerLanesMap;

    bool whetherOutputState;
    bool ignoreErrors;

    std::string currentState;
    std::string currentR1State;
    std::string currentR2State;

    std::string outputStateFilePath;
    std::ofstream outputStateFile;
    bool coordinateMode;
    
    // Cabinet Type
    // #TODO write a parser to convert parameter to type 
    enum cabinetType {
        Type170,
        TS2
    };

    // Store the cabinet type
    cabinetType myCabinetType;

    cabinetType parseCabinetType(std::string inputType){
        std::string cleanString;
        for (const char& c : inputType){
            if (isalpha(c) || isdigit(c)){
                cleanString += (char)::tolower(c);
            }
        }
        if (cleanString == "type170"){
            return Type170;
        } else if (cleanString == "ts2"){
            return TS2;
        } else {
            throw InvalidArgument("Please set cabinetType for NEMA tlLogic to either Type170 or TS2");
        }
    };

    /// @brief virtual phase that holds the current state
    MSPhaseDefinition myPhase;

    /// helps to construct myRingBarrierMapping
    void constructBarrierMap(int ring, std::vector<std::vector<int>>& barrierMap);
    int findBarrier(int desiredPhase, int ring);

    /// @brief measures the ring distance between two phases
    int NEMALogic::measureRingDistance(int currentPhase, int nextPhase, int ring);

    // Green Transfer Option
    bool greenTransfer;

    // handle error
    void error_handle_not_set(std::string param_variable, std::string param_name);
    void validate_timing();

    // read All Detectors
    void checkDetectors();
    // clear Detectors
    void clearDetectors();
    // read 1 detector state
    bool readDetector(int phase);

    // TS2 Specific Timing
    void calculateForceOffsTS2();
    // Type170 Specific Timing
    void calculateForceOffs170(int r1StartIndex = 0, int r2StartIndex = 0);
    // General Force Offs Function
    void calculateForceOffs(){
        switch (myCabinetType){
            case Type170:
                return calculateForceOffs170();
            case TS2:
                return calculateForceOffsTS2();
            default:
                return calculateForceOffs170();
        }
    }


    // TS2 Specific Initial Phases
    void calculateInitialPhasesTS2();
    // Type170 Specific Initial Phases
    void calculateInitialPhases170();
    // General Force Offs Function
    void calculateInitialPhases(){
        switch (myCabinetType){
            case Type170:
                return calculateInitialPhases170();
            case TS2:
                return calculateInitialPhasesTS2();
            default:
                // Default to Type170
                return calculateInitialPhases170();
        }
    }

    // TS2 Specific Coordinated Mode Cycle
    double coordModeCycleTS2(double currentTime, int phase);
    // Type170 Specific Coordinated Mode Cycle
    double coordModeCycle170(double currentTime, int phase);
    // General Force Offs Function
    double coordModeCycle(double currentTime, int phase){
        switch (myCabinetType){
            case Type170:
                return coordModeCycle170(currentTime, phase);
            case TS2:
                return coordModeCycleTS2(currentTime, phase);
            default:
                // Default to Type 170
                return coordModeCycle170(currentTime, phase);
        }
    }

    // TS2 Specific fit in cycle algorithm
    bool fitInCycleTS2(int phase,  int ringNum);
    // Type170 fitInCycle algorithm
    // bool fitInCycle170(int _phase, int _ringNum){
    //     return true;
    // }
    // 
    double fitInCycle(int phase, int ringNum){
        switch (myCabinetType){
            case Type170:
                return true;
            case TS2:
                return fitInCycleTS2(phase, ringNum);
            default:
                // Default to Type 170
                return true;
        }
    }
};
