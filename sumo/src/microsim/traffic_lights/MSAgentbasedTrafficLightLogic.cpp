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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 11:09:53  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:22:18  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2005/02/01 10:10:46  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.3  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.2  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.1  2004/11/23 10:18:41  dkrajzew
// all traffic lights moved to microsim/traffic_lights
//
// Revision 1.19  2004/07/02 09:53:58  dkrajzew
// some design things
//
// Revision 1.18  2004/04/02 11:38:28  dkrajzew
// extended traffic lights are now no longer template classes
//
// Revision 1.17  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.16  2004/02/18 05:26:09  dkrajzew
// newest version by jringel
//
// Revision 1.15  2004/01/26 07:32:46  dkrajzew
// added the possibility to specify the position (actuated-tlls) / length
//  (agentbased-tlls) of used detectors
//
// Revision 1.14  2004/01/13 08:06:55  dkrajzew
// recent changes applied
//
// Revision 1.13  2004/01/12 15:04:16  dkrajzew
// more wise definition of lane predeccessors implemented
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
// Revision 1.5 2003/10/30 jringel
// agent-based logic implemented
//
// Revision 1.4  2003/10/08 14:50:28  dkrajzew
//
// Revision 1.1  2003/10/01 11:24:35  dkrajzew
// agent-based traffic lights added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utility>
#include <vector>
#include <bitset>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSLaneState.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "MSAgentbasedTrafficLightLogic.h"
#include <netload/NLDetectorBuilder.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSAgentbasedTrafficLightLogic::MSAgentbasedTrafficLightLogic(
            const std::string &id, const Phases &phases,
            size_t step, size_t delay, int learnHorizon, int decHorizon,
            SUMOReal minDiff, int tcycle)
    : MSExtendedTrafficLightLogic(id, phases, step, delay),
    tDecide(decHorizon), tSinceLastDecision (0), stepOfLastDecision (0),
    numberOfValues(learnHorizon), tCycle(tcycle), deltaLimit (minDiff)
{
}


void
MSAgentbasedTrafficLightLogic::init(
        NLDetectorBuilder &nb,
        const std::vector<MSLane*> &lanes,
        const std::map<std::string, std::vector<std::string> > &laneContinuations,
        SUMOReal det_offset)
{
    sproutDetectors(nb, lanes, laneContinuations, det_offset);
    initializeDuration();
}


MSAgentbasedTrafficLightLogic::~MSAgentbasedTrafficLightLogic()
{
}


void
MSAgentbasedTrafficLightLogic::sproutDetectors(
        NLDetectorBuilder &nb,
        const std::vector<MSLane*> &lanes,
        const std::map<std::string, std::vector<std::string> > &laneContinuations,
        SUMOReal det_offset)
{
    // change values for setting the detectors, here
//    SUMOReal laneStateDetectorLength = 75; // length of the detecor
    std::vector<MSLane*>::const_iterator i;
    // build the E2-detectors
    for(i=lanes.begin(); i!=lanes.end(); i++) {
        MSLane *lane = (*i);
        // Build the lane state detetcor and set it into the container
        std::string id = "TL_" + _id + "_E2OverLanesDetectorStartingAt_" + lane->id();

        if ( myE2Detectors.find(lane)==myE2Detectors.end()){
            MS_E2_ZS_CollectorOverLanes* det =
                nb.buildMultiLaneE2Det(laneContinuations, id,
                    DU_TL_CONTROL, lane, 0, det_offset,
                    /*haltingTimeThreshold*/ 1,
                    /*haltingSpeedThreshold*/(SUMOReal) (5.0/3.6),
                    /*jamDistThreshold*/ 10,
                    /*deleteDataAfterSeconds*/ 1800);
            myE2Detectors[lane] = det;
        }
    }
}


void
MSAgentbasedTrafficLightLogic::initializeDuration()
{
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


void
MSAgentbasedTrafficLightLogic::lengthenCycleTime(size_t toLengthen)
{
    typedef std::pair <size_t, size_t> contentType;
    typedef vector< pair <size_t,size_t> > GreenPhasesVector;
    GreenPhasesVector tmp_phases (myPhases.size());
    tmp_phases.clear();
    size_t maxLengthen = 0;  // the sum of all times, that is possible to lengthen

    /* fills the vector tmp_phases with the difference between
       duration and maxduration and the _step of the phases.
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
        size_t toLengthenPerPhase = 0;
        SUMOReal tmpdb = ((*i).first * toLengthen / SUMOReal(maxLengthen)) + (SUMOReal) 0.5;
        toLengthenPerPhase = static_cast<size_t>(tmpdb);
        toLengthen = toLengthen - toLengthenPerPhase;
        maxLengthen = maxLengthen - (*i).first;
        size_t newDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration + toLengthenPerPhase;
        static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration = newDur;
    }
}


void
MSAgentbasedTrafficLightLogic::cutCycleTime(size_t toCut)
{
    typedef std::pair <size_t, size_t> contentType;
    typedef vector< pair <size_t,size_t> > GreenPhasesVector;
    GreenPhasesVector tmp_phases (myPhases.size());
    tmp_phases.clear();
    size_t maxCut = 0;  // the sum of all times, that is possible to cut

    /* fills the vector tmp_phases with the difference between
       duration and minduration and the _step of the phases.
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
        size_t toCutPerPhase = 0;
        SUMOReal tmpdb = ((*i).first * toCut / SUMOReal(maxCut)) + (SUMOReal) 0.5;
        toCutPerPhase = static_cast<size_t>(tmpdb);
        toCut = toCut - toCutPerPhase;
        maxCut = maxCut - (*i).first;
        size_t newDur = static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration - toCutPerPhase;
        static_cast<MSActuatedPhaseDefinition*>(myPhases[(*i).second])->duration = newDur;
    }
}


SUMOTime
MSAgentbasedTrafficLightLogic::nextPhase()
{
    assert (currentPhaseDef()->minDuration >=0);
    assert (currentPhaseDef()->minDuration <= currentPhaseDef()->duration);
    if(isGreenPhase(myStep)) {
        // collects the data for the signal control
        collectData();
        // decides wheter greentime shall distributed between phases
        if(tDecide <= tSinceLastDecision) {
            calculateDuration();
        }
    }

    // some output for control
/*    if (_step == 0) {
        cut << endl << "JunctionID: "<< _id  <<"  Zeit: " << MSNet::globaltime;
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
    // reset the link priorities
    setLinkPriorities();
    // set the next event
    return duration();
}


size_t
MSAgentbasedTrafficLightLogic::nextStep()
{
    // increment the index to the current phase
    myStep++;
    assert(myStep<=myPhases.size());
    if(myStep==myPhases.size()) {
        myStep = 0;
    }
    // increment the number of cycles since last decision
    if(myStep == stepOfLastDecision) {
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
        if(isgreen.test(i))  {
            const std::vector<MSLane*> &lanes = getLanesAt(i);
            if (lanes.empty())    {
                break;
            }
            SUMOReal maxPerBit = 0;
            for (LaneVector::const_iterator j=lanes.begin(); j!=lanes.end();j++) {
                if((*j)->edge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
                    continue;
                }
                SUMOReal tmp = currentForLane(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES, *j);
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
    if (myRawDetectorData.find(myStep) == myRawDetectorData.end() ) {
        ValueType firstData;
        myRawDetectorData[myStep] = firstData;
    }
    /* checks whether the number of values that are already in the dataqueue is
       the same number of values taht shall be consideres in the traffic control
       if both numbers are the same, the oldest value is deleted */
    if(myRawDetectorData[myStep].size()== numberOfValues) {
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
        for (ValueType:: const_iterator it = myRawDetectorData[(*i).first].begin(); it != myRawDetectorData[(*i).first].end(); it ++){
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
    for (MeanDataMap::iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++){

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
    for (MeanDataMap::iterator it = myMeanDetectorData.begin(); it!=myMeanDetectorData.end(); it++){

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


SUMOTime
MSAgentbasedTrafficLightLogic::duration()
{
    while (currentPhaseDef()->duration==0) {
        nextStep();
        setLinkPriorities();
    }
    assert(myPhases.size()>myStep);
    return currentPhaseDef()->duration;
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


SUMOReal
MSAgentbasedTrafficLightLogic::currentForLane(E2::DetType what,
                                              MSLane *lane) const
{

    E2DetectorMap::const_iterator i=myE2Detectors.find(lane);
    return (*i).second->getCurrent(what);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


