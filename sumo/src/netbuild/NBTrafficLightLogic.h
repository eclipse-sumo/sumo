#ifndef NBTrafficLightLogic_h
#define NBTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightLogic.h -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.5  2003/04/01 15:15:23  dkrajzew
// some documentation added
//
// Revision 1.4  2003/03/20 16:23:10  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:59:21  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <iostream>
#include "NBCont.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NBTrafficLightLogic
 * A completely build logic for a traffic light; Held until being saved
 * at the end of a networks's building
 */
class NBTrafficLightLogic {
public:
    /// Constructor
    NBTrafficLightLogic(const std::string &key, size_t noLinks,
        size_t tyellow);

    /// Copy constructor
    NBTrafficLightLogic(const NBTrafficLightLogic &s);

    /// Destructor
    ~NBTrafficLightLogic();

    /** @brief Adds a phase to the logic
        This is done during the building, the new phase is inserted at the end of
        the list of already added phases */
    void addStep(size_t duration, std::bitset<64> driveMask,
        std::bitset<64> brakeMask);

    /// Writes the traffic light logic into the given stream in it's XML-representation
    void writeXML(std::ostream &into, size_t no,
        const EdgeVector &inLanes) const;

    /// Debug method showing the phases
    void _debugWritePhases() const;

    /// Information whether the given logic is equal to this
    bool equals(const NBTrafficLightLogic &logic) const;

private:
    /// The key (id) of the logic
    std::string _key;

    /// The number of participating links
    size_t _noLinks;

    /// The duration of the yellow-phase
    size_t myTYellow;

    /**
     * @class PhaseDefinition
     * The definition of a single phase of the logic
     */
    class PhaseDefinition {
    public:
        /// The duration of the phase in s
        size_t              duration;

        /// The information which links may drive within this phase
        std::bitset<64>     driveMask;

        /// The information which links have to brake within this phase
        std::bitset<64>     brakeMask;

        /// Constructor
        PhaseDefinition(size_t durationArg, std::bitset<64> driveMaskArg,
            std::bitset<64> brakeMaskArg)
            : duration(durationArg), driveMask(driveMaskArg),
            brakeMask(brakeMaskArg) { }

        /// Destructor
        ~PhaseDefinition() { }

        /// Comparison operator
        bool operator!=(const PhaseDefinition &pd) const {
            return pd.duration != duration ||
                pd.driveMask != driveMask ||
                pd.brakeMask != brakeMask;
        }

    };

    /// Definition of a vector of traffic light phases
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;

    /// The junction logic's storage for traffic light phase list
    PhaseDefinitionVector _phases;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBTrafficLightLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

