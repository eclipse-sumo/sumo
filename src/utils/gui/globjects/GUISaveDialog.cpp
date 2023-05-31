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
/// @file    GUISaveDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Dialog for ask user about saving elements
/****************************************************************************/

#include "GUISaveDialog.h"

#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/MsgHandler.h>

// Padding for message box buttons
#define HORZ_PAD 30
#define VERT_PAD 2

// Map
FXDEFMAP(GUISaveDialog) GUISaveDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  GUISaveDialog::CLICKED_SAVE,    GUISaveDialog::onCmdClicked),
    FXMAPFUNC(SEL_COMMAND,  GUISaveDialog::CLICKED_DISCARD, GUISaveDialog::onCmdClicked),
    FXMAPFUNC(SEL_COMMAND,  GUISaveDialog::CLICKED_ABORT,   GUISaveDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GUISaveDialog, FXDialogBox, GUISaveDialogMap, ARRAYNUMBER(GUISaveDialogMap))


GUISaveDialog::GUISaveDialog(FXApp* a, const FXString& caption, const FXString& text, FXIcon* ic):
    FXDialogBox(a, caption, DECOR_TITLE | DECOR_BORDER, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4) {
    initialize(text, ic);
}


long
GUISaveDialog::onCmdClicked(FXObject*, FXSelector sel, void*) {
    if (sel == FXSEL(SEL_COMMAND, GUISaveDialog::CLICKED_SAVE)) {
        getApp()->stopModal(this, GUISaveDialog::CLICKED_SAVE);
    } else if (sel == FXSEL(SEL_COMMAND, GUISaveDialog::CLICKED_DISCARD)) {
        getApp()->stopModal(this, GUISaveDialog::CLICKED_DISCARD);
    } else {
        getApp()->stopModal(this, GUISaveDialog::CLICKED_ABORT);
    }
    hide();
    return 1;
}


long
GUISaveDialog::onCmdCancel(FXObject* sender, FXSelector, void* ptr) {
    return GUISaveDialog::onCmdClicked(sender, FXSEL(SEL_COMMAND, FXMessageBox::ID_CLICKED_CANCEL), ptr);
}


FXuint
GUISaveDialog::question(FXApp* app, const char* caption, const char* message, ...) {
    FXGIFIcon icon(app, myQuestionIcon);
    va_list arguments;
    va_start(arguments, message);
    GUISaveDialog box(app, caption, FXStringVFormat(message, arguments), &icon);
    va_end(arguments);
    return box.execute(PLACEMENT_SCREEN);
}


void
GUISaveDialog::initialize(const FXString& text, FXIcon* ic) {
    FXVerticalFrame* content = new FXVerticalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    FXHorizontalFrame* info = new FXHorizontalFrame(content, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    new FXLabel(info, FXString::null, ic, ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXLabel(info, text, NULL, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXHorizontalSeparator(content, SEPARATOR_GROOVE | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X);
    FXHorizontalFrame* buttons = new FXHorizontalFrame(content, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH, 0, 0, 0, 0, 10, 10, 5, 5);
    buttons->setPackingHints(PACK_NORMAL);
    FXButton* saveChanges = new FXButton(buttons, TL("&Save changes"), NULL, this, CLICKED_SAVE, GUIDesignSaveDialogButtonInitial);
    new FXButton(buttons, TL("&Don't save"), NULL, this, CLICKED_DISCARD, GUIDesignSaveDialogButton);
    new FXButton(buttons, TL("&Abort"), NULL, this, CLICKED_ABORT, GUIDesignSaveDialogButton);
    saveChanges->setFocus();
}


const unsigned char
GUISaveDialog::myQuestionIcon[] = {
    0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x20, 0x00, 0x20, 0x00, 0xf2, 0x00, 0x00, 0x80, 0x80, 0x80,
    0xc0, 0xc0, 0xc0, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x03,
    0x97, 0x08, 0xba, 0xdc, 0xfe, 0xf0, 0x05, 0x41, 0x6b, 0x88, 0x58, 0xcd, 0xca, 0xf9, 0xcd, 0xcc,
    0xd6, 0x8d, 0x16, 0x08, 0x90, 0x02, 0x30, 0x0c, 0x80, 0xc8, 0x11, 0x18, 0x19, 0xac, 0xf4, 0xe0,
    0x56, 0xf0, 0x23, 0xdb, 0x54, 0x7d, 0x53, 0xb9, 0x10, 0x69, 0xc5, 0xa9, 0xc9, 0x82, 0x0a, 0x94,
    0x51, 0xb0, 0x1c, 0x21, 0x7f, 0x3d, 0x9a, 0x60, 0x26, 0x3d, 0x2e, 0x50, 0xd3, 0xd5, 0xa4, 0x49,
    0x0a, 0x62, 0x3d, 0x55, 0xac, 0xf7, 0x1b, 0x1d, 0x90, 0x05, 0x63, 0xb2, 0xf6, 0x9c, 0xfe, 0x12,
    0xd9, 0x0b, 0xe8, 0xd9, 0x7a, 0xfd, 0x52, 0xe5, 0xb8, 0x06, 0x9e, 0x19, 0xa6, 0x33, 0xdc, 0x7d,
    0x4e, 0x0f, 0x04, 0x58, 0x77, 0x62, 0x11, 0x84, 0x73, 0x82, 0x18, 0x04, 0x7b, 0x23, 0x01, 0x48,
    0x88, 0x8d, 0x67, 0x90, 0x91, 0x19, 0x04, 0x98, 0x3f, 0x95, 0x96, 0x26, 0x0a, 0x98, 0x93, 0x40,
    0x9d, 0x26, 0x89, 0x68, 0xa2, 0x20, 0xa4, 0x9c, 0xa6, 0x0d, 0x89, 0xa9, 0xaa, 0x0c, 0x84, 0xad,
    0xae, 0x0b, 0x98, 0xb2, 0xb5, 0xb6, 0xaa, 0x09, 0x00, 0x3b
};

/*******************************************************************************/
