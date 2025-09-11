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
/// @file    GNEHelpBasicDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Custom FXDialogBox help dialog used in Netedit that supports internal tests
/****************************************************************************/
#include <config.h>

#include <sstream>

#include "GNEHelpBasicDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEHelpBasicDialog::GNEHelpBasicDialog(GNEApplicationWindow* applicationWindow,
                                       const std::string& name, const std::ostringstream& stream) :
    GNEBasicDialog(applicationWindow, name, stream.str(), GUIIcon::HELP, DialogType::BASIC_HELP,
                   GNEDialog::Buttons::OK, GUIIcon::EMPTY) {
}
