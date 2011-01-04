/****************************************************************************/
/// @file    MSSimpleTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A fixed traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
MSSimpleTrafficLightLogic::MSSimpleTrafficLightLogic(MSTLLogicControl &tlcontrol,
        const std::string &id, const std::string &subid, const Phases &phases,
        unsigned int step, SUMOTime delay) throw()
        : MSTrafficLightLogic(tlcontrol, id, subid, delay), myPhases(phases),
        myStep(step) {
    for (size_t i=0; i<myPhases.size(); i++) {
        myDefaultCycleTime += myPhases[i]->duration;
    }
}


MSSimpleTrafficLightLogic::~MSSimpleTrafficLightLogic() throw() {
    for (size_t i=0; i<myPhases.size(); i++) {
        delete myPhases[i];
    }
}


// ------------ Switching and setting current rows
SUMOTime
MSSimpleTrafficLightLogic::trySwitch(bool) throw() {
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


bool
MSSimpleTrafficLightLogic::setTrafficLightSignals(SUMOTime t) const throw() {
    // get the current traffic light signal combination
    const std::string &state = myPhases[myStep]->getState();
    // go through the links
    for (size_t i=0; i<myLinks.size(); i++) {
        const LinkVector &currGroup = myLinks[i];
        MSLink::LinkState ls = (MSLink::LinkState) state[i];
        for (LinkVector::const_iterator j=currGroup.begin(); j!=currGroup.end(); j++) {
            (*j)->setTLState(ls, t);
        }
    }
    return true;
}


// ------------ Static Information Retrieval
unsigned int
MSSimpleTrafficLightLogic::getPhaseNumber() const throw() {
    return (unsigned int) myPhases.size();
}


const MSSimpleTrafficLightLogic::Phases &
MSSimpleTrafficLightLogic::getPhases() const throw() {
    return myPhases;
}


MSSimpleTrafficLightLogic::Phases &
MSSimpleTrafficLightLogic::getPhases() throw() {
    return myPhases;
}


const MSPhaseDefinition &
MSSimpleTrafficLightLogic::getPhase(unsigned int givenStep) const throw() {
    assert(myPhases.size()>givenStep);
    return *myPhases[givenStep];
}


// ------------ Dynamic Information Retrieval
unsigned int
MSSimpleTrafficLightLogic::getCurrentPhaseIndex() const throw() {
    return myStep;
}


const MSPhaseDefinition &
MSSimpleTrafficLightLogic::getCurrentPhaseDef() const throw() {
    return *myPhases[myStep];
}


// ------------ Conversion between time and phase
SUMOTime
MSSimpleTrafficLightLogic::getPhaseIndexAtTime(SUMOTime simStep) const throw() {
    SUMOTime position = 0;
    if (myStep > 0)	{
        for (unsigned int i=0; i < myStep; i++) {
            position = position + getPhase(i).duration;
        }
    }
    position = position + simStep - getPhase(myStep).myLastSwitch;
    position = position % myDefaultCycleTime;
    assert(position <= myDefaultCycleTime);
    return position;
}


SUMOTime
MSSimpleTrafficLightLogic::getOffsetFromIndex(unsigned int index) const throw() {
    assert(index < myPhases.size());
    if (index == 0) {
        return 0;
    }
    unsigned int pos = 0;
    for (unsigned int i=0; i < index; i++)	{
        pos += getPhase(i).duration;
    }
    return pos;
}


unsigned int
MSSimpleTrafficLightLogic::getIndexFromOffset(SUMOTime offset) const throw() {
    assert(offset <= myDefaultCycleTime);
    if (offset == myDefaultCycleTime) {
        return 0;
    }
    SUMOTime testPos = 0;
    for (unsigned int i=0; i < myPhases.size(); i++)	{
        testPos = testPos + getPhase(i).duration;
        if (testPos > offset) {
            return i;
        }
        if (testPos == offset) {
            assert(myPhases.size() > (i+1));
            return (i+1);
        }
    }
    return 0;
}


// ------------ Changing phases and phase durations
void
MSSimpleTrafficLightLogic::changeStepAndDuration(MSTLLogicControl &tlcontrol,
        SUMOTime simStep, unsigned int step, SUMOTime stepDuration) throw() {
    mySwitchCommand->deschedule(this);
    mySwitchCommand = new SwitchCommand(tlcontrol, this, stepDuration+simStep);
    myStep = step;
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        mySwitchCommand, stepDuration+simStep,
        MSEventControl::ADAPT_AFTER_EXECUTION);
}


/****************************************************************************/

