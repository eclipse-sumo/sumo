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
/// @file    MFXCheckButtonTooltip.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2026
///
// reimplementation of MFXColorDialog, which allow to test it using internal test
/****************************************************************************/

#include "MFXColorDialog.h"

MFXColorDialog::MFXColorDialog(FXWindow* owner, const FXString& name, FXuint opts, FXint x, FXint y, FXint w, FXint h) :
    FXColorDialog(owner, name, opts, x, y, w, h) {
}


MFXColorDialog::~MFXColorDialog() {
    colorbox = (FXColorSelector*) - 1L;
}


FXuint
MFXColorDialog::openDialog(const InternalTest* internalTest, FXuint placement) {
    if (internalTest) {
        create();
        show(placement);
        getApp()->refresh();
        return getApp()->runModalFor(this);
    } else {
        return execute(placement);
    }
}
