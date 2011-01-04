/****************************************************************************/
/// @file    MSActuatedTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// An actuated (adaptive) traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
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
class MSActuatedTrafficLightLogic :
            public MSSimpleTrafficLightLogic {
public:
    /// @brief Definition of a map from lanes to induct loops lying on them
    typedef std::map<MSLane*, MSInductLoop*> InductLoopMap;

public:
    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] programID This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] maxGap !!!
     * @param[in] passingTime !!!
     * @param[in] detectorGap !!!
     */
    MSActuatedTrafficLightLogic(MSTLLogicControl &tlcontrol,
                                const std::string &id, const std::string &programID,
                                const MSSimpleTrafficLightLogic::Phases &phases,
                                unsigned int step, SUMOTime delay,
                                SUMOReal maxGap, SUMOReal passingTime, SUMOReal detectorGap) throw();


    /** @brief Initialises the tls with information about incoming lanes
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder &nb) throw(ProcessError);


    /// @brief Destructor
    ~MSActuatedTrafficLightLogic() throw();



    /// @name Switching and setting current rows
    /// @{

    /** @brief Switches to the next phase
     * @param[in] isActive Whether this program is the currently used one
     * @return The time of the next switch
     * @see MSTrafficLightLogic::trySwitch
     */
    SUMOTime trySwitch(bool isActive) throw();
    /// @}


protected:
    /// @name "actuated" algorithm methods
    /// @{

    /** @brief Returns the duration of the given step
     * @return The wanted duration of the current step
     */
    SUMOTime duration() const throw();


    /** @brief Decides, whether a phase should be continued by checking the gaps of vehicles having green
     */
    void gapControl() throw();
    /// @}


protected:
    /// A map from lanes to induct loops lying on them
    InductLoopMap myInductLoops;

    /// information whether the current phase should be lenghtend
    bool myContinue;

    /// The maximum gap to check
    SUMOReal myMaxGap;

    /// The passing time used
    SUMOReal myPassingTime;

    /// The detector distance
    SUMOReal myDetectorGap;

};


#endif

/****************************************************************************/

