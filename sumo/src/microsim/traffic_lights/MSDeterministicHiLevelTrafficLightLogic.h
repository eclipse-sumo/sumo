/****************************************************************************/
/// @file    MSDeterministicHiLevelTrafficLightLogic.h
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
#ifndef MSDeterministicHiLevelTrafficLightLogic_h
#define MSDeterministicHiLevelTrafficLightLogic_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] parameters Parameters defined for the tll
     */
    MSDeterministicHiLevelTrafficLightLogic(MSTLLogicControl& tlcontrol, const std::string& id,
                                            const std::string& subid, const Phases& phases, int step,
                                            SUMOTime delay,
                                            const std::map<std::string, std::string>& parameters);

    ~MSDeterministicHiLevelTrafficLightLogic();

    /**
     * @brief Initialises the tls with sensors on incoming and outgoing lanes
     * Sensors are built in the simulation according to the type of sensor specified in the simulation parameter
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder& nb) throw(ProcessError);

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
    SUMOReal getMeanSpeedForInputLanes();

    /*
     * @return The average pheromone level regarding congestion on output lanes
     */
    SUMOReal getMeanSpeedForOutputLanes();



    /**
     * @brief Decide the current policy according to pheromone levels
     * The decision reflects on currentPolicy value
     */
    void decidePolicy();

    void choosePolicy(SUMOReal mean_vSpeed_in, SUMOReal mean_vSpeed_out);


};

#endif
/****************************************************************************/
