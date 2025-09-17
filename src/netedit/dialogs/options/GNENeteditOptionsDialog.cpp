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
/// @file    GNENeteditOptionsDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>
#include <utils/foxtools/MFXCheckButtonTooltip.h>

#include "GNENeteditOptionsDialog.h"
#include "GNEOptionsEditor.h"

// ===========================================================================
// Defines
// ===========================================================================

#define TREELISTWIDTH 200

// ===========================================================================
// method definitions
// ===========================================================================

GNENeteditOptionsDialog::GNENeteditOptionsDialog(GNEApplicationWindow* applicationWindow, OptionsCont& optionsContainer,
        const OptionsCont& originalOptionsContainer) :
    GNEDialog(applicationWindow, TL("Edit netedit options"), GUIIcon::OPTIONS, DialogType::OPTIONS_NETEDIT,
              GNEDialog::Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, GNEDialog::ResizeMode::RESIZABLE, 800, 600) {
    // build options editor
    myOptionsEditor = new GNEOptionsEditor(this, "Netedit", optionsContainer, originalOptionsContainer);
    // open modal dialog
    openDialog();
}


GNENeteditOptionsDialog::~GNENeteditOptionsDialog() { }


void
GNENeteditOptionsDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    myOptionsEditor->runInternalTest(dialogArgument);
}


bool
GNENeteditOptionsDialog::isOptionModified() const {
    return myOptionsEditor->isOptionModified();
}


long
GNENeteditOptionsDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // reset options
    myOptionsEditor->resetAllOptions();
    // close dialog canceling
    return closeDialogCanceling();
}


long
GNENeteditOptionsDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset options
    myOptionsEditor->resetAllOptions();
    return 1;
}

/****************************************************************************/
