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
/// @file    GNEOverwriteElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2017
///
// Dialog used to ask user if overwrite elements during loading
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/CommonHandler.h>

#include "GNEOverwriteElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEOverwriteElement::GNEOverwriteElement(CommonHandler* commonHandler, const GNEAttributeCarrier* AC) :
    GNEDialog(AC->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
              TLF("Overwrite % '%'", AC->getTagProperty()->getTagStr(), AC->getID()), GUIIcon::QUESTION_SMALL,
              DialogType::OVERWRITE, GNEDialog::Buttons::YES_NO_CANCEL, GNEDialog::OpenType::MODAL, ResizeMode::STATIC),
    myCommonHandler(commonHandler) {
    // create dialog layout (obtained from FXMessageBox)
    auto infoFrame = new FXVerticalFrame(myContentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    // add information label
    new FXLabel(infoFrame, TLF("There is already a % '%'. Overwrite?", AC->getTagProperty()->getTagStr(), AC->getID()).c_str(),
                nullptr, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // add checkButton
    myApplySolutionToAllCheckButon =  new FXCheckButton(infoFrame, TL("Apply this solution to all conflicted elements"), nullptr, 0, GUIDesignCheckButton);
    // open modal dialog
    openDialog();
}


GNEOverwriteElement::~GNEOverwriteElement() {
}


void
GNEOverwriteElement::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    if (dialogArgument->getCustomAction() == "applyToAll") {
        myApplySolutionToAllCheckButon->setCheck(TRUE);
    }
}



long
GNEOverwriteElement::onCmdAccept(FXObject*, FXSelector, void*) {
    if (myApplySolutionToAllCheckButon->getCheck() == TRUE) {
        myCommonHandler->forceOverwriteElements();
    }
    return closeDialogAccepting();
}


long
GNEOverwriteElement::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myApplySolutionToAllCheckButon->getCheck() == TRUE) {
        myCommonHandler->forceRemainElements();
    }
    return closeDialogCanceling();
}


long
GNEOverwriteElement::onCmdAbort(FXObject*, FXSelector, void*) {
    myCommonHandler->abortLoading();
    return closeDialogAborting();
}

/****************************************************************************/
