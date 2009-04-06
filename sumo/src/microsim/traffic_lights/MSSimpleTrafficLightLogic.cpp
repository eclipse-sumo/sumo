/****************************************************************************/
/// @file    MSSimpleTrafficLightLogic.cpp
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

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <sstream>
#include <microsim/MSEventControl.h>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSSimpleTrafficLightLogic::MSSimpleTrafficLightLogic(MSNet& /*net*/,
        MSTLLogicControl &tlcontrol,
        const std::string &id,
        const std::string &subid,
        const Phases &phases,
        size_t step,
        SUMOTime delay)
        : MSTrafficLightLogic(tlcontrol, id, subid, delay), myPhases(phases),
        myStep(step), myCycleTime(0) {
    myCycleTime=getCycleTime();
}


MSSimpleTrafficLightLogic::~MSSimpleTrafficLightLogic() {
    for (size_t i=0; i<myPhases.size(); i++) {
        delete myPhases[i];
    }
}


SUMOTime
MSSimpleTrafficLightLogic::trySwitch(bool) {
    // check whether the current duration shall be increased
    if (myCurrentDurationIncrement>0) {
        SUMOTime delay = myCurrentDurationIncrement;
        myCurrentDurationIncrement = 0;
        return delay;
    }

    // increment the index
    myStep++;
    // if the last phase was reached ...
    if (myStep==myPhases.size()) {
        // ... set the index to the first phase
        myStep = 0;
    }
    assert(myPhases.size()>myStep);
    //stores the time the phase started
    myPhases[myStep]->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
    // check whether the next duration was overridden
    if (myOverridingTimes.size()>0) {
        SUMOTime nextDuration = myOverridingTimes[0];
        myOverridingTimes.erase(myOverridingTimes.begin());
        return nextDuration;
    }
    // return offset to the next switch
    return myPhases[myStep]->duration;
}


unsigned int
MSSimpleTrafficLightLogic::getPhaseNumber() const {
    return (unsigned int) myPhases.size();
}


size_t
MSSimpleTrafficLightLogic::getCurrentPhaseIndex() const {
    return myStep;
}


void
MSSimpleTrafficLightLogic::setLinkPriorities() const {
    const std::bitset<64> &linkPrios = myPhases[myStep]->getBreakMask();
    for (size_t i=0; i<myLinks.size(); i++) {
        const LinkVector &currGroup = myLinks[i];
        for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
            (*j)->setPriority(linkPrios.test(i));
        }
    }
}


bool
MSSimpleTrafficLightLogic::maskRedLinks() const {
    // get the current traffic light signal combination
    const std::bitset<64> &allowedLinks = myPhases[myStep]->getDriveMask();
    const std::bitset<64> &yellowLinks = myPhases[myStep]->getYellowMask();
    // go through the links
    for (size_t i=0; i<myLinks.size(); i++) {
        // mark out links having red
        if (!allowedLinks.test(i)&&!yellowLinks.test(i)) {
            const LinkVector &currGroup = myLinks[i];
            for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                (*j)->deleteRequest();
            }
        }
        // set the states for assigned links
        // !!! one should let the links ask for it
        if (!allowedLinks.test(i)) {
            if (yellowLinks.test(i)) {
                const LinkVector &currGroup = myLinks[i];
                for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    (*j)->setTLState(MSLink::LINKSTATE_TL_YELLOW);
                }
            } else {
                const LinkVector &currGroup = myLinks[i];
                for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                    (*j)->setTLState(MSLink::LINKSTATE_TL_RED);
                }
            }
        } else {
            const LinkVector &currGroup = myLinks[i];
            for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                (*j)->setTLState(MSLink::LINKSTATE_TL_GREEN);
            }
        }
    }
    return true;
}


bool
MSSimpleTrafficLightLogic::maskYellowLinks() const {
    // get the current traffic light signal combination
    const std::bitset<64> &allowedLinks = myPhases[myStep]->getDriveMask();
    // go through the links
    for (size_t i=0; i<myLinks.size(); i++) {
        // mark out links having red
        if (!allowedLinks.test(i)) {
            const LinkVector &currGroup = myLinks[i];
            for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
                (*j)->deleteRequest();
            }
        }
    }
    return true;
}


MSPhaseDefinition
MSSimpleTrafficLightLogic::getCurrentPhaseDef() const {
    return *myPhases[myStep];
}


size_t
MSSimpleTrafficLightLogic::getCycleTime() {
    myCycleTime = 0;
    for (size_t i=0; i<myPhases.size(); i++) {
        myCycleTime = myCycleTime + myPhases[i]->duration;
    }
    return myCycleTime;
}


size_t
MSSimpleTrafficLightLogic::getPosition(SUMOTime simStep) {
    size_t position = 0;
    if (myStep > 0)	{
        for (size_t i=0; i < myStep; i++) {
            position = position + getPhaseFromStep(i).duration;
        }
    }
    position = position + simStep - getPhaseFromStep(myStep).myLastSwitch;
    position = position % myCycleTime;
    assert(position <= myCycleTime);
    return position;
}

unsigned int
MSSimpleTrafficLightLogic::getStepFromPos(unsigned int position) {
    assert(position <= myCycleTime);
    if (position == myCycleTime) {
        return 0;
    }
    unsigned int pos = position;
    unsigned int testPos = 0;
    for (unsigned int i=0; i < myPhases.size(); i++)	{
        testPos = testPos + getPhaseFromStep(i).duration;
        if (testPos > pos) {
            return i;
        }
        if (testPos == pos) {
            assert(myPhases.size() > (i+1));
            return (i+1);
        }
    }
    return 0;
}


unsigned int
MSSimpleTrafficLightLogic::getPosFromStep(unsigned int step) {
    assert(step < myPhases.size());
    if (step == 0) {
        return 0;
    }
    unsigned int pos = 0;
    for (size_t i=0; i < step; i++)	{
        pos += getPhaseFromStep(i).duration;
    }
    return pos;
}


const MSSimpleTrafficLightLogic::Phases &
MSSimpleTrafficLightLogic::getPhases() const {
    return myPhases;
}


MSSimpleTrafficLightLogic::Phases &
MSSimpleTrafficLightLogic::getPhases() {
    return myPhases;
}


const MSPhaseDefinition &
MSSimpleTrafficLightLogic::getPhaseFromStep(size_t givenStep) const {
    assert(myPhases.size()>givenStep);
    return *myPhases[givenStep];
}


void
MSSimpleTrafficLightLogic::changeStepAndDuration(MSTLLogicControl &tlcontrol,
        SUMOTime simStep,
        unsigned int step,
        SUMOTime stepDuration) {
    mySwitchCommand->deschedule(this);
    mySwitchCommand = new SwitchCommand(tlcontrol, this);
    myStep = step;
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        mySwitchCommand, stepDuration+simStep,
        MSEventControl::ADAPT_AFTER_EXECUTION);
}


std::string
MSSimpleTrafficLightLogic::buildStateList() const {
    MSPhaseDefinition curr = getCurrentPhaseDef();
    std::ostringstream strm;
    const std::bitset<64> &allowedLinks = curr.getDriveMask();
    const std::bitset<64> &yellowLinks = curr.getYellowMask();
    for (size_t i=0; i<myLinks.size(); i++) {
        if (yellowLinks.test(i)) {
            strm << 'Y';
        } else {
            if (allowedLinks.test(i)) {
                strm << 'G';
            } else {
                strm << 'R';
            }
        }
    }
    return strm.str();
}



/****************************************************************************/

