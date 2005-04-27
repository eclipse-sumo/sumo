#ifndef NBTrafficLightLogic_h
#define NBTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightLogic.h -
//  A single traffic light logic (a possible variant)
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
// Revision 1.13  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.12  2004/04/23 12:41:02  dkrajzew
// some further work on vissim-import
//
// Revision 1.11  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.10  2003/11/17 07:26:02  dkrajzew
// computations needed for collecting e2-values over multiple lanes added
//
// Revision 1.9  2003/09/25 09:02:51  dkrajzew
// multiple lane in tl-logic - bug patched
//
// Revision 1.8  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.7  2003/05/21 15:18:19  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.6  2003/04/07 12:15:45  dkrajzew
// first steps towards a junctions geometry; tyellow removed again, traffic lights have yellow times given explicitely, now
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
#include <set>
#include <iostream>
#include "NBConnectionDefs.h"


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
    void writeXML(std::ostream &into, size_t no, double distance,
        std::string type, const std::set<std::string> &inLanes) const;
    // !!! the key should be given here, too, instead of storing it

    /// Debug method showing the phases
    void _debugWritePhases() const;

    /// Information whether the given logic is equal to this
    bool equals(const NBTrafficLightLogic &logic) const;

    /// closes the building process (joins equal steps)
    void closeBuilding();

private:
    size_t getOffset() const;

    bool checkOffsetFor(const std::string &optionName) const;

    size_t computeOffsetFor(double offsetMult) const;


private:
    /// The key (id) of the logic
    std::string _key;

    /// The number of participating links
    size_t _noLinks;

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
    PhaseDefinitionVector _phases;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#endif

#endif

// Local Variables:
// mode:C++
// End:

