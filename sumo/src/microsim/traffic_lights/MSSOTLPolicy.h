/****************************************************************************/
/// @file    MSSOTLPolicy.h
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @author  Anna Chiara Bellini
/// @author  Federico Caselli
/// @date    Jun 2013
/// @version $Id$
///
// The class for low-level policy
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef MSTLPolicy_h
#define MSTLPolicy_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include <sstream>
#include <math.h>
#include <utility>
#include <vector>
#include <utils/common/Parameterised.h>
#include "MSPhaseDefinition.h"
#include "MSSOTLPolicyDesirability.h"

class PushButtonLogic {
protected:
    void init(std::string prefix, const Parameterised* parameterised);

    bool pushButtonLogic(SUMOTime elapsed, bool pushButtonPressed, const MSPhaseDefinition* stage);

    SUMOReal m_pushButtonScaleFactor;
    std::string m_prefix;
};

class SigmoidLogic {
protected:
    void init(std::string prefix, const Parameterised* parameterised);

    bool sigmoidLogic(SUMOTime elapsed, const MSPhaseDefinition* stage, int vehicleCount);

    bool m_useSigmoid;
    SUMOReal m_k;
    std::string m_prefix;
};

/**
 * @class MSSOTLPolicy
 * @brief Class for a low-level policy.
 *
 */
class MSSOTLPolicy: public Parameterised {
private:

    /**
     * \brief The sensitivity of this policy
     */
    SUMOReal theta_sensitivity;
    /**
     * \brief The name of the policy
     */
    std::string myName;
    /**
     * \brief A pointer to the policy desirability object.\nIt's an optional component related to the computeDesirability() method and it's necessary
     * only when the policy is used in combination with an high level policy.
     */
    MSSOTLPolicyDesirability* myDesirabilityAlgorithm;

protected:
    virtual void init() {}

public:
    /** @brief Simple constructor
     * @param[in] name The name of the policy
     * @param[in] parameters Parameters defined for the policy
     */
    MSSOTLPolicy(std::string name,
                 const std::map<std::string, std::string>& parameters);
    /** @brief Constructor when the policy is a low-level policy used by an high level policy
     * @param[in] name The name of the policy
     * @param[in] desirabilityAlgorithm The desirability algorithm to be used for this policy
     */
    MSSOTLPolicy(std::string name, MSSOTLPolicyDesirability* desirabilityAlgorithm);
    /** @brief Constructor when the policy is a low-level policy used by an high level policy
     * @param[in] name The name of the policy
     * @param[in] desirabilityAlgorithm The desirability algorithm to be used for this policy
     * @param[in] parameters Parameters defined for the policy
     */
    MSSOTLPolicy(std::string name, MSSOTLPolicyDesirability* desirabilityAlgorithm,
                 const std::map<std::string, std::string>& parameters);
    virtual ~MSSOTLPolicy();

    virtual bool canRelease(SUMOTime elapsed, bool thresholdPassed, bool pushButtonPressed,
                            const MSPhaseDefinition* stage, int vehicleCount) = 0;
    virtual int decideNextPhase(SUMOTime elapsed, const MSPhaseDefinition* stage,
                                int currentPhaseIndex, int phaseMaxCTS, bool thresholdPassed, bool pushButtonPressed,
                                int vehicleCount);

    virtual SUMOReal getThetaSensitivity() {
        return theta_sensitivity;
    }
    virtual void setThetaSensitivity(SUMOReal val) {
        theta_sensitivity = val;
    }
    std::string getName() {
        return myName;
    }
    MSSOTLPolicyDesirability* getDesirabilityAlgorithm() {
        return myDesirabilityAlgorithm;
    }
    /**
     * @brief Computes the desirability of this policy, necessary when used in combination with an high level policy.
     */
    SUMOReal computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure, SUMOReal vehInDispersionMeasure, SUMOReal vehOutDispersionMeasure);

    SUMOReal computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure);
};

#endif
