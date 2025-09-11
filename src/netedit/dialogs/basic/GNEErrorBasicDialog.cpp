/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEErrorBasicDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Custom FXDialogBox error dialog used in Netedit that supports internal tests
/****************************************************************************/
#include <config.h>

#include "GNEErrorBasicDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEErrorBasicDialog::GNEErrorBasicDialog(GNEApplicationWindow* applicationWindow,
        const std::string& name, const std::string& info) :
    GNEBasicDialog(applicationWindow, name, info, GUIIcon::ERROR_SMALL, DialogType::BASIC_ERROR,
                   GNEDialog::Buttons::OK, GUIIcon::ERROR_LARGE) {
}


GNEErrorBasicDialog::GNEErrorBasicDialog(GNEApplicationWindow* applicationWindow,
        const std::string& name, const std::string& infoLineA, const std::string& infoLineB) :
    GNEBasicDialog(applicationWindow, name, infoLineA + "\n" + infoLineB, GUIIcon::ERROR_SMALL,
                   DialogType::BASIC_ERROR, GNEDialog::Buttons::OK, GUIIcon::ERROR_LARGE) {
}
