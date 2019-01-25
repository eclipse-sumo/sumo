/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDeterministicHiLevelTrafficLightLogic.h
/// @author  Riccardo Belletti
/// @date    Mar 2014
/// @version $Id$
///
// The class for deterministic high level traffic light logic
/****************************************************************************/
#ifndef MSDeterministicHiLevelTrafficLightLogic_h
#define MSDeterministicHiLevelTrafficLightLogic_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include "MSSOTLHiLevelTrafficLightLogic.h"
#include "MSSOTLPhasePolicy.h"
#include "MSSOTLPlatoonPolicy.h"
#include "MSSOTLMarchingPolicy.h"
#include "MSSOTLCongestionPolicy.h"
#include "MSSOTLPolicy3DStimulus.h"

class MSDeterministicHiLevelTrafficLightLogic: public MSSOTLHiLevelTrafficLightLogic {
public:


    //****************************************************

    /**
     * @brief Constructor without sensors passed
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This traffic light id
     * @param[in] programID This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] parameters Parameters defined for the tll
     */
    MSDeterministicHiLevelTrafficLightLogic(MSTLLogicControl& tlcontrol, const std::string& id,
                                            const std::string& programID, const Phases& phases, int step,
                                            SUMOTime delay,
                                            const std::map<std::string, std::string>& parameters);

    ~MSDeterministicHiLevelTrafficLightLogic();

    /**
     * @brief Initialises the tls with sensors on incoming and outgoing lanes
     * Sensors are built in the simulation according to the type of sensor specified in the simulation parameter
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder& nb);

    /** @brief Returns the type of the logic as a string
    * @return The type of the logic
    */
    const std::string getLogicType() const {
        return "DeterministicHighLevelTrafficLightLogic";
    }
    /// @}

protected:

    /**
     * \brief This pheronome is an indicator of congestion on input lanes.\n
     * Its levels refer to the average speed of vehicles passing the input lane:
     * the lower the speed the higher the pheromone.\n
     * These levels are updated on every input lane, independently on lights state.
     */
    MSLaneID_set inputLanes;

    /**
     * \brief This pheromone is an indicator of congestion on output lanes.\n
     * Its levels refer to the average speed of vehicles passing the output lane:
     * the lower the speed the higher the pheromone.\n
     * These levels are updated on every output lane, independently on lights state.
     */
    MSLaneID_set outputLanes;

    /*
     * This member has to contain the switching logic for SOTL policies
     */
    int decideNextPhase();

    bool canRelease();


    /*
     * @return The average pheromone level regarding congestion on input lanes
     */
    double getMeanSpeedForInputLanes();

    /*
     * @return The average pheromone level regarding congestion on output lanes
     */
    double getMeanSpeedForOutputLanes();



    /**
     * @brief Decide the current policy according to pheromone levels
     * The decision reflects on currentPolicy value
     */
    void decidePolicy();

    void choosePolicy(double mean_vSpeed_in, double mean_vSpeed_out);


};

#endif
/****************************************************************************/
