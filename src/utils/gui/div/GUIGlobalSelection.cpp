/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGlobalSelection.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// A global holder of selected objects
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GUIGlobalSelection.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUISelectedStorage.h>

#include <algorithm>


// ===========================================================================
// global variables definitions
// ===========================================================================
GUISelectedStorage gSelected;



/****************************************************************************/

