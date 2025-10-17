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
/// @file    GNEElementList.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Table used in GNEElementList
/****************************************************************************/

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
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

GNEElementList::GNEElementList(FXVerticalFrame* contentFrame, const GNETagProperties* tagProperty, GNEElementList::Options options) :
    FXVerticalFrame(contentFrame, GUIDesignAuxiliarVerticalFrame),
    myTagProperty(tagProperty) {
    // horizontal frame for buttons
    FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create add button
    myAddButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::ADD),
                                            this, MID_GNE_ELEMENTLIST_ADD, GUIDesignButtonIcon);
    // add label with tag
    myLabel = new FXLabel(buttonFrame, TLF("%s", myTagProperty->getTagStr()).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    // check if add sort button
    if (options & GNEElementList::Options::SORTELEMENTS) {
        mySortButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::RELOAD),
                       this, MID_GNE_ELEMENTLIST_SORT, GUIDesignButtonIcon);
    }
    // create element table
    myElementTable = new GNEElementTable(this, myTagProperty, options);
}


GNEElementList::~GNEElementList() {}


void
GNEElementList::enableList() {
    myLabel->enable();
    myLabel->setText(TLF("%s", myTagProperty->getTagStr()).c_str());
    myAddButton->enable();
    if (mySortButton) {
        mySortButton->enable();
    }
    myElementTable->enable();
}


void
GNEElementList::disableList(const std::string& reason) {
    myLabel->disable();
    myLabel->setText(reason.c_str());
    myAddButton->disable();
    if (mySortButton) {
        mySortButton->disable();
    }
    myElementTable->disable();
}


bool
GNEElementList::isListValid() const {
    return myElementTable->isValid();
}


long
GNEElementList::onCmdAddRow(FXObject*, FXSelector, void*) {
    return addNewElement();
}


long
GNEElementList::onCmdSort(FXObject*, FXSelector, void*) {
    return sortRows();
}


void
GNEElementList::removeElementRecursively(GNEAdditional* additionalElement) const {
    // iterate over all children and delete it recursively
    const GNEHierarchicalContainerChildren<GNEAdditional*> additionalChildren = additionalElement->getChildAdditionals();
    for (const auto& additionalChild : additionalChildren) {
        removeElementRecursively(additionalChild);
    }
    const GNEHierarchicalContainerChildren<GNEDemandElement*> demandChildren = additionalElement->getChildDemandElements();
    for (const auto& demandChild : demandChildren) {
        removeElementRecursively(demandChild);
    }
    // delete element
    additionalElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_Additional(additionalElement, false), true);
}


void
GNEElementList::removeElementRecursively(GNEDemandElement* demandElement) const {
    // iterate over all children and delete it recursively
    const GNEHierarchicalContainerChildren<GNEAdditional*> additionalChildren = demandElement->getChildAdditionals();
    for (const auto& additionalChild : additionalChildren) {
        removeElementRecursively(additionalChild);
    }
    const GNEHierarchicalContainerChildren<GNEDemandElement*> demandChildren = demandElement->getChildDemandElements();
    for (const auto& demandChild : demandChildren) {
        removeElementRecursively(demandChild);
    }
    // delete element
    demandElement->getNet()->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(demandElement, false), true);
}

/****************************************************************************/
