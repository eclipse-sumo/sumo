/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    MSPhaseDefinition.h
/// @author  Daniel Krajzewicz
/// @author  Julia Ringel
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Jan 2004
///
// The definition of a single phase of a tls logic
/****************************************************************************/
#pragma once
#include <config.h>

#define TARGET_BIT 0
#define TRANSIENT_NOTDECISIONAL_BIT 1
#define COMMIT_BIT 2
#define UNDEFINED_BIT 3
#include <config.h>

#include <bitset>
#include <string>
#include <vector>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPhaseDefinition
 * @brief The definition of a single phase of a tls logic
 */


class MSPhaseDefinition {
public:

    static const SUMOTime UNSPECIFIED_DURATION = -1;

    /// @brief The duration of the phase
    SUMOTime duration;

    /// @brief The previous duration of the phase
    SUMOTime lastDuration;

    /// @brief The minimum duration of the phase
    SUMOTime minDuration;

    /// @brief The maximum duration of the phase
    SUMOTime maxDuration;

    /// @brief The minimum time within the cycle for switching (for coordinated actuation)
    SUMOTime earliestEnd;

    /// @brief The maximum time within the cycle for switching (for coordinated actuation)
    SUMOTime latestEnd;

    /// @brief Stores the timestep of the last on-switched of the phase
    SUMOTime myLastSwitch;

    /// @brief The index of the phase that suceeds this one (or -1)
    std::vector<int> nextPhases;

    /// @brief Optional name or description for the current phase
    std::string name;

    /// @brief for NEMA phase
    SUMOTime yellow;

    /// @brief for NEMA phase
    SUMOTime red;

    /// @brief for NEMA phase
    SUMOTime vehext;

    /// @brief the phase is a transient one or a decisional one, compulsory directive for SOTL policies
    bool myTransientNotDecisional;

    /// @brief the phase is a commit, compulsory directive for SOTL policies
    bool myCommit;

    /// @brief Leaving the phase type as "undefined" lets SOTL policies malfunction
    bool myUndefined;

    /*
     * @brief The lanes-set
     * This array can be empty if this phase is not a target step,
     * otherwise, a bit is true if the corresponding lane belongs to a
     * set of input lanes.
     * SOTL traffic light logics choose the target step according to sensors
     * belonging to the lane-set.
     */
    std::vector<std::string> myTargetLaneSet;

private:
    /// @brief The phase definition
    std::string myState;


public:
    /** @brief Constructor **/
    MSPhaseDefinition(SUMOTime _duration, const std::string& state, const std::string& _name = ""):
        duration(_duration),
        lastDuration(UNSPECIFIED_DURATION),
        minDuration(duration),
        maxDuration(duration),
        earliestEnd(UNSPECIFIED_DURATION),
        latestEnd(UNSPECIFIED_DURATION),
        myLastSwitch(UNSPECIFIED_DURATION),
        name(_name),
        yellow(UNSPECIFIED_DURATION),
        red(UNSPECIFIED_DURATION),
        vehext(UNSPECIFIED_DURATION),
        myTransientNotDecisional(false),
        myCommit(false),
        myUndefined(false),
        myState(state)
        {}


    /// @brief Destructor
    virtual ~MSPhaseDefinition() { }


    /** @brief Returns the state within this phase
     * @return The state in this phase
     */
    const std::string& getState() const {
        return myState;
    }

    void setState(const std::string& _state) {
        myState = _state;
    }

    const std::vector<std::string>& getTargetLaneSet() const {
        return myTargetLaneSet;
    }

    const std::vector<int>& getNextPhases() const {
        return nextPhases;
    }

    const std::string& getName() const {
        return name;
    }

    void setName(const std::string& _name) {
        name = _name;
    }

    /** @brief Returns whether this phase is a pure "green" phase
     *
     * "pure green" means in this case that at least one stream has green
     *  and no stream has yellow. Such phases are meant to be candidates
     *  for being stretched by actuated or agentbased traffic light logics.
     * @return Whether this phase is a "pure green" phase
     */
    bool isGreenPhase() const {
        if (myState.find_first_of("gG") == std::string::npos) {
            return false;
        }
        if (myState.find_first_of("yY") != std::string::npos) {
            return false;
        }
        return true;
    }


    /** @brief Returns the state of the tls signal at the given position
     * @param[in] pos The position of the signal to return the state for
     * @return The state of the signal at the given position
     */
    LinkState getSignalState(int pos) const {
        return (LinkState) myState[pos];
    }

    inline bool isActuted() const {
        return minDuration != maxDuration;
    }

    /** @brief Comparison operator
     *
     * Note that only the state must differ, not the duration!
     * @param[in] pd The phase definition to compare against
     * @return Whether the given phase definition differs
     */
    bool operator!=(const MSPhaseDefinition& pd) {
        return myState != pd.myState;
    }


    /*
    * @return true if the phase type is undefined
    */
    bool isUndefined() const {
        return myUndefined;
    }

    /*
    * @return true if this is a target phase
    */
    bool isTarget() const {
        return !myTargetLaneSet.empty();
    }

    /*
    * @return true if this is a transient phase
    */
    bool isTransient() const {
        return myTransientNotDecisional;
    }

    /*
    * @return true if this is a decisional phase
    */
    bool isDecisional() const {
        return !myTransientNotDecisional;
    }

    /*
    * @return true if this is a commit phase
    */
    bool isCommit() const {
        return myCommit;
    }

};
