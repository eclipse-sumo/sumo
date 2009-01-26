/****************************************************************************/
/// @file    MSUpdateEachTimestep.h
/// @author  Christian Roessel
/// @date    Thu Oct 23 16:22:53 2003
/// @version $Id$
///
// Base class for objects that shall be updated in each simulation step
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSUpdateEachTimestep_h
#define MSUpdateEachTimestep_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSUpdateEachTimestepContainer.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSUpdateEachTimestep
 * @brief Base class for objects that shall be updated in each simulation step
 */
template < class ToUpdate >
class MSUpdateEachTimestep
{
public:
    /// @brief Counstructor
    MSUpdateEachTimestep() throw() {
        MSUpdateEachTimestepContainer< MSUpdateEachTimestep<
        ToUpdate > >::getInstance()->addItemToUpdate(this);
    }


    /// @brief Destructor
    virtual ~MSUpdateEachTimestep() throw() {
        MSUpdateEachTimestepContainer< MSUpdateEachTimestep<
        ToUpdate > >::getInstance()->removeItemToUpdate(this);
    }


    /** @brief The update method
     * 
     * Derived objects will be called in each time step using this method
     * @return discarded (unused)
     */
    virtual bool updateEachTimestep() throw() = 0;

};


#endif

/****************************************************************************/

