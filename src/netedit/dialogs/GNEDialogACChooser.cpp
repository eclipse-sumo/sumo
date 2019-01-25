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
#include <netedit/netelements/GNEJunction.h>

#include "GNEDialogACChooser.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEDialogACChooser::GNEDialogACChooser(GNEViewParent* viewParent, FXIcon* icon, const std::string& title, const std::vector<GNEAttributeCarrier*>& ACs):
    GUIDialog_GLObjChooser(viewParent, icon, title.c_str(), std::vector<GUIGlID>(), GUIGlObjectStorage::gIDStorage),
    myACs(ACs),
    myViewParent(viewParent),
    myLocateTLS(title.find("TLS") != std::string::npos)
{
    // @note refresh must be called here because the base class constructor cannot
    // call the virtual function getObjectName
    std::vector<GUIGlID> ids;
    for (auto ac : ACs) {
        ids.push_back(dynamic_cast<GUIGlObject*>(ac)->getGlID());
    }
    refreshList(ids);
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


std::string
GNEDialogACChooser::getObjectName(GUIGlObject* o) const {
    if (myLocateTLS) {
        GNEJunction* junction = dynamic_cast<GNEJunction*>(o);
        assert(junction != nullptr);
        const std::set<NBTrafficLightDefinition*>& defs = junction->getNBNode()->getControllingTLS();
        assert(defs.size() > 0);
        NBTrafficLightDefinition* tlDef = *defs.begin();
        if (tlDef->getID() == o->getMicrosimID()) {
            return o->getMicrosimID();
        } else {
            return tlDef->getID() + " (" + o->getMicrosimID() + ")";
        }
    } else {
        return o->getMicrosimID();
    }
}

/****************************************************************************/

