/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEDataHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDataHandler::GNEDataHandler(const std::string& file, GNEViewNet* viewNet, bool undoDataElements) :
    SUMOSAXHandler(file, ""),
    myViewNet(viewNet),
    myUndoDataElements(undoDataElements) {
}


GNEDataHandler::~GNEDataHandler() {}



/****************************************************************************/
