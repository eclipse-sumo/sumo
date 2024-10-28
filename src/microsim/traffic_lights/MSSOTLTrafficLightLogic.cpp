/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSSOTLTrafficLightLogic.cpp
/// @author  Gianfilippo Slager
/// @author  Anna Chiara Bellini
/// @author  Federico Caselli
/// @date    Apr 2013
///
// The base abstract class for SOTL logics
/****************************************************************************/

#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include "MSPushButton.h"
#include "MSSOTLTrafficLightLogic.h"
//#define SWARM_DEBUG
//#define ANALYSIS_DEBUG

// ===========================================================================
// member method definitions
// ===========================================================================
MSSOTLTrafficLightLogic::MSSOTLTrafficLightLogic(
    MSTLLogicControl& tlcontrol,
    const std::string& id,
    const std::string& programID,
    const TrafficLightType logicType,
    const Phases& phases,
    int step,
    SUMOTime delay,
    const Parameterised::Map& parameters) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, 0, logicType, phases, step, delay, parameters) {
    this->mySensors = nullptr;
    this->myCountSensors = nullptr;
    sensorsSelfBuilt = true;
    checkPhases();
    setupCTS();
    setToATargetPhase();
}

MSSOTLTrafficLightLogic::MSSOTLTrafficLightLogic(
    MSTLLogicControl& tlcontrol,
    const std::string& id,
    const std::string& programID,
    const TrafficLightType logicType,
    const Phases& phases,
    int step,
    SUMOTime delay,
    const Parameterised::Map& parameters,
    MSSOTLSensors* sensors) :
    MSSimpleTrafficLightLogic(tlcontrol, id, programID, 0, logicType, phases, step, delay, parameters) {
    this->mySensors = sensors;
    sensorsSelfBuilt = false;
    checkPhases();
    setupCTS();
    setToATargetPhase();
}

MSSOTLTrafficLightLogic::~MSSOTLTrafficLightLogic() {
    for (PhasePushButtons::iterator mapIt = m_pushButtons.begin(); mapIt != m_pushButtons.end(); ++mapIt)
        for (std::vector<MSPushButton*>::iterator vIt = mapIt->second.begin(); vIt != mapIt->second.end(); ++vIt) {
            delete *vIt;
        }
    m_pushButtons.clear();
    if (sensorsSelfBuilt) {
        delete mySensors;
//		delete myCountSensors;
    }
}

void MSSOTLTrafficLightLogic::logStatus() {

}

void
MSSOTLTrafficLightLogic::checkPhases() {
    for (int step = 0; step < (int)getPhases().size(); step++) {
        if (getPhase(step).isUndefined()) {
            MsgHandler::getErrorInstance()->inform("Step " + toString(step) + " of traffic light logic " + myID + " phases declaration has its type undeclared!");
        }
    }
}

void
MSSOTLTrafficLightLogic::setupCTS() {
    for (int phaseStep = 0; phaseStep < (int)getPhases().size(); phaseStep++) {
        if (getPhase(phaseStep).isTarget()) {
            targetPhasesCTS[phaseStep] = 0;
            lastCheckForTargetPhase[phaseStep] = MSNet::getInstance()->getCurrentTimeStep();
            targetPhasesLastSelection[phaseStep] = 0;
        }
    }
}

void
MSSOTLTrafficLightLogic::setToATargetPhase() {
    for (int step = 0; step < (int)getPhases().size(); step++) {
        if (getPhase(step).isTarget()) {
            setStep(step);
            lastChain = step;
            return;
        }
    }
    MsgHandler::getErrorInstance()->inform("No phase of type target found for traffic light logic " + myID + " The logic could malfunction. Check phases declaration.");
}


void
MSSOTLTrafficLightLogic::init(NLDetectorBuilder& nb) {

    MSTrafficLightLogic::init(nb);

    if (isDecayThresholdActivated()) {
        decayThreshold = 1;
    }
    if (sensorsSelfBuilt) {
        //Building SOTLSensors
        switch (SENSORS_TYPE) {
            case SENSORS_TYPE_E1:
                assert(0); // Throw exception because TLS can only handle E2 sensors
            case SENSORS_TYPE_E2:

                //Adding Sensors to the ingoing Lanes

                LaneVectorVector lvv = getLaneVectors();

#ifdef SWARM_DEBUG
                WRITE_MESSAGE("Listing lanes for TLS " + getID());

                for (int i = 0; i < lvv.size(); i++) {
                    LaneVector lv = lvv[i];

                    for (int j = 0; j < lv.size(); j++) {
                        MSLane* lane = lv[j];
                        WRITE_MESSAGE(lane ->getID());
                    }
                }
#endif

                mySensors = new MSSOTLE2Sensors(myID, &(getPhases()));
                ((MSSOTLE2Sensors*)mySensors)->buildSensors(myLanes, nb, getInputSensorsLength());
                mySensors->stepChanged(getCurrentPhaseIndex());
                if (getParameter("USE_VEHICLE_TYPES_WEIGHTS", "0") == "1") {
                    ((MSSOTLE2Sensors*) mySensors)->setVehicleWeigths(getParameter("VEHICLE_TYPES_WEIGHTS", ""));
                }

                //threshold speed param for tuning with irace
                ((MSSOTLE2Sensors*)mySensors)->setSpeedThresholdParam(getSpeedThreshold());

                myCountSensors = new MSSOTLE2Sensors(myID + "Count", &(getPhases()));
                myCountSensors->buildCountSensors(myLanes, nb);
                myCountSensors->stepChanged(getCurrentPhaseIndex());

                //Adding Sensors to the outgoing Lanes

                LinkVectorVector links = getLinks();

#ifdef SWARM_DEBUG
                WRITE_MESSAGE(TL("Listing output lanes"));
                for (int i = 0; i < links.size(); i++) {
                    LinkVector oneLink = getLinksAt(i);
                    for (int j = 0; j < oneLink.size(); j++) {
                        MSLane* lane  = oneLink[j]->getLane();
                        WRITE_MESSAGE(lane ->getID());
                    }
                }
#endif

                LaneVectorVector myLaneVector;
                LaneVector outLanes;
                LinkVectorVector myoutLinks = getLinks();

                for (int i = 0; i < (int)links.size(); i++) {
                    LinkVector oneLink = getLinksAt(i);
                    for (int j = 0; j < (int)oneLink.size(); j++) {
                        MSLane* lane  = oneLink[j]->getLane();
                        outLanes.push_back(lane);
                    }
                }

                if (outLanes.size() > 0) {
                    myLaneVector.push_back(outLanes);
                }
                if (myLaneVector.size() > 0) {
                    ((MSSOTLE2Sensors*)mySensors)->buildOutSensors(myLaneVector, nb, getOutputSensorsLength());
                    myCountSensors->buildCountOutSensors(myLaneVector, nb);
                }

        }
    }
}


void
MSSOTLTrafficLightLogic::resetCTS(int phaseStep) {
    std::map<int, SUMOTime>::iterator phaseIterator = targetPhasesCTS.find(phaseStep);
    if (phaseIterator != targetPhasesCTS.end()) {
        phaseIterator->second = 0;
        lastCheckForTargetPhase[phaseStep] = MSNet::getInstance()->getCurrentTimeStep();
    }
}

void
MSSOTLTrafficLightLogic::updateCTS() {
    SUMOTime elapsedTimeSteps = 0;
    SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    //Iterate over the target phase map and update CTS value for every target phase except for the one belonging to the current steps chain
    for (std::map<int, SUMOTime>::iterator mapIterator = targetPhasesCTS.begin();
            mapIterator != targetPhasesCTS.end();
            mapIterator++) {
        int chain = mapIterator->first;
        SUMOTime oldVal = mapIterator->second;
        if (chain != lastChain) {
            //Get the number of timesteps since the last check for that phase
            elapsedTimeSteps = now - lastCheckForTargetPhase[chain];
            //Update the last check time
            lastCheckForTargetPhase[chain] = now;
            //Increment the CTS
            //SWITCH between 3 counting vehicles function
            switch (getMode()) {
                case (0):
                    mapIterator->second += elapsedTimeSteps
                                           * countVehicles(getPhase(chain)); //SUMO
                    break;
                case (1):
                    mapIterator->second += elapsedTimeSteps
                                           * countVehicles(getPhase(chain)); //COMPLEX
                    break;
                case (2):
                    mapIterator->second = countVehicles(getPhase(chain)); //QUEUE
                    break;
                default:
                    WRITE_ERROR(TL("Unrecognized traffic threshold calculation mode"));
            }
            std::ostringstream oss;
            oss << "MSSOTLTrafficLightLogic::updateCTS->TLC " << getID() << " chain " << chain << " oldVal " << oldVal << " newVal " << mapIterator->second;
            WRITE_MESSAGE(oss.str());
        }
        if (isDecayThresholdActivated()) {
            updateDecayThreshold();
        }
    }
}

int
MSSOTLTrafficLightLogic::countVehicles(MSPhaseDefinition phase) {

    if (!phase.isTarget()) {
        return 0;
    }

    int accumulator = 0;
    //Iterate over the target lanes for the current target phase to get the number of approaching vehicles
    for (const std::string& lane : phase.getTargetLaneSet()) {
        //SWITCH between 3 counting vehicles function
        switch (getMode()) {
            case (0):
                accumulator += mySensors->countVehicles(lane); //SUMO
                break;
            case (1):
                accumulator += ((MSSOTLE2Sensors*)mySensors)->estimateVehicles(lane);  //COMPLEX
                break;
            case (2):
                accumulator = MAX2((int)((MSSOTLE2Sensors*)mySensors)->getEstimateQueueLength(lane), accumulator);  //QUEUE
                break;
            default:
                WRITE_ERROR(TL("Unrecognized traffic threshold calculation mode"));
        }
    }
    return accumulator;
}

void
MSSOTLTrafficLightLogic::updateDecayThreshold() {
    if (getCurrentPhaseDef().isGreenPhase()) {
        decayThreshold = decayThreshold * exp(getDecayConstant());
    }
#ifdef SWARM_DEBUG
    std::stringstream out;
    out << decayThreshold;
    WRITE_MESSAGE("\n" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + "\tMSSOTLTrafficLightLogic::updateDecayThreshold()::  " + out.str());
#endif
}
bool
MSSOTLTrafficLightLogic::isThresholdPassed() {
#ifdef SWARM_DEBUG
    //	WRITE_MESSAGEF(TL("\n% tlsid=%  //	WRITE_MESSAGEF(TL("\n% tlsid=" + getID()), ime2string(MSNet::getInstance()->getCurrentTimeStep()) +"\tMSSOTLTrafficLightLogic::isThresholdPassed()::  ", getID()), ime2string(MSNet::getInstance()->getCurrentTimeStep()) +"\tMSSOTLTrafficLightLogic::isThresholdPassed()::  ");

    std::ostringstream threshold_str;
    //	threshold_str << "tlsid=" << getID() << " targetPhaseCTS size=" << targetPhasesCTS.size();
//			threshold_str << "\n";
    WRITE_MESSAGE(threshold_str.str());
#endif
    /*
     * if a dynamic threshold based on the exponential decrease, if passed we force the phase change
     */
//	double random = ((double) RandHelper::rand(RAND_MAX) / (RAND_MAX));
    double random = RandHelper::rand();
//	ANALYSIS_DBG(
#ifdef SWARM_DEBUG
    if (isDecayThresholdActivated()) {
        std::ostringstream str;
        str << time2string(MSNet::getInstance()->getCurrentTimeStep()) << "\tMSSOTLTrafficLightLogic::isThresholdPassed()::  "
            << " tlsid=" << getID() << " decayThreshold=" << decayThreshold << " random=" << random << ">" << (1 - decayThreshold)
            << (random > (1 - decayThreshold) ? " true" : " false");

        WRITE_MESSAGE(str.str());
    }
#endif
    if (!isDecayThresholdActivated() || (isDecayThresholdActivated() && random > (1 - decayThreshold))) {
        for (std::map<int, SUMOTime>::const_iterator iterator =
                    targetPhasesCTS.begin(); iterator != targetPhasesCTS.end();
                iterator++) {
#ifdef SWARM_DEBUG
            SUMOTime step = MSNet::getInstance()->getCurrentTimeStep();
            std::ostringstream threshold_str;
            //	threshold_str <<"\tTL " +getID()<<" time=" +time2string(step)<< "(getThreshold()= " << getThreshold()
            //		<< ", targetPhaseCTS= " << iterator->second << " )" << " phase="<<getPhase(iterator->first).getState();
            threshold_str << getCurrentPhaseDef().getState() << ";" << time2string(step) << ";" << getThreshold()
                          << ";" << iterator->second << ";" << getPhase(iterator->first).getState() << ";"
                          << iterator->first << "!=" << lastChain;
            WRITE_MESSAGE(threshold_str.str());
#endif
            //Note that the current chain is not eligible to be directly targeted again, it would be unfair
            if ((iterator->first != lastChain) && (getThreshold() <= iterator->second)) {
                return true;
            }
        }
        return false;
    } else {
        return true;
    }
}


SUMOTime
MSSOTLTrafficLightLogic::getCurrentPhaseElapsed() {
    MSPhaseDefinition currentPhase = getCurrentPhaseDef();

    SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    SUMOTime elapsed = now - currentPhase.myLastSwitch;

    return elapsed;
}


int
MSSOTLTrafficLightLogic::getPhaseIndexWithMaxCTS() {
    SUMOTime maxCTS = 0;
    int maxLastStep = getTargetPhaseMaxLastSelection();
    bool usedMaxCTS = false;
    std::vector<int> equalIndexes;
    for (std::map<int, int>::const_iterator it = targetPhasesLastSelection.begin();
            it != targetPhasesLastSelection.end(); ++it) {
        if (it->first != lastChain) {
            if (maxLastStep < it->second) {
                maxLastStep = it->second;
                equalIndexes.clear();
                equalIndexes.push_back(it->first);
            } else if (maxLastStep == it->second) {
                equalIndexes.push_back(it->first);
            }
        }
    }
    if (equalIndexes.size() == 0) {
        usedMaxCTS = true;
        for (std::map<int, SUMOTime>::const_iterator iterator = targetPhasesCTS.begin();
                iterator != targetPhasesCTS.end(); ++iterator) {
            if (iterator->first != lastChain) {
                if (maxCTS < iterator->second) {
                    maxCTS = iterator->second;
                    equalIndexes.clear();
                    equalIndexes.push_back(iterator->first);
                } else if (maxCTS == iterator->second) {
                    equalIndexes.push_back(iterator->first);
                }
            }
        }
    }

    std::ostringstream oss;
    oss << "MSSOTLTrafficLightLogic::getPhaseIndexWithMaxCTS-> TLC " << getID();
    if (usedMaxCTS) {
        oss << " maxCTS " << maxCTS;
    } else {
        oss << " forcing selection since not selected for " << maxLastStep;
    }
    if (equalIndexes.size() == 1) {
        oss << " phase " << equalIndexes[0];
        WRITE_MESSAGE(oss.str());
        return equalIndexes[0];
    } else {
        const int index = RandHelper::getRandomFrom(equalIndexes);
        oss << " phases [";
        for (std::vector<int>::const_iterator it = equalIndexes.begin(); it != equalIndexes.end(); ++it) {
            oss << *it << ", ";
        }
        oss << "]. Random select " << index;
        WRITE_MESSAGE(oss.str());
        return index;
    }
}

int
MSSOTLTrafficLightLogic::decideNextPhase() {
    MSPhaseDefinition currentPhase = getCurrentPhaseDef();
    //If the junction was in a commit step
    //=> go to the target step that gives green to the set with the current highest CTS
    //   and return computeReturnTime()
    if (currentPhase.isCommit()) {
        // decide which chain to activate. Gotta work on this
        return getPhaseIndexWithMaxCTS();
    }
    if (currentPhase.isTransient()) {
        //If the junction was in a transient step
        //=> go to the next step and return computeReturnTime()
        return getCurrentPhaseIndex() + 1;
    }

    if (currentPhase.isDecisional()) {

        if (canRelease()) {
            return getCurrentPhaseIndex() + 1;
        }
    }

    return getCurrentPhaseIndex();
}

SUMOTime
MSSOTLTrafficLightLogic::trySwitch() {
    if (MSNet::getInstance()->getCurrentTimeStep() % 1000 == 0) {
        WRITE_MESSAGE("MSSOTLTrafficLightLogic::trySwitch()");
        // To check if decideNextPhase changes the step
        int previousStep = getCurrentPhaseIndex() ;
#ifdef ANALYSIS_DEBUG
        SUMOTime elapsed = getCurrentPhaseElapsed();
#endif
        // Update CTS according to sensors
        updateCTS();

        // Invoking the function member, specialized for each SOTL logic
        setStep(decideNextPhase());
        MSPhaseDefinition currentPhase = getCurrentPhaseDef();

        //At the end, check if new step started
        if (getCurrentPhaseIndex() != previousStep) {
            //Check if a new steps chain started
            if (currentPhase.isTarget())  {
                //Reset CTS for the ending steps chain
                resetCTS(lastChain);
                //Update lastTargetPhase
                lastChain = getCurrentPhaseIndex();
                for (std::map<int, int>::iterator it = targetPhasesLastSelection.begin(); it != targetPhasesLastSelection.end(); ++ it) {
                    if (it->first == lastChain) {
                        if (it->second >= getTargetPhaseMaxLastSelection()) {
                            std::ostringstream oss;
                            oss << "Forced selection of the phase " << lastChain << " since its last selection was " << it->second << " changes ago";
                            WRITE_MESSAGE(oss.str())
                        }
                        it->second = 0;
                    } else if (it->first != previousStep) {
                        ++it->second;
                    }
                }
                if (isDecayThresholdActivated()) {
                    decayThreshold = 1;
                }
            }
            //Inform the sensors logic
            mySensors->stepChanged(getCurrentPhaseIndex());
            //Store the time the new phase started
            currentPhase.myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
            if (isDecayThresholdActivated()) {
                decayThreshold = 1;
            }
#ifdef ANALYSIS_DEBUG
            std::ostringstream oss;
            oss << getID() << " from " << getPhase(previousStep).getState() << " to " << currentPhase.getState() << " after " << time2string(elapsed);
            WRITE_MESSAGE(time2string(MSNet::getInstance()->getCurrentTimeStep()) + "\tMSSOTLTrafficLightLogic::trySwitch " + oss.str());
#endif
        }
    }
    return computeReturnTime();
}

bool MSSOTLTrafficLightLogic::isPushButtonPressed() {
    if (getParameter("USE_PUSH_BUTTON", "0") == "0") {
        return false;
    }
    const MSPhaseDefinition currentPhase = getCurrentPhaseDef();
    if (m_pushButtons.find(currentPhase.getState()) == m_pushButtons.end()) {
        m_pushButtons[currentPhase.getState()] = MSPedestrianPushButton::loadPushButtons(&currentPhase);
    }
    return MSPushButton::anyActive(m_pushButtons[currentPhase.getState()]);
}


void MSSOTLTrafficLightLogic::setStep(int step) {
    step = step % myPhases.size();
    if (myStep != step) {
        myStep = step;
        myPhases[myStep]->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
    }
}
