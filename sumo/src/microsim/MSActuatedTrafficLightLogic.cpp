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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.27  2004/07/02 09:53:58  dkrajzew
// some design things
//
// Revision 1.26  2004/04/02 11:38:28  dkrajzew
// extended traffic lights are now no longer template classes
//
// Revision 1.25  2004/01/26 07:32:46  dkrajzew
// added the possibility to specify the position (actuated-tlls) / length
//  (agentbased-tlls) of used detectors
//
// Revision 1.24  2004/01/12 15:04:16  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.23  2003/11/24 10:21:20  dkrajzew
// some documentation added and dead code removed
//
// Revision 1.22  2003/11/17 07:18:21  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.21  2003/11/11 08:36:21  dkrajzew
// removed some debug-variables
//
// Revision 1.20  2003/10/08 07:09:16  dkrajzew
// gcc did not knew a const_iterator to a map (?)
//
// Revision 1.19  2003/10/01 13:59:53  dkrajzew
// logic building completed (Julia Ringel)
//
// Revision 1.18  2003/09/25 09:01:49  dkrajzew
// ambigous naming of detectors changed
//
// Revision 1.17  2003/09/24 09:55:11  dkrajzew
// bug on duplictae induct loop ids patched
//
// Revision 1.16  2003/09/23 14:19:59  dkrajzew
// an easier usage of the current actuated phase definition added
//
// Revision 1.15  2003/09/22 12:31:06  dkrajzew
// actuated traffic lights are now derived from simple traffic lights
//
// Revision 1.14  2003/07/21 18:13:05  roessel
// Changes due to new MSInductLoop.
//
// Revision 1.13  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.12  2003/06/06 10:39:16  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.11  2003/06/05 16:01:28  dkrajzew
// MSTLLogicControl added
//
// Revision 1.9  2003/05/27 18:47:35  roessel
// Changed call to MSLaneState ctor.
//
// Revision 1.8  2003/05/22 09:20:49  roessel
// Renamed method call numberOfWaiting to getCurrentNumberOfWaiting.
//
// Revision 1.7  2003/05/21 16:20:44  dkrajzew
// further work detectors
//
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
#include "MSInductLoop.h"
#include "MSLaneState.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"
#include "MSActuatedTrafficLightLogic.h"
#include "MSLane.h"
#include <netload/NLDetectorBuilder.h>


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSActuatedTrafficLightLogic::MSActuatedTrafficLightLogic(
            const std::string &id,
            const Phases &phases,
            size_t step, size_t delay)
    : MSExtendedTrafficLightLogic(id, phases, step, delay),
    _continue(false)
{
}


void
MSActuatedTrafficLightLogic::init(NLDetectorBuilder &nb,
        const std::vector<MSLane*> &lanes,
        std::map<std::string, std::vector<std::string> > &laneContinuations,
        double det_offset)
{
    sproutDetectors(nb, lanes, laneContinuations, det_offset);
}



MSActuatedTrafficLightLogic::~MSActuatedTrafficLightLogic()
{
}


MSNet::Time
MSActuatedTrafficLightLogic::duration() const
{
    if(_continue) {
        return 1;
    }
    assert(_phases.size()>_step);
    if(!isGreenPhase()) {
        return currentPhaseDef()->duration;
    }
    // define the duration depending from the number of waiting vehicles of the actual phase
    int newduration = currentPhaseDef()->minDuration;
    const std::bitset<64> &isgreen = currentPhaseDef()->getDriveMask();
    for (size_t i=0; i<isgreen.size(); i++)  {
        if(isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end();j++) {
                LaneStateMap::const_iterator k = myLaneStates.find(*j);
                double waiting =  (*k).second->getCurrentNumberOfWaiting();
                double passingTime = 1.9;
                double tmpdur =  passingTime * waiting;
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


MSNet::Time
MSActuatedTrafficLightLogic::nextPhase()
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



void
MSActuatedTrafficLightLogic::sproutDetectors(
        NLDetectorBuilder &nb,
        const std::vector<MSLane*> &lanes,
        std::map<std::string, std::vector<std::string> > &laneContinuations,
        double det_offset)
{
    // change values for setting the loops and lanestate-detectors, here
    MSNet::Time inductLoopInterval = 1; //
//    double laneStateDetectorLength = 100; // length of the detecor
    // as the laneStateDetector shall end at the end of the lane, the position
    // is calculated, not given
    MSNet::Time laneStateDetectorInterval = 1; //

    std::vector<MSLane*>::const_iterator i;
    // build the induct loops
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        double length = lane->length();
        double speed = lane->maxSpeed();
        double gap = 3.0;
        double inductLoopPosition = gap * speed;
        // check whether the lane is long enough
        double ilpos = length - inductLoopPosition;
        if(ilpos<0) {
            ilpos = 0;
        }
        // Build the induct loop and set it into the container
        std::string id = "TLS" + _id + "_InductLoopOn_" + lane->id();
        if(myInductLoops.find(lane)==myInductLoops.end()) {
            myInductLoops[lane] = nb.createInductLoop(id, lane, ilpos );
        }
    }
    // build the lane state-detectors
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        double length = lane->length();
        // check whether the position is o.k. (not longer than the lane)
        double lslen = det_offset;
        if(lslen>length) {
            lslen = length;
        }
        double lspos = length - lslen;
        // Build the lane state detetcor and set it into the container
        std::string id = "TLS" + _id + "_LaneStateOff_" + lane->id();
        if(myLaneStates.find(lane)==myLaneStates.end()) {
            MSLaneState* loop =
                new MSLaneState( id, lane, lspos, lslen );
            myLaneStates[lane] = loop;
        }
    }
}


size_t
MSActuatedTrafficLightLogic::nextStep()
{
    // increment the index to the current phase
    _step++;
    assert(_step<=_phases.size());
    if(_step==_phases.size()) {
        _step = 0;
    }
    //stores the time the phase started
    static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->_lastSwitch = MSNet::globaltime;
    return _step;
}


bool
MSActuatedTrafficLightLogic::isGreenPhase() const
{
    if (currentPhaseDef()->getDriveMask().none()) {
        return false;
    }
    if (currentPhaseDef()->getYellowMask().any()) {
        return false;
    }
    return true;
}


bool
MSActuatedTrafficLightLogic::gapControl()
{
    //intergreen times should not be lenghtend
    assert(_phases.size()>_step);
    if(!isGreenPhase()) {
        return _continue = false;
    }

    // Checks, if the maxDuration is kept. No phase should longer send than maxDuration.
    MSNet::Time actDuration = MSNet::globaltime - static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->_lastSwitch;
    if (actDuration >= currentPhaseDef()->maxDuration) {
        return _continue = false;
    }

    // now the gapcontrol starts
    const std::bitset<64> &isgreen = currentPhaseDef()->getDriveMask();
    for (size_t i=0; i<isgreen.size(); i++)  {
        if(isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end(); j++) {
        if(myInductLoops.find(*j)==myInductLoops.end()) {
            continue;
        }
                double actualGap =  myInductLoops.find(*j)->second->getTimestepsSinceLastDetection();
                double maxGap = 3.1;
                if (actualGap < maxGap) {
                return _continue = true;
                }
            }
        }
    }
    return _continue = false;
}


MSActuatedPhaseDefinition *
MSActuatedTrafficLightLogic::currentPhaseDef() const
{
    assert(_phases.size()>_step);
    return static_cast<MSActuatedPhaseDefinition*>(_phases[_step]);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


