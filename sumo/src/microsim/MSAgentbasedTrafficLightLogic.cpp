//---------------------------------------------------------------------------//
//                        MSAgentbasedTrafficLightLogic.cpp -
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 01. Oct 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.2  2003/10/02 14:58:26  dkrajzew
// methods needed for visualisation added
//
// Revision 1.1  2003/10/01 11:24:35  dkrajzew
// agent-based traffic lights added
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
//#include "MSDetector.h"
#include "MSInductLoop.h"
#include "MSLaneState.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"
#include "MSAgentbasedTrafficLightLogic.h"


template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>(
        const std::string &id, const Phases &phases, size_t step,
        const std::vector<MSLane*> &lanes, size_t delay)
    : MSSimpleTrafficLightLogic(id, phases, step, delay),
    _continue(false)
{
    sproutDetectors(lanes);
}


template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::~MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>()
{
}


template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector > MSNet::Time
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::duration() const
{
    if(_continue) {
        return 1;
    }
    assert(_phases.size()>_step);

    // define the duration depending from the number of waiting vehicles of the actual phase

    int duration = static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->minDuration;
    for(typename LaneStateMap::const_iterator i=myLaneStates.begin(); i!=myLaneStates.end(); i++)   {
        MSLane *lane = (*i).first;
        const MSLinkCont &cont = lane->getLinkCont();
        for(MSLinkCont::const_iterator j=cont.begin(); j!=cont.end(); j++)  {
            if((*j)->myPrio)    {
                double waiting = (*i).second->getCurrentNumberOfWaiting();
                double passingTime = 1.9;
                double tmpdur =  passingTime * waiting;
                if (tmpdur > duration) {
                    // here we cut the decimal places, because we have to return an integer
                    duration = (int) tmpdur;
                }
                if (duration > (int) static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->maxDuration)  {
                    return static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->maxDuration;
                }
            }
        }
    }

    return duration;
}




template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector > MSNet::Time
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::nextPhase()
{
    // checks if the actual phase should be continued
    gapControl();
    if(_continue) {
        return duration();
    }
    // increment the index to the current phase
    nextStep();
    // reset the link priorities
    setLinkPriorities();
    // set the next event
    return duration();
}



template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
void
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::sproutDetectors(
                                const std::vector<MSLane*> &lanes)
{
    // change values for setting the loops and lanestate-detectors, here
    double inductLoopPosition = 10; // 10m from the end
    MSNet::Time inductLoopInterval = 1; //
    double laneStateDetectorLength = 100; // length of the detecor
    // as the laneStateDetector shall end at the end of the lane, the position
    // is calculated, not given
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
        std::string id = "TL" + _id + "_InductLoopOn_" + lane->id();
        _TInductLoop *loop = new _TInductLoop(id, lane, ilpos );
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
        std::string id = "TL" + _id + "_LaneStateOf_" + lane->id();
        MSLaneState* loop =
            new MSLaneState( id, lane, lspos, lslen );
        myLaneStates[lane] = loop;
    }
    // build the E2-detectors
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        double length = lane->length() - 0.1; // !!!
        // check whether the position is o.k. (not longer than the lane)
        double lslen = laneStateDetectorLength;
        if(lslen>length) {
            lslen = length;
        }
        double lspos = length - lslen;
        // Build the lane state detetcor and set it into the container
        std::string id = "TL" + _id + "_E2DetectorOn_" + lane->id();
        _TE2_ZS_Collector* det =
            new _TE2_ZS_Collector( id, lane, lspos, lslen );
		det->addDetector( MS_E2_ZS_Collector::ALL, "detectors" );
        myE2Detectors[lane] = det;
    }
}



/*
template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
const std::bitset<64> &
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::linkPriorities() const
{
    assert(_phases.size()>_step);
    return _phases[_step].breakMask;
}


template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
const std::bitset<64> &
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::yellowMask() const
{
    assert(_phases.size()>_step);
    return _phases[_step].yellowMask;
}


template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
const std::bitset<64> &
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::allowed() const
{
    assert(_phases.size()>_step);
    return _phases[_step].driveMask;
}
*/



template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
size_t
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::nextStep()
{
    // increment the index to the current phase
    MSNet::Time phaseStart = static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->_lastSwitch;
    MSNet::Time lastDuration = MSNet::globaltime - phaseStart;
    static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->duration = lastDuration;
    return _step;
}

template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
bool
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::gapControl()
{
    // Checks, if the maxDuration is kept. No phase should be longer send than maxDuration.
    MSNet::Time actDuration = MSNet::globaltime - static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->_lastSwitch;
    if (actDuration >= currentPhaseDef()->maxDuration) {
        return _continue = false;
    }

    // now the gapcontrol starts
    for(typename InductLoopMap::const_iterator i=myInductLoops.begin(); i!=myInductLoops.end(); i++)   {
        MSLane *lane = (*i).first;
        const MSLinkCont &cont = lane->getLinkCont();
        for(MSLinkCont::const_iterator j=cont.begin(); j!=cont.end(); j++)  {
            if((*j)->myPrio)    {
                double actualGap =
                    (*i).second->getTimestepsSinceLastDetection();
                double maxGap = 3.1;
                if (actualGap < maxGap) {
                return _continue = true;
                }
            }
        }
    }
    return _continue = false;
}


// template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
// MSNet::DetectorCont
// MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::getDetectorList() const
// {
//     MSNet::DetectorCont ret;
//     for(typename InductLoopMap::const_iterator i=myInductLoops.begin(); i!=myInductLoops.end(); i++) {
//         ret.push_back((*i).second);
//     }
// //     for(typename LaneStateMap::const_iterator j=myLaneStates.begin(); j!=myLaneStates.end(); j++) {
// //         ret.push_back((*j).second);
// //     }
//     return ret;
// }



template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
MSActuatedPhaseDefinition *
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::currentPhaseDef()
{
    return static_cast<MSActuatedPhaseDefinition*>(_phases[_step]);
}


template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
double
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::currentForLane(
		MS_E2_ZS_Collector::DetType what, MSLane *lane) const
{
	E2DetectorMap::const_iterator i=myE2Detectors.find(lane);
	return (*i).second->getGurrent(what);
}


template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
double
MSAgentbasedTrafficLightLogic<_TInductLoop, _TLaneState, _TE2_ZS_Collector>::currentForLane(
		MS_E2_ZS_Collector::DetType what, MSUnit::Seconds lanstNSeconds,
		MSLane *lane) const
{
	E2DetectorMap::const_iterator i=myE2Detectors.find(lane);
	return (*i).second->getAggregate(what, lanstNSeconds);
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSAgentbasedTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


