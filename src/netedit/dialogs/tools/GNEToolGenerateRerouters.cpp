/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEToolGenerateRerouters.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for generate rerouters
/****************************************************************************/

#include <netedit/templates.h>

#include "GNEToolGenerateRerouters.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEToolGenerateRerouters::GNEToolGenerateRerouters(GNEApplicationWindow* GNEApp) :
    GNEToolDialog(GNEApp, "Generate rerouters", generateReroutersTemplate, 200, 200) {
}


GNEToolGenerateRerouters::~GNEToolGenerateRerouters() {}



/****************************************************************************/
