/****************************************************************************/
/// @file    MSSOTLHiLevelTrafficLightLogic.h
/// @author  Alessio Bonfietti
/// @author  Riccardo Belletti
/// @author  Anna Chiara Bellini
/// @author  Federico Caselli
/// @date    Jun 2013
/// @version $Id$
///
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
#ifndef MSSOTLTrafficLightLogic_H
#define MSSOTLTrafficLightLogic_H

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
#include <vector>
#include "MSSOTLPolicy.h"
#include <stdlib.h>
#include <sstream>

#include "MSSOTLTrafficLightLogic.h"

/**
 * @class MSSOTLHiLevelTrafficLightLogic
 * @brief A self-organizing high-level traffic light logic
 *
 * A base class for a high-level traffic light logic able
 * to manage different low-level policies for different
 * traffic conditions.
 */

class MSSOTLHiLevelTrafficLightLogic: public MSSOTLTrafficLightLogic {
public:
    /**
     * @brief Constructor without sensors passed
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This traffic light id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] parameters Parameters defined for the tll
     */
    MSSOTLHiLevelTrafficLightLogic(MSTLLogicControl& tlcontrol,
                                   const std::string& id, const std::string& subid, const Phases& phases,
                                   int step, SUMOTime delay,
                                   const std::map<std::string, std::string>& parameters);

    /**
     * @brief Constructor with sensors passed
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] parameters Parameters defined for the tll
     * @param[in] sensors The already defined sensor logic
     */
    MSSOTLHiLevelTrafficLightLogic(MSTLLogicControl& tlcontrol,
                                   const std::string& id, const std::string& subid, const Phases& phases,
                                   int step, SUMOTime delay,
                                   const std::map<std::string, std::string>& parameters,
                                   MSSOTLSensors* sensors);
    ~MSSOTLHiLevelTrafficLightLogic();

    /**
     * \brief Returns the vector of the low-level policies used by this high-level tll.
     */
    std::vector<MSSOTLPolicy*>& getPolicies() {
        return policies;
    }
    /**
     * \brief Returns the low-level policy currently selected by this high-level tll.
     */
    MSSOTLPolicy* getCurrentPolicy() {
        return currentPolicy;
    }

    /**
     * @brief Initialises the tls
     * @param[in] nb The detector builder
     */
    void init(NLDetectorBuilder& nb) throw(ProcessError);

    /*
     * \brief Adds a low-level policy to this high-level tll.
     * \param[in] policy The low-level policy to be added.
     */
    void addPolicy(MSSOTLPolicy* policy);

    /*
     * \brief Activates the given low-level policy.
     * \param[in] policy The low-level policy to be activated.
     */
    void activate(MSSOTLPolicy* policy);

protected:
    virtual void decidePolicy() = 0;

private:

    std::vector<MSSOTLPolicy*> policies;
    MSSOTLPolicy* currentPolicy;

};

#endif
