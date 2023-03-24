/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    NBTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A SUMO-compliant built logic for a traffic light
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <set>
#include "NBConnectionDefs.h"
#include <utils/common/SUMOTime.h>
#include <utils/common/Named.h>
#include <utils/common/Parameterised.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTrafficLightLogic
 * @brief A SUMO-compliant built logic for a traffic light
 */
class NBTrafficLightLogic : public Named, public Parameterised {
public:
    /**
     * @class PhaseDefinition
     * @brief The definition of a single phase of the logic
     */
    class PhaseDefinition {

    public:
        /// @brief The duration of the phase in s
        SUMOTime duration;

        /// @brief The state definition
        std::string state;

        /// @brief minimum duration (for actuated)
        SUMOTime minDur;

        /// @brief maximum duration duration (for actuated)
        SUMOTime maxDur;

        /// @brief minimum duration (for actuated)
        SUMOTime earliestEnd;

        /// @brief maximum duration duration (for actuated)
        SUMOTime latestEnd;

        /// @brief veh ext (for NEMA)
        SUMOTime vehExt;

        /// @brief yellow (for NEMA)
        SUMOTime yellow;

        /// @brief red (for NEMA)
        SUMOTime red;

        /// @brief next phase indices or empty list
        std::vector<int> next;

        /// @brief option phase name
        std::string name;

        /** @brief Constructor
         * @param[in] durationArg The duration of the phase
         * @param[in] stateArg Signals per link
         */
        PhaseDefinition(const SUMOTime duration_, const std::string& state_, const SUMOTime minDur_, const SUMOTime maxDur_,
                        const SUMOTime earliestEnd_, const SUMOTime latestEnd_, const SUMOTime vehExt_, const SUMOTime yellow_,
                        const SUMOTime red_, const std::vector<int>& next_, const std::string& name_) :
            duration(duration_),
            state(state_),
            minDur(minDur_),
            maxDur(maxDur_),
            earliestEnd(earliestEnd_),
            latestEnd(latestEnd_),
            vehExt(vehExt_),
            yellow(yellow_),
            red(red_),
            next(next_),
            name(name_)
        { }

        /// @brief Destructor
        ~PhaseDefinition() { }

        /** @brief Comparison operator
         * @param[in] pd A second phase
         * @return Whether this and the given phases are same
         */
        bool operator!=(const PhaseDefinition& pd) const {
            return ((pd.duration != duration) ||
                    (pd.minDur != minDur) ||
                    (pd.maxDur != maxDur) ||
                    (pd.earliestEnd != earliestEnd) ||
                    (pd.latestEnd != latestEnd) ||
                    (pd.state != state) ||
                    (pd.next != next) ||
                    (pd.name != name));
        }

    };


    /** @brief Constructor
     * @param[in] id The id of the traffic light
     * @param[in] subid The id of the program
     * @param[in] noLinks Number of links that are controlled by this tls. 0 means the value is not known beforehand
     * @param[in] offset The offset of the program (delay)
     * @param[in] type The algorithm type for the computed traffic light
     */
    NBTrafficLightLogic(const std::string& id, const std::string& subid, int noLinks,
                        SUMOTime offset = 0, TrafficLightType type = TrafficLightType::STATIC);


    /** @brief Copy Constructor
     * @param[in] logic The logic to copy
     */
    NBTrafficLightLogic(const NBTrafficLightLogic* logic);


    /// @brief Destructor
    ~NBTrafficLightLogic();


    /** @brief Adds a phase to the logic (static)
     *
     * @param[in] duration The duration of the phase to add
     * @param[in] state The state definition of a tls phase
     * @param[in] name The name of the phase
     * @param[in] next The index of the next phase
     * @param[in] index The index of the new phase (-1 means append to end)
     * @note: the length of the state has to match the number of links
     *        and the length given in previous calls to addStep (throws ProcessError)
     */
    void addStep(const SUMOTime duration, const std::string& state, const std::vector<int>& next = std::vector<int>(),
                 const std::string& name = "", const int index = -1);

    /** @brief Adds a phase to the logic (actuated)
     *
     * @param[in] duration The duration of the phase to add
     * @param[in] state The state definition of a tls phase
     * @param[in] name The name of the phase
     * @param[in] minDur The minimum duration of the phase to add
     * @param[in] maxDur The maximum duration of the phase to add
     * @param[in] earliestEnd The earliest end of the phase to add
     * @param[in] latestEnd The latest end of the phase to add
     * @param[in] vehExt The vehExt of the phase to add
     * @param[in] yellow The yellow of the phase to add
     * @param[in] red The red of the phase to add
     * @param[in] next The index of the next phase
     * @param[in] index The index of the new phase (-1 means append to end)
     * @note: the length of the state has to match the number of links
     *        and the length given in previous calls to addStep (throws ProcessError)
     */
    void addStep(const SUMOTime duration, const std::string& state, const SUMOTime minDur, const SUMOTime maxDur, const SUMOTime earliestEnd,
                 const SUMOTime latestEnd,
                 const SUMOTime vehExt = -1, // UNSPECIFIED_DURATION
                 const SUMOTime yellow = -1, // UNSPECIFIED_DURATION
                 const SUMOTime red = -1, // UNSPECIFIED_DURATION
                 const std::string& name = "",
                 const std::vector<int>& next = std::vector<int>(),
                 int index = -1);

    /** @brief Modifies the state for an existing phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] tlIndex The index at which to modify the state
     * @param[in] linkState The new link state for the given index
     */
    void setPhaseState(int phaseIndex, int tlIndex, LinkState linkState);

    /** @brief Modifies the duration for an existing phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new duration for this phase
     */
    void setPhaseDuration(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the min duration for an existing phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new min duration for this phase
     */
    void setPhaseMinDuration(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the max duration for an existing phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new max duration for this phase
     */
    void setPhaseMaxDuration(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the min duration for an existing phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new earliestEnd for this phase
     */
    void setPhaseEarliestEnd(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the max duration for an existing phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new latestEnd for this phase
     */
    void setPhaseLatestEnd(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the veh ex for an existing phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new vehEx for this phase
     */
    void setPhaseVehExt(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the veh ex for an existing phase (used by netedit)
    * @param[in] phaseIndex The index of the phase to modify
    * @param[in] duration The new vehEx for this phase
    */
    void setPhaseYellow(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the veh ex for an existing phase (used by netedit)
    * @param[in] phaseIndex The index of the phase to modify
    * @param[in] duration The new vehEx for this phase
    */
    void setPhaseRed(int phaseIndex, SUMOTime duration);

    /** @brief Modifies the next phase (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new duration for this phase
     */
    void setPhaseNext(int phaseIndex, const std::vector<int>& next);

    /** @brief Modifies the phase name (used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new duration for this phase
     */
    void setPhaseName(int phaseIndex, const std::string& name);

    /** @brief override state with the given character(used by netedit)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] c the character to override (r, y, g, G...)
     */
    void overrideState(int phaseIndex, const char c);

    /* @brief deletes the phase at the given index
     * @note throws InvalidArgument on out-of range index
     */
    void deletePhase(int index);

    /* @brief swap phases
     * @note throws InvalidArgument on out-of range index
     */
    void swapPhase(int indexPhaseA, int indexPhaseB);

    /// @brief swap first phase
    void swapfirstPhase();

    /// @brief swap first phase
    void swaplastPhase();

    /* @brief changes state size either by cutting of at the end or by adding
     * new states at the end
     */
    void setStateLength(int numLinks, LinkState fill = LINKSTATE_TL_RED);

    /// @brief remove the index from all phase states
    void deleteStateIndex(int index);

    /// @brief deletes all phases and reset the expect number of links
    void resetPhases();

    /** @brief closes the building process
     *
     * Joins equal steps.
     */
    void closeBuilding(bool checkVarDurations = true);

    /// @brief Returns the duration of the complete cycle
    SUMOTime getDuration() const;

    /** @brief Sets the offset of this tls
     * @param[in] offset The offset of this cycle
     */
    void setOffset(SUMOTime offset) {
        myOffset = offset;
    }

    /// @brief Returns the ProgramID
    const std::string& getProgramID() const {
        return mySubID;
    };

    /// @brief Returns the phases
    const std::vector<PhaseDefinition>& getPhases() const {
        return myPhases;
    }

    /// @brief copy phase values in other
    void copyPhase(const int origin, const int destiny) {
        myPhases.at(destiny) = myPhases.at(origin);
    }

    /// @brief Returns the offset of first switch
    SUMOTime getOffset() const {
        return myOffset;
    };

    /// @brief Returns the number of participating links
    int getNumLinks() {
        return myNumLinks;
    }

    /// @brief get the algorithm type (static etc..)
    TrafficLightType getType() const {
        return myType;
    }

    /// @brief set the algorithm type (static etc..)
    void setType(TrafficLightType type) {
        myType = type;
    }

    /** @brief Sets the programID
     * @param[in] programID The new ID of the program (subID)
     */
    void setProgramID(const std::string& programID) {
        mySubID = programID;
    }

private:
    /// @brief The number of participating links
    int myNumLinks;

    /// @brief The tls program's subid
    std::string mySubID;

    /// @brief The tls program's offset
    SUMOTime myOffset;

    /// @brief Definition of a vector of traffic light phases
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;

    /// @brief The junction logic's storage for traffic light phase list
    PhaseDefinitionVector myPhases;

    /// @brief The algorithm type for the traffic light
    TrafficLightType myType;

    /// @brief Invalidated assignment operator
    NBTrafficLightLogic& operator=(const NBTrafficLightLogic& s) = delete;
};
