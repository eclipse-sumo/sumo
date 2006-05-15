#ifndef MSPhaseDefinition_h
#define MSPhaseDefinition_h
//---------------------------------------------------------------------------//
//                        MSPhaseDefinition.h -
//  The definition of a single phase of a tls logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.6  2006/05/15 06:01:51  dkrajzew
// added the possibility to stretch/change the current phase and consecutive phases
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/common/SUMOTime.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * The definition of a single phase */
class MSPhaseDefinition {
public:
    /// the duration of the phase
    SUMOTime          duration;

	/// stores the timestep of the last on-switched of the phase
    SUMOTime _lastSwitch;

    /// constructor
    MSPhaseDefinition(size_t durationArg, const std::bitset<64> &driveMaskArg,
            const std::bitset<64> &breakMaskArg,
            const std::bitset<64> &yellowMaskArg)
        : duration(durationArg), driveMask(driveMaskArg),
            breakMask(breakMaskArg), yellowMask(yellowMaskArg), _lastSwitch(0)
    {
		_lastSwitch = OptionsSubSys::getOptions().getInt("b");
    }

    /// destructor
    virtual ~MSPhaseDefinition() { }

    const std::bitset<64> &getDriveMask() const {
        return driveMask;
    }

    const std::bitset<64> &getBreakMask() const {
        return breakMask;
    }

    const std::bitset<64> &getYellowMask() const {
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

