/****************************************************************************/
/// @file    MSPhaseDefinition.h
/// @author  Daniel Krajzewicz
/// @date    Jan 2004
/// @version $Id$
///
// The definition of a single phase of a tls logic
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
#ifndef MSPhaseDefinition_h
#define MSPhaseDefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <utils/options/OptionsCont.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPhaseDefinition
 *
 * The definition of a single phase 
 */
class MSPhaseDefinition
{
public:
    /// the duration of the phase
    SUMOTime duration;

    /// stores the timestep of the last on-switched of the phase
    SUMOTime myLastSwitch;

    /// constructor
    MSPhaseDefinition(size_t durationArg, const std::bitset<64> &driveMaskArg,
                      const std::bitset<64> &breakMaskArg,
                      const std::bitset<64> &yellowMaskArg)
            : duration(durationArg), myLastSwitch(0), driveMask(driveMaskArg),
            breakMask(breakMaskArg), yellowMask(yellowMaskArg)
    {
        myLastSwitch = OptionsCont::getOptions().getInt("begin");
    }

    /// destructor
    virtual ~MSPhaseDefinition()
    { }

    const std::bitset<64> &getDriveMask() const
    {
        return driveMask;
    }

    const std::bitset<64> &getBreakMask() const
    {
        return breakMask;
    }

    const std::bitset<64> &getYellowMask() const
    {
        return yellowMask;
    }

private:
    /// invalidated standard constructor
    MSPhaseDefinition();

    /// the mask which links are allowed to drive within this phase (green light)
    std::bitset<64>  driveMask;

    /// the mask which links must not drive within this phase (red light)
    std::bitset<64>  breakMask;

    /// the mask which links have to decelerate(yellow light)
    std::bitset<64>  yellowMask;


};


#endif

/****************************************************************************/

