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
// Revision 1.2  2003/03/17 14:12:19  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/03 14:56:18  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
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
    _allRed(false), _step(step), _phases(phases)
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
    assert(_phases.size()>_step);
    return _phases[_step].duration;
}




template< class _TInductLoop, class _TLaneState > MSNet::Time
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::nextPhase(MSLogicJunction::InLaneCont &inLanes)
{
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
    double laneStateDetectorPosition = 0; // 0m from the end (at the beginning)
    double laneStateDetectorLength = 0; // 0m from the end (at the beginning)
    MSNet::Time laneStateDetectorInterval = 1; //

    std::vector<MSLane*>::const_iterator i;
    // build the induct loops
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        double length = lane->length();
        // check whether the lane is long enough
        double ilpos = length - inductLoopPosition;
        if(ilpos<0) {
            ilpos = 0;
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
        // check whether the position and the length are ok
        //  (not longer than the lane)
        double lspos = length - laneStateDetectorPosition - laneStateDetectorLength;
        if(lspos<0) {
            lspos = 0;
        }
        double lslen = lspos + laneStateDetectorLength;
        if(lslen>length) {
            lslen = length - lspos;
        }
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
size_t
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::nextStep()
{
    // increment the index to the current phase
    if(!_allRed) {
        _allRed = true;
        return _step;
    }
    _allRed = false;
    _step++;
    if(_step==_phases.size()) {
        _step = 0;
    }
    return _step;
}


template< class _TInductLoop, class _TLaneState >
MSNet::DetectorCont
MSActuatedTrafficLightLogic<_TInductLoop, _TLaneState>::getDetectorList() const
{
    MSNet::DetectorCont ret;
    for(InductLoopMap::const_iterator i=myInductLoops.begin(); i!=myInductLoops.end(); i++) {
        ret.push_back((*i).second);
    }
    for(LaneStateMap::const_iterator j=myLaneStates.begin(); j!=myLaneStates.end(); j++) {
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


