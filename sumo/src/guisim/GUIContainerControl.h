/****************************************************************************/
/// @file    GUIContainerControl.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Wed, 01.08.2014
/// @version $Id$
///
// GUI-version of the container control for building gui containers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIContainerControl_h
#define GUIContainerControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <microsim/MSContainerControl.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class GUIContainerControl
 * @brief GUI-version of the container control for building gui containers
 */
class GUIContainerControl : public MSContainerControl {
public:
    /// constructor
    GUIContainerControl();


    /// destructor
    virtual ~GUIContainerControl();


    /** @brief Builds a new container
     * @param[in] pars The parameter
     * @param[in] vtype The type (reusing vehicle type container here)
     * @param[in] plan This container's plan
     */
    virtual MSContainer* buildContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSContainer::MSContainerPlan* plan) const;

};


#endif

/****************************************************************************/
