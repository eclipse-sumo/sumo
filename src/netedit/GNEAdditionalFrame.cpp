/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
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


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame) GNEAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDITIONALFRAME_SELECTADDITIONALTYPE,    GNEAdditionalFrame::onCmdSelectAdditional),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributeSingle) GNEsingleAdditionalParameterMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT,   GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_BOOL,   GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetBooleanAttribute),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributeList) GNEsingleAdditionalParameterListMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_ADDROW,                 GNEAdditionalFrame::AdditionalAttributeList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_REMOVEROW,              GNEAdditionalFrame::AdditionalAttributeList::onCmdRemoveRow),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributes) GNEadditionalParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEAdditionalFrame::AdditionalAttributes::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::NeteditAttributes) GNEEditorParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT,   GNEAdditionalFrame::NeteditAttributes::onCmdSetLength),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_REFERENCEPOINT,         GNEAdditionalFrame::NeteditAttributes::onCmdSelectReferencePoint),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_BLOCKMOVEMENT,          GNEAdditionalFrame::NeteditAttributes::onCmdSetBlocking),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                       GNEAdditionalFrame::NeteditAttributes::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::SelectorParentAdditional) GNEadditionalParentSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECTADDITIONALPARENT, GNEAdditionalFrame::SelectorParentAdditional::onCmdSelectAdditionalParent),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                       GNEAdditionalFrame::SelectorParentAdditional::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::SelectorParentEdges) GNEedgeParentsSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SHOWONLYSELECTEDEDGES,  GNEAdditionalFrame::SelectorParentEdges::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEAREDGESELECTION,     GNEAdditionalFrame::SelectorParentEdges::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTEDGESELECTION,    GNEAdditionalFrame::SelectorParentEdges::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCHEDGE,             GNEAdditionalFrame::SelectorParentEdges::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECTEDGE,             GNEAdditionalFrame::SelectorParentEdges::onCmdSelectEdge),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                       GNEAdditionalFrame::SelectorParentEdges::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::SelectorParentLanes) GNElaneParentsSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTEDLANES,       GNEAdditionalFrame::SelectorParentLanes::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARLANESELECTION,     GNEAdditionalFrame::SelectorParentLanes::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTLANESELECTION,    GNEAdditionalFrame::SelectorParentLanes::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCHLANE,             GNEAdditionalFrame::SelectorParentLanes::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECTLANE,             GNEAdditionalFrame::SelectorParentLanes::onCmdSelectLane),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                       GNEAdditionalFrame::SelectorParentLanes::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame,                                 FXVerticalFrame,    GNEAdditionalMap,                       ARRAYNUMBER(GNEAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributeSingle,      FXHorizontalFrame,  GNEsingleAdditionalParameterMap,        ARRAYNUMBER(GNEsingleAdditionalParameterMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributeList,        FXVerticalFrame,    GNEsingleAdditionalParameterListMap,    ARRAYNUMBER(GNEsingleAdditionalParameterListMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributes,           FXGroupBox,         GNEadditionalParametersMap,             ARRAYNUMBER(GNEadditionalParametersMap))
FXIMPLEMENT(GNEAdditionalFrame::NeteditAttributes,              FXGroupBox,         GNEEditorParametersMap,                 ARRAYNUMBER(GNEEditorParametersMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentAdditional,       FXGroupBox,         GNEadditionalParentSelectorMap,         ARRAYNUMBER(GNEadditionalParentSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentEdges,            FXGroupBox,         GNEedgeParentsSelectorMap,              ARRAYNUMBER(GNEedgeParentsSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentLanes,            FXGroupBox,         GNElaneParentsSelectorMap,              ARRAYNUMBER(GNElaneParentsSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalFrame::GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Additionals"),
    myActualAdditionalType(SUMO_TAG_NOTHING) {

    // Create groupBox for myAdditionalMatchBox
    myGroupBoxForMyAdditionalMatchBox = new FXGroupBox(myContentFrame, "Additional element", GUIDesignGroupBoxFrame);

    // Create FXListBox in myGroupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(myGroupBoxForMyAdditionalMatchBox, GUIDesignComboBoxNCol, this, MID_GNE_ADDITIONALFRAME_SELECTADDITIONALTYPE, GUIDesignComboBox);

    // Create additional parameters
    myadditionalParameters = new GNEAdditionalFrame::AdditionalAttributes(myViewNet, myContentFrame);

    // Create Netedit parameter
    myEditorParameters = new GNEAdditionalFrame::NeteditAttributes(myContentFrame);

    // Create create list for additional Set
    myAdditionalParentSelector = new GNEAdditionalFrame::SelectorParentAdditional(myContentFrame, myViewNet);

    /// Create list for SelectorParentEdges
    myedgeParentsSelector = new GNEAdditionalFrame::SelectorParentEdges(myContentFrame, myViewNet);

    /// Create list for SelectorParentLanes
    mylaneParentsSelector = new GNEAdditionalFrame::SelectorParentLanes(myContentFrame, myViewNet);

    // Add options to myAdditionalMatchBox
    for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
        myAdditionalMatchBox->appendItem(toString(i).c_str());
    }

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems());

    // If there are additionals
    if (GNEAttributeCarrier::allowedAdditionalTags().size() > 0) {
        // Set myActualAdditionalType and show
        myActualAdditionalType = GNEAttributeCarrier::allowedAdditionalTags().front();
        setParametersOfAdditional(myActualAdditionalType);
    }
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    gSelected.remove2Update();
}


GNEAdditionalFrame::AddAdditionalResult
GNEAdditionalFrame::addAdditional(GNENetElement* netElement, GUISUMOAbstractView* abstractViewParent) {
    // check if current selected additional is valid
    if (myActualAdditionalType == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }

    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement = myadditionalParameters->getAttributesAndValues();

    // limit position depending if show grid is enabled
    Position currentPosition = abstractViewParent->snapToActiveGrid(abstractViewParent->getPositionInformation());

    // Declare pointer to netElements
    GNEJunction* pointed_junction = NULL;
    GNEEdge* pointed_edge = NULL;
    GNELane* pointed_lane = NULL;
    GNECrossing* pointed_crossing = NULL;

    // Check if additional should be placed over a junction
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_JUNCTION)) {
        pointed_junction = dynamic_cast<GNEJunction*>(netElement);
        if (pointed_junction != NULL) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myadditionalParameters->areValuesValid() == false) {
                myadditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute junction
            valuesOfElement[SUMO_ATTR_JUNCTION] = pointed_junction->getID();
            // Generate id of element based on the junction
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_junction);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    }
    // Check if additional should be placed over a edge
    else if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGE)) {
        // Due a edge is composed of lanes, its neccesary check if clicked element is an lane
        if (dynamic_cast<GNELane*>(netElement) != NULL) {
            pointed_edge = &(dynamic_cast<GNELane*>(netElement)->getParentEdge());
        }
        if (pointed_edge != NULL) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myadditionalParameters->areValuesValid() == false) {
                myadditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute edge
            valuesOfElement[SUMO_ATTR_EDGE] = pointed_edge->getID();
            // Generate id of element based on the edge
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_edge);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    }
    // Check if additional should be placed over a lane
    else if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANE)) {
        pointed_lane = dynamic_cast<GNELane*>(netElement);
        if (pointed_lane != NULL) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myadditionalParameters->areValuesValid() == false) {
                myadditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute lane
            valuesOfElement[SUMO_ATTR_LANE] = pointed_lane->getID();
            // Generate id of element based on the lane
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_lane);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    }
    // Check if additional should be placed over a crossing
    else if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_CROSSING)) {
        pointed_crossing = dynamic_cast<GNECrossing*>(netElement);
        if (pointed_crossing != NULL) {
            // show warning dialogbox and stop check if input parameters are valid
            if (myadditionalParameters->areValuesValid() == false) {
                myadditionalParameters->showWarningMessage();
                return ADDADDITIONAL_INVALID_ARGUMENTS;
            }
            // Get attribute crossing
            valuesOfElement[SUMO_ATTR_CROSSING] = pointed_crossing->getID();
            // Generate id of element based on the crossing
            valuesOfElement[SUMO_ATTR_ID] = generateID(pointed_crossing);
        } else {
            return ADDADDITIONAL_INVALID_PARENT;
        }
    } else {
        // Generate id of element
        valuesOfElement[SUMO_ATTR_ID] = generateID(NULL);
    }

    // show warning dialogbox and stop check if input parameters are valid
    if (myadditionalParameters->areValuesValid() == false) {
        myadditionalParameters->showWarningMessage();
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }

    // Obtain position attribute if wasn't previously setted
    if (valuesOfElement.find(SUMO_ATTR_POSITION) == valuesOfElement.end()) {
        if (pointed_edge) {
            // Obtain position of the mouse over edge
            double positionOfTheMouseOverEdge = pointed_edge->getLanes().at(0)->getShape().nearest_offset_to_point2D(currentPosition);
            // If element has a StartPosition and EndPosition over edge, extract attributes
            if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_ENDPOS)) {
                // First check that current length is valid
                if (myEditorParameters->isCurrentLengthValid()) {
                    // check if current reference point is valid
                    if (myEditorParameters->getActualReferencePoint() == NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                        myadditionalParameters->showWarningMessage("Current selected reference point isn't valid");
                        return ADDADDITIONAL_INVALID_ARGUMENTS;
                    } else {
                        // set start and end position
                        valuesOfElement[SUMO_ATTR_STARTPOS] = toString(setStartPosition(positionOfTheMouseOverEdge, myEditorParameters->getLength()));
                        valuesOfElement[SUMO_ATTR_ENDPOS] = toString(setEndPosition(pointed_edge->getLanes().at(0)->getLaneShapeLength(), positionOfTheMouseOverEdge, myEditorParameters->getLength()));
                    }
                } else {
                    return ADDADDITIONAL_INVALID_ARGUMENTS;
                }
            }
            // Extract position of lane
            valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverEdge);
        } else if (pointed_lane) {
            // Obtain position of the mouse over lane
            double positionOfTheMouseOverLane = pointed_lane->getShape().nearest_offset_to_point2D(currentPosition);
            // If element has a StartPosition and EndPosition over lane, extract attributes
            if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_ENDPOS)) {
                // First check that current length is valid
                if (myEditorParameters->isCurrentLengthValid()) {
                    // check if current reference point is valid
                    if (myEditorParameters->getActualReferencePoint() == NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                        myadditionalParameters->showWarningMessage("Current selected reference point isn't valid");
                        return ADDADDITIONAL_INVALID_ARGUMENTS;
                    } else {
                        // set start and end position
                        valuesOfElement[SUMO_ATTR_STARTPOS] = toString(setStartPosition(positionOfTheMouseOverLane, myEditorParameters->getLength()));
                        valuesOfElement[SUMO_ATTR_ENDPOS] = toString(setEndPosition(pointed_lane->getLaneShapeLength(), positionOfTheMouseOverLane, myEditorParameters->getLength()));
                    }
                } else {
                    return ADDADDITIONAL_INVALID_ARGUMENTS;
                }
            }
            // Extract position of lane
            valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverLane);
        } else {
            // get position in map
            valuesOfElement[SUMO_ATTR_POSITION] = toString(currentPosition);
        }
    }

    // If additional has a interval defined by a begin or end, check that is valid
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_STARTTIME) && GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_END)) {
        double begin = GNEAttributeCarrier::parse<double>(valuesOfElement[SUMO_ATTR_STARTTIME]);
        double end = GNEAttributeCarrier::parse<double>(valuesOfElement[SUMO_ATTR_END]);
        if (begin > end) {
            myadditionalParameters->showWarningMessage("Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' cannot be greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.txt
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_FILE) && valuesOfElement[SUMO_ATTR_FILE] == "") {
        valuesOfElement[SUMO_ATTR_FILE] = (valuesOfElement[SUMO_ATTR_ID] + ".txt");
    }

    // If additional own the attribute SUMO_ATTR_OUTPUT but was't defined, will defined as <ID>.txt
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_OUTPUT) && valuesOfElement[SUMO_ATTR_OUTPUT] == "") {
        valuesOfElement[SUMO_ATTR_OUTPUT] = (valuesOfElement[SUMO_ATTR_ID] + ".txt");
    }

    // Save block value if additional can be blocked
    if (GNEAttributeCarrier::canBlockMovement(myActualAdditionalType)) {
        valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myEditorParameters->isBlockEnabled());
    }

    // If element belongst to an additional Set, get id of parent from myAdditionalParentSelector
    if ((myActualAdditionalType == SUMO_TAG_DET_ENTRY) || (myActualAdditionalType == SUMO_TAG_DET_EXIT)) {
        if (myAdditionalParentSelector->getIdSelected() != "") {
            valuesOfElement[GNE_ATTR_PARENT] = myAdditionalParentSelector->getIdSelected();
        } else {
            myadditionalParameters->showWarningMessage("A " + toString(SUMO_TAG_E3DETECTOR) + " must be selected before insertion of " + toString(myActualAdditionalType) + ".");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // If element own a list of SelectorParentEdges as attribute
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGES)) {
        if (myedgeParentsSelector->isUseSelectedEdgesEnable()) {
            // Declare a vector of Id's
            std::vector<std::string> vectorOfIds;
            // get Selected edges
            std::vector<GNEEdge*> selectedEdges = myViewNet->getNet()->retrieveEdges(true);
            // Iterate over selectedEdges and getId
            for (auto i : selectedEdges) {
                vectorOfIds.push_back(i->getID());
            }
            // Set saved Ids in attribute edges
            valuesOfElement[SUMO_ATTR_EDGES] = joinToString(vectorOfIds, " ");
        } else {
            valuesOfElement[SUMO_ATTR_EDGES] = myedgeParentsSelector->getIdsSelected();
        }
        // check if attribute has at least one edge
        if (valuesOfElement[SUMO_ATTR_EDGES] == "") {
            myadditionalParameters->showWarningMessage("List of " + toString(SUMO_TAG_EDGE) + "s cannot be empty");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // If element own a list of SelectorParentLanes as attribute
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANES)) {
        if (mylaneParentsSelector->isUseSelectedLanesEnable()) {
            // Declare a vector of Id's
            std::vector<std::string> vectorOfIds;
            // get Selected lanes
            std::vector<GNELane*> selectedLanes = myViewNet->getNet()->retrieveLanes(true);
            // Iterate over selectedLanes and getId
            for (auto i : selectedLanes) {
                vectorOfIds.push_back(i->getID());
            }
            // Set saved Ids in attribute lanes
            valuesOfElement[SUMO_ATTR_LANES] = joinToString(vectorOfIds, " ");
        } else {
            valuesOfElement[SUMO_ATTR_LANES] = mylaneParentsSelector->getIdsSelected();
        }
        // check if attribute has at least a lane
        if (valuesOfElement[SUMO_ATTR_LANES] == "") {
            myadditionalParameters->showWarningMessage("List of " + toString(SUMO_TAG_LANE) + "s cannot be empty");
            return ADDADDITIONAL_INVALID_ARGUMENTS;
        }
    }

    // Create additional
    if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myActualAdditionalType, valuesOfElement)) {
        return ADDADDITIONAL_SUCCESS;
    } else {
        return ADDADDITIONAL_INVALID_ARGUMENTS;
    }
}

void
GNEAdditionalFrame::removeAdditional(GNEAdditional* additional) {
    myViewNet->getUndoList()->p_begin("delete " + toString(additional->getTag()));
    // save selection status
    if (gSelected.isSelected(GLO_ADDITIONAL, additional->getGlID())) {
        std::set<GUIGlID> deselected;
        deselected.insert(additional->getGlID());
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), deselected, true), true);
    }
    // first remove all additional childs of this additional calling this function recursively
    while (additional->getAdditionalChilds().size() > 0) {
        removeAdditional(additional->getAdditionalChilds().front());
    }
    // remove additional
    myViewNet->getUndoList()->add(new GNEChange_Additional(additional, false), true);
    myViewNet->getUndoList()->p_end();
}


long
GNEAdditionalFrame::onCmdSelectAdditional(FXObject*, FXSelector, void*) {
    // declare flag to save if additional name is correct
    bool additionalNameCorrect = false;
    // set parameters of additional, if it's correct
    for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
        if (toString(i) == myAdditionalMatchBox->getText().text()) {
            myAdditionalMatchBox->setTextColor(FXRGB(0, 0, 0));
            myadditionalParameters->show();
            if (GNEAttributeCarrier::canBlockMovement(i)) {
                myEditorParameters->show();
            } else {
                myEditorParameters->hide();
            }
            setParametersOfAdditional(i);
            additionalNameCorrect = true;
        }
    }
    // if additional name isn't correct, hidde all
    if (additionalNameCorrect == false) {
        myActualAdditionalType = SUMO_TAG_NOTHING;
        myAdditionalMatchBox->setTextColor(FXRGB(255, 0, 0));
        myadditionalParameters->hide();
        myEditorParameters->hide();
        myAdditionalParentSelector->hide();
        myedgeParentsSelector->hide();
        mylaneParentsSelector->hide();
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
    // Clear internal attributes
    myadditionalParameters->clearAttributes();
    // Hide length field and reference point
    myEditorParameters->hideLengthFieldAndReferecePoint();
    // Iterate over attributes of myActualAdditionalType
    for (auto i : GNEAttributeCarrier::allowedAttributes(myActualAdditionalType)) {
        if (!GNEAttributeCarrier::isUnique(myActualAdditionalType, i.first)) {
            myadditionalParameters->addAttribute(myActualAdditionalType, i.first);
        } else if (i.first == SUMO_ATTR_ENDPOS) {
            myEditorParameters->showLengthFieldAndReferecePoint();
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
    // Show SelectorParentEdges if we're adding an additional that own the attribute SUMO_ATTR_EDGES
    if (GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGES)) {
        myedgeParentsSelector->showList();
    } else {
        myedgeParentsSelector->hideList();
    }
    // Show SelectorParentLanes if we're adding an additional that own the attribute SUMO_ATTR_LANES
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


double
GNEAdditionalFrame::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) {
    switch (myEditorParameters->getActualReferencePoint()) {
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNEAdditionalFrame::setEndPosition(double /*laneLength*/, double positionOfTheMouseOverLane, double lengthOfAdditional) {
    switch (myEditorParameters->getActualReferencePoint()) {
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case NeteditAttributes::GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributeSingle - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributeSingle::AdditionalAttributeSingle(FXComposite* parent) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myAdditionalTag(SUMO_TAG_NOTHING),
    myAdditionalAttr(SUMO_ATTR_NOTHING) {
    // Create visual elements
    myLabel = new FXLabel(this, "name", 0, GUIDesignLabelAttribute);
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextFieldInt);
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextFieldReal);
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextField);
    myBoolCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_BOOL, GUIDesignCheckButtonAttribute);
    // Hide elements
    hideParameter();
}


GNEAdditionalFrame::AdditionalAttributeSingle::~AdditionalAttributeSingle() {}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::string value) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
    myTextFieldStrings->setText(value.c_str());
    myTextFieldStrings->show();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, int value) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
    myTextFieldInt->setText(toString(value).c_str());
    myTextFieldInt->show();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, double value) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
    myTextFieldReal->setText(toString(value).c_str());
    myTextFieldReal->show();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, bool value) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    if (value) {
        myBoolCheckButton->setCheck(true);
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setCheck(false);
        myBoolCheckButton->setText("false");
    }
    myBoolCheckButton->show();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::hideParameter() {
    myAdditionalTag = SUMO_TAG_NOTHING;
    myAdditionalAttr = SUMO_ATTR_NOTHING;
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myBoolCheckButton->hide();
    hide();
}


SumoXMLTag
GNEAdditionalFrame::AdditionalAttributeSingle::getTag() const {
    return myAdditionalTag;
}


SumoXMLAttr
GNEAdditionalFrame::AdditionalAttributeSingle::getAttr() const {
    return myAdditionalAttr;
}


std::string
GNEAdditionalFrame::AdditionalAttributeSingle::getValue() const {
    if (GNEAttributeCarrier::isBool(myAdditionalTag, myAdditionalAttr)) {
        return (myBoolCheckButton->getCheck() == 1) ? "true" : "false";
    } else if (GNEAttributeCarrier::isInt(myAdditionalTag, myAdditionalAttr)) {
        return myTextFieldInt->getText().text();
    } else if (GNEAttributeCarrier::isFloat(myAdditionalTag, myAdditionalAttr) || GNEAttributeCarrier::isTime(myAdditionalTag, myAdditionalAttr)) {
        return myTextFieldReal->getText().text();
    } else {
        return myTextFieldStrings->getText().text();
    }
}


const std::string&
GNEAdditionalFrame::AdditionalAttributeSingle::isAttributeValid() const {
    return myInvalidValue;
}


long
GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // Check if format of current value of myTextField is correct
    if (GNEAttributeCarrier::isInt(myAdditionalTag, myAdditionalAttr)) {
        if (GNEAttributeCarrier::canParse<int>(myTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myTextFieldInt->getText().text());
            // Check if int value must be positive
            if (GNEAttributeCarrier::isPositive(myAdditionalTag, myAdditionalAttr) && (intValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'int' format";
        }
    } else if (GNEAttributeCarrier::isTime(myAdditionalTag, myAdditionalAttr)) {
        // time attributes work as positive doubles
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if parsed value is negative
            if (doubleValue < 0) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'time' format";
        }
    } else if (GNEAttributeCarrier::isFloat(myAdditionalTag, myAdditionalAttr)) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if double value must be positive
            if (GNEAttributeCarrier::isPositive(myAdditionalTag, myAdditionalAttr) && (doubleValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
                // check if double value is a probability
            } else if (GNEAttributeCarrier::isProbability(myAdditionalTag, myAdditionalAttr) && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' takes only values between 0 and 1";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'float' format";
        }
    } else if (GNEAttributeCarrier::isFilename(myAdditionalTag, myAdditionalAttr)) {
        // check if filename format is valid
        if (GNEAttributeCarrier::isValidFilename(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        }
    } else if (GNEAttributeCarrier::isVClass(myAdditionalTag, myAdditionalAttr)) {
        // check if lists of Vclass are valid
        if (canParseVehicleClasses(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "list of VClass isn't valid";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_ROUTEPROBE) {
        // check if filename format is valid
        if (GNEAttributeCarrier::isValidID(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "RouteProbe ID contains invalid characters";
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myInvalidValue.size() == 0) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->killFocus();
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->killFocus();
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetBooleanAttribute(FXObject*, FXSelector, void*) {
    if (myBoolCheckButton->getCheck()) {
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setText("false");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributeList - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributeList::AdditionalAttributeList(FXComposite* parent) :
    FXVerticalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myAdditionalTag(SUMO_TAG_NOTHING),
    myAdditionalAttr(SUMO_ATTR_NOTHING),
    myNumberOfVisibleTextfields(1),
    myMaxNumberOfValuesInParameterList(20) {
    // Create elements
    for (int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myHorizontalFrames.push_back(new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame));
        myLabels.push_back(new FXLabel(myHorizontalFrames.back(), "name", 0, GUIDesignLabelAttribute));
        myTextFields.push_back(new FXTextField(myHorizontalFrames.back(), GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextField));
    }
    // Create label Row
    myHorizontalFrameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabels.push_back(new FXLabel(myHorizontalFrameButtons, "Rows", 0, GUIDesignLabelAttribute));
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(myHorizontalFrameButtons, GUIDesignAuxiliarHorizontalFrame);
    // Create add button
    myAddButton = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_ADDITIONALFRAME_ADDROW, GUIDesignButtonIcon);
    // Create remove buttons
    myRemoveButton = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), this, MID_GNE_ADDITIONALFRAME_REMOVEROW, GUIDesignButtonIcon);
    // Hide all para meters
    hideParameter();
}


GNEAdditionalFrame::AdditionalAttributeList::~AdditionalAttributeList() {}


void
GNEAdditionalFrame::AdditionalAttributeList::showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<int> /* value */) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    // @todo finish
}


void
GNEAdditionalFrame::AdditionalAttributeList::showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<double> /* value */, bool /*isTime*/) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    // @todo finish
}


void
GNEAdditionalFrame::AdditionalAttributeList::showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<bool> /* value */) {
    myAdditionalTag = additionalTag;
    myAdditionalAttr = additionalAttr;
    // @todo finish
}


void
GNEAdditionalFrame::AdditionalAttributeList::showListParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::vector<std::string> value) {
    if ((int)value.size() < myMaxNumberOfValuesInParameterList) {
        myAdditionalTag = additionalTag;
        myAdditionalAttr = additionalAttr;
        myNumberOfVisibleTextfields = (int)value.size();
        if (myNumberOfVisibleTextfields == 0) {
            myNumberOfVisibleTextfields++;
        }
        for (int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
            myLabels.at(i)->setText((toString(myAdditionalAttr) + ": " + toString(i)).c_str());
        }
        for (int i = 0; i < myNumberOfVisibleTextfields; i++) {
            myHorizontalFrames.at(i)->show();
        }
        myHorizontalFrameButtons->show();
        FXVerticalFrame::show();
    }
}


void
GNEAdditionalFrame::AdditionalAttributeList::hideParameter() {
    myAdditionalTag = SUMO_TAG_NOTHING;
    myAdditionalAttr = SUMO_ATTR_NOTHING;
    for (int i = 0; i < (int)myHorizontalFrames.size(); i++) {
        myHorizontalFrames.at(i)->hide();
    }
    myHorizontalFrameButtons->hide();
    FXVerticalFrame::hide();
}


SumoXMLTag
GNEAdditionalFrame::AdditionalAttributeList::getTag() const {
    return myAdditionalTag;
}

SumoXMLAttr
GNEAdditionalFrame::AdditionalAttributeList::getAttr() const {
    return myAdditionalAttr;
}


std::string
GNEAdditionalFrame::AdditionalAttributeList::getListValues() {
    // Declare, fill and return a string with the list values
    std::string value;
    for (int i = 0; i < myNumberOfVisibleTextfields; i++) {
        if (!myTextFields.at(i)->getText().empty()) {
            value += (myTextFields.at(i)->getText().text() + std::string(" "));
        }
    }
    return value;
}


bool
GNEAdditionalFrame::AdditionalAttributeList::isCurrentListValid() const {
    /// @todo finish check validity of values in lists
    return true;
}


long
GNEAdditionalFrame::AdditionalAttributeList::onCmdAddRow(FXObject*, FXSelector, void*) {
    if (myNumberOfVisibleTextfields < (myMaxNumberOfValuesInParameterList - 1)) {
        myHorizontalFrames.at(myNumberOfVisibleTextfields)->show();
        myNumberOfVisibleTextfields++;
        getParent()->recalc();
    }
    return 1;
}


long
GNEAdditionalFrame::AdditionalAttributeList::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    if (myNumberOfVisibleTextfields > 1) {
        myNumberOfVisibleTextfields--;
        myHorizontalFrames.at(myNumberOfVisibleTextfields)->hide();
        myTextFields.at(myNumberOfVisibleTextfields)->setText("");
        getParent()->recalc();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributes::AdditionalAttributes(GNEViewNet* viewNet, FXComposite* parent) :
    FXGroupBox(parent, "Internal attributes", GUIDesignGroupBoxFrame),
    myViewNet(viewNet),
    myIndexParameter(0),
    myIndexParameterList(0),
    myMaxNumberOfParameters(GNEAttributeCarrier::getHigherNumberOfAttributes()),
    myMaxNumberOfListParameters(2) {

    // Create single parameters
    for (int i = 0; i < myMaxNumberOfParameters; i++) {
        myVectorOfsingleAdditionalParameter.push_back(new AdditionalAttributeSingle(this));
    }

    // Create single list parameters
    for (int i = 0; i < myMaxNumberOfListParameters; i++) {
        myVectorOfsingleAdditionalParameterList.push_back(new AdditionalAttributeList(this));
    }

    // Create help button
    new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEAdditionalFrame::AdditionalAttributes::~AdditionalAttributes() {}


void
GNEAdditionalFrame::AdditionalAttributes::clearAttributes() {
    // Hidde al fields
    for (int i = 0; i < myMaxNumberOfParameters; i++) {
        myVectorOfsingleAdditionalParameter.at(i)->hideParameter();
    }

    // Hidde al list fields
    for (int i = 0; i < myMaxNumberOfListParameters; i++) {
        myVectorOfsingleAdditionalParameterList.at(i)->hideParameter();
    }

    // Reset indexs
    myIndexParameterList = 0;
    myIndexParameter = 0;
}


void
GNEAdditionalFrame::AdditionalAttributes::addAttribute(SumoXMLTag additionalTag, SumoXMLAttr AdditionalAttributeSingle) {
    // Set current additional
    myAdditionalTag = additionalTag;
    // If  parameter is of type list
    if (GNEAttributeCarrier::isList(myAdditionalTag, AdditionalAttributeSingle)) {
        // If parameter can be show
        if (myIndexParameterList < myMaxNumberOfListParameters) {
            // Check type of attribute list
            if (GNEAttributeCarrier::isInt(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue< std::vector<int> >(myAdditionalTag, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isFloat(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue< std::vector<double> >(myAdditionalTag, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isTime(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue< std::vector<double> >(myAdditionalTag, AdditionalAttributeSingle), true);
            } else if (GNEAttributeCarrier::isBool(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue< std::vector<bool> >(myAdditionalTag, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isString(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameterList.at(myIndexParameterList)->showListParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue< std::vector<std::string> >(myAdditionalTag, AdditionalAttributeSingle));
            }
            // Update index
            myIndexParameterList++;
        } else {
            WRITE_ERROR("Max number of list attributes reached (" + toString(myMaxNumberOfListParameters) + ").");
        }
    } else {
        if (myIndexParameter < myMaxNumberOfParameters) {
            // Check type of attribute list
            if (GNEAttributeCarrier::isInt(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<int>(myAdditionalTag, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isFloat(myAdditionalTag, AdditionalAttributeSingle) || GNEAttributeCarrier::isTime(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<double>(myAdditionalTag, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isBool(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<bool>(myAdditionalTag, AdditionalAttributeSingle));
            } else if (GNEAttributeCarrier::isString(myAdditionalTag, AdditionalAttributeSingle)) {
                myVectorOfsingleAdditionalParameter.at(myIndexParameter)->showParameter(myAdditionalTag, AdditionalAttributeSingle, GNEAttributeCarrier::getDefaultValue<std::string>(myAdditionalTag, AdditionalAttributeSingle));
            } else {
                WRITE_WARNING("Attribute '" + toString(AdditionalAttributeSingle) + "' doesn't have a defined type. Check definition in GNEAttributeCarrier");
            }
            // Update index parameter
            myIndexParameter++;
        } else {
            WRITE_ERROR("Max number of attributes reached (" + toString(myMaxNumberOfParameters) + ").");
        }
    }
}


void
GNEAdditionalFrame::AdditionalAttributes::showAdditionalParameters() {
    recalc();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributes::hideAdditionalParameters() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEAdditionalFrame::AdditionalAttributes::getAttributesAndValues() const {
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


void
GNEAdditionalFrame::AdditionalAttributes::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standar parameters
    for (int i = 0; (i < myIndexParameter) && errorMessage.empty(); i++) {
        // Return string with the error if at least one of the parameter isn't valid
        std::string attributeValue = myVectorOfsingleAdditionalParameter.at(i)->isAttributeValid();
        if (attributeValue.size() != 0) {
            errorMessage = attributeValue;
        }
    }
    // iterate over list parameters
    for (int i = 0; i < myIndexParameterList; i++) {
        // Return false if at least one of the list parameter isn't valid
        if (myVectorOfsingleAdditionalParameterList.at(i)->isCurrentListValid() == false) {
            ;
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + toString(myAdditionalTag) + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + toString(myAdditionalTag) + ": " + extra;
    }

    // set message in status bar
    myViewNet->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING(errorMessage);
    }
}


bool
GNEAdditionalFrame::AdditionalAttributes::areValuesValid() const {
    // iterate over standar parameters
    for (int i = 0; i < myIndexParameter; i++) {
        // Return false if error message of attriuve isn't empty
        if (myVectorOfsingleAdditionalParameter.at(i)->isAttributeValid().size() != 0) {
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
GNEAdditionalFrame::AdditionalAttributes::getNumberOfAddedAttributes() const {
    return (myIndexParameter + myIndexParameterList);
}


long
GNEAdditionalFrame::AdditionalAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
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
        SumoXMLTag additionalTag = myVectorOfsingleAdditionalParameter.at(i)->getTag();
        SumoXMLAttr additionalAttr = myVectorOfsingleAdditionalParameter.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(additionalAttr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(additionalTag, additionalAttr)) {
            type->setText("int");
        } else if (GNEAttributeCarrier::isFloat(additionalTag, additionalAttr)) {
            type->setText("float");
        } else if (GNEAttributeCarrier::isTime(additionalTag, additionalAttr)) {
            type->setText("time");
        } else if (GNEAttributeCarrier::isBool(additionalTag, additionalAttr)) {
            type->setText("bool");
        } else if (GNEAttributeCarrier::isColor(additionalTag, additionalAttr)) {
            type->setText("color");
        } else if (GNEAttributeCarrier::isString(additionalTag, additionalAttr)) {
            type->setText("string");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(additionalTag, additionalAttr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(additionalTag, additionalAttr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(additionalTag, additionalAttr).size());
        }
    }
    // Iterate over vector of additional parameters list
    for (int i = 0; i < myIndexParameterList; i++) {
        SumoXMLTag additionalTag = myVectorOfsingleAdditionalParameterList.at(i)->getTag();
        SumoXMLAttr additionalAttr = myVectorOfsingleAdditionalParameterList.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(additionalAttr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(additionalTag, additionalAttr)) {
            type->setText("list of integers");
        } else if (GNEAttributeCarrier::isFloat(additionalTag, additionalAttr)) {
            type->setText("list of floats");
        } else if (GNEAttributeCarrier::isTime(additionalTag, additionalAttr)) {
            type->setText("list of times");
        } else if (GNEAttributeCarrier::isBool(additionalTag, additionalAttr)) {
            type->setText("list of booleans");
        } else if (GNEAttributeCarrier::isString(additionalTag, additionalAttr)) {
            type->setText("list of strings");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(additionalTag, additionalAttr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(additionalTag, additionalAttr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(additionalTag, additionalAttr).size());
        }
    }
    // Set size of column
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, maxSizeColumnDefinitions * 6);
    // Button Close
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::NeteditAttributes::NeteditAttributes(FXComposite* parent) :
    FXGroupBox(parent, "Netedit attributes", GUIDesignGroupBoxFrame),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT),
    myCurrentLengthValid(true) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_ADDITIONALFRAME_REFERENCEPOINT, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");
    // Create Frame for Length Label and textField
    FXHorizontalFrame* lengthFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLengthLabel = new FXLabel(lengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelAttribute);
    myLengthTextField = new FXTextField(lengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_CHANGEPARAMETER_TEXT, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_ADDITIONALFRAME_BLOCKMOVEMENT, GUIDesignCheckButtonAttribute);
    myBlockMovementCheckButton->setCheck(false);
    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEAdditionalFrame::NeteditAttributes::~NeteditAttributes() {}


void
GNEAdditionalFrame::NeteditAttributes::showLengthFieldAndReferecePoint() {
    myLengthLabel->show();
    myLengthTextField->show();
    myReferencePointMatchBox->show();
}


void
GNEAdditionalFrame::NeteditAttributes::hideLengthFieldAndReferecePoint() {
    myLengthLabel->hide();
    myLengthTextField->hide();
    myReferencePointMatchBox->hide();
}


GNEAdditionalFrame::NeteditAttributes::additionalReferencePoint
GNEAdditionalFrame::NeteditAttributes::getActualReferencePoint() const {
    return myActualAdditionalReferencePoint;
}


double
GNEAdditionalFrame::NeteditAttributes::getLength() const {
    return GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
}


bool
GNEAdditionalFrame::NeteditAttributes::isBlockEnabled() const {
    return myBlockMovementCheckButton->getCheck() == 1 ? true : false;
}


bool
GNEAdditionalFrame::NeteditAttributes::isCurrentLengthValid() const {
    return myCurrentLengthValid;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdSetLength(FXObject*, FXSelector, void*) {
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
    return 1;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdSelectReferencePoint(FXObject*, FXSelector, void*) {
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
    return 1;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdSetBlocking(FXObject*, FXSelector, void*) {
    if (myBlockMovementCheckButton->getCheck()) {
        myBlockMovementCheckButton->setText("true");
    } else {
        myBlockMovementCheckButton->setText("false");
    }
    return 1;
}


long
GNEAdditionalFrame::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "Parameter editor Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << "Referece point: Mark the initial position of the additional element.\n"
            << "Example: If you want to create a busStop with a length of 30 in the point 100 of the lane:\n"
            << "- Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "- Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "- Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "the mouse. This option can be modified with the Inspector.";
    new FXLabel(helpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
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
// GNEAdditionalFrame::SelectorParentAdditional - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentAdditional::SelectorParentAdditional(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Additional Set selector", GUIDesignGroupBoxFrame),
    myUniqueSelection(false),
    myViewNet(viewNet) {

    // Create label with the type of SelectorParentAdditional
    mySetLabel = new FXLabel(this, "No additional selected", 0, GUIDesignLabelLeftThick);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECTADDITIONALPARENT, GUIDesignList, 0, 0, 0, 100);

    // Create help button
    myHelpAdditionalParentSelector = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);

    // Hide List
    hideListOfAdditionals();
}


GNEAdditionalFrame::SelectorParentAdditional::~SelectorParentAdditional() {}


std::string
GNEAdditionalFrame::SelectorParentAdditional::getIdSelected() const {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->isItemSelected(i)) {
            return myList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEAdditionalFrame::SelectorParentAdditional::showListOfAdditionals(SumoXMLTag type, bool uniqueSelection) {
    myUniqueSelection = uniqueSelection;
    mySetLabel->setText(("" + toString(type)).c_str());
    myList->clearItems();
    // obtain all additionals of class "type"
    std::vector<GNEAdditional*> vectorOfAdditionalParents = myViewNet->getNet()->getAdditionals(type);
    // fill list with IDs of additionals
    for (auto i : vectorOfAdditionalParents) {
        myList->appendItem(i->getID().c_str());
    }
    show();
}


void
GNEAdditionalFrame::SelectorParentAdditional::hideListOfAdditionals() {
    hide();
}


long
GNEAdditionalFrame::SelectorParentAdditional::onCmdSelectAdditionalParent(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorParentAdditional::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorParentEdges - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentEdges::SelectorParentEdges(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Edges", GUIDesignGroupBoxFrame),
    myViewNet(viewNet) {
    // Create menuCheck for selected edges
    myUseSelectedEdgesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_SHOWONLYSELECTEDEDGES, GUIDesignCheckButtonAttribute);

    // Create search box
    myEdgesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCHEDGE, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECTEDGE, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "Clear", 0, this, MID_GNE_ADDITIONALFRAME_CLEAREDGESELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "Invert", 0, this, MID_GNE_ADDITIONALFRAME_INVERTEDGESELECTION, GUIDesignButtonRectangular);

    // Create help button
    myHelpedgeParentsSelector = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);

    // Hide List
    hideList();
}


GNEAdditionalFrame::SelectorParentEdges::~SelectorParentEdges() {}


std::string
GNEAdditionalFrame::SelectorParentEdges::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}


void
GNEAdditionalFrame::SelectorParentEdges::showList(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // get all edges of net
    /// @todo this function must be improved.
    std::vector<GNEEdge*> vectorOfEdges = myViewNet->getNet()->retrieveEdges(false);
    // iterate over edges of net
    for (auto i : vectorOfEdges) {
        // If search criterium is correct, then append ittem
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedEdges isn't checked
    myUseSelectedEdgesCheckButton->setCheck(false);
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    // Show dialog
    show();
}


void
GNEAdditionalFrame::SelectorParentEdges::hideList() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorParentEdges::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myViewNet->getNet()->retrieveEdges(true).size() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


bool
GNEAdditionalFrame::SelectorParentEdges::isUseSelectedEdgesEnable() const {
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    if (myUseSelectedEdgesCheckButton->getCheck()) {
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
GNEAdditionalFrame::SelectorParentEdges::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorParentEdges that contains the searched string
    showList(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorParentEdges::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorParentEdges::onCmdHelp(FXObject*, FXSelector, void*) {
    std::cout << "IMPLEMENT" << std::endl;
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorParentLanes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentLanes::SelectorParentLanes(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Lanes", GUIDesignGroupBoxFrame),
    myViewNet(viewNet) {
    // Create CheckBox for selected lanes
    myUseSelectedLanesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTEDLANES, GUIDesignCheckButtonAttribute);

    // Create search box
    myLanesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCHLANE, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECTLANE, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_ADDITIONALFRAME_CLEARLANESELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_ADDITIONALFRAME_INVERTLANESELECTION, GUIDesignButtonRectangular);

    // Create help button
    helpLanes = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);

    // Hide List
    hideList();
}


GNEAdditionalFrame::SelectorParentLanes::~SelectorParentLanes() {}


std::string
GNEAdditionalFrame::SelectorParentLanes::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}


void
GNEAdditionalFrame::SelectorParentLanes::showList(std::string search) {
    myList->clearItems();
    std::vector<GNELane*> vectorOfLanes = myViewNet->getNet()->retrieveLanes(false);
    for (auto i : vectorOfLanes) {
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedLanes isn't checked
    myUseSelectedLanesCheckButton->setCheck(false);
    // Show list
    show();
}


void
GNEAdditionalFrame::SelectorParentLanes::hideList() {
    hide();
}


void
GNEAdditionalFrame::SelectorParentLanes::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myViewNet->getNet()->retrieveLanes(true).size() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


bool
GNEAdditionalFrame::SelectorParentLanes::isUseSelectedLanesEnable() const {
    if (myUseSelectedLanesCheckButton->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
    if (myUseSelectedLanesCheckButton->getCheck()) {
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
GNEAdditionalFrame::SelectorParentLanes::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorParentLanes that contains the searched string
    showList(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorParentLanes::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
