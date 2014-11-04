/****************************************************************************/
/// @file    MSActuatedTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @author  Julia Ringel
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// An actuated (adaptive) traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "MSActuatedTrafficLightLogic.h"
#include <microsim/MSLane.h>
#include <netload/NLDetectorBuilder.h>
#include <utils/common/TplConvert.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// parameter defaults definitions
// ===========================================================================
#define DEFAULT_MAX_GAP "3.1"
#define DEFAULT_PASSING_TIME "1.9"
#define DEFAULT_DETECTOR_GAP "3.0"


// ===========================================================================
// method definitions
// ===========================================================================
MSActuatedTrafficLightLogic::MSActuatedTrafficLightLogic(MSTLLogicControl& tlcontrol,
        const std::string& id, const std::string& programID,
        const Phases& phases,
        unsigned int step, SUMOTime delay,
        const std::map<std::string, std::string>& parameter) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, phases, step, delay, parameter) {

    myMaxGap = TplConvert::_2SUMOReal(getParameter("max-gap", DEFAULT_MAX_GAP).c_str());
    myPassingTime = TplConvert::_2SUMOReal(getParameter("passing-time", DEFAULT_PASSING_TIME).c_str());
    myDetectorGap = TplConvert::_2SUMOReal(getParameter("detector-gap", DEFAULT_DETECTOR_GAP).c_str());
}


void
MSActuatedTrafficLightLogic::init(NLDetectorBuilder& nb) {
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
            SUMOReal length = lane->getLength();
            SUMOReal speed = lane->getSpeedLimit();
            SUMOReal inductLoopPosition = myDetectorGap * speed;
            // check whether the lane is long enough
            SUMOReal ilpos = length - inductLoopPosition;
            if (ilpos < 0) {
                ilpos = 0;
            }
            // Build the induct loop and set it into the container
            std::string id = "TLS" + myID + "_" + myProgramID + "_InductLoopOn_" + lane->getID();
            if (myInductLoops.find(lane) == myInductLoops.end()) {
                myInductLoops[lane] = dynamic_cast<MSInductLoop*>(nb.createInductLoop(id, lane, ilpos, false));
                assert(myInductLoops[lane] != 0);
            }
        }
    }
}


MSActuatedTrafficLightLogic::~MSActuatedTrafficLightLogic() {
    for (InductLoopMap::iterator i = myInductLoops.begin(); i != myInductLoops.end(); ++i) {
        delete(*i).second;
    }
}


// ------------ Switching and setting current rows
SUMOTime
MSActuatedTrafficLightLogic::trySwitch(bool) {
    // checks if the actual phase should be continued
    // @note any vehicles which arrived during the previous phases which are now waiting between the detector and the stop line are not
    // considere here. RiLSA recommends to set minDuration in a way that lets all vehicles pass the detector
    // @todo: it would be nice to warn users if (inductLoopPosition / defaultLengthWithGap * myPassingTime > minDuration)
    const SUMOReal detectionGap = gapControl();
    if (detectionGap < std::numeric_limits<SUMOReal>::max()) {
        return duration(detectionGap);
    }
    // increment the index to the current phase
    myStep++;
    assert(myStep <= myPhases.size());
    if (myStep == myPhases.size()) {
        myStep = 0;
    }
    //stores the time the phase started
    myPhases[myStep]->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
    // set the next event
    return getCurrentPhaseDef().minDuration;
}


// ------------ "actuated" algorithm methods
SUMOTime
MSActuatedTrafficLightLogic::duration(const SUMOReal detectionGap) const {
    assert(getCurrentPhaseDef().isGreenPhase());
    assert(myPhases.size() > myStep);
    const SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - myPhases[myStep]->myLastSwitch;
    // ensure that minimum duration is kept
    SUMOTime newDuration = getCurrentPhaseDef().minDuration - actDuration;
    // try to let the last detected vehicle pass the intersection (duration must be positive)
    newDuration = MAX3(newDuration, TIME2STEPS(myDetectorGap - detectionGap), 1);
    // cut the decimal places to ensure that phases always have integer duration
    if (newDuration % 1000 != 0) {
        const SUMOTime totalDur = newDuration + actDuration;
        newDuration = (totalDur / 1000 + 1) * 1000 - actDuration;
    }
    // ensure that the maximum duration is not exceeded
    newDuration = MIN2(newDuration, getCurrentPhaseDef().maxDuration - actDuration);
    return newDuration;
}


SUMOReal
MSActuatedTrafficLightLogic::gapControl() {
    //intergreen times should not be lenghtend
    assert(myPhases.size() > myStep);
    SUMOReal result = std::numeric_limits<SUMOReal>::max();
    if (!getCurrentPhaseDef().isGreenPhase()) {
        return result; // end current phase
    }

    // Checks, if the maxDuration is kept. No phase should longer send than maxDuration.
    SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - myPhases[myStep]->myLastSwitch;
    if (actDuration >= getCurrentPhaseDef().maxDuration) {
        return result; // end current phase
    }

    // now the gapcontrol starts
    const std::string& state = getCurrentPhaseDef().getState();
    for (unsigned int i = 0; i < (unsigned int) state.size(); i++)  {
        if (state[i] == LINKSTATE_TL_GREEN_MAJOR || state[i] == LINKSTATE_TL_GREEN_MINOR) {
            const std::vector<MSLane*>& lanes = getLanesAt(i);
            for (LaneVector::const_iterator j = lanes.begin(); j != lanes.end(); j++) {
                if (myInductLoops.find(*j) == myInductLoops.end()) {
                    continue;
                }
                SUMOReal actualGap =
                    myInductLoops.find(*j)->second->getTimestepsSinceLastDetection();
                if (actualGap < myMaxGap) {
                    result = MIN2(result, actualGap);
                }
            }
        }
    }
    return result;
}



/****************************************************************************/

