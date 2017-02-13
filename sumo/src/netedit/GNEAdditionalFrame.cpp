/****************************************************************************/
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// The Widget for add additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplCheck.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEAdditionalFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNECrossing.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"
#include "GNEChange_Additional.h"
#include "GNEAdditional.h"
#include "GNEAdditionalHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame) GNEAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_ITEM, GNEAdditionalFrame::onCmdSelectAdditional),
};

FXDEFMAP(GNEAdditionalFrame::singleAdditionalParameter) GNEsingleAdditionalParameterMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, GNEAdditionalFrame::singleAdditionalParameter::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_BOOL, GNEAdditionalFrame::singleAdditionalParameter::onCmdSetBooleanAttribute),
};

FXDEFMAP(GNEAdditionalFrame::singleAdditionalParameterList) GNEsingleAdditionalParameterListMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDROW,    GNEAdditionalFrame::singleAdditionalParameterList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVEROW, GNEAdditionalFrame::singleAdditionalParameterList::onCmdRemoveRow),
};

FXDEFMAP(GNEAdditionalFrame::additionalParameters) GNEadditionalParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_HELP, GNEAdditionalFrame::additionalParameters::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::editorParameters) GNEEditorParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, GNEAdditionalFrame::editorParameters::onCmdSetLength),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT,       GNEAdditionalFrame::editorParameters::onCmdSelectReferencePoint),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                                     GNEAdditionalFrame::editorParameters::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::additionalParentSelector) GNEadditionalParentSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTADDITIONALPARENT, GNEAdditionalFrame::additionalParentSelector::onCmdSelectAdditionalParent),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                       GNEAdditionalFrame::additionalParentSelector::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::edgeParentsSelector) GNEedgeParentsSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHOWONLYSELECTEDEDGES, GNEAdditionalFrame::edgeParentsSelector::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CLEAREDGESELECTION,    GNEAdditionalFrame::edgeParentsSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INVERTEDGESELECTION,   GNEAdditionalFrame::edgeParentsSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SEARCHEDGE,            GNEAdditionalFrame::edgeParentsSelector::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTEDGE,            GNEAdditionalFrame::edgeParentsSelector::onCmdSelectEdge),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                      GNEAdditionalFrame::edgeParentsSelector::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::laneParentsSelector) GNElaneParentsSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_USESELECTEDLANES,    GNEAdditionalFrame::laneParentsSelector::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CLEARLANESELECTION,  GNEAdditionalFrame::laneParentsSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INVERTLANESELECTION, GNEAdditionalFrame::laneParentsSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SEARCHLANE,          GNEAdditionalFrame::laneParentsSelector::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTLANE,          GNEAdditionalFrame::laneParentsSelector::onCmdSelectLane),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                    GNEAdditionalFrame::laneParentsSelector::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame,                                FXVerticalFrame, GNEAdditionalMap,                    ARRAYNUMBER(GNEAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::singleAdditionalParameter,     FXMatrix,        GNEsingleAdditionalParameterMap,     ARRAYNUMBER(GNEsingleAdditionalParameterMap))
FXIMPLEMENT(GNEAdditionalFrame::singleAdditionalParameterList, FXMatrix,        GNEsingleAdditionalParameterListMap, ARRAYNUMBER(GNEsingleAdditionalParameterListMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalParameters,          FXGroupBox,      GNEadditionalParametersMap,          ARRAYNUMBER(GNEadditionalParametersMap))
FXIMPLEMENT(GNEAdditionalFrame::editorParameters,              FXGroupBox,      GNEEditorParametersMap,              ARRAYNUMBER(GNEEditorParametersMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalParentSelector,      FXGroupBox,      GNEadditionalParentSelectorMap,      ARRAYNUMBER(GNEadditionalParentSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::edgeParentsSelector,           FXGroupBox,      GNEedgeParentsSelectorMap,           ARRAYNUMBER(GNEedgeParentsSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::laneParentsSelector,           FXGroupBox,      GNElaneParentsSelectorMap,           ARRAYNUMBER(GNElaneParentsSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalFrame::GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Additionals"),
    myActualAdditionalType(SUMO_TAG_NOTHING) {

    // Create groupBox for myAdditionalMatchBox
    myGroupBoxForMyAdditionalMatchBox = new FXGroupBox(myContentFrame, "Additional element", GUIDesignGroupBoxFrame);

    // Create FXListBox in myGroupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(myGroupBoxForMyAdditionalMatchBox, GUIDesignComboBoxNCol, this, MID_GNE_MODE_ADDITIONAL_ITEM, GUIDesignComboBox);

    // Create additional parameters
    myadditionalParameters = new GNEAdditionalFrame::additionalParameters(myContentFrame);

    // Create editor parameter
    myEditorParameters = new GNEAdditionalFrame::editorParameters(myContentFrame);

    // Create create list for additional Set
    myAdditionalParentSelector = new GNEAdditionalFrame::additionalParentSelector(myContentFrame, myViewNet);

    /// Create list for edgeParentsSelector
    myedgeParentsSelector = new GNEAdditionalFrame::edgeParentsSelector(myContentFrame, myViewNet);

    /// Create list for laneParentsSelector
    mylaneParentsSelector = new GNEAdditionalFrame::laneParentsSelector(myContentFrame, myViewNet);

    // Add options to myAdditionalMatchBox
    const std::vector<SumoXMLTag>& additionalTags = GNEAttributeCarrier::allowedTags(false);
    for (std::vector<SumoXMLTag>::const_iterator i = additionalTags.begin(); i != additionalTags.end(); i++) {
        myAdditionalMatchBox->appendItem(toString(*i).c_str());
    }

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems());

    // If there are additionals
    if (additionalTags.size() > 0) {
        // Set myActualAdditionalType and show
        myActualAdditionalType = additionalTags.front();
        setParametersOfAdditional(myActualAdditionalType);
    }
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    gSelected.remove2Update();
}

bool
GNEAdditionalFrame::addAdditional(GNENetElement* netElement, GUISUMOAbstractView* parent) {
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement;

    if (myadditionalParameters->isValuesValid()) {
        valuesOfElement = myadditionalParameters->getAttributesAndValues();
    } else {
        WRITE_WARNING("Some attributes of additional '" + toString(myActualAdditionalType) + "' aren't valid.");
        return false;
    }

    // Declare pointer to netElements
    GNEJunction* pointed_junction = NULL;
    GNEEdge* pointed_edge = NULL;
    GNELane* pointed_lane = NULL;
    GNECrossing* pointed_crossing = NULL;

    // Check if additional should be placed over a junction
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_JUNCTION)) {
        pointed_junction = dynamic_cast<GNEJunction*>(netElement);
        if (pointed_junction != NULL) {
            // Get attribute junction
            valuesOfElement[SUMO_ATTR_JUNCTION] = pointed_junction->getID();
            // Generate id of element based on the junction
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_junction);
        } else {
            return false;
        }
    }
    // Check if additional should be placed over a edge
    else if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGE)) {
        // Due a edge is composed of lanes, its neccesary check if clicked element is an lane
        if (dynamic_cast<GNELane*>(netElement) != NULL) {
            pointed_edge = &(dynamic_cast<GNELane*>(netElement)->getParentEdge());
        }
        if (pointed_edge != NULL) {
            // Get attribute edge
            valuesOfElement[SUMO_ATTR_EDGE] = pointed_edge->getID();
            // Generate id of element based on the edge
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_edge);
        } else {
            return false;
        }
    }
    // Check if additional should be placed over a lane
    else if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANE)) {
        pointed_lane = dynamic_cast<GNELane*>(netElement);
        if (pointed_lane != NULL) {
            // Get attribute lane
            valuesOfElement[SUMO_ATTR_LANE] = pointed_lane->getID();
            // Generate id of element based on the lane
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_lane);
        } else {
            return false;
        }
    }
    // Check if additional should be placed over a crossing
    else if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_CROSSING)) {
        pointed_crossing = dynamic_cast<GNECrossing*>(netElement);
        if (pointed_crossing != NULL) {
            // Get attribute crossing
            valuesOfElement[SUMO_ATTR_CROSSING] = pointed_crossing->getID();
            // Generate id of element based on the crossing
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_crossing);
        } else {
            return false;
        }
    } else {
        // Generate id of element
        valuesOfElement[SUMO_ATTR_ID] = generateID(NULL);
    }

    // Obtain position attribute
    if (pointed_edge) {
        // Obtain position of the mouse over edge
        SUMOReal positionOfTheMouseOverEdge = pointed_edge->getLanes().at(0)->getShape().nearest_offset_to_point2D(parent->getPositionInformation());
        // If element has a StartPosition and EndPosition over edge, extract attributes
        if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_ENDPOS)) {
            // First check that current length is valid
            if (myEditorParameters->isCurrentLenghtValid()) {
                SUMOReal startPos = setStartPosition(positionOfTheMouseOverEdge, myEditorParameters->getLenght());
                SUMOReal endPos = setEndPosition(pointed_edge->getLanes().at(0)->getLaneShapeLenght(), positionOfTheMouseOverEdge, myEditorParameters->getLenght());
                // Only set start position if are valid (!= -1)
                if (startPos != -1) {
                    valuesOfElement[SUMO_ATTR_STARTPOS] = toString(startPos);
                } else {
                    WRITE_WARNING("Additional '" + toString(myActualAdditionalType) + "' cannot be placed over edge. Attribute '" + toString(SUMO_ATTR_STARTPOS) + "' isn't valid");
                    return false;
                }
                // Only set end position if are valid (!= -1)
                if (endPos != -1) {
                    valuesOfElement[SUMO_ATTR_ENDPOS] = toString(endPos);
                } else {
                    WRITE_WARNING("Additional '" + toString(myActualAdditionalType) + "' cannot be placed over edge. Attribute '" + toString(SUMO_ATTR_ENDPOS) + "' isn't valid");
                    return false;
                }
            } else {
                return false;
            }
        }
        // Extract position of lane
        valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverEdge);
    } else if (pointed_lane) {
        // Obtain position of the mouse over lane
        SUMOReal positionOfTheMouseOverLane = pointed_lane->getShape().nearest_offset_to_point2D(parent->getPositionInformation());
        // If element has a StartPosition and EndPosition over lane, extract attributes
        if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_ENDPOS)) {
            // First check that current length is valid
            if (myEditorParameters->isCurrentLenghtValid()) {
                SUMOReal startPos = setStartPosition(positionOfTheMouseOverLane, myEditorParameters->getLenght());
                SUMOReal endPos = setEndPosition(pointed_lane->getLaneShapeLenght(), positionOfTheMouseOverLane, myEditorParameters->getLenght());
                // Only set start position if are valid (!= -1)
                if (startPos != -1) {
                    valuesOfElement[SUMO_ATTR_STARTPOS] = toString(startPos);
                } else {
                    WRITE_WARNING("Additonal '" + toString(myActualAdditionalType) + "' cannot be placed over lane. Attribute '" + toString(SUMO_ATTR_STARTPOS) + "' isn't valid");
                    return false;
                }
                // Only set end position if are valid (!= -1)
                if (endPos != -1) {
                    valuesOfElement[SUMO_ATTR_ENDPOS] = toString(endPos);
                } else {
                    WRITE_WARNING("Additonal '" + toString(myActualAdditionalType) + "' cannot be placed over lane. Attribute '" + toString(SUMO_ATTR_ENDPOS) + "' isn't valid");
                    return false;
                }
            } else {
                return false;
            }
        }
        // Extract position of lane
        valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverLane);
    } else {
        // get position in map
        valuesOfElement[SUMO_ATTR_POSITION] = toString(parent->getPositionInformation());
    }

    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.txt
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_FILE) && valuesOfElement[SUMO_ATTR_FILE] == "") {
        valuesOfElement[SUMO_ATTR_FILE] = (valuesOfElement[SUMO_ATTR_ID] + ".txt");
    }

    // If additional own the attribute SUMO_ATTR_OUTPUT but was't defined, will defined as <ID>.txt
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_OUTPUT) && valuesOfElement[SUMO_ATTR_OUTPUT] == "") {
        valuesOfElement[SUMO_ATTR_OUTPUT] = (valuesOfElement[SUMO_ATTR_ID] + ".txt");
    }

    // Save block value
    valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myEditorParameters->isBlockEnabled());

    // If element belongst to an additional Set, get id of parent from myAdditionalParentSelector
    if ((myActualAdditionalType == SUMO_TAG_DET_ENTRY) || (myActualAdditionalType == SUMO_TAG_DET_EXIT)) {
        if (myAdditionalParentSelector->getIdSelected() != "") {
            valuesOfElement[GNE_ATTR_PARENT] = myAdditionalParentSelector->getIdSelected();
        } else {
            WRITE_WARNING("A " + toString(SUMO_TAG_E3DETECTOR) + " must be selected before insertion of " + toString(myActualAdditionalType) + ".");
            return false;
        }
    }

    // If element own a list of edgeParentsSelector as attribute
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGES)) {
        if (myedgeParentsSelector->isUseSelectedEdgesEnable()) {
            // Declare a vector of Id's
            std::vector<std::string> vectorOfIds;
            // get Selected edges
            std::vector<GNEEdge*> selectedEdges = myViewNet->getNet()->retrieveEdges(true);
            // Iterate over selectedEdges and getId
            for (std::vector<GNEEdge*>::iterator i = selectedEdges.begin(); i != selectedEdges.end(); i++) {
                vectorOfIds.push_back((*i)->getID());
            }
            // Set saved Ids in attribute edges
            valuesOfElement[SUMO_ATTR_EDGES] = joinToString(vectorOfIds, " ");
        } else {
            valuesOfElement[SUMO_ATTR_EDGES] = myedgeParentsSelector->getIdsSelected();
        }
        // check if attribute has at least an Edge
        if (valuesOfElement[SUMO_ATTR_EDGES] == "") {
            WRITE_WARNING("A " + toString(myActualAdditionalType) + " must have at least one edge associated.");
            return false;
        }
    }

    // If element own a list of laneParentsSelector as attribute
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANES)) {
        if (mylaneParentsSelector->isUseSelectedLanesEnable()) {
            // Declare a vector of Id's
            std::vector<std::string> vectorOfIds;
            // get Selected lanes
            std::vector<GNELane*> selectedLanes = myViewNet->getNet()->retrieveLanes(true);
            // Iterate over selectedLanes and getId
            for (std::vector<GNELane*>::iterator i = selectedLanes.begin(); i != selectedLanes.end(); i++) {
                vectorOfIds.push_back((*i)->getID());
            }
            // Set saved Ids in attribute lanes
            valuesOfElement[SUMO_ATTR_LANES] = joinToString(vectorOfIds, " ");
        } else {
            valuesOfElement[SUMO_ATTR_LANES] = mylaneParentsSelector->getIdsSelected();
        }
        // check if attribute has at least a lane
        if (valuesOfElement[SUMO_ATTR_LANES] == "") {
            WRITE_WARNING("A " + toString(myActualAdditionalType) + " must have at least one lane associated.");
            return false;
        }
    }

    // Create additional
    return GNEAdditionalHandler::buildAdditional(myViewNet, myActualAdditionalType, valuesOfElement);
}

void
GNEAdditionalFrame::removeAdditional(GNEAdditional* additional) {
    myViewNet->getUndoList()->p_begin("delete " + additional->getDescription());
    myViewNet->getUndoList()->add(new GNEChange_Additional(additional, false), true);
    myViewNet->getUndoList()->p_end();
}


long
GNEAdditionalFrame::onCmdSelectAdditional(FXObject*, FXSelector, void*) {
    // set myActualAdditionalType
    const std::vector<SumoXMLTag>& additionalTags = GNEAttributeCarrier::allowedTags(false);
    for (std::vector<SumoXMLTag>::const_iterator i = additionalTags.begin(); i != additionalTags.end(); i++) {
        if (toString(*i) == myAdditionalMatchBox->getText().text()) {
            setParametersOfAdditional(*i);
        }
    }
    return 1;
}


void
GNEAdditionalFrame::show() {
    // Show frame
    GNEFrame::show();
    // Update UseSelectedLane CheckBox
    myedgeParentsSelector->updateUseSelectedEdges();
    // Update UseSelectedLane CheckBox
    mylaneParentsSelector->updateUseSelectedLanes();
}


void
GNEAdditionalFrame::setParametersOfAdditional(SumoXMLTag actualAdditionalType) {
    // Set new actualAdditionalType
    myActualAdditionalType = actualAdditionalType;
    // Clear default parameters
    myadditionalParameters->clearAttributes();
    // Hide lenght field and reference point
    myEditorParameters->hideLengthField();
    myEditorParameters->hideReferencePoint();
    // Obtain attributes of actual myActualAdditionalType
    std::vector<std::pair <SumoXMLAttr, std::string> > attrs = GNEAttributeCarrier::allowedAttributes(myActualAdditionalType);
    // Iterate over attributes of myActualAdditionalType
    for (std::vector<std::pair <SumoXMLAttr, std::string> >::iterator i = attrs.begin(); i != attrs.end(); i++) {
        if (!GNEAttributeCarrier::isUnique(myActualAdditionalType, i->first)) {
            myadditionalParameters->addAttribute(myActualAdditionalType, i->first);
        } else if (i->first == SUMO_ATTR_ENDPOS) {
            myEditorParameters->showLengthField();
            myEditorParameters->showReferencePoint();
        }
    }
    // if there are parmeters, show and Recalc groupBox
    if (myadditionalParameters->getNumberOfAddedAttributes() > 0) {
        myadditionalParameters->showAdditionalParameters();
    } else {
        myadditionalParameters->hideAdditionalParameters();
    }
    // Show myAdditionalParentSelector if we're adding a Entry/Exit
    if ((myActualAdditionalType == SUMO_TAG_DET_ENTRY) || (myActualAdditionalType == SUMO_TAG_DET_EXIT)) {
        myAdditionalParentSelector->showListOfAdditionals(SUMO_TAG_E3DETECTOR, true);
    } else {
        myAdditionalParentSelector->hideListOfAdditionals();
    }
    // Show edgeParentsSelector if we're adding an additional that own the attribute SUMO_ATTR_EDGES
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGES)) {
        myedgeParentsSelector->showList();
    } else {
        myedgeParentsSelector->hideList();
    }
    // Show laneParentsSelector if we're adding an additional that own the attribute SUMO_ATTR_LANES
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANES)) {
        mylaneParentsSelector->showList();
    } else {
        mylaneParentsSelector->hideList();
    }
}


std::string
GNEAdditionalFrame::generateID(GNENetElement* netElement) const {
    int additionalIndex = myViewNet->getNet()->getNumberOfAdditionals(myActualAdditionalType);
    if (netElement) {
        // generate ID using netElement
        while (myViewNet->getNet()->getAdditional(myActualAdditionalType, toString(myActualAdditionalType) + "_" + netElement->getID() + "_" + toString(additionalIndex)) != NULL) {
            additionalIndex++;
        }
        return toString(myActualAdditionalType) + "_" + netElement->getID() + "_" + toString(additionalIndex);
    } else {
        // generate ID without netElement
        while (myViewNet->getNet()->getAdditional(myActualAdditionalType, toString(myActualAdditionalType) + "_" + toString(additionalIndex)) != NULL) {
            additionalIndex++;
        }
        return toString(myActualAdditionalType) + "_" + toString(additionalIndex);
    }
}


SUMOReal
GNEAdditionalFrame::setStartPosition(SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional) {
    switch (myEditorParameters->getActualReferencePoint()) {
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane;
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_RIGHT: {
            if (positionOfTheMouseOverLane - lenghtOfAdditional >= 0.01) {
                return positionOfTheMouseOverLane - lenghtOfAdditional;
            } else if (myEditorParameters->isForcePositionEnabled()) {
                return 0.01;
            } else {
                return -1;
            }
        }
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_CENTER: {
            if (positionOfTheMouseOverLane - lenghtOfAdditional / 2 >= 0.01) {
                return positionOfTheMouseOverLane - lenghtOfAdditional / 2;
            } else if (myEditorParameters->isForcePositionEnabled()) {
                return 0;
            } else {
                return -1;
            }
        }
        default:
            return -1;
    }
}


SUMOReal
GNEAdditionalFrame::setEndPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional) {
    switch (myEditorParameters->getActualReferencePoint()) {
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_LEFT: {
            if (positionOfTheMouseOverLane + lenghtOfAdditional <= laneLenght - 0.01) {
                return positionOfTheMouseOverLane + lenghtOfAdditional;
            } else if (myEditorParameters->isForcePositionEnabled()) {
                return laneLenght - 0.01;
            } else {
                return -1;
            }
        }
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_CENTER: {
            if (positionOfTheMouseOverLane + lenghtOfAdditional / 2 <= laneLenght - 0.01) {
                return positionOfTheMouseOverLane + lenghtOfAdditional / 2;
            } else if (myEditorParameters->isForcePositionEnabled()) {
                return laneLenght - 0.01;
            } else {
                return -1;
            }
        }
        default:
            return -1;
    }
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::singleAdditionalParameter - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::singleAdditionalParameter::singleAdditionalParameter(FXComposite* parent) :
    FXMatrix(parent, 4, GUIDesignMatrixAttributes),
    myAdditionalTag(SUMO_TAG_NOTHING),
    myAdditionalAttr(SUMO_ATTR_NOTHING),
    myCurrentValueValid(true) {
    // Create visual elements
    myLabel = new FXLabel(this, "name", 0, GUIDesignLabelAttribute, 0, 0, 60, 0);
    myTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, GUIDesignTextFieldAttributeStr);
    myTimeSpinDial = new FXSpinner(this, 7, this, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_DIAL, GUIDesignSpinDialAttribute);
    myMenuCheck = new FXMenuCheck(this, "Disabled", this, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_BOOL, GUIDesignMenuCheckAttribute);
    // Set widht of menuCheck manually
    myMenuCheck->setWidth(20);
    // Hide elements
    hideParameter();
}


GNEAdditionalFrame::singleAdditionalParameter::~singleAdditionalParameter() {}


void
GNEAdditionalFrame::singleAdditionalParameter::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::string value) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    myTextField->setText(value.c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::singleAdditionalParameter::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, int value) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    myTextField->setText(toString(value).c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::singleAdditionalParameter::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, SUMOReal value, bool isTime) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    if (isTime) {
        myTimeSpinDial->setValue((int)value);
        myTimeSpinDial->show();
    } else {
        myTextField->setText(toString(value).c_str());
        myTextField->show();
    }
    show();
}


void
GNEAdditionalFrame::singleAdditionalParameter::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, bool value) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myTextField->setTextColor(FXRGB(0, 0, 0));
    myLabel->show();
    if (value) {
        myMenuCheck->setCheck(true);
        myMenuCheck->setText("True");
    } else {
        myMenuCheck->setCheck(false);
        myMenuCheck->setText("False");
    }
    myMenuCheck->show();
    myCurrentValueValid = true;
    show();
}


void
GNEAdditionalFrame::singleAdditionalParameter::hideParameter() {
    myAdditionalTag = SUMO_TAG_NOTHING;
    myAdditionalAttr = SUMO_ATTR_NOTHING;
    myLabel->hide();
    myTextField->hide();
    myMenuCheck->hide();
    myTimeSpinDial->hide();
    hide();
}


SumoXMLTag
GNEAdditionalFrame::singleAdditionalParameter::getTag() const {
    return myAdditionalTag;
}


SumoXMLAttr
GNEAdditionalFrame::singleAdditionalParameter::getAttr() const {
    return myAdditionalAttr;
}


std::string
GNEAdditionalFrame::singleAdditionalParameter::getValue() const {
    if (GNEAttributeCarrier::isBool(myAdditionalTag, myAdditionalAttr)) {
        return (myMenuCheck->getCheck() == 1) ? "true" : "false";
    } else if (GNEAttributeCarrier::isTime(myAdditionalTag, myAdditionalAttr)) {
        return toString(myTimeSpinDial->getValue());
    } else  {
        return myTextField->getText().text();
    }
}


bool
GNEAdditionalFrame::singleAdditionalParameter::isCurrentValueValid() const {
    return myCurrentValueValid;
}


long
GNEAdditionalFrame::singleAdditionalParameter::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Check if format of current value of myTextField is correct
    if (GNEAttributeCarrier::isInt(myAdditionalTag, myAdditionalAttr)) {
        if (TplCheck::_str2int(myTextField->getText().text())) {
            myCurrentValueValid = true;
            // convert string to int
            int intValue = TplConvert::_str2int(myTextField->getText().text());
            // Check if int value must be positive
            if (GNEAttributeCarrier::isPositive(myAdditionalTag, myAdditionalAttr) && (intValue < 0)) {
                myCurrentValueValid = false;
            }
        } else {
            myCurrentValueValid = false;
        }
    } else if (GNEAttributeCarrier::isFloat(myAdditionalTag, myAdditionalAttr)) {
        if (TplCheck::_str2SUMOReal(myTextField->getText().text())) {
            myCurrentValueValid = true;
            // convert string to double
            SUMOReal SUMORealValue = TplConvert::_str2SUMOReal(myTextField->getText().text());
            // Check if double value must be positive
            if (GNEAttributeCarrier::isPositive(myAdditionalTag, myAdditionalAttr) && (SUMORealValue < 0)) {
                myCurrentValueValid = false;
                // check if double value is a probability
            } else if (GNEAttributeCarrier::isProbability(myAdditionalTag, myAdditionalAttr) && ((SUMORealValue < 0) || SUMORealValue > 1)) {
                myCurrentValueValid = false;
            }
        } else {
            myCurrentValueValid = false;
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myCurrentValueValid) {
        myTextField->setTextColor(FXRGB(0, 0, 0));
        myTextField->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myTextField->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEAdditionalFrame::singleAdditionalParameter::onCmdSetBooleanAttribute(FXObject*, FXSelector, void*) {
    if (myMenuCheck->getCheck()) {
        myMenuCheck->setText("True");
    } else {
        myMenuCheck->setText("False");
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::singleAdditionalParameterList - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::singleAdditionalParameterList::singleAdditionalParameterList(FXComposite* parent) :
    FXMatrix(parent, 2, GUIDesignMatrixAttributes),
    myAdditionalTag(SUMO_TAG_NOTHING),
    myAdditionalAttr(SUMO_ATTR_NOTHING),
    numberOfVisibleTextfields(1),
    myMaxNumberOfValuesInParameterList(20) {
    // Create elements
    for (int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myLabels.push_back(new FXLabel(this, "name", 0, GUIDesignLabelAttribute, 0, 0, 60, 0));
        myTextFields.push_back(new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, GUIDesignTextFieldAttributeStr));
    }
    // Create label Row
    myLabels.push_back(new FXLabel(this, "Rows", 0, GUIDesignLabelAttribute, 0, 0, 60, 0));
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    // Create add button
    add = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_ADDROW, GUIDesignButtonAttribute, 0, 0, 20, 20);
    // Create delete buttons
    remove = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), this, MID_GNE_REMOVEROW, GUIDesignButtonAttribute, 0, 0, 20, 20);
    // Hide all para meters
    hideParameter();
}


GNEAdditionalFrame::singleAdditionalParameterList::~singleAdditionalParameterList() {}


void
GNEAdditionalFrame::singleAdditionalParameterList::showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<int> /* value */) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    std::cout << "FINISH" << std::endl;
}


void
GNEAdditionalFrame::singleAdditionalParameterList::showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<SUMOReal> /* value */, bool /*isTime*/) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    std::cout << "FINISH" << std::endl;
}


void
GNEAdditionalFrame::singleAdditionalParameterList::showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<bool> /* value */) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    std::cout << "FINISH" << std::endl;
}


void
GNEAdditionalFrame::singleAdditionalParameterList::showListParameter(SumoXMLTag tag, SumoXMLAttr attr, std::vector<std::string> value) {
    if ((int)value.size() < myMaxNumberOfValuesInParameterList) {
        myAdditionalTag = tag;
        myAdditionalAttr = attr;
        numberOfVisibleTextfields = (int)value.size();
        if (numberOfVisibleTextfields == 0) {
            numberOfVisibleTextfields++;
        }
        for (int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
            myLabels.at(i)->setText((toString(attr) + ": " + toString(i)).c_str());
        }
        for (int i = 0; i < numberOfVisibleTextfields; i++) {
            myLabels.at(i)->show();
            myTextFields.at(i)->show();
        }
        add->show();
        remove->show();
        show();
    }
}


void
GNEAdditionalFrame::singleAdditionalParameterList::hideParameter() {
    myAdditionalTag = SUMO_TAG_NOTHING;
    myAdditionalAttr = SUMO_ATTR_NOTHING;
    for (int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myLabels.at(i)->hide();
        myTextFields.at(i)->hide();
        myTextFields.at(i)->setText("");
    }
    add->hide();
    remove->hide();
    hide();
}


SumoXMLTag
GNEAdditionalFrame::singleAdditionalParameterList::getTag() const {
    return myAdditionalTag;
}

SumoXMLAttr
GNEAdditionalFrame::singleAdditionalParameterList::getAttr() const {
    return myAdditionalAttr;
}


std::string
GNEAdditionalFrame::singleAdditionalParameterList::getListValues() {
    // Declare, fill and return a string with the list values
    std::string value;
    for (int i = 0; i < numberOfVisibleTextfields; i++) {
        if (!myTextFields.at(i)->getText().empty()) {
            value += (myTextFields.at(i)->getText().text() + std::string(" "));
        }
    }
    return value;
}


bool
GNEAdditionalFrame::singleAdditionalParameterList::isCurrentListValid() const {
    /// @todo finish check validity of values in lists
    return true;
}


long
GNEAdditionalFrame::singleAdditionalParameterList::onCmdAddRow(FXObject*, FXSelector, void*) {
    if (numberOfVisibleTextfields < (myMaxNumberOfValuesInParameterList - 1)) {
        myLabels.at(numberOfVisibleTextfields)->show();
        myTextFields.at(numberOfVisibleTextfields)->show();
        numberOfVisibleTextfields++;
        getParent()->recalc();
    }
    return 1;
}


long
GNEAdditionalFrame::singleAdditionalParameterList::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    if (numberOfVisibleTextfields > 1) {
        numberOfVisibleTextfields--;
        myLabels.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->setText("");

        getParent()->recalc();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::editorParameters- methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalParameters::additionalParameters(FXComposite* parent) :
    FXGroupBox(parent, "Default parameters", GUIDesignGroupBoxFrame),
    myIndexParameter(0),
    myIndexParameterList(0),
    maxNumberOfParameters(GNEAttributeCarrier::getHigherNumberOfAttributes()),
    maxNumberOfListParameters(2) {

    // Create single parameters
    for (int i = 0; i < maxNumberOfParameters; i++) {
        myVectorOfsingleAdditionalParameter.push_back(new singleAdditionalParameter(this));
    }

    // Create single list parameters
    for (int i = 0; i < maxNumberOfListParameters; i++) {
        myVectorOfsingleAdditionalParameterList.push_back(new singleAdditionalParameterList(this));
    }

    // Create help button
    helpAdditional = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);
}


GNEAdditionalFrame::additionalParameters::~additionalParameters() {
}


void
GNEAdditionalFrame::additionalParameters::clearAttributes() {
    // Hidde al fields
    for (int i = 0; i < maxNumberOfParameters; i++) {
        myVectorOfsingleAdditionalParameter.at(i)->hideParameter();
    }

    // Hidde al list fields
    for (int i = 0; i < maxNumberOfListParameters; i++) {
        myVectorOfsingleAdditionalParameterList.at(i)->hideParameter();
    }

    // Reset indexs
    myIndexParameterList = 0;
    myIndexParameter = 0;
}


void
GNEAdditionalFrame::additionalParameters::addAttribute(SumoXMLTag additionalTag, SumoXMLAttr additionalAttribute) {
    // Set current additional
    myAdditionalTag = additionalTag;
    // If  parameter is of type list
    if (GNEAttributeCarrier::isList(myAdditionalTag, additionalAttribute)) {
        // If parameter can be show
        if (myIndexParameterList < maxNumberOfListParameters) {
            // Check type of attribute list
            if (GNEAttributeCarrier::isInt(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue< std::vector<int> >(myAdditionalTag, additionalAttribute));
            } else if (GNEAttributeCarrier::isFloat(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue< std::vector<SUMOReal> >(myAdditionalTag, additionalAttribute));
            } else if (GNEAttributeCarrier::isTime(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue< std::vector<SUMOReal> >(myAdditionalTag, additionalAttribute), true);
            } else if (GNEAttributeCarrier::isBool(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue< std::vector<bool> >(myAdditionalTag, additionalAttribute));
            } else if (GNEAttributeCarrier::isString(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue< std::vector<std::string> >(myAdditionalTag, additionalAttribute));
            }
            // Update index
            myIndexParameterList++;
        } else {
            WRITE_ERROR("Max number of list attributes reached (" + toString(maxNumberOfListParameters) + ").");
        }
    } else {
        if (myIndexParameter < maxNumberOfParameters) {
            // Check type of attribute list
            if (GNEAttributeCarrier::isInt(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue<int>(myAdditionalTag, additionalAttribute));
            } else if (GNEAttributeCarrier::isFloat(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue<SUMOReal>(myAdditionalTag, additionalAttribute));
            } else if (GNEAttributeCarrier::isTime(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue<SUMOReal>(myAdditionalTag, additionalAttribute), true);
            } else if (GNEAttributeCarrier::isBool(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue<bool>(myAdditionalTag, additionalAttribute));
            } else if (GNEAttributeCarrier::isString(myAdditionalTag, additionalAttribute)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, additionalAttribute, GNEAttributeCarrier::getDefaultValue<std::string>(myAdditionalTag, additionalAttribute));
            } else {
                WRITE_WARNING("Attribute '" + toString(additionalAttribute) + "' don't have a defined type. Check definition in GNEAttributeCarrier");
            }
            // Update index parameter
            myIndexParameter++;
        } else {
            WRITE_ERROR("Max number of attributes reached (" + toString(maxNumberOfParameters) + ").");
        }
    }
}


void
GNEAdditionalFrame::additionalParameters::showAdditionalParameters() {
    recalc();
    show();
}


void
GNEAdditionalFrame::additionalParameters::hideAdditionalParameters() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEAdditionalFrame::additionalParameters::getAttributesAndValues() const {
    std::map<SumoXMLAttr, std::string> values;
    // get standar Parameters
    for (int i = 0; i < myIndexParameter; i++) {
        values[myVectorOfsingleAdditionalParameter.at(i)->getAttr()] = myVectorOfsingleAdditionalParameter.at(i)->getValue();
    }
    // get list parameters
    for (int i = 0; i < myIndexParameterList; i++) {
        values[myVectorOfsingleAdditionalParameterList.at(i)->getAttr()] = myVectorOfsingleAdditionalParameterList.at(i)->getListValues();
    }
    return values;
}


bool
GNEAdditionalFrame::additionalParameters::isValuesValid() const {
    // iterate over standar parameters
    for (int i = 0; i < myIndexParameter; i++) {
        // Return false if at least one of the parameter isn't valid
        if (myVectorOfsingleAdditionalParameter.at(i)->isCurrentValueValid() == false) {
            return false;
        }
    }
    // iterate over list parameters
    for (int i = 0; i < myIndexParameterList; i++) {
        // Return false if at least one of the list parameter isn't valid
        if (myVectorOfsingleAdditionalParameterList.at(i)->isCurrentListValid() == false) {
            return false;
        }
    }
    return true;
}


int
GNEAdditionalFrame::additionalParameters::getNumberOfAddedAttributes() const {
    return (myIndexParameter + myIndexParameterList);
}


long
GNEAdditionalFrame::additionalParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create help dialog
    FXDialogBox* helpDialog = new FXDialogBox(this, ("Parameters of " + toString(myAdditionalTag)).c_str(), GUIDesignDialogBox);
    // Create FXTable
    FXTable* myTable = new FXTable(helpDialog, this, MID_TABLE, TABLE_READONLY);
    myTable->setVisibleRows((FXint)(myIndexParameter + myIndexParameterList));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(myIndexParameter + myIndexParameterList), 3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Name");
    myTable->setColumnText(1, "Value");
    myTable->setColumnText(2, "Definition");
    myTable->getRowHeader()->setWidth(0);
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, 80);
    int maxSizeColumnDefinitions = 0;
    // Iterate over vector of additional parameters
    for (int i = 0; i < myIndexParameter; i++) {
        SumoXMLTag tag = myVectorOfsingleAdditionalParameter.at(i)->getTag();
        SumoXMLAttr attr = myVectorOfsingleAdditionalParameter.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(tag, attr)) {
            type->setText("int");
        } else if (GNEAttributeCarrier::isFloat(tag, attr)) {
            type->setText("float");
        } else if (GNEAttributeCarrier::isTime(tag, attr)) {
            type->setText("time");
        } else if (GNEAttributeCarrier::isBool(tag, attr)) {
            type->setText("bool");
        } else if (GNEAttributeCarrier::isString(tag, attr)) {
            type->setText("string");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(myAdditionalTag, attr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(myAdditionalTag, attr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(myAdditionalTag, attr).size());
        }
    }
    // Iterate over vector of additional parameters list
    for (int i = 0; i < myIndexParameterList; i++) {
        SumoXMLTag tag = myVectorOfsingleAdditionalParameterList.at(i)->getTag();
        SumoXMLAttr attr = myVectorOfsingleAdditionalParameterList.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(tag, attr)) {
            type->setText("list of integers");
        } else if (GNEAttributeCarrier::isFloat(tag, attr)) {
            type->setText("list of floats");
        } else if (GNEAttributeCarrier::isTime(tag, attr)) {
            type->setText("list of times");
        } else if (GNEAttributeCarrier::isBool(tag, attr)) {
            type->setText("list of booleans");
        } else if (GNEAttributeCarrier::isString(tag, attr)) {
            type->setText("list of strings");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(myAdditionalTag, attr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(myAdditionalTag, attr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(myAdditionalTag, attr).size());
        }
    }
    // Set size of column
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, maxSizeColumnDefinitions * 6);
    // Button Close
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonDialog, 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::editorParameters- methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::editorParameters::editorParameters(FXComposite* parent) :
    FXGroupBox(parent, "editor parameters", GUIDesignGroupBoxFrame),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT),
    myCurrentLengthValid(true) {
    // Create FXListBox for the reference points
    myReferencePointMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT, GUIDesignComboBox);

    // Create Frame for Label and TextField
    FXHorizontalFrame* lengthFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);

    // Create length label
    myLengthLabel = new FXLabel(lengthFrame, "Length:", 0, GUIDesignLabelAttribute);

    // Create length text field
    myLengthTextField = new FXTextField(lengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, GUIDesignTextField);

    // Set default value of length
    myLengthTextField->setText("10");

    // Create FXMenuCheck for the force option
    myCheckForcePosition = new FXMenuCheck(this, "Force position", this, MID_GNE_MODE_ADDITIONAL_FORCEPOSITION, GUIDesignMenuCheck);

    // Create FXMenuCheck for the force option
    myCheckBlock = new FXMenuCheck(this, "Block movement", this, MID_GNE_SET_BLOCKING, GUIDesignMenuCheck);

    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);

    // Add options to myReferencePointMatchBox
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");

    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEAdditionalFrame::editorParameters::~editorParameters() {}


void
GNEAdditionalFrame::editorParameters::showLengthField() {
    myLengthLabel->show();
    myLengthTextField->show();
}


void
GNEAdditionalFrame::editorParameters::hideLengthField() {
    myLengthLabel->hide();
    myLengthTextField->hide();
}


void
GNEAdditionalFrame::editorParameters::showReferencePoint() {
    myReferencePointMatchBox->show();
    myCheckForcePosition->show();
}


void
GNEAdditionalFrame::editorParameters::hideReferencePoint() {
    myReferencePointMatchBox->hide();
    myCheckForcePosition->hide();
}


GNEAdditionalFrame::editorParameters::additionalReferencePoint
GNEAdditionalFrame::editorParameters::getActualReferencePoint() {
    return myActualAdditionalReferencePoint;
}


SUMOReal
GNEAdditionalFrame::editorParameters::getLenght() {
    return GNEAttributeCarrier::parse<SUMOReal>(myLengthTextField->getText().text());
}


bool
GNEAdditionalFrame::editorParameters::isBlockEnabled() {
    return myCheckBlock->getCheck() == 1 ? true : false;
}


bool
GNEAdditionalFrame::editorParameters::isForcePositionEnabled() {
    return myCheckForcePosition->getCheck() == 1 ? true : false;
}

bool
GNEAdditionalFrame::editorParameters::isCurrentLenghtValid() const {
    return myCurrentLengthValid;
}


long
GNEAdditionalFrame::editorParameters::onCmdSetLength(FXObject*, FXSelector, void*) {
    // change color of text field depending of the input lenght
    if (TplCheck::_str2SUMOReal(myLengthTextField->getText().text()) &&
            TplConvert::_str2SUMOReal(myLengthTextField->getText().text()) > 0) {
        myLengthTextField->setTextColor(FXRGB(0, 0, 0));
        myLengthTextField->killFocus();
        myCurrentLengthValid = true;
    } else {
        myLengthTextField->setTextColor(FXRGB(255, 0, 0));
        myCurrentLengthValid = false;
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEAdditionalFrame::editorParameters::onCmdSelectReferencePoint(FXObject*, FXSelector, void*) {
    // Cast actual reference point type
    myActualAdditionalReferencePoint = static_cast<additionalReferencePoint>(myReferencePointMatchBox->getCurrentItem());
    return 1;
}


long
GNEAdditionalFrame::editorParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "Parameter editor Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << "Referece point: Mark the initial position of the additional element.\n"
            << "Example: If you want to create a busStop with a lenght of 30 in the point 100 of the lane:\n"
            << "- Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "- Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "- Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "Force position: if is enabled, will create the additional adapting size of additional element to lane.\n"
            << "Example: If you have a lane with lenght = 100, but you try to create a busStop with size = 50\n"
            << "in the position 80 of the lane, a busStop with startPos = 80 and endPos = 100 will be created\n"
            << "instead of a busStop with startPos = 80 and endPos = 130.\n"
            << "\n"
            << "Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "the mouse. This option can be modified with the Inspector.";
    new FXLabel(helpDialog, help.str().c_str(), 0, GUIDesignLabelLeft);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonDialog, 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}


std::string
GNEAdditionalFrame::getIdsSelected(const FXList* list) {
    // Obtain Id's of list
    std::string vectorOfIds;
    for (int i = 0; i < list->getNumItems(); i++) {
        if (list->isItemSelected(i)) {
            if (vectorOfIds.size() > 0) {
                vectorOfIds += " ";
            }
            vectorOfIds += (list->getItem(i)->getText()).text();
        }
    }
    return vectorOfIds;
}



// ---------------------------------------------------------------------------
// GNEAdditionalFrame::additionalParentSelector - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalParentSelector::additionalParentSelector(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Additional Set selector", GUIDesignGroupBoxFrame),
    myUniqueSelection(false),
    myViewNet(viewNet) {

    // Create label with the type of additionalParentSelector
    mySetLabel = new FXLabel(this, "No additional selected", 0, GUIDesignLabelLeftThick);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTADDITIONALPARENT, GUIDesignList, 0, 0, 0, 100);

    // Create help button
    myHelpAdditionalParentSelector = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);

    // Hide List
    hideListOfAdditionals();
}


GNEAdditionalFrame::additionalParentSelector::~additionalParentSelector() {}


std::string
GNEAdditionalFrame::additionalParentSelector::getIdSelected() const {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->isItemSelected(i)) {
            return myList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEAdditionalFrame::additionalParentSelector::showListOfAdditionals(SumoXMLTag type, bool uniqueSelection) {
    myUniqueSelection = uniqueSelection;
    mySetLabel->setText(("" + toString(type)).c_str());
    myList->clearItems();
    // obtain all additionals of class "type"
    std::vector<GNEAdditional*> vectorOfAdditionalParents = myViewNet->getNet()->getAdditionals(type);
    // fill list with IDs of additionals
    for (std::vector<GNEAdditional*>::iterator i = vectorOfAdditionalParents.begin(); i != vectorOfAdditionalParents.end(); i++) {
        myList->appendItem((*i)->getID().c_str());
    }
    show();
}


void
GNEAdditionalFrame::additionalParentSelector::hideListOfAdditionals() {
    hide();
}


long
GNEAdditionalFrame::additionalParentSelector::onCmdSelectAdditionalParent(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::additionalParentSelector::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::edgeParentsSelector - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::edgeParentsSelector::edgeParentsSelector(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Edges", GUIDesignGroupBoxFrame),
    myViewNet(viewNet) {
    // Create menuCheck for selected edges
    myUseSelectedEdges = new FXMenuCheck(this, "Use selected Edges", this, MID_GNE_SHOWONLYSELECTEDEDGES, GUIDesignMenuCheck);

    // Create search box
    myEdgesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SEARCHEDGE, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTEDGE, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "Clear", 0, this, MID_GNE_CLEAREDGESELECTION, GUIDesignButtonHelp);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "Invert", 0, this, MID_GNE_INVERTEDGESELECTION, GUIDesignButtonHelp);

    // Create help button
    myHelpedgeParentsSelector = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);

    // Hide List
    hideList();
}


GNEAdditionalFrame::edgeParentsSelector::~edgeParentsSelector() {}


std::string
GNEAdditionalFrame::edgeParentsSelector::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}


void
GNEAdditionalFrame::edgeParentsSelector::showList(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // get all edges of net
    /// @todo this function must be improved.
    std::vector<GNEEdge*> vectorOfEdges = myViewNet->getNet()->retrieveEdges(false);
    // iterate over edges of net
    for (std::vector<GNEEdge*>::iterator i = vectorOfEdges.begin(); i != vectorOfEdges.end(); i++) {
        // If search criterium is correct, then append ittem
        if ((*i)->getID().find(search) != std::string::npos) {
            myList->appendItem((*i)->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedEdges isn't checked
    myUseSelectedEdges->setCheck(false);
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    // Show dialog
    show();
}


void
GNEAdditionalFrame::edgeParentsSelector::hideList() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::edgeParentsSelector::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myViewNet->getNet()->retrieveEdges(true).size() > 0) {
        myUseSelectedEdges->enable();
    } else {
        myUseSelectedEdges->disable();
    }
}


bool
GNEAdditionalFrame::edgeParentsSelector::isUseSelectedEdgesEnable() const {
    if (myUseSelectedEdges->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNEAdditionalFrame::edgeParentsSelector::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    if (myUseSelectedEdges->getCheck()) {
        myEdgesSearch->hide();
        myList->hide();
        myClearEdgesSelection->hide();
        myInvertEdgesSelection->hide();
        myHelpedgeParentsSelector->hide();
    } else {
        myEdgesSearch->show();
        myList->show();
        myClearEdgesSelection->show();
        myInvertEdgesSelection->show();
        myHelpedgeParentsSelector->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNEAdditionalFrame::edgeParentsSelector::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of edgeParentsSelector that contains the searched string
    showList(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::edgeParentsSelector::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::edgeParentsSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::edgeParentsSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::edgeParentsSelector::onCmdHelp(FXObject*, FXSelector, void*) {
    std::cout << "IMPLEMENT" << std::endl;
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::laneParentsSelector - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::laneParentsSelector::laneParentsSelector(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Lanes", GUIDesignGroupBoxFrame),
    myViewNet(viewNet) {
    // Create CheckBox for selected lanes
    myUseSelectedLanes = new FXMenuCheck(this, "Use selected Lanes", this, MID_GNE_USESELECTEDLANES, GUIDesignMenuCheck);

    // Create search box
    myLanesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SEARCHLANE, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTLANE, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_CLEARLANESELECTION, GUIDesignButtonHelp);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_INVERTLANESELECTION, GUIDesignButtonHelp);

    // Create help button
    helpLanes = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);

    // Hide List
    hideList();
}


GNEAdditionalFrame::laneParentsSelector::~laneParentsSelector() {}


std::string
GNEAdditionalFrame::laneParentsSelector::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}


void
GNEAdditionalFrame::laneParentsSelector::showList(std::string search) {
    myList->clearItems();
    std::vector<GNELane*> vectorOfLanes = myViewNet->getNet()->retrieveLanes(false);
    for (std::vector<GNELane*>::iterator i = vectorOfLanes.begin(); i != vectorOfLanes.end(); i++) {
        if ((*i)->getID().find(search) != std::string::npos) {
            myList->appendItem((*i)->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedLanes isn't checked
    myUseSelectedLanes->setCheck(false);
    // Show list
    show();
}


void
GNEAdditionalFrame::laneParentsSelector::hideList() {
    hide();
}


void
GNEAdditionalFrame::laneParentsSelector::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myViewNet->getNet()->retrieveLanes(true).size() > 0) {
        myUseSelectedLanes->enable();
    } else {
        myUseSelectedLanes->disable();
    }
}


bool
GNEAdditionalFrame::laneParentsSelector::isUseSelectedLanesEnable() const {
    if (myUseSelectedLanes->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNEAdditionalFrame::laneParentsSelector::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
    if (myUseSelectedLanes->getCheck()) {
        myLanesSearch->hide();
        myList->hide();
        clearLanesSelection->hide();
        invertLanesSelection->hide();
        helpLanes->hide();
    } else {
        myLanesSearch->show();
        myList->show();
        clearLanesSelection->show();
        invertLanesSelection->show();
        helpLanes->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNEAdditionalFrame::laneParentsSelector::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of laneParentsSelector that contains the searched string
    showList(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::laneParentsSelector::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::laneParentsSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::laneParentsSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::laneParentsSelector::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
