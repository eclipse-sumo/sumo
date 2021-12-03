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

#include "FXGroupBoxModul.h"


// Map
FXDEFMAP(FXGroupBoxModul) FXGroupBoxModulMap[] = {
    FXMAPFUNC(SEL_PAINT,  0,  FXGroupBoxModul::onPaint),

};

// Object implementation
FXIMPLEMENT(FXGroupBoxModul, FXGroupBox, FXGroupBoxModulMap, ARRAYNUMBER(FXGroupBoxModulMap))


FXGroupBoxModul::FXGroupBoxModul(FXComposite* p, const FXString& text, FXuint opts, FXint x, FXint y, FXint w, FXint h, 
                                 FXint pl, FXint pr, FXint pt, FXint pb, FXint hs, FXint vs) :
    FXGroupBox(p, text, opts, x, y, w, h, pl, pr, pt, pb, hs, vs) {
}


FXGroupBoxModul::FXGroupBoxModul() {
}