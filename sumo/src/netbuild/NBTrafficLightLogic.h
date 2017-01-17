/****************************************************************************/
/// @file    NBTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A SUMO-compliant built logic for a traffic light
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBTrafficLightLogic_h
#define NBTrafficLightLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

        /** @brief Constructor
         * @param[in] durationArg The duration of the phase
         * @param[in] stateArg Signals per link
         */
        PhaseDefinition(SUMOTime durationArg, const std::string& stateArg)
            : duration(durationArg), state(stateArg) { }

        /// @brief Destructor
        ~PhaseDefinition() { }

        /** @brief Comparison operator
         * @param[in] pd A second phase
         * @return Whether this and the given phases are same
         */
        bool operator!=(const PhaseDefinition& pd) const {
            return pd.duration != duration || pd.state != state;
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
                        SUMOTime offset = 0, TrafficLightType type = TLTYPE_STATIC);


    /** @brief Copy Constructor
     * @param[in] logic The logic to copy
     */
    NBTrafficLightLogic(const NBTrafficLightLogic* logic);


    /// @brief Destructor
    ~NBTrafficLightLogic();


    /** @brief Adds a phase to the logic
     *
     * @param[in] duration The duration of the phase to add
     * @param[in] state The state definition of a tls phase
     * @param[in] index The index of the new phase (-1 means append to end)
     * @note: the length of the state has to match the number of links
     *        and the length given in previous calls to addStep (throws ProcessError)
     */
    void addStep(SUMOTime duration, const std::string& state, int index = -1);


    /** @brief Modifies the state for an existing phase (used by NETEDIT)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] tlIndex The index at which to modify the state
     * @param[in] linkState The new link state for the given index
     */
    void setPhaseState(int phaseIndex, int tlIndex, LinkState linkState);

    /** @brief Modifies the duration for an existing phase (used by NETEDIT)
     * @param[in] phaseIndex The index of the phase to modify
     * @param[in] duration The new duration for this phase
     */
    void setPhaseDuration(int phaseIndex, SUMOTime duration);

    /* @brief deletes the phase at the given index
     * @note thhrows InvalidArgument on out-of range index
    */
    void deletePhase(int index);

    /* @brief deletes all phases and reset the expect number of links
    */
    void resetPhases();

    /** @brief closes the building process
     *
     * Joins equal steps.
     */
    void closeBuilding();


    /** @brief Returns the duration of the complete cycle
     * @return The duration of this logic's cycle
     */
    SUMOTime getDuration() const;


    /** @brief Sets the offset of this tls
     * @param[in] offset The offset of this cycle
     */
    void setOffset(SUMOTime offset) {
        myOffset = offset;
    }


    /** @brief Returns the ProgramID
     * @return The ID of the program (subID)
     */
    const std::string& getProgramID() const {
        return mySubID;
    };


    /** @brief Returns the phases
     * @return The phase list
     */
    const std::vector<PhaseDefinition>& getPhases() const {
        return myPhases;
    }


    /** @brief Returns the offset of first switch
     * @return The switch offset
     */
    SUMOTime getOffset() const {
        return myOffset;
    };


    /** @brief Returns the number of participating links
     */
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

private:
    /// @brief The number of participating links
    int myNumLinks;

    /// @brief The tls program's subid
    const std::string mySubID;

    /// @brief The tls program's offset
    SUMOTime myOffset;

    /// @brief Definition of a vector of traffic light phases
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;

    /// @brief The junction logic's storage for traffic light phase list
    PhaseDefinitionVector myPhases;

    /// @brief The algorithm type for the traffic light
    TrafficLightType myType;

    /// @brief all allowed characters for phase state (see SUMOXMLDefinitions)
    static const char allowedStatesInitializer[];
    static const std::string ALLOWED_STATES;

private:
    /// @brief Invalidated assignment operator
    NBTrafficLightLogic& operator=(const NBTrafficLightLogic& s);

};


#endif

/****************************************************************************/

