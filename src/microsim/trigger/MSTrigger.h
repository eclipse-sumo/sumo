/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTrigger.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// An abstract device that changes the state of the micro simulation
/****************************************************************************/
#ifndef MSTrigger_h
#define MSTrigger_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <set>
#include <utils/common/Named.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTrigger
 * @brief An abstract device that changes the state of the micro simulation
 *
 * We name most of the additional microsim-structures "trigger" in order to
 *  allow some common operation on them.
 */
class MSTrigger : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the trigger
     */
    MSTrigger(const std::string& id);

    /// @brief Destructor
    virtual ~MSTrigger();

    /// @brief properly deletes all trigger instances
    static void cleanup();

private:
    /// @brief Invalidated copy constructor.
    MSTrigger(const MSTrigger&);

    /// @brief Invalidated assignment operator.
    MSTrigger& operator=(const MSTrigger&);

    static std::set<MSTrigger*> myInstances;

};


#endif

/****************************************************************************/

