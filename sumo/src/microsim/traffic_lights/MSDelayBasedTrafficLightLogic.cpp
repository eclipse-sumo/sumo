/****************************************************************************/
/// @file    MSDelayBasedTrafficLightLogic.cpp
/// @author  Leonhard Luecken
/// @date    Feb 2017
/// @version
///
// An actuated traffic light logic based on time delay of approaching vehicles
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
#include <vector>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSDetectorControl.h>
#include "MSSimpleTrafficLightLogic.h"
#include "MSDelayBasedTrafficLightLogic.h"
#include <microsim/MSLane.h>
#include <netload/NLDetectorBuilder.h>
#include <utils/common/TplConvert.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// parameter defaults definitions
// ===========================================================================

//#define DEBUG_TIMELOSS_CONTROL

// ===========================================================================
// method definitions
// ===========================================================================
MSDelayBasedTrafficLightLogic::MSDelayBasedTrafficLightLogic(MSTLLogicControl& tlcontrol,
        const std::string& id, const std::string& programID,
        const Phases& phases,
        int step, SUMOTime delay,
        const std::map<std::string, std::string>& parameter,
        const std::string& basePath) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, phases, step, delay, parameter) {

    myShowDetectors = TplConvert::_2bool(getParameter("show-detectors", "false").c_str());
    myDetectionRange = TplConvert::_2SUMOReal(getParameter("range", "-1.0").c_str());
    myTimeLossThreshold = TplConvert::_2SUMOReal(getParameter("minTimeloss", "1.0").c_str());
    myFile = FileHelpers::checkForRelativity(getParameter("file", "NUL"), basePath);
    myFreq = TIME2STEPS(TplConvert::_2SUMOReal(getParameter("freq", "300").c_str()));
    myVehicleTypes = getParameter("vTypes", "");
}


void
MSDelayBasedTrafficLightLogic::init(NLDetectorBuilder& nb) {
    MSTrafficLightLogic::init(nb);
    assert(myLanes.size() > 0);
    // change values for setting the loops and lanestate-detectors, here
    //SUMOTime inductLoopInterval = 1; //
    LaneVectorVector::const_iterator i2;
    LaneVector::const_iterator i;
    // build the induct loops
    for (i2 = myLanes.begin(); i2 != myLanes.end(); ++i2) {
        const LaneVector& lanes = *i2;
        for (i = lanes.begin(); i != lanes.end(); i++) {
            MSLane* lane = (*i);
            // Build the induct loop and set it into the container
            std::string id = "TLS" + myID + "_" + myProgramID + "_E2CollectorOn_" + lane->getID();
            if (myLaneDetectors.find(lane) == myLaneDetectors.end()) {
                myLaneDetectors[lane] = new MSE2Collector(id, DU_TL_CONTROL, lane, std::numeric_limits<SUMOReal>::max(), lane->getLength(), myDetectionRange, 0, 0, 0, myVehicleTypes);
                MSNet::getInstance()->getDetectorControl().add(SUMO_TAG_E2DETECTOR, myLaneDetectors[lane], myFile, myFreq);
            }
        }
    }
}



MSDelayBasedTrafficLightLogic::~MSDelayBasedTrafficLightLogic() { }

// ------------ Switching and setting current rows


SUMOTime
MSDelayBasedTrafficLightLogic::proposeProlongation() {
#ifdef DEBUG_TIMELOSS_CONTROL
    std::cout << "\n" << SIMTIME << " MSDelayBasedTrafficLightLogic::proposeProlongation() for TLS '" << this->getID() << "'" << std::endl;
#endif
    SUMOReal prolongationTime = 0.;
    const std::string& state = getCurrentPhaseDef().getState();
    // iterate over green lanes, eventually increase the proposed prolongationTime to the estimated passing time for each lane.
    for (int i = 0; i < (int) state.size(); i++)  {
        if (state[i] == LINKSTATE_TL_GREEN_MAJOR || state[i] == LINKSTATE_TL_GREEN_MINOR) {
            const std::vector<MSLane*>& lanes = getLanesAt(i);
            for (LaneVector::const_iterator j = lanes.begin(); j != lanes.end(); j++) {
                LaneDetectorMap::iterator i = myLaneDetectors.find(*j);

#ifdef DEBUG_TIMELOSS_CONTROL
                if (i == myLaneDetectors.end()) {
                    // no detector for this lane!?
                    std::cout << "no detector on lane '" << (*j)->getID() << std::endl;
                    continue;
                }
#endif

                MSE2Collector* detector = static_cast<MSE2Collector* >(i->second);
                const MSE2Collector::VehicleInfoMap& vehInfos = detector->getVehicleInfos();

#ifdef DEBUG_TIMELOSS_CONTROL
                std::cout << "Number of current vehicles on detector: " << vehInfos.size() << std::endl;
#endif

                for (MSE2Collector::VehicleInfoMap::const_iterator iv = vehInfos.begin(); iv != vehInfos.end(); ++iv){
                    if (iv->second->accumulatedTimeLoss > myTimeLossThreshold && iv->second->distToDetectorEnd > 0){
                        SUMOReal estimatedTimeToJunction = (iv->second->distToDetectorEnd)/(*j)->getSpeedLimit();
                        prolongationTime = MAX2(prolongationTime, estimatedTimeToJunction);

#ifdef DEBUG_TIMELOSS_CONTROL
                        std::cout << "vehicle '" << iv->id << "' with accumulated timeloss: " << iv->accumulatedTimeLoss
                                  << "\nestimated passing time: " << estimatedTimeToJunction << std::endl;
                    } else {
                        std::cout << "disregarded: (vehicle '" << iv->id << "' with accumulated timeloss " << iv->accumulatedTimeLoss << ")" << std::endl;
#endif
                    }
                }
            }
        }
    }
#ifdef DEBUG_TIMELOSS_CONTROL
    std::cout << "Proposed prolongation (maximal estimated passing time): " << prolongationTime << std::endl; // debug
#endif
    return TIME2STEPS(prolongationTime);
};



SUMOTime
MSDelayBasedTrafficLightLogic::trySwitch() {
    /* check if the actual phase should be prolonged */
    const MSPhaseDefinition& currentPhase = getCurrentPhaseDef();
    // time since last switch
    const SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - currentPhase.myLastSwitch;

#ifdef DEBUG_TIMELOSS_CONTROL
    std::cout << "last switch = " << currentPhase.myLastSwitch
              << "\nactDuration = " << actDuration << std::endl;
#endif

    // flag whether to prolong or not
    bool prolong = currentPhase.isGreenPhase() && actDuration < currentPhase.maxDuration && !MSGlobals::gUseMesoSim;
    if (prolong) {
        // keep this phase
        SUMOTime proposedProlongation = proposeProlongation();
        proposedProlongation = MAX3(SUMOTime(0), MIN2(proposedProlongation, currentPhase.maxDuration - actDuration), currentPhase.minDuration - actDuration);

#ifdef DEBUG_TIMELOSS_CONTROL
        std::cout << "Proposed prolongation = " << proposedProlongation << std::endl;
#endif

        prolong = proposedProlongation > 0;
        if (prolong) {
            // check again after the prolonged period (must be positive...)
            // XXX: Can it be harmful not to return a duration of integer seconds?
            return proposedProlongation;
        }
    }
    // Don't prolong... switch to the next phase
    myStep++;
    assert(myStep <= (int)myPhases.size());
    if (myStep == (int)myPhases.size()) {
        myStep = 0;
    }
    MSPhaseDefinition* newPhase = myPhases[myStep];
    //stores the time the phase started
    newPhase->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
    // set the next event
    return newPhase->minDuration;
}


/****************************************************************************/

