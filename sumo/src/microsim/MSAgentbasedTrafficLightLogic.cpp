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
// Revision 1.4  2003/10/08 14:50:28  dkrajzew
// new usage of MSAgentbased... impemented (Julia Ringel)
//
// Revision 1.1  2003/10/01 11:24:35  dkrajzew
// agent-based traffic lights added
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





template< class _TE2_ZS_Collector >
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>(
        const std::string &id, const Phases &phases, size_t step,
        const std::vector<MSLane*> &lanes, size_t delay)
    : MSSimpleTrafficLightLogic(id, phases, step, delay),
    tSinceLastDecision (0), tDecide(2), tCycle(100)
{
    sproutDetectors(lanes);
    initializeDuration();
}


template< class _TE2_ZS_Collector >
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::~MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>()
{
}


template< class _TE2_ZS_Collector > MSNet::Time
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::duration() const
{
    assert(_phases.size()>_step);
    return currentPhaseDef()->duration;
}


template< class _TE2_ZS_Collector > MSNet::Time
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::nextPhase()
{
    // checks if the actual phase should be continued
    if(isGreenPhase(_step)) {
        collectData();
    }
    if(tDecide==tSinceLastDecision) {
        calculateDuration();
    }
    // increment the index to the current phase
    nextStep();
    // reset the link priorities
    setLinkPriorities();
    // set the next event
    return duration();
}



template< class _TE2_ZS_Collector >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::sproutDetectors(
                                const std::vector<MSLane*> &lanes)
{
    // change values for setting the detectors, here
    double laneStateDetectorLength = 75; // length of the detecor
    std::vector<MSLane*>::const_iterator i;

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

        if ( myE2Detectors.find(lane)==myE2Detectors.end()){
            _TE2_ZS_Collector* det =
                new _TE2_ZS_Collector( id, lane, lspos, lslen );
		    det->addDetector( MS_E2_ZS_Collector::ALL, "detectors" );
            myE2Detectors[lane] = det;
        }
    }
}


template< class _TE2_ZS_Collector >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::initializeDuration()
{
     size_t tCycleIst = 0;          // the actual cycletime
     size_t tCycleMin = 0;          // the minimum cycle time
     size_t tDeltaGreen = 0;         // the difference between the actual cycle time and the required cycle time

     /// Calculation of starting values
     for (size_t actStep = 0; actStep!=_phases.size(); actStep++) {
         size_t dur = static_cast<MSActuatedPhaseDefinition*>(_phases[actStep])->duration;
         tCycleIst = tCycleIst + dur;
         if (isGreenPhase(actStep)) {
             size_t mindur = static_cast<MSActuatedPhaseDefinition*>(_phases[actStep])->minDuration;
             tCycleMin = tCycleMin + mindur;
         }
         else {
             tCycleMin = tCycleMin + dur;
         }
     }
     if (tCycle < tCycleMin) {
         tCycle = tCycleMin;
     }
     tDeltaGreen = tCycle - tCycleIst;
     if (tDeltaGreen < 0) {
         cutCycleTime(tDeltaGreen);
     }
     if (tDeltaGreen > 0) {
         lengthenCycleTime(tDeltaGreen);
     }

     return;
}


template< class _TE2_ZS_Collector >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::lengthenCycleTime(size_t toLengthen)
{
    // stores the _step and the duration of the phases, except the intergrenn phases
    typedef std::pair <size_t, size_t> contentType;
    typedef vector< pair <size_t,size_t> > GreenPhasesVector;
    GreenPhasesVector myPhases (_phases.size());
    myPhases.clear();
    // the sum of all greentimes per phases
    size_t tGreenAllPhases = 0;
    size_t remainingGreen = toLengthen;
    size_t newdur = 0;

    for (size_t i_Step = 0; i_Step!=_phases.size(); i_Step++) {
        if (isGreenPhase(i_Step)) {
            contentType tmp;
            tmp.second = i_Step;
            tmp.first = static_cast<MSActuatedPhaseDefinition*>(_phases[i_Step])->duration;
            myPhases.push_back(tmp);
            tGreenAllPhases = tGreenAllPhases + tmp.first;
        }
    }
    sort(myPhases.begin(), myPhases.end());

    for (GreenPhasesVector::iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        double tmpdb = ((*i).first * toLengthen / double(tGreenAllPhases)) + 0.5;
        size_t toContrib = static_cast<size_t>(tmpdb);
        if (remainingGreen > toContrib) {
            remainingGreen = remainingGreen - toContrib;
            newdur = toContrib +  (*i).first;
        }
        else {
            newdur = remainingGreen + (*i).first;
        }
        static_cast<MSActuatedPhaseDefinition*>(_phases[(*i).second])->duration = newdur;
        cout << tmpdb << "   " << toContrib << "      ";
        cout << (_phases[(*i).second])->duration << "   " << (*i).second << endl;
    }

}



template< class _TE2_ZS_Collector >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::cutCycleTime(size_t toCut)

{
    // stores the _step and the duration of the phases, except the intergrenn phases
    typedef std::pair <size_t, size_t> contentType;
    typedef std::vector< contentType > GreenPhasesVector;
    GreenPhasesVector myPhases (_phases.size());
    myPhases.clear();
    // the sum of all greentimes per phases
    size_t tGreenAllPhases = 0;

    for (size_t i_Step = 0; i_Step!=_phases.size(); i_Step++) {
        if (isGreenPhase(i_Step)) {
            size_t dur = static_cast<MSActuatedPhaseDefinition*>(_phases[i_Step])->duration;
            size_t mindur = static_cast<MSActuatedPhaseDefinition*>(_phases[i_Step])->minDuration;
            contentType tmp;
            tmp.second = i_Step;
            tmp.first = static_cast<MSActuatedPhaseDefinition*>(_phases[i_Step])->duration;
            if (dur > mindur) {
                myPhases.push_back(tmp);
                tGreenAllPhases = tGreenAllPhases + dur;
            }
         }
     }

     cout << myPhases.size() << "  " << myPhases.capacity() << endl;
     for (GreenPhasesVector::const_iterator i=myPhases.begin(); i!=myPhases.end();i++) {
         size_t teststep = (*i).first;
         size_t dur = (*i).second;
         cout << teststep << "   "  << dur <<  endl;
     }
}


template< class _TE2_ZS_Collector >
size_t
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::nextStep()
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

template< class _TE2_ZS_Collector >
bool
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::isGreenPhase(const size_t testStep) const
{
    assert(testStep<=_phases.size());
    if (static_cast<MSActuatedPhaseDefinition*>(_phases[testStep])->getDriveMask().none()) {
        return false;
    }
    if (static_cast<MSActuatedPhaseDefinition*>(_phases[testStep])->getYellowMask().any()) {
        return false;
    }
    return true;
}


template< class _TE2_ZS_Collector >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::collectData() const
{
//collects the traffic data

    const std::bitset<64> &isgreen = currentPhaseDef()->getDriveMask();
    for (size_t i=0; i<isgreen.size(); i++)  {
        if(isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end();j++) {
                double bla1 = currentForLane(MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_VEHICLES, *j);
                double bla2 = currentForLane(MS_E2_ZS_Collector::MAX_JAM_LENGTH_IN_METERS, *j);
                double bla3 = currentForLane(MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_VEHICLES, *j);
                double bla4 = currentForLane(MS_E2_ZS_Collector::JAM_LENGTH_SUM_IN_METERS, *j);
                double bla5 = currentForLane(MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES, *j);
                double bla6 = currentForLane(MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS, *j);
                cout << bla1 <<"   " << bla2 <<"   " << bla3 <<"   " << bla4 <<"   " << bla5 <<"   " << bla6 << endl;
            }
        }
    }
}


template< class _TE2_ZS_Collector >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::calculateDuration()
{
//calculates the duration of all phases, except the intergreen phase

/*
//  Checks, if the minDuration is kept. No phase should send shorter than minDuration.
    MSNet::Time actDuration = MSNet::globaltime - static_cast<MSActuatedPhaseDefinition*>(_phases[_step])->_lastSwitch;
    if (actDuration >= currentPhaseDef()->minDuration) {
        actDuration = minDuration;
    }
*/

}

template< class _TE2_ZS_Collector >
MSActuatedPhaseDefinition *
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::currentPhaseDef() const
{
	assert(_phases.size()>_step);
    return static_cast<MSActuatedPhaseDefinition*>(_phases[_step]);
}


template< class _TE2_ZS_Collector >
double
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::currentForLane(
		MS_E2_ZS_Collector::DetType what, MSLane *lane) const
{
	E2DetectorMap::const_iterator i=myE2Detectors.find(lane);
	return (*i).second->getGurrent(what);
}


template< class _TE2_ZS_Collector >
double
MSAgentbasedTrafficLightLogic<_TE2_ZS_Collector>::aggregatedForLane(
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


