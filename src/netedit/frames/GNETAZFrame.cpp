/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
/// @version $Id$
///
// The Widget for add TAZ elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNENet.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNETAZ.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEAttributeCarrier.h>

#include "GNETAZFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETAZFrame::TAZSelector) SelectorTAZMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNETAZFrame::TAZSelector::onCmdselectAttributeCarrier),
};

FXDEFMAP(GNETAZFrame) GNETAZMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAZFRAME_CREATETAZ,    GNETAZFrame::onCmdCreateTAZ),
};

FXDEFMAP(GNETAZFrame::edgesSelector) GNEEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNETAZFrame::edgesSelector::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNETAZFrame::edgesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNETAZFrame::edgesSelector::onCmdInvertSelection),
};

FXDEFMAP(GNETAZFrame::TAZParameters) GNETAZParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,   GNETAZFrame::TAZParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                GNETAZFrame::TAZParameters::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNETAZFrame::TAZSelector,       FXGroupBox,         SelectorTAZMap,         ARRAYNUMBER(SelectorTAZMap))
FXIMPLEMENT(GNETAZFrame,                    FXVerticalFrame,    GNETAZMap,              ARRAYNUMBER(GNETAZMap))
FXIMPLEMENT(GNETAZFrame::edgesSelector,     FXGroupBox,         GNEEdgesMap,            ARRAYNUMBER(GNEEdgesMap))
FXIMPLEMENT(GNETAZFrame::TAZParameters,     FXGroupBox,         GNETAZParametersMap,    ARRAYNUMBER(GNETAZParametersMap))


// ===========================================================================
// method definitions
// ===========================================================================


// ---------------------------------------------------------------------------
// GNETAZFrame::TAZSelector - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZSelector::TAZSelector(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ element", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myCurrentTAZType(SUMO_TAG_NOTHING) {

    // Create FXListBox in myGroupBoxForMyTAZMatchBox
    myTAZMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);

    // Add options to myTAZMatchBox
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TAGProperty::TAGPROPERTY_TAZ, false);
    for (auto i : listOfTags) {
        myTAZMatchBox->appendItem(toString(i).c_str());
    }
    // Set visible items
    myTAZMatchBox->setNumVisible((int)myTAZMatchBox->getNumItems());
    // TAZSelector is always shown
    show();
}


GNETAZFrame::TAZSelector::~TAZSelector() {}


SumoXMLTag
GNETAZFrame::TAZSelector::getCurrentTAZType() const {
    return myCurrentTAZType;
}


void
GNETAZFrame::TAZSelector::setCurrentTAZ(SumoXMLTag actualTAZType) {
    // Set new actualTAZType
    myCurrentTAZType = actualTAZType;
    /*
    // Check that current TAZ type is valid
    if (myCurrentTAZType != SUMO_TAG_NOTHING) {

        // obtain tag property (only for improve code legibility)
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentTAZType);
        // first check if TAZ can block movement or mask the start/end position, then show neteditParameters
        if (tagValue.canBlockMovement() || tagValue.canMaskStartEndPos()) {
            myTAZFrameParent->myNeteditAttributes->showNeteditAttributesModul(tagValue.canMaskStartEndPos());
        } else {
            myTAZFrameParent->myNeteditAttributes->hideNeteditAttributesModul();
        }
        // Clear internal attributes
        myTAZFrameParent->myTAZAttributes->clearAttributes();
        // iterate over attributes of myCurrentTAZType
        for (auto i : tagValue) {
            // only show attributes that aren't uniques
            if (!i.second.isUnique()) {
                myTAZFrameParent->myTAZAttributes->addAttribute(i.first);
            }
        }
        // show TAZ attribute modul
        myTAZFrameParent->myTAZAttributes->showTAZAttributesModul();
        // Show SelectorEdgeChilds if we're adding an TAZ that own the attribute SUMO_ATTR_EDGES
        if (tagValue.hasAttribute(SUMO_ATTR_EDGES)) {
            myTAZFrameParent->mySelectorEdgeChilds->showSelectorEdgeChildsModul();
        } else {
            myTAZFrameParent->mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
        }
    } else {
        // hide all moduls if TAZ isn't valid
        myTAZFrameParent->myTAZAttributes->hideTAZAttributesModul();
        myTAZFrameParent->myNeteditAttributes->hideNeteditAttributesModul();
        myTAZFrameParent->mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
    }
    */
}


long
GNETAZFrame::TAZSelector::onCmdselectAttributeCarrier(FXObject*, FXSelector, void*) {
    // Check if value of myTAZMatchBox correspond of an allowed TAZ tags
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TAGProperty::TAGPROPERTY_TAZ, false);
    for (auto i : listOfTags) {
        if (toString(i) == myTAZMatchBox->getText().text()) {
            myTAZMatchBox->setTextColor(FXRGB(0, 0, 0));
            setCurrentTAZ(i);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected TAZ '" + myTAZMatchBox->getText() + "' in TAZSelector").text());
            return 1;
        }
    }
    // if TAZ name isn't correct, hidde all
    setCurrentTAZ(SUMO_TAG_NOTHING);
    myTAZMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid TAZ in TAZSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::edgesSelector - methods
// ---------------------------------------------------------------------------

GNETAZFrame::edgesSelector::edgesSelector(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, ("selection of " + toString(SUMO_TAG_EDGE) + "s").c_str(), GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myCurrentJunction(0) {

    // Create button for selected edges
    myUseSelectedEdges = new FXButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignButton);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(this, ("Clear " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButton);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(this, ("Invert " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButton);
}


GNETAZFrame::edgesSelector::~edgesSelector() {}


GNEJunction*
GNETAZFrame::edgesSelector::getCurrentJunction() const {
    return myCurrentJunction;
}


void
GNETAZFrame::edgesSelector::enableEdgeSelector(GNEJunction* currentJunction) {
    // restore color of all lanes of edge candidates
    restoreEdgeColors();
    // Set current junction
    myCurrentJunction = currentJunction;
    // Update view net to show the new colors
    myTAZFrameParent->getViewNet()->update();
    // check if use selected eges must be enabled
    myUseSelectedEdges->disable();
    for (auto i : myCurrentJunction->getGNEEdges()) {
        if (i->isAttributeCarrierSelected()) {
            myUseSelectedEdges->enable();
        }
    }
    // Enable rest of elements
    myClearEdgesSelection->enable();
    myInvertEdgesSelection->enable();
}


void
GNETAZFrame::edgesSelector::disableEdgeSelector() {
    // disable current junction
    myCurrentJunction = NULL;
    // disable all elements of the edgesSelector
    myUseSelectedEdges->disable();
    myClearEdgesSelection->disable();
    myInvertEdgesSelection->disable();
    // Disable TAZ parameters
    myTAZFrameParent->getTAZParameters()->disableTAZParameters();
}


void
GNETAZFrame::edgesSelector::restoreEdgeColors() {
    if (myCurrentJunction != NULL) {
        // restore color of all lanes of edge candidates
        for (auto i : myCurrentJunction->getGNEEdges()) {
            for (auto j : i->getLanes()) {
                j->setSpecialColor(0);
            }
        }
        // Update view net to show the new colors
        myTAZFrameParent->getViewNet()->update();
        myCurrentJunction = NULL;
    }
}


long
GNETAZFrame::edgesSelector::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    myTAZFrameParent->getTAZParameters()->useSelectedEdges(myCurrentJunction);
    return 1;
}


long
GNETAZFrame::edgesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    myTAZFrameParent->getTAZParameters()->clearEdges();
    return 1;
}


long
GNETAZFrame::edgesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    myTAZFrameParent->getTAZParameters()->invertEdges(myCurrentJunction);
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZParameters::TAZParameters(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ parameters", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myCurrentParametersValid(true) {
    FXHorizontalFrame* TAZParameter = NULL;
    // create label and string textField for edges
    TAZParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myTAZEdgesLabel = new FXLabel(TAZParameter, toString(SUMO_ATTR_EDGES).c_str(), 0, GUIDesignLabelAttribute);
    myTAZEdges = new FXTextField(TAZParameter, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTAZEdgesLabel->disable();
    myTAZEdges->disable();
    // create label and checkbox for Priority
    TAZParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myTAZPriorityLabel = new FXLabel(TAZParameter, toString(SUMO_ATTR_PRIORITY).c_str(), 0, GUIDesignLabelAttribute);
    myTAZPriorityCheckButton = new FXCheckButton(TAZParameter, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myTAZPriorityLabel->disable();
    myTAZPriorityCheckButton->disable();
    // create label and textfield for width
    TAZParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myTAZWidthLabel = new FXLabel(TAZParameter, toString(SUMO_ATTR_WIDTH).c_str(), 0, GUIDesignLabelAttribute);
    myTAZWidth = new FXTextField(TAZParameter, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTAZWidthLabel->disable();
    myTAZWidth->disable();
    // Create help button
    myHelpTAZAttribute = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    myHelpTAZAttribute->disable();
    // set colors
    myCandidateColor = RGBColor(0, 64, 0, 255);
    mySelectedColor = RGBColor::GREEN;
}


GNETAZFrame::TAZParameters::~TAZParameters() {}


void
GNETAZFrame::TAZParameters::enableTAZParameters(bool hasTLS) {
    // obtain Tag Values
    const auto& tagProperties = GNEAttributeCarrier::getTagProperties(SUMO_TAG_TAZ);
    // Enable all elements of the TAZ frames
    myTAZEdgesLabel->enable();
    myTAZEdges->enable();
    myTAZPriorityLabel->enable();
    if (hasTLS) {
        myTAZPriorityCheckButton->disable();
    } else {
        myTAZPriorityCheckButton->enable();
    }
    myTAZWidthLabel->enable();
    myTAZWidth->enable();
    myHelpTAZAttribute->enable();
    // set values of parameters
    onCmdSetAttribute(0, 0, 0);
    myTAZPriorityCheckButton->setCheck(hasTLS ? true :
                                            GNEAttributeCarrier::parse<bool>(tagProperties.getDefaultValue(SUMO_ATTR_PRIORITY)));
    myTAZWidth->setText(tagProperties.getDefaultValue(SUMO_ATTR_WIDTH).c_str());
    myTAZWidth->setTextColor(FXRGB(0, 0, 0));
}


void
GNETAZFrame::TAZParameters::disableTAZParameters() {
    // clear all values of parameters
    myTAZEdges->setText("");
    myTAZPriorityCheckButton->setCheck(false);
    myTAZPriorityCheckButton->setText("false");
    myTAZWidth->setText("");
    // Disable all elements of the TAZ frames
    myTAZEdgesLabel->disable();
    myTAZEdges->disable();
    myTAZPriorityLabel->disable();
    myTAZPriorityCheckButton->disable();
    myTAZWidthLabel->disable();
    myTAZWidth->disable();
    myHelpTAZAttribute->disable();
    myTAZFrameParent->setCreateTAZButton(false);
}


bool
GNETAZFrame::TAZParameters::isTAZParametersEnabled() const {
    return myTAZEdgesLabel->isEnabled();
}


void
GNETAZFrame::TAZParameters::markEdge(GNEEdge* edge) {
    GNEJunction* currentJunction = myTAZFrameParent->getEdgeSelector()->getCurrentJunction();
    if (currentJunction != NULL) {
        // Check if edge belongs to junction's edge
        if (std::find(currentJunction->getGNEEdges().begin(), currentJunction->getGNEEdges().end(), edge) != currentJunction->getGNEEdges().end()) {
            // Update text field with the new edge
            std::vector<std::string> TAZEdges = GNEAttributeCarrier::parse<std::vector<std::string> > (myTAZEdges->getText().text());
            // Check if new edge must be added or removed
            std::vector<std::string>::iterator itFinder = std::find(TAZEdges.begin(), TAZEdges.end(), edge->getID());
            if (itFinder == TAZEdges.end()) {
                TAZEdges.push_back(edge->getID());
            } else {
                TAZEdges.erase(itFinder);
            }
            myTAZEdges->setText(joinToString(TAZEdges, " ").c_str());
        }
        // Update colors and attributes
        onCmdSetAttribute(0, 0, 0);
    }
}


void
GNETAZFrame::TAZParameters::clearEdges() {
    myTAZEdges->setText("");
    // Update colors and attributes
    onCmdSetAttribute(0, 0, 0);
}


void
GNETAZFrame::TAZParameters::invertEdges(GNEJunction* parentJunction) {
    std::vector<std::string> TAZEdges;
    for (auto i : parentJunction->getGNEEdges()) {
        if (std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), i) == myCurrentSelectedEdges.end()) {
            TAZEdges.push_back(i->getID());
        }
    }
    myTAZEdges->setText(joinToString(TAZEdges, " ").c_str());
    // Update colors and attributes
    onCmdSetAttribute(0, 0, 0);
}


void
GNETAZFrame::TAZParameters::useSelectedEdges(GNEJunction* parentJunction) {
    std::vector<std::string> TAZEdges;
    for (auto i : parentJunction->getGNEEdges()) {
        if (i->isAttributeCarrierSelected()) {
            TAZEdges.push_back(i->getID());
        }
    }
    myTAZEdges->setText(joinToString(TAZEdges, " ").c_str());
    // Update colors and attributes
    onCmdSetAttribute(0, 0, 0);
}


std::vector<NBEdge*>
GNETAZFrame::TAZParameters::getTAZEdges() const {
    std::vector<NBEdge*> NBEdgeVector;
    // Iterate over myCurrentSelectedEdges
    for (auto i : myCurrentSelectedEdges) {
        NBEdgeVector.push_back(i->getNBEdge());
    }
    return NBEdgeVector;
}


bool
GNETAZFrame::TAZParameters::getTAZPriority() const {
    if (myTAZPriorityCheckButton->getCheck()) {
        return true;
    } else {
        return false;
    }
}


bool
GNETAZFrame::TAZParameters::isCurrentParametersValid() const {
    return myCurrentParametersValid;
}


double
GNETAZFrame::TAZParameters::getTAZWidth() const {
    return GNEAttributeCarrier::parse<double>(myTAZWidth->getText().text());
}


const RGBColor&
GNETAZFrame::TAZParameters::getCandidateColor() const {
    return myCandidateColor;
}


const RGBColor&
GNETAZFrame::TAZParameters::getSelectedColor() const {
    return mySelectedColor;
}


long
GNETAZFrame::TAZParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    myCurrentParametersValid = true;
    // get string vector with the edges
    std::vector<std::string> TAZEdges = GNEAttributeCarrier::parse<std::vector<std::string> > (myTAZEdges->getText().text());
    // Clear selected edges
    myCurrentSelectedEdges.clear();
    // iterate over vector of edge IDs
    for (auto i : TAZEdges) {
        GNEEdge* edge = myTAZFrameParent->getViewNet()->getNet()->retrieveEdge(i, false);
        GNEJunction* currentJunction = myTAZFrameParent->getEdgeSelector()->getCurrentJunction();
        // Check that edge exists and belongs to Junction
        if (edge == 0) {
            myCurrentParametersValid = false;
        } else if (std::find(currentJunction->getGNEEdges().begin(), currentJunction->getGNEEdges().end(), edge) == currentJunction->getGNEEdges().end()) {
            myCurrentParametersValid = false;
        } else {
            // select or unselected edge
            auto itFinder = std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), edge);
            if (itFinder == myCurrentSelectedEdges.end()) {
                myCurrentSelectedEdges.push_back(edge);
            } else {
                myCurrentSelectedEdges.erase(itFinder);
            }
        }
    }

    // change color of textfield dependig of myCurrentParametersValid
    if (myCurrentParametersValid) {
        myTAZEdges->setTextColor(FXRGB(0, 0, 0));
        myTAZEdges->killFocus();
    } else {
        myTAZEdges->setTextColor(FXRGB(255, 0, 0));
        myCurrentParametersValid = false;
    }

    // Update colors of edges
    for (auto i : myTAZFrameParent->getEdgeSelector()->getCurrentJunction()->getGNEEdges()) {
        if (std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), i) != myCurrentSelectedEdges.end()) {
            for (auto j : i->getLanes()) {
                j->setSpecialColor(&mySelectedColor);
            }
        } else {
            for (auto j : i->getLanes()) {
                j->setSpecialColor(&myCandidateColor);
            }
        }
    }
    // Update view net
    myTAZFrameParent->getViewNet()->update();

    // Check that at least there are a selected edge
    if (TAZEdges.empty()) {
        myCurrentParametersValid = false;
    }

    // change label of TAZ priority
    if (myTAZPriorityCheckButton->getCheck()) {
        myTAZPriorityCheckButton->setText("true");
    } else {
        myTAZPriorityCheckButton->setText("false");
    }

    // Check width
    if (GNEAttributeCarrier::canParse<double>(myTAZWidth->getText().text()) &&
            GNEAttributeCarrier::parse<double>(myTAZWidth->getText().text()) > 0) {
        myTAZWidth->setTextColor(FXRGB(0, 0, 0));
        myTAZWidth->killFocus();
    } else {
        myTAZWidth->setTextColor(FXRGB(255, 0, 0));
        myCurrentParametersValid = false;
    }

    // Enable or disable create TAZ button depending of the current parameters
    myTAZFrameParent->setCreateTAZButton(myCurrentParametersValid);
    return 0;
}


long
GNETAZFrame::TAZParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    myTAZFrameParent->openHelpAttributesDialog(SUMO_TAG_TAZ);
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame - methods
// ---------------------------------------------------------------------------

GNETAZFrame::GNETAZFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "TAZs") {
    // Create TAZ Selector
    myTAZSelector = new TAZSelector(this);

    // Create edge Selector
    myEdgeSelector = new edgesSelector(this);

    // Create TAZParameters
    myTAZParameters = new TAZParameters(this);

    // Create groupbox for create TAZs
    myGroupBoxButtons = new FXGroupBox(myContentFrame, "Create", GUIDesignGroupBoxFrame);
    myCreateTAZButton = new FXButton(myGroupBoxButtons, "Create TAZ", 0, this, MID_GNE_TAZFRAME_CREATETAZ, GUIDesignButton);
    myCreateTAZButton->disable();

    // Create groupbox and labels for legends
    myGroupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);
    myColorCandidateLabel = new FXLabel(myGroupBoxLegend, "Candidate", 0, GUIDesignLabelLeft);
    myColorCandidateLabel->setBackColor(MFXUtils::getFXColor(myTAZParameters->getCandidateColor()));
    myColorSelectedLabel = new FXLabel(myGroupBoxLegend, "Selected", 0, GUIDesignLabelLeft);
    myColorSelectedLabel->setBackColor(MFXUtils::getFXColor(myTAZParameters->getSelectedColor()));

    // disable edge selector
    myEdgeSelector->disableEdgeSelector();
}


GNETAZFrame::~GNETAZFrame() {
}


void
GNETAZFrame::hide() {
    // restore color of all lanes of edge candidates
    myEdgeSelector->restoreEdgeColors();
    // hide frame
    GNEFrame::hide();
}


bool
GNETAZFrame::addTAZ(const GNEViewNet::ObjectsUnderCursor &objectsUnderCursor) {
    // If current element is a junction
    if (objectsUnderCursor.junction) {
        // Enable edge selector and TAZ parameters
        myEdgeSelector->enableEdgeSelector(objectsUnderCursor.junction);
        myTAZParameters->enableTAZParameters(objectsUnderCursor.junction->getNBNode()->isTLControlled());
        // clears selected edges
        myTAZParameters->clearEdges();
    } else if (objectsUnderCursor.edge) {
        myTAZParameters->markEdge(objectsUnderCursor.edge);
    } else {
        // restore  color of all lanes of edge candidates
        myEdgeSelector->restoreEdgeColors();
        // Disable edge selector
        myEdgeSelector->disableEdgeSelector();
    }
    return false;
}


long
GNETAZFrame::onCmdCreateTAZ(FXObject*, FXSelector, void*) {
    // First check that current parameters are valid
    if (myTAZParameters->isCurrentParametersValid()) {
        /*
        // iterate over junction's TAZ to find duplicated TAZs
        if (myEdgeSelector->getCurrentJunction()->getNBNode()->checkTAZDuplicated(myTAZParameters->getTAZEdges()) == false) {
            // create new TAZ
            myViewNet->getUndoList()->add(new GNEChange_TAZ(myEdgeSelector->getCurrentJunction(),
                                          myTAZParameters->getTAZEdges(),
                                          myTAZParameters->getTAZWidth(),
                                          myTAZParameters->getTAZPriority(),
                                          -1, -1,
                                          PositionVector::EMPTY,
                                          false, true), true);

            // clear selected edges
            myEdgeSelector->onCmdClearSelection(0, 0, 0);
        } else {
            WRITE_WARNING("There is already another TAZ with the same edges in the junction; Duplicated TAZ aren't allowed.");
        }
        */
    }
    return 1;
}


void
GNETAZFrame::setCreateTAZButton(bool value) {
    if (value) {
        myCreateTAZButton->enable();
    } else {
        myCreateTAZButton->disable();
    }
}


GNETAZFrame::edgesSelector*
GNETAZFrame::getEdgeSelector() const {
    return myEdgeSelector;
}


GNETAZFrame::TAZParameters*
GNETAZFrame::getTAZParameters() const {
    return myTAZParameters;
}

/****************************************************************************/
