/****************************************************************************/
/// @file    MSJunctionLogic.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// kinds of logic-implementations.
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
#ifndef MSJunctionLogic_h
#define MSJunctionLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLogicJunction.h"
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunctionLogic
 */
class MSJunctionLogic
{
public:
    /// Destructor.
    virtual ~MSJunctionLogic();

    /// Modifies the passed respond according to the request.
    virtual void respond(const MSLogicJunction::Request& request,
                         const MSLogicJunction::InnerState& innerState,
                         MSLogicJunction::Respond& respond) const = 0;

    /// Returns the logic's number of links.
    unsigned int nLinks();

    /// Returns the logic's number of inLanes.
    unsigned int nInLanes();

    /// Returns the foes of the given link
    virtual const MSLogicJunction::LinkFoes &getFoesFor(unsigned int linkIndex) const {
        return myDummyFoes;
    }

protected:
    /// Constructor.
    MSJunctionLogic(unsigned int nLinks, unsigned int nInLanes);

    /// The logic's number of links.
    unsigned int myNLinks;

    /// The logic's number of inLanes.
    unsigned int myNInLanes;

    /// @brief A dummy foe container
    static MSLogicJunction::LinkFoes myDummyFoes;

private:
    /// Default constructor.
    MSJunctionLogic();

    /// Copy constructor.
    MSJunctionLogic(const MSJunctionLogic&);

    /// Assignment operator.
    MSJunctionLogic& operator=(const MSJunctionLogic&);

};


#endif

/****************************************************************************/

