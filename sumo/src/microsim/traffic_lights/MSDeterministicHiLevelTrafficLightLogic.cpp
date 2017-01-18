/****************************************************************************/
/// @file    MSDeterministicHiLevelTrafficLightLogic.cpp
/// @author  Riccardo Belletti
/// @date    Mar 2014
/// @version $Id$
///
// The class for deterministic high level traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include "MSDeterministicHiLevelTrafficLightLogic.h"

MSDeterministicHiLevelTrafficLightLogic::MSDeterministicHiLevelTrafficLightLogic(
    MSTLLogicControl& tlcontrol, const std::string& id,
    const std::string& subid, const Phases& phases, int step,
    SUMOTime delay, const std::map<std::string, std::string>& parameters) :
    MSSOTLHiLevelTrafficLightLogic(tlcontrol, id, subid, phases, step,
                                   delay, parameters) {

    addPolicy(new MSSOTLPlatoonPolicy(new MSSOTLPolicy3DStimulus("PLATOON", parameters), parameters));
    addPolicy(new MSSOTLPhasePolicy(new MSSOTLPolicy3DStimulus("PHASE", parameters), parameters));
    addPolicy(new MSSOTLMarchingPolicy(new MSSOTLPolicy3DStimulus("MARCHING", parameters), parameters));
    addPolicy(new MSSOTLCongestionPolicy(new MSSOTLPolicy3DStimulus("CONGESTION", parameters), parameters));

}

MSDeterministicHiLevelTrafficLightLogic::~MSDeterministicHiLevelTrafficLightLogic() {

}

void MSDeterministicHiLevelTrafficLightLogic::init(NLDetectorBuilder& nb)
throw(ProcessError) {
    MSSOTLHiLevelTrafficLightLogic::init(nb);
    //Setting the startup policy
    choosePolicy(0, 0);
    WRITE_MESSAGE(
        "*** Intersection " + getID()
        + " will run using MSDeterministicHiLevelTrafficLightLogic ***");

    MSLane* currentLane = NULL;
    for (MSTrafficLightLogic::LaneVectorVector::const_iterator laneVector =
                myLanes.begin(); laneVector != myLanes.end(); laneVector++) {
        for (MSTrafficLightLogic::LaneVector::const_iterator lane =
                    laneVector->begin(); lane != laneVector->end(); lane++) {
            currentLane = (*lane);
            if (inputLanes.find(currentLane->getID()) == inputLanes.end()) {
                inputLanes.insert(currentLane->getID());
                DBG(
                    WRITE_MESSAGE("*** Intersection " + getID() + " inputLanes adding " + currentLane->getID());)
            }
        }
    }

    LinkVectorVector myLinks = getLinks();

    for (int i = 0; i < (int)myLinks.size(); i++) {
        LinkVector oneLink = getLinksAt(i);
        for (int j = 0; j < (int)oneLink.size(); j++) {
            currentLane = oneLink[j]->getLane();
            if (outputLanes.find(currentLane->getID()) == outputLanes.end()) {
                outputLanes.insert(currentLane->getID());
                DBG(
                    WRITE_MESSAGE("*** Intersection " + getID() + " outputLanes adding " + currentLane->getID());)
            }
        }
    }

}

int MSDeterministicHiLevelTrafficLightLogic::decideNextPhase() {

    DBG(
        MsgHandler::getMessageInstance()->inform("\n" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + " MSDeterministicHiLevelTrafficLightLogic decideNextPhase()"); std::ostringstream dnp; dnp << (MSNet::getInstance()->getCurrentTimeStep()) << " MSDeterministicHiLevelTrafficLightLogic::decideNextPhase:: " << "tlsid=" << getID() << " getCurrentPhaseDef().getState()=" << getCurrentPhaseDef().getState() << " is commit?" << getCurrentPhaseDef().isCommit(); MsgHandler::getMessageInstance()->inform(dnp.str());)

    //Decide the current policy according to pheromone levels. this should be done only at the end of a chain, before selecting the new one
    if (getCurrentPhaseDef().isCommit()) {
        decidePolicy();
    }

    DBG(
        std::ostringstream str; str << "tlsID=" << getID() << " currentPolicyname=" + getCurrentPolicy()->getName(); WRITE_MESSAGE(str.str());)

    //Execute current policy. congestion "policy" must maintain the commit phase, and that must be an all-red one
    return getCurrentPolicy()->decideNextPhase(getCurrentPhaseElapsed(),
            &getCurrentPhaseDef(), getCurrentPhaseIndex(),
            getPhaseIndexWithMaxCTS(), isThresholdPassed(), isPushButtonPressed(),
            countVehicles(getCurrentPhaseDef()));
}

SUMOReal MSDeterministicHiLevelTrafficLightLogic::getMeanSpeedForInputLanes() {
    if (inputLanes.size() == 0) {
        return 0;
    }
    SUMOReal vSpeedInTot = 0;
    for (MSLaneID_set::iterator laneIterator = inputLanes.begin();
            laneIterator != inputLanes.end(); laneIterator++) {
        std::string laneId = *laneIterator;
        SUMOReal maxSpeed = getSensors()->meanVehiclesSpeed(laneId);
        if (maxSpeed > -1) {
            vSpeedInTot += (13.89 - maxSpeed) * 10. / 13.89;
        }
        DBG(
            std::ostringstream i_str; i_str << " meanVehiclesSpeed " << maxSpeed << " inputLane " << laneId << " ID " << getID() << " ."; WRITE_MESSAGE(time2string(MSNet::getInstance()->getCurrentTimeStep()) + " MSDeterministicHiLevelTrafficLightLogic::getMeanSpeedForInputLanes:: in" + i_str.str());)
    }
    return vSpeedInTot / inputLanes.size();
}

SUMOReal MSDeterministicHiLevelTrafficLightLogic::getMeanSpeedForOutputLanes() {
    if (outputLanes.size() == 0) {
        return 0;
    }
    SUMOReal vSpeedOutTot = 0;
    for (MSLaneID_set::iterator laneIterator = outputLanes.begin();
            laneIterator != outputLanes.end(); laneIterator++) {
        std::string laneId = *laneIterator;
        SUMOReal maxSpeed = getSensors()->meanVehiclesSpeed(laneId);
        if (maxSpeed > -1) {
            vSpeedOutTot += (13.89 - maxSpeed) * 10. / 13.89;
        }
        DBG(
            std::ostringstream i_str; i_str << " meanVehiclesSpeed " << maxSpeed << " outputLane " << laneId << " ID " << getID() << " ."; WRITE_MESSAGE(time2string(MSNet::getInstance()->getCurrentTimeStep()) + " MSDeterministicHiLevelTrafficLightLogic::getMeanSpeedForOutputLanes:: out" + i_str.str());)
    }
    return vSpeedOutTot / outputLanes.size();
}

void MSDeterministicHiLevelTrafficLightLogic::decidePolicy() {
    // Decide if it is the case to check for another plan
    SUMOReal mean_vSpeed_in = getMeanSpeedForInputLanes();
    SUMOReal mean_vSpeed_out = getMeanSpeedForOutputLanes();
    MSSOTLPolicy* oldPolicy = getCurrentPolicy();
    choosePolicy(mean_vSpeed_in, mean_vSpeed_out);
    MSSOTLPolicy* newPolicy = getCurrentPolicy();

    if (newPolicy != oldPolicy) {
        SUMOTime step = MSNet::getInstance()->getCurrentTimeStep();
        DBG(
            std::ostringstream phero_str; phero_str << " (mean_vSpeed_in= " << mean_vSpeed_in << " ,mean_vSpeed_out= " << mean_vSpeed_out << " )"; WRITE_MESSAGE("TL " + getID() + " time " + time2string(step) + " Policy: " + newPolicy->getName() + phero_str.str() + " OldPolicy: " + oldPolicy->getName() + " id " + getID() + " .");)
    } else { //debug purpose only
        DBG(
            std::ostringstream phero_str; phero_str << " (mean_vSpeed_in= " << mean_vSpeed_in << " ,mean_vSpeed_out= " << mean_vSpeed_out << " )"; SUMOTime step = MSNet::getInstance()->getCurrentTimeStep(); WRITE_MESSAGE("TL " + getID() + " time " + time2string(step) + " Policy: Nochanges" + phero_str.str() + " OldPolicy: " + oldPolicy->getName() + " id " + getID() + " .");)
    }

}

void MSDeterministicHiLevelTrafficLightLogic::choosePolicy(
    SUMOReal mean_vSpeed_in, SUMOReal mean_vSpeed_out) {

    int index_maxStimulus = 0;
    SUMOReal maxStimulus = -1;
    // Compute simulus for each policy
    for (int i = 0; i < (int)getPolicies().size(); i++) {
        SUMOReal stimulus = getPolicies()[i]->computeDesirability(mean_vSpeed_in,
                            mean_vSpeed_out);
        if (stimulus > maxStimulus) {
            maxStimulus = stimulus;
            index_maxStimulus = i;
        }
        DBG(
            std::ostringstream so_str; so_str << " policy " << getPolicies()[i]->getName() << " stimulus " << stimulus; WRITE_MESSAGE("MSDeterministicHiLevelTrafficLightLogic::choosePolicy::" + so_str.str());)

    }
    activate(getPolicies()[index_maxStimulus]);

}

bool MSDeterministicHiLevelTrafficLightLogic::canRelease() {
    DBG(
        std::ostringstream phero_str; phero_str << "getCurrentPhaseElapsed()=" << time2string(getCurrentPhaseElapsed()) << " isThresholdPassed()=" << isThresholdPassed() << " currentPhase=" << (&getCurrentPhaseDef())->getState() << " countVehicles()=" << countVehicles(getCurrentPhaseDef()); WRITE_MESSAGE("\nMSDeterministicHiLevelTrafficLightLogic::canRelease(): " + phero_str.str());)
    return getCurrentPolicy()->canRelease(getCurrentPhaseElapsed(),
                                          isThresholdPassed(), isPushButtonPressed(), &getCurrentPhaseDef(),
                                          countVehicles(getCurrentPhaseDef()));
}
