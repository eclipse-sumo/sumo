/****************************************************************************/
/// @file    MSSOTLPolicy.cpp
/// @author  Alessio Bonfietti
/// @author  Anna Chiara Bellini
/// @author  Federico Caselli
/// @date    Jun 2013
/// @version $Id$
///
// The class for low-level policy
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include "MSSOTLPolicy.h"
#include <math.h>
#include <typeinfo>
#include "utils/common/RandHelper.h"

double s2f(string str) {
    istringstream buffer(str);
    double temp;
    buffer >> temp;
    return temp;
}

void PushButtonLogic::init(std::string prefix, const Parameterised* parameterised) {
    m_prefix = prefix;
    m_pushButtonScaleFactor = s2f(parameterised->getParameter("PUSH_BUTTON_SCALE_FACTOR", "1"));
    WRITE_MESSAGE(m_prefix + "::PushButtonLogic::init use " + parameterised->getParameter("USE_PUSH_BUTTON", "0") + " scale " + parameterised->getParameter("PUSH_BUTTON_SCALE_FACTOR", "1"));
}

bool PushButtonLogic::pushButtonLogic(int elapsed, bool pushButtonPressed, const MSPhaseDefinition* stage) {
    //pushbutton logic
    if (pushButtonPressed && elapsed >= (stage->duration * m_pushButtonScaleFactor)) {
        //If the stage duration has been passed
//    DBG(
        std::ostringstream oss;
        oss << m_prefix << "::pushButtonLogic pushButtonPressed elapsed " << elapsed << " stage duration " << (stage->duration * m_pushButtonScaleFactor);
        WRITE_MESSAGE(oss.str());
//    );
        return true;
    }
    return false;
}

void SigmoidLogic::init(std::string prefix, const Parameterised* parameterised) {
    m_prefix = prefix;
    m_useSigmoid = parameterised->getParameter("PLATOON_USE_SIGMOID", "0") != "0";
    m_k = s2f(parameterised->getParameter("PLATOON_SIGMOID_K_VALUE", "1"));
//  DBG(
    WRITE_MESSAGE(m_prefix + "::SigmoidLogic::init use " + parameterised->getParameter("PLATOON_USE_SIGMOID", "0") + " k " + parameterised->getParameter("PLATOON_SIGMOID_K_VALUE", "1"));
//    for (int elapsed = 10; elapsed < 51; ++elapsed)
//    {
//        double sigmoidValue = 1.0 / (1.0 + exp(-m_k * (elapsed - 31)));
//        std::ostringstream oss;
//        oss << "elapsed " << elapsed << " value " << sigmoidValue;
//        WRITE_MESSAGE(oss.str())
//    }
//  )
}

bool SigmoidLogic::sigmoidLogic(int elapsed, const MSPhaseDefinition* stage, int vehicleCount) {
    //use the sigmoid logic
    if (m_useSigmoid && vehicleCount == 0) {
        double sigmoidValue = 1.0 / (1.0 + exp(-m_k * (elapsed / 1000 - stage->duration / 1000)));
        double rnd = RandHelper::rand();
//    DBG(
        std::ostringstream oss;
        oss << m_prefix << "::sigmoidLogic [k=" << m_k << " elapsed " << elapsed << " stage->duration " << stage->duration << " ] value "
            << sigmoidValue;
        oss << " rnd " << rnd << " retval " << (rnd < sigmoidValue ? "true" : "false");
        WRITE_MESSAGE(oss.str())
//    );
        return rnd < sigmoidValue;
    }
    return false;
}


MSSOTLPolicy::MSSOTLPolicy(string name,
                           const std::map<std::string, std::string>& parameters) :
    Parameterised(parameters), myName(name) {
    theta_sensitivity = 0;
}

MSSOTLPolicy::MSSOTLPolicy(string name,
                           MSSOTLPolicyDesirability* desirabilityAlgorithm) :
    Parameterised(), myName(name), myDesirabilityAlgorithm(
        desirabilityAlgorithm) {
    theta_sensitivity = 0;
}

MSSOTLPolicy::MSSOTLPolicy(string name,
                           MSSOTLPolicyDesirability* desirabilityAlgorithm,
                           const std::map<std::string, std::string>& parameters) :
    Parameterised(parameters), myName(name), myDesirabilityAlgorithm(
        desirabilityAlgorithm) {

    std::ostringstream key;
    key << "THETA_INIT";
    std::ostringstream def;
    def << "0.5";
    theta_sensitivity = s2f(getParameter(key.str(), def.str()));
}

MSSOTLPolicy::~MSSOTLPolicy(void) {
    int r = 0;
}

double MSSOTLPolicy::computeDesirability(double vehInMeasure, double vehOutMeasure, double vehInDispersionMeasure,	double vehOutDispersionMeasure) {

    DBG(
        std::ostringstream str; str << "\nMSSOTLPolicy::computeStimulus\n" << getName(); WRITE_MESSAGE(str.str());)

    return myDesirabilityAlgorithm->computeDesirability(vehInMeasure, vehOutMeasure, vehInDispersionMeasure, vehOutDispersionMeasure);

}

double MSSOTLPolicy::computeDesirability(double vehInMeasure, double vehOutMeasure) {

    DBG(
        std::ostringstream str; str << "\nMSSOTLPolicy::computeStimulus\n" << getName(); WRITE_MESSAGE(str.str());)

    return myDesirabilityAlgorithm->computeDesirability(vehInMeasure, vehOutMeasure, 0, 0);

}

size_t MSSOTLPolicy::decideNextPhase(int elapsed,
                                     const MSPhaseDefinition* stage, size_t currentPhaseIndex,
                                     size_t phaseMaxCTS, bool thresholdPassed, bool pushButtonPressed, int vehicleCount) {

    //If the junction was in a commit step
    //=> go to the target step that gives green to the set with the current highest CTS
    //   and return computeReturnTime()
    if (stage->isCommit()) {
        // decide which chain to activate. Gotta work on this
        return phaseMaxCTS;
    }
    if (stage->isTransient()) {
        //If the junction was in a transient step
        //=> go to the next step and return computeReturnTime()
        return currentPhaseIndex + 1;
    }

    if (stage->isDecisional()) {
        DBG(
            std::ostringstream phero_str;
            phero_str << "getCurrentPhaseElapsed()=" << time2string(elapsed) << " isThresholdPassed()=" << thresholdPassed << " countVehicles()=" << vehicleCount;
            WRITE_MESSAGE("MSSOTLPolicy::decideNextPhase: " + phero_str.str());
        )
        if (canRelease(elapsed, thresholdPassed, pushButtonPressed, stage, vehicleCount)) {
            return currentPhaseIndex + 1;
        }
    }

    return currentPhaseIndex;
}

/*
 bool MSSOTLPolicy::canRelease(int elapsed, bool thresholdPassed, const MSPhaseDefinition* stage, int vehicleCount) {
 if (getName().compare("request") == 0) {
 return elapsed > 3000 && thresholdPassed;
 } else if (getName().compare("phase") == 0) {
 return thresholdPassed && elapsed >= stage->minDuration;
 } else if (getName().compare("platoon") == 0) {
 return thresholdPassed && (vehicleCount == 0 || elapsed >= stage->maxDuration);
 } else if (getName().compare("marching") == 0) {
 return elapsed >= stage->duration;
 } else if (getName().compare("congestion") == 0) {
 return elapsed >= stage->minDuration;
 }
 return true; //

 }
 */

