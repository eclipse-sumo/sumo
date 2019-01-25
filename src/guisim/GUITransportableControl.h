/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUITransportableControl.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 13.06.2012
/// @version $Id$
///
// GUI-version of the transportable control for building gui persons and containers
/****************************************************************************/
#ifndef GUITransportableControl_h
#define GUITransportableControl_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <microsim/MSTransportableControl.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class GUITransportableControl
 * @brief GUI-version of the transportable control for building gui persons and containers
 */
class GUITransportableControl : public MSTransportableControl {
public:
    /// constructor
    GUITransportableControl();


    /// destructor
    virtual ~GUITransportableControl();


    /** @brief Builds a new person
     * @param[in] pars The parameter
     * @param[in] vtype The type (reusing vehicle type container here)
     * @param[in] plan This person's plan
     * @param[in] rng The RNG to compute the optional speed deviation
     */
    virtual MSTransportable* buildPerson(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan,
                                         std::mt19937* rng) const;

    /** @brief Builds a new container
    * @param[in] pars The parameter
    * @param[in] vtype The type (reusing vehicle type container here)
    * @param[in] plan This container's plan
    */
    virtual MSTransportable* buildContainer(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const;

    /** @brief Returns the list of all known persons by gl-id
     * @param[fill] into The list to fill with vehicle ids
     * @todo Well, what about concurrent modifications?
     */
    void insertPersonIDs(std::vector<GUIGlID>& into);
};


#endif

/****************************************************************************/
