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
    /// @brief The duration of the phase
    SUMOTime duration;

    /// @brief The previous duration of the phase
    SUMOTime lastDuration;

    /// @brief The minimum duration of the phase
    SUMOTime minDuration;

    /// @brief The maximum duration of the phase
    SUMOTime maxDuration;

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

private:
    /// @brief The phase definition
    std::string myState;

    /// @brief the phase is a transient one or a decisional one, compulsory directive for SOTL policies
    bool myTransientNotDecisional = false;

    /// @brief the phase is a commit, compulsory directive for SOTL policies
    bool myCommit = false;

    /// @brief Leaving the phase type as "undefined" lets SOTL policies malfunction
    bool myUndefined = true;

    /*
     * @brief The lanes-set
     * This array can be empty if this phase is not a target step,
     * otherwise, a bit is true if the corresponding lane belongs to a
     * set of input lanes.
     * SOTL traffic light logics choose the target step according to sensors
     * belonging to the lane-set.
     */
    std::vector<std::string> myTargetLaneSet;

    void init(SUMOTime durationArg, const std::string& stateArg, SUMOTime minDurationArg, SUMOTime maxDurationArg,
              const std::vector<int> nextPhasesArg, const std::string& nameArg) {
        this->duration = durationArg;
        this->myState = stateArg;
        this->minDuration = minDurationArg < 0 ? durationArg : minDurationArg;
        this->maxDuration = (maxDurationArg < 0 || maxDurationArg < minDurationArg) ? durationArg : maxDurationArg;
        // assert(this->minDuration <= this->maxDuration); // not ensured by the previous lines
        this->myLastSwitch = string2time(OptionsCont::getOptions().getString("begin")); // SUMOTime-option
        this->nextPhases = nextPhasesArg;
        this->name = nameArg;
    }

    void init(SUMOTime durationArg, const std::string& stateArg, SUMOTime minDurationArg, SUMOTime maxDurationArg,
              SUMOTime vehextArg, SUMOTime yellowArg, SUMOTime redArg, const std::vector<int> nextPhasesArg, const std::string& nameArg) {
        init(durationArg, stateArg, minDurationArg, maxDurationArg, nextPhasesArg, nameArg);
        this->vehext = vehextArg;
        this->yellow = yellowArg;
        this->red = redArg;
    }

    void init(SUMOTime durationArg, SUMOTime minDurationArg, SUMOTime maxDurationArg, const std::string& stateArg,
              const std::vector<int>& nextPhasesArg, const std::string& nameArg, std::vector<std::string>* targetLaneSetArg) {
        init(durationArg, stateArg, minDurationArg, maxDurationArg, nextPhasesArg, nameArg);
        //For SOTL target phases
        if (targetLaneSetArg != nullptr) {
            this->myTargetLaneSet = *targetLaneSetArg;
        }
    }


public:
    /** @brief Constructor
     *
     * minDuration and maxDuration are set to duration.
     *
     * @param[in] durationArg The duration of the phase
     * @param[in] stateArg The state in the phase
     */
    MSPhaseDefinition(SUMOTime durationArg, const std::string& stateArg, const std::vector<int>& nextPhases, const std::string& name = "") {
        init(durationArg, stateArg, durationArg, durationArg, nextPhases, name);
    }


    /** @brief Constructor
     * In this phase the duration is constrained between min and max duration
     * @param[in] durationArg The duration of the phase
     * @param[in] stateArg The state in the phase
     * @param[in] minDurationArg The minimum duration of the phase
     * @param[in] maxDurationArg The maximum duration of the phase
     */
    MSPhaseDefinition(SUMOTime durationArg, const std::string& stateArg, SUMOTime minDurationArg = -1, SUMOTime maxDurationArg = -1,
                      const std::vector<int>& nextPhases = std::vector<int>(), const std::string& name = "") {
        init(durationArg, stateArg, minDurationArg, maxDurationArg, nextPhases, name);
    }

    /** @brief Constructor
     * In this phase the duration is constrained between min and max duration
     * @param[in] durationArg The duration of the phase
     * @param[in] stateArg The state in the phase
     * @param[in] minDurationArg The minimum duration of the phase
     * @param[in] maxDurationArg The maximum duration of the phase
     */
    MSPhaseDefinition(SUMOTime durationArg, const std::string& stateArg, SUMOTime minDurationArg, SUMOTime maxDurationArg,
                      SUMOTime vehextTime, SUMOTime redTime, SUMOTime yellowTime,
                      const std::vector<int>& nextPhases = std::vector<int>(), const std::string& name = "") {
        init(durationArg, stateArg, minDurationArg, maxDurationArg, vehextTime, yellowTime, redTime, nextPhases, name);
    }

    /*
     * @brief Constructor for definitions for SOTL target step
     * In this phase the duration is constrained between min and max duration
     * @param[in] phaseType Indicates the type of the step
     * @param[in] targetLaneSet If not null, specifies this MSPhaseDefinition is a target step
     * @see MSPhaseDefinition::PhaseType
     */
    MSPhaseDefinition(SUMOTime durationArg, const std::string& stateArg, SUMOTime minDurationArg, SUMOTime maxDurationArg,
                      const std::vector<int>& nextPhases, const std::string& name, bool transient_notdecisional, bool commit,
                      std::vector<std::string>* targetLaneSetArg = nullptr) :
        myTransientNotDecisional(transient_notdecisional), myCommit(commit), myUndefined(false) {
        if (targetLaneSetArg != nullptr && targetLaneSetArg->size() == 0) {
            MsgHandler::getErrorInstance()->inform("MSPhaseDefinition::MSPhaseDefinition -> targetLaneSetArg cannot be empty for a target phase");
        }
        init(durationArg, minDurationArg, maxDurationArg, stateArg, nextPhases, name, targetLaneSetArg);
    }

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
