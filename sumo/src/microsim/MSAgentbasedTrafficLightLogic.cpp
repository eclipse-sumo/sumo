//---------------------------------------------------------------------------//
//                        MSAgentbasedTrafficLightLogic.cpp -
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 01. Oct 2003
//  copyright            : (C) 2003 by DLR e.V.
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
// Revision 1.10  2003/11/28 10:20:37  jringel
// phases with duration == 0 considered
//
// Revision 1.9  2003/11/24 10:21:20  dkrajzew
// some documentation added and dead code removed
//
// Revision 1.8  2003/11/20 13:25:41  dkrajzew
// unneded debug outputs removed
//
// Revision 1.7  2003/11/18 14:26:55  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.6  2003/11/17 07:18:21  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.5  2003/11/04 08:55:28  jringel
// implemetation of the agentbased trafficlightlogic
//
// Revision 1.4  2003/10/08 14:50:28  dkrajzew
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
#include "MSInductLoop.h"
#include "MSLaneState.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"
#include "MSAgentbasedTrafficLightLogic.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
template< class _TE2_ZS_CollectorOverLanes >
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>(
            const std::string &id, const Phases &phases, size_t step,
            const std::vector<MSLane*> &lanes, size_t delay,
            std::map<std::string, std::vector<std::string> > &laneContinuations)
    : MSSimpleTrafficLightLogic(id, phases, step, delay),
    tSinceLastDecision (0), tDecide(1), tCycle(75), numberOfValues(3),
    deltaLimit (0.1), stepOfLastDecision (0)
{
    sproutDetectors(lanes, laneContinuations);
    initializeDuration();
}


template< class _TE2_ZS_CollectorOverLanes >
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::~MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>()
{
}


template< class _TE2_ZS_CollectorOverLanes >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::sproutDetectors(
        const std::vector<MSLane*> &lanes,
        std::map<std::string, std::vector<std::string> > &laneContinuations)
{
    // change values for setting the detectors, here
    double laneStateDetectorLength = 75; // length of the detecor
    std::vector<MSLane*>::const_iterator i;
    // build the E2-detectors
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        // Build the lane state detetcor and set it into the container
        std::string id = "TL_" + _id + "_E2OverLanesDetectorStartingAt_" + lane->id();

        if ( myE2Detectors.find(lane)==myE2Detectors.end()){
            _TE2_ZS_CollectorOverLanes* det =
                new _TE2_ZS_CollectorOverLanes( id, lane, 0 );
            det->init(lane, laneStateDetectorLength, laneContinuations);
		    det->addDetector( MS_E2_ZS_Collector::ALL, "detectors" );
            myE2Detectors[lane] = det;
        }
    }
}


template< class _TE2_ZS_CollectorOverLanes >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::initializeDuration()
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
     if (tCycleIst < tCycle) {
         tDeltaGreen = tCycle - tCycleIst;
         lengthenCycleTime(tDeltaGreen);
     }
     if (tCycleIst > tCycle) {
         tDeltaGreen = tCycleIst - tCycle;
         cutCycleTime(tDeltaGreen);
     }
     return;
}


template< class _TE2_ZS_CollectorOverLanes >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::lengthenCycleTime(size_t toLengthen)
{
    typedef std::pair <size_t, size_t> contentType;
    typedef vector< pair <size_t,size_t> > GreenPhasesVector;
    GreenPhasesVector myPhases (_phases.size());
    myPhases.clear();
    size_t tGreenAllPhases = 0;  // the sum of all greentimes per phases
    size_t remainingGreen = toLengthen;
    size_t newdur = 0;

    /* fills the vector myPhases with the duration
       and the _step of the phases (except the intergreen phases)
       sorts them corresponding to their duration */
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

    //devides the time toLengthen to the greenphases corresponding to their duration
    for (GreenPhasesVector::iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        double tmpdb = ((*i).first * toLengthen / double(tGreenAllPhases)) + 0.5;
        size_t toContrib = static_cast<size_t>(tmpdb);
        if (remainingGreen > toContrib) {
            remainingGreen = remainingGreen - toContrib;
            newdur = toContrib +  (*i).first;
        }
        else {
            newdur = remainingGreen + (*i).first;
            remainingGreen = 0;
        }
        static_cast<MSActuatedPhaseDefinition*>(_phases[(*i).second])->duration = newdur;
    }
    // gives eventually through rounding remaining green to the shortest phase
    if (remainingGreen > 0) {
        size_t destStep = myPhases.begin()->second;
        size_t actdur = static_cast<MSActuatedPhaseDefinition*>(_phases[destStep])->duration;
        size_t ndur = actdur + remainingGreen;
        static_cast<MSActuatedPhaseDefinition*>(_phases[destStep])->duration = ndur;
    }
}


template< class _TE2_ZS_CollectorOverLanes >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::cutCycleTime(size_t toCut)
{
    typedef std::pair <size_t, size_t> contentType;
    typedef vector< pair <size_t,size_t> > GreenPhasesVector;
    GreenPhasesVector myPhases (_phases.size());
    myPhases.clear();
    size_t maxCut = 0;  // the sum of all times, that is possible to cut

    /* fills the vector myPhases with the difference between
       duration and minduration and the _step of the phases.
       only phases with duration > minDuration are written in the vector.
       sorts the vector after the difference. */
    for (size_t i_Step = 0; i_Step!=_phases.size(); i_Step++) {
        if (isGreenPhase(i_Step)) {
            size_t dur = static_cast<MSActuatedPhaseDefinition*>(_phases[i_Step])->duration;
            size_t mindur = static_cast<MSActuatedPhaseDefinition*>(_phases[i_Step])->minDuration;
            if (dur > mindur) {
                contentType tmp;
                tmp.second = i_Step;
                tmp.first = dur - mindur;
                myPhases.push_back(tmp);
                maxCut = maxCut + tmp.first;
            }
        }
    }
    sort(myPhases.begin(), myPhases.end());

    //cuts the phases acording to the difference between duration and minDuration
    for (GreenPhasesVector::iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        size_t toCutPerPhase = 0;
        double tmpdb = ((*i).first * toCut / double(maxCut)) + 0.5;
        toCutPerPhase = static_cast<size_t>(tmpdb);
        toCut = toCut - toCutPerPhase;
        maxCut = maxCut - (*i).first;
        size_t newDur = static_cast<MSActuatedPhaseDefinition*>(_phases[(*i).second])->duration - toCutPerPhase;
        static_cast<MSActuatedPhaseDefinition*>(_phases[(*i).second])->duration = newDur;
    }
}


template< class _TE2_ZS_CollectorOverLanes > MSNet::Time
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::nextPhase()
{
    assert (currentPhaseDef()->minDuration >=0);
	assert (currentPhaseDef()->minDuration <= currentPhaseDef()->duration);
	if(isGreenPhase(_step)) {
        // collects the data for the signal control
        collectData();
        // decides wheter greentime shall distributed between phases
        if(tDecide <= tSinceLastDecision) {
        calculateDuration();
        }
    }

    // some output for control
    if (_step == 0) {
        cout << endl << "JunctionID: "<< _id  <<"  Zeit: " << MSNet::globaltime;
        for (PhaseValueMap:: const_iterator it = myRawDetectorData.begin(); it!=myRawDetectorData.end(); it++) {
            cout<< " step: "<<(*it).first << "  Anz.Werte: " << (*it).second.size();
            for (ValueType:: const_iterator itV = myRawDetectorData[(*it).first].begin(); itV!=myRawDetectorData[(*it).first].end(); itV++) {
                cout<<"  Wert: " << (*itV) ;
            }
            cout<<" Dauer: " << _phases[(*it).first]->duration << "  " ;
        }
    }


    // increment the index to the current phase
    nextStep();
    // reset the link priorities
    setLinkPriorities();
    // set the next event
    return duration();
}


template< class _TE2_ZS_CollectorOverLanes >
size_t
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::nextStep() 
{
    // increment the index to the current phase
    _step++;
    assert(_step<=_phases.size());
    if(_step==_phases.size()) {
        _step = 0;
    }
    // increment the number of cycles since last decision
    if(_step == stepOfLastDecision) {
        tSinceLastDecision = tSinceLastDecision +1;
    }
    return _step;
}


template< class _TE2_ZS_CollectorOverLanes >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::collectData()
{
    //collects the traffic data

    // gets a copy of the driveMask
    const std::bitset<64> &isgreen = currentPhaseDef()->getDriveMask();
    // finds the maximum QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES of one phase
    double maxPerPhase = 0;
    for (size_t i=0; i<isgreen.size(); i++)  {
        /* finds the maximum QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES
           of all lanes of a bit of the drivemask, that shows green */
        if(isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            double maxPerBit = 0;
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end();j++) {
                if((*j)->edge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
                    continue;
                }
                double tmp = currentForLane(MS_E2_ZS_Collector::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES, *j);
                if (maxPerBit < tmp)  {
                    maxPerBit = tmp;
                }
                E2DetectorMap::const_iterator it=myE2Detectors.find(*j);
	            (*it).second->resetQueueLengthAheadOfTrafficLights();
            }
            if (maxPerPhase < maxPerBit) {
                maxPerPhase = maxPerBit;
            }
        }
    }
    // if still no entry for the phase exists a new entry with an empty value is created
    if (myRawDetectorData.find(_step) == myRawDetectorData.end() ) {
                    ValueType firstData;
                    myRawDetectorData[_step] = firstData;
    }
    /* checks whether the number of values that are already in the dataqueue is
       the same number of values taht shall be consideres in the traffic control
       if both numbers are the same, the oldest value is deleted */
    if(myRawDetectorData[_step].size()== numberOfValues) {
        myRawDetectorData[_step].pop_back();
    }
    // adds the detectorvalue of the considered phase
    myRawDetectorData[_step].push_front(maxPerPhase);
}


template< class _TE2_ZS_CollectorOverLanes >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::aggregateRawData()
{
    for (PhaseValueMap::const_iterator i = myRawDetectorData.begin(); i!=myRawDetectorData.end(); i++) {
        double sum = 0;
        for (ValueType:: const_iterator it = myRawDetectorData[(*i).first].begin(); it != myRawDetectorData[(*i).first].end(); it ++){
            sum = sum + *it;
        }
    double meanvalue = sum / myRawDetectorData[(*i).first].size();
    myMeanDetectorData[(*i).first] = meanvalue;
    }
}


template< class _TE2_ZS_CollectorOverLanes >
void
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::calculateDuration()
{
    aggregateRawData();
    size_t stepOfMaxValue = findStepOfMaxValue();
    size_t stepOfMinValue = findStepOfMinValue();
    if (stepOfMinValue == _phases.size())    {
        return;
    }
    double deltaIst = (myMeanDetectorData[stepOfMaxValue] - myMeanDetectorData[stepOfMinValue])
                        / myMeanDetectorData[stepOfMaxValue];
    if (deltaIst > deltaLimit) {
        _phases[stepOfMaxValue]->duration = _phases[stepOfMaxValue]->duration +1;
        _phases[stepOfMinValue]->duration = _phases[stepOfMinValue]->duration -1;
        tSinceLastDecision = 0;
        stepOfLastDecision = _step;
    }
}


template< class _TE2_ZS_CollectorOverLanes >
size_t
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::findStepOfMaxValue()
{
    size_t StepOfMaxValue = _phases.size();
    double MaxValue = -1;
    for (MeanDataMap::iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++){
        if ((*it).second > MaxValue) {
            MaxValue = (*it).second;
            StepOfMaxValue = (*it).first;
        }
    }
    assert (StepOfMaxValue < _phases.size());
    return StepOfMaxValue;
}


template< class _TE2_ZS_CollectorOverLanes >
size_t
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::findStepOfMinValue()
{
    /* checks all green phases, wheter their duration is longer than minduration
    otherwise the corresponding entry in myMeanDetectorData is deleted,
    so that it's not possible to return this phase for shortening */
    for (size_t i=0; i!=_phases.size(); i++){
        if (!isGreenPhase(i)) {
            continue;
        }
        if (i == findStepOfMaxValue()){
            continue;
        }
        size_t minDur = static_cast<MSActuatedPhaseDefinition*>(_phases[i])->minDuration;
        size_t istDur = static_cast<MSActuatedPhaseDefinition*>(_phases[i])->duration;
        if (minDur == istDur) {
            myMeanDetectorData.erase(i);
        }
    }
    size_t StepOfMinValue = _phases.size();
    if (myMeanDetectorData.size() < 2) {
        return StepOfMinValue;
    }
    double MinValue = 5000;
    for (MeanDataMap::iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++){
        if ((*it).second < MinValue) {
            MinValue = (*it).second;
            StepOfMinValue = (*it).first;
        }
    }
    return StepOfMinValue;
}


template< class _TE2_ZS_CollectorOverLanes > MSNet::Time
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::duration() 
{
   	while (currentPhaseDef()->duration==0) {
		nextStep();
		setLinkPriorities();
	}
	assert(_phases.size()>_step);
    return currentPhaseDef()->duration;
}


template< class _TE2_ZS_CollectorOverLanes >
MSActuatedPhaseDefinition *
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::currentPhaseDef() const
{
	assert(_phases.size()>_step);
    return static_cast<MSActuatedPhaseDefinition*>(_phases[_step]);
}


template< class _TE2_ZS_CollectorOverLanes >
bool
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::isGreenPhase(const size_t testStep) const
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


template< class _TE2_ZS_CollectorOverLanes >
double
MSAgentbasedTrafficLightLogic<_TE2_ZS_CollectorOverLanes>::currentForLane(
		MS_E2_ZS_Collector::DetType what, MSLane *lane) const
{

	E2DetectorMap::const_iterator i=myE2Detectors.find(lane);
	return (*i).second->getCurrent(what);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSAgentbasedTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


