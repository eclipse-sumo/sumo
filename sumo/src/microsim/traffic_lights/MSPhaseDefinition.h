/****************************************************************************/
/// @file    MSPhaseDefinition.h
/// @author  Daniel Krajzewicz
/// @author  Julia Ringel
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Jan 2004
/// @version $Id$
///
// The definition of a single phase of a tls logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSPhaseDefinition_h
#define MSPhaseDefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <bitset>
#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSLink.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPhaseDefinition
 * @brief The definition of a single phase of a tls logic
 */
class MSPhaseDefinition {
public:
    /// @brief The duration of the phase
    SUMOTime duration;

    /// @brief The minimum duration of the phase
    SUMOTime minDuration;

    /// @brief The maximum duration of the phase
    SUMOTime maxDuration;

    /// @brief Stores the timestep of the last on-switched of the phase
    SUMOTime myLastSwitch;


public:
    /** @brief Constructor
     *
     * minDuration and maxDuration are set to duration.
     *
     * @param[in] durationArg The duration of the phase
     * @param[in] stateArg The state in the phase
     */
    MSPhaseDefinition(SUMOTime durationArg, const std::string& stateArg)
        : duration(durationArg), minDuration(durationArg), maxDuration(durationArg),
          myLastSwitch(0), state(stateArg) {
        myLastSwitch = string2time(OptionsCont::getOptions().getString("begin"));
    }


    /** @brief Constructor
     *
     * @param[in] durationArg The duration of the phase
     * @param[in] minDurationArg The minimum duration of the phase
     * @param[in] maxDurationArg The maximum duration of the phase
     * @param[in] stateArg The state in the phase
     */
    MSPhaseDefinition(SUMOTime durationArg, SUMOTime minDurationArg, SUMOTime maxDurationArg,
                      const std::string& stateArg)
        : duration(durationArg),
          myLastSwitch(0), state(stateArg) {
        myLastSwitch = string2time(OptionsCont::getOptions().getString("begin"));
        minDuration = minDurationArg < 0 ? durationArg : minDurationArg;
        maxDuration = maxDurationArg < 0 ? durationArg : maxDurationArg;
    }


    /// @brief Destructor
    virtual ~MSPhaseDefinition() { }


    /** @brief Returns the state within this phase
     * @return The state in this phase
     */
    const std::string& getState() const {
        return state;
    }


    /** @brief Returns whether this phase is a pure "green" phase
     *
     * "pure green" means in this case that at least one stream has green
     *  and no stream has yellow. Such phases are meant to be candidates
     *  for being stretched by actuated or agentbased traffic light logics.
     * @return Whether this phase is a "pure green" phase
     */
    bool isGreenPhase() const {
        if (state.find_first_of("gG") == std::string::npos) {
            return false;
        }
        if (state.find_first_of("yY") != std::string::npos) {
            return false;
        }
        return true;
    }


    /** @brief Returns the state of the tls signal at the given position
     * @param[in] pos The position of the signal to return the state for
     * @return The state of the signal at the given position
     */
    LinkState getSignalState(unsigned int pos) const {
        return (LinkState) state[pos];
    }


    /** @brief Comparison operator
     *
     * Note that only the state must differ, not the duration!
     * @param[in] pd The phase definition to compare against
     * @return Whether the given phase definition differs
     */
    bool operator!=(const MSPhaseDefinition& pd) {
        return state != pd.state;
    }


private:
    /// @brief The phase definition
    std::string state;


};

#endif

/****************************************************************************/

