/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSActuatedTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// An actuated (adaptive) traffic light logic
/****************************************************************************/
#ifndef MSActuatedTrafficLightLogic_h
#define MSActuatedTrafficLightLogic_h


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
#include <map>
#include <microsim/MSEventControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include "MSSimpleTrafficLightLogic.h"
#include <microsim/output/MSInductLoop.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NLDetectorBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSActuatedTrafficLightLogic
 * @brief An actuated (adaptive) traffic light logic
 */
class MSActuatedTrafficLightLogic : public MSSimpleTrafficLightLogic {
public:
    /// @brief Definition of a map from lanes to induct loops lying on them
    typedef std::map<MSLane*, MSDetectorFileOutput*> InductLoopMap;

public:
    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] programID This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] parameter The parameter to use for tls set-up
     */
    MSActuatedTrafficLightLogic(MSTLLogicControl& tlcontrol,
                                const std::string& id, const std::string& programID,
                                const MSSimpleTrafficLightLogic::Phases& phases,
                                int step, SUMOTime delay,
                                const std::map<std::string, std::string>& parameter,
                                const std::string& basePath);


    /** @brief Initialises the tls with information about incoming lanes
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder& nb);


    /// @brief Destructor
    ~MSActuatedTrafficLightLogic();



    /// @name Switching and setting current rows
    /// @{

    /** @brief Switches to the next phase
     * @return The time of the next switch
     * @see MSTrafficLightLogic::trySwitch
     */
    SUMOTime trySwitch();
    /// @}


protected:
    /// @name "actuated" algorithm methods
    /// @{

    /** @brief Returns the minimum duration of the current phase
     * @param[in] detectionGap The minimum detection gap for the current phase
     * @return The minimum duration of the current phase
     */
    SUMOTime duration(const double detectionGap) const;

    /// @brief get the minimum min duration for all stretchable phases
    SUMOTime getMinimumMinDuration() const;

    /** @brief Return the minimum detection gap of all detectors if the current phase should be extended and double::max otherwise
     */
    double gapControl();
    /// @}


protected:
    /// A map from lanes to induct loops lying on them
    InductLoopMap myInductLoops;

    /// The maximum gap to check in seconds
    double myMaxGap;

    /// The passing time used in seconds
    double myPassingTime;

    /// The detector distance in seconds
    double myDetectorGap;

    /// Whether the detectors shall be shown in the GUI
    bool myShowDetectors;

    /// The output file for generated detectors
    std::string myFile;

    /// The frequency for aggregating detector output
    SUMOTime myFreq;

    /// Whether detector output separates by vType
    std::string myVehicleTypes;

};


#endif

/****************************************************************************/

