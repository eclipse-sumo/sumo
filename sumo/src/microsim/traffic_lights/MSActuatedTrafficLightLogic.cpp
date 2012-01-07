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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
// method definitions
// ===========================================================================
MSActuatedTrafficLightLogic::MSActuatedTrafficLightLogic(MSTLLogicControl& tlcontrol,
        const std::string& id, const std::string& programID,
        const Phases& phases,
        unsigned int step, SUMOTime delay, const std::map<std::string, std::string> &parameter)
    : MSSimpleTrafficLightLogic(tlcontrol, id, programID, phases, step, delay),
      myContinue(false) {
    myMaxGap = SUMOReal(3.1);
    if (parameter.find("max-gap") != parameter.end()) {
        myMaxGap = TplConvert<char>::_2SUMOReal(parameter.find("max-gap")->second.c_str());
    }
    myPassingTime = SUMOReal(1.9);
    if (parameter.find("passing-time") != parameter.end()) {
        myPassingTime = TplConvert<char>::_2SUMOReal(parameter.find("passing-time")->second.c_str());
    }
    myDetectorGap = SUMOReal(3.0);
    if (parameter.find("detector-gap") != parameter.end()) {
        myDetectorGap = TplConvert<char>::_2SUMOReal(parameter.find("detector-gap")->second.c_str());
    }
}


void
MSActuatedTrafficLightLogic::init(NLDetectorBuilder& nb) {
    SUMOReal det_offset = TplConvert<char>::_2SUMOReal(myParameter.find("detector_offset")->second.c_str());
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
            SUMOReal speed = lane->getMaxSpeed();
            SUMOReal inductLoopPosition = myDetectorGap * speed;
            // check whether the lane is long enough
            SUMOReal ilpos = length - inductLoopPosition;
            if (ilpos < 0) {
                ilpos = 0;
            }
            // Build the induct loop and set it into the container
            std::string id = "TLS" + myID + "_" + myProgramID + "_InductLoopOn_" + lane->getID();
            if (myInductLoops.find(lane) == myInductLoops.end()) {
                myInductLoops[lane] = static_cast<MSInductLoop*>(nb.createInductLoop(id, lane, ilpos, false));
            }
        }
        // build the lane state-detectors
        for (i = lanes.begin(); i != lanes.end(); i++) {
            MSLane* lane = (*i);
            SUMOReal length = lane->getLength();
            // check whether the position is o.k. (not longer than the lane)
            SUMOReal lslen = det_offset;
            if (lslen > length) {
                lslen = length;
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
    gapControl();
    if (myContinue) {
        return duration();
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
    return duration();
}


// ------------ "actuated" algorithm methods
SUMOTime
MSActuatedTrafficLightLogic::duration() const {
    if (myContinue) {
        return 1;
    }
    assert(myPhases.size() > myStep);
    if (!getCurrentPhaseDef().isGreenPhase()) {
        return getCurrentPhaseDef().duration;
    }
    // define the duration depending from the number of waiting vehicles of the actual phase
    int newduration = (int) getCurrentPhaseDef().minDuration;
    const std::string& state = getCurrentPhaseDef().getState();
    for (unsigned int i = 0; i < (unsigned int) state.size(); i++) {
        if (state[i] == LINKSTATE_TL_GREEN_MAJOR || state[i] == LINKSTATE_TL_GREEN_MINOR) {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty()) {
                break;
            }
            for (LaneVector::const_iterator j = lanes.begin(); j != lanes.end(); j++) {
                InductLoopMap::const_iterator k = myInductLoops.find(*j);
                SUMOReal waiting = (SUMOReal)(*k).second->getCurrentPassedNumber();
                SUMOReal tmpdur =  myPassingTime * waiting;
                if (tmpdur > newduration) {
                    // here we cut the decimal places, because we have to return an integer
                    newduration = (int) tmpdur;
                }
                if (newduration > (int) getCurrentPhaseDef().maxDuration)  {
                    return getCurrentPhaseDef().maxDuration;
                }
            }
        }
    }
    return newduration;
}


void
MSActuatedTrafficLightLogic::gapControl() {
    //intergreen times should not be lenghtend
    assert(myPhases.size() > myStep);
    if (!getCurrentPhaseDef().isGreenPhase()) {
        myContinue = false;
        return;
    }

    // Checks, if the maxDuration is kept. No phase should longer send than maxDuration.
    SUMOTime actDuration = MSNet::getInstance()->getCurrentTimeStep() - myPhases[myStep]->myLastSwitch;
    if (actDuration >= getCurrentPhaseDef().maxDuration) {
        myContinue = false;
        return;
    }

    // now the gapcontrol starts
    const std::string& state = getCurrentPhaseDef().getState();
    for (unsigned int i = 0; i < (unsigned int) state.size(); i++)  {
        if (state[i] == LINKSTATE_TL_GREEN_MAJOR || state[i] == LINKSTATE_TL_GREEN_MINOR) {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            for (LaneVector::const_iterator j = lanes.begin(); j != lanes.end(); j++) {
                if (myInductLoops.find(*j) == myInductLoops.end()) {
                    continue;
                }
                SUMOReal actualGap =
                    myInductLoops.find(*j)->second->getTimestepsSinceLastDetection();
                if (actualGap < myMaxGap) {
                    myContinue = true;
                    return;
                }
            }
        }
    }
    myContinue = false;
}



/****************************************************************************/

