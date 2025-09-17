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
/// @file    GNEColorDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Custom GNEColorDialog used in Netedit that supports internal tests
/****************************************************************************/

#include <netedit/elements/GNEAttributeCarrier.h>
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

GNEColorDialog::GNEColorDialog(GNEApplicationWindow* applicationWindow, const RGBColor color):
    GNEDialog(applicationWindow, TL("Edit color"), GUIIcon::COLORWHEEL, DialogType::COLOR,
              Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC, 600, 300),
    myOriginalColor(color) {
    myColorbox = new FXColorSelector(getContentFrame(), this, FXColorDialog::ID_COLORSELECTOR, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    // set color
    myColorbox->setRGBA(MFXUtils::getFXColor(color));
    // hide buttons
    myColorbox->acceptButton()->disable();
    myColorbox->acceptButton()->hide();
    myColorbox->cancelButton()->disable();
    myColorbox->cancelButton()->hide();
    // open dialog
    openDialog();
}


GNEColorDialog::~GNEColorDialog() {
    myColorbox = (FXColorSelector*) - 1L;
}


void
GNEColorDialog::runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
    if (GNEAttributeCarrier::canParse<RGBColor>(dialogArgument->getCustomAction())) {
        // parse color
        const auto color = GNEAttributeCarrier::parse<RGBColor>(dialogArgument->getCustomAction());
        // set color in colorbox
        myColorbox->setRGBA(MFXUtils::getFXColor(color));
    } else {
        WRITE_ERROR("Cannot parse color " + dialogArgument->getCustomAction() + " in internal test");
    }
}


RGBColor
GNEColorDialog::getColor() const {
    return MFXUtils::getRGBColor(myColorbox->getRGBA());
}


long
GNEColorDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // restore original color
    myColorbox->setRGBA(MFXUtils::getFXColor(myOriginalColor));
    return 1;
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
