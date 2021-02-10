/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2021 German Aerospace Center (DLR) and others.
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
/// @file    FXRecentNetworks.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2021
///
//
/****************************************************************************/


#include "FXRecentNetworks.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(FXRecentNetworks) FXRecentNetworksMap[]={
    FXMAPFUNC(SEL_LEFTBUTTONPRESS, 0, FXRecentNetworks::onLeftBtnPress),
};

// Object implementation
FXIMPLEMENT(FXRecentNetworks, FXRecentFiles, FXRecentNetworksMap, ARRAYNUMBER(FXRecentNetworksMap))

// ===========================================================================
// member method definitions
// ===========================================================================

FXRecentNetworks::FXRecentNetworks() :
    FXRecentFiles() {
}


FXRecentNetworks::FXRecentNetworks(FXApp* a, const FXString& gp) :
    FXRecentFiles(a, gp) {
}


long
FXRecentNetworks::onLeftBtnPress(FXObject* obj, FXSelector sel, void* ptr) {
/*
    FXTreeList::onLeftBtnPress(obj, sel, ptr);
    // update height
    setHeight(getContentHeight() + 20);
*/
    return 1;
}


