/****************************************************************************/
/// @file    MSActuatedTrafficLightLogic.cpp
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
MSActuatedTrafficLightLogic::MSActuatedTrafficLightLogic(
    MSNet &net, MSTLLogicControl &tlcontrol,
    const std::string &id, const std::string &subid,
    const Phases &phases,
    size_t step, SUMOTime delay, SUMOReal maxGap, SUMOReal passingTime,
    SUMOReal detectorGap)
        : MSSimpleTrafficLightLogic(net, tlcontrol, id, subid, phases, step, delay),
        myContinue(false),
        myMaxGap(maxGap), myPassingTime(passingTime), myDetectorGap(detectorGap) {}


void
MSActuatedTrafficLightLogic::init(NLDetectorBuilder &nb,
                                  const MSEdgeContinuations &/*edgeContinuations*/) {
    SUMOReal det_offset = TplConvert<char>::_2SUMOReal(myParameter.find("detector_offset")->second.c_str());
    // change values for setting the loops and lanestate-detectors, here
    SUMOTime inductLoopInterval = 1; //
    // as the laneStateDetector shall end at the end of the lane, the position
    // is calculated, not given
    //!!!SUMOTime laneStateDetectorInterval = 1; //

    LaneVectorVector::const_iterator i2;
    LaneVector::const_iterator i;
    // build the induct loops
    for (i2=myLanes.begin(); i2!=myLanes.end(); ++i2) {
        const LaneVector &lanes = *i2;
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
            std::string id = "TLS" + myID + "_" + mySubID + "_InductLoopOn_" + lane->getID();
            if (myInductLoops.find(lane)==myInductLoops.end()) {
                myInductLoops[lane] =
                    nb.createInductLoop(id, lane, ilpos);
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
            /*!!!!
            SUMOReal lspos = length - lslen;
            // Build the lane state detetcor and set it into the container
            std::string id = "TLS" + myID + "_" + mySubID + "_LaneStateOff_" + lane->getID();

            if(myLaneStates.find(lane)==myLaneStates.end()) {
            MSLaneState* loop =
                new MSLaneState( id, lane, lspos, lslen,
                    laneStateDetectorInterval );
            myLaneStates[lane] = loop;
            }
            */
        }
    }
}



MSActuatedTrafficLightLogic::~MSActuatedTrafficLightLogic() {
    {
        for (InductLoopMap::iterator i=myInductLoops.begin(); i!=myInductLoops.end(); ++i) {
            delete(*i).second;
        }
    }
    /*!!!!
    {
        for(LaneStateMap::iterator i=myLaneStates.begin(); i!=myLaneStates.end(); ++i) {
            delete (*i).second;
        }
    }
    */
}


SUMOTime
MSActuatedTrafficLightLogic::duration() const {
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
                InductLoopMap::const_iterator k = myInductLoops.find(*j);
                SUMOReal waiting = (SUMOReal)(*k).second->getNVehContributed();
                SUMOReal tmpdur =  myPassingTime * waiting;
                if (tmpdur > newduration) {
                    // here we cut the decimal places, because we have to return an integer
                    newduration = (int) tmpdur;
                }
                if (newduration > (int) currentPhaseDef()->maxDuration)  {
                    return currentPhaseDef()->maxDuration;
                }

                /*!!!!
                LaneStateMap::const_iterator k = myLaneStates.find(*j);
                SUMOReal waiting =  (SUMOReal) (*k).second->getCurrentNumberOfWaiting();
                SUMOReal tmpdur =  myPassingTime * waiting;
                if (tmpdur > newduration) {
                    // here we cut the decimal places, because we have to return an integer
                    newduration = (int) tmpdur;
                }
                if (newduration > (int) currentPhaseDef()->maxDuration)  {
                    return currentPhaseDef()->maxDuration;
                }
                */
            }
        }
    }
//    currentPhaseDef()->duration = newduration;
    return newduration;
}


SUMOTime
MSActuatedTrafficLightLogic::trySwitch(bool) {
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
MSActuatedTrafficLightLogic::isGreenPhase() const {
    if (currentPhaseDef()->getDriveMask().none()) {
        return false;
    }
    if (currentPhaseDef()->getYellowMask().any()) {
        return false;
    }
    return true;
}


void
MSActuatedTrafficLightLogic::gapControl() {
    //intergreen times should not be lenghtend
    assert(myPhases.size()>myStep);
    if (!isGreenPhase()) {
        myContinue = false;
        return;
    }

    // Checks, if the maxDuration is kept. No phase should longer send than maxDuration.
    SUMOTime actDuration =
        MSNet::getInstance()->getCurrentTimeStep() - static_cast<MSActuatedPhaseDefinition*>(myPhases[myStep])->myLastSwitch;
    if (actDuration >= currentPhaseDef()->maxDuration) {
        myContinue = false;
        return;
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
                    myContinue = true;
                    return;
                }
            }
        }
    }
    myContinue = false;
}


MSActuatedPhaseDefinition *
MSActuatedTrafficLightLogic::currentPhaseDef() const {
    assert(myPhases.size()>myStep);
    return static_cast<MSActuatedPhaseDefinition*>(myPhases[myStep]);
}



/****************************************************************************/

