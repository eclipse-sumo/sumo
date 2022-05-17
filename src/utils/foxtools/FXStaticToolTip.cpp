/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2022 German Aerospace Center (DLR) and others.
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
/// @file    FXStaticToolTip.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2022
///
//
/****************************************************************************/

/* =========================================================================
* included modules
* ======================================================================= */
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "FXStaticToolTip.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXStaticToolTip) FXStaticToolTipMap[] = {
    FXMAPFUNC(SEL_PAINT,    0,  FXStaticToolTip::onPaint),
};

// Object implementation
FXIMPLEMENT(FXStaticToolTip, FXToolTip, FXStaticToolTipMap, ARRAYNUMBER(FXStaticToolTipMap))

// ===========================================================================
// method definitions
// ===========================================================================

FXStaticToolTip::FXStaticToolTip(FXApp* app) :
    FXToolTip(app) {
}


FXStaticToolTip::~FXStaticToolTip() {}


long 
FXStaticToolTip::onPaint(FXObject* obj, FXSelector sel, void* ptr) {
    return FXToolTip::onPaint(obj, sel, ptr);
}


FXStaticToolTip::FXStaticToolTip() :
    FXToolTip() {
}
