/****************************************************************************/
/// @file    NBTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single traffic light logic (a possible variant)
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


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTrafficLightLogic
 * A completely build logic for a traffic light; Held until being saved
 * at the end of a networks's building
 */
class NBTrafficLightLogic
{
public:
    /// Constructor
    NBTrafficLightLogic(const std::string &key, size_t noLinks);

    /// Copy constructor
    NBTrafficLightLogic(const NBTrafficLightLogic &s);

    /// Destructor
    ~NBTrafficLightLogic();

    /** @brief Adds a phase to the logic
        This is done during the building, the new phase is inserted at the end of
        the list of already added phases */
    void addStep(size_t duration, std::bitset<64> driveMask,
                 std::bitset<64> brakeMask, std::bitset<64> yellowMask);

    /// Writes the traffic light logic into the given stream in it's XML-representation
    void writeXML(OutputDevice &into, size_t no, SUMOReal distance,
                  std::string type, const std::set<std::string> &inLanes) const;
    // !!! the key should be given here, too, instead of storing it

    /// Information whether the given logic is equal to this
    bool equals(const NBTrafficLightLogic &logic) const;

    /// closes the building process (joins equal steps)
    void closeBuilding();

private:
    size_t getOffset() const;

    bool checkOffsetFor(const std::string &optionName) const;

    size_t computeOffsetFor(SUMOReal offsetMult) const;


private:
    /// The key (id) of the logic
    std::string myKey;

    /// The number of participating links
    size_t myNoLinks;

    /**
     * @class PhaseDefinition
     * The definition of a single phase of the logic
     */
    class PhaseDefinition
    {
    public:
        /// The duration of the phase in s
        size_t              duration;

        /// The information which links may drive within this phase
        std::bitset<64>     driveMask;

        /// The information which links have to brake within this phase
        std::bitset<64>     brakeMask;

        std::bitset<64>     yellowMask;

        /// Constructor
        PhaseDefinition(size_t durationArg, std::bitset<64> driveMaskArg,
                        std::bitset<64> brakeMaskArg, std::bitset<64> yellowMaskArg)
                : duration(durationArg), driveMask(driveMaskArg),
                brakeMask(brakeMaskArg), yellowMask(yellowMaskArg) { }

        /// Destructor
        ~PhaseDefinition() { }

        /// Comparison operator
        bool operator!=(const PhaseDefinition &pd) const {
            return pd.duration != duration ||
                   pd.driveMask != driveMask ||
                   pd.brakeMask != brakeMask ||
                   pd.yellowMask != yellowMask;
        }

    };

    /// Definition of a vector of traffic light phases
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;

    /// The junction logic's storage for traffic light phase list
    PhaseDefinitionVector myPhases;

};


#endif

/****************************************************************************/

