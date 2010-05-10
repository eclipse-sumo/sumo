/****************************************************************************/
/// @file    MSAgentbasedTrafficLightLogic.cpp
/// @author  Julia Ringel
/// @date    Wed, 01. Oct 2003
/// @version $Id$
///
// An agentbased traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include "MSAgentbasedTrafficLightLogic.h"
#include <netload/NLDetectorBuilder.h>
#include <utils/common/TplConvert.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSAgentbasedTrafficLightLogic::MSAgentbasedTrafficLightLogic(
    MSTLLogicControl &tlcontrol,
    const std::string &id, const std::string &programID,
    const Phases &phases, unsigned int step, SUMOTime delay,
    int learnHorizon, int decHorizon, SUMOReal minDiff, int tcycle) throw()
        : MSSimpleTrafficLightLogic(tlcontrol, id, programID, phases, step, delay),
        tDecide(decHorizon), tSinceLastDecision(0), stepOfLastDecision(0),
        numberOfValues(learnHorizon), tCycle(tcycle), deltaLimit(minDiff) {}


void
MSAgentbasedTrafficLightLogic::init(NLDetectorBuilder &nb) throw(ProcessError) {
    SUMOReal det_offset = TplConvert<char>::_2SUMOReal(myParameter.find("detector_offset")->second.c_str());
    LaneVectorVector::const_iterator i2;
    LaneVector::const_iterator i;
    // build the detectors
    for (i2=myLanes.begin(); i2!=myLanes.end(); ++i2) {
        const LaneVector &lanes = *i2;
        for (i=lanes.begin(); i!=lanes.end(); i++) {
            MSLane *lane = (*i);
            // Build the lane state detetcor and set it into the container
            std::string id = "TL_" + myID + "_" + myProgramID + "_E2OverLanesDetectorStartingAt_" + lane->getID();

            if (myE2Detectors.find(lane)==myE2Detectors.end()) {
                MS_E2_ZS_CollectorOverLanes* det =
                    nb.buildMultiLaneE2Det(id,
                                           DU_TL_CONTROL, lane, 0, det_offset,
                                           /*haltingTimeThreshold!!!*/ 1,
                                           /*haltingSpeedThreshold!!!*/(SUMOReal)(5.0/3.6),
                                           /*jamDistThreshold!!!*/ 10);
                myE2Detectors[lane] = det;
            }
        }
    }


    // initialise the duration
    unsigned int tCycleIst = 0;          // the actual cycletime
    unsigned int tCycleMin = 0;          // the minimum cycle time
    unsigned int tDeltaGreen = 0;         // the difference between the actual cycle time and the required cycle time

    /// Calculation of starting values
    for (unsigned int actStep = 0; actStep!=myPhases.size(); actStep++) {
        unsigned int dur = (unsigned int) myPhases[actStep]->duration;
        tCycleIst = tCycleIst + dur;
        if (myPhases[actStep]->isGreenPhase()) {
            unsigned int mindur = (unsigned int) myPhases[actStep]->minDuration;
            tCycleMin = tCycleMin + mindur;
        } else {
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
}


MSAgentbasedTrafficLightLogic::~MSAgentbasedTrafficLightLogic() throw() {}


// ------------ Switching and setting current rows
SUMOTime
MSAgentbasedTrafficLightLogic::trySwitch(bool) throw() {
    assert(getCurrentPhaseDef().minDuration >=0);
    assert(getCurrentPhaseDef().minDuration <= getCurrentPhaseDef().duration);
    if (myPhases[myStep]->isGreenPhase()) {
        // collects the data for the signal control
        collectData();
        // decides wheter greentime shall distributed between phases
        if (tDecide <= tSinceLastDecision) {
            calculateDuration();
        }
    }
    // increment the index to the current phase
    nextStep();
    // set the next event
    while (getCurrentPhaseDef().duration==0) {
        nextStep();
    }
    assert(myPhases.size()>myStep);
    return getCurrentPhaseDef().duration;
}


// ------------ "agentbased" algorithm methods
unsigned int
MSAgentbasedTrafficLightLogic::nextStep() throw() {
    // increment the index to the current phase
    myStep++;
    assert(myStep<=myPhases.size());
    if (myStep==myPhases.size()) {
        myStep = 0;
    }
    // increment the number of cycles since last decision
    if (myStep == stepOfLastDecision) {
        tSinceLastDecision = tSinceLastDecision +1;
    }
    return myStep;
}


void
MSAgentbasedTrafficLightLogic::collectData() throw() {
    //collects the traffic data

    // gets a copy of the driveMask
    const std::string &state = getCurrentPhaseDef().getState();
    // finds the maximum QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES of one phase
    SUMOReal maxPerPhase = 0;
    for (unsigned int i=0; i<(unsigned int) state.size(); i++)  {
        /* finds the maximum QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES
           of all lanes of a bit of the drivemask, that shows green */
        if (state[i]==MSLink::LINKSTATE_TL_GREEN_MAJOR||state[i]==MSLink::LINKSTATE_TL_GREEN_MINOR) {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            SUMOReal maxPerBit = 0;
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end(); j++) {
                if ((*j)->getEdge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
                    continue;
                }
                /*!!!
                SUMOReal tmp = currentForLane(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES, *j);
                if (maxPerBit < tmp)  {
                    maxPerBit = tmp;
                }
                E2DetectorMap::const_iterator it=myE2Detectors.find(*j);
                (*it).second->resetQueueLengthAheadOfTrafficLights();
                */
            }
            if (maxPerPhase < maxPerBit) {
                maxPerPhase = maxPerBit;
            }
        }
    }
    // if still no entry for the phase exists a new entry with an empty value is created
    if (myRawDetectorData.find(myStep) == myRawDetectorData.end()) {
        ValueType firstData;
        myRawDetectorData[myStep] = firstData;
    }
    /* checks whether the number of values that are already in the dataqueue is
       the same number of values taht shall be consideres in the traffic control
       if both numbers are the same, the oldest value is deleted */
    if (myRawDetectorData[myStep].size()== numberOfValues) {
        myRawDetectorData[myStep].pop_back();
    }
    // adds the detectorvalue of the considered phase
    myRawDetectorData[myStep].push_front(maxPerPhase);
}


void
MSAgentbasedTrafficLightLogic::aggregateRawData() throw() {
    for (PhaseValueMap::const_iterator i = myRawDetectorData.begin(); i!=myRawDetectorData.end(); i++) {
        SUMOReal sum = 0;
        for (ValueType:: const_iterator it = myRawDetectorData[(*i).first].begin(); it != myRawDetectorData[(*i).first].end(); it ++) {
            sum = sum + *it;
        }
        SUMOReal meanvalue = sum / myRawDetectorData[(*i).first].size();
        myMeanDetectorData[(*i).first] = meanvalue;
    }
}


void
MSAgentbasedTrafficLightLogic::calculateDuration() throw() {
    aggregateRawData();
    unsigned int stepOfMaxValue = findStepOfMaxValue();
    if (stepOfMaxValue == myPhases.size())    {
        return;
    }
    unsigned int stepOfMinValue = findStepOfMinValue();
    if (stepOfMinValue == myPhases.size())    {
        return;
    }
    if (stepOfMinValue == stepOfMaxValue)    {
        return;
    }
    SUMOReal deltaIst = (myMeanDetectorData[stepOfMaxValue] - myMeanDetectorData[stepOfMinValue])
                        / myMeanDetectorData[stepOfMaxValue];
    if (deltaIst > deltaLimit) {
        myPhases[stepOfMaxValue]->duration = myPhases[stepOfMaxValue]->duration +1;
        myPhases[stepOfMinValue]->duration = myPhases[stepOfMinValue]->duration -1;
        tSinceLastDecision = 0;
        stepOfLastDecision = myStep;
    }
}


void
MSAgentbasedTrafficLightLogic::lengthenCycleTime(unsigned int toLengthen) throw() {
    typedef std::pair <unsigned int, unsigned int> contentType;
    typedef std::vector< std::pair <unsigned int, unsigned int> > GreenPhasesVector;
    GreenPhasesVector tmp_phases(myPhases.size());
    tmp_phases.clear();
    unsigned int maxLengthen = 0;  // the sum of all times, that is possible to lengthen

    /* fills the vector tmp_phases with the difference between
       duration and maxduration and the myStep of the phases.
       only phases with duration < maxDuration are written in the vector.
       sorts the vector after the difference. */
    for (unsigned int i_Step = 0; i_Step!=myPhases.size(); i_Step++) {
        if (myPhases[i_Step]->isGreenPhase()) {
            unsigned int dur = (unsigned int) myPhases[i_Step]->duration;
            unsigned int maxdur = (unsigned int) myPhases[i_Step]->maxDuration;
            if (dur < maxdur) {
                contentType tmp;
                tmp.second = i_Step;
                tmp.first = maxdur - dur;
                tmp_phases.push_back(tmp);
                maxLengthen = maxLengthen + tmp.first;
            }
        }
    }
    sort(tmp_phases.begin(), tmp_phases.end());
    //lengthens the phases acording to the difference between duration and maxDuration
    for (GreenPhasesVector::iterator i=tmp_phases.begin(); i!=tmp_phases.end(); i++) {
        SUMOTime toLengthenPerPhase = 0;
        SUMOReal tmpdb = ((*i).first * toLengthen / SUMOReal(maxLengthen)) + (SUMOReal) 0.5;
        toLengthenPerPhase = static_cast<SUMOTime>(tmpdb);
        toLengthen = toLengthen - (unsigned int) toLengthenPerPhase;
        maxLengthen = maxLengthen - (*i).first;
        SUMOTime newDur = myPhases[(*i).second]->duration + toLengthenPerPhase;
        myPhases[(*i).second]->duration = newDur;
    }
}


void
MSAgentbasedTrafficLightLogic::cutCycleTime(unsigned int toCut) throw() {
    typedef std::pair <unsigned int, unsigned int> contentType;
    typedef std::vector< std::pair <unsigned int, unsigned int> > GreenPhasesVector;
    GreenPhasesVector tmp_phases(myPhases.size());
    tmp_phases.clear();
    unsigned maxCut = 0;  // the sum of all times, that is possible to cut

    /* fills the vector tmp_phases with the difference between
       duration and minduration and the myStep of the phases.
       only phases with duration > minDuration are written in the vector.
       sorts the vector after the difference. */
    for (unsigned i_Step = 0; i_Step!=myPhases.size(); i_Step++) {
        if (myPhases[i_Step]->isGreenPhase()) {
            unsigned int dur = (unsigned int) myPhases[i_Step]->duration;
            unsigned int mindur = (unsigned int) myPhases[i_Step]->minDuration;
            if (dur > mindur) {
                contentType tmp;
                tmp.second = i_Step;
                tmp.first = dur - mindur;
                tmp_phases.push_back(tmp);
                maxCut = maxCut + tmp.first;
            }
        }
    }
    std::sort(tmp_phases.begin(), tmp_phases.end());
    //cuts the phases acording to the difference between duration and minDuration
    for (GreenPhasesVector::iterator i=tmp_phases.begin(); i!=tmp_phases.end(); i++) {
        SUMOTime toCutPerPhase = 0;
        SUMOReal tmpdb = ((*i).first * toCut / SUMOReal(maxCut)) + (SUMOReal) 0.5;
        toCutPerPhase = static_cast<SUMOTime>(tmpdb);
        toCut = toCut - (unsigned int) toCutPerPhase;
        maxCut = maxCut - (*i).first;
        SUMOTime newDur = myPhases[(*i).second]->duration - toCutPerPhase;
        myPhases[(*i).second]->duration = newDur;
    }
}


unsigned int
MSAgentbasedTrafficLightLogic::findStepOfMaxValue() const throw() {
    unsigned int StepOfMaxValue = (unsigned int) myPhases.size();
    SUMOReal MaxValue = -1;
    for (MeanDataMap::const_iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++) {
        // checks whether the actual duruation is shorter than maxduration
        // otherwise the phase can't be lenghten
        unsigned int maxDur = (unsigned int) myPhases[(*it).first]->maxDuration;
        unsigned int actDur = (unsigned int) myPhases[(*it).first]->duration;
        if (actDur >= maxDur) {
            continue;
        }
        if ((*it).second > MaxValue) {
            MaxValue = (*it).second;
            StepOfMaxValue = (*it).first;
        }
    }
    return StepOfMaxValue;
}


unsigned int
MSAgentbasedTrafficLightLogic::findStepOfMinValue() const throw() {
    unsigned int StepOfMinValue = (unsigned int) myPhases.size();
    SUMOReal MinValue = 9999;
    for (MeanDataMap::const_iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++) {
        // checks whether the actual duruation is longer than minduration
        // otherwise the phase can't be cut
        unsigned int minDur = (unsigned int) myPhases[(*it).first]->minDuration;
        unsigned int actDur = (unsigned int) myPhases[(*it).first]->duration;
        if (actDur <= minDur) {
            continue;
        }
        if ((*it).second < MinValue) {
            MinValue = (*it).second;
            StepOfMinValue = (*it).first;
        }
    }
    return StepOfMinValue;
}


/*
SUMOReal
MSAgentbasedTrafficLightLogic::currentForLane(E2::DetType what,
        MSLane *lane) const
{

    E2DetectorMap::const_iterator i=myE2Detectors.find(lane);
    return (*i).second->getCurrent(what);
}
*/


/****************************************************************************/

