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
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNETAZ.h>
#include <netedit/additionals/GNETAZSink.h>
#include <netedit/additionals/GNETAZSource.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEAttributeCarrier.h>

#include "GNETAZFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETAZFrame::TAZParameters) TAZParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE_DIALOG,    GNETAZFrame::TAZParameters::onCmdSetColorAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,           GNETAZFrame::TAZParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                        GNETAZFrame::TAZParameters::onCmdHelp),
};

FXDEFMAP(GNETAZFrame::SaveTAZEdges) SaveTAZEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_OK,         GNETAZFrame::SaveTAZEdges::onCmdSaveChanges),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,     GNETAZFrame::SaveTAZEdges::onCmdCancelChanges),
};

FXDEFMAP(GNETAZFrame::EdgesTAZSelector) EdgesTAZSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,      GNETAZFrame::EdgesTAZSelector::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,   GNETAZFrame::EdgesTAZSelector::onCmdRemoveEdgeTAZ),
};

// Object implementation
FXIMPLEMENT(GNETAZFrame::TAZParameters,     FXGroupBox,     TAZParametersMap,       ARRAYNUMBER(TAZParametersMap))
FXIMPLEMENT(GNETAZFrame::SaveTAZEdges,      FXGroupBox,     SaveTAZEdgesMap,        ARRAYNUMBER(SaveTAZEdgesMap))
FXIMPLEMENT(GNETAZFrame::EdgesTAZSelector,  FXGroupBox,     EdgesTAZSelectorMap,    ARRAYNUMBER(EdgesTAZSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETAZFrame::CurrentTAZ - methods
// ---------------------------------------------------------------------------

GNETAZFrame::CurrentTAZ::CurrentTAZ(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myCurrentTAZ(nullptr) {
    // create TAZ label
    myCurrentTAZLabel = new FXLabel(this, "No TAZ selected", 0, GUIDesignLabelLeft);
}


GNETAZFrame::CurrentTAZ::~CurrentTAZ() {}


void 
GNETAZFrame::CurrentTAZ::setTAZ(GNETAZ* currentTAZ) {
    // set new current TAZ
    myCurrentTAZ = currentTAZ;
    // update label and moduls
    if(myCurrentTAZ != nullptr) {
        myCurrentTAZLabel->setText((std::string("Current TAZ: ") + myCurrentTAZ->getID()).c_str());
        // hide TAZ parameters
        myTAZFrameParent->myTAZParameters->hideTAZParametersModul();
        // hide Netedit parameters
        myTAZFrameParent->myNeteditAttributes->hideNeteditAttributesModul();
        // hide drawing shape
        myTAZFrameParent->myDrawingShape->hideDrawingShape();
        // show save TAZ Edges
        myTAZFrameParent->mySaveTAZEdges->showSaveTAZEdgesModul();
        // show edge selector
        myTAZFrameParent->myEdgesTAZSelector->showEdgeTAZSelectorModul();
    } else {
        myCurrentTAZLabel->setText("No TAZ selected");
        // show TAZ parameters
        myTAZFrameParent->myTAZParameters->showTAZParametersModul();
        // show Netedit parameters
        myTAZFrameParent->myNeteditAttributes->showNeteditAttributesModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_TAZ));
        // show drawing shape
        myTAZFrameParent->myDrawingShape->showDrawingShape();
        // hide save TAZ Edges
        myTAZFrameParent->mySaveTAZEdges->hideSaveTAZEdgesModul();
        // hide edge selector
        myTAZFrameParent->myEdgesTAZSelector->hideEdgeTAZSelectorModul();
    }
}


GNETAZ* 
GNETAZFrame::CurrentTAZ::getTAZ() const {
    return myCurrentTAZ;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::EdgesSelector - methods
// ---------------------------------------------------------------------------

GNETAZFrame::EdgesTAZSelector::EdgesTAZSelector(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, ("Selection of " + toString(SUMO_TAG_EDGE) + "s").c_str(), GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    myCurrentEdgesLabel = new FXLabel(this, "TAZ without edges", 0, GUIDesignLabelLeft);
    // update current edges label
    if(myTAZFrameParent->getCurrentTAZ()->getTAZ() && (myTAZFrameParent->getCurrentTAZ()->getTAZ()->getEdgeChilds().size() > 0)) {
        myCurrentEdgesLabel->hide();
    } else {
        myCurrentEdgesLabel->show();
    }
    // update list
    updateList();
}


GNETAZFrame::EdgesTAZSelector::~EdgesTAZSelector() {
}


void 
GNETAZFrame::EdgesTAZSelector::showEdgeTAZSelectorModul() {
    // update list
    updateList();
    show();
}


void 
GNETAZFrame::EdgesTAZSelector::hideEdgeTAZSelectorModul() {
    // remove all EdgeTAZRows
    for (auto i : myEdgeTAZRows) {
        delete i;
    }
    myEdgeTAZRows.clear();
    // hidde selector modul
    hide();
}


bool 
GNETAZFrame::EdgesTAZSelector::selectEdge(GNEEdge *edge) {
    if(edge) {
        // first check if edge is already selected
        for (auto i : myEdgeTAZRows) {
            if (i->getEdge() == edge) {
                // simply call remove edge TAZ giving as argument the button of EdgeTAZRow
                onCmdRemoveEdgeTAZ(i->getRemoveButton(), 0, 0);
                return true;
            }
        }
        // enable save and cancel buttons
        myTAZFrameParent->mySaveTAZEdges->enableButtons();
        // if edge was found, that means that GNETAZSource and GNETAZSink doesn't exist, then create it
        GNETAZSource* TAZSource = new GNETAZSource(myTAZFrameParent->getCurrentTAZ()->getTAZ(), edge, 0);
        GNETAZSink* TAZSink = new GNETAZSink(myTAZFrameParent->getCurrentTAZ()->getTAZ(), edge, 0);
        // create a GNEChange_Additional command, but without adding it into UndoList
        myTAZFrameParent->myViewNet->getUndoList()->add(new GNEChange_Additional(TAZSource, true), true);
        myTAZFrameParent->myViewNet->getUndoList()->add(new GNEChange_Additional(TAZSink, true), true);
        // update EdgeTAZ Rows (don't use updateList due flickering)
        myEdgeTAZRows.push_back(new EdgeTAZRow(this, edge, TAZSource, TAZSink));
        // hide myCurrentEdgesLabel (because at least there is a EdgeTAZRow)
        myCurrentEdgesLabel->hide();
        // recalc frame
        recalc();
        // update view
        myTAZFrameParent->getViewNet()->update();
        return true;
    } else {
        return false;
    }
}


void 
GNETAZFrame::EdgesTAZSelector::updateList() {
    // remove all EdgeTAZRows
    for (auto i : myEdgeTAZRows) {
        delete i;
    }
    myEdgeTAZRows.clear();
    // fill myEdgeTAZRows again
    if (myTAZFrameParent->myCurrentTAZ->getTAZ()) {
        // declare a container to group TAZ Childs by Edge
        std::map<std::string, std::pair<GNEAdditional*, GNEAdditional*> > TAZChilds;
        for (auto i : myTAZFrameParent->myCurrentTAZ->getTAZ()->getAdditionalChilds()) {
            // first check if a new pair has to be added to TAZChilds
            if(TAZChilds.count(i->getAttribute(SUMO_ATTR_EDGE)) == 0) {
                TAZChilds[i->getAttribute(SUMO_ATTR_EDGE)] = std::make_pair(nullptr, nullptr);
            }
            // set TAZChilds depending of TAZ child type
            if(i->getTag() == SUMO_TAG_TAZSOURCE) {
                TAZChilds.at(i->getAttribute(SUMO_ATTR_EDGE)).first = i;
            } else {
                TAZChilds.at(i->getAttribute(SUMO_ATTR_EDGE)).second = i;
            }
        }
        // iterate over TAZChilds and add rows
        for (auto i : TAZChilds) {
            myEdgeTAZRows.push_back(new EdgeTAZRow(this, myTAZFrameParent->myViewNet->getNet()->retrieveEdge(i.first), i.second.first, i.second.second));
        }
        // check if myCurrentEdgesLabel has to be shown
        if(myEdgeTAZRows.size() > 0) {
            myCurrentEdgesLabel->hide();
        } else {
            myCurrentEdgesLabel->show();
        }
    }
    // recalc frame
    recalc();
    // update view
    myTAZFrameParent->getViewNet()->update();
}


long 
GNETAZFrame::EdgesTAZSelector::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // find edited attribute
    for (auto i : myEdgeTAZRows) {
        if (obj == i->getDepartWeightTextField()) {
            // check if attribute is valid
            if (GNEAttributeCarrier::canParse<double>(i->getDepartWeightTextField()->getText().text()) && (GNEAttributeCarrier::parse<double>(i->getDepartWeightTextField()->getText().text()) >= 0)) {
                // enable save buttons (To begin undo_list)
                myTAZFrameParent->mySaveTAZEdges->enableButtons();
                // change value
                i->getEditedTAZSource()->setAttribute(SUMO_ATTR_WEIGHT, toString(i->getDepartWeightTextField()->getText().text()), myTAZFrameParent->getViewNet()->getUndoList());
                i->getDepartWeightTextField()->setTextColor(FXRGB(0, 0, 0));
                i->getDepartWeightTextField()->killFocus();
            } else {
                i->getDepartWeightTextField()->setTextColor(FXRGB(255, 0, 0));
            }
        } else if (obj == i->getArrivalWeightTextField()) {
            // check if attribute is valid
            if (GNEAttributeCarrier::canParse<double>(i->getArrivalWeightTextField()->getText().text()) && (GNEAttributeCarrier::parse<double>(i->getArrivalWeightTextField()->getText().text()) >= 0)) {
                // enable save buttons (To begin undo_list)
                myTAZFrameParent->mySaveTAZEdges->enableButtons();
                // change value
                i->getEditedTAZSink()->setAttribute(SUMO_ATTR_WEIGHT, toString(i->getArrivalWeightTextField()->getText().text()), myTAZFrameParent->getViewNet()->getUndoList());
                i->getArrivalWeightTextField()->setTextColor(FXRGB(0, 0, 0));
                i->getArrivalWeightTextField()->killFocus();
            } else {
                i->getArrivalWeightTextField()->setTextColor(FXRGB(255, 0, 0));
            }
        }
    }
    return 1;
}


long 
GNETAZFrame::EdgesTAZSelector::onCmdRemoveEdgeTAZ(FXObject* obj, FXSelector, void*) {
    // search remove button in all EdgeTAZs
    for (int i = 0; i < (int)myEdgeTAZRows.size(); i++) {
        if(myEdgeTAZRows.at(i)->getRemoveButton() == obj) {
            // enable save buttons and begin undo_list
            myTAZFrameParent->mySaveTAZEdges->enableButtons();
            // Remove both GNETAZSource and GNETAZSink using GNEChange_Additional
            myTAZFrameParent->getViewNet()->getUndoList()->add(new GNEChange_Additional(myEdgeTAZRows.at(i)->getEditedTAZSource(), false), true);
            myTAZFrameParent->getViewNet()->getUndoList()->add(new GNEChange_Additional(myEdgeTAZRows.at(i)->getEditedTAZSink(), false), true);
            // delete EdgeTAZRows and remove it from myEdgeTAZRows
            delete myEdgeTAZRows.at(i);
            myEdgeTAZRows.erase(myEdgeTAZRows.begin() + i);
            // check if label has to be shown
            if(myEdgeTAZRows.size() > 0) {
                myCurrentEdgesLabel->hide();
            } else {
                myCurrentEdgesLabel->show();
            }
            // recalc frame
            recalc();
            // update view
            myTAZFrameParent->getViewNet()->update();
            return 1;
        }
    }
    return 0;
}


GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::EdgeTAZRow(EdgesTAZSelector *edgesTAZSelector, GNEEdge *edge, GNEAdditional *TAZSource, GNEAdditional *TAZSink) :
    FXVerticalFrame(edgesTAZSelector, GUIDesignAuxiliarHorizontalFrame),
    myEdgesTAZSelectorParent(edgesTAZSelector),
    myEdge(edge),
    myEditedTAZSource(TAZSource),
    myEditedTAZSink(TAZSink) {
    // create Edge Label and button
    FXHorizontalFrame* horizontalFrameButton = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myRemoveButton = new FXButton(horizontalFrameButton, "", GUIIconSubSys::getIcon(ICON_REMOVE), edgesTAZSelector, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    myEdgeLabel = new FXLabel(horizontalFrameButton, toString("edge: " + edge->getID()).c_str(), 0, GUIDesignLabelLeftThick);
    // create Label and textfield for Arrival Weight
    FXHorizontalFrame* departWeightFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(departWeightFrame, "Source weight", 0, GUIDesignLabelAttribute);
    myDepartWeightTextField = new FXTextField(departWeightFrame, GUIDesignTextFieldNCol, edgesTAZSelector, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myDepartWeightTextField->setText(TAZSource->getAttribute(SUMO_ATTR_WEIGHT).c_str());
    // create Label and textfield for Arrival Weight
    FXHorizontalFrame* arrivalWeightFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(arrivalWeightFrame, "Sink weight", 0, GUIDesignLabelAttribute);
    myArrivalWeightTextField = new FXTextField(arrivalWeightFrame, GUIDesignTextFieldNCol, edgesTAZSelector, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myArrivalWeightTextField->setText(TAZSink->getAttribute(SUMO_ATTR_WEIGHT).c_str());
    // set edge color
    for (auto j : myEdge->getLanes()) {
        j->setSpecialColor(&myEdgesTAZSelectorParent->myTAZFrameParent->getEdgeCandidateSelectedColor());
    }
    // create EdgeTAZRow (and their childs)
    create();
}


GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::~EdgeTAZRow() {
    // restore edge color
    for (auto j : myEdge->getLanes()) {
        j->setSpecialColor(nullptr);
    }
}


GNEAdditional*
GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::getEditedTAZSource() const {
    return myEditedTAZSource;
}


GNEAdditional*
GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::getEditedTAZSink() const {
    return myEditedTAZSink;
}


FXButton*
GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::getRemoveButton() const {
    return myRemoveButton;
}


FXTextField*
GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::getDepartWeightTextField() const {
    return myDepartWeightTextField;
}


FXTextField*
GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::getArrivalWeightTextField() const {
    return myArrivalWeightTextField;
}


GNEEdge*
GNETAZFrame::EdgesTAZSelector::EdgeTAZRow::getEdge() const {
    return myEdge;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::SaveTAZEdges - methods
// ---------------------------------------------------------------------------

GNETAZFrame::SaveTAZEdges::SaveTAZEdges(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "Save TAZ Edges", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // Create groupbox for save changes
    mySaveChangesButton = new FXButton(this, "Save changes", GUIIconSubSys::getIcon(ICON_SAVE), this, MID_OK, GUIDesignButton);
    mySaveChangesButton->disable();
    // Create groupbox cancel changes
    myCancelChangesButton = new FXButton(this, "Sancel changes", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_CANCEL, GUIDesignButton);
    myCancelChangesButton->disable();
    // Create groupbox and label for legend
    FXLabel *colorSelectedLabel = new FXLabel(this, "Edge Selected", 0, GUIDesignLabelLeft);
    colorSelectedLabel->setBackColor(MFXUtils::getFXColor(TAZFrameParent->getEdgeCandidateSelectedColor()));
}


GNETAZFrame::SaveTAZEdges::~SaveTAZEdges() {}


void 
GNETAZFrame::SaveTAZEdges::showSaveTAZEdgesModul() {
    show();
}


void 
GNETAZFrame::SaveTAZEdges::hideSaveTAZEdgesModul() {
    // cancel changes
    onCmdCancelChanges(0,0,0);
    hide();
}


void
GNETAZFrame::SaveTAZEdges::enableButtons() {
    // enable mySaveChangesButton and myCancelChangesButton, and start a undolist set
    if (!mySaveChangesButton->isEnabled()) {
        mySaveChangesButton->enable();
        myCancelChangesButton->enable();
        myTAZFrameParent->myViewNet->getUndoList()->p_begin("TAZ attributes");
    }
}

long
GNETAZFrame::SaveTAZEdges::onCmdSaveChanges(FXObject*, FXSelector, void*) {
    // disable mySaveChangesButton and myCancelChangesButtonand, and finish undolist set
    if (mySaveChangesButton->isEnabled()) {
        mySaveChangesButton->disable();
        myCancelChangesButton->disable();
        myTAZFrameParent->myViewNet->getUndoList()->p_end();
    }
    return 1;
}


long
GNETAZFrame::SaveTAZEdges::onCmdCancelChanges(FXObject*, FXSelector, void*) {
    // cancel changes and disable buttons
    if (mySaveChangesButton->isEnabled()) {
        myTAZFrameParent->myViewNet->getUndoList()->p_abort();
        mySaveChangesButton->disable();
        myCancelChangesButton->disable();
        // update list of myEdgesTAZSelector
        myTAZFrameParent->myEdgesTAZSelector->updateList();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZParameters- methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZParameters::TAZParameters(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ parameters", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // create Button and string textField for color and set blue as default color
    FXHorizontalFrame* colorParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myColorEditor = new FXButton(colorParameter, toString(SUMO_ATTR_COLOR).c_str(), 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myTextFieldColor = new FXTextField(colorParameter, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextFieldColor->setText("blue");
    // create Label and CheckButton for use innen edges with true as default value
    FXHorizontalFrame* useInnenEdges = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(useInnenEdges, "Innen Edges", 0, GUIDesignLabelAttribute);
    myUseInnenEdgesCheckButton = new FXCheckButton(useInnenEdges, "true", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myUseInnenEdgesCheckButton->setCheck(true);
    // Create help button
    myHelpTAZAttribute = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNETAZFrame::TAZParameters::~TAZParameters() {}


void
GNETAZFrame::TAZParameters::showTAZParametersModul() {
    FXGroupBox::show();
}


void
GNETAZFrame::TAZParameters::hideTAZParametersModul() {
    FXGroupBox::hide();
}


bool
GNETAZFrame::TAZParameters::isCurrentParametersValid() const {
    return GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text());
}


bool 
GNETAZFrame::TAZParameters::isUseInnenEdgesEnabled() const {
    return (myUseInnenEdgesCheckButton->getCheck() == TRUE);
}


std::map<SumoXMLAttr, std::string> 
GNETAZFrame::TAZParameters::getAttributesAndValues() const {
    std::map<SumoXMLAttr, std::string> parametersAndValues;
    // get color (currently the only editable attribute)
    parametersAndValues[SUMO_ATTR_COLOR] = myTextFieldColor->getText().text();
    return parametersAndValues;
}


long 
GNETAZFrame::TAZParameters::onCmdSetColorAttribute(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldColor->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor("blue")));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        myTextFieldColor->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
        onCmdSetAttribute(0, 0, 0);
    }
    return 0;
}


long
GNETAZFrame::TAZParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // only COLOR text field has to be checked
    bool currentParametersValid = GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text());
    // change color of textfield dependig of myCurrentParametersValid
    if (currentParametersValid) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myTextFieldColor->killFocus();
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        currentParametersValid = false;
    }
    // change useInnenEdgesCheckButton text
    if(myUseInnenEdgesCheckButton->getCheck() == TRUE) {
        myUseInnenEdgesCheckButton->setText("true");
    } else {
        myUseInnenEdgesCheckButton->setText("false");
    }
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

    // create current TAZ modul
    myCurrentTAZ = new CurrentTAZ(this);

    // Create TAZ Parameters modul
    myTAZParameters = new TAZParameters(this);

    /// @brief create  Netedit parameter
    myNeteditAttributes = new NeteditAttributes(this);

    // Create drawing controls modul
    myDrawingShape = new DrawingShape(this);

    // Create save TAZ Edges modul
    mySaveTAZEdges = new SaveTAZEdges(this);

    // Create edge Selector modul
    myEdgesTAZSelector = new EdgesTAZSelector(this);

    // by default there isn't a TAZ
    myCurrentTAZ->setTAZ(nullptr);
}


GNETAZFrame::~GNETAZFrame() {
}


void
GNETAZFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


bool 
GNETAZFrame::processClick(const Position& clickedPosition, GNETAZ *TAZ, GNEEdge* edge) {
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement;
    if (myDrawingShape->isDrawing()) {
        // add or delete a new point depending of flag "delete last created point"
        if (myDrawingShape->getDeleteLastCreatedPoint()) {
            myDrawingShape->removeLastPoint();
        } else {
            myDrawingShape->addNewPoint(clickedPosition);
        }
        return true;
    } else if (myCurrentTAZ->getTAZ() == nullptr) {
        // avoid reset of Frame if user doesn't click over an TAZ
        if(TAZ) {
            myCurrentTAZ->setTAZ(TAZ);
            return true;
        } else {
            return false;
        }
    } else if (edge) {
        // select edge if is unselected, or unselect if it was already selected
        myEdgesTAZSelector->selectEdge(edge);
        return true;
    } else {
        // nothing to do
        return false;
    }
}


GNETAZFrame::DrawingShape*
GNETAZFrame::getDrawingShape() const {
    return myDrawingShape;
}


GNETAZFrame::CurrentTAZ* 
GNETAZFrame::getCurrentTAZ() const {
    return myCurrentTAZ;
}


bool
GNETAZFrame::buildShape() {
    // show warning dialogbox and stop check if input parameters are valid
    if (myTAZParameters->isCurrentParametersValid() == false) {
        return false;
    } else if(myDrawingShape->getTemporalShape().size() == 0) {
        WRITE_WARNING("TAZ shape cannot be empty");
        return false;
    } else {
        // Declare map to keep TAZ Parameters values
        std::map<SumoXMLAttr, std::string> valuesOfElement = myTAZParameters->getAttributesAndValues();

        // obtain Netedit attributes
        myNeteditAttributes->getNeteditAttributesAndValues(valuesOfElement, nullptr);

        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateAdditionalID(SUMO_TAG_TAZ);

        // obtain shape and close it
        PositionVector shape = myDrawingShape->getTemporalShape();
        shape.closePolygon();
        valuesOfElement[SUMO_ATTR_SHAPE] = toString(shape);

        // check if TAZ has to be created with edges
        if (myTAZParameters->isUseInnenEdgesEnabled()) {
            std::vector<std::string> edgeIDs;
            auto ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(shape.getBoxBoundary(), true);
            for (auto i : ACsInBoundary) {
                if(i.second->getTag() == SUMO_TAG_EDGE) {
                    edgeIDs.push_back(i.first);
                }
            }
            valuesOfElement[SUMO_ATTR_EDGES] = toString(edgeIDs);
        } else {
            // TAZ is created without edges
            valuesOfElement[SUMO_ATTR_EDGES] = "";
        }
        // return true if TAZ was sucesfully created
        return GNEAdditionalHandler::buildAdditional(myViewNet, true, SUMO_TAG_TAZ, valuesOfElement);
    }
}


void 
GNETAZFrame::enableModuls(const GNEAttributeCarrier::TagValues &/*tagValue*/) {

}


void 
GNETAZFrame::disableModuls() {

}

/****************************************************************************/
