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
/// @file    GNEColorDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Custom GNEColorDialog used in Netedit that supports internal tests
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>

#include "GNEColorDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEColorDialog) GNEColorDialogMap[] = {
    FXMAPFUNC(SEL_CHANGED,  FXColorDialog::ID_COLORSELECTOR,   GNEColorDialog::onChgColor),
    FXMAPFUNC(SEL_COMMAND,  FXColorDialog::ID_COLORSELECTOR,   GNEColorDialog::onCmdColor),
};

// Object implementation
FXIMPLEMENT(GNEColorDialog, GNEDialog, GNEColorDialogMap, ARRAYNUMBER(GNEColorDialogMap))

// ===========================================================================
// method definitions
// ===========================================================================

// Separator item
GNEColorDialog::GNEColorDialog(GNEApplicationWindow* applicationWindow, const RGBColor color):
    GNEDialog(applicationWindow, TL("Custom Geometry Point"), GUIIcon::COLORWHEEL,
              Buttons::ACCEPT_CANCEL, OpenType::MODAL, 600, 300) {
    colorbox = new FXColorSelector(getContentFrame(), this, FXColorDialog::ID_COLORSELECTOR, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // set color
    colorbox->setRGBA(MFXUtils::getFXColor(color));
    // hide buttons
    colorbox->acceptButton()->disable();
    colorbox->acceptButton()->hide();
    colorbox->cancelButton()->disable();
    colorbox->cancelButton()->hide();
    // open dialog
    openDialog();
}


GNEColorDialog::~GNEColorDialog() {
    colorbox = (FXColorSelector*) - 1L;
}


void
GNEColorDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
}


RGBColor
GNEColorDialog::getColor() const {
    return MFXUtils::getRGBColor(colorbox->getRGBA());
}


long
GNEColorDialog::onChgColor(FXObject*, FXSelector, void* ptr) {
    if (target) {
        return target->tryHandle(this, FXSEL(SEL_CHANGED, message), ptr);
    } else {
        return 0;
    }
}


long
GNEColorDialog::onCmdColor(FXObject*, FXSelector, void* ptr) {
    if (target) {
        return target->tryHandle(this, FXSEL(SEL_COMMAND, message), ptr);
    } else {
        return 0;
    }
}
