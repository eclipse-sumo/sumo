/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrameModuls.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Moduls
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Children.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/data/GNEDataSet.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFrameModuls.h"



// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameModuls::TagSelector) TagSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAGTYPE_SELECTED,    GNEFrameModuls::TagSelector::onCmdSelectTagType),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAG_SELECTED,        GNEFrameModuls::TagSelector::onCmdSelectTag)
};

FXDEFMAP(GNEFrameModuls::DemandElementSelector) DemandElementSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEFrameModuls::DemandElementSelector::onCmdSelectDemandElement),
};

FXDEFMAP(GNEFrameModuls::EdgePathCreator) EdgePathCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_ABORT,      GNEFrameModuls::EdgePathCreator::onCmdAbortRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_FINISH,     GNEFrameModuls::EdgePathCreator::onCmdFinishRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_REMOVELAST, GNEFrameModuls::EdgePathCreator::onCmdRemoveLastInsertedElement)
};

FXDEFMAP(GNEFrameModuls::AttributeCarrierHierarchy) AttributeCarrierHierarchyMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_CENTER,                     GNEFrameModuls::AttributeCarrierHierarchy::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECT,                    GNEFrameModuls::AttributeCarrierHierarchy::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_DELETE,                     GNEFrameModuls::AttributeCarrierHierarchy::onCmdDeleteItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_ACHIERARCHY_MOVEUP,         GNEFrameModuls::AttributeCarrierHierarchy::onCmdMoveItemUp),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_ACHIERARCHY_MOVEDOWN,       GNEFrameModuls::AttributeCarrierHierarchy::onCmdMoveItemDown),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_ACHIERARCHY_SHOWCHILDMENU,  GNEFrameModuls::AttributeCarrierHierarchy::onCmdShowChildMenu)
};

FXDEFMAP(GNEFrameModuls::DrawingShape) DrawingShapeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STARTDRAWING,   GNEFrameModuls::DrawingShape::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STOPDRAWING,    GNEFrameModuls::DrawingShape::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORTDRAWING,   GNEFrameModuls::DrawingShape::onCmdAbortDrawing)
};

FXDEFMAP(GNEFrameModuls::OverlappedInspection) OverlappedInspectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_NEXT,            GNEFrameModuls::OverlappedInspection::onCmdNextElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_PREVIOUS,        GNEFrameModuls::OverlappedInspection::onCmdPreviousElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_SHOWLIST,        GNEFrameModuls::OverlappedInspection::onCmdShowList),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_ITEMSELECTED,    GNEFrameModuls::OverlappedInspection::onCmdListItemSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                           GNEFrameModuls::OverlappedInspection::onCmdOverlappingHelp)
};

// Object implementation
FXIMPLEMENT(GNEFrameModuls::TagSelector,                FXGroupBox,     TagSelectorMap,                 ARRAYNUMBER(TagSelectorMap))
FXIMPLEMENT(GNEFrameModuls::DemandElementSelector,      FXGroupBox,     DemandElementSelectorMap,       ARRAYNUMBER(DemandElementSelectorMap))
FXIMPLEMENT(GNEFrameModuls::EdgePathCreator,            FXGroupBox,     EdgePathCreatorMap,             ARRAYNUMBER(EdgePathCreatorMap))
FXIMPLEMENT(GNEFrameModuls::AttributeCarrierHierarchy,  FXGroupBox,     AttributeCarrierHierarchyMap,   ARRAYNUMBER(AttributeCarrierHierarchyMap))
FXIMPLEMENT(GNEFrameModuls::DrawingShape,               FXGroupBox,     DrawingShapeMap,                ARRAYNUMBER(DrawingShapeMap))
FXIMPLEMENT(GNEFrameModuls::OverlappedInspection,       FXGroupBox,     OverlappedInspectionMap,        ARRAYNUMBER(OverlappedInspectionMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameModuls::TagSelector - methods
// ---------------------------------------------------------------------------

GNEFrameModuls::TagSelector::TagSelector(GNEFrame* frameParent, GNETagProperties::TagType type, bool onlyDrawables) :
    FXGroupBox(frameParent->myContentFrame, "Element", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // first check that property is valid
    switch (type) {
        case GNETagProperties::TagType::NETWORKELEMENT:
            setText("network elements");
            break;
        case GNETagProperties::TagType::ADDITIONALELEMENT:
            setText("Additional elements");
            break;
        case GNETagProperties::TagType::SHAPE:
            setText("Shape elements");
            break;
        case GNETagProperties::TagType::TAZ:
            setText("TAZ elements");
            break;
        case GNETagProperties::TagType::VEHICLE:
            setText("Vehicles");
            break;
        case GNETagProperties::TagType::STOP:
            setText("Stops");
            break;
        case GNETagProperties::TagType::PERSON:
            setText("Persons");
            break;
        case GNETagProperties::TagType::PERSONPLAN:
            setText("Person plans");
            // person plan type has four sub-groups
            myListOfTagTypes.push_back(std::make_pair("person trips", GNETagProperties::TagType::PERSONTRIP));
            myListOfTagTypes.push_back(std::make_pair("walks", GNETagProperties::TagType::WALK));
            myListOfTagTypes.push_back(std::make_pair("rides", GNETagProperties::TagType::RIDE));
            myListOfTagTypes.push_back(std::make_pair("stops", GNETagProperties::TagType::PERSONSTOP));
            break;
        case GNETagProperties::TagType::PERSONTRIP:
            setText("Person trips");
            break;
        case GNETagProperties::TagType::WALK:
            setText("Walks");
            break;
        case GNETagProperties::TagType::RIDE:
            setText("Rides");
            break;
        case GNETagProperties::TagType::PERSONSTOP:
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
        myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::PERSONTRIP, onlyDrawables);
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


GNEFrameModuls::TagSelector::~TagSelector() {}


void
GNEFrameModuls::TagSelector::showTagSelector() {
    show();
}


void
GNEFrameModuls::TagSelector::hideTagSelector() {
    hide();
}


const GNETagProperties&
GNEFrameModuls::TagSelector::getCurrentTagProperties() const {
    return myCurrentTagProperties;
}


void
GNEFrameModuls::TagSelector::setCurrentTagType(GNETagProperties::TagType tagType) {
    // set empty tag properties
    myCurrentTagProperties = GNETagProperties();
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTagsMatchBox->getNumItems(); i++) {
        if (myTagsMatchBox->getItem(i).text() == toString(tagType)) {
            myTagsMatchBox->setCurrentItem(i);
            // fill myListOfTags with personTrips (the first Tag Type)
            myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::PERSONTRIP, true);
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
GNEFrameModuls::TagSelector::setCurrentTag(SumoXMLTag newTag) {
    // set empty tag properties
    myCurrentTagProperties = GNETagProperties();
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
GNEFrameModuls::TagSelector::refreshTagProperties() {
    // simply call onCmdSelectItem (to avoid duplicated code)
    onCmdSelectTag(0, 0, 0);
}


long GNEFrameModuls::TagSelector::onCmdSelectTagType(FXObject*, FXSelector, void*) {
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
GNEFrameModuls::TagSelector::onCmdSelectTag(FXObject*, FXSelector, void*) {
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
// GNEFrameModuls::DemandElementSelector - methods
// ---------------------------------------------------------------------------

GNEFrameModuls::DemandElementSelector::DemandElementSelector(GNEFrame* frameParent, SumoXMLTag demandElementTag) :
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


GNEFrameModuls::DemandElementSelector::DemandElementSelector(GNEFrame* frameParent, const std::vector<GNETagProperties::TagType>& tagTypes) :
    FXGroupBox(frameParent->myContentFrame, "Parent element", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myCurrentDemandElement(nullptr) {
    // fill myDemandElementTags
    for (const auto& i : tagTypes) {
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


GNEFrameModuls::DemandElementSelector::~DemandElementSelector() {}


GNEDemandElement*
GNEFrameModuls::DemandElementSelector::getCurrentDemandElement() const {
    return myCurrentDemandElement;
}


const std::vector<SumoXMLTag>&
GNEFrameModuls::DemandElementSelector::getAllowedTags() const {
    return myDemandElementTags;
}

void
GNEFrameModuls::DemandElementSelector::setDemandElement(GNEDemandElement* demandElement) {
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
GNEFrameModuls::DemandElementSelector::showDemandElementSelector() {
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
GNEFrameModuls::DemandElementSelector::hideDemandElementSelector() {
    hide();
}


bool
GNEFrameModuls::DemandElementSelector::isDemandElementSelectorShown() const {
    return shown();
}


void
GNEFrameModuls::DemandElementSelector::refreshDemandElementSelector() {
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
            for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(i)) {
                // avoid insert duplicated default vType
                if ((j.first != DEFAULT_VTYPE_ID) && (j.first != DEFAULT_BIKETYPE_ID)) {
                    myDemandElementsMatchBox->appendItem(j.first.c_str());
                }
            }
        } else if (i == SUMO_TAG_PTYPE) {
            // add default Person type in the firs
            myDemandElementsMatchBox->appendItem(DEFAULT_PEDTYPE_ID.c_str());
            // add rest of pTypes
            for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(i)) {
                // avoid insert duplicated default pType
                if (j.first != DEFAULT_PEDTYPE_ID) {
                    myDemandElementsMatchBox->appendItem(j.first.c_str());
                }
            }
        } else {
            // insert all Ids
            for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(i)) {
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
            myCurrentDemandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE).at(DEFAULT_VTYPE_ID);
        } else if (myDemandElementsMatchBox->getItem(0).text() == DEFAULT_PEDTYPE_ID) {
            myCurrentDemandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PTYPE).at(DEFAULT_PEDTYPE_ID);
        } else {
            // disable myCurrentDemandElement
            myCurrentDemandElement = nullptr;
            // update myCurrentDemandElement with the first allowed element
            for (auto i = myDemandElementTags.begin(); (i != myDemandElementTags.end()) && (myCurrentDemandElement == nullptr); i++) {
                if (myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(*i).size() > 0) {
                    myCurrentDemandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(*i).begin()->second;
                }
            }
        }
    }
}


long
GNEFrameModuls::DemandElementSelector::onCmdSelectDemandElement(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond to a demand element
    for (const auto& i : myDemandElementTags) {
        for (const auto& j : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(i)) {
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
// GNEFrameModuls::EdgePathCreator - methods
// ---------------------------------------------------------------------------

GNEFrameModuls::EdgePathCreator::EdgePathCreator(GNEFrame* frameParent, int edgePathCreatorModes) :
    FXGroupBox(frameParent->myContentFrame, "Path creator", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myVClass(SVC_PASSENGER),
    mySelectedBusStop(nullptr),
    myModes(edgePathCreatorModes) {

    // create button for create GEO POIs
    myFinishCreationButton = new FXButton(this, "Finish path creation", nullptr, this, MID_GNE_EDGEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();

    // create button for create GEO POIs
    myAbortCreationButton = new FXButton(this, "Abort path creation", nullptr, this, MID_GNE_EDGEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();

    // create button for create GEO POIs
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_EDGEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
}


GNEFrameModuls::EdgePathCreator::~EdgePathCreator() {}


void
GNEFrameModuls::EdgePathCreator::edgePathCreatorName(const std::string& name) {
    // header needs the first capitalized letter
    std::string nameWithFirstCapitalizedLetter = name;
    nameWithFirstCapitalizedLetter[0] = (char)toupper(nameWithFirstCapitalizedLetter.at(0));
    setText((nameWithFirstCapitalizedLetter + " creator").c_str());
    myFinishCreationButton->setText(("Finish " + name + " creation").c_str());
    myAbortCreationButton->setText(("Abort " + name + " creation").c_str());
}


void
GNEFrameModuls::EdgePathCreator::showEdgePathCreator() {
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    // show modul
    show();
}


void
GNEFrameModuls::EdgePathCreator::hideEdgePathCreator() {
    // restore colors
    for (const auto& i : myClickedEdges) {
        restoreEdgeColor(i);
    }
    // clear edges
    myClickedEdges.clear();
    // clear myTemporalEdgePath
    myTemporalRoute.clear();
    // hide modul
    hide();
}


void
GNEFrameModuls::EdgePathCreator::setVClass(SUMOVehicleClass vClass) {
    myVClass = vClass;
}


void
GNEFrameModuls::EdgePathCreator::setModes(int edgePathCreatorModes) {
    myModes = edgePathCreatorModes;
}


std::vector<GNEEdge*>
GNEFrameModuls::EdgePathCreator::getClickedEdges() const {
    return myClickedEdges;
}


GNEAdditional*
GNEFrameModuls::EdgePathCreator::getClickedBusStop() const {
    return mySelectedBusStop;
}


bool
GNEFrameModuls::EdgePathCreator::addPathEdge(GNEEdge* edge) {
    bool addEdge = true;
    // check if final busStop was selected
    if (mySelectedBusStop != nullptr) {
        addEdge = false;
        // write status bar text
        myFrameParent->getViewNet()->setStatusBarText("Final " + mySelectedBusStop->getTagProperty().getTagStr() + " selected");
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG("Final " + mySelectedBusStop->getTagProperty().getTagStr() + " selected");
    } else if ((myClickedEdges.size() > 0) && (myClickedEdges.back() == edge)) {
        // avoid duplicated consecutive edges
        addEdge = false;
        // write status bar text
        myFrameParent->getViewNet()->setStatusBarText("Duplicated consecutive edges aren't allowed");
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG("Duplicated consecutive edges aren't allowed");
    }
    // check permissions
    if (addEdge) {
        addEdge = false;
        for (const auto& i : edge->getNBEdge()->getLanes()) {
            if ((i.permissions & myVClass) != 0) {
                addEdge = true;
            }
        }
        if (addEdge == false) {
            // write status bar text
            myFrameParent->getViewNet()->setStatusBarText("Invalid edge permissions");
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG("Invalid edge permissions");
        }
    }
    // check if edge can be added
    if (addEdge) {
        // insert edge in myClickedEdges
        myClickedEdges.push_back(edge);
        // enable abort route button
        myAbortCreationButton->enable();
        // disable undo/redo
        myFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("trip creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myFrameParent->getEdgeCandidateSelectedColor());
        }
        // enable remove last edge button
        myRemoveLastInsertedEdge->enable();
        // enable finish button
        myFinishCreationButton->enable();
        // calculate route if there is more than two edges
        if (myClickedEdges.size() > 1) {
            // calculate temporal route
            myTemporalRoute = myFrameParent->getViewNet()->getNet()->getPathCalculator()->calculatePath(myVClass, myClickedEdges);
        } else {
            // use single edge as temporal route
            myTemporalRoute = myClickedEdges;
        }
        return true;
    } else {
        return false;
    }
}


bool
GNEFrameModuls::EdgePathCreator::addBusStop(GNEAdditional* busStop) {
    // check that at least there is a selected edge
    if (!myClickedEdges.empty() && (mySelectedBusStop == nullptr)) {
        mySelectedBusStop = busStop;
        mySelectedBusStop->setSpecialColor(&myFrameParent->getEdgeCandidateSelectedColor());
    }
    return false;
}


void
GNEFrameModuls::EdgePathCreator::clearEdges() {
    // restore colors
    for (const auto& i : myClickedEdges) {
        restoreEdgeColor(i);
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
GNEFrameModuls::EdgePathCreator::drawTemporalPath() const {
    // draw depending of number of edges
    if (myClickedEdges.size() > 0) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // we have two possibilites, depending of myTemporalRoute
        if (myTemporalRoute.empty()) {
            // draw first line
            GLHelper::drawLine(myClickedEdges.at(0)->getNBEdge()->getLanes().front().shape.front(),
                               myClickedEdges.at(0)->getNBEdge()->getLanes().front().shape.back());
            // draw rest of lines
            for (int i = 1; i < (int)myClickedEdges.size(); i++) {
                GLHelper::drawLine(myClickedEdges.at(i - 1)->getNBEdge()->getLanes().front().shape.back(),
                                   myClickedEdges.at(i)->getNBEdge()->getLanes().front().shape.front());
                GLHelper::drawLine(myClickedEdges.at(i)->getNBEdge()->getLanes().front().shape.front(),
                                   myClickedEdges.at(i)->getNBEdge()->getLanes().front().shape.back());
            }
            // draw a line to center of selected bus
            if (mySelectedBusStop) {
                GLHelper::drawLine(myClickedEdges.back()->getNBEdge()->getLanes().front().shape.back(),
                                   mySelectedBusStop->getAdditionalGeometry().getShape().getLineCenter());
            }
        } else {
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
                                   mySelectedBusStop->getAdditionalGeometry().getShape().getLineCenter());
            }
        }
        // Pop last matrix
        glPopMatrix();
    }
}


void
GNEFrameModuls::EdgePathCreator::abortEdgePathCreation() {
    if (myAbortCreationButton->isEnabled()) {
        onCmdAbortRouteCreation(nullptr, 0, nullptr);
    }
}


void
GNEFrameModuls::EdgePathCreator::finishEdgePathCreation() {
    if (myFinishCreationButton->isEnabled()) {
        onCmdFinishRouteCreation(nullptr, 0, nullptr);
    }
}


void
GNEFrameModuls::EdgePathCreator::removeLastInsertedElement() {
    if (myRemoveLastInsertedEdge->isEnabled()) {
        onCmdRemoveLastInsertedElement(nullptr, 0, nullptr);
    }
}


long
GNEFrameModuls::EdgePathCreator::onCmdAbortRouteCreation(FXObject*, FXSelector, void*) {
    // clear edges
    clearEdges();
    // disable buttons
    myAbortCreationButton->disable();
    myFinishCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    // update view
    myFrameParent->myViewNet->updateViewNet();
    return 1;
}


long
GNEFrameModuls::EdgePathCreator::onCmdFinishRouteCreation(FXObject*, FXSelector, void*) {
    // only create route if there is more than two edges
    if (myClickedEdges.size() > 0) {
        // depending of tag, check if last element is a busStop
        if ((myModes == TO_BUSSTOP) && (mySelectedBusStop == nullptr)) {
            WRITE_WARNING("Last clicked element must be a " + toString(SUMO_TAG_BUS_STOP));
            return 1;
        }
        // call edgePathCreated
        myFrameParent->edgePathCreated();
        // update view
        myFrameParent->myViewNet->updateViewNet();
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
GNEFrameModuls::EdgePathCreator::onCmdRemoveLastInsertedElement(FXObject*, FXSelector, void*) {
    if (myClickedEdges.size() > 1) {
        // restore color of last clicked edge
        restoreEdgeColor(myClickedEdges.back());
        // remove last edge
        myClickedEdges.pop_back();
        // calculate temporal route
        myTemporalRoute = myFrameParent->getViewNet()->getNet()->getPathCalculator()->calculatePath(myVClass, myClickedEdges);
        // update view (to see the new temporal route)
        myFrameParent->myViewNet->updateViewNet();
        // check if after pop edge, there is more than one edge
        if (myClickedEdges.size() == 1) {
            // disable remove last edge button
            myRemoveLastInsertedEdge->disable();
        }
    }
    return 1;
}


void
GNEFrameModuls::EdgePathCreator::restoreEdgeColor(const GNEEdge* edge) {
    // restore color of every lane
    for (const auto& i : edge->getLanes()) {
        i->setSpecialColor(nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNEFrameModuls::AttributeCarrierHierarchy - methods
// ---------------------------------------------------------------------------

GNEFrameModuls::AttributeCarrierHierarchy::AttributeCarrierHierarchy(GNEFrame* frameParent) :
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
    myClickedDemandElement(nullptr),
    myClickedDataSet(nullptr),
    myClickedDataInterval(nullptr),
    myClickedGenericData(nullptr) {
    // Create three list
    myTreelist = new FXTreeList(this, this, MID_GNE_ACHIERARCHY_SHOWCHILDMENU, GUIDesignTreeListFrame);
    hide();
}


GNEFrameModuls::AttributeCarrierHierarchy::~AttributeCarrierHierarchy() {}


void
GNEFrameModuls::AttributeCarrierHierarchy::showAttributeCarrierHierarchy(GNEAttributeCarrier* AC) {
    myAC = AC;
    // show AttributeCarrierHierarchy and refresh AttributeCarrierHierarchy
    if (myAC) {
        show();
        refreshAttributeCarrierHierarchy();
    }
}


void
GNEFrameModuls::AttributeCarrierHierarchy::hideAttributeCarrierHierarchy() {
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
    myClickedDataSet = nullptr;
    myClickedDataInterval = nullptr;
    myClickedGenericData = nullptr;
    // hide modul
    hide();
}


void
GNEFrameModuls::AttributeCarrierHierarchy::refreshAttributeCarrierHierarchy() {
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsConnections.clear();
    // show ACChildren of myAC
    if (myAC) {
        showAttributeCarrierChildren(myAC, showAttributeCarrierParents());
    }
}


void 
GNEFrameModuls::AttributeCarrierHierarchy::removeCurrentEditedAttribute(const GNEAttributeCarrier* AC) {
    // simply check if AC is the same of myAC
    if (AC == myAC) {
        myAC = nullptr;
    }
}


long
GNEFrameModuls::AttributeCarrierHierarchy::onCmdShowChildMenu(FXObject*, FXSelector, void* eventData) {
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
GNEFrameModuls::AttributeCarrierHierarchy::onCmdCenterItem(FXObject*, FXSelector, void*) {
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
    } else if (myClickedGenericData) {
        myFrameParent->myViewNet->centerTo(myClickedGenericData->getGlID(), true, -1);
    }
    // update view after centering
    myFrameParent->myViewNet->updateViewNet();
    return 1;
}


long
GNEFrameModuls::AttributeCarrierHierarchy::onCmdInspectItem(FXObject*, FXSelector, void*) {
    if ((myAC != nullptr) && (myClickedAC != nullptr)) {
        myFrameParent->myViewNet->getViewParent()->getInspectorFrame()->inspectChild(myClickedAC, myAC);
    }
    return 1;
}


long
GNEFrameModuls::AttributeCarrierHierarchy::onCmdDeleteItem(FXObject*, FXSelector, void*) {
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
            WRITE_WARNING("Default Vehicle Type '" + myClickedDemandElement->getAttribute(SUMO_ATTR_ID) + "' cannot be removed");
            return 1;
        } else if (myClickedDemandElement->getTagProperty().isPersonPlan() && (myClickedDemandElement->getParentDemandElements().front()->getChildDemandElements().size() == 1)) {
            // we need to check if we're removing the last person plan of a person.
            myFrameParent->myViewNet->getNet()->deleteDemandElement(myClickedDemandElement->getParentDemandElements().front(), myFrameParent->myViewNet->getUndoList());
        } else {
            myFrameParent->myViewNet->getNet()->deleteDemandElement(myClickedDemandElement, myFrameParent->myViewNet->getUndoList());
        }
    } else if (myClickedDataSet) {
        myFrameParent->myViewNet->getNet()->deleteDataSet(myClickedDataSet, myFrameParent->myViewNet->getUndoList());
    } else if (myClickedDataInterval) {
        // check if we have to remove data Set
        if (myClickedDataInterval->getDataSetParent()->getDataIntervalChildren().size() == 1) {
            myFrameParent->myViewNet->getNet()->deleteDataSet(myClickedDataInterval->getDataSetParent(), myFrameParent->myViewNet->getUndoList());
        } else {
            myFrameParent->myViewNet->getNet()->deleteDataInterval(myClickedDataInterval, myFrameParent->myViewNet->getUndoList());
        }
    } else if (myClickedGenericData) {
        // check if we have to remove interval
        if (myClickedGenericData->getDataIntervalParent()->getGenericDataChildren().size() == 1) {
            // check if we have to remove data Set
            if (myClickedGenericData->getDataIntervalParent()->getDataSetParent()->getDataIntervalChildren().size() == 1) {
                myFrameParent->myViewNet->getNet()->deleteDataSet(myClickedGenericData->getDataIntervalParent()->getDataSetParent(), myFrameParent->myViewNet->getUndoList());
            } else {
                myFrameParent->myViewNet->getNet()->deleteDataInterval(myClickedGenericData->getDataIntervalParent(), myFrameParent->myViewNet->getUndoList());
            }
        } else {
            myFrameParent->myViewNet->getNet()->deleteGenericData(myClickedGenericData, myFrameParent->myViewNet->getUndoList());
        }
    }
    // update viewNet
    myFrameParent->myViewNet->updateViewNet();
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
GNEFrameModuls::AttributeCarrierHierarchy::onCmdMoveItemUp(FXObject*, FXSelector, void*) {
    // currently only children of demand elements can be moved
    if (myClickedDemandElement) {
        myFrameParent->myViewNet->getUndoList()->p_begin(("moving up " + myClickedDemandElement->getTagStr()).c_str());
        // move element one position back
        myFrameParent->myViewNet->getUndoList()->add(new GNEChange_Children(myClickedDemandElement->getParentDemandElements().at(0), myClickedDemandElement,
                GNEChange_Children::Operation::MOVE_BACK), true);
        myFrameParent->myViewNet->getUndoList()->p_end();
    }
    // refresh after moving child
    refreshAttributeCarrierHierarchy();
    return 1;
}


long
GNEFrameModuls::AttributeCarrierHierarchy::onCmdMoveItemDown(FXObject*, FXSelector, void*) {
    // currently only children of demand elements can be moved
    if (myClickedDemandElement) {
        myFrameParent->myViewNet->getUndoList()->p_begin(("moving down " + myClickedDemandElement->getTagStr()).c_str());
        // move element one position front
        myFrameParent->myViewNet->getUndoList()->add(new GNEChange_Children(myClickedDemandElement->getParentDemandElements().at(0), myClickedDemandElement,
                GNEChange_Children::Operation::MOVE_FRONT), true);
        myFrameParent->myViewNet->getUndoList()->p_end();
    }
    // refresh after moving child
    refreshAttributeCarrierHierarchy();
    return 1;
}


void
GNEFrameModuls::AttributeCarrierHierarchy::createPopUpMenu(int X, int Y, GNEAttributeCarrier* clickedAC) {
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
        myClickedDataSet = dynamic_cast<GNEDataSet*>(clickedAC);
        myClickedDataInterval = dynamic_cast<GNEDataInterval*>(clickedAC);
        myClickedGenericData = dynamic_cast<GNEGenericData*>(clickedAC);
        // create FXMenuPane
        FXMenuPane* pane = new FXMenuPane(myTreelist);
        // set item name and icon
        new MFXMenuHeader(pane, myFrameParent->myViewNet->getViewParent()->getGUIMainWindow()->getBoldFont(), myClickedAC->getPopUpID().c_str(), myClickedAC->getIcon());
        // insert separator
        new FXMenuSeparator(pane);
        // create center menu command
        FXMenuCommand* centerMenuCommand = new FXMenuCommand(pane, "Center", GUIIconSubSys::getIcon(GUIIcon::RECENTERVIEW), this, MID_GNE_CENTER);
        // disable Centering for Vehicle Types, data sets and data intervals
        if (myClickedAC->getTagProperty().isVehicleType() || (myClickedAC->getTagProperty().getTag() == SUMO_TAG_DATASET) ||
            (myClickedAC->getTagProperty().getTag() == SUMO_TAG_DATAINTERVAL)) {
            centerMenuCommand->disable();
        }
        // create inspect and delete menu commands
        FXMenuCommand* inspectMenuCommand = new FXMenuCommand(pane, "Inspect", GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), this, MID_GNE_INSPECT);
        FXMenuCommand* deleteMenuCommand = new FXMenuCommand(pane, "Delete", GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE);
        // check if inspect and delete menu commands has to be disabled
        if (GNEFrameAttributesModuls::isSupermodeValid(myFrameParent->myViewNet, myClickedAC) == false) {
            inspectMenuCommand->disable();
            deleteMenuCommand->disable();
        }
        // now chec if given AC support manually moving of their item up and down (Currently only for demand elements
        if (myClickedDemandElement && myClickedAC->getTagProperty().canBeSortedManually()) {
            // insert separator
            new FXMenuSeparator(pane);
            // create both moving menu commands
            FXMenuCommand* moveUpMenuCommand = new FXMenuCommand(pane, "Move up", GUIIconSubSys::getIcon(GUIIcon::ARROW_UP), this, MID_GNE_ACHIERARCHY_MOVEUP);
            FXMenuCommand* moveDownMenuCommand = new FXMenuCommand(pane, "Move down", GUIIconSubSys::getIcon(GUIIcon::ARROW_DOWN), this, MID_GNE_ACHIERARCHY_MOVEDOWN);
            // check if both commands has to be disabled
            if (myClickedDemandElement->getTagProperty().isPersonStop()) {
                moveUpMenuCommand->setText("Move up (Stops cannot be moved)");
                moveDownMenuCommand->setText("Move diwb (Stops cannot be moved)");
                moveUpMenuCommand->disable();
                moveDownMenuCommand->disable();
            } else {
                // check if moveUpMenuCommand has to be disabled
                if (myClickedDemandElement->getParentDemandElements().front()->getChildDemandElements().front() == myClickedDemandElement) {
                    moveUpMenuCommand->setText("Move up (It's already the first element)");
                    moveUpMenuCommand->disable();
                } else if (myClickedDemandElement->getParentDemandElements().front()->getPreviousChildDemandElement(myClickedDemandElement)->getTagProperty().isPersonStop()) {
                    moveUpMenuCommand->setText("Move up (Previous element is a Stop)");
                    moveUpMenuCommand->disable();
                }
                // check if moveDownMenuCommand has to be disabled
                if (myClickedDemandElement->getParentDemandElements().front()->getChildDemandElements().back() == myClickedDemandElement) {
                    moveDownMenuCommand->setText("Move down (It's already the last element)");
                    moveDownMenuCommand->disable();
                } else if (myClickedDemandElement->getParentDemandElements().front()->getNextChildDemandElement(myClickedDemandElement)->getTagProperty().isPersonStop()) {
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
        myClickedDataSet = nullptr;
        myClickedDataInterval = nullptr;
        myClickedGenericData = nullptr;
    }
}


FXTreeItem*
GNEFrameModuls::AttributeCarrierHierarchy::showAttributeCarrierParents() {
    if (myAC->getTagProperty().isNetworkElement()) {
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
                    // obtain parent edge
                    GNEEdge* edge = myFrameParent->myViewNet->getNet()->retrieveEdge(lane->getParentEdge()->getID());
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
        GNEShape* POILane = myFrameParent->myViewNet->getNet()->retrieveShape(SUMO_TAG_POILANE, myAC->getID(), false);
        if (POILane) {
            // obtain parent lane
            GNELane* lane = myFrameParent->myViewNet->getNet()->retrieveLane(POILane->getParentLanes().at(0)->getID());
            // obtain parent edge
            GNEEdge* edge = myFrameParent->myViewNet->getNet()->retrieveEdge(lane->getParentEdge()->getID());
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
    } else if (myAC->getTagProperty().isAdditionalElement() || myAC->getTagProperty().isTAZ()) {
        // Obtain Additional
        GNEAdditional* additional = myFrameParent->myViewNet->getNet()->retrieveAdditional(myAC->getTagProperty().getTag(), myAC->getID(), false);
        if (additional) {
            // declare auxiliar FXTreeItem, due a demand element can have multiple "roots"
            FXTreeItem* root = nullptr;
            // check if there is demand elements parents
            if (additional->getParentAdditionals().size() > 0) {
                // check if we have more than one edge
                if (additional->getParentAdditionals().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentAdditionals().front());
                    // insert "spacer"
                    if (additional->getParentAdditionals().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentAdditionals().size() - 2) + " additionals...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentAdditionals().back());
            }
            // check if there is parent demand elements
            if (additional->getParentDemandElements().size() > 0) {
                // check if we have more than one demand element
                if (additional->getParentDemandElements().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentDemandElements().front());
                    // insert "spacer"
                    if (additional->getParentDemandElements().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentDemandElements().size() - 2) + " demand elements...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentDemandElements().back());
            }
            // check if there is parent edges
            if (additional->getParentEdges().size() > 0) {
                // check if we have more than one edge
                if (additional->getParentEdges().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentEdges().front());
                    // insert "spacer"
                    if (additional->getParentEdges().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentEdges().size() - 2) + " edges...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentEdges().back());
            }
            // check if there is parent lanes
            if (additional->getParentLanes().size() > 0) {
                // check if we have more than one parent lane
                if (additional->getParentLanes().size() > 1) {
                    // insert first item
                    addListItem(additional->getParentLanes().front());
                    // insert "spacer"
                    if (additional->getParentLanes().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)additional->getParentLanes().size() - 2) + " lanes...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(additional->getParentLanes().back());
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
            if (demandElement->getParentAdditionals().size() > 0) {
                // check if we have more than one edge
                if (demandElement->getParentAdditionals().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentAdditionals().front());
                    // insert "spacer"
                    if (demandElement->getParentAdditionals().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentAdditionals().size() - 2) + " additionals...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentAdditionals().back());
            }
            // check if there is parent demand elements
            if (demandElement->getParentDemandElements().size() > 0) {
                // check if we have more than one demand element
                if (demandElement->getParentDemandElements().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentDemandElements().front());
                    // insert "spacer"
                    if (demandElement->getParentDemandElements().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentDemandElements().size() - 2) + " demand elements...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentDemandElements().back());
            }
            // check if there is parent edges
            if (demandElement->getParentEdges().size() > 0) {
                // check if we have more than one edge
                if (demandElement->getParentEdges().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentEdges().front());
                    // insert "spacer"
                    if (demandElement->getParentEdges().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentEdges().size() - 2) + " edges...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentEdges().back());
            }
            // check if there is parent lanes
            if (demandElement->getParentLanes().size() > 0) {
                // check if we have more than one parent lane
                if (demandElement->getParentLanes().size() > 1) {
                    // insert first item
                    addListItem(demandElement->getParentLanes().front());
                    // insert "spacer"
                    if (demandElement->getParentLanes().size() > 2) {
                        addListItem(nullptr, ("..." + toString((int)demandElement->getParentLanes().size() - 2) + " lanes...").c_str(), 0, false);
                    }
                }
                // return last inserted item
                root = addListItem(demandElement->getParentLanes().back());
            }
            // return last inserted list item
            return root;
        }

    } else if (myAC->getTagProperty().isDataElement()) {
        // check if is a GNEDataInterval or a GNEGenericData
        if (myAC->getTagProperty().getTag() == SUMO_TAG_DATASET) {
            return nullptr;
        } else if (myAC->getTagProperty().getTag() == SUMO_TAG_DATAINTERVAL) {
            return addListItem(myFrameParent->myViewNet->getNet()->retrieveDataSet(myAC->getID()));
        } else {
            // Obtain DataElement
            GNEGenericData* dataElement = dynamic_cast<GNEGenericData*>(myAC);
            if (dataElement) {
                // declare auxiliar FXTreeItem, due a data element can have multiple "roots"
                FXTreeItem* root = nullptr;
                // set dataset
                addListItem(dataElement->getDataIntervalParent()->getDataSetParent());
                // set data interval
                addListItem(dataElement->getDataIntervalParent());
                // check if there is data elements parents
                if (dataElement->getParentAdditionals().size() > 0) {
                    // check if we have more than one edge
                    if (dataElement->getParentAdditionals().size() > 1) {
                        // insert first item
                        addListItem(dataElement->getParentAdditionals().front());
                        // insert "spacer"
                        if (dataElement->getParentAdditionals().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentAdditionals().size() - 2) + " additionals...").c_str(), 0, false);
                        }
                    }
                    // return last inserted item
                    root = addListItem(dataElement->getParentAdditionals().back());
                }
                // check if there is parent demand elements
                if (dataElement->getParentDemandElements().size() > 0) {
                    // check if we have more than one demand element
                    if (dataElement->getParentDemandElements().size() > 1) {
                        // insert first item
                        addListItem(dataElement->getParentDemandElements().front());
                        // insert "spacer"
                        if (dataElement->getParentDemandElements().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentDemandElements().size() - 2) + " demand elements...").c_str(), 0, false);
                        }
                    }
                    // return last inserted item
                    root = addListItem(dataElement->getParentDemandElements().back());
                }
                // check if there is parent edges
                if (dataElement->getParentEdges().size() > 0) {
                    // check if we have more than one edge
                    if (dataElement->getParentEdges().size() > 1) {
                        // insert first ege
                        if (dataElement->getTagProperty().getTag() == SUMO_TAG_EDGEREL) {
                            addListItem(dataElement->getParentEdges().front(), nullptr, "from ");
                        } else {
                            addListItem(dataElement->getParentEdges().front());
                        }
                        // insert "spacer"
                        if (dataElement->getParentEdges().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentEdges().size() - 2) + " edges...").c_str(), 0, false);
                        }
                    }
                    // insert last ege
                    if (dataElement->getTagProperty().getTag() == SUMO_TAG_EDGEREL) {
                        addListItem(dataElement->getParentEdges().back(), nullptr, "to ");
                    } else {
                        addListItem(dataElement->getParentEdges().back());
                    }
                }
                // check if there is parent lanes
                if (dataElement->getParentLanes().size() > 0) {
                    // check if we have more than one parent lane
                    if (dataElement->getParentLanes().size() > 1) {
                        // insert first item
                        addListItem(dataElement->getParentLanes().front());
                        // insert "spacer"
                        if (dataElement->getParentLanes().size() > 2) {
                            addListItem(nullptr, ("..." + toString((int)dataElement->getParentLanes().size() - 2) + " lanes...").c_str(), 0, false);
                        }
                    }
                    // return last inserted item
                    root = addListItem(dataElement->getParentLanes().back());
                }
                // return last inserted list item
                return root;
            }
        }
    }
    // there aren't parents
    return nullptr;
}


void
GNEFrameModuls::AttributeCarrierHierarchy::showAttributeCarrierChildren(GNEAttributeCarrier* AC, FXTreeItem* itemParent) {
    if (AC->getTagProperty().isNetworkElement()) {
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
                    // insert child shapes
                    for (const auto& i : edge->getChildShapes()) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    // insert child additional
                    for (const auto& i : edge->getChildAdditionals()) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    // insert demand elements children (note: use getChildDemandElementsSortedByType to avoid duplicated elements)
                    for (const auto& i : edge->getChildDemandElementsByType(SUMO_TAG_ROUTE)) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    for (const auto& i : edge->getChildDemandElementsByType(SUMO_TAG_TRIP)) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    for (const auto& i : edge->getChildDemandElementsByType(SUMO_TAG_FLOW)) {
                        showAttributeCarrierChildren(i, edgeItem);
                    }
                    // show data elements
                    for (const auto& i : edge->getChildGenericDataElements()) {
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
                    // insert child shapes
                    for (const auto& i : lane->getChildShapes()) {
                        showAttributeCarrierChildren(i, laneItem);
                    }
                    // insert child additional
                    for (const auto& i : lane->getChildAdditionals()) {
                        showAttributeCarrierChildren(i, laneItem);
                    }
                    // insert demand elements children
                    for (const auto& i : lane->getChildDemandElements()) {
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
    } else if (AC->getTagProperty().isAdditionalElement() || AC->getTagProperty().isTAZ()) {
        // retrieve additional
        GNEAdditional* additional = myFrameParent->myViewNet->getNet()->retrieveAdditional(AC->getTagProperty().getTag(), AC->getID(), false);
        if (additional) {
            // insert additional item
            FXTreeItem* additionalItem = addListItem(AC, itemParent);
            // insert child edges
            for (const auto& i : additional->getChildEdges()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert child lanes
            for (const auto& i : additional->getChildLanes()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert child shapes
            for (const auto& i : additional->getChildShapes()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert additionals children
            for (const auto& i : additional->getChildAdditionals()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
            // insert child demand elements
            for (const auto& i : additional->getChildDemandElements()) {
                showAttributeCarrierChildren(i, additionalItem);
            }
        }
    } else if (AC->getTagProperty().isDemandElement()) {
        // retrieve demandElement
        GNEDemandElement* demandElement = myFrameParent->myViewNet->getNet()->retrieveDemandElement(AC->getTagProperty().getTag(), AC->getID(), false);
        if (demandElement) {
            // insert demandElement item
            FXTreeItem* demandElementItem = addListItem(AC, itemParent);
            // insert child edges
            for (const auto& i : demandElement->getChildEdges()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert child lanes
            for (const auto& i : demandElement->getChildLanes()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert child shapes
            for (const auto& i : demandElement->getChildShapes()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert additionals children
            for (const auto& i : demandElement->getChildAdditionals()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
            // insert child demand elements
            for (const auto& i : demandElement->getChildDemandElements()) {
                showAttributeCarrierChildren(i, demandElementItem);
            }
        }
    } else if (AC->getTagProperty().isDataElement()) {
        // insert data item
        FXTreeItem* dataElementItem = addListItem(AC, itemParent);
        // insert intervals
        if (AC->getTagProperty().getTag() == SUMO_TAG_DATASET) {
            GNEDataSet* dataSet = myFrameParent->myViewNet->getNet()->retrieveDataSet(AC->getID());
            // iterate over intevals
            for (const auto& interval : dataSet->getDataIntervalChildren()) {
                showAttributeCarrierChildren(interval.second, dataElementItem);
            }
        } else if (AC->getTagProperty().getTag() == SUMO_TAG_DATAINTERVAL) {
            GNEDataInterval* dataInterval = dynamic_cast<GNEDataInterval*>(AC);
            // iterate over generic datas
            for (const auto& genericData : dataInterval->getGenericDataChildren()) {
                showAttributeCarrierChildren(genericData, dataElementItem);
            }
        }
    }
}


FXTreeItem*
GNEFrameModuls::AttributeCarrierHierarchy::addListItem(GNEAttributeCarrier* AC, FXTreeItem* itemParent, std::string prefix, std::string sufix) {
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
GNEFrameModuls::AttributeCarrierHierarchy::addListItem(FXTreeItem* itemParent, const std::string& text, FXIcon* icon, bool expanded) {
    // insert item in Tree list
    FXTreeItem* item = myTreelist->insertItem(nullptr, itemParent, text.c_str(), icon, icon);
    // expand item depending of flag expanded
    item->setExpanded(expanded);
    // return created FXTreeItem
    return item;
}

// ---------------------------------------------------------------------------
// GNEFrameModuls::DrawingShape - methods
// ---------------------------------------------------------------------------

GNEFrameModuls::DrawingShape::DrawingShape(GNEFrame* frameParent) :
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


GNEFrameModuls::DrawingShape::~DrawingShape() {}


void GNEFrameModuls::DrawingShape::showDrawingShape() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEFrameModuls::DrawingShape::hideDrawingShape() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrameModuls::DrawingShape::startDrawing() {
    // Only start drawing if DrawingShape modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrameModuls::DrawingShape::stopDrawing() {
    // try to build shape
    if (myFrameParent->shapeDrawed()) {
        // clear created points
        myTemporalShapeShape.clear();
        myFrameParent->myViewNet->updateViewNet();
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
GNEFrameModuls::DrawingShape::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->myViewNet->updateViewNet();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrameModuls::DrawingShape::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrameModuls::DrawingShape::removeLastPoint() {

}


const PositionVector&
GNEFrameModuls::DrawingShape::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrameModuls::DrawingShape::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEFrameModuls::DrawingShape::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEFrameModuls::DrawingShape::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEFrameModuls::DrawingShape::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrameModuls::DrawingShape::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrameModuls::DrawingShape::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrameModuls::SelectorParent - methods
// ---------------------------------------------------------------------------

GNEFrameModuls::SelectorParent::SelectorParent(GNEFrame* frameParent) :
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


GNEFrameModuls::SelectorParent::~SelectorParent() {}


std::string
GNEFrameModuls::SelectorParent::getIdSelected() const {
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->isItemSelected(i)) {
            return myParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEFrameModuls::SelectorParent::setIDSelected(const std::string& id) {
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
GNEFrameModuls::SelectorParent::showSelectorParentModul(SumoXMLTag additionalType) {
    // make sure that we're editing an additional tag
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::ADDITIONALELEMENT, false);
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
GNEFrameModuls::SelectorParent::hideSelectorParentModul() {
    myParentTag = SUMO_TAG_NOTHING;
    hide();
}


void
GNEFrameModuls::SelectorParent::refreshSelectorParentModul() {
    myParentsList->clearItems();
    if (myParentTag != SUMO_TAG_NOTHING) {
        // fill list with IDs of additionals
        for (const auto& i : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getAdditionals().at(myParentTag)) {
            myParentsList->appendItem(i.first.c_str());
        }
    }
}

// ---------------------------------------------------------------------------
// GNEFrameModuls::OverlappedInspection - methods
// ---------------------------------------------------------------------------

GNEFrameModuls::OverlappedInspection::OverlappedInspection(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Overlapped elements", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myFilteredTag(SUMO_TAG_NOTHING),
    myItemIndex(0) {
    // build elements
    buildFXElements();
}


GNEFrameModuls::OverlappedInspection::OverlappedInspection(GNEFrame* frameParent, const SumoXMLTag filteredTag) :
    FXGroupBox(frameParent->myContentFrame, ("Overlapped " + toString(filteredTag) + "s").c_str(), GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myFilteredTag(filteredTag),
    myItemIndex(0) {
    // build elements
    buildFXElements();
}


GNEFrameModuls::OverlappedInspection::~OverlappedInspection() {}


void
GNEFrameModuls::OverlappedInspection::showOverlappedInspection(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition) {
    // first clear myOverlappedACs
    myOverlappedACs.clear();
    // check if we have to filter objects under cursor
    if (myFilteredTag == SUMO_TAG_NOTHING) {
        myOverlappedACs = objectsUnderCursor.getClickedAttributeCarriers();
    } else {
        // filter objects under cursor
        for (const auto& i : objectsUnderCursor.getClickedAttributeCarriers()) {
            if (i->getTagProperty().getTag() == myFilteredTag) {
                myOverlappedACs.push_back(i);
            }
        }
    }
    mySavedClickedPosition = clickedPosition;
    // by default we inspect first element
    myItemIndex = 0;
    // update text of current index button
    myCurrentIndexButton->setText(("1 / " + toString(myOverlappedACs.size())).c_str());
    // clear and fill list again
    myOverlappedElementList->clearItems();
    for (int i = 0; i < (int)myOverlappedACs.size(); i++) {
        myOverlappedElementList->insertItem(i, myOverlappedACs.at(i)->getID().c_str(), myOverlappedACs.at(i)->getIcon());
    }
    // set first element as selected element
    myOverlappedElementList->getItem(0)->setSelected(TRUE);
    // by default list hidden
    myOverlappedElementList->hide();
    // show template editor
    show();
}


void
GNEFrameModuls::OverlappedInspection::hideOverlappedInspection() {
    // hide modul
    hide();
}


bool
GNEFrameModuls::OverlappedInspection::overlappedInspectionShown() const {
    return shown();
}


int
GNEFrameModuls::OverlappedInspection::getNumberOfOverlappedACs() const {
    return (int)myOverlappedACs.size();
}


bool
GNEFrameModuls::OverlappedInspection::checkSavedPosition(const Position& clickedPosition) const {
    return (mySavedClickedPosition.distanceSquaredTo2D(clickedPosition) < 0.25);
}


bool
GNEFrameModuls::OverlappedInspection::nextElement(const Position& clickedPosition) {
    // first check if OverlappedInspection is shown
    if (shown()) {
        // check if given position is near saved position
        if (checkSavedPosition(clickedPosition)) {
            // inspect next element
            onCmdNextElement(0, 0, 0);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNEFrameModuls::OverlappedInspection::previousElement(const Position& clickedPosition) {
    // first check if OverlappedInspection is shown
    if (shown()) {
        // check if given position is near saved position
        if (checkSavedPosition(clickedPosition)) {
            // inspect previousElement
            onCmdPreviousElement(0, 0, 0);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


long
GNEFrameModuls::OverlappedInspection::onCmdPreviousElement(FXObject*, FXSelector, void*) {
    // check if there is items
    if (myOverlappedElementList->getNumItems() > 0) {
        // unselect current list element
        myOverlappedElementList->getItem((int)myItemIndex)->setSelected(FALSE);
        // set index (it works as a ring)
        if (myItemIndex > 0) {
            myItemIndex--;
        } else {
            myItemIndex = (myOverlappedACs.size() - 1);
        }
        // selected current list element
        myOverlappedElementList->getItem((int)myItemIndex)->setSelected(TRUE);
        myOverlappedElementList->update();
        // update current index button
        myCurrentIndexButton->setText((toString(myItemIndex + 1) + " / " + toString(myOverlappedACs.size())).c_str());
        // inspect overlapped attribute carrier
        myFrameParent->selectedOverlappedElement(myOverlappedACs.at(myItemIndex));
        // show OverlappedInspection again (because it's hidden in inspectSingleElement)
        show();
    }
    return 1;
}


long
GNEFrameModuls::OverlappedInspection::onCmdNextElement(FXObject*, FXSelector, void*) {
    // check if there is items
    if (myOverlappedElementList->getNumItems() > 0) {
        // unselect current list element
        myOverlappedElementList->getItem((int)myItemIndex)->setSelected(FALSE);
        // set index (it works as a ring)
        myItemIndex = (myItemIndex + 1) % myOverlappedACs.size();
        // selected current list element
        myOverlappedElementList->getItem((int)myItemIndex)->setSelected(TRUE);
        myOverlappedElementList->update();
        // update current index button
        myCurrentIndexButton->setText((toString(myItemIndex + 1) + " / " + toString(myOverlappedACs.size())).c_str());
        // inspect overlapped attribute carrier
        myFrameParent->selectedOverlappedElement(myOverlappedACs.at(myItemIndex));
        // show OverlappedInspection again (because it's hidden in inspectSingleElement)
        show();
    }
    return 1;
}


long
GNEFrameModuls::OverlappedInspection::onCmdShowList(FXObject*, FXSelector, void*) {
    // show or hidde element list
    if (myOverlappedElementList->shown()) {
        myOverlappedElementList->hide();
    } else {
        myOverlappedElementList->show();
    }
    myOverlappedElementList->recalc();
    // recalc and update frame
    recalc();
    return 1;
}

long
GNEFrameModuls::OverlappedInspection::onCmdListItemSelected(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myOverlappedElementList->getNumItems(); i++) {
        if (myOverlappedElementList->getItem(i)->isSelected()) {
            myItemIndex = i;
            // update current index button
            myCurrentIndexButton->setText((toString(myItemIndex + 1) + " / " + toString(myOverlappedACs.size())).c_str());
            // inspect overlapped attribute carrier
            myFrameParent->selectedOverlappedElement(myOverlappedACs.at(myItemIndex));
            // show OverlappedInspection again (because it's hidden in inspectSingleElement)
            show();
            return 1;
        }
    }
    return 0;
}


long
GNEFrameModuls::OverlappedInspection::onCmdOverlappingHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "GEO attributes Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << " - Click in the same position\n"
            << "   for inspect next element\n"
            << " - Shift + Click in the same\n"
            << "   position for inspect\n"
            << "   previous element";
    new FXLabel(helpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}


GNEFrameModuls::OverlappedInspection::OverlappedInspection() :
    myFilteredTag(SUMO_TAG_NOTHING) {
}


void
GNEFrameModuls::OverlappedInspection::buildFXElements() {
    FXHorizontalFrame* frameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create previous Item Button
    myPreviousElement = new FXButton(frameButtons, "", GUIIconSubSys::getIcon(GUIIcon::BIGARROWLEFT), this, MID_GNE_OVERLAPPED_PREVIOUS, GUIDesignButtonIconRectangular);
    // create current index button
    myCurrentIndexButton = new FXButton(frameButtons, "", nullptr, this, MID_GNE_OVERLAPPED_SHOWLIST, GUIDesignButton);
    // Create next Item Button
    myNextElement = new FXButton(frameButtons, "", GUIIconSubSys::getIcon(GUIIcon::BIGARROWRIGHT), this, MID_GNE_OVERLAPPED_NEXT, GUIDesignButtonIconRectangular);
    // Create list of overlapped elements (by default hidden)
    myOverlappedElementList = new FXList(this, this, MID_GNE_OVERLAPPED_ITEMSELECTED, GUIDesignListSingleElement);
    // disable vertical scrolling
    myOverlappedElementList->setScrollStyle(VSCROLLING_OFF);
    // by default list of overlapped elements is hidden)
    myOverlappedElementList->hide();
    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


std::vector<RGBColor>
GNEFrameModuls::buildRainbow(FXComposite* parent) {
    // declare scale colors
    std::vector<RGBColor> scaleColors;
    // create label for color information
    new FXLabel(parent, "Scale: Min -> Max", nullptr, GUIDesignLabelCenterThick);
    // fill scale colors (10)
    scaleColors.push_back(RGBColor(232, 35,  0));
    scaleColors.push_back(RGBColor(255, 165, 0));
    scaleColors.push_back(RGBColor(255, 255, 0));
    scaleColors.push_back(RGBColor(28,  215, 0));
    scaleColors.push_back(RGBColor(0,   181, 100));
    scaleColors.push_back(RGBColor(0,   255, 191));
    scaleColors.push_back(RGBColor(178, 255, 255));
    scaleColors.push_back(RGBColor(0,   112, 184));
    scaleColors.push_back(RGBColor(56,  41,  131));
    scaleColors.push_back(RGBColor(127, 0,   255));
    // create frame for color scale
    FXHorizontalFrame* horizontalFrameColors = new FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame);
    for (const auto& color : scaleColors) {
        FXLabel* colorLabel = new FXLabel(horizontalFrameColors, "", nullptr, GUIDesignLabelLeft);
        colorLabel->setBackColor(MFXUtils::getFXColor(color));
    }
    return scaleColors;
}

/****************************************************************************/
