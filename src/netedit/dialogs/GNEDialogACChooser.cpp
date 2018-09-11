/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialogACChooser.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2018
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <string>
#include <vector>
#include <fxkeys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIGlChildWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEViewParent.h>

#include "GNEDialogACChooser.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEDialogACChooser::GNEDialogACChooser(GNEViewParent* viewParent, FXIcon* icon, const std::string& title, const std::vector<GNEAttributeCarrier*>& ACs):
    GUIDialog_GLObjChooser(viewParent, icon, title.c_str(), getGLIds(ACs), GUIGlObjectStorage::gIDStorage),
    myACs(ACs),
    myViewParent(viewParent) {
}


GNEDialogACChooser::~GNEDialogACChooser() {
    myViewParent->eraseACChooserDialog(this);
}


void
GNEDialogACChooser::toggleSelection(int listIndex) {
    GNEAttributeCarrier* ac = myACs[listIndex];
    if (ac->isAttributeCarrierSelected()) {
        ac->unselectAttributeCarrier();
    } else {
        ac->selectAttributeCarrier();
    }
}


std::vector<GUIGlID>
GNEDialogACChooser::getGLIds(const std::vector<GNEAttributeCarrier*>& ACs) {
    std::vector<GUIGlID> ids;
    for (auto ac : ACs) {
        ids.push_back(dynamic_cast<GUIGlObject*>(ac)->getGlID());
    }
    return ids;
}

/****************************************************************************/

