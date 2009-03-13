/****************************************************************************/
/// @file    MSUnboundActuatedTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The basic traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <microsim/MSLaneState.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "MSUnboundActuatedTrafficLightLogic.h"
#include <microsim/MSLane.h>
#include <netload/NLDetectorBuilder.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSUnboundActuatedTrafficLightLogic::MSUnboundActuatedTrafficLightLogic(
    const std::string &id,
    const Phases &phases,
    size_t step, size_t delay,
    SUMOReal maxGap, SUMOReal passingTime, SUMOReal detectorGap)
        : MSSimpleTrafficLightLogic(id, phases, step, delay),
        myContinue(false),
        myMaxGap(maxGap), myPassingTime(passingTime), myDetectorGap(detectorGap) {}


void
MSUnboundActuatedTrafficLightLogic::init(NLDetectorBuilder &nb,
        const std::vector<MSLane*> &lanes,
        std::map<std::string, std::vector<std::string> > &laneContinuations,
        SUMOReal det_offset) {
    // change values for setting the loops and lanestate-detectors, here
    SUMOTime inductLoopInterval = 1; //
    // as the laneStateDetector shall end at the end of the lane, the position
    // is calculated, not given
    SUMOTime laneStateDetectorInterval = 1; //

    std::vector<MSLane*>::const_iterator i;
    // build the induct loops
    for (i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        SUMOReal length = lane->length();
        SUMOReal speed = lane->maxSpeed();
        SUMOReal inductLoopPosition = myDetectorGap * speed;
        // check whether the lane is long enough
        SUMOReal ilpos = length - inductLoopPosition;
        if (ilpos<0) {
            ilpos = 0;
        }
        // Build the induct loop and set it into the container
        std::string id = "TLS" + myId + "_InductLoopOn_" + lane->getID();
        if (myInductLoops.find(lane)==myInductLoops.end()) {
            myInductLoops[lane] =
                nb.createInductLoop(id, lane, ilpos, inductLoopInterval);
        }
    }
    // build the lane state-detectors
    for (i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        SUMOReal length = lane->length();
        // check whether the position is o.k. (not longer than the lane)
        SUMOReal lslen = det_offset;
        if (lslen>length) {
            lslen = length;
        }
        SUMOReal lspos = length - lslen;
        // Build the lane state detetcor and set it into the container
        std::string id = "TLS" + myId + "_LaneStateOff_" + lane->getID();
        if (myLaneStates.find(lane)==myLaneStates.end()) {
            MSLaneState* loop =
                new MSLaneState(id, lane, lspos, lslen,
                                laneStateDetectorInterval);
            myLaneStates[lane] = loop;
        }
    }
}



MSUnboundActuatedTrafficLightLogic::~MSUnboundActuatedTrafficLightLogic() {}


SUMOTime
MSUnboundActuatedTrafficLightLogic::duration() const {
    if (myContinue) {
        return 1;
    }
    assert(myPhases.size()>myStep);
    if (!isGreenPhase()) {
        return currentPhaseDef()->duration;
    }
    // define the duration depending from the number of waiting vehicles of the actual phase
    int newduration = currentPhaseDef()->minDuration;
    const std::bitset<64> &isgreen = currentPhaseDef()->getDriveMask();
    for (size_t i=0; i<isgreen.size(); i++)  {
        if (isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end(); j++) {
                LaneStateMap::const_iterator k = myLaneStates.find(*j);
                SUMOReal waiting = (*k).second->getCurrentNumberOfWaiting();
                SUMOReal tmpdur =  myPassingTime * waiting;
                if (tmpdur > newduration) {
                    // here we cut the decimal places, because we have to return an integer
                    newduration = (int) tmpdur;
                }
                if (newduration > (int) currentPhaseDef()->maxDuration)  {
                    return currentPhaseDef()->maxDuration;
                }
            }
        }
    }
    return newduration;
}


SUMOTime
MSUnboundActuatedTrafficLightLogic::trySwitch(bool) {
    // checks if the actual phase should be continued
    gapControl();
    if (myContinue) {
        return duration();
    }
    // increment the index to the current phase
    myStep++;
    assert(myStep<=myPhases.size());
    if (myStep==myPhases.size()) {
        myStep = 0;
    }
    //stores the time the phase started
    static_cast<MSActuatedPhaseDefinition*>(myPhases[myStep])->myLastSwitch =
        MSNet::getInstance()->getCurrentTimeStep();
    // set the next event
    return duration();
}


bool
MSUnboundActuatedTrafficLightLogic::isGreenPhase() const {
    if (currentPhaseDef()->getDriveMask().none()) {
        return false;
    }
    if (currentPhaseDef()->getYellowMask().any()) {
        return false;
    }
    return true;
}


bool
MSUnboundActuatedTrafficLightLogic::gapControl() {
    //intergreen times should not be lenghtend
    assert(myPhases.size()>myStep);
    if (!isGreenPhase()) {
        return myContinue = false;
    }

    // Checks, if the maxDuration is kept. No phase should longer send than maxDuration.
    SUMOTime actDuration =
        MSNet::getInstance()->getCurrentTimeStep() - static_cast<MSActuatedPhaseDefinition*>(myPhases[myStep])->myLastSwitch;
    if (actDuration >= currentPhaseDef()->maxDuration) {
        return myContinue = false;
    }

    // now the gapcontrol starts
    const std::bitset<64> &isgreen = currentPhaseDef()->getDriveMask();
    for (size_t i=0; i<isgreen.size(); i++)  {
        if (isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end(); j++) {
                if (myInductLoops.find(*j)==myInductLoops.end()) {
                    continue;
                }
                SUMOReal actualGap =
                    myInductLoops.find(*j)->second->getTimestepsSinceLastDetection();
                if (actualGap < myMaxGap) {
                    return myContinue = true;
                }
            }
        }
    }
    return myContinue = false;
}


MSActuatedPhaseDefinition *
MSUnboundActuatedTrafficLightLogic::currentPhaseDef() const {
    assert(myPhases.size()>myStep);
    return static_cast<MSActuatedPhaseDefinition*>(myPhases[myStep]);
}



/****************************************************************************/

