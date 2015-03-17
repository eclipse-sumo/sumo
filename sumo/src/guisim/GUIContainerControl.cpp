/****************************************************************************/
/// @file    GUIContainerControl.cpp
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <algorithm>
#include "GUINet.h"
#include "GUIContainerControl.h"
#include "GUIContainer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIContainerControl::GUIContainerControl() {}


GUIContainerControl::~GUIContainerControl() {
}


MSContainer*
GUIContainerControl::buildContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSContainer::MSContainerPlan* plan) const {
    return new GUIContainer(pars, vtype, plan);
}


/****************************************************************************/
