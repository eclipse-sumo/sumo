/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEElementTable.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Table used in GNEElementList
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEElementList.h"
#include "GNEElementTable.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEElementList) GNEElementListMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTLIST_ADD,    GNEElementList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ELEMENTLIST_SORT,   GNEElementList::onCmdSort)
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEElementList, FXVerticalFrame, GNEElementListMap, ARRAYNUMBER(GNEElementListMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEElementList::GNEElementList(FXVerticalFrame* contentFrame, const GNETagProperties* tagProperty,
                               const bool fixHeight) :
    FXVerticalFrame(contentFrame, GUIDesignAuxiliarVerticalFrame),
    myTagProperty(tagProperty) {
    // horizontal frame for buttons
    FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create buttons and labels
    myAddButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::ADD),
                                            this, MID_GNE_ELEMENTLIST_ADD, GUIDesignButtonIcon);
    mySortButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::RELOAD),
                   this, MID_GNE_ELEMENTLIST_SORT, GUIDesignButtonIcon);
    myLabel = new FXLabel(buttonFrame, TLF("%s", myTagProperty->getTagStr()).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    // create element table
    myElementTable = new GNEElementTable(this, myTagProperty, fixHeight);
}


GNEElementList::~GNEElementList() {}


void
GNEElementList::enableList() {
    myLabel->enable();
    myLabel->setText(TLF("%s", myTagProperty->getTagStr()).c_str());
    myAddButton->enable();
    mySortButton->enable();
    myElementTable->enable();
}


void
GNEElementList::disableList(const std::string& reason) {
    myLabel->disable();
    myLabel->setText(reason.c_str());
    myAddButton->disable();
    mySortButton->disable();
    myElementTable->disable();
}


bool
GNEElementList::isListValid() const {
    return myElementTable->isValid();
}


long
GNEElementList::onCmdAddRow(FXObject* sender, FXSelector, void*) {
    return addRow();
}


long
GNEElementList::onCmdSort(FXObject* sender, FXSelector, void*) {
    return sortRows();
}

/****************************************************************************/
