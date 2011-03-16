/****************************************************************************/
/// @file    NBTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SUMO-compliant built logic for a traffic light
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
class NBTrafficLightLogic : public Named {
public:
    /** @brief Constructor
     * @param[in] id The id of the traffic light
     * @param[in] subid The id of the program
     * @param[in] noLinks Number of links that are controlled by this tls
     */
    NBTrafficLightLogic(const std::string &id, const std::string &subid, unsigned int noLinks) throw();


    /// @brief Destructor
    ~NBTrafficLightLogic() throw();


    /** @brief Adds a phase to the logic
     *
     * This is done during the building; the new phase is inserted at the end of
     *  the list of already added phases
     * @param[in] duration The duration of the phase to add
     * @param[in] state The state definition of a tls phase
     */
    void addStep(SUMOTime duration, const std::string &state) throw();


    /** @brief Writes the traffic light logic into the given stream in it's XML-representation
     * @param[in] into The stream to write the definition into
     */
    void writeXML(OutputDevice &into) const throw();


    /** @brief closes the building process
     *
     * Joins equal steps.
     */
    void closeBuilding() throw();


    /** @brief Returns the duration of the complete cycle
     * @return The duration of this logic's cycle
     */
    SUMOTime getDuration() const throw();


    /** @brief Sets the offset of this tls
     * @param[in] offset The offset of this cycle
     */
    void setOffset(SUMOTime offset) throw() {
        myOffset = offset;
    }


    /** @brief Returns the ProgramID */
    const std::string & getProgramID() const {
        return mySubID;
    };


private:
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
         * @param[in] stateArg Information which links may drive during the phase
         * @param[in] brakeMaskArg Information which links have to decelerate during the phase
         * @param[in] yellowMaskArg Information which links have yellow during the phase
         */
        PhaseDefinition(SUMOTime durationArg, const std::string &stateArg) throw()
                : duration(durationArg), state(stateArg) { }

        /// @brief Destructor
        ~PhaseDefinition() throw() { }

        /** @brief Comparison operator
         * @param[in] pd A second phase
         * @return Whether this and the given phases are same
         */
        bool operator!=(const PhaseDefinition &pd) const throw() {
            return pd.duration != duration || pd.state != state;
        }

    };


private:
    /// @brief The number of participating links
    unsigned int myNoLinks;

    /// @brief The tls program's subid
    const std::string mySubID;

    /// @brief The tls program's offset
    SUMOTime myOffset;

    /// @brief Definition of a vector of traffic light phases
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;

    /// @brief The junction logic's storage for traffic light phase list
    PhaseDefinitionVector myPhases;


};


#endif

/****************************************************************************/

