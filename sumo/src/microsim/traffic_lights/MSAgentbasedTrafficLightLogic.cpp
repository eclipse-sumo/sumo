/****************************************************************************/
/// @file    MSAgentbasedTrafficLightLogic.cpp
/// @author  Julia Ringel
/// @date    Wed, 01. Oct 2003
/// @version $Id$
///
// -------------------
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSAgentbasedTrafficLightLogic::MSAgentbasedTrafficLightLogic(
    MSNet &net, MSTLLogicControl &tlcontrol,
    const std::string &id, const std::string &subid,
    const Phases &phases, size_t step, size_t delay,
    int learnHorizon, int decHorizon, SUMOReal minDiff, int tcycle)
        : MSSimpleTrafficLightLogic(net, tlcontrol, id, subid, phases, step, delay),
        tDecide(decHorizon), tSinceLastDecision(0), stepOfLastDecision(0),
        numberOfValues(learnHorizon), tCycle(tcycle), deltaLimit(minDiff)
{}


void
MSAgentbasedTrafficLightLogic::init(
    NLDetectorBuilder &nb,
    const MSEdgeContinuations &edgeContinuations)
{
    SUMOReal det_offset = TplConvert<char>::_2SUMOReal(myParameter.find("detector_offset")->second.c_str());
    LaneVectorVector::const_iterator i2;
    LaneVector::const_iterator i;
    // build the detectors
    for (i2=myLanes.begin(); i2!=myLanes.end(); ++i2) {
        const LaneVector &lanes = *i2;
        for (i=lanes.begin(); i!=lanes.end(); i++) {
            MSLane *lane = (*i);
            // Build the lane state detetcor and set it into the container
            std::string id = "TL_" + myID + "_" + mySubID + "_E2OverLanesDetectorStartingAt_" + lane->getID();

            if (myE2Detectors.find(lane)==myE2Detectors.end()) {
                MS_E2_ZS_CollectorOverLanes* det =
                    nb.buildMultiLaneE2Det(edgeContinuations, id,
                                           DU_TL_CONTROL, lane, 0, det_offset,
                                           /*haltingTimeThreshold!!!*/ 1,
                                           /*haltingSpeedThreshold!!!*/(SUMOReal)(5.0/3.6),
                                           /*jamDistThreshold!!!*/ 10);
                myE2Detectors[lane] = det;
            }
        }
    }


    // initialise the duration
    size_t tCycleIst = 0;          // the actual cycletime
    size_t tCycleMin = 0;          // the minimum cycle time
    size_t tDeltaGreen = 0;         // the difference between the actual cycle time and the required cycle time

    /// Calculation of starting values
    for (size_t actStep = 0; actStep!=myPhases.size(); actStep++) {
        size_t dur = static_cast<MSActuatedPhaseDefinition*>(myPhases[actStep])->duration;
        tCycleIst = tCycleIst + dur;
        if (isGreenPhase(actStep)) {
            size_t mindur = static_cast<MSActuatedPhaseDefinition*>(myPhases[actStep])->minDuration;
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


MSAgentbasedTrafficLightLogic::~MSAgentbasedTrafficLightLogic()
{}


void
MSAgentbasedTrafficLightLogic::lengthenCycleTime(size_t toLengthen)
{
    typedef std::pair <size_t, size_t> contentType;
    typedef vector< pair <size_t,size_t> > GreenPhasesVector;
    GreenPhasesVector tmp_phases(myPhases.size());
    tmp_phases.clear();
    size_t maxLengthen = 0;  // the sum of all times, that is possible to lengthen

    /* fills the vector tmp_phases with the difference between
       duration and maxduration and the myStep of the phases.
       only phases with duration < maxDuration are written in the vector.
       sorts the vector after the difference. */
    for (size_t i_Step = 0; i_Step!=myPhases.size(); i_Step++) {
        if (isGreenPhase(i_Step)) {
            size_t dur = static_cast<MSActuatedPhaseDefinition*>(myPhases[i_Step])->duration;
            size_t maxdur = static_cast<MSActuatedPhaseDefinition*>(myPhases[i_Step])->maxDuration;
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
        toLengthen = toLengthen - toLengthenPerPhase;
        maxLengthen = maxLengthen - (*i).first;
        SUMOTime newDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration + toLengthenPerPhase;
        static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration = newDur;
    }
}


void
MSAgentbasedTrafficLightLogic::cutCycleTime(size_t toCut)
{
    typedef std::pair <size_t, size_t> contentType;
    typedef vector< pair <size_t,size_t> > GreenPhasesVector;
    GreenPhasesVector tmp_phases(myPhases.size());
    tmp_phases.clear();
    size_t maxCut = 0;  // the sum of all times, that is possible to cut

    /* fills the vector tmp_phases with the difference between
       duration and minduration and the myStep of the phases.
       only phases with duration > minDuration are written in the vector.
       sorts the vector after the difference. */
    for (size_t i_Step = 0; i_Step!=myPhases.size(); i_Step++) {
        if (isGreenPhase(i_Step)) {
            size_t dur = static_cast<MSActuatedPhaseDefinition*>(myPhases[i_Step])->duration;
            size_t mindur = static_cast<MSActuatedPhaseDefinition*>(myPhases[i_Step])->minDuration;
            if (dur > mindur) {
                contentType tmp;
                tmp.second = i_Step;
                tmp.first = dur - mindur;
                tmp_phases.push_back(tmp);
                maxCut = maxCut + tmp.first;
            }
        }
    }
    sort(tmp_phases.begin(), tmp_phases.end());

    //cuts the phases acording to the difference between duration and minDuration
    for (GreenPhasesVector::iterator i=tmp_phases.begin(); i!=tmp_phases.end(); i++) {
        SUMOTime toCutPerPhase = 0;
        SUMOReal tmpdb = ((*i).first * toCut / SUMOReal(maxCut)) + (SUMOReal) 0.5;
        toCutPerPhase = static_cast<SUMOTime>(tmpdb);
        toCut = toCut - toCutPerPhase;
        maxCut = maxCut - (*i).first;
        SUMOTime newDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration - toCutPerPhase;
        static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration = newDur;
    }
}


SUMOTime
MSAgentbasedTrafficLightLogic::trySwitch(bool)
{
    assert(currentPhaseDef()->minDuration >=0);
    assert(currentPhaseDef()->minDuration <= currentPhaseDef()->duration);
    if (isGreenPhase(myStep)) {
        // collects the data for the signal control
        collectData();
        // decides wheter greentime shall distributed between phases
        if (tDecide <= tSinceLastDecision) {
            calculateDuration();
        }
    }

    // some output for control
    /*    if (myStep == 0) {
            cut << endl << "JunctionID: "<< myId  <<"  Zeit: " << MSNet::globaltime;
            for (PhaseValueMap:: const_iterator it = myRawDetectorData.begin(); it!=myRawDetectorData.end(); it++) {
                cut<< " step: "<<(*it).first << "  Anz.Werte: " << (*it).second.size();
                for (ValueType:: const_iterator itV = myRawDetectorData[(*it).first].begin(); itV!=myRawDetectorData[(*it).first].end(); itV++) {
                    cot<<"  Wert: " << (*itV) ;
                }
                  cot<<" Dauer: " << myPhases[(*it).first]->duration << "  " ;
            }
        }
    */
    // increment the index to the current phase
    nextStep();
    // set the next event
    while (currentPhaseDef()->duration==0) {
        nextStep();
    }
    assert(myPhases.size()>myStep);
    return currentPhaseDef()->duration;
}


size_t
MSAgentbasedTrafficLightLogic::nextStep()
{
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
MSAgentbasedTrafficLightLogic::collectData()
{
    //collects the traffic data

    // gets a copy of the driveMask
    const std::bitset<64> &isgreen = currentPhaseDef()->getDriveMask();
    // finds the maximum QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES of one phase
    SUMOReal maxPerPhase = 0;
    for (size_t i=0; i<isgreen.size(); i++)  {
        /* finds the maximum QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES
           of all lanes of a bit of the drivemask, that shows green */
        if (isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            SUMOReal maxPerBit = 0;
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end();j++) {
                if ((*j)->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
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
MSAgentbasedTrafficLightLogic::aggregateRawData()
{
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
MSAgentbasedTrafficLightLogic::calculateDuration()
{
    aggregateRawData();
    size_t stepOfMaxValue = findStepOfMaxValue();
    if (stepOfMaxValue == myPhases.size())    {
        return;
    }
    size_t stepOfMinValue = findStepOfMinValue();
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


size_t
MSAgentbasedTrafficLightLogic::findStepOfMaxValue()
{
    size_t StepOfMaxValue = myPhases.size();
    SUMOReal MaxValue = -1;
    for (MeanDataMap::iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++) {

        // checks whether the actual duruation is shorter than maxduration
        // otherwise the phase can't be lenghten
        size_t maxDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*it).first])->maxDuration;
        size_t actDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*it).first])->duration;
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


size_t
MSAgentbasedTrafficLightLogic::findStepOfMinValue()
{
    size_t StepOfMinValue = myPhases.size();
    SUMOReal MinValue = 9999;
    for (MeanDataMap::iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++) {

        // checks whether the actual duruation is longer than minduration
        // otherwise the phase can't be cut
        size_t minDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*it).first])->minDuration;
        size_t actDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*it).first])->duration;
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


MSActuatedPhaseDefinition *
MSAgentbasedTrafficLightLogic::currentPhaseDef() const
{
    assert(myPhases.size()>myStep);
    return static_cast<MSActuatedPhaseDefinition*>(myPhases[myStep]);
}


bool
MSAgentbasedTrafficLightLogic::isGreenPhase(const size_t testStep) const
{
    assert(testStep<=myPhases.size());
    if (static_cast<MSActuatedPhaseDefinition*>(myPhases[testStep])->getDriveMask().none()) {
        return false;
    }
    if (static_cast<MSActuatedPhaseDefinition*>(myPhases[testStep])->getYellowMask().any()) {
        return false;
    }
    return true;
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

