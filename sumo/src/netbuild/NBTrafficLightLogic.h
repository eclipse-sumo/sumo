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
 * NBTrafficLightLogic
 */
class NBTrafficLightLogic {
private:
    std::string _key;
    size_t _noLinks;
    class PhaseDefinition {
    public:
        size_t              duration;
        std::bitset<64>     driveMask;
        std::bitset<64>     brakeMask;
        PhaseDefinition(size_t durationArg, std::bitset<64> driveMaskArg,
            std::bitset<64> brakeMaskArg)
            : duration(durationArg), driveMask(driveMaskArg),
            brakeMask(brakeMaskArg) { }
        ~PhaseDefinition() { }
        bool operator!=(const PhaseDefinition &pd) const {
            return pd.duration != duration ||
                pd.driveMask != driveMask ||
                pd.brakeMask != brakeMask;
        }
    };
    typedef std::vector<PhaseDefinition> PhaseDefinitionVector;
    PhaseDefinitionVector _phases;
public:
    NBTrafficLightLogic(const std::string &key, size_t noLinks);
    NBTrafficLightLogic(const NBTrafficLightLogic &s);
    ~NBTrafficLightLogic();
    void addStep(size_t duration, std::bitset<64> driveMask,
        std::bitset<64> brakeMask);
    void writeXML(std::ostream &into, size_t no,
        const EdgeVector &inLanes) const;
    void _debugWritePhases() const;
    bool equals(const NBTrafficLightLogic &logic) const;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBTrafficLightLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

