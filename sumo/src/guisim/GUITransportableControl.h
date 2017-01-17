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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUITransportableControl_h
#define GUITransportableControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
     */
    virtual MSTransportable* buildPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const;

    /** @brief Builds a new container
    * @param[in] pars The parameter
    * @param[in] vtype The type (reusing vehicle type container here)
    * @param[in] plan This container's plan
    */
    virtual MSTransportable* buildContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const;

    /** @brief Returns the list of all known persons by gl-id
     * @param[fill] into The list to fill with vehicle ids
     * @todo Well, what about concurrent modifications?
     */
    void insertPersonIDs(std::vector<GUIGlID>& into);
};


#endif

/****************************************************************************/
