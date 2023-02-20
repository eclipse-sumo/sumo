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
/// @file    MFXRealSpinner.cpp
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
///
// missing_desc
/****************************************************************************/
#include <config.h>
#include <iostream>

#include <fxkeys.h>
#include "MFXRealSpinner.h"


FXDEFMAP(MFXRealSpinner) MFXRealSpinnerMap[] = {
    FXMAPFUNC(SEL_KEYPRESS,             0,      MFXRealSpinner::onKeyPress),
};


// Object implementation
FXIMPLEMENT(MFXRealSpinner, FXRealSpinner, MFXRealSpinnerMap, ARRAYNUMBER(MFXRealSpinnerMap))

MFXRealSpinner::MFXRealSpinner(FXComposite* p, FXint cols, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb):
    FXRealSpinner(p, cols, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb) {
}


MFXRealSpinner::~MFXRealSpinner() {}


long
MFXRealSpinner::onKeyPress(FXObject* o, FXSelector sel, void* ptr) {
    FXEvent* e = (FXEvent*) ptr;
    if ((e->code >= FX::KEY_a && e->code <= FX::KEY_z) || e->code == FX::KEY_space) {
        return 0;
    }
    return FXRealSpinner::onKeyPress(o, sel, ptr);
}

/****************************************************************************/
