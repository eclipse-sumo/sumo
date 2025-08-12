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
GNEColorDialog::GNEColorDialog(GNEApplicationWindow* applicationWindow):
    GNEDialog(applicationWindow, TL("Custom Geometry Point"), GUIIcon::COLORWHEEL,
              Buttons::ACCEPT_CANCEL, OpenType::MODAL, 320, 80) {
    colorbox = new FXColorSelector(this, this, FXColorDialog::ID_COLORSELECTOR, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    colorbox->acceptButton()->setTarget(this);
    colorbox->acceptButton()->setSelector(FXDialogBox::ID_ACCEPT);
    colorbox->cancelButton()->setTarget(this);
    colorbox->cancelButton()->setSelector(FXDialogBox::ID_CANCEL);
}


GNEColorDialog::~GNEColorDialog() {
    colorbox = (FXColorSelector*) - 1L;
}


void
GNEColorDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
}


void
GNEColorDialog::setRGBA(FXColor clr) {
    colorbox->setRGBA(clr);
}


FXColor
GNEColorDialog::getRGBA() const {
    return colorbox->getRGBA();
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


FXbool
GNEColorDialog::isOpaqueOnly() const {
    return colorbox->isOpaqueOnly();
}


void
GNEColorDialog::setOpaqueOnly(FXbool forceopaque) {
    colorbox->setOpaqueOnly(forceopaque);
}


void
GNEColorDialog::save(FXStream& store) const {
    FXDialogBox::save(store);
    store << colorbox;
}


void
GNEColorDialog::load(FXStream& store) {
    FXDialogBox::load(store);
    store >> colorbox;
}
