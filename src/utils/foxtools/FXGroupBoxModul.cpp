/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    FXGroupBoxModul.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>

#include "FXGroupBoxModul.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXGroupBoxModul) FXGroupBoxModulMap[] = {
    FXMAPFUNC(SEL_PAINT,  0,  FXGroupBoxModul::onPaint),
};

// Object implementation
FXIMPLEMENT(FXGroupBoxModul, FXGroupBox, FXGroupBoxModulMap, ARRAYNUMBER(FXGroupBoxModulMap))

// ===========================================================================
// method definitions
// ===========================================================================

FXGroupBoxModul::FXGroupBoxModul(FXVerticalFrame* contentFrame, const std::string &text) :
    FXGroupBox(contentFrame, text.c_str(), GUIDesignGroupBoxFrame) {
}


FXGroupBoxModul::~FXGroupBoxModul() {}


long
FXGroupBoxModul::onPaint(FXObject* obj ,FXSelector sel, void* ptr) {
    return FXGroupBox::onPaint(obj, sel, ptr);
}


FXGroupBoxModul::FXGroupBoxModul() {}