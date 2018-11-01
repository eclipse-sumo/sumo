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

FXDEFMAP(GNETAZFrame::TAZSaveEdges) TAZSaveEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_OK,         GNETAZFrame::TAZSaveEdges::onCmdSaveChanges),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,     GNETAZFrame::TAZSaveEdges::onCmdCancelChanges),
};

FXDEFMAP(GNETAZFrame::TAZEdgesCommonParameters) TAZEdgesCommonParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNETAZFrame::TAZEdgesCommonParameters::onCmdSetDefaultValues),
};

FXDEFMAP(GNETAZFrame::TAZEdgesSelector) TAZEdgesSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,      GNETAZFrame::TAZEdgesSelector::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,   GNETAZFrame::TAZEdgesSelector::onCmdRemoveEdgeTAZ),
};

// Object implementation
FXIMPLEMENT(GNETAZFrame::TAZParameters,             FXGroupBox,     TAZParametersMap,               ARRAYNUMBER(TAZParametersMap))
FXIMPLEMENT(GNETAZFrame::TAZSaveEdges,              FXGroupBox,     TAZSaveEdgesMap,                ARRAYNUMBER(TAZSaveEdgesMap))
FXIMPLEMENT(GNETAZFrame::TAZEdgesCommonParameters,  FXGroupBox,     TAZEdgesCommonParametersMap,    ARRAYNUMBER(TAZEdgesCommonParametersMap))
FXIMPLEMENT(GNETAZFrame::TAZEdgesSelector,          FXGroupBox,     TAZEdgesSelectorMap,            ARRAYNUMBER(TAZEdgesSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZCurrent - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZCurrent::TAZCurrent(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myTAZCurrent(nullptr) {
    // create TAZ label
    myTAZCurrentLabel = new FXLabel(this, "No TAZ selected", 0, GUIDesignLabelLeft);
}


GNETAZFrame::TAZCurrent::~TAZCurrent() {}


void 
GNETAZFrame::TAZCurrent::setTAZ(GNETAZ* TAZCurrent) {
    // set new current TAZ
    myTAZCurrent = TAZCurrent;
    // update label and moduls
    if(myTAZCurrent != nullptr) {
        myTAZCurrentLabel->setText((std::string("Current TAZ: ") + myTAZCurrent->getID()).c_str());
        // hide TAZ parameters
        myTAZFrameParent->myTAZParameters->hideTAZParametersModul();
        // hide Netedit parameters
        myTAZFrameParent->myNeteditAttributes->hideNeteditAttributesModul();
        // hide drawing shape
        myTAZFrameParent->myDrawingShape->hideDrawingShape();
        // show save TAZ Edges
        myTAZFrameParent->myTAZSaveEdges->showTAZSaveEdgesModul();
        // show edge common parameters
        myTAZFrameParent->myTAZEdgesCommonParameters->showTAZEdgesCommonParametersModul();
        // show edge selector
        myTAZFrameParent->myTAZEdgesSelector->showEdgeTAZSelectorModul();
    } else {
        myTAZCurrentLabel->setText("No TAZ selected");
        // show TAZ parameters
        myTAZFrameParent->myTAZParameters->showTAZParametersModul();
        // show Netedit parameters
        myTAZFrameParent->myNeteditAttributes->showNeteditAttributesModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_TAZ));
        // show drawing shape
        myTAZFrameParent->myDrawingShape->showDrawingShape();
        // show edge common parameters
        myTAZFrameParent->myTAZEdgesCommonParameters->hideTAZEdgesCommonParametersModul();
        // hide save TAZ Edges
        myTAZFrameParent->myTAZSaveEdges->hideTAZSaveEdgesModul();
        // hide edge selector
        myTAZFrameParent->myTAZEdgesSelector->hideEdgeTAZSelectorModul();
    }
}


GNETAZ* 
GNETAZFrame::TAZCurrent::getTAZ() const {
    return myTAZCurrent;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::EdgesSelector - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZEdgesSelector::TAZEdgesSelector(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, ("Selection of " + toString(SUMO_TAG_EDGE) + "s").c_str(), GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    myCurrentEdgesLabel = new FXLabel(this, "TAZ without edges", 0, GUIDesignLabelLeft);
    // update current edges label
    if(myTAZFrameParent->getTAZCurrent()->getTAZ() && (myTAZFrameParent->getTAZCurrent()->getTAZ()->getEdgeChilds().size() > 0)) {
        myCurrentEdgesLabel->hide();
    } else {
        myCurrentEdgesLabel->show();
    }
    // update list
    updateList();
}


GNETAZFrame::TAZEdgesSelector::~TAZEdgesSelector() {
}


void 
GNETAZFrame::TAZEdgesSelector::showEdgeTAZSelectorModul() {
    // update list
    updateList();
    show();
}


void 
GNETAZFrame::TAZEdgesSelector::hideEdgeTAZSelectorModul() {
    // remove all EdgeTAZRows
    for (auto i : myEdgeTAZRows) {
        delete i;
    }
    myEdgeTAZRows.clear();
    // hidde selector modul
    hide();
}


bool 
GNETAZFrame::TAZEdgesSelector::selectEdge(GNEEdge *edge) {
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
        myTAZFrameParent->myTAZSaveEdges->enableButtons();
        // if edge was found, that means that GNETAZSource and GNETAZSink doesn't exist, then create it using the values of myTAZEdgesCommonParameters
        GNETAZSource* TAZSource = new GNETAZSource(myTAZFrameParent->getTAZCurrent()->getTAZ(), edge, myTAZFrameParent->myTAZEdgesCommonParameters->getDefaultTAZSourceWeight());
        GNETAZSink* TAZSink = new GNETAZSink(myTAZFrameParent->getTAZCurrent()->getTAZ(), edge, myTAZFrameParent->myTAZEdgesCommonParameters->getDefaultTAZSinkWeight());
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
GNETAZFrame::TAZEdgesSelector::updateList() {
    // remove all EdgeTAZRows
    for (auto i : myEdgeTAZRows) {
        delete i;
    }
    myEdgeTAZRows.clear();
    // fill myEdgeTAZRows again
    if (myTAZFrameParent->myTAZCurrent->getTAZ()) {
        // declare a container to group TAZ Childs by Edge
        std::map<std::string, std::pair<GNEAdditional*, GNEAdditional*> > TAZChilds;
        for (auto i : myTAZFrameParent->myTAZCurrent->getTAZ()->getAdditionalChilds()) {
            // first check if a new pair has to be added to TAZChilds
            if(TAZChilds.count(i->getAttribute(SUMO_ATTR_EDGE)) == 0) {
                TAZChilds[i->getAttribute(SUMO_ATTR_EDGE)] = std::make_pair(nullptr, nullptr);
            }
            // set TAZChilds depending of TAZ child type
            if(i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
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
GNETAZFrame::TAZEdgesSelector::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // find edited attribute
    for (auto i : myEdgeTAZRows) {
        if (obj == i->getDepartWeightTextField()) {
            // check if attribute is valid
            if (GNEAttributeCarrier::canParse<double>(i->getDepartWeightTextField()->getText().text()) && (GNEAttributeCarrier::parse<double>(i->getDepartWeightTextField()->getText().text()) >= 0)) {
                // enable save buttons (To begin undo_list)
                myTAZFrameParent->myTAZSaveEdges->enableButtons();
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
                myTAZFrameParent->myTAZSaveEdges->enableButtons();
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
GNETAZFrame::TAZEdgesSelector::onCmdRemoveEdgeTAZ(FXObject* obj, FXSelector, void*) {
    // search remove button in all EdgeTAZs
    for (int i = 0; i < (int)myEdgeTAZRows.size(); i++) {
        if(myEdgeTAZRows.at(i)->getRemoveButton() == obj) {
            // enable save buttons and begin undo_list
            myTAZFrameParent->myTAZSaveEdges->enableButtons();
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


GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::EdgeTAZRow(TAZEdgesSelector *TAZEdgesSelector, GNEEdge *edge, GNEAdditional *TAZSource, GNEAdditional *TAZSink) :
    FXVerticalFrame(TAZEdgesSelector, GUIDesignAuxiliarHorizontalFrame),
    myTAZEdgesSelectorParent(TAZEdgesSelector),
    myEdge(edge),
    myEditedTAZSource(TAZSource),
    myEditedTAZSink(TAZSink) {
    // create Edge Label and button
    FXHorizontalFrame* horizontalFrameButton = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myRemoveButton = new FXButton(horizontalFrameButton, "", GUIIconSubSys::getIcon(ICON_REMOVE), TAZEdgesSelector, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    myEdgeLabel = new FXLabel(horizontalFrameButton, toString("edge: " + edge->getID()).c_str(), 0, GUIDesignLabelLeftThick);
    // create Label and textfield for Arrival Weight
    FXHorizontalFrame* departWeightFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(departWeightFrame, "Source weight", 0, GUIDesignLabelAttribute);
    myDepartWeightTextField = new FXTextField(departWeightFrame, GUIDesignTextFieldNCol, TAZEdgesSelector, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myDepartWeightTextField->setText(TAZSource->getAttribute(SUMO_ATTR_WEIGHT).c_str());
    // create Label and textfield for Arrival Weight
    FXHorizontalFrame* arrivalWeightFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(arrivalWeightFrame, "Sink weight", 0, GUIDesignLabelAttribute);
    myArrivalWeightTextField = new FXTextField(arrivalWeightFrame, GUIDesignTextFieldNCol, TAZEdgesSelector, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myArrivalWeightTextField->setText(TAZSink->getAttribute(SUMO_ATTR_WEIGHT).c_str());
    // set edge color
    for (auto j : myEdge->getLanes()) {
        j->setSpecialColor(&myTAZEdgesSelectorParent->myTAZFrameParent->getEdgeCandidateSelectedColor());
    }
    // create EdgeTAZRow (and their childs)
    create();
}


GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::~EdgeTAZRow() {
    // restore edge color
    for (auto j : myEdge->getLanes()) {
        j->setSpecialColor(nullptr);
    }
}


GNEAdditional*
GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::getEditedTAZSource() const {
    return myEditedTAZSource;
}


GNEAdditional*
GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::getEditedTAZSink() const {
    return myEditedTAZSink;
}


FXButton*
GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::getRemoveButton() const {
    return myRemoveButton;
}


FXTextField*
GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::getDepartWeightTextField() const {
    return myDepartWeightTextField;
}


FXTextField*
GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::getArrivalWeightTextField() const {
    return myArrivalWeightTextField;
}


GNEEdge*
GNETAZFrame::TAZEdgesSelector::EdgeTAZRow::getEdge() const {
    return myEdge;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZSaveEdges - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZSaveEdges::TAZSaveEdges(GNETAZFrame* TAZFrameParent) : 
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


GNETAZFrame::TAZSaveEdges::~TAZSaveEdges() {}


void 
GNETAZFrame::TAZSaveEdges::showTAZSaveEdgesModul() {
    show();
}


void 
GNETAZFrame::TAZSaveEdges::hideTAZSaveEdgesModul() {
    // cancel changes
    onCmdCancelChanges(0,0,0);
    hide();
}


void
GNETAZFrame::TAZSaveEdges::enableButtons() {
    // enable mySaveChangesButton and myCancelChangesButton, and start a undolist set
    if (!mySaveChangesButton->isEnabled()) {
        mySaveChangesButton->enable();
        myCancelChangesButton->enable();
        myTAZFrameParent->myViewNet->getUndoList()->p_begin("TAZ attributes");
    }
}

long
GNETAZFrame::TAZSaveEdges::onCmdSaveChanges(FXObject*, FXSelector, void*) {
    // disable mySaveChangesButton and myCancelChangesButtonand, and finish undolist set
    if (mySaveChangesButton->isEnabled()) {
        mySaveChangesButton->disable();
        myCancelChangesButton->disable();
        myTAZFrameParent->myViewNet->getUndoList()->p_end();
    }
    return 1;
}


long
GNETAZFrame::TAZSaveEdges::onCmdCancelChanges(FXObject*, FXSelector, void*) {
    // cancel changes and disable buttons
    if (mySaveChangesButton->isEnabled()) {
        myTAZFrameParent->myViewNet->getUndoList()->p_abort();
        mySaveChangesButton->disable();
        myCancelChangesButton->disable();
        // update list of myTAZEdgesSelector
        myTAZFrameParent->myTAZEdgesSelector->updateList();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZEdgesCommonParameters - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZEdgesCommonParameters::TAZEdgesCommonParameters(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "Default values", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myDefaultTAZSourceWeight(1),
    myDefaultTAZSinkWeight(1) {
    // create checkbox for toogle membership
    FXHorizontalFrame* toogleMembershipFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(toogleMembershipFrame, "Membership", 0, GUIDesignLabelAttribute);
    myToggleMembership = new FXCheckButton(toogleMembershipFrame, "Toggle", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    // by default enabled
    myToggleMembership->setCheck(TRUE);
    // create default TAZ Source weight
    FXHorizontalFrame* defaultTAZSourcesFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myCheckBoxSetDefaultValueTAZSources = new FXCheckButton(defaultTAZSourcesFrame, "Def. source", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttributeLabel);
    myTextFieldDefaultValueTAZSources = new FXTextField(defaultTAZSourcesFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTextFieldDefaultValueTAZSources->setText("1");
    // by default disable
    myCheckBoxSetDefaultValueTAZSources->setCheck(FALSE);
    myTextFieldDefaultValueTAZSources->disable();
    // create default TAZ Sink weight
    FXHorizontalFrame* defaultTAZSinksFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myCheckBoxSetDefaultValueTAZSinks = new FXCheckButton(defaultTAZSinksFrame, "Def. sink", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttributeLabel);
    myTextFieldDefaultValueTAZSinks = new FXTextField(defaultTAZSinksFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTextFieldDefaultValueTAZSinks->setText("1");
    // by default disable
    myCheckBoxSetDefaultValueTAZSinks->setCheck(FALSE);
    myTextFieldDefaultValueTAZSinks->disable();
}


GNETAZFrame::TAZEdgesCommonParameters::~TAZEdgesCommonParameters() {}


void 
GNETAZFrame::TAZEdgesCommonParameters::showTAZEdgesCommonParametersModul() {
    show();
}


void 
GNETAZFrame::TAZEdgesCommonParameters::hideTAZEdgesCommonParametersModul() {
    hide();
}


double 
GNETAZFrame::TAZEdgesCommonParameters::getDefaultTAZSourceWeight() const {
    if (myCheckBoxSetDefaultValueTAZSources->getCheck() == TRUE) {
        return myDefaultTAZSourceWeight;
    } else {
        return 1;
    }
}


double 
GNETAZFrame::TAZEdgesCommonParameters::getDefaultTAZSinkWeight() const {
    if (myCheckBoxSetDefaultValueTAZSinks->getCheck() == TRUE) {
        return myDefaultTAZSinkWeight;
    } else {
        return 1;
    }
}


long
GNETAZFrame::TAZEdgesCommonParameters::onCmdSetDefaultValues(FXObject* obj, FXSelector, void*) {
    // find object
    if(obj == myToggleMembership) {
        // set text of myToggleMembership
        if (myToggleMembership->getCheck() == TRUE) {
            myToggleMembership->setText("toogle");
        } else {
            myToggleMembership->setText("keep");
        }
    } else if (obj == myCheckBoxSetDefaultValueTAZSources) {
        // enable or disable myTextFieldDefaultValueTAZSources
        if (myCheckBoxSetDefaultValueTAZSources->getCheck() == TRUE) {
            myTextFieldDefaultValueTAZSources->enable();
        } else {
            myTextFieldDefaultValueTAZSources->disable();
        }
    } else if (obj == myTextFieldDefaultValueTAZSources) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldDefaultValueTAZSources->getText().text())) {
            myDefaultTAZSourceWeight = GNEAttributeCarrier::parse<double>(myTextFieldDefaultValueTAZSources->getText().text());
            // check if myDefaultTAZSourceWeight is greather than 0
            if (myDefaultTAZSourceWeight >= 0) {
                // set valid color
                myTextFieldDefaultValueTAZSources->setTextColor(FXRGB(0, 0, 0));
            } else {
                // set invalid color
                myTextFieldDefaultValueTAZSources->setTextColor(FXRGB(255, 0, 0));
                myDefaultTAZSourceWeight = 1;
            }
        } else {
            // set invalid color
            myTextFieldDefaultValueTAZSources->setTextColor(FXRGB(255, 0, 0));
            myDefaultTAZSourceWeight = 1;
        }
    } else if (obj == myCheckBoxSetDefaultValueTAZSinks) {
        // enable or disable myTextFieldDefaultValueTAZSources
        if (myCheckBoxSetDefaultValueTAZSinks->getCheck() == TRUE) {
            myTextFieldDefaultValueTAZSinks->enable();
        } else {
            myTextFieldDefaultValueTAZSinks->disable();
        }
    } else if (obj == myTextFieldDefaultValueTAZSinks) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldDefaultValueTAZSinks->getText().text())) {
            myDefaultTAZSinkWeight = GNEAttributeCarrier::parse<double>(myTextFieldDefaultValueTAZSinks->getText().text());
            // check if myDefaultTAZSinkWeight is greather than 0
            if (myDefaultTAZSinkWeight >= 0) {
                // set valid color
                myTextFieldDefaultValueTAZSinks->setTextColor(FXRGB(0, 0, 0));
            } else {
                // set invalid color
                myTextFieldDefaultValueTAZSinks->setTextColor(FXRGB(255, 0, 0));
                myDefaultTAZSinkWeight = 1;
            }
        } else {
            // set invalid color
            myTextFieldDefaultValueTAZSinks->setTextColor(FXRGB(255, 0, 0));
            myDefaultTAZSinkWeight = 1;
        }
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
    new FXLabel(useInnenEdges, "Edges within", 0, GUIDesignLabelAttribute);
    myAddEdgesWithinCheckButton = new FXCheckButton(useInnenEdges, "use", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myAddEdgesWithinCheckButton->setCheck(true);
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
GNETAZFrame::TAZParameters::isAddEdgesWithinEnabled() const {
    return (myAddEdgesWithinCheckButton->getCheck() == TRUE);
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
    if(myAddEdgesWithinCheckButton->getCheck() == TRUE) {
        myAddEdgesWithinCheckButton->setText("use");
    } else {
        myAddEdgesWithinCheckButton->setText("not use");
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
    myTAZCurrent = new TAZCurrent(this);

    // Create TAZ Parameters modul
    myTAZParameters = new TAZParameters(this);

    /// @brief create  Netedit parameter
    myNeteditAttributes = new NeteditAttributes(this);

    // Create drawing controls modul
    myDrawingShape = new DrawingShape(this);

    // Create save TAZ Edges modul
    myTAZSaveEdges = new TAZSaveEdges(this);

    // Create TAZ Edges Common Parameters modul
    myTAZEdgesCommonParameters = new TAZEdgesCommonParameters(this);

    // Create edge Selector modul
    myTAZEdgesSelector = new TAZEdgesSelector(this);

    // by default there isn't a TAZ
    myTAZCurrent->setTAZ(nullptr);
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
    } else if (myTAZCurrent->getTAZ() == nullptr) {
        // avoid reset of Frame if user doesn't click over an TAZ
        if(TAZ) {
            myTAZCurrent->setTAZ(TAZ);
            return true;
        } else {
            return false;
        }
    } else if (edge) {
        // select edge if is unselected, or unselect if it was already selected
        myTAZEdgesSelector->selectEdge(edge);
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


GNETAZFrame::TAZCurrent* 
GNETAZFrame::getTAZCurrent() const {
    return myTAZCurrent;
}


bool
GNETAZFrame::buildShape() {
    // show warning dialogbox and stop check if input parameters are valid
    if (!myTAZParameters->isCurrentParametersValid()) {
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
        if (myTAZParameters->isAddEdgesWithinEnabled()) {
            std::vector<std::string> edgeIDs;
            auto ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(shape.getBoxBoundary(), true);
            for (auto i : ACsInBoundary) {
                if(i.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    edgeIDs.push_back(i.first);
                }
            }
            valuesOfElement[SUMO_ATTR_EDGES] = toString(edgeIDs);
        } else {
            // TAZ is created without edges
            valuesOfElement[SUMO_ATTR_EDGES] = "";
        }
        // return true if TAZ was successfully created
        return GNEAdditionalHandler::buildAdditional(myViewNet, true, SUMO_TAG_TAZ, valuesOfElement) != nullptr;
    }
}


void 
GNETAZFrame::enableModuls(const GNEAttributeCarrier::TagProperties &/*tagValue*/) {

}


void 
GNETAZFrame::disableModuls() {

}

/****************************************************************************/
