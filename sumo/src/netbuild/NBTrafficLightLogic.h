/****************************************************************************/
/// @file    NBTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SUMO-compliant built logic for a traffic light
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
class NBTrafficLightLogic : public Named
{
public:
    /** @brief Constructor
     *
     */
    NBTrafficLightLogic(const std::string &id, const std::string &subid,
                        const std::string &type, unsigned int noLinks) throw();


    /// @brief Destructor
    ~NBTrafficLightLogic() throw();


    /** @brief Adds a phase to the logic
     *
     * This is done during the building; the new phase is inserted at the end of
     * the list of already added phases
     *
     * @param[in] duration The duration of the phase to add
     * @param[in] driveMask Information which links may drive during the phase to add
     * @param[in] brakeMask Information which links have to decelerate during the phase to add
     * @param[in] yellowMask Information which links have yellow during the phase to add
     */
    void addStep(SUMOTime duration, std::bitset<64> driveMask,
                 std::bitset<64> brakeMask, std::bitset<64> yellowMask) throw();


    /** @brief Writes the traffic light logic into the given stream in it's XML-representation
     *
     * @param[in] into The stream to write the definition into
     * @param[in] no Index (subid) of the program
     * @param[in] distance !!!unused
     * @param[in] type The type of the tls
     * @param[in] inLanes !!!unused
     */
    void writeXML(OutputDevice &into, size_t no, SUMOReal distance,
                  std::string type, const std::set<std::string> &inLanes) const throw();
    // !!! the key should be given here, too, instead of storing it

    /// Information whether the given logic is equal to this
    bool equals(const NBTrafficLightLogic &logic) const throw();

    /// closes the building process (joins equal steps)
    void closeBuilding() throw();

    SUMOTime getDuration() const throw();
    void setOffset(SUMOTime offset) throw() {
        myOffset = offset;
    }


private:
    SUMOTime computeOffsetFor(SUMOReal offsetMult) const throw();


private:
    /**
     * @class PhaseDefinition
     * @brief The definition of a single phase of the logic
     */
    class PhaseDefinition
    {
    public:
        /// @brief The duration of the phase in s
        SUMOTime duration;

        /// @brief The information which links may drive within this phase
        std::bitset<64> driveMask;

        /// @brief The information which links have to brake within this phase
        std::bitset<64> brakeMask;

        /// @brief The information which links have yellow within this phase
        std::bitset<64> yellowMask;

        /** @brief Constructor
         * @param[in] durationArg The duration of the phase
         * @param[in] driveMaskArg Information which links may drive during the phase
         * @param[in] brakeMaskArg Information which links have to decelerate during the phase
         * @param[in] yellowMaskArg Information which links have yellow during the phase
         */
        PhaseDefinition(SUMOTime durationArg, std::bitset<64> driveMaskArg,
                        std::bitset<64> brakeMaskArg, std::bitset<64> yellowMaskArg) throw()
                : duration(durationArg), driveMask(driveMaskArg),
                brakeMask(brakeMaskArg), yellowMask(yellowMaskArg) { }

        /// @brief Destructor
        ~PhaseDefinition() throw() { }

        /** @brief Comparison operator
         * @param[in] pd A second phase
         * @return Whether this and the given phases are same
         */
        bool operator!=(const PhaseDefinition &pd) const throw() {
            return pd.duration != duration ||
                   pd.driveMask != driveMask ||
                   pd.brakeMask != brakeMask ||
                   pd.yellowMask != yellowMask;
        }

    };


private:
    /// @brief The number of participating links
    unsigned int myNoLinks;

    /// @brief The tls program's subid
    std::string mySubID;

    /// @brief The tls program's type
    std::string myType;

    /// @brief The tls program's offset
    SUMOTime myOffset;

    /// @brief Definition of a vector of traffic light phases
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;

    /// @brief The junction logic's storage for traffic light phase list
    PhaseDefinitionVector myPhases;


};


#endif

/****************************************************************************/

