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
/// @file    GNEDistributionRefDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog for edit attribute carriers
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewParent.h>
#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDistributionRefDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionRefDialog) GNEDistributionRefDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DISTRIBUTIOND_REFERENCE,    GNEDistributionRefDialog::onCmdSetReference),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DISTRIBUTIOND_PROBABILITY,  GNEDistributionRefDialog::onCmdSetProbability)
};

// Object implementation
FXIMPLEMENT(GNEDistributionRefDialog, GNEDialog, GNEDistributionRefDialogMap, ARRAYNUMBER(GNEDistributionRefDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDistributionRefDialog::GNEDistributionRefDialog(GNEAttributeCarrier* distributionParent) :
    GNEDialog(distributionParent->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
              TLF("Add % reference", distributionParent->getTagStr()), distributionParent->getTagProperty()->getGUIIcon(),
              DialogType::DISTRIBUTION_REF, GNEDialog::Buttons::ACCEPT_CANCEL, OpenType::MODAL, ResizeMode::STATIC) {
    auto tooltipMenu = distributionParent->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // create elements
    FXHorizontalFrame* referenceFrames = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    myReferencesComboBox = new MFXComboBoxIcon(referenceFrames, tooltipMenu, true, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_DISTRIBUTIOND_REFERENCE, GUIDesignComboBox);

    FXHorizontalFrame* probabilityFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    myProbabilityTextField = new MFXTextFieldIcon(probabilityFrame, tooltipMenu, GUIIcon::EMPTY, this, MID_GNE_ATTRIBUTESEDITORROW_SETATTRIBUTE, GUIDesignTextField);
    // open dialog
    openDialog();
}


GNEDistributionRefDialog::~GNEDistributionRefDialog() {}


void
GNEDistributionRefDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEDistributionRefDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEDistributionRefDialog::onCmdSetReference(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEDistributionRefDialog::onCmdSetProbability(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
