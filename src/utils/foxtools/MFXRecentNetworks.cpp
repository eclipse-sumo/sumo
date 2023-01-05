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
/// @file    MFXRecentNetworks.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2021
///
//
/****************************************************************************/


#include "MFXRecentNetworks.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXRecentNetworks) MFXRecentNetworksMap[] = {
    FXMAPFUNC(SEL_UPDATE,   MFXRecentNetworks::ID_NOFILES,   MFXRecentNetworks::onUpdNoFiles),
    FXMAPFUNCS(SEL_UPDATE,  FXRecentFiles::ID_FILE_1,       FXRecentFiles::ID_FILE_10,  MFXRecentNetworks::onUpdFile),
};

// Object implementation
FXIMPLEMENT(MFXRecentNetworks, FXRecentFiles, MFXRecentNetworksMap, ARRAYNUMBER(MFXRecentNetworksMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXRecentNetworks::MFXRecentNetworks() :
    FXRecentFiles() {
}


MFXRecentNetworks::MFXRecentNetworks(FXApp* a, const FXString& gp) :
    FXRecentFiles(a, gp) {
}


long
MFXRecentNetworks::onUpdFile(FXObject* obj, FXSelector sel, void*) {
    // get filename index
    const FXint which = FXSELID(sel) - ID_FILE_1 + 1;
    // get filename
    const FXchar* filename;
    FXchar key[20];
    sprintf(key, "FILE%d", which);
    filename = getApp()->reg().readStringEntry(getGroupName().text(), key, NULL);
    // update myIndexFilenames
    myIndexFilenames[which] = filename;
    // check filename
    if (filename) {
        FXString string;
        if (which < 10) {
            string.format("&%d %s", which, filename);
        } else {
            string.format("1&0 %s", filename);
        }
        obj->handle(this, FXSEL(SEL_COMMAND, FXWindow::ID_SETSTRINGVALUE), (void*)&string);
        obj->handle(this, FXSEL(SEL_COMMAND, FXWindow::ID_SHOW), NULL);
    } else {
        obj->handle(this, FXSEL(SEL_COMMAND, FXWindow::ID_HIDE), NULL);
    }
    return 1;
}


long
MFXRecentNetworks::onUpdNoFiles(FXObject* obj, FXSelector, void*) {
    // first disable object
    obj->handle(obj, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), NULL);
    // iterate over myIndexFilenames
    for (const auto& indexFilename : myIndexFilenames) {
        // if filename isn't empty, then hide object and stop
        if (!indexFilename.second.empty()) {
            obj->handle(obj, FXSEL(SEL_COMMAND, FXWindow::ID_HIDE), NULL);
            return 1;
        }
    }
    //show object
    obj->handle(obj, FXSEL(SEL_COMMAND, FXWindow::ID_SHOW), NULL);
    return 1;
}
