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
GNETAZFrame::CurrentTAZ::setCurrentTAZ(GNETAZ* currentTAZ) {
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
GNETAZFrame::CurrentTAZ::getCurrentTAZ() const {
    return myCurrentTAZ;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::EdgesSelector - methods
// ---------------------------------------------------------------------------

GNETAZFrame::EdgesTAZSelector::EdgesTAZSelector(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, ("selection of " + toString(SUMO_TAG_EDGE) + "s").c_str(), GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    myCurrentEdgesLabel = new FXLabel(this, "TAZ without edges", 0, GUIDesignLabelLeft);
    // update current edges label
    if(myTAZFrameParent->getCurrentTAZ()->getCurrentTAZ() && (myTAZFrameParent->getCurrentTAZ()->getCurrentTAZ()->getEdgeChilds().size() > 0)) {
        myCurrentEdgesLabel->hide();
    } else {
        myCurrentEdgesLabel->show();
    }
}


GNETAZFrame::EdgesTAZSelector::~EdgesTAZSelector() {
}


void 
GNETAZFrame::EdgesTAZSelector::showEdgeTAZSelectorModul() {
    show();
}


void 
GNETAZFrame::EdgesTAZSelector::hideEdgeTAZSelectorModul() {
    hide();
}


bool 
GNETAZFrame::EdgesTAZSelector::selectEdge(GNEEdge *edge) {
    if(edge) {
        // add TAZ;
        myEdgeTAZs.push_back(new EdgeTAZ(this));
        // update current edges label
        if(myTAZFrameParent->getCurrentTAZ()->getCurrentTAZ() && (myTAZFrameParent->getCurrentTAZ()->getCurrentTAZ()->getEdgeChilds().size() > 0)) {
            myCurrentEdgesLabel->setText("");
        } else {
            myCurrentEdgesLabel->setText("TAZ without edges");
        }
        // recalc frame
        recalc();
        return true;
    } else {
        return false;
    }
}


long 
GNETAZFrame::EdgesTAZSelector::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    return 0;
}


long 
GNETAZFrame::EdgesTAZSelector::onCmdRemoveEdgeTAZ(FXObject* obj, FXSelector, void*) {
    // search remove button in all EdgeTAZs
    for (int i = 0; i < (int)myEdgeTAZs.size(); i++) {
        if(myEdgeTAZs.at(i)->removeButton == obj) {
            // delete EdgeTAZ
            delete myEdgeTAZs.at(i);
            // remove EdgeTAZ from myEdgeTAZs
            myEdgeTAZs.erase(myEdgeTAZs.begin() + i);
            // recalc frame
            recalc();
            return 1;
        }
    }
    return 0;
}


GNETAZFrame::EdgesTAZSelector::EdgeTAZ::EdgeTAZ(EdgesTAZSelector *edgesTAZSelector) {
    // create vertical frame
    myVerticalFrame = new FXVerticalFrame(edgesTAZSelector, GUIDesignAuxiliarHorizontalFrame);
    // create Edge Label and button
    FXHorizontalFrame* horizontalFrameButton = new FXHorizontalFrame(myVerticalFrame, GUIDesignAuxiliarHorizontalFrame);
    removeButton = new FXButton(horizontalFrameButton, "", GUIIconSubSys::getIcon(ICON_REMOVE), edgesTAZSelector, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    edgeLabel = new FXLabel(horizontalFrameButton, "edge: ", 0, GUIDesignLabelLeftThick);
    // create Label and textfield for Arrival Weight
    FXHorizontalFrame* departWeightFrame = new FXHorizontalFrame(myVerticalFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(departWeightFrame, toString(GNE_ATTR_TAZ_DEPARTWEIGHT).c_str(), 0, GUIDesignLabelAttribute);
    departWeightTextField = new FXTextField(departWeightFrame, GUIDesignTextFieldNCol, edgesTAZSelector, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Label and textfield for Arrival Weight
    FXHorizontalFrame* arrivalWeightFrame = new FXHorizontalFrame(myVerticalFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(arrivalWeightFrame, toString(GNE_ATTR_TAZ_ARRIVALWEIGHT).c_str(), 0, GUIDesignLabelAttribute);
    arrivalWeightTextField = new FXTextField(arrivalWeightFrame, GUIDesignTextFieldNCol, edgesTAZSelector, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create myVerticalFrame (and their childs)
    myVerticalFrame->create();
}


GNETAZFrame::EdgesTAZSelector::EdgeTAZ::~EdgeTAZ() {
    // delete vertical frame (and all of their childs)
    delete myVerticalFrame;
}


// ---------------------------------------------------------------------------
// GNETAZFrame::SaveTAZEdges - methods
// ---------------------------------------------------------------------------

GNETAZFrame::SaveTAZEdges::SaveTAZEdges(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "Save changes", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // Create groupbox for save changes
    mySaveChangesButton = new FXButton(this, "Save TAZ Edges", GUIIconSubSys::getIcon(ICON_SAVE), this, MID_OK, GUIDesignButton);
    mySaveChangesButton->disable();
    // Create groupbox cancel changes
    myCancelChangesButton = new FXButton(this, "cancel TAZ Edges", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_CANCEL, GUIDesignButton);
    myCancelChangesButton->disable();
}


GNETAZFrame::SaveTAZEdges::~SaveTAZEdges() {}


void 
GNETAZFrame::SaveTAZEdges::showSaveTAZEdgesModul() {
    show();
}


void 
GNETAZFrame::SaveTAZEdges::hideSaveTAZEdgesModul() {
    hide();
}


void
GNETAZFrame::SaveTAZEdges::setSaveChangesButton(bool value) {
    if (value) {
        mySaveChangesButton->enable();
    } else {
        mySaveChangesButton->disable();
    }
}


void
GNETAZFrame::SaveTAZEdges::setCancelChangesButton(bool value) {
    if (value) {
        myCancelChangesButton->enable();
    } else {
        myCancelChangesButton->disable();
    }
}


long
GNETAZFrame::SaveTAZEdges::onCmdSaveChanges(FXObject*, FXSelector, void*) {


    return 1;
}


long
GNETAZFrame::SaveTAZEdges::onCmdCancelChanges(FXObject*, FXSelector, void*) {

    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZParameters- methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZParameters::TAZParameters(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ parameters", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // create label and string textField for edges
    FXHorizontalFrame* TAZParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myColorEditor = new FXButton(TAZParameter, toString(SUMO_ATTR_COLOR).c_str(), 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myTextFieldColor = new FXTextField(TAZParameter, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // set blue as default color
    myTextFieldColor->setText("blue");
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

    /*
    // Create groupbox and labels for legends
    FXGroupBox *groupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);
    FXLabel* colorCandidateLabel = new FXLabel(groupBoxLegend, "Candidate", 0, GUIDesignLabelLeft);
    colorCandidateLabel->setBackColor(MFXUtils::getFXColor(myTAZParameters->getCandidateColor()));
    FXLabel *colorSelectedLabel = new FXLabel(groupBoxLegend, "Selected", 0, GUIDesignLabelLeft);
    colorSelectedLabel->setBackColor(MFXUtils::getFXColor(myTAZParameters->getSelectedColor()));
    */
    // by default there isn't a TAZ
    myCurrentTAZ->setCurrentTAZ(nullptr);
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
    } else if (myCurrentTAZ->getCurrentTAZ() == nullptr) {
        // avoid reset of Frame if user doesn't click over an TAZ
        if(TAZ) {
            myCurrentTAZ->setCurrentTAZ(TAZ);
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
