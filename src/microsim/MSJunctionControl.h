/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#ifndef MSJunctionControl_h
#define MSJunctionControl_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <map>
#include <string>

#include <utils/common/NamedObjectCont.h>
#include <utils/common/UtilExceptions.h>
#include "MSJunction.h"


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

