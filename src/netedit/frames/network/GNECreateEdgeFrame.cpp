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
/// @file    GNECreateEdgeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// The Widget for create edges (and junctions)
/****************************************************************************/
#include <config.h>

#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/GNEAllowDisallow.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNECreateEdgeFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECreateEdgeFrame::CustomEdgeSelector) CustomEdgeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON,  GNECreateEdgeFrame::CustomEdgeSelector::onCmdRadioButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_NUMLANES,           GNECreateEdgeFrame::CustomEdgeSelector::onCmdChangeNumLanes),

};

FXDEFMAP(GNECreateEdgeFrame::EdgeParameters) EdgeParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNECreateEdgeFrame::EdgeParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNECreateEdgeFrame::EdgeParameters::onCmdOpenAttributeDialog),
};

FXDEFMAP(GNECreateEdgeFrame::LaneParameters) LaneParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNECreateEdgeFrame::LaneParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNECreateEdgeFrame::LaneParameters::onCmdOpenAttributeDialog),
};

// Object implementation
FXIMPLEMENT(GNECreateEdgeFrame::CustomEdgeSelector,     FXGroupBox,     CustomEdgeSelectorMap,  ARRAYNUMBER(CustomEdgeSelectorMap))
FXIMPLEMENT(GNECreateEdgeFrame::EdgeParameters,         FXGroupBox,     EdgeParametersMap,      ARRAYNUMBER(EdgeParametersMap))
FXIMPLEMENT(GNECreateEdgeFrame::LaneParameters,         FXGroupBox,     LaneParametersMap,      ARRAYNUMBER(LaneParametersMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeParameters - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeParameters::EdgeParameters(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Edge parameters", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // declare horizontalFrameAttribute
    FXHorizontalFrame* horizontalFrameAttribute = nullptr;
    // create textField for speed
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPEED).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpeed = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for priority
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_PRIORITY).c_str(), nullptr, GUIDesignLabelAttribute);
    myPriority = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for numLanes
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_NUMLANES).c_str(), nullptr, GUIDesignLabelAttribute);
    myNumLanes = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldInt);
    // create textField for Type
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_TYPE).c_str(), nullptr, GUIDesignLabelAttribute);
    myType = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for allow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    myAllowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_ALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAllow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for disallow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    myDisallowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_DISALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myDisallow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create ComboBox for spread type
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPREADTYPE).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpreadType = new FXComboBox(horizontalFrameAttribute, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // fill comboBox
    mySpreadType->insertItem(0, "Right");
    mySpreadType->insertItem(1, "RoadCenter");
    mySpreadType->insertItem(2, "Center");
    // create textField for name
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_NAME).c_str(), nullptr, GUIDesignLabelAttribute);
    myName = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for width
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_WIDTH).c_str(), nullptr, GUIDesignLabelAttribute);
    myWidth = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for distance
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_DISTANCE).c_str(), nullptr, GUIDesignLabelAttribute);
    myDistance = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // fill default parameters
    fillDefaultParameters();
}


GNECreateEdgeFrame::EdgeParameters::~EdgeParameters() {}


void 
GNECreateEdgeFrame::EdgeParameters::setAttributes(GNEEdge* edge, GNEUndoList *undoList) const {
    // set speed
    edge->setAttribute(SUMO_ATTR_SPEED, toString(mySpeed->getText().text()), undoList);
    // set priority
    edge->setAttribute(SUMO_ATTR_PRIORITY, toString(myPriority->getText().text()), undoList);
    // set num lanes
    edge->setAttribute(SUMO_ATTR_NUMLANES, toString(myNumLanes->getText().text()), undoList);
    // set allow (no disallow)
    edge->setAttribute(SUMO_ATTR_ALLOW, toString(myAllow->getText().text()), undoList);
    // set spreadtype
    edge->setAttribute(SUMO_ATTR_SPREADTYPE, toString(mySpreadType->getText().text()), undoList);
    // set name
    edge->setAttribute(SUMO_ATTR_NAME, toString(myName->getText().text()), undoList);
    // set witdth
    edge->setAttribute(SUMO_ATTR_WIDTH, toString(myWidth->getText().text()), undoList);
    // set distance
    edge->setAttribute(SUMO_ATTR_DISTANCE, toString(myDistance->getText().text()), undoList);
}


long 
GNECreateEdgeFrame::EdgeParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNECreateEdgeFrame::EdgeParameters::onCmdOpenAttributeDialog(FXObject*, FXSelector, void*) {
    // declare strings
    std::string allow = myAllow->getText().text();
    std::string disallow = myDisallow->getText().text();
    // open dialog
    GNEAllowDisallow(myCreateEdgeFrameParent->getViewNet(), &allow, &disallow).execute();
    // update allow/disallow
    myAllow->setText(allow.c_str(), FALSE);
    myDisallow->setText(disallow.c_str(), FALSE);
    return 1;
}


void 
GNECreateEdgeFrame::EdgeParameters::fillDefaultParameters() {
    // set speed
    mySpeed->setText("13.89");
    // set priority
    myPriority->setText("-1");
    // set numLanes
    myNumLanes->setText("1");
    // set type
    myType->setText("");
    // set allow
    myAllow->setText("all");
    // set disallow
    myDisallow->setText("");
    // set spreadType
    mySpreadType->setText("right");
    // set name
    myName->setText("");
    // set width
    myWidth->setText("-1.00");
    // set distance
    myDistance->setText("0.00");
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::LaneParameters - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::LaneParameters::LaneParameters(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Lane parameters", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // declare horizontalFrameAttribute
    FXHorizontalFrame* horizontalFrameAttribute = nullptr;

    /*
    // create ComboBox for spread type
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
        new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPREADTYPE).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpreadType = new FXComboBox(horizontalFrameAttribute, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // fill comboBox
    mySpreadType->insertItem(0, "Right");
    mySpreadType->insertItem(1, "RoadCenter");
    mySpreadType->insertItem(2, "Center");
    */

    // create textField for speed
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
        new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPEED).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpeed = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);

    // create Button for allow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
        myAllowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_ALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAllow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for disallow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
        myDisallowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_DISALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myDisallow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for width
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
        new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_WIDTH).c_str(), nullptr, GUIDesignLabelAttribute);
    myWidth = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // fill default parameters
    fillDefaultParameters();
}


GNECreateEdgeFrame::LaneParameters::~LaneParameters() {}


void 
GNECreateEdgeFrame::LaneParameters::setAttributes(GNEEdge* edge, GNEUndoList *undoList) const {
    // set speed
    edge->setAttribute(SUMO_ATTR_SPEED, toString(mySpeed->getText().text()), undoList);
    // set allow (no disallow)
    edge->setAttribute(SUMO_ATTR_ALLOW, toString(myAllow->getText().text()), undoList);
    // set witdth
    edge->setAttribute(SUMO_ATTR_WIDTH, toString(myWidth->getText().text()), undoList);
}


long 
GNECreateEdgeFrame::LaneParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNECreateEdgeFrame::LaneParameters::onCmdOpenAttributeDialog(FXObject*, FXSelector, void*) {
    // declare strings
    std::string allow = myAllow->getText().text();
    std::string disallow = myDisallow->getText().text();
    // open dialog
    GNEAllowDisallow(myCreateEdgeFrameParent->getViewNet(), &allow, &disallow).execute();
    // update allow/disallow
    myAllow->setText(allow.c_str(), FALSE);
    myDisallow->setText(disallow.c_str(), FALSE);
    return 1;
}


void 
GNECreateEdgeFrame::LaneParameters::fillDefaultParameters() {
    // set speed
    mySpeed->setText("13.89");
    // set allow
    myAllow->setText("all");
    // set disallow
    myDisallow->setText("");
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::CustomEdgeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::CustomEdgeSelector::CustomEdgeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Custom edge selector", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // default edge radio button
    myUseDefaultEdgeRadioButton = new FXRadioButton(this, "Default edge\t\tUse default edge",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // custom edge radio button
    myCustomRadioButton = new FXRadioButton(this, "Custom edge\t\tUse a custom edge",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // add separator
    myRadioButtonSeparator = new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);
    // edge attributes radio button
    myEdgeAttributes = new FXRadioButton(this, "Use edge attributes\t\tUse edge attributes",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // lane attributes radio button
    myLaneAttributes = new FXRadioButton(this, "Use lane attributes\t\tUse lane attributes",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // create numlanes elements
    myNumLanesHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myNumLanesHorizontalFrame, toString(SUMO_ATTR_NUMLANES).c_str(), nullptr, GUIDesignLabelAttribute);
    myNumLanesSpinner = new FXSpinner(myNumLanesHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_CREATEEDGEFRAME_NUMLANES, GUIDesignSpinDialAttribute);
    myNumLanesSpinner->setRange(1, 100);
    // hide spinner
    myNumLanesHorizontalFrame->hide();
    // by default, use default edge
    myUseDefaultEdgeRadioButton->setCheck(TRUE);
    // hide separator
    myRadioButtonSeparator->hide();
    // use edge attributes
    myEdgeAttributes->setCheck(TRUE);
    // hide edge/lane attributes
    myEdgeAttributes->hide();
    myLaneAttributes->hide();
}


GNECreateEdgeFrame::CustomEdgeSelector::~CustomEdgeSelector() {}


int 
GNECreateEdgeFrame::CustomEdgeSelector::getNumLanes() const {
    if (myNumLanesSpinner->shown()) {
        return myNumLanesSpinner->getValue();
    } else {
        return -1;
    }
}


long
GNECreateEdgeFrame::CustomEdgeSelector::onCmdRadioButton(FXObject* obj, FXSelector, void*) {
    // check what object was pressed
    if (obj == myUseDefaultEdgeRadioButton) {
        // update buttons
        myUseDefaultEdgeRadioButton->setCheck(TRUE, FALSE);
        myCustomRadioButton->setCheck(FALSE, FALSE);
        // hide separator
        myRadioButtonSeparator->hide();
        // hide spinner
        myNumLanesHorizontalFrame->hide();
        // hide edge/lane attributes
        myEdgeAttributes->hide();
        myLaneAttributes->hide();
    } else if (obj == myCustomRadioButton) {
        // update buttons
        myUseDefaultEdgeRadioButton->setCheck(FALSE, FALSE);
        myCustomRadioButton->setCheck(TRUE, FALSE);
        // show separator
        myRadioButtonSeparator->show();
        // show spinner
        myNumLanesHorizontalFrame->show();
        // show edge/lane attributes
        myEdgeAttributes->show();
        myLaneAttributes->show();
    } else if (obj == myEdgeAttributes) {
        // update buttons
        myEdgeAttributes->setCheck(TRUE, FALSE);
        myLaneAttributes->setCheck(FALSE, FALSE);
        /* */
    } else if (obj == myLaneAttributes) {
        // update buttons
        myEdgeAttributes->setCheck(FALSE, FALSE);
        myLaneAttributes->setCheck(TRUE, FALSE);
        /* */
    }
    // recalc
    recalc();
    return 0;
}


long
GNECreateEdgeFrame::CustomEdgeSelector::onCmdChangeNumLanes(FXObject*, FXSelector, void*) {
    //
    return 0;
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::Legend - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeSelectorLegend::EdgeSelectorLegend(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Legend", GUIDesignGroupBoxFrame) {
    // crate information
    std::ostringstream information;
    // add label for shift+click
    information
        << "- Control+Click:" << "\n"
        << "  Move view" << "\n"
        << "- Shift+Click:" << "\n"
        << "  Splits edge in both directions" << "\n"
        << "- Alt+Shift+Click:" << "\n"
        << "  Splits edge in one direction";
    // create label
    new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameThicked);
}


GNECreateEdgeFrame::EdgeSelectorLegend::~EdgeSelectorLegend() {}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::GNECreateEdgeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Create Edge"),
    myObjectsUnderSnappedCursor(viewNet),
    myCreateEdgeSource(nullptr) {
    // create edge parameters
    myEdgeParameters = new EdgeParameters(this);
    // create lane parameters
    myLaneParameters = new LaneParameters(this);
    // create custom edge selector
    myCustomEdgeSelector = new CustomEdgeSelector(this);
    // create edge selector legend
    myEdgeSelectorLegend = new EdgeSelectorLegend(this);
}


GNECreateEdgeFrame::~GNECreateEdgeFrame() {}


void
GNECreateEdgeFrame::processClick(const Position& clickedPosition, const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor,
                                 const bool oppositeEdge, const bool chainEdge) {
    // obtain junction depending of gridEnabled
    GNEJunction* junction = nullptr;
    if (objectsUnderCursor.getJunctionFront()) {
        junction = objectsUnderCursor.getJunctionFront();
    } else if (myObjectsUnderSnappedCursor.getJunctionFront()) {
        junction = myObjectsUnderSnappedCursor.getJunctionFront();
    }
    // begin undo list
    if (!myViewNet->getUndoList()->hasCommandGroup()) {
        myViewNet->getUndoList()->p_begin("create new " + toString(SUMO_TAG_EDGE));
    }
    // if we didn't clicked over another junction, then create a new
    if (junction == nullptr) {
        junction = myViewNet->getNet()->createJunction(myViewNet->snapToActiveGrid(clickedPosition), myViewNet->getUndoList());
    }
    // now check if we have to create a new edge
    if (myCreateEdgeSource == nullptr) {
        myCreateEdgeSource = junction;
        myCreateEdgeSource->markAsCreateEdgeSource();
        update();
    } else {
        // make sure that junctions source and destiny are different
        if (myCreateEdgeSource != junction) {
            // may fail to prevent double edges
            GNEEdge* newEdge = myViewNet->getNet()->createEdge(myCreateEdgeSource, junction,
                               myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList());
            // check if edge was sucesfully created
            if (newEdge) {
                // set parameters
                myEdgeParameters->setAttributes(newEdge, myViewNet->getUndoList());
                // create another edge, if create opposite edge is enabled
                if (oppositeEdge) {
                    GNEEdge* newOppositeEdge = myViewNet->getNet()->createEdge(junction, myCreateEdgeSource, 
                        myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(),
                        myViewNet->getUndoList(), "-" + newEdge->getNBEdge()->getID());
                    // set parameters
                    myEdgeParameters->setAttributes(newOppositeEdge, myViewNet->getUndoList());
                }
                // edge created, then unmark as create edge source
                myCreateEdgeSource->unMarkAsCreateEdgeSource();
                // end undo list
                if (myViewNet->getUndoList()->hasCommandGroup()) {
                    myViewNet->getUndoList()->p_end();
                } else {
                    std::cout << "edge created without an open CommandGroup )-:\n";
                }
                // if we're creating edges in chain mode, mark junction as junction edge source
                if (chainEdge) {
                    myCreateEdgeSource = junction;
                    myCreateEdgeSource->markAsCreateEdgeSource();
                    myViewNet->getUndoList()->p_begin("create new " + toString(SUMO_TAG_EDGE));
                } else {
                    myCreateEdgeSource = nullptr;
                }
            } else {
                myViewNet->setStatusBarText("An " + toString(SUMO_TAG_EDGE) + " with the same geometry already exists!");
            }
        } else {
            myViewNet->setStatusBarText("Start- and endpoint for an " + toString(SUMO_TAG_EDGE) + " must be distinct!");
        }
        update();
    }
}


void
GNECreateEdgeFrame::abortEdgeCreation() {
    // if myCreateEdgeSource exist, unmark ist as create edge source
    if (myCreateEdgeSource != nullptr) {
        // remove current created edge source
        myCreateEdgeSource->unMarkAsCreateEdgeSource();
        myCreateEdgeSource = nullptr;
    }
}


const GNEJunction*
GNECreateEdgeFrame::getJunctionSource() const {
    return myCreateEdgeSource;
}


void
GNECreateEdgeFrame::updateObjectsUnderSnappedCursor(const std::vector<GUIGlObject*>& GUIGlObjects) {
    myObjectsUnderSnappedCursor.updateObjectUnderCursor(GUIGlObjects);
}


void
GNECreateEdgeFrame::show() {
    GNEFrame::show();
}


void
GNECreateEdgeFrame::hide() {
    GNEFrame::hide();
}

/****************************************************************************/
