#ifndef MSSimpleTrafficLightLogic_h
#define MSSimpleTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSSimpleTrafficLightLogic.h -
//  The basic traffic light logic
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
// Revision 1.8  2003/06/06 10:39:16  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.7  2003/06/05 16:07:36  dkrajzew
// new usage of traffic lights implemented
//
// Revision 1.6  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.5  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.4  2003/04/04 07:13:20  dkrajzew
// Yellow phases must be now explicetely given
//
// Revision 1.3  2003/03/03 14:56:25  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utility>
#include <vector>
#include <bitset>
#include "MSEventControl.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSSimpleTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
template< size_t N >
class MSSimpleTrafficLightLogic : public MSTrafficLightLogic
{
public:
    /**
     * The definition of a single phase */
    class PhaseDefinition {
    public:
        /// the duration of the phase
        size_t          duration;

        /// the mask which links are allowed to drive within this phase (green light)
        std::bitset<N>  driveMask;

        /// the mask which links must not drive within this phase (red light)
        std::bitset<N>  breakMask;

        /// the mask which links have to decelerate(yellow light)
        std::bitset<N>  yellowMask;

        /// constructor
        PhaseDefinition(size_t durationArg,
            const std::bitset<N> &driveMaskArg,
            const std::bitset<N> &breakMaskArg,
			const std::bitset<N> &yellowMaskArg)
            : duration(durationArg), driveMask(driveMaskArg),
            breakMask(breakMaskArg), yellowMask(yellowMask) { }

        /// destructor
        ~PhaseDefinition() { }

    private:
        /// invalidated standard constructor
        PhaseDefinition();

    };

    /// definition of a list of phases, being the junction logic
    typedef std::vector<PhaseDefinition> Phases;

public:
    /// constructor
    MSSimpleTrafficLightLogic(const std::string &id, const Phases &phases,
        size_t step, size_t delay);

    /// destructor
    ~MSSimpleTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual MSNet::Time nextPhase();

    /// Returns the priorities for all lanes for the current phase
    virtual const std::bitset<64> &linkPriorities() const;

    /// Returns a bitset where all links having yellow are set
    virtual const std::bitset<64> &yellowMask() const;

    virtual const std::bitset<64> &allowed() const;

    /** @brief Switches to the next step
        Returns the number of the next step what is needed as
        the number of following steps is not known */
    virtual size_t nextStep();

    /// Returns the duration of the given step
    virtual MSNet::Time duration() const;

	/// returns the current step
	size_t step() const { return _step; }

protected:
    /// the list of phases this logic uses
    Phases _phases;

    /// static container for all lights being set to red
    static std::bitset<64> _allClear;

    /// The current step
    size_t _step;

};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "MSSimpleTrafficLightLogic.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSSimpleTrafficLightLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

