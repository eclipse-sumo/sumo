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
/// @file    GNEAdditionalListed.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// A abstract class for representation of additional listed elements
/****************************************************************************/

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEPathCreator.h>
#include <netedit/frames/GNEPlanCreator.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEAdditionalListed.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalListed::GNEAdditionalListed() {
}

/****************************************************************************/
