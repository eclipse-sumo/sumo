/****************************************************************************/
/// @file    MSSimpleTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// The basic traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// member method definitions
// ===========================================================================
MSSimpleTrafficLightLogic::MSSimpleTrafficLightLogic(MSNet &net,
        MSTLLogicControl &tlcontrol,
        const std::string &id,
        const std::string &subid,
        const Phases &phases,
        size_t step,
        size_t delay)
        : MSTrafficLightLogic(net, tlcontrol, id, subid, delay), myPhases(phases),
        myStep(step), myCycleTime(0)
{
    myCycleTime=getCycleTime();
}


MSSimpleTrafficLightLogic::~MSSimpleTrafficLightLogic()
{
    for (size_t i=0; i<myPhases.size(); i++) {
        delete myPhases[i];
    }
}


const std::bitset<64> &
MSSimpleTrafficLightLogic::linkPriorities() const
{
    assert(myPhases.size()>myStep);
    return myPhases[myStep]->getBreakMask();
}


const std::bitset<64> &
MSSimpleTrafficLightLogic::yellowMask() const
{
    assert(myPhases.size()>myStep);
    return myPhases[myStep]->getYellowMask();
}


const std::bitset<64> &
MSSimpleTrafficLightLogic::allowed() const
{
    assert(myPhases.size()>myStep);
    return myPhases[myStep]->getDriveMask();
}


SUMOTime
MSSimpleTrafficLightLogic::trySwitch(bool)
{
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
    myPhases[myStep]->_lastSwitch = MSNet::getInstance()->getCurrentTimeStep();
    // check whether the next duration was overridden
    if (myOverridingTimes.size()>0) {
        SUMOTime nextDuration = myOverridingTimes[0];
        myOverridingTimes.erase(myOverridingTimes.begin());
        return nextDuration;
    }
    // return offset to the next switch
    return myPhases[myStep]->duration;
}


size_t
MSSimpleTrafficLightLogic::getStepNo() const
{
    return myStep;
}

size_t
MSSimpleTrafficLightLogic::getCycleTime()
{
    myCycleTime = 0;
    for (size_t i=0; i<myPhases.size(); i++) {
        myCycleTime = myCycleTime + myPhases[i]->duration;
    }
    return myCycleTime;
}

size_t
MSSimpleTrafficLightLogic::getPosition(SUMOTime simStep)
{
    size_t position = 0;
    if (myStep > 0)	{
        for (size_t i=0; i < myStep; i++)	{
            position = position + getPhaseFromStep(i).duration;
        }
    }
    position = position + simStep - getPhaseFromStep(myStep)._lastSwitch;
    assert(position <= myCycleTime);
    return position;
}

size_t
MSSimpleTrafficLightLogic::getStepFromPos(size_t position)
{
    assert(position <= myCycleTime);
    size_t pos = position;
    if (pos == myCycleTime) {
        pos = 0;
    }
    if (pos == 0)	{
        return 0;
    }
    size_t testPos = 0;
    for (size_t i=0; i < myPhases.size(); i++)	{
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

size_t
MSSimpleTrafficLightLogic::getPosFromStep(size_t step)
{
    assert(step < myPhases.size());

    size_t pos = 0;
    size_t myStep = step;

    if (myStep == 0) {
        return 0;
    }

    for (size_t i=0; i < myStep; i++)	{
        pos = pos + getPhaseFromStep(i).duration;
    }
    return pos;
}


const MSSimpleTrafficLightLogic::Phases &
MSSimpleTrafficLightLogic::getPhases() const
{
    return myPhases;
}

const MSPhaseDefinition &
MSSimpleTrafficLightLogic::getPhaseFromStep(size_t givenStep) const
{
    assert(myPhases.size()>givenStep);
    return *myPhases[givenStep];
}


void
MSSimpleTrafficLightLogic::changeStepAndDuration(MSTLLogicControl &tlcontrol,
        SUMOTime simStep,
        int step,
        SUMOTime stepDuration)
{
    mySwitchCommand->deschedule(this);
    mySwitchCommand = new SwitchCommand(tlcontrol, this);
    myStep = step;
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        mySwitchCommand, stepDuration+simStep,
        MSEventControl::ADAPT_AFTER_EXECUTION);
}



/****************************************************************************/

