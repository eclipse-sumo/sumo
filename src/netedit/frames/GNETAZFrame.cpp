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

/*
FXDEFMAP(GNETAZFrame::EdgesSelector) EdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEPolygonFrame::ShapeAttributeSingle::onCmdSetColorAttribute),
};
*/
FXDEFMAP(GNETAZFrame::TAZParameters) TAZParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE_DIALOG,    GNETAZFrame::TAZParameters::onCmdSetColorAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,           GNETAZFrame::TAZParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                        GNETAZFrame::TAZParameters::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNETAZFrame::TAZParameters,     FXGroupBox,     TAZParametersMap,   ARRAYNUMBER(TAZParametersMap))
//FXIMPLEMENT(GNETAZFrame::EdgesSelector,     FXGroupBox,     EdgesMap,           ARRAYNUMBER(EdgesMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECrossingFrame::CurrentJunction - methods
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
        // show edge selector
        myTAZFrameParent->myEdgeSelector->show();
    } else {
        myCurrentTAZLabel->setText("No TAZ selected");
        // show TAZ parameters
        myTAZFrameParent->myTAZParameters->showTAZParametersModul();
        // show Netedit parameters
        myTAZFrameParent->myNeteditAttributes->showNeteditAttributesModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_TAZ));
        // show drawing shape
        myTAZFrameParent->myDrawingShape->showDrawingShape();
        // hide edge selector
        myTAZFrameParent->myEdgeSelector->hide();
    }
}


GNETAZ* 
GNETAZFrame::CurrentTAZ::getCurrentTAZ() const {
    return myCurrentTAZ;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::EdgesSelector - methods
// ---------------------------------------------------------------------------

GNETAZFrame::EdgesSelector::EdgesSelector(GNETAZFrame* TAZFrameParent) :
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


GNETAZFrame::EdgesSelector::~EdgesSelector() {}


GNEJunction*
GNETAZFrame::EdgesSelector::getCurrentJunction() const {
    return myCurrentJunction;
}


void
GNETAZFrame::EdgesSelector::enableEdgeSelector(GNEJunction* currentJunction) {
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
GNETAZFrame::EdgesSelector::disableEdgeSelector() {
    // disable current junction
    myCurrentJunction = NULL;
    // disable all elements of the EdgesSelector
    myUseSelectedEdges->disable();
    myClearEdgesSelection->disable();
    myInvertEdgesSelection->disable();
}


void
GNETAZFrame::EdgesSelector::restoreEdgeColors() {
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

// ---------------------------------------------------------------------------
// GNETAZFrame::NeteditAttributes- methods
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
    return (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text()) == false);
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

    // Create edge Selector modul
    myEdgeSelector = new EdgesSelector(this);

    /*
    // Create groupbox and labels for legends
    FXGroupBox *groupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);
    FXLabel* colorCandidateLabel = new FXLabel(groupBoxLegend, "Candidate", 0, GUIDesignLabelLeft);
    colorCandidateLabel->setBackColor(MFXUtils::getFXColor(myTAZParameters->getCandidateColor()));
    FXLabel *colorSelectedLabel = new FXLabel(groupBoxLegend, "Selected", 0, GUIDesignLabelLeft);
    colorSelectedLabel->setBackColor(MFXUtils::getFXColor(myTAZParameters->getSelectedColor()));
    */
    // disable edge selector
    myEdgeSelector->disableEdgeSelector();

    // by default there isn't a TAZ
    myCurrentTAZ->setCurrentTAZ(nullptr);
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


GNETAZFrame::AddTAZResult 
GNETAZFrame::processClick(const Position& clickedPosition, GNEEdge* edge) {
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement;
    // obtain TAZ values
    //valuesOfElement = myTAZAttributes->getAttributesAndValues();
    if (myDrawingShape->isDrawing()) {
        // add or delete a new point depending of flag "delete last created point"
        if (myDrawingShape->getDeleteLastCreatedPoint()) {
            myDrawingShape->removeLastPoint();
        } else {
            myDrawingShape->addNewPoint(clickedPosition);
        }
        return ADDTAZ_UPDATEDTEMPORALTAZ;
    } else {
        // return ADDTAZ_NOTHING if is drawing isn't enabled
        return ADDTAZ_NOTHING;
    }
}

/*
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
*/


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
    /*if (myTAZParameters->isCurrentParametersValid() == false) {
        return false;
    } else */if(myDrawingShape->getTemporalShape().size() == 0) {
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
