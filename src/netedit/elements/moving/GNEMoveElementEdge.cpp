/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEMoveElementEdge.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving edge shapes
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNEViewParent.h>

#include "GNEMoveElementEdge.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementEdge::GNEMoveElementEdge(GNEEdge* edge) :
    GNEMoveElement(edge),
    myEdge(edge) {
}


GNEMoveElementEdge::~GNEMoveElementEdge() {}

/****************************************************************************/
