/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEDialogACChooser.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2018
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/

#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>

#include "GNEDialogACChooser.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEDialogACChooser::GNEDialogACChooser(GNEViewParent* viewParent, int messageId, FXIcon* icon, const std::string& title, const std::vector<GNEAttributeCarrier*>& ACs):
    GUIDialog_ChooserAbstract(viewParent, messageId, icon, title.c_str(), std::vector<GUIGlID>(), GUIGlObjectStorage::gIDStorage),
    myViewParent(viewParent),
    myACs(ACs),
    myFilteredACs(ACs),
    myLocateTLS(title.find("TLS") != std::string::npos) {
    // @note refresh must be called here because the base class constructor cannot
    // call the virtual function getObjectName
    std::vector<GUIGlID> ids;
    for (const auto& AC : ACs) {
        ids.push_back(AC->getGUIGlObject()->getGlID());
    }
    refreshList(ids);
}


GNEDialogACChooser::~GNEDialogACChooser() {
    myViewParent->eraseACChooserDialog(this);
}


void
GNEDialogACChooser::toggleSelection(int listIndex) {
    // always filtered ACs
    GNEAttributeCarrier* ac = myFilteredACs[listIndex];
    if (ac->isAttributeCarrierSelected()) {
        ac->unselectAttributeCarrier();
    } else {
        ac->selectAttributeCarrier();
    }
}


void
GNEDialogACChooser::select(int listIndex) {
    // always filtered ACs
    GNEAttributeCarrier* ac = myFilteredACs[listIndex];
    if (!ac->isAttributeCarrierSelected()) {
        ac->selectAttributeCarrier();
    }
}


void
GNEDialogACChooser::deselect(int listIndex) {
    // always filtered ACs
    GNEAttributeCarrier* ac = myFilteredACs[listIndex];
    if (ac->isAttributeCarrierSelected()) {
        ac->unselectAttributeCarrier();
    }
}


void
GNEDialogACChooser::filterACs(const std::vector<GUIGlID>& GLIDs) {
    if (GLIDs.empty()) {
        myFilteredACs = myACs;
    } else {
        // clear myFilteredACs
        myFilteredACs.clear();
        // iterate over myACs
        for (const auto& AC : myACs) {
            // search in GLIDs
            if (std::find(GLIDs.begin(), GLIDs.end(), AC->getGUIGlObject()->getGlID()) != GLIDs.end()) {
                myFilteredACs.push_back(AC);
            }
        }
    }
}


std::string
GNEDialogACChooser::getObjectName(GUIGlObject* o) const {
    // check if we're locating a TLS
    if (myLocateTLS) {
        // obtain junction
        GNEJunction* junction = dynamic_cast<GNEJunction*>(o);
        // check that junction exist
        if (junction == nullptr) {
            throw ProcessError(TL("Invalid Junction"));
        }
        // get definitions
        const std::set<NBTrafficLightDefinition*>& defs = junction->getNBNode()->getControllingTLS();
        // check that traffic light exists
        if (defs.empty()) {
            throw ProcessError(TL("Invalid number of TLSs"));
        }
        // get TLDefinition
        const std::string& tlDefID = (*defs.begin())->getID();
        if (tlDefID == o->getMicrosimID()) {
            return o->getMicrosimID();
        } else {
            return tlDefID + " (" + o->getMicrosimID() + ")";
        }
    } else {
        return GUIDialog_ChooserAbstract::getObjectName(o);
    }
}


/****************************************************************************/
