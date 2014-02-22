/****************************************************************************/
/// @file    MSJunctionControl.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Container for junctions; performs operations on all stored junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSJunctionControl_h
#define MSJunctionControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>

#include <utils/common/NamedObjectCont.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSJunctionControl
 * @brief Container for junctions; performs operations on all stored junctions
 */
class MSJunctionControl : public NamedObjectCont<MSJunction*> {
public:
    /// @brief Constructor
    MSJunctionControl();


    /// @brief Destructor
    ~MSJunctionControl();


    /** @brief Closes building of junctions
     *
     * Calls "postloadInit" on all stored junctions.
     * @exception ProcessError From the called "postloadInit"
     * @see MSJunction::postloadInit
     */
    void postloadInitContainer();


private:
    /// @brief Invalidated copy constructor.
    MSJunctionControl(const MSJunctionControl&);

    /// @brief Invalidated assignment operator.
    MSJunctionControl& operator=(const MSJunctionControl&);

};


#endif

/****************************************************************************/

