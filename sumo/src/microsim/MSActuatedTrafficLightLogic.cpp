//---------------------------------------------------------------------------//
//                        MSActuatedTrafficLightLogic.cpp -
//  The basic traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2003/05/21 15:15:41  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.5  2003/04/04 15:26:55  roessel
// Added the keyword "typename" for derived types in for-loops
//
// Revision 1.4  2003/04/02 11:44:02  dkrajzew
// continuation of implementation of actuated traffic lights
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <utility>
#include <vector>
#include <bitset>
#include "MSEventControl.h"
#include "MSDetector.h"
#include "MSInductLoop.h"
#include "MSLaneState.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"
#include "MSActuatedTrafficLightLogic.h"


template< class _TInductLoop, class _TLaneState >
std::bitset<64> MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::_allClear;



template< class _TInductLoop, class _TLaneState >
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>(
    const std::string &id, const ActuatedPhases &phases, size_t step,
    const std::vector<MSLane*> &lanes)
    : MSTrafficLightLogic(id),
    _allRed(false), _step(step), _phases(phases), _continue(false)
{
    sproutDetectors(lanes);
}


template< class _TInductLoop, class _TLaneState >
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::~MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>()
{
}


template< class _TInductLoop, class _TLaneState > MSNet::Time
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::duration() const
{
    if(_allRed) {
        return 20;
    }
    if(_continue) {
        return 1;
    }
    assert(_phases.size()>_step);

    // define the duration depending from the number of waiting vehicles of the actual phase

    int duration = _phases[_step].minDuration;
    for(typename LaneStateMap::const_iterator i=myLaneStates.begin(); i!=myLaneStates.end(); i++)   {
        MSLane *lane = (*i).first;
        const MSLinkCont &cont = lane->getLinkCont();
        for(MSLinkCont::const_iterator j=cont.begin(); j!=cont.end(); j++)  {
            if((*j)->myPrio)    {
                double waiting = (*i).second->numberOfWaiting();
                double passingTime = 1.9;
                double tmpdur =  passingTime * waiting;
                if (tmpdur > duration) {
                    // here we cut the decimal places, because we have to return an integer
                    duration = tmpdur;
                }
                if (duration > _phases[_step].maxDuration)  {
                    return _phases[_step].maxDuration;
                }
            }
        }
    }

    return duration;
}




template< class _TInductLoop, class _TLaneState > MSNet::Time
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::nextPhase(MSLogicJunction::InLaneCont &inLanes)
{
    // checks if the actual phase should be continued
    gapControl();
    if(_continue) {
        return duration();
    }
    // increment the index to the current phase
    nextStep();
    // reset the link priorities
    setLinkPriorities(inLanes);
    // set the next event
    return duration();
}



template< class _TInductLoop, class _TLaneState >
void
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::sproutDetectors(const std::vector<MSLane*> &lanes)
{
    // change values for setting the loops and lanestate-detectors, here
    double inductLoopPosition = 10; // 10m from the end
    MSNet::Time inductLoopInterval = 1; //
    double laneStateDetectorLength = 100; // length of the detecor
    // as the laneStateDetector shall end at the end of the lane, the position is calculated, not given
    MSNet::Time laneStateDetectorInterval = 1; //

    std::vector<MSLane*>::const_iterator i;
    // build the induct loops
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        double length = lane->length();
        double speed = lane->maxSpeed();
        double gap = 2.5;
        inductLoopPosition = gap * speed;
        // check whether the lane is long enough
        double ilpos = length - inductLoopPosition;
        if(ilpos<0) {
            ilpos = length;
        }
        // Build the induct loop and set it into the container
        _TInductLoop *loop = new _TInductLoop( "", lane, ilpos,
            inductLoopInterval, MSDetector::CSV, 0, true);
        myInductLoops[lane] = loop;
    }
    // build the lane state-detectors
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        double length = lane->length();
        // check whether the position is o.k. (not longer than the lane)
        double lslen = laneStateDetectorLength;
        if(lslen>length) {
            lslen = length;
        }
        double lspos = length - lslen;
        // Build the lane state detetcor and set it into the container
        _TLaneState *loop = new _TLaneState( "", lane, lspos,
            lslen, laneStateDetectorInterval, MSDetector::CSV, 0);
        myLaneStates[lane] = loop;
    }
}




template< class _TInductLoop, class _TLaneState >
void
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::applyPhase(MSLogicJunction::Request &request) const
{
    if(_allRed) {
        request = std::bitset<64>(0);
    } else {
        assert(_phases.size()>_step);
        std::bitset<64> allowed = _phases[_step].driveMask;
        for(size_t i=0; i<request.size(); i++) {
            request[i] = request[i] & allowed.test(i);
        }
    }
}


template< class _TInductLoop, class _TLaneState >
const std::bitset<64> &
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::linkPriorities() const
{
    if(_allRed) {
        return _allClear;
    } else {
        assert(_phases.size()>_step);
        return _phases[_step].breakMask;
    }
}


template< class _TInductLoop, class _TLaneState >
const std::bitset<64> &
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::yellowMask() const
{
    if(_allRed) {
        return _allClear;
    } else {
        assert(_phases.size()>_step);
        return _phases[_step].yellowMask;
    }
}





template< class _TInductLoop, class _TLaneState >
size_t
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::nextStep()
{
    // increment the index to the current phase
    if(!_allRed) {
        _allRed = true;
        MSNet::Time phaseStart = _phases[_step]._lastSwitch;
        MSNet::Time lastDuration = MSNet::globaltime - phaseStart;
        _phases[_step].duration = lastDuration;
        return _step;
    }
    _allRed = false;
    _step++;
    if(_step==_phases.size()) {
        _step = 0;
    }
    // resets the _lastSwitch
    _phases[_step]._lastSwitch = MSNet::globaltime;
    return _step;
}

template< class _TInductLoop, class _TLaneState >
bool
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::gapControl()
{

    // Checks, if the _allRed-Phase is the actual phase. Of course this phase should not be expanded
    if(_allRed) {
        return _continue = false;
    }

    // Checks, if the maxDuration is kept. No phase should be longer send than maxDuration.
    MSNet::Time actDuration = MSNet::globaltime - _phases[_step]._lastSwitch;
    if (actDuration >= _phases[_step].maxDuration) {
        return _continue = false;
    }

    // now the gapcontrol starts
    for(typename InductLoopMap::const_iterator i=myInductLoops.begin(); i!=myInductLoops.end(); i++)   {
        MSLane *lane = (*i).first;
        const MSLinkCont &cont = lane->getLinkCont();
        for(MSLinkCont::const_iterator j=cont.begin(); j!=cont.end(); j++)  {
            if((*j)->myPrio)    {
                double actualGap = (*i).second->getGap();
                double maxGap = 3.1;
                if (actualGap < maxGap) {
                return _continue = true;
                }
            }
        }
    }

    return _continue = false;
}


template< class _TInductLoop, class _TLaneState >
MSNet::DetectorCont
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::getDetectorList() const
{
    MSNet::DetectorCont ret;
    for(typename InductLoopMap::const_iterator i=myInductLoops.begin(); i!=myInductLoops.end(); i++) {
        ret.push_back((*i).second);
    }
    for(typename LaneStateMap::const_iterator j=myLaneStates.begin(); j!=myLaneStates.end(); j++) {
        ret.push_back((*j).second);
    }
    return ret;
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSActuatedTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


