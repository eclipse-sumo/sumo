/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXComboBox.cpp
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
///
// missing_desc
/****************************************************************************/
#include <config.h>
#include <iostream>

#include <fxkeys.h>
#include "MFXComboBox.h"


FXDEFMAP(MFXComboBox) MFXComboBoxMap[] = {
    FXMAPFUNC(SEL_KEYPRESS,             0,      MFXComboBox::onKeyPress),
};


// Object implementation
FXIMPLEMENT(MFXComboBox, FXComboBox, MFXComboBoxMap, ARRAYNUMBER(MFXComboBoxMap))

MFXComboBox::MFXComboBox(FXComposite* p, FXint cols, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb):
    FXComboBox(p, cols, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb) {
}


MFXComboBox::~MFXComboBox() {}


long
MFXComboBox::onKeyPress(FXObject* o, FXSelector sel, void* ptr) {
    FXEvent* e = (FXEvent*) ptr;
    if (e->code == FX::KEY_uparrow || e->code == FX::KEY_downarrow) {
        return FXComboBox::onKeyPress(o, sel, ptr);
    }
    return 0;
}

/****************************************************************************/
