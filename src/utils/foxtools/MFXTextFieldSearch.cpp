/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXTextFieldSearch.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// TextField for search elements
/****************************************************************************/

#include "MFXTextFieldSearch.h"


MFXTextFieldSearch::MFXTextFieldSearch(FXComposite* p, FXint ncols, FXObject* tgt, FXSelector sel, FXuint opt, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) :
    FXTextField(p, ncols, tgt, sel, opt, x, y, w, h, pl, pr, pt, pb) {
}