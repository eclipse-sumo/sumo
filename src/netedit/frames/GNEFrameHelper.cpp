/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNETAZ.h>
#include <netedit/changes/GNEChange_Children.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/dialogs/GNEDialog_AllowDisallow.h>
#include <netedit/dialogs/GNEGenericParameterDialog.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/vehicle/SUMOVehicleParameter.h>

#include "GNEFrame.h"
#include "GNEFrameHelper.h"
#include "GNEInspectorFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameHelper::TagSelector) TagSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAGTYPE_SELECTED,    GNEFrameHelper::TagSelector::onCmdSelectTagType),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAG_SELECTED,        GNEFrameHelper::TagSelector::onCmdSelectTag)
};

FXDEFMAP(GNEFrameHelper::DemandElementSelector) DemandElementSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEFrameHelper::DemandElementSelector::onCmdSelectDemandElement),
};

FXDEFMAP(GNEFrameHelper::EdgePathCreator) EdgePathCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_ABORT,      GNEFrameHelper::EdgePathCreator::onCmdAbortRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_FINISH,     GNEFrameHelper::EdgePathCreator::onCmdFinishRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_REMOVELAST, GNEFrameHelper::EdgePathCreator::onCmdRemoveLastInsertedElement)
};

FXDEFMAP(GNEFrameHelper::AttributesCreatorRow) RowCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,              GNEFrameHelper::AttributesCreatorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,         GNEFrameHelper::AttributesCreatorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,       GNEFrameHelper::AttributesCreatorRow::onCmdSelectColorButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_RADIOBUTTON,  GNEFrameHelper::AttributesCreatorRow::onCmdSelectRadioButton)
};

FXDEFMAP(GNEFrameHelper::AttributesCreator) AttributesCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameHelper::AttributesCreator::onCmdHelp)
};

FXDEFMAP(GNEFrameHelper::AttributesEditorRow) AttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,              GNEFrameHelper::AttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,         GNEFrameHelper::AttributesEditorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,       GNEFrameHelper::AttributesEditorRow::onCmdOpenAttributeDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_RADIOBUTTON,  GNEFrameHelper::AttributesEditorRow::onCmdSelectRadioButton)
};

FXDEFMAP(GNEFrameHelper::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameHelper::AttributesEditor::onCmdAttributesEditorHelp)
};

FXDEFMAP(GNEFrameHelper::AttributesEditorExtended) AttributesEditorExtendedMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameHelper::AttributesEditorExtended::onCmdOpenDialog)
};

FXDEFMAP(GNEFrameHelper::AttributeCarrierHierarchy) AttributeCarrierHierarchyMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_CENTER,                     GNEFrameHelper::AttributeCarrierHierarchy::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECT,                    GNEFrameHelper::AttributeCarrierHierarchy::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETE,                     GNEFrameHelper::AttributeCarrierHierarchy::onCmdDeleteItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_ACHIERARCHY_MOVEUP,         GNEFrameHelper::AttributeCarrierHierarchy::onCmdMoveItemUp),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_ACHIERARCHY_MOVEDOWN,       GNEFrameHelper::AttributeCarrierHierarchy::onCmdMoveItemDown),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_ACHIERARCHY_SHOWCHILDMENU,  GNEFrameHelper::AttributeCarrierHierarchy::onCmdShowChildMenu)
};

FXDEFMAP(GNEFrameHelper::GenericParametersEditor) GenericParametersEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameHelper::GenericParametersEditor::onCmdEditGenericParameter),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameHelper::GenericParametersEditor::onCmdSetGenericParameter)
};

FXDEFMAP(GNEFrameHelper::DrawingShape) DrawingShapeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STARTDRAWING,   GNEFrameHelper::DrawingShape::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STOPDRAWING,    GNEFrameHelper::DrawingShape::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORTDRAWING,   GNEFrameHelper::DrawingShape::onCmdAbortDrawing)
};

FXDEFMAP(GNEFrameHelper::NeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEFrameHelper::NeteditAttributes::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEFrameHelper::NeteditAttributes::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNEFrameHelper::TagSelector,                  FXGroupBox,         TagSelectorMap,                 ARRAYNUMBER(TagSelectorMap))
FXIMPLEMENT(GNEFrameHelper::DemandElementSelector,        FXGroupBox,         DemandElementSelectorMap,       ARRAYNUMBER(DemandElementSelectorMap))
FXIMPLEMENT(GNEFrameHelper::EdgePathCreator,              FXGroupBox,         EdgePathCreatorMap,             ARRAYNUMBER(EdgePathCreatorMap))
FXIMPLEMENT(GNEFrameHelper::AttributesCreatorRow,         FXHorizontalFrame,  RowCreatorMap,                  ARRAYNUMBER(RowCreatorMap))
FXIMPLEMENT(GNEFrameHelper::AttributesCreator,            FXGroupBox,         AttributesCreatorMap,           ARRAYNUMBER(AttributesCreatorMap))
FXIMPLEMENT(GNEFrameHelper::AttributesEditorRow,          FXHorizontalFrame,  AttributesEditorRowMap,         ARRAYNUMBER(AttributesEditorRowMap))
FXIMPLEMENT(GNEFrameHelper::AttributesEditor,             FXGroupBox,         AttributesEditorMap,            ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEFrameHelper::AttributesEditorExtended,     FXGroupBox,         AttributesEditorExtendedMap,    ARRAYNUMBER(AttributesEditorExtendedMap))
FXIMPLEMENT(GNEFrameHelper::AttributeCarrierHierarchy,    FXGroupBox,         AttributeCarrierHierarchyMap,   ARRAYNUMBER(AttributeCarrierHierarchyMap))
FXIMPLEMENT(GNEFrameHelper::GenericParametersEditor,      FXGroupBox,         GenericParametersEditorMap,     ARRAYNUMBER(GenericParametersEditorMap))
FXIMPLEMENT(GNEFrameHelper::DrawingShape,                 FXGroupBox,         DrawingShapeMap,                ARRAYNUMBER(DrawingShapeMap))
FXIMPLEMENT(GNEFrameHelper::NeteditAttributes,            FXGroupBox,         NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameHelper::TagSelector - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::TagSelector::TagSelector(GNEFrame* frameParent, GNEAttributeCarrier::TagType type, bool onlyDrawables) :
    FXGroupBox(frameParent->myContentFrame, "Element", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // first check that property is valid
    switch (type) {
        case GNEAttributeCarrier::TagType::TAGTYPE_NETELEMENT:
            setText("Net elements");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL:
            setText("Additional elements");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_SHAPE:
            setText("Shape elements");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_TAZ:
            setText("TAZ elements");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_VEHICLE:
            setText("Vehicles");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_STOP:
            setText("Stops");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_PERSON:
            setText("Persons");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_PERSONPLAN:
            setText("Person plans");
            // person plan type has four sub-groups
            myListOfTagTypes.push_back(std::make_pair("person trips", GNEAttributeCarrier::TagType::TAGTYPE_PERSONTRIP));
            myListOfTagTypes.push_back(std::make_pair("walks", GNEAttributeCarrier::TagType::TAGTYPE_WALK));
            myListOfTagTypes.push_back(std::make_pair("rides", GNEAttributeCarrier::TagType::TAGTYPE_RIDE));
            myListOfTagTypes.push_back(std::make_pair("stops", GNEAttributeCarrier::TagType::TAGTYPE_PERSONSTOP));
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_PERSONTRIP:
            setText("Person trips");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_WALK:
            setText("Walks");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_RIDE:
            setText("Rides");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_PERSONSTOP:
            setText("Person stops");
            break;
        default:
            throw ProcessError("invalid tag property");
    }

    // Create FXComboBox
    myTagTypesMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_TAGTYPE_SELECTED, GUIDesignComboBox);
    // Create FXComboBox
    myTagsMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_TAG_SELECTED, GUIDesignComboBox);
    // Fill comboBox depending of TagTypes
    if (myListOfTagTypes.size() > 0) {
        // fill myTypeMatchBox with list of tags
        for (const auto& i : myListOfTagTypes) {
            myTagTypesMatchBox->appendItem(i.first.c_str());
        }
        // Set visible items
        myTagTypesMatchBox->setNumVisible((int)myTagTypesMatchBox->getNumItems());
        // fill myListOfTags with personTrips (the first Tag Type)
        myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_PERSONTRIP, onlyDrawables);
    } else {
        myTagTypesMatchBox->hide();
        // fill myListOfTags
        myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(type, onlyDrawables);

    }
    // fill myTypeMatchBox with list of tags
    for (const auto& i : myListOfTags) {
        myTagsMatchBox->appendItem(toString(i).c_str());
    }
    // Set visible items
    myTagsMatchBox->setNumVisible((int)myTagsMatchBox->getNumItems());
    // TagSelector is always shown
    show();
}


GNEFrameHelper::TagSelector::~TagSelector() {}


void
GNEFrameHelper::TagSelector::showTagSelector() {
    show();
}


void
GNEFrameHelper::TagSelector::hideTagSelector() {
    hide();
}


const GNEAttributeCarrier::TagProperties&
GNEFrameHelper::TagSelector::getCurrentTagProperties() const {
    return myCurrentTagProperties;
}


void 
GNEFrameHelper::TagSelector::setCurrentTagType(GNEAttributeCarrier::TagType tagType) {
    // set empty tag properties
    myCurrentTagProperties = GNEAttributeCarrier::TagProperties();
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTagsMatchBox->getNumItems(); i++) {
        if (myTagsMatchBox->getItem(i).text() == toString(tagType)) {
            myTagsMatchBox->setCurrentItem(i);
            // fill myListOfTags with personTrips (the first Tag Type)
            myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_PERSONTRIP, true);
            // clear myTagsMatchBox
            myTagsMatchBox->clearItems();
            // fill myTypeMatchBox with list of tags
            for (const auto& j : myListOfTags) {
                myTagsMatchBox->appendItem(toString(j).c_str());
            }
            // Set visible items
            myTagsMatchBox->setNumVisible((int)myTagsMatchBox->getNumItems());
        }
    }
    // call tag selected function
    myFrameParent->tagSelected();
}


void
GNEFrameHelper::TagSelector::setCurrentTag(SumoXMLTag newTag) {
    // set empty tag properties
    myCurrentTagProperties = GNEAttributeCarrier::TagProperties();
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTagsMatchBox->getNumItems(); i++) {
        if (myTagsMatchBox->getItem(i).text() == toString(newTag)) {
            myTagsMatchBox->setCurrentItem(i);
            // Set new current type
            myCurrentTagProperties = GNEAttributeCarrier::getTagProperties(newTag);
        }
    }
    // call tag selected function
    myFrameParent->tagSelected();
}


void
GNEFrameHelper::TagSelector::refreshTagProperties() {
    // simply call onCmdSelectItem (to avoid duplicated code)
    onCmdSelectTag(0, 0, 0);
}


long GNEFrameHelper::TagSelector::onCmdSelectTagType(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myListOfTagTypes) {
        if (i.first == myTagTypesMatchBox->getText().text()) {
            // set color of myTagTypesMatchBox to black (valid)
            myTagTypesMatchBox->setTextColor(FXRGB(0, 0, 0));
            // fill myListOfTags with personTrips (the first Tag Type)
            myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(i.second, true);
            // show and clear myTagsMatchBox
            myTagsMatchBox->show();
            myTagsMatchBox->clearItems();
            // fill myTypeMatchBox with list of tags
            for (const auto& j : myListOfTags) {
                myTagsMatchBox->appendItem(toString(j).c_str());
            }
            // Set visible items
            myTagsMatchBox->setNumVisible((int)myTagsMatchBox->getNumItems());
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTagsMatchBox->getText() + "' in TagTypeSelector").text());
            // call onCmdSelectTag
            return onCmdSelectTag(nullptr, 0, nullptr);
        }
    }
    // if TagType isn't valid, hide myTagsMatchBox
    myTagsMatchBox->hide();
    // if additional name isn't correct, set SUMO_TAG_NOTHING as current type
    myCurrentTagProperties = myInvalidTagProperty;
    // call tag selected function
    myFrameParent->tagSelected();
    // set color of myTagTypesMatchBox to red (invalid)
    myTagTypesMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in TagTypeSelector");
    return 1;
}


long
GNEFrameHelper::TagSelector::onCmdSelectTag(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myListOfTags) {
        if (toString(i) == myTagsMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTagsMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentTagProperties = GNEAttributeCarrier::getTagProperties(i);
            // call tag selected function
            myFrameParent->tagSelected();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTagsMatchBox->getText() + "' in TagSelector").text());
            return 1;
        }
    }
    // if additional name isn't correct, set SUMO_TAG_NOTHING as current type
    myCurrentTagProperties = myInvalidTagProperty;
    // call tag selected function
    myFrameParent->tagSelected();
    // set color of myTypeMatchBox to red (invalid)
    myTagsMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in TagSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::DemandElementSelector - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::DemandElementSelector::DemandElementSelector(GNEFrame* frameParent, SumoXMLTag demandElementTag) :
    FXGroupBox(frameParent->myContentFrame, ("Parent " + toString(demandElementTag)).c_str(), GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myCurrentDemandElement(nullptr),
    myDemandElementTags({demandElementTag}) {
    // Create FXComboBox
    myDemandElementsMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh demand element MatchBox
    refreshDemandElementSelector();
    // shown after creation
    show();
}


GNEFrameHelper::DemandElementSelector::DemandElementSelector(GNEFrame* frameParent, const std::vector<GNEAttributeCarrier::TagType> &tagTypes) :
    FXGroupBox(frameParent->myContentFrame, "Parent element", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myCurrentDemandElement(nullptr) {
    // fill myDemandElementTags
    for (const auto &i : tagTypes) {
        auto tags = GNEAttributeCarrier::allowedTagsByCategory(i, false);
        myDemandElementTags.insert(myDemandElementTags.end(), tags.begin(), tags.end());
    }
    // Create FXComboBox
    myDemandElementsMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh demand element MatchBox
    refreshDemandElementSelector();
    // shown after creation
    show();
}


GNEFrameHelper::DemandElementSelector::~DemandElementSelector() {}


GNEDemandElement*
GNEFrameHelper::DemandElementSelector::getCurrentDemandElement() const {
    return myCurrentDemandElement;
}


void 
GNEFrameHelper::DemandElementSelector::setDemandElement(GNEDemandElement* demandElement) {
    // first check that demandElement tag correspond to a tag of myDemandElementTags
    if (std::find(myDemandElementTags.begin(), myDemandElementTags.end(), demandElement->getTagProperty().getTag()) != myDemandElementTags.end()) {
        // update text of myDemandElementsMatchBox
        myDemandElementsMatchBox->setText(demandElement->getID().c_str());
        // Set new current demand element
        myCurrentDemandElement = demandElement;
        // call demandElementSelected function
        myFrameParent->demandElementSelected();
    }
}


void
GNEFrameHelper::DemandElementSelector::showDemandElementSelector() {
    // first refresh modul
    refreshDemandElementSelector();
    // if current selected item isn't valid, set DEFAULT_VTYPE_ID or DEFAULT_PEDTYPE_ID
    if (myCurrentDemandElement) {
        myDemandElementsMatchBox->setText(myCurrentDemandElement->getID().c_str());
    } else if (myDemandElementTags.size() == 1) {
        if (myDemandElementTags.at(0) == SUMO_TAG_VTYPE) {
            myDemandElementsMatchBox->setText(DEFAULT_VTYPE_ID.c_str());
        } else if (myDemandElementTags.at(0) == SUMO_TAG_PTYPE) {
            myDemandElementsMatchBox->setText(DEFAULT_PEDTYPE_ID.c_str());
        }
    }
    onCmdSelectDemandElement(nullptr, 0, nullptr);
    show();
}


void
GNEFrameHelper::DemandElementSelector::hideDemandElementSelector() {
    hide();
}


bool 
GNEFrameHelper::DemandElementSelector::isDemandElementSelectorShown() const {
    return shown();
}


void
GNEFrameHelper::DemandElementSelector::refreshDemandElementSelector() {
    // clear demand elements comboBox
    myDemandElementsMatchBox->clearItems();
    // fill myTypeMatchBox with list of demand elements
    for (const auto& i : myDemandElementTags) {
        // special case for VTypes and PTypes
        if (i == SUMO_TAG_VTYPE) {
            // add default Vehicle an Bike types in the first and second positions
            myDemandElementsMatchBox->appendItem(DEFAULT_VTYPE_ID.c_str());
            myDemandElementsMatchBox->appendItem(DEFAULT_BIKETYPE_ID.c_str());
            // add rest of vTypes
            for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(i)) {
                // avoid insert duplicated default vType
                if ((j.first != DEFAULT_VTYPE_ID) && (j.first != DEFAULT_BIKETYPE_ID)) {
                    myDemandElementsMatchBox->appendItem(j.first.c_str());
                }
            }
        } else if (i == SUMO_TAG_PTYPE) {
            // add default Person type in the firs
            myDemandElementsMatchBox->appendItem(DEFAULT_PEDTYPE_ID.c_str());
            // add rest of pTypes
            for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(i)) {
                // avoid insert duplicated default pType
                if (j.first != DEFAULT_PEDTYPE_ID) {
                    myDemandElementsMatchBox->appendItem(j.first.c_str());
                }
            }
        } else {
            // insert all Ids
            for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(i)) {
                myDemandElementsMatchBox->appendItem(j.first.c_str());
            }
        }
    }
    // Set number of  items (maximum 10)
    if (myDemandElementsMatchBox->getNumItems() < 10) {
        myDemandElementsMatchBox->setNumVisible((int)myDemandElementsMatchBox->getNumItems());
    } else {
        myDemandElementsMatchBox->setNumVisible(10);
    }
    // update myCurrentDemandElement
    if (myDemandElementsMatchBox->getNumItems() == 0) {
        myCurrentDemandElement = nullptr;
    } else if (myCurrentDemandElement) {
        for (int i = 0; i < myDemandElementsMatchBox->getNumItems(); i++) {
            if (myDemandElementsMatchBox->getItem(i).text() == myCurrentDemandElement->getID()) {
                myDemandElementsMatchBox->setCurrentItem(i, FALSE);
            }
        }
    } else {
        // set first element in the list as myCurrentDemandElement (Special case for default person and vehicle type)
        if (myDemandElementsMatchBox->getItem(0).text() == DEFAULT_VTYPE_ID) {
            myCurrentDemandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VTYPE).at(DEFAULT_VTYPE_ID);
        } else if (myDemandElementsMatchBox->getItem(0).text() == DEFAULT_PEDTYPE_ID) {
            myCurrentDemandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PTYPE).at(DEFAULT_PEDTYPE_ID);
        } else {
            myCurrentDemandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(myDemandElementTags.front()).begin()->second;
        }
    }
}


long
GNEFrameHelper::DemandElementSelector::onCmdSelectDemandElement(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond to a demand element
    for (const auto& i : myDemandElementTags) {
        for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(i)) {
            if (j.first == myDemandElementsMatchBox->getText().text()) {
                // set color of myTypeMatchBox to black (valid)
                myDemandElementsMatchBox->setTextColor(FXRGB(0, 0, 0));
                // Set new current demand element
                myCurrentDemandElement = j.second;
                // call demandElementSelected function
                myFrameParent->demandElementSelected();
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(("Selected item '" + myDemandElementsMatchBox->getText() + "' in DemandElementSelector").text());
                return 1;
            }
        }
    }
    // if demand element selected is invalid, set demand element as null
    myCurrentDemandElement = nullptr;
    // call demandElementSelected function
    myFrameParent->demandElementSelected();
    // change color of myDemandElementsMatchBox to red (invalid)
    myDemandElementsMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in DemandElementSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::EdgePathCreator - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::EdgePathCreator::EdgePathCreator(GNEFrame* frameParent, int edgePathCreatorModes) :
    FXGroupBox(frameParent->myContentFrame, "Route creator", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myVClass(SVC_PASSENGER),
    mySelectedBusStop(nullptr),
    myEdgePathCreatorModes(edgePathCreatorModes) {

    // create button for create GEO POIs
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_EDGEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();

    // create button for create GEO POIs
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_EDGEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();

    // create button for create GEO POIs
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_EDGEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
}


GNEFrameHelper::EdgePathCreator::~EdgePathCreator() {}


void 
GNEFrameHelper::EdgePathCreator::edgePathCreatorName(const std::string &name) {
    // header needs the first capitalized letter
    std::string nameWithFirstCapitalizedLetter = name;
    nameWithFirstCapitalizedLetter[0] = (char)toupper(nameWithFirstCapitalizedLetter.at(0));
    setText((nameWithFirstCapitalizedLetter + " creator").c_str());
    myFinishCreationButton->setText(("Finish " + name + " creation").c_str());
    myAbortCreationButton->setText(("Abort " + name + " creation").c_str());
}


void
GNEFrameHelper::EdgePathCreator::showEdgePathCreator() {
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    show();
}


void
GNEFrameHelper::EdgePathCreator::hideEdgePathCreator() {
    hide();
}


void 
GNEFrameHelper::EdgePathCreator::setVClass(SUMOVehicleClass vClass) {
    myVClass = vClass;
}


void
GNEFrameHelper::EdgePathCreator::setEdgePathCreatorModes(int edgePathCreatorModes) {
    myEdgePathCreatorModes = edgePathCreatorModes;
}


std::vector<GNEEdge*>
GNEFrameHelper::EdgePathCreator::getClickedEdges() const {
    return myClickedEdges;
}


GNEAdditional*
GNEFrameHelper::EdgePathCreator::getClickedBusStop() const {
    return mySelectedBusStop;
}


bool
GNEFrameHelper::EdgePathCreator::addEdge(GNEEdge* edge) {
    if ((mySelectedBusStop == nullptr) && (myClickedEdges.empty() || ((myClickedEdges.size() > 0) && (myClickedEdges.back() != edge)))) {
        myClickedEdges.push_back(edge);
        // enable abort route button
        myAbortCreationButton->enable();
        // disable undo/redo
        myFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("trip creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myFrameParent->getEdgeCandidateSelectedColor());
        }
        // calculate route if there is more than two edges
        if (myClickedEdges.size() > 1) {
            // enable remove last edge button
            myRemoveLastInsertedEdge->enable();
            // enable finish button
            myFinishCreationButton->enable();
            // calculate temporal route
            myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(myVClass, myClickedEdges);
        }
        return true;
    } else {
        return false;
    }
}


bool 
GNEFrameHelper::EdgePathCreator::addBusStop(GNEAdditional* busStop) {
    // check that at least there is a selected edge
    if (!myClickedEdges.empty() && (mySelectedBusStop == nullptr)) {
        mySelectedBusStop = busStop;
        mySelectedBusStop->setSpecialColor(&myFrameParent->getEdgeCandidateSelectedColor());
    }
    return false;
}


void
GNEFrameHelper::EdgePathCreator::clearEdges() {
    // restore colors
    for (const auto& i : myClickedEdges) {
        for (const auto& j : i->getLanes()) {
            j->setSpecialColor(nullptr);
        }
    }
    // clear edges
    myClickedEdges.clear();
    myTemporalRoute.clear();
    // clear busStop
    if (mySelectedBusStop) {
        mySelectedBusStop->setSpecialColor(nullptr);
        mySelectedBusStop = nullptr;
    }
    // enable undo/redo
    myFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
}


void
GNEFrameHelper::EdgePathCreator::drawTemporalRoute() const {
    // draw depending of number of edges
    if (myClickedEdges.size() == 1) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // draw line in first selected edge edge
        GLHelper::drawLine(myClickedEdges.front()->getNBEdge()->getLanes().front().shape.front(),
                           myClickedEdges.front()->getNBEdge()->getLanes().front().shape.back());
        // draw line to center of selected bus
        if (mySelectedBusStop) {
            GLHelper::drawLine(myClickedEdges.front()->getNBEdge()->getLanes().front().shape.back(), 
                               mySelectedBusStop->getAdditionalGeometry().shape.getLineCenter());
        }
        // Pop last matrix
        glPopMatrix();
    } else if (myTemporalRoute.size() > 1) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // draw first line
        GLHelper::drawLine(myTemporalRoute.at(0)->getNBEdge()->getLanes().front().shape.front(),
                           myTemporalRoute.at(0)->getNBEdge()->getLanes().front().shape.back());
        // draw rest of lines
        for (int i = 1; i < (int)myTemporalRoute.size(); i++) {
            GLHelper::drawLine(myTemporalRoute.at(i - 1)->getNBEdge()->getLanes().front().shape.back(),
                               myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front());
            GLHelper::drawLine(myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front(),
                               myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.back());
        }    
        // draw a line to center of selected bus
        if (mySelectedBusStop) {
            GLHelper::drawLine(myTemporalRoute.back()->getNBEdge()->getLanes().front().shape.back(), 
                               mySelectedBusStop->getAdditionalGeometry().shape.getLineCenter());
        }
        // Pop last matrix
        glPopMatrix();
    }
}


void 
GNEFrameHelper::EdgePathCreator::abortEdgePathCreation() {
    if (myAbortCreationButton->isEnabled()) {
        onCmdAbortRouteCreation(nullptr, 0, nullptr);
    }
}


void 
GNEFrameHelper::EdgePathCreator::finishEdgePathCreation() {
    if (myFinishCreationButton->isEnabled()) {
        onCmdFinishRouteCreation(nullptr, 0, nullptr);
    }
}


void 
GNEFrameHelper::EdgePathCreator::removeLastInsertedElement() {
    if (myRemoveLastInsertedEdge->isEnabled()) {
        onCmdRemoveLastInsertedElement(nullptr, 0, nullptr);
    }
}


long
GNEFrameHelper::EdgePathCreator::onCmdAbortRouteCreation(FXObject*, FXSelector, void*) {
    clearEdges();
    // disable buttons
    myAbortCreationButton->disable();
    myFinishCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    return 1;
}


long
GNEFrameHelper::EdgePathCreator::onCmdFinishRouteCreation(FXObject*, FXSelector, void*) {
    // only create route if there is more than two edges
    if (myClickedEdges.size() > 1) {
        // call edgePathCreated
        myFrameParent->edgePathCreated();
        // update view
        myFrameParent->myViewNet->update();
        // clear edges after creation
        clearEdges();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedEdge->disable();
    }
    return 1;
}


long
GNEFrameHelper::EdgePathCreator::onCmdRemoveLastInsertedElement(FXObject*, FXSelector, void*) {
    if (myClickedEdges.size() > 1) {
        // remove last edge
        myClickedEdges.pop_back();
        // calculate temporal route
        myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(myVClass, myClickedEdges);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::AttributesCreatorRow - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::AttributesCreatorRow::AttributesCreatorRow(AttributesCreator* AttributesCreatorParent, const GNEAttributeCarrier::AttributeProperties& attrProperties) :
    FXHorizontalFrame(AttributesCreatorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesCreatorParent(AttributesCreatorParent),
    myAttrProperties(attrProperties) {
    // Create left visual elements
    myAttributeLabel = new FXLabel(this, "name", nullptr, GUIDesignLabelAttribute);
    myAttributeLabel->hide();
    myAttributeRadioButton = new FXRadioButton(this, "name", this, MID_GNE_SET_ATTRIBUTE_RADIOBUTTON, GUIDesignRadioButtonAttribute);
    myAttributeRadioButton->hide();
    myAttributeCheckButton = new FXCheckButton(this, "name", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    myAttributeCheckButton->hide();
    myAttributeColorButton = new FXButton(this, "ColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeColorButton->hide();
    // Create right visual elements
    myValueTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldInt);
    myValueTextFieldInt->hide();
    myValueTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myValueTextFieldReal->hide();
    myValueTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextFieldStrings->hide();
    myValueCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    // by default attribute check button is true
    myAttributeCheckButton->setCheck(true);
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesCreatorRow
        FXHorizontalFrame::create();
        // reset invalid value 
        myInvalidValue = "";
        // show label, button for edit colors or radio button
        if (myAttrProperties.isColor()) {
            myAttributeColorButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeColorButton->setText(myAttrProperties.getAttrStr().c_str());
            myAttributeColorButton->show();
        } else if (myAttrProperties.isEnablitable()) {
            myAttributeRadioButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeRadioButton->setText(myAttrProperties.getAttrStr().c_str());
            myAttributeRadioButton->show();
        } else if (myAttrProperties.isOptional()) {
            myAttributeCheckButton->setText(myAttrProperties.getAttrStr().c_str());
            myAttributeCheckButton->show();
        } else {
            myAttributeLabel->setText(myAttrProperties.getAttrStr().c_str());
            myAttributeLabel->show();
        }
        if (myAttrProperties.isInt()) {
            myValueTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldInt->setText(attrProperties.getDefaultValue().c_str());
            myValueTextFieldInt->show();
            // if it's associated to a radio button and is disabled, then disabled myValueTextFieldInt
            if (myAttributeRadioButton->shown() && (myAttributeRadioButton->getCheck() == FALSE)) {
                myValueTextFieldInt->disable();
            }
            // if it's associated to a label button and is disabled, then disabled myValueTextFieldInt
            if (myAttributeCheckButton->shown() && (myAttributeCheckButton->getCheck() == FALSE)) {
                myValueTextFieldInt->disable();
            }
        } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
            myValueTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldReal->setText(attrProperties.getDefaultValue().c_str());
            myValueTextFieldReal->show();
            // if it's associated to a radio button and is disabled, then disable myValueTextFieldReal
            if (myAttributeRadioButton->shown() && (myAttributeRadioButton->getCheck() == FALSE)) {
                myValueTextFieldReal->disable();
            }
            // if it's associated to a label button and is disabled, then disable myValueTextFieldReal
            if (myAttributeCheckButton->shown() && (myAttributeCheckButton->getCheck() == FALSE)) {
                myValueTextFieldReal->disable();
            }
        } else if (myAttrProperties.isBool()) {
            if (GNEAttributeCarrier::parse<bool>(attrProperties.getDefaultValue())) {
                myValueCheckButton->setCheck(true);
                myValueCheckButton->setText("true");
            } else {
                myValueCheckButton->setCheck(false);
                myValueCheckButton->setText("false");
            }
            myValueCheckButton->show();
            // if it's associated to a radio button and is disabled, then disable myValueCheckButton
            if (myAttributeRadioButton->shown() && (myAttributeRadioButton->getCheck() == FALSE)) {
                myValueCheckButton->disable();
            }
            // if it's associated to a label button and is disabled, then disable myValueCheckButton
            if (myAttributeCheckButton->shown() && (myAttributeCheckButton->getCheck() == FALSE)) {
                myValueCheckButton->disable();
            }
        } else {
            myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldStrings->setText(attrProperties.getDefaultValue().c_str());
            myValueTextFieldStrings->show();
            // if it's associated to a radio button and is disabled, then disable myValueTextFieldStrings
            if (myAttributeRadioButton->shown() && (myAttributeRadioButton->getCheck() == FALSE)) {
                myValueTextFieldStrings->disable();
            }
            // if it's associated to a label button and is disabled, then disable myValueTextFieldStrings
            if (myAttributeCheckButton->shown() && (myAttributeCheckButton->getCheck() == FALSE)) {
                myValueTextFieldStrings->disable();
            }
        }
        // show AttributesCreatorRow
        show();
    }
}


void 
GNEFrameHelper::AttributesCreatorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


const GNEAttributeCarrier::AttributeProperties&
GNEFrameHelper::AttributesCreatorRow::getAttrProperties() const {
    return myAttrProperties;
}


std::string
GNEFrameHelper::AttributesCreatorRow::getValue() const {
    if (myAttrProperties.isBool()) {
        return (myValueCheckButton->getCheck() == 1) ? "1" : "0";
    } else if (myAttrProperties.isInt()) {
        return myValueTextFieldInt->getText().text();
    } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
        return myValueTextFieldReal->getText().text();
    } else {
        return myValueTextFieldStrings->getText().text();
    }
}


bool
GNEFrameHelper::AttributesCreatorRow::getAttributeRadioButtonCheck() const {
    if (shown()) {
        return myAttributeRadioButton->getCheck() == TRUE;
    } else {
        return false;
    }
}


void
GNEFrameHelper::AttributesCreatorRow::setAttributeRadioButtonCheck(bool value) {
    if (shown()) {
        // set radio button
        myAttributeRadioButton->setCheck(value);
        // enable or disable input fields
        if (value) {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->enable();
            } else if (myAttrProperties.isInt()) {
                myValueTextFieldInt->enable();
            } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
                myValueTextFieldReal->enable();
            } else {
                myValueTextFieldStrings->enable();
            }
        } else {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->disable();
            } else if (myAttrProperties.isInt()) {
                myValueTextFieldInt->disable();
            } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
                myValueTextFieldReal->disable();
            } else {
                myValueTextFieldStrings->disable();
            }
        }
    }
}


bool
GNEFrameHelper::AttributesCreatorRow::getAttributeCheckButtonCheck() const {
    if (shown()) {
        return myAttributeCheckButton->getCheck() == TRUE;
    } else {
        return false;
    }
}


void
GNEFrameHelper::AttributesCreatorRow::setAttributeCheckButtonCheck(bool value) {
    if (shown()) {
        // set radio button
        myAttributeCheckButton->setCheck(value);
        // enable or disable input fields
        if (value) {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->enable();
            } else if (myAttrProperties.isInt()) {
                myValueTextFieldInt->enable();
            } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
                myValueTextFieldReal->enable();
            } else {
                myValueTextFieldStrings->enable();
            }
        } else {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->disable();
            } else if (myAttrProperties.isInt()) {
                myValueTextFieldInt->disable();
            } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
                myValueTextFieldReal->disable();
            } else {
                myValueTextFieldStrings->disable();
            }
        }
    }
}


void
GNEFrameHelper::AttributesCreatorRow::enableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->enable();
    } else if (myAttrProperties.isInt()) {
        return myValueTextFieldInt->enable();
    } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
        return myValueTextFieldReal->enable();
    } else {
        return myValueTextFieldStrings->enable();
    }
}


void
GNEFrameHelper::AttributesCreatorRow::disableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->disable();
    } else if (myAttrProperties.isInt()) {
        return myValueTextFieldInt->disable();
    } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
        return myValueTextFieldReal->disable();
    } else {
        return myValueTextFieldStrings->disable();
    }
}


bool
GNEFrameHelper::AttributesCreatorRow::isAttributesCreatorRowEnabled() const {
    if (!shown()) {
        return false;
    } else if (myAttrProperties.isBool()) {
        return myValueCheckButton->isEnabled();
    } else if (myAttrProperties.isInt()) {
        return myValueTextFieldInt->isEnabled();
    } else if (myAttrProperties.isFloat() || myAttrProperties.isSUMOTime()) {
        return myValueTextFieldReal->isEnabled();
    } else {
        return myValueTextFieldStrings->isEnabled();
    }
}


const std::string&
GNEFrameHelper::AttributesCreatorRow::isAttributeValid() const {
    return myInvalidValue;
}


GNEFrameHelper::AttributesCreator*
GNEFrameHelper::AttributesCreatorRow::getAttributesCreatorParent() const {
    return myAttributesCreatorParent;
}


long
GNEFrameHelper::AttributesCreatorRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // Check if format of current value of myTextField is correct
    if (obj == myValueCheckButton) {
        if (myValueCheckButton->getCheck()) {
            myValueCheckButton->setText("true");
        } else {
            myValueCheckButton->setText("false");
        }
        // update disjoint attribute
        myAttributesCreatorParent->updateDisjointAttributes(nullptr);
    } else if (myAttrProperties.isComplex()) {
        // check complex attribute
        myInvalidValue = checkComplexAttribute(myValueTextFieldStrings->getText().text());
    } else if (myAttrProperties.isInt()) {
        if (GNEAttributeCarrier::canParse<int>(myValueTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myValueTextFieldInt->getText().text());
            // Check if int value must be positive
            if (myAttrProperties.isPositive() && (intValue < 0)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'int' format";
        }
    } else if (myAttrProperties.isSUMOTime()) {
        // time attributes work as positive doubles
        if (!GNEAttributeCarrier::canParse<SUMOTime>(myValueTextFieldReal->getText().text())) {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid SUMOTime format";
        }
    } else if (myAttrProperties.isFloat()) {
        if (GNEAttributeCarrier::canParse<double>(myValueTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myValueTextFieldReal->getText().text());
            // Check if double value must be positive
            if (myAttrProperties.isPositive() && (doubleValue < 0)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' cannot be negative";
                // check if double value is a probability
            } else if (myAttrProperties.isProbability() && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' takes only values between 0 and 1";
            } else if (myAttrProperties.hasAttrRange() && ((doubleValue < myAttrProperties.getMinimumRange()) || doubleValue > myAttrProperties.getMaximumRange())) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' takes only values between " + toString(myAttrProperties.getMinimumRange()) + " and " + toString(myAttrProperties.getMaximumRange());
            } else if ((myAttributesCreatorParent->getCurrentTagProperties().getTag() == SUMO_TAG_E2DETECTOR) && (myAttrProperties.getAttr() == SUMO_ATTR_LENGTH) && (doubleValue == 0)) {
                myInvalidValue = "E2 length cannot be 0";
            }
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'float' format";
        }
    } else if (myAttrProperties.isColor()) {
        // check if filename format is valid
        if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'RBGColor' format";
        }
    } else if (myAttrProperties.isFilename()) {
        std::string file = myValueTextFieldStrings->getText().text();
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidFilename(file) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        } else if (myAttrProperties.getAttr() == SUMO_ATTR_IMGFILE) {
            if (!file.empty()) {
                // only load value if file exist and can be loaded
                if (GUITexturesHelper::getTextureID(file) == -1) {
                    myInvalidValue = "doesn't exist image '" + file + "'";
                }
            }
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_NAME) {
        std::string name = myValueTextFieldStrings->getText().text();
        // check if name format is valid
        if (SUMOXMLDefinitions::isValidAttribute(name) == false) {
            myInvalidValue = "input contains invalid characters";
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_VTYPES) {
        std::string name = myValueTextFieldStrings->getText().text();
        // if list of VTypes isn't empty, check that all characters are valid
        if (!name.empty() && !SUMOXMLDefinitions::isValidListOfTypeID(name)) {
            myInvalidValue = "list of IDs contains invalid characters";
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_INDEX) {
        // special case for stop indx
        std::string index = myValueTextFieldStrings->getText().text();
        if ((index != "fit") && (index != "end") && !GNEAttributeCarrier::canParse<int>(index)) {
            myInvalidValue = "index isn't either 'fit' or 'end' or a valid positive int";
        } else if (GNEAttributeCarrier::parse<int>(index) < 0) {
            myInvalidValue = "index cannot be negative";
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myInvalidValue.size() == 0) {
        myValueTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myValueTextFieldInt->killFocus();
        myValueTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myValueTextFieldReal->killFocus();
        myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myValueTextFieldStrings->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myValueTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        myValueTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        myValueTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEFrameHelper::AttributesCreatorRow::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
    if (myAttributeCheckButton->getCheck()) {
        // enable input values
        myValueCheckButton->enable();
        myValueTextFieldInt->enable();
        myValueTextFieldReal->enable();
        myValueTextFieldStrings->enable();
    } else {
        // disable input values
        myValueCheckButton->disable();
        myValueTextFieldInt->disable();
        myValueTextFieldReal->disable();
        myValueTextFieldStrings->disable();
    }
    return 0;
}


long
GNEFrameHelper::AttributesCreatorRow::onCmdSelectColorButton(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextFieldStrings->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myValueTextFieldStrings->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myAttrProperties.getDefaultValue())));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        myValueTextFieldStrings->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
        onCmdSetAttribute(nullptr, 0, nullptr);
    }
    return 0;
}

long
GNEFrameHelper::AttributesCreatorRow::onCmdSelectRadioButton(FXObject*, FXSelector, void*) {
    // write debug (for Netedit tests)
    WRITE_DEBUG("Selected radio button for attribute '" + myAttrProperties.getAttrStr() + "'");
    // update disjoint attributes in AC Attributes parent
    myAttributesCreatorParent->updateDisjointAttributes(this);
    return 0;
}


std::string 
GNEFrameHelper::AttributesCreatorRow::checkComplexAttribute(const std::string &value) {
    // declare values needed to check if given complex parameters are valid
    std::string errorMessage;
    DepartDefinition dd;
    DepartLaneDefinition dld;
    DepartPosDefinition dpd;
    DepartPosLatDefinition dpld;
    ArrivalLaneDefinition ald;
    DepartSpeedDefinition dsd;
    ArrivalPosDefinition apd;
    ArrivalPosLatDefinition apld;
    ArrivalSpeedDefinition asd;
    SVCPermissions mode;
    int valueInt;
    double valueDouble;
    SUMOTime valueSUMOTime;
    // check complex attribute
    switch (myAttrProperties.getAttr()) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
            SUMOVehicleParameter::parseDepart(value, myAttrProperties.getAttrStr(), "", valueSUMOTime, dd, errorMessage);
            break;
        case SUMO_ATTR_DEPARTLANE: 
            SUMOVehicleParameter::parseDepartLane(value, myAttrProperties.getAttrStr(), "", valueInt, dld, errorMessage);
            break;
        case SUMO_ATTR_DEPARTPOS: 
            SUMOVehicleParameter::parseDepartPos(value, myAttrProperties.getAttrStr(), "", valueDouble, dpd, errorMessage);
            break;
        case SUMO_ATTR_DEPARTSPEED: 
            SUMOVehicleParameter::parseDepartSpeed(value, myAttrProperties.getAttrStr(), "", valueDouble, dsd, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALLANE: 
            SUMOVehicleParameter::parseArrivalLane(value, myAttrProperties.getAttrStr(), "", valueInt, ald, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALPOS: 
            SUMOVehicleParameter::parseArrivalPos(value, myAttrProperties.getAttrStr(), "", valueDouble, apd, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALSPEED: 
            SUMOVehicleParameter::parseArrivalSpeed(value, myAttrProperties.getAttrStr(), "", valueDouble, asd, errorMessage);
            break;
        case SUMO_ATTR_DEPARTPOS_LAT: 
            SUMOVehicleParameter::parseDepartPosLat(value, myAttrProperties.getAttrStr(), "", valueDouble, dpld, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT: 
            SUMOVehicleParameter::parseArrivalPosLat(value, myAttrProperties.getAttrStr(), "", valueDouble, apld, errorMessage);
            break;
        case SUMO_ATTR_MODES:
            SUMOVehicleParameter::parsePersonModes(value, myAttrProperties.getAttrStr(), "", mode , errorMessage);
            break;
        default:
            throw ProcessError("Invalid complex attribute");
    }
    // return error message (Will be empty if value is valid)
    return errorMessage;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::AttributesCreator - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::AttributesCreator::AttributesCreator(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // resize myAttributesCreatorRows
    myAttributesCreatorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrameHelper::AttributesCreator::~AttributesCreator() {}


void
GNEFrameHelper::AttributesCreator::showAttributesCreatorModul(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // get current tag Properties
    myTagProperties = tagProperties;
    // destroy all rows
    for (int i = 0; i < (int)myAttributesCreatorRows.size(); i++) {
        // destroy and delete all rows
        if (myAttributesCreatorRows.at(i) != nullptr) {
            myAttributesCreatorRows.at(i)->destroy();
            delete myAttributesCreatorRows.at(i);
            myAttributesCreatorRows.at(i) = nullptr;
        }
    }
    // iterate over tag attributes and create a AttributesCreatorRow
    for (const auto &i : myTagProperties) {
        //  make sure that only non-unique attributes are created (And depending of includeExtendedAttributes)
        if (!i.isUnique()) {
            myAttributesCreatorRows.at(i.getPositionListed()) = new AttributesCreatorRow(this, i);
        }
    }
    // update disjoint attributes
    updateDisjointAttributes(nullptr);
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
    // recalc
    recalc();
    // show
    show();
}


void
GNEFrameHelper::AttributesCreator::hideAttributesCreatorModul() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEFrameHelper::AttributesCreator::getAttributesAndValues(bool includeAll) const {
    std::map<SumoXMLAttr, std::string> values;
    // get standard parameters
    for (int i = 0; i < (int)myAttributesCreatorRows.size(); i++) {
        if (myAttributesCreatorRows.at(i) && myAttributesCreatorRows.at(i)->getAttrProperties().getAttr() != SUMO_ATTR_NOTHING) {
            // ignore default values (except for disjont attributes, that has to be always writted)
            if (myAttributesCreatorRows.at(i)->isAttributesCreatorRowEnabled() &&
                    (includeAll || !myAttributesCreatorRows.at(i)->getAttrProperties().hasStaticDefaultValue() || (myAttributesCreatorRows.at(i)->getAttrProperties().getDefaultValue() != myAttributesCreatorRows.at(i)->getValue()))) {
                values[myAttributesCreatorRows.at(i)->getAttrProperties().getAttr()] = myAttributesCreatorRows.at(i)->getValue();
            }
        }
    }
    return values;
}


GNEAttributeCarrier::TagProperties 
GNEFrameHelper::AttributesCreator::getCurrentTagProperties() const {
    return myTagProperties;
}


void
GNEFrameHelper::AttributesCreator::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standar parameters
    for (const auto &i : myTagProperties) {
        if (errorMessage.empty() && myAttributesCreatorRows.at(i.getPositionListed())) {
            // Return string with the error if at least one of the parameter isn't valid
            std::string attributeValue = myAttributesCreatorRows.at(i.getPositionListed())->isAttributeValid();
            if (attributeValue.size() != 0) {
                errorMessage = attributeValue;
            }
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + myTagProperties.getTagStr() + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + myTagProperties.getTagStr() + ": " + extra;
    }

    // set message in status bar
    myFrameParent->myViewNet->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEFrameHelper::AttributesCreator::areValuesValid() const {
    // iterate over standar parameters
    for (auto i : myTagProperties) {
        // Return false if error message of attriuve isn't empty
        if (myAttributesCreatorRows.at(i.getPositionListed()) && myAttributesCreatorRows.at(i.getPositionListed())->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


void
GNEFrameHelper::AttributesCreator::updateDisjointAttributes(AttributesCreatorRow* row) {
    // currently only Flows supports disjoint attributes
    if ((myTagProperties.getTag() == SUMO_TAG_ROUTEFLOW) || (myTagProperties.getTag() == SUMO_TAG_FLOW)) {
        // obtain all rows (to improve code legibility)
        AttributesCreatorRow* endRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_END).getPositionListed()];
        AttributesCreatorRow* numberRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_NUMBER).getPositionListed()];
        AttributesCreatorRow* vehsperhourRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_VEHSPERHOUR).getPositionListed()];
        AttributesCreatorRow* periodRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_PERIOD).getPositionListed()];
        AttributesCreatorRow* probabilityRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_PROB).getPositionListed()];
        if (row == nullptr) {
            // by default routeFlows uses end and number
            endRow->setAttributeRadioButtonCheck(true);
            numberRow->setAttributeRadioButtonCheck(true);
            vehsperhourRow->setAttributeRadioButtonCheck(false);
            periodRow->setAttributeRadioButtonCheck(false);
            probabilityRow->setAttributeRadioButtonCheck(false);
        } else {
            // check what row was clicked
            switch (row->getAttrProperties().getAttr()) {
                // end has more priority as number
                case SUMO_ATTR_END:
                    endRow->setAttributeRadioButtonCheck(true);
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(false);
                    periodRow->setAttributeRadioButtonCheck(false);
                    probabilityRow->setAttributeRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_NUMBER:
                    numberRow->setAttributeRadioButtonCheck(true);
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck()) {
                        endRow->setAttributeRadioButtonCheck(false);
                    } else {
                        // disable other combinations
                        vehsperhourRow->setAttributeRadioButtonCheck(false);
                        periodRow->setAttributeRadioButtonCheck(false);
                        probabilityRow->setAttributeRadioButtonCheck(false);
                    }
                    break;
                case SUMO_ATTR_VEHSPERHOUR:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck() && numberRow->getAttributeRadioButtonCheck()) {
                        numberRow->setAttributeRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(true);
                    periodRow->setAttributeRadioButtonCheck(false);
                    probabilityRow->setAttributeRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_PERIOD:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck() && numberRow->getAttributeRadioButtonCheck()) {
                        numberRow->setAttributeRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(false);
                    periodRow->setAttributeRadioButtonCheck(true);
                    probabilityRow->setAttributeRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_PROB:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck() && numberRow->getAttributeRadioButtonCheck()) {
                        numberRow->setAttributeRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(false);
                    periodRow->setAttributeRadioButtonCheck(false);
                    probabilityRow->setAttributeRadioButtonCheck(true);
                    break;
                default:
                    break;
            }
        }
    } else if (myTagProperties.isStop()) {
        // check if expected has to be enabled or disabled
        if (myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_TRIGGERED).getPositionListed()]->getValue() == "1") {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED).getPositionListed()]->enableAttributesCreatorRow();
        } else {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED).getPositionListed()]->disableAttributesCreatorRow();
        }
        // check if expected contaienrs has to be enabled or disabled
        if (myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_CONTAINER_TRIGGERED).getPositionListed()]->getValue() == "1") {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED_CONTAINERS).getPositionListed()]->enableAttributesCreatorRow();
        } else {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED_CONTAINERS).getPositionListed()]->disableAttributesCreatorRow();
        }
    }
}


long
GNEFrameHelper::AttributesCreator::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myFrameParent->openHelpAttributesDialog(myTagProperties);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::AttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::AttributesEditorRow::AttributesEditorRow(GNEFrameHelper::AttributesEditor* attributeEditorParent, const GNEAttributeCarrier::AttributeProperties& ACAttr, const std::string& value, bool attributeEnabled) :
    FXHorizontalFrame(attributeEditorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr),
    myMultiple(GNEAttributeCarrier::parse<std::vector<std::string>>(value).size() > 1) {
    // Create and hide label
    myAttributeLabel = new FXLabel(this, "attributeLabel", nullptr, GUIDesignLabelAttribute);
    myAttributeLabel->hide();
    // Create and hide radio button
    myAttributeRadioButton = new FXRadioButton(this, "attributeRadioButton", this, MID_GNE_SET_ATTRIBUTE_RADIOBUTTON, GUIDesignRadioButtonAttribute);
    myAttributeRadioButton->hide();
    // Create and hide check button
    myAttributeCheckButton = new FXCheckButton(this, "attributeCheckButton", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    myAttributeCheckButton->hide();
    // Create and hide ButtonCombinableChoices
    myAttributeButtonCombinableChoices = new FXButton(this, "attributeButtonCombinableChoices", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeButtonCombinableChoices->hide();
    // create and hidde color editor
    myAttributeColorButton = new FXButton(this, "attributeColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeColorButton->hide();
    // Create and hide textField for int attributes
    myValueTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldInt);
    myValueTextFieldInt->hide();
    // Create and hide textField for real/time attributes
    myValueTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myValueTextFieldReal->hide();
    // Create and hide textField for string attributes
    myValueTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextFieldStrings->hide();
    // Create and hide ComboBox
    myValueComboBoxChoices = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myValueComboBoxChoices->hide();
    // Create and hide checkButton
    myValueCheckButton = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesEditorRow
        FXHorizontalFrame::create();
        // start enabling all elements, depending if attribute is editable and enabled
        if (myACAttr.isNonEditable() || !attributeEnabled) {
            myValueTextFieldInt->disable();
            myValueTextFieldReal->disable();
            myValueTextFieldStrings->disable();
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
            myAttributeButtonCombinableChoices->disable();
            myAttributeColorButton->disable();
            myAttributeCheckButton->disable();
        } else {
            myValueTextFieldInt->enable();
            myValueTextFieldReal->enable();
            myValueTextFieldStrings->enable();
            myValueComboBoxChoices->enable();
            myValueCheckButton->enable();
            myAttributeButtonCombinableChoices->enable();
            myAttributeColorButton->enable();
            myAttributeCheckButton->enable();
        }
        // set left column
        if (myACAttr.isColor()) {
            myAttributeColorButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeColorButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeColorButton->show();
        } else if (myACAttr.isOptional()) {
            myAttributeCheckButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeCheckButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeCheckButton->setCheck(FALSE);
            myAttributeCheckButton->show();
        } else if (myACAttr.isEnablitable()) {
            myAttributeRadioButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeRadioButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeRadioButton->show();
            // enable or disable depending if is editable
            if (myACAttr.isNonEditable()) {
                myAttributeRadioButton->disable();
            } else {
                myAttributeRadioButton->enable();
            }
            // check if radio button has to be check
            if (attributeEnabled) {
                myAttributeRadioButton->setCheck(TRUE);
            } else {
                myAttributeRadioButton->setCheck(FALSE);
            }
        } else {
            // Show attribute Label
            myAttributeLabel->setText(myACAttr.getAttrStr().c_str());
            myAttributeLabel->show();
        }
        // Set field depending of the type of value
        if (myACAttr.isBool()) {
            // first we need to check if all boolean values are equal
            bool allBooleanValuesEqual = true;
            // declare  boolean vector
            std::vector<bool> booleanVector;
            // check if value can be parsed to a boolean vector
            if (GNEAttributeCarrier::canParse<std::vector<bool> >(value)) {
                booleanVector = GNEAttributeCarrier::parse<std::vector<bool> >(value);
            }
            // iterate over pased booleans comparing all element with the first
            for (const auto& i : booleanVector) {
                if (i != booleanVector.front()) {
                    allBooleanValuesEqual = false;
                }
            }
            // use checkbox or textfield depending if all booleans are equal
            if (allBooleanValuesEqual) {
                // set check button
                if ((booleanVector.size() > 0) && booleanVector.front()) {
                    myValueCheckButton->setCheck(true);
                    myValueCheckButton->setText("true");
                } else {
                    myValueCheckButton->setCheck(false);
                    myValueCheckButton->setText("false");
                }
                // show check button
                myValueCheckButton->show();
            } else {
                // show list of bools (0 1)
                myValueTextFieldStrings->setText(value.c_str());
                myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
                myValueTextFieldStrings->show();
                // enable or disable depending if attribute is editable and is enabled (used by disjoint attributes)
                if (myACAttr.isNonEditable() || !attributeEnabled) {
                    myValueTextFieldStrings->disable();
                }
            }
        } else if (myACAttr.isDiscrete()) {
            // Check if are combinable choices
            if ((myACAttr.getDiscreteValues().size() > 0) && myACAttr.isCombinable()) {
                // hide label
                myAttributeLabel->hide();
                // Show button combinable choices
                myAttributeButtonCombinableChoices->setText(myACAttr.getAttrStr().c_str());
                myAttributeButtonCombinableChoices->show();
                // Show string with the values
                myValueTextFieldStrings->setText(value.c_str());
                myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
                myValueTextFieldStrings->show();
            } else if (!myMultiple) {
                // fill comboBox
                myValueComboBoxChoices->clearItems();
                for (const auto& it : myACAttr.getDiscreteValues()) {
                    myValueComboBoxChoices->appendItem(it.c_str());
                }
                // show combo box with values
                myValueComboBoxChoices->setNumVisible((int)myACAttr.getDiscreteValues().size());
                myValueComboBoxChoices->setCurrentItem(myValueComboBoxChoices->findItem(value.c_str()));
                myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
                myValueComboBoxChoices->show();
            } else {
                // represent combinable choices in multiple selections always with a textfield instead with a comboBox
                myValueTextFieldStrings->setText(value.c_str());
                myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
                myValueTextFieldStrings->show();
            }
        } else if (myACAttr.isFloat() || myACAttr.isSUMOTime()) {
            // show TextField for real/time values
            myValueTextFieldReal->setText(value.c_str());
            myValueTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldReal->show();
        } else if (myACAttr.isInt()) {
            // Show textField for int attributes
            myValueTextFieldInt->setText(value.c_str());
            myValueTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldInt->show();
            // we need an extra check for connection attribute "TLIndex", because it cannot be edited if junction's connection doesn' have a TLS
            if ((myACAttr.getTagPropertyParent().getTag() == SUMO_TAG_CONNECTION) && (myACAttr.getAttr() == SUMO_ATTR_TLLINKINDEX) && (value == "No TLS")) {
                myValueTextFieldInt->disable();
            }
        } else {
            // In any other case (String, list, etc.), show value as String
            myValueTextFieldStrings->setText(value.c_str());
            myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldStrings->show();
        }
        // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
        if (((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myACAttr.getTagPropertyParent().isDemandElement()) ||
                ((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myACAttr.getTagPropertyParent().isDemandElement())) {
            myAttributeColorButton->disable();
            myAttributeRadioButton->disable();
            myAttributeCheckButton->disable();
            myValueTextFieldInt->disable();
            myValueTextFieldReal->disable();
            myValueTextFieldStrings->disable();
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
            myAttributeButtonCombinableChoices->disable();
        }
        // special case for Traffic Lights
        if ((myACAttr.getTagPropertyParent().getTag() == SUMO_TAG_JUNCTION) && (myACAttr.getAttr() == SUMO_ATTR_TLID) && value.empty()) {
            myValueTextFieldStrings->disable();
        }
        // special case for Default vehicle types (ID cannot be edited)
        if ((ACAttr.getTagPropertyParent().getTag() == SUMO_TAG_VTYPE) && (ACAttr.getAttr() == SUMO_ATTR_ID) &&
            ((value == DEFAULT_VTYPE_ID) || (value == DEFAULT_PEDTYPE_ID) || (value == DEFAULT_BIKETYPE_ID))) {
            myValueTextFieldStrings->disable();
        }
        // Show AttributesEditorRow
        show();
    }
}


void 
GNEFrameHelper::AttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEFrameHelper::AttributesEditorRow::refreshAttributesEditorRow(const std::string& value, bool forceRefresh, bool disjointAttributeEnabled) {
    // start enabling all elements
    myValueTextFieldInt->enable();
    myValueTextFieldReal->enable();
    myValueTextFieldStrings->enable();
    myValueComboBoxChoices->enable();
    myValueCheckButton->enable();
    myAttributeButtonCombinableChoices->enable();
    myAttributeColorButton->enable();
    myAttributeRadioButton->enable();
    myAttributeCheckButton->enable();
    // set radio buton
    if (myAttributeRadioButton->shown()) {
        myAttributeRadioButton->setCheck(disjointAttributeEnabled);
    }
    // set check buton
    if (myAttributeCheckButton->shown()) {
        myAttributeCheckButton->setCheck(/*disjointAttributeEnabled*/ FALSE );
    }
    if (myValueTextFieldInt->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myValueTextFieldInt->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myValueTextFieldInt->setText(value.c_str());
            myValueTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        }
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !disjointAttributeEnabled) {
            myValueTextFieldInt->disable();
        }
    } else if (myValueTextFieldReal->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myValueTextFieldReal->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myValueTextFieldReal->setText(value.c_str());
            myValueTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        }
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !disjointAttributeEnabled) {
            myValueTextFieldReal->disable();
        }
    } else if (myValueTextFieldStrings->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myValueTextFieldStrings->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myValueTextFieldStrings->setText(value.c_str());
            myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        }
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !disjointAttributeEnabled) {
            myValueTextFieldStrings->disable();
        }
    } else if (myValueComboBoxChoices->shown()) {
        // fill comboBox again
        myValueComboBoxChoices->clearItems();
        for (const auto& it : myACAttr.getDiscreteValues()) {
            myValueComboBoxChoices->appendItem(it.c_str());
        }
        // show combo box with values
        myValueComboBoxChoices->setNumVisible((int)myACAttr.getDiscreteValues().size());
        myValueComboBoxChoices->setCurrentItem(myValueComboBoxChoices->findItem(value.c_str()));
        myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
        myValueComboBoxChoices->show();
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !disjointAttributeEnabled) {
            myValueComboBoxChoices->disable();
        }
    } else if (myValueCheckButton->shown()) {
        if (GNEAttributeCarrier::canParse<bool>(value)) {
            myValueCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(value));
        } else {
            myValueCheckButton->setCheck(false);
        }
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !disjointAttributeEnabled) {
            myValueCheckButton->disable();
        }
    }
    // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
    if (myACAttr.getAttr() != SUMO_ATTR_NOTHING) {
        if (((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myACAttr.getTagPropertyParent().isDemandElement()) ||
            ((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myACAttr.getTagPropertyParent().isDemandElement())) {
            myAttributeColorButton->disable();
            myAttributeRadioButton->disable();
            myAttributeCheckButton->disable();
            myValueTextFieldInt->disable();
            myValueTextFieldReal->disable();
            myValueTextFieldStrings->disable();
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
            myAttributeButtonCombinableChoices->disable();
        }
        // special case for Traffic Lights
        if ((myACAttr.getTagPropertyParent().getTag() == SUMO_TAG_JUNCTION) && (myACAttr.getAttr() == SUMO_ATTR_TLID) && value.empty()) {
            myValueTextFieldStrings->disable();
        }
        // special case for Default vehicle types (ID cannot be edited)
        if ((myACAttr.getTagPropertyParent().getTag() == SUMO_TAG_VTYPE) && (myACAttr.getAttr() == SUMO_ATTR_ID) &&
            ((value == DEFAULT_VTYPE_ID) || (value == DEFAULT_PEDTYPE_ID) || (value == DEFAULT_BIKETYPE_ID))) {
            myValueTextFieldStrings->disable();
        }
    }
}


bool
GNEFrameHelper::AttributesEditorRow::isAttributesEditorRowValid() const {
    return ((myValueTextFieldInt->getTextColor() == FXRGB(0, 0, 0)) && (myValueTextFieldReal->getTextColor() == FXRGB(0, 0, 0)) &&
            (myValueTextFieldStrings->getTextColor() == FXRGB(0, 0, 0)) && (myValueComboBoxChoices->getTextColor() == FXRGB(0, 0, 0)));
}


long
GNEFrameHelper::AttributesEditorRow::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    if (obj == myAttributeColorButton) {
        // create FXColorDialog
        FXColorDialog colordialog(this, tr("Color Dialog"));
        colordialog.setTarget(this);
        // If previous attribute wasn't correct, set black as default color
        if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextFieldStrings->getText().text())) {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myValueTextFieldStrings->getText().text())));
        } else if (!myACAttr.getDefaultValue().empty()) {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myACAttr.getDefaultValue())));
        } else {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::BLACK));
        }
        // execute dialog to get a new color
        if (colordialog.execute()) {
            std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
            myValueTextFieldStrings->setText(newValue.c_str());
            if (myAttributesEditorParent->getEditedACs().front()->isValid(myACAttr.getAttr(), newValue)) {
                // if its valid for the first AC than its valid for all (of the same type)
                if (myAttributesEditorParent->getEditedACs().size() > 1) {
                    myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("Change multiple attributes");
                }
                // Set new value of attribute in all selected ACs
                for (const auto& it_ac : myAttributesEditorParent->getEditedACs()) {
                    it_ac->setAttribute(myACAttr.getAttr(), newValue, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
                }
                // If previously value was incorrect, change font color to black
                myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
                myValueTextFieldStrings->killFocus();
            }
        }
        return 0;
    } else if (obj == myAttributeButtonCombinableChoices) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("Change multiple attributes");
        }
        // open GNEDialog_AllowDisallow
        GNEDialog_AllowDisallow(myAttributesEditorParent->getFrameParent()->myViewNet, myAttributesEditorParent->getEditedACs().front()).execute();
        std::string allowed = myAttributesEditorParent->getEditedACs().front()->getAttribute(SUMO_ATTR_ALLOW);
        // Set new value of attribute in all selected ACs
        for (const auto& it_ac : myAttributesEditorParent->getEditedACs()) {
            it_ac->setAttribute(SUMO_ATTR_ALLOW, allowed, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
        }
        // finish change multiple attributes
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_end();
        }
        // update frame parent after attribute sucesfully set
        myAttributesEditorParent->getFrameParent()->attributeUpdated();
        return 1;
    } else {
        throw ProcessError("Invalid call to onCmdOpenAttributeDialog");
    }
}


long
GNEFrameHelper::AttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // First, obtain the string value of the new attribute depending of their type
    if (myACAttr.isBool()) {
        // first check if we're editing boolean as a list of string or as a checkbox
        if (myValueCheckButton->shown()) {
            // Set true o false depending of the checkBox
            if (myValueCheckButton->getCheck()) {
                myValueCheckButton->setText("true");
                newVal = "true";
            } else {
                myValueCheckButton->setText("false");
                newVal = "false";
            }
        } else {
            // obtain boolean value of myValueTextFieldStrings (because we're inspecting multiple attribute carriers with different values)
            newVal = myValueTextFieldStrings->getText().text();
        }
    } else if (myACAttr.isDiscrete()) {
        // Check if are combinable choices (for example, Vehicle Types)
        if ((myACAttr.getDiscreteValues().size() > 0) &&
                myACAttr.isCombinable()) {
            // Get value obtained using AttributesEditor
            newVal = myValueTextFieldStrings->getText().text();
        } else if (!myMultiple) {
            // Get value of ComboBox
            newVal = myValueComboBoxChoices->getText().text();
        } else {
            // due this is a multiple selection, obtain value of myValueTextFieldStrings instead of comboBox
            newVal = myValueTextFieldStrings->getText().text();
        }
    } else if (myACAttr.isFloat() || myACAttr.isSUMOTime()) {
        // Check if default value of attribute must be set
        if (myValueTextFieldReal->getText().empty() && myACAttr.hasStaticDefaultValue()) {
            newVal = myACAttr.getDefaultValue();
            myValueTextFieldReal->setText(newVal.c_str());
        } else {
            // obtain value of myValueTextFieldReal
            newVal = myValueTextFieldReal->getText().text();
        }
    } else if (myACAttr.isInt()) {
        // Check if default value of attribute must be set
        if (myValueTextFieldInt->getText().empty() && myACAttr.hasStaticDefaultValue()) {
            newVal = myACAttr.getDefaultValue();
            myValueTextFieldInt->setText(newVal.c_str());
        } else {
            // obtain value of myValueTextFieldInt
            newVal = myValueTextFieldInt->getText().text();
        }
    } else {
        // Check if default value of attribute must be set
        if (myValueTextFieldStrings->getText().empty() && myACAttr.hasStaticDefaultValue()) {
            newVal = myACAttr.getDefaultValue();
            myValueTextFieldStrings->setText(newVal.c_str());
        } else {
            // obtain value of myValueTextFieldStrings
            newVal = myValueTextFieldStrings->getText().text();
        }
    }

    // we need a extra check for Position and Shape Values, due #2658
    if ((myACAttr.getAttr() == SUMO_ATTR_POSITION) || (myACAttr.getAttr() == SUMO_ATTR_SHAPE)) {
        newVal = stripWhitespaceAfterComma(newVal);
    }

    // Check if attribute must be changed
    if (myAttributesEditorParent->getEditedACs().front()->isValid(myACAttr.getAttr(), newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("Change multiple attributes");
        } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
            // IDs attribute has to be encapsulated
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("change " + myACAttr.getTagPropertyParent().getTagStr() + " attribute");
        }
        // Set new value of attribute in all selected ACs
        for (const auto& it_ac : myAttributesEditorParent->getEditedACs()) {
            it_ac->setAttribute(myACAttr.getAttr(), newVal, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
        }
        // finish change multiple attributes or ID Attributes
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_end();
        } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_end();
        }
        // If previously value was incorrect, change font color to black
        if (myACAttr.isCombinable()) {
            myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldStrings->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor(false, false);
        } else if (myACAttr.isDiscrete()) {
            myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
            myValueComboBoxChoices->killFocus();
        } else if (myACAttr.isFloat() || myACAttr.isSUMOTime()) {
            myValueTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldReal->killFocus();
        } else if (myACAttr.isInt() && myValueTextFieldStrings != nullptr) {
            myValueTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldInt->killFocus();
        } else if (myValueTextFieldStrings != nullptr) {
            myValueTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myValueTextFieldStrings->killFocus();
        }
        // update frame parent after attribute sucesfully set
        myAttributesEditorParent->getFrameParent()->attributeUpdated();
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (myACAttr.isCombinable()) {
            myValueTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
            myValueTextFieldStrings->killFocus();
        } else if (myACAttr.isDiscrete()) {
            myValueComboBoxChoices->setTextColor(FXRGB(255, 0, 0));
            myValueComboBoxChoices->killFocus();
        } else if (myACAttr.isFloat() || myACAttr.isSUMOTime()) {
            myValueTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        } else if (myACAttr.isInt() && myValueTextFieldStrings != nullptr) {
            myValueTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        } else if (myValueTextFieldStrings != nullptr) {
            myValueTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
        }
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG("Value '" + newVal + "' for attribute " + myACAttr.getAttrStr() + " of " + myACAttr.getTagPropertyParent().getTagStr() + " isn't valid");
    }
    return 1;
}


long
GNEFrameHelper::AttributesEditorRow::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
    if (myAttributeCheckButton->getCheck()) {
        // enable input values
        myValueCheckButton->enable();
        myValueTextFieldInt->enable();
        myValueTextFieldReal->enable();
        myValueTextFieldStrings->enable();
    } else {
        // disable input values
        myValueCheckButton->disable();
        myValueTextFieldInt->disable();
        myValueTextFieldReal->disable();
        myValueTextFieldStrings->disable();
    }
    return 0;
}


long
GNEFrameHelper::AttributesEditorRow::onCmdSelectRadioButton(FXObject*, FXSelector, void*) {
    // obtain undoList (To improve code legibly)
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList();
    // write debug (for Netedit tests)
    WRITE_DEBUG("Selected radio button for attribute '" + myACAttr.getAttrStr() + "'");
    // begin undoList
    undoList->p_begin("enable attribute '" + myACAttr.getAttrStr() + "'");
    // change disjoint attribute with undo/redo
    myAttributesEditorParent->getEditedACs().front()->enableAttribute(myACAttr.getAttr(), undoList);
    // begin undoList
    undoList->p_end();
    // refresh Attributes edito parent
    myAttributesEditorParent->refreshAttributeEditor(false, false);
    return 0;
}


GNEFrameHelper::AttributesEditorRow::AttributesEditorRow() :
    myMultiple(false) {
}


std::string
GNEFrameHelper::AttributesEditorRow::stripWhitespaceAfterComma(const std::string& stringValue) {
    std::string result(stringValue);
    while (result.find(", ") != std::string::npos) {
        result = StringUtils::replace(result, ", ", ",");
    }
    return result;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::AttributesEditor::AttributesEditor(GNEFrame* FrameParent) :
    FXGroupBox(FrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myFrameParent(FrameParent),
    myIncludeExtended(true) {
    // resize myAttributesEditorRows
    myAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEFrameHelper::AttributesEditor::showAttributeEditorModul(const std::vector<GNEAttributeCarrier*>& ACs, bool includeExtended) {
    myEditedACs = ACs;
    myIncludeExtended = includeExtended;
    // remove all rows
    for (int i = 0; i < (int)myAttributesEditorRows.size(); i++) {
        // destroy and delete all rows
        if (myAttributesEditorRows.at(i) != nullptr) {
            myAttributesEditorRows.at(i)->destroy();
            delete myAttributesEditorRows.at(i);
            myAttributesEditorRows.at(i) = nullptr;
        }
    }
    if (myEditedACs.size() > 0) {
        // Iterate over attributes
        for (const auto& i : myEditedACs.front()->getTagProperty()) {
            // disable editing for unique attributes in case of multi-selection
            if ((myEditedACs.size() > 1) && i.isUnique()) {
                continue;
            }
            // disable editing of extended attributes if includeExtended isn't enabled
            if (i.isExtended() && !includeExtended) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item (note: We use a set to avoid repeated values)
            std::set<std::string> occuringValues;
            for (const auto& it_ac : myEditedACs) {
                occuringValues.insert(it_ac->getAttribute(i.getAttr()));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            std::string value = oss.str();
            if ((myEditedACs.front()->getTagProperty().getTag() == SUMO_TAG_CONNECTION) &&
                    (i.getAttr() == SUMO_ATTR_TLLINKINDEX)
                    && value == toString(NBConnection::InvalidTlIndex)) {
                // possibly the connections are newly created (allow assigning
                // tlIndex if the junction(s) have a traffic light
                for (const auto& it_ac : myEditedACs) {
                    if (!it_ac->isValid(SUMO_ATTR_TLLINKINDEX, "0")) {
                        value =  "No TLS";
                        break;
                    }
                }
            }
            // show AttributesEditor
            show();
            // create attribute editor row
            myAttributesEditorRows[i.getPositionListed()] = new AttributesEditorRow(this, i, value, myEditedACs.front()->isAttributeEnabled(i.getAttr()));
        }
    }
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
}


void
GNEFrameHelper::AttributesEditor::hideAttributesEditorModul() {
    // clear myEditedACs
    myEditedACs.clear();
    // hide also AttributesEditor
    hide();
}


void
GNEFrameHelper::AttributesEditor::refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition) {
    if (myEditedACs.size() > 0) {
        // Iterate over attributes
        for (const auto& i : myEditedACs.front()->getTagProperty()) {
            // disable editing for unique attributes in case of multi-selection
            if ((myEditedACs.size() > 1) && i.isUnique()) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (const auto& it_ac : myEditedACs) {
                occuringValues.insert(it_ac->getAttribute(i.getAttr()));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            // check if is a disjoint attribute
            bool disjointAttributeSet = myEditedACs.front()->isAttributeEnabled(i.getAttr());
            // Check if refresh of Position or Shape has to be forced
            if ((i.getAttr()  == SUMO_ATTR_SHAPE) && forceRefreshShape) {
                myAttributesEditorRows[i.getPositionListed()]->refreshAttributesEditorRow(oss.str(), true, disjointAttributeSet);
            } else if ((i.getAttr()  == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                // Refresh attributes maintain invalid values
                myAttributesEditorRows[i.getPositionListed()]->refreshAttributesEditorRow(oss.str(), true, disjointAttributeSet);
            } else {
                // Refresh attributes maintain invalid values
                myAttributesEditorRows[i.getPositionListed()]->refreshAttributesEditorRow(oss.str(), false, disjointAttributeSet);
            }
        }
    }
}


GNEFrame* 
GNEFrameHelper::AttributesEditor::getFrameParent() const {
    return myFrameParent;
}


const std::vector<GNEAttributeCarrier*>&
GNEFrameHelper::AttributesEditor::getEditedACs() const {
    return myEditedACs;
}


void
GNEFrameHelper::AttributesEditor::removeEditedAC(GNEAttributeCarrier* AC) {
    // Only remove if there is inspected ACs
    if (myEditedACs.size() > 0) {
        // Try to find AC in myACs
        auto i = std::find(myEditedACs.begin(), myEditedACs.end(), AC);
        // if was found
        if (i != myEditedACs.end()) {
            // erase AC from inspected ACs
            myEditedACs.erase(i);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG("Removed inspected element from Inspected ACs. " + toString(myEditedACs.size()) + " ACs remains.");
            // Inspect multi selection again (To refresh Modul)
            showAttributeEditorModul(myEditedACs, myIncludeExtended);
        }
    }
}


long
GNEFrameHelper::AttributesEditor::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog if there is inspected ACs
    if (myEditedACs.size() > 0) {
        // open Help attributes dialog
        myFrameParent->openHelpAttributesDialog(myEditedACs.front()->getTagProperty());
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::AttributesEditorExtended- methods
// ---------------------------------------------------------------------------

GNEFrameHelper::AttributesEditorExtended::AttributesEditorExtended(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Extended attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // Create open dialog button
    new FXButton(this, "Open attributes editor", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrameHelper::AttributesEditorExtended::~AttributesEditorExtended() {}


void
GNEFrameHelper::AttributesEditorExtended::showAttributesEditorExtendedModul() {
    show();
}


void
GNEFrameHelper::AttributesEditorExtended::hideAttributesEditorExtendedModul() {
    hide();
}


long
GNEFrameHelper::AttributesEditorExtended::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // open AttributesCreator extended dialog
    myFrameParent->attributesEditorExtendedDialogOpened();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::AttributeCarrierHierarchy - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::AttributeCarrierHierarchy::AttributeCarrierHierarchy(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Hierarchy", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myAC(nullptr),
    myClickedAC(nullptr),
    myClickedJunction(nullptr),
    myClickedEdge(nullptr),
    myClickedLane(nullptr),
    myClickedCrossing(nullptr),
    myClickedConnection(nullptr),
    myClickedShape(nullptr),
    myClickedAdditional(nullptr),
    myClickedDemandElement(nullptr) {
    // Create three list
    myTreelist = new FXTreeList(this, this, MID_GNE_ACHIERARCHY_SHOWCHILDMENU, GUIDesignTreeListFrame);
    hide();
}


GNEFrameHelper::AttributeCarrierHierarchy::~AttributeCarrierHierarchy() {}


void
GNEFrameHelper::AttributeCarrierHierarchy::showAttributeCarrierHierarchy(GNEAttributeCarrier* AC) {
    myAC = AC;
    // show AttributeCarrierHierarchy and refresh AttributeCarrierHierarchy
    if (myAC) {
        show();
        refreshAttributeCarrierHierarchy();
    }
}


void
GNEFrameHelper::AttributeCarrierHierarchy::hideAttributeCarrierHierarchy() {
    // set all pointers null
    myAC = nullptr;
    myClickedAC = nullptr;
    myClickedJunction = nullptr;
    myClickedEdge = nullptr;
    myClickedLane = nullptr;
    myClickedCrossing = nullptr;
    myClickedConnection = nullptr;
    myClickedShape = nullptr;
    myClickedAdditional = nullptr;
    myClickedDemandElement = nullptr;
    // hide modul
    hide();
}


void
GNEFrameHelper::AttributeCarrierHierarchy::refreshAttributeCarrierHierarchy() {
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsConnections.clear();
    // show ACChildren of myAC
    if (myAC) {
        showAttributeCarrierChildren(myAC, showAttributeCarrierParents());
    }
}


long
GNEFrameHelper::AttributeCarrierHierarchy::onCmdShowChildMenu(FXObject*, FXSelector, void* eventData) {
    // Obtain event
    FXEvent* e = (FXEvent*)eventData;
    // obtain FXTreeItem in the given position
    FXTreeItem* item = myTreelist->getItemAt(e->win_x, e->win_y);
    // open Pop-up if FXTreeItem has a Attribute Carrier vinculated
    if (item && (myTreeItemsConnections.find(item) == myTreeItemsConnections.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[item]);
    }
    return 1;
}


long
GNEFrameHelper::AttributeCarrierHierarchy::onCmdCenterItem(FXObject*, FXSelector, void*) {
    // Center item
    if (myClickedJunction) {
        myFrameParent->myViewNet->centerTo(myClickedJunction->getGlID(), true, -1);
    } else if (myClickedEdge) {
        myFrameParent->myViewNet->centerTo(myClickedEdge->getGlID(), true, -1);
    } else if (myClickedLane) {
        myFrameParent->myViewNet->centerTo(myClickedLane->getGlID(), true, -1);
    } else if (myClickedCrossing) {
        myFrameParent->myViewNet->centerTo(myClickedCrossing->getGlID(), true, -1);
    } else if (myClickedConnection) {
        myFrameParent->myViewNet->centerTo(myClickedConnection->getGlID(), true, -1);
    } else if (myClickedAdditional) {
        myFrameParent->myViewNet->centerTo(myClickedAdditional->getGlID(), true, -1);
    } else if (myClickedShape) {
        myFrameParent->myViewNet->centerTo(myClickedShape->getGlID(), true, -1);
    } else if (myClickedDemandElement) {
        myFrameParent->myViewNet->centerTo(myClickedDemandElement->getGlID(), true, -1);
    }
    // update view after centering
    myFrameParent->myViewNet->update();
    return 1;
}


long
GNEFrameHelper::AttributeCarrierHierarchy::onCmdInspectItem(FXObject*, FXSelector, void*) {
    if ((myAC != nullptr) && (myClickedAC != nullptr)) {
        myFrameParent->myViewNet->getViewParent()->getInspectorFrame()->inspectChild(myClickedAC, myAC);
    }
    return 1;
}


long
GNEFrameHelper::AttributeCarrierHierarchy::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    // check if Inspector frame was opened before removing
    const std::vector<GNEAttributeCarrier*>& currentInspectedACs = myFrameParent->myViewNet->getViewParent()->getInspectorFrame()->getAttributesEditor()->getEditedACs();
    // Remove Attribute Carrier
    if (myClickedJunction) {
        myFrameParent->myViewNet->getNet()->deleteJunction(myClickedJunction, myFrameParent->myViewNet->getUndoList());
    } else if (myClickedEdge) {
        myFrameParent->myViewNet->getNet()->deleteEdge(myClickedEdge, myFrameParent->myViewNet->getUndoList(), false);
    } else if (myClickedLane) {
        myFrameParent->myViewNet->getNet()->deleteLane(myClickedLane, myFrameParent->myViewNet->getUndoList(), false);
    } else if (myClickedCrossing) {
        myFrameParent->myViewNet->getNet()->deleteCrossing(myClickedCrossing, myFrameParent->myViewNet->getUndoList());
    } else if (myClickedConnection) {
        myFrameParent->myViewNet->getNet()->deleteConnection(myClickedConnection, myFrameParent->myViewNet->getUndoList());
    } else if (myClickedAdditional) {
        myFrameParent->myViewNet->getNet()->deleteAdditional(myClickedAdditional, myFrameParent->myViewNet->getUndoList());
    } else if (myClickedShape) {
        myFrameParent->myViewNet->getNet()->deleteShape(myClickedShape, myFrameParent->myViewNet->getUndoList());
    } else if (myClickedDemandElement) {
        // check that default VTypes aren't removed
        if ((myClickedDemandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && (GNEAttributeCarrier::parse<bool>(myClickedDemandElement->getAttribute(GNE_ATTR_DEFAULT_VTYPE)))) {
            WRITE_WARNING("Default Vehicle Type '" + myClickedDemandElement->getAttribute(SUMO_ATTR_ID) +"' cannot be removed");
            return 1;
        } else {
            myFrameParent->myViewNet->getNet()->deleteDemandElement(myClickedDemandElement, myFrameParent->myViewNet->getUndoList());
        }
    }
    // update viewNet
    myFrameParent->myViewNet->update();
    // refresh AC Hierarchy
    refreshAttributeCarrierHierarchy();
    // check if inspector frame has to be shown again
    if (currentInspectedACs.size() == 1) {
        if (currentInspectedACs.front() != myClickedAC) {
            myFrameParent->myViewNet->getViewParent()->getInspectorFrame()->inspectSingleElement(currentInspectedACs.front());
        } else {
            // inspect a nullprt element to reset inspector frame
            myFrameParent->myViewNet->getViewParent()->getInspectorFrame()->inspectSingleElement(nullptr);
        }
    }
    return 1;
}


long 
GNEFrameHelper::AttributeCarrierHierarchy::onCmdMoveItemUp(FXObject*, FXSelector, void*) {
    // currently only children of demand elements can be moved
    if(myClickedDemandElement) {
        myFrameParent->myViewNet->getUndoList()->p_begin(("moving up " + myClickedDemandElement->getTagStr()).c_str());
        // move element one position back
        myFrameParent->myViewNet->getUndoList()->add(new GNEChange_Children(myClickedDemandElement->getDemandElementParents().at(0), myClickedDemandElement, 
                                                                            GNEChange_Children::Operation::MOVE_BACK), true);
        myFrameParent->myViewNet->getUndoList()->p_end();
    }
    // refresh after moving child
    refreshAttributeCarrierHierarchy();
    return 1;
}


long 
GNEFrameHelper::AttributeCarrierHierarchy::onCmdMoveItemDown(FXObject*, FXSelector, void*) {
    // currently only children of demand elements can be moved
    if(myClickedDemandElement) {
        myFrameParent->myViewNet->getUndoList()->p_begin(("moving down " + myClickedDemandElement->getTagStr()).c_str());
        // move element one position front
        myFrameParent->myViewNet->getUndoList()->add(new GNEChange_Children(myClickedDemandElement->getDemandElementParents().at(0), myClickedDemandElement, 
                                                                            GNEChange_Children::Operation::MOVE_FRONT), true);
        myFrameParent->myViewNet->getUndoList()->p_end();
    }
    // refresh after moving child
    refreshAttributeCarrierHierarchy();
    return 1;
}


void
GNEFrameHelper::AttributeCarrierHierarchy::createPopUpMenu(int X, int Y, GNEAttributeCarrier* clickedAC) {
    // first check that AC exist
    if (clickedAC) {
        // set current clicked AC
        myClickedAC = clickedAC;
        // cast all elements
        myClickedJunction = dynamic_cast<GNEJunction*>(clickedAC);
        myClickedEdge = dynamic_cast<GNEEdge*>(clickedAC);
        myClickedLane = dynamic_cast<GNELane*>(clickedAC);
        myClickedCrossing = dynamic_cast<GNECrossing*>(clickedAC);
        myClickedConnection = dynamic_cast<GNEConnection*>(clickedAC);
        myClickedShape = dynamic_cast<GNEShape*>(clickedAC);
        myClickedAdditional = dynamic_cast<GNEAdditional*>(clickedAC);
        myClickedDemandElement = dynamic_cast<GNEDemandElement*>(clickedAC);
        // create FXMenuPane
        FXMenuPane* pane = new FXMenuPane(myTreelist);
        // set item name and icon
        new MFXMenuHeader(pane, myFrameParent->myViewNet->getViewParent()->getGUIMainWindow()->getBoldFont(), myClickedAC->getPopUpID().c_str(), myClickedAC->getIcon());
        // insert separator
        new FXMenuSeparator(pane);
        // create center menu command
        FXMenuCommand* centerMenuCommand = new FXMenuCommand(pane, "Center", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_GNE_CENTER);
        // disable Centering for Vehicle Types
        if (myClickedAC->getTagProperty().isVehicleType()) {
            centerMenuCommand->disable();
        }
        // create inspect and delete menu commands
        FXMenuCommand* inspectMenuCommand = new FXMenuCommand(pane, "Inspect", GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_INSPECT);
        FXMenuCommand* deleteMenuCommand = new FXMenuCommand(pane, "Delete", GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_DELETE);
        // check if inspect and delete menu commands has to be disabled
        if ((myClickedAC->getTagProperty().isNetElement() && (myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND)) ||
            (myClickedAC->getTagProperty().isDemandElement() && (myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK))) {
            inspectMenuCommand->disable();
            deleteMenuCommand->disable();
        }
        // now chec if given AC support manually moving of their item up and down (Currently only for demand elements
        if (myClickedDemandElement && myClickedAC->getTagProperty().canBeSortedManually()) {
            // insert separator
            new FXMenuSeparator(pane);
            // create both moving menu commands
            FXMenuCommand* moveUpMenuCommand = new FXMenuCommand(pane, "Move up", GUIIconSubSys::getIcon(ICON_ARROW_UP), this, MID_GNE_ACHIERARCHY_MOVEUP);
            FXMenuCommand* moveDownMenuCommand = new FXMenuCommand(pane, "Move down", GUIIconSubSys::getIcon(ICON_ARROW_DOWN), this, MID_GNE_ACHIERARCHY_MOVEDOWN);
            // check if both commands has to be disabled
            if (myClickedDemandElement->getTagProperty().isPersonStop()) {
                moveUpMenuCommand->setText("Move up (Stops cannot be moved)");
                moveDownMenuCommand->setText("Move diwb (Stops cannot be moved)");
                moveUpMenuCommand->disable();
                moveDownMenuCommand->disable();
            } else {
                // check if moveUpMenuCommand has to be disabled
                if (myClickedDemandElement->getDemandElementParents().front()->getDemandElementChildren().front() == myClickedDemandElement) {
                    moveUpMenuCommand->setText("Move up (It's already the first element)");
                    moveUpMenuCommand->disable();
                } else if (myClickedDemandElement->getDemandElementParents().front()->getPreviousemandElement(myClickedDemandElement)->getTagProperty().isPersonStop()) {
                    moveUpMenuCommand->setText("Move up (Previous element is a Stop)");
                    moveUpMenuCommand->disable();
                }
                // check if moveDownMenuCommand has to be disabled
                if (myClickedDemandElement->getDemandElementParents().front()->getDemandElementChildren().back() == myClickedDemandElement) {
                    moveDownMenuCommand->setText("Move down (It's already the last element)");
                    moveDownMenuCommand->disable();
                } else if (myClickedDemandElement->getDemandElementParents().front()->getNextDemandElement(myClickedDemandElement)->getTagProperty().isPersonStop()) {
                    moveDownMenuCommand->setText("Move down (Next element is a Stop)");
                    moveDownMenuCommand->disable();
                }
            }
        }
        // Center in the mouse position and create pane
        pane->setX(X);
        pane->setY(Y);
        pane->create();
        pane->show();
    } else {
        // set all clicked elements to null
        myClickedAC = nullptr;
        myClickedJunction = nullptr;
        myClickedEdge = nullptr;
        myClickedLane = nullptr;
        myClickedCrossing = nullptr;
        myClickedConnection = nullptr;
        myClickedShape = nullptr;
        myClickedAdditional = nullptr;
        myClickedDemandElement = nullptr;
    }
}


FXTreeItem*
GNEFrameHelper::AttributeCarrierHierarchy::showAttributeCarrierParents() {
    if (myAC->getTagProperty().isNetElement()) {
        // check demand element type
        switch (myAC->getTagProperty().getTag()) {
            case SUMO_TAG_EDGE: {
                // obtain Edge
                GNEEdge* edge = myFrameParent->myViewNet->getNet()->retrieveEdge(myAC->getID(), false);
                if (edge) {
                    // insert Junctions of edge in tree (Pararell because a edge has always two Junctions)
                    FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                    FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                    junctionDestinyItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                    myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                    // return junction destiny Item
                    return junctionDestinyItem;
                } else {
                    return nullptr;
                }
            }
            case SUMO_TAG_LANE: {
                // obtain lane
                GNELane* lane = myFrameParent->myViewNet->getNet()->retrieveLane(myAC->getID(), false);
                if (lane) {
                    // obtain edge parent
                    GNEEdge* edge = myFrameParent->myViewNet->getNet()->retrieveEdge(lane->getParentEdge().getID());
                    //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                    FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                    FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                    junctionDestinyItem->setExpanded(true);
                    // Create edge item
                    FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                    edgeItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                    myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                    myTreeItemToACMap[edgeItem] = edge;
                    // return edge item
                    return edgeItem;
                } else {
                    return nullptr;
                }
            }
            case SUMO_TAG_CROSSING: {
                // obtain Crossing
                GNECrossing* crossing = myFrameParent->myViewNet->getNet()->retrieveCrossing(myAC->getID(), false);
                if (crossing) {
                    // obtain junction
                    GNEJunction* junction = crossing->getParentJunction();
                    // create junction item
                    FXTreeItem* junctionItem = myTreelist->insertItem(nullptr, nullptr, junction->getHierarchyName().c_str(), junction->getIcon(), junction->getIcon());
                    junctionItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[junctionItem] = junction;
                    // return junction Item
                    return junctionItem;
                } else {
                    return nullptr;
                }
            }
            case SUMO_TAG_CONNECTION: {
                // obtain Connection
                GNEConnection* connection = myFrameParent->myViewNet->getNet()->retrieveConnection(myAC->getID(), false);
                if (connection) {
                    // create edge from item
                    FXTreeItem* edgeFromItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeFrom()->getHierarchyName().c_str(), connection->getEdgeFrom()->getIcon(), connection->getEdgeFrom()->getIcon());
                    edgeFromItem->setExpanded(true);
                    // create edge to item
                    FXTreeItem* edgeToItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeTo()->getHierarchyName().c_str(), connection->getEdgeTo()->getIcon(), connection->getEdgeTo()->getIcon());
                    edgeToItem->setExpanded(true);
                    // create connection item
                    FXTreeItem* connectionItem = myTreelist->insertItem(nullptr, edgeToItem, connection->getHierarchyName().c_str(), connection->getIcon(), connection->getIcon());
                    connectionItem->setExpanded(true);
                    // Save items in myTreeItemToACMap
                    myTreeItemToACMap[edgeFromItem] = connection->getEdgeFrom();
                    myTreeItemToACMap[edgeToItem] = connection->getEdgeTo();
                    myTreeItemToACMap[connectionItem] = connection;
                    // return connection item
                    return connectionItem;
                } else {
                    return nullptr;
                }
            }
            default:
                break;
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_POILANE) {
        // Obtain POILane
        GNEPOI* POILane = myFrameParent->myViewNet->getNet()->retrievePOI(myAC->getID(), false);
        if (POILane) {
            // obtain lane parent
            GNELane* lane = myFrameParent->myViewNet->getNet()->retrieveLane(POILane->getLaneParents().at(0)->getID());
            // obtain edge parent
            GNEEdge* edge = myFrameParent->myViewNet->getNet()->retrieveEdge(lane->getParentEdge().getID());
            //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
            FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
            FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
            junctionDestinyItem->setExpanded(true);
            // Create edge item
            FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
            edgeItem->setExpanded(true);
            // Create lane item
            FXTreeItem* laneItem = myTreelist->insertItem(nullptr, edgeItem, lane->getHierarchyName().c_str(), lane->getIcon(), lane->getIcon());
            laneItem->setExpanded(true);
            // Save items in myTreeItemToACMap
            myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
            myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
            myTreeItemToACMap[edgeItem] = edge;
            myTreeItemToACMap[laneItem] = lane;
            // return Lane item
            return laneItem;
        } else {
            return nullptr;
        }
    } else if (myAC->getTagProperty().isAdditional() || myAC->getTagProperty().isTAZ()) {
        // Obtain Additional
        GNEAdditional* additional = myFrameParent->myViewNet->getNet()->retrieveAdditional(myAC->getTagProperty().getTag(), myAC->getID(), false);
        if (additional) {
            // declare auxiliar FXTreeItem, due a demand element can have multiple "roots"
            FXTreeItem* root = nullptr;
            // check if there is demand elements parents
            if (additional->getAdditionalParents().size() > 0) {
                // check if we have more than one edge
                if (additional->getAdditionalParents().size() > 1) {
                    // insert first item
                    addListItem(additional->getAdditionalParents().front());
                    // insert "spacer"
                    if (additional->getAdditionalParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getAdditionalParents().size() - 2) + " additionals...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getAdditionalParents().back());
            }
            // check if there is demand element parents
            if (additional->getDemandElementParents().size() > 0) {
                // check if we have more than one demand element
                if (additional->getDemandElementParents().size() > 1) {
                    // insert first item
                    addListItem(additional->getDemandElementParents().front());
                    // insert "spacer"
                    if (additional->getDemandElementParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getDemandElementParents().size() - 2) + " demand elements...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getDemandElementParents().back());
            }
            // check if there is edge parents
            if (additional->getEdgeParents().size() > 0) {
                // check if we have more than one edge
                if (additional->getEdgeParents().size() > 1) {
                    // insert first item
                    addListItem(additional->getEdgeParents().front());
                    // insert "spacer"
                    if (additional->getEdgeParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getEdgeParents().size() - 2) + " edges...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getEdgeParents().back());
            }
            // check if there is lane parents
            if (additional->getLaneParents().size() > 0) {
                // check if we have more than one lane parent
                if (additional->getLaneParents().size() > 1) {
                    // insert first item
                    addListItem(additional->getLaneParents().front());
                    // insert "spacer"
                    if (additional->getLaneParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getLaneParents().size() - 2) + " lanes...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getLaneParents().back());
            }
            // return last inserted list item
            return root;
        }
    } else if (myAC->getTagProperty().isDemandElement()) {
        // Obtain DemandElement
        GNEDemandElement* demandElement = myFrameParent->myViewNet->getNet()->retrieveDemandElement(myAC->getTagProperty().getTag(), myAC->getID(), false);
        if (demandElement) {
            // declare auxiliar FXTreeItem, due a demand element can have multiple "roots"
            FXTreeItem* root = nullptr;
            // check if there is demand elements parents
            if (demandElement->getAdditionalParents().size() > 0) {
                // check if we have more than one edge
                if (demandElement->getAdditionalParents().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getAdditionalParents().front());
                    // insert "spacer"
                    if (demandElement->getAdditionalParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getAdditionalParents().size() - 2) + " additionals...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getAdditionalParents().back());
            }
            // check if there is demand element parents
            if (demandElement->getDemandElementParents().size() > 0) {
                // check if we have more than one demand element
                if (demandElement->getDemandElementParents().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getDemandElementParents().front());
                    // insert "spacer"
                    if (demandElement->getDemandElementParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getDemandElementParents().size() - 2) + " demand elements...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getDemandElementParents().back());
            }
            // check if there is edge parents
            if (demandElement->getEdgeParents().size() > 0) {
                // check if we have more than one edge
                if (demandElement->getEdgeParents().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getEdgeParents().front());
                    // insert "spacer"
                    if (demandElement->getEdgeParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getEdgeParents().size() - 2) + " edges...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getEdgeParents().back());
            }
            // check if there is lane parents
            if (demandElement->getLaneParents().size() > 0) {
                // check if we have more than one lane parent
                if (demandElement->getLaneParents().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getLaneParents().front());
                    // insert "spacer"
                    if (demandElement->getLaneParents().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getLaneParents().size() - 2) + " lanes...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getLaneParents().back());
            }
            // return last inserted list item
            return root;
        }
    }
    // there aren't parents
    return nullptr;
}


void
GNEFrameHelper::AttributeCarrierHierarchy::showAttributeCarrierChildren(GNEAttributeCarrier* AC, FXTreeItem* itemParent) {
    if (AC->getTagProperty().isNetElement()) {
        // Switch gl type of ac
        switch (AC->getTagProperty().getTag()) {
            case SUMO_TAG_JUNCTION: {
                // retrieve junction
                GNEJunction* junction = myFrameParent->myViewNet->getNet()->retrieveJunction(AC->getID(), false);
                if (junction) {
                    // insert junction item
                    FXTreeItem* junctionItem = addListItem(AC, itemParent);
                    // insert edges
                    for (auto i : junction->getGNEEdges()) {
                        showAttributeCarrierChildren(i, junctionItem);
                    }
                    // insert crossings
                    for (auto i : junction->getGNECrossings()) {
                        showAttributeCarrierChildren(i, junctionItem);
                    }
                }
                break;
            }
            case SUMO_TAG_EDGE: {
                // retrieve edge
                GNEEdge* edge = myFrameParent->myViewNet->getNet()->retrieveEdge(AC->getID(), false);
                if (edge) {
                    // insert edge item
                    FXTreeItem* edgeItem = addListItem(AC, itemParent);
                    // insert lanes
                    for (const auto& i : edge->getLanes()) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    // insert shape children
                    for (const auto& i : edge->getShapeChildren()) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    // insert additional children
                    for (const auto& i : edge->getAdditionalChildren()) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    // insert demand elements children (note: use getSortedDemandElementChildrenByType to avoid duplicated elements)
                    for (const auto &i : edge->getSortedDemandElementChildrenByType(SUMO_TAG_ROUTE)) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    for (const auto &i : edge->getSortedDemandElementChildrenByType(SUMO_TAG_TRIP)) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    for (const auto &i : edge->getSortedDemandElementChildrenByType(SUMO_TAG_FLOW)) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                }
                break;
            }
            case SUMO_TAG_LANE: {
                // retrieve lane
                GNELane* lane = myFrameParent->myViewNet->getNet()->retrieveLane(AC->getID(), false);
                if (lane) {
                    // insert lane item
                    FXTreeItem* laneItem = addListItem(AC, itemParent);
                    // insert shape children
                    for (const auto& i : lane->getShapeChildren()) {
                        showAttributeCarrierChildren(i, laneItem);
                    }
                    // insert additional children
                    for (const auto& i : lane->getAdditionalChildren()) {
                        showAttributeCarrierChildren(i, laneItem);
                    }
                    // insert demand elements children
                    for (const auto& i : lane->getDemandElementChildren()) {
                        showAttributeCarrierChildren(i, laneItem);
                    }
                    // insert incoming connections of lanes (by default isn't expanded)
                    if (lane->getGNEIncomingConnections().size() > 0) {
                        std::vector<GNEConnection*> incomingLaneConnections = lane->getGNEIncomingConnections();
                        // insert intermediate list item
                        FXTreeItem* incomingConnections = addListItem(laneItem, "Incomings", incomingLaneConnections.front()->getIcon(), false);
                        // insert incoming connections
                        for (auto i : incomingLaneConnections) {
                            showAttributeCarrierChildren(i, incomingConnections);
                        }
                    }
                    // insert outcoming connections of lanes (by default isn't expanded)
                    if (lane->getGNEOutcomingConnections().size() > 0) {
                        std::vector<GNEConnection*> outcomingLaneConnections = lane->getGNEOutcomingConnections();
                        // insert intermediate list item
                        FXTreeItem* outgoingConnections = addListItem(laneItem, "Outgoing", outcomingLaneConnections.front()->getIcon(), false);
                        // insert outcoming connections
                        for (auto i : outcomingLaneConnections) {
                            showAttributeCarrierChildren(i, outgoingConnections);
                        }
                    }
                }
                break;
            }
            case SUMO_TAG_CROSSING:
            case SUMO_TAG_CONNECTION: {
                // insert connection item
                addListItem(AC, itemParent);
                break;
            }
            default:
                break;
        }
    } else if (AC->getTagProperty().isShape()) {
        // insert shape item
        addListItem(AC, itemParent);
    } else if (AC->getTagProperty().isAdditional() || AC->getTagProperty().isTAZ()) {
        // retrieve additional
        GNEAdditional* additional = myFrameParent->myViewNet->getNet()->retrieveAdditional(AC->getTagProperty().getTag(), AC->getID(), false);
        if (additional) {
            // insert additional item
            FXTreeItem* additionalItem = addListItem(AC, itemParent);
            // insert edge children
            for (const auto& i : additional->getEdgeChildren()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert lane children
            for (const auto& i : additional->getLaneChildren()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert shape children
            for (const auto& i : additional->getShapeChildren()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert additionals children
            for (const auto& i : additional->getAdditionalChildren()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert demand element children
            for (const auto& i : additional->getDemandElementChildren()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
        }
    } else if (AC->getTagProperty().isDemandElement()) {
        // retrieve demandElement
        GNEDemandElement* demandElement = myFrameParent->myViewNet->getNet()->retrieveDemandElement(AC->getTagProperty().getTag(), AC->getID(), false);
        if (demandElement) {
            // insert demandElement item
            FXTreeItem* demandElementItem = addListItem(AC, itemParent);
            // insert edge children
            for (const auto& i : demandElement->getEdgeChildren()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert lane children
            for (const auto& i : demandElement->getLaneChildren()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert shape children
            for (const auto& i : demandElement->getShapeChildren()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert additionals children
            for (const auto& i : demandElement->getAdditionalChildren()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert demand element children
            for (const auto& i : demandElement->getDemandElementChildren()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
        }
    }
}


FXTreeItem*
GNEFrameHelper::AttributeCarrierHierarchy::addListItem(GNEAttributeCarrier* AC, FXTreeItem* itemParent, std::string prefix, std::string sufix) {
    // insert item in Tree list
    FXTreeItem* item = myTreelist->insertItem(nullptr, itemParent, (prefix + AC->getHierarchyName() + sufix).c_str(), AC->getIcon(), AC->getIcon());
    // insert item in map
    myTreeItemToACMap[item] = AC;
    // by default item is expanded
    item->setExpanded(true);
    // return created FXTreeItem
    return item;
}


FXTreeItem*
GNEFrameHelper::AttributeCarrierHierarchy::addListItem(FXTreeItem* itemParent, const std::string& text, FXIcon* icon, bool expanded) {
    // insert item in Tree list
    FXTreeItem* item = myTreelist->insertItem(nullptr, itemParent, text.c_str(), icon, icon);
    // expand item depending of flag expanded
    item->setExpanded(expanded);
    // return created FXTreeItem
    return item;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::GenericParametersEditor - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::GenericParametersEditor::GenericParametersEditor(GNEFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Generic parameters", GUIDesignGroupBoxFrame),
    myFrameParent(inspectorFrameParent),
    myAC(nullptr),
    myGenericParameters(nullptr) {
    // create empty vector with generic parameters
    myGenericParameters = new std::vector<std::pair<std::string, std::string> >;
    // create textfield and buttons
    myTextFieldGenericParameter = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEditGenericParameterButton = new FXButton(this, "Edit generic parameter", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrameHelper::GenericParametersEditor::~GenericParametersEditor() {
    delete myGenericParameters;
}


void
GNEFrameHelper::GenericParametersEditor::showGenericParametersEditor(GNEAttributeCarrier* AC) {
    if (AC != nullptr) {
        myAC = AC;
        myACs.clear();
        // obtain a copy of generic parameters of AC
        if (myAC) {
            *myGenericParameters = myAC->getGenericParameters();
        }
        // refresh GenericParametersEditor
        refreshGenericParametersEditor();
        // show groupbox
        show();
    }
}


void
GNEFrameHelper::GenericParametersEditor::showGenericParametersEditor(std::vector<GNEAttributeCarrier*> ACs) {
    if (ACs.size() > 0) {
        myAC = nullptr;
        myACs = ACs;
        // check if generic parameters are different
        bool differentsGenericParameters = false;
        std::string genericParameter = myACs.front()->getAttribute(GNE_ATTR_GENERIC);
        for (auto i : myACs) {
            if (genericParameter != i->getAttribute(GNE_ATTR_GENERIC)) {
                differentsGenericParameters = true;
            }
        }
        // set generic Parameters editor
        if (differentsGenericParameters) {
            myGenericParameters->clear();
        } else {
            *myGenericParameters = myACs.front()->getGenericParameters();
        }
        // refresh GenericParametersEditor
        refreshGenericParametersEditor();
        // show groupbox
        show();
    }
}


void
GNEFrameHelper::GenericParametersEditor::hideGenericParametersEditor() {
    myAC = nullptr;
    // hide groupbox
    hide();
}


void
GNEFrameHelper::GenericParametersEditor::refreshGenericParametersEditor() {
    // update text field depending of AC
    if (myAC) {
        myTextFieldGenericParameter->setText(myAC->getAttribute(GNE_ATTR_GENERIC).c_str());
        myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
        // disable myTextFieldGenericParameter if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
        if (((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myAC->getTagProperty().isDemandElement()) ||
                ((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myAC->getTagProperty().isDemandElement())) {
            myTextFieldGenericParameter->disable();
            myEditGenericParameterButton->disable();
        } else {
            myTextFieldGenericParameter->enable();
            myEditGenericParameterButton->enable();
        }
    } else if (myACs.size() > 0) {
        // check if generic parameters of all inspected ACs are different
        std::string genericParameter = myACs.front()->getAttribute(GNE_ATTR_GENERIC);
        for (auto i : myACs) {
            if (genericParameter != i->getAttribute(GNE_ATTR_GENERIC)) {
                genericParameter = "different generic attributes";
            }
        }
        myTextFieldGenericParameter->setText(genericParameter.c_str());
        myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
        // disable myTextFieldGenericParameter if we're in demand mode and inspected AC isn't a demand element (or viceversa)
        if (((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myACs.front()->getTagProperty().isDemandElement()) ||
                ((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myACs.front()->getTagProperty().isDemandElement())) {
            myTextFieldGenericParameter->disable();
            myEditGenericParameterButton->disable();
        } else {
            myTextFieldGenericParameter->enable();
            myEditGenericParameterButton->enable();
        }
    }
}


std::string
GNEFrameHelper::GenericParametersEditor::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
        result += i->first + "=" + i->second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


long
GNEFrameHelper::GenericParametersEditor::onCmdEditGenericParameter(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open generic parameter dialog");
    // edit generic parameters using dialog
    if (GNEGenericParameterDialog(myFrameParent->myViewNet, myGenericParameters).execute()) {
        // write debug information
        WRITE_DEBUG("Close generic parameter dialog");
        // set values edited in Parameter dialog in Edited AC
        if (myAC) {
            myAC->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->myViewNet->getUndoList());
        } else if (myACs.size() > 0) {
            myFrameParent->myViewNet->getUndoList()->p_begin("Change multiple generic attributes");
            for (auto i : myACs) {
                i->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->myViewNet->getUndoList());
            }
            myFrameParent->myViewNet->getUndoList()->p_end();
            // update frame parent after attribute sucesfully set
            myFrameParent->attributeUpdated();
        }
        // Refresh parameter editor
        refreshGenericParametersEditor();
    } else {
        // write debug information
        WRITE_DEBUG("Cancel generic parameter dialog");
    }
    return 1;
}


long
GNEFrameHelper::GenericParametersEditor::onCmdSetGenericParameter(FXObject*, FXSelector, void*) {
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer st(myTextFieldGenericParameter->getText().text(), "|", true);
    while (st.hasNext()) {
        parsedValues.push_back(st.next());
    }
    // first check if parsed generic parameters are valid
    for (auto i : parsedValues) {
        if (!GNEAttributeCarrier::isGenericParametersValid(i)) {
            WRITE_WARNING("Invalid format of Generic Parameter (" + i + ")");
            myTextFieldGenericParameter->setTextColor(FXRGB(255, 0, 0));
            return 1;
        }
    }
    // now check if there is duplicated parameters
    std::sort(parsedValues.begin(), parsedValues.end());
    for (auto i = parsedValues.begin(); i != parsedValues.end(); i++) {
        if (((i + 1) != parsedValues.end())) {
            std::vector<std::string> firstKey, secondKey;
            StringTokenizer stKey1(*i, "=", true);
            StringTokenizer stKey2(*(i + 1), "=", true);
            //parse both keys
            while (stKey1.hasNext()) {
                firstKey.push_back(stKey1.next());
            }
            while (stKey2.hasNext()) {
                secondKey.push_back(stKey2.next());
            }
            // compare both keys and stop if are equal
            if ((firstKey.size() != 2) || (secondKey.size() != 2) || (firstKey.front() == secondKey.front())) {
                WRITE_WARNING("Generic Parameters wit the same key aren't allowed (" + (*i) + "," + * (i + 1) + ")");
                myTextFieldGenericParameter->setTextColor(FXRGB(255, 0, 0));
                return 1;
            }
        }
    }
    // parsed generic parameters ok, then set text field black and continue
    myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
    myTextFieldGenericParameter->killFocus();
    // clear current existent generic parameters and set parsed generic parameters
    myGenericParameters->clear();
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            myGenericParameters->push_back(std::make_pair(parsedParameters.front(), parsedParameters.back()));
        }
    }
    // if we're editing generic attributes of an AttributeCarrier, set it
    if (myAC) {
        myAC->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->myViewNet->getUndoList());
    } else if (myACs.size() > 0) {
        myFrameParent->myViewNet->getUndoList()->p_begin("Change multiple generic attributes");
        for (auto i : myACs) {
            i->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->myViewNet->getUndoList());
        }
        myFrameParent->myViewNet->getUndoList()->p_end();
        // update frame parent after attribute sucesfully set
        myFrameParent->attributeUpdated();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::DrawingShape - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::DrawingShape::DrawingShape(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Drawing", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myDeleteLastCreatedPoint(false) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(this, "Start drawing", 0, this, MID_GNE_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(this, "Stop drawing", 0, this, MID_GNE_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(this, "Abort drawing", 0, this, MID_GNE_ABORTDRAWING, GUIDesignButton);

    // create information label
    std::ostringstream information;
    information
            << "- 'Start drawing' or ENTER\n"
            << "  draws shape boundary.\n"
            << "- 'Stop drawing' or ENTER\n"
            << "  creates shape.\n"
            << "- 'Shift + Click'\n"
            << "  removes last created point.\n"
            << "- 'Abort drawing' or ESC\n"
            << "  removes drawed shape.";
    myInformationLabel = new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEFrameHelper::DrawingShape::~DrawingShape() {}


void GNEFrameHelper::DrawingShape::showDrawingShape() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEFrameHelper::DrawingShape::hideDrawingShape() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrameHelper::DrawingShape::startDrawing() {
    // Only start drawing if DrawingShape modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrameHelper::DrawingShape::stopDrawing() {
    // try to build shape
    if (myFrameParent->shapeDrawed()) {
        // clear created points
        myTemporalShapeShape.clear();
        myFrameParent->myViewNet->update();
        // change buttons
        myStartDrawingButton->enable();
        myStopDrawingButton->disable();
        myAbortDrawingButton->disable();
    } else {
        // abort drawing if shape cannot be created
        abortDrawing();
    }
}


void
GNEFrameHelper::DrawingShape::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->myViewNet->update();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrameHelper::DrawingShape::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrameHelper::DrawingShape::removeLastPoint() {

}


const PositionVector&
GNEFrameHelper::DrawingShape::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrameHelper::DrawingShape::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEFrameHelper::DrawingShape::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEFrameHelper::DrawingShape::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEFrameHelper::DrawingShape::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrameHelper::DrawingShape::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrameHelper::DrawingShape::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEFrameHelper::NeteditAttributes::NeteditAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myCurrentLengthValid(true),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");
    // Create Frame for Length Label and textField
    FXHorizontalFrame* lengthFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLengthLabel = new FXLabel(lengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelAttribute);
    myLengthTextField = new FXTextField(lengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockMovementLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myBlockMovementCheckButton->setCheck(false);
    // Create Frame for block shape label and checkBox (By default disabled)
    FXHorizontalFrame* blockShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockShapeLabel = new FXLabel(blockShapeFrame, "block shape", 0, GUIDesignLabelAttribute);
    myBlockShapeCheckButton = new FXCheckButton(blockShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // Create Frame for block close polygon and checkBox (By default disabled)
    FXHorizontalFrame* closePolygonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myClosePolygonLabel = new FXLabel(closePolygonFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myCloseShapeCheckButton = new FXCheckButton(closePolygonFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myBlockShapeCheckButton->setCheck(false);
    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEFrameHelper::NeteditAttributes::~NeteditAttributes() {}


void
GNEFrameHelper::NeteditAttributes::showNeteditAttributesModul(const GNEAttributeCarrier::TagProperties& tagProperty) {
    // we assume that frame will not be show
    bool showFrame = false;
    // check if lenght text field has to be showed
    if (tagProperty.canMaskStartEndPos()) {
        myLengthLabel->show();
        myLengthTextField->show();
        myReferencePointMatchBox->show();
        showFrame = true;
    } else {
        myLengthLabel->hide();
        myLengthTextField->hide();
        myReferencePointMatchBox->hide();
    }
    // check if block movement check button has to be show
    if (tagProperty.canBlockMovement()) {
        myBlockMovementLabel->show();
        myBlockMovementCheckButton->show();
        showFrame = true;
    } else {
        myBlockMovementLabel->hide();
        myBlockMovementCheckButton->hide();
    }
    // check if block shape check button has to be show
    if (tagProperty.canBlockShape()) {
        myBlockShapeLabel->show();
        myBlockShapeCheckButton->show();
        showFrame = true;
    } else {
        myBlockShapeLabel->hide();
        myBlockShapeCheckButton->hide();
    }
    // check if close shape check button has to be show
    if (tagProperty.canCloseShape()) {
        myClosePolygonLabel->show();
        myCloseShapeCheckButton->show();
        showFrame = true;
    } else {
        myClosePolygonLabel->hide();
        myCloseShapeCheckButton->hide();
    }
    // if at least one element is show, show modul
    if (showFrame) {
        show();
    } else {
        hide();
    }
}


void
GNEFrameHelper::NeteditAttributes::hideNeteditAttributesModul() {
    hide();
}


bool
GNEFrameHelper::NeteditAttributes::getNeteditAttributesAndValues(std::map<SumoXMLAttr, std::string>& valuesMap, const GNELane* lane) const {
    // check if we need to obtain a start and end position over an edge
    if (myReferencePointMatchBox->shown()) {
        // we need a valid lane to calculate position over lane
        if (lane == nullptr) {
            return false;
        } else if (myCurrentLengthValid) {
            // Obtain position of the mouse over lane (limited over grid)
            double mousePositionOverLane = lane->getGeometry().shape.nearest_offset_to_point2D(myFrameParent->myViewNet->snapToActiveGrid(myFrameParent->myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
            // check if current reference point is valid
            if (myActualAdditionalReferencePoint == GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                std::string errorMessage = "Current selected reference point isn't valid";
                myFrameParent->myViewNet->setStatusBarText(errorMessage);
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(errorMessage);
                return false;
            } else {
                // obtain lenght
                double lenght = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
                // set start and end position
                valuesMap[SUMO_ATTR_STARTPOS] = toString(setStartPosition(mousePositionOverLane, lenght));
                valuesMap[SUMO_ATTR_ENDPOS] = toString(setEndPosition(mousePositionOverLane, lenght));
            }
        } else {
            return false;
        }
    }
    // Save block value if element can be blocked
    if (myBlockMovementCheckButton->shown()) {
        if (myBlockMovementCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "1";
        } else {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "0";
        }
    }
    // Save block shape value if shape's element can be blocked
    if (myBlockShapeCheckButton->shown()) {
        if (myBlockShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "1";
        } else {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "0";
        }
    }
    // Save close shape value if shape's element can be closed
    if (myCloseShapeCheckButton->shown()) {
        if (myCloseShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "1";
        } else {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "0";
        }
    }
    // all ok, then return true to continue creating element
    return true;
}


long
GNEFrameHelper::NeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myBlockMovementCheckButton) {
        if (myBlockMovementCheckButton->getCheck()) {
            myBlockMovementCheckButton->setText("true");
        } else {
            myBlockMovementCheckButton->setText("false");
        }
    } else if (obj == myBlockShapeCheckButton) {
        if (myBlockShapeCheckButton->getCheck()) {
            myBlockShapeCheckButton->setText("true");
        } else {
            myBlockShapeCheckButton->setText("false");
        }
    } else if (obj == myCloseShapeCheckButton) {
        if (myCloseShapeCheckButton->getCheck()) {
            myCloseShapeCheckButton->setText("true");
        } else {
            myCloseShapeCheckButton->setText("false");
        }
    } else if (obj == myLengthTextField) {
        // change color of text field depending of the input length
        if (GNEAttributeCarrier::canParse<double>(myLengthTextField->getText().text()) &&
                GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text()) > 0) {
            myLengthTextField->setTextColor(FXRGB(0, 0, 0));
            myLengthTextField->killFocus();
            myCurrentLengthValid = true;
        } else {
            myLengthTextField->setTextColor(FXRGB(255, 0, 0));
            myCurrentLengthValid = false;
        }
        // Update aditional frame
        update();
    } else if (obj == myReferencePointMatchBox) {
        // Cast actual reference point type
        if (myReferencePointMatchBox->getText() == "reference left") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_LEFT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference right") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_RIGHT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference center") {
            myLengthTextField->enable();
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_CENTER;
            myLengthTextField->enable();
        } else {
            myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_INVALID;
            myLengthTextField->disable();
        }
    }

    return 1;
}


long
GNEFrameHelper::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << "- Referece point: Mark the initial position of the additional element.\n"
            << "  Example: If you want to create a busStop with a length of 30 in the point 100 of the lane:\n"
            << "  - Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "  - Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "  - Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "- Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening NeteditAttributes help dialog");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing NeteditAttributes help dialog");
    return 1;
    /**********
    help from PolygonFrame
            << "- Block movement: If enabled, the created polygon element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Block shape: If enabled, the shape of created polygon element will be blocked. i.e. their geometry points\n"
            << "  cannot be edited be moved with the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Close shape: If enabled, the created polygon element will be closed. i.e. the last created geometry point\n"
            << "  will be connected with the first geometry point automatically. This option can be modified inspecting element.";

    ****************/
}


double
GNEFrameHelper::NeteditAttributes::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const {
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNEFrameHelper::NeteditAttributes::setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional)  const {
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

// ---------------------------------------------------------------------------
// GNEFrameHelper::SelectorParent - methods
// ---------------------------------------------------------------------------

GNEFrameHelper::SelectorParent::SelectorParent(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Parent selector", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myParentTag(SUMO_TAG_NOTHING) {
    // Create label with the type of SelectorParent
    myParentsLabel = new FXLabel(this, "No additional selected", nullptr, GUIDesignLabelLeftThick);
    // Create list
    myParentsList = new FXList(this, this, MID_GNE_SET_TYPE, GUIDesignListSingleElementFixedHeight);
    // Hide List
    hideSelectorParentModul();
}


GNEFrameHelper::SelectorParent::~SelectorParent() {}


std::string
GNEFrameHelper::SelectorParent::getIdSelected() const {
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->isItemSelected(i)) {
            return myParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEFrameHelper::SelectorParent::setIDSelected(const std::string& id) {
    // first unselect all
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        myParentsList->getItem(i)->setSelected(false);
    }
    // select element if correspond to given ID
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->getItem(i)->getText().text() == id) {
            myParentsList->getItem(i)->setSelected(true);
        }
    }
    // recalc myFirstParentsList
    myParentsList->recalc();
}


bool
GNEFrameHelper::SelectorParent::showSelectorParentModul(SumoXMLTag additionalType) {
    // make sure that we're editing an additional tag
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL, false);
    for (auto i : listOfTags) {
        if (i == additionalType) {
            myParentTag = additionalType;
            myParentsLabel->setText(("Parent type: " + toString(additionalType)).c_str());
            refreshSelectorParentModul();
            show();
            return true;
        }
    }
    return false;
}


void
GNEFrameHelper::SelectorParent::hideSelectorParentModul() {
    myParentTag = SUMO_TAG_NOTHING;
    hide();
}


void
GNEFrameHelper::SelectorParent::refreshSelectorParentModul() {
    myParentsList->clearItems();
    if (myParentTag != SUMO_TAG_NOTHING) {
        // fill list with IDs of additionals
        for (const auto &i : myFrameParent->getViewNet()->getNet()->getAttributeCarriers().additionals.at(myParentTag)) {
            myParentsList->appendItem(i.first.c_str());
        }
    }
}

/****************************************************************************/
