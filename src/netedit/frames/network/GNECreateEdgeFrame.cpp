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

#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_EdgeType.h>
#include <netedit/changes/GNEChange_LaneType.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNELaneType.h>
#include <netedit/dialogs/GNEAllowDisallow.h>
#include <netimport/NITypeLoader.h>
#include <netimport/NIXMLTypesHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>


#include "GNECreateEdgeFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECreateEdgeFrame::EdgeSelector) EdgeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON,  GNECreateEdgeFrame::EdgeSelector::onCmdRadioButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE,        GNECreateEdgeFrame::EdgeSelector::onCmdAddEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE,     GNECreateEdgeFrame::EdgeSelector::onCmdDeleteEdgeType),
};

FXDEFMAP(GNECreateEdgeFrame::EdgeParameters) EdgeParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNECreateEdgeFrame::EdgeParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNECreateEdgeFrame::EdgeParameters::onCmdOpenAttributeDialog),
};

FXDEFMAP(GNECreateEdgeFrame::LaneParameters) LaneParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNECreateEdgeFrame::LaneParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNECreateEdgeFrame::LaneParameters::onCmdOpenAttributeDialog),
};

FXDEFMAP(GNECreateEdgeFrame::EdgeTypeFile) EdgeTypeFileMap[] = {
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_LOAD_PROGRAM,    GNECreateEdgeFrame::EdgeTypeFile::onCmdLoadEdgeProgram),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_SAVE_PROGRAM,    GNECreateEdgeFrame::EdgeTypeFile::onCmdSaveEdgeProgram),
};

// Object implementation
FXIMPLEMENT(GNECreateEdgeFrame::EdgeSelector,       FXGroupBox,     EdgeSelectorMap,    ARRAYNUMBER(EdgeSelectorMap))
FXIMPLEMENT(GNECreateEdgeFrame::EdgeParameters,     FXGroupBox,     EdgeParametersMap,  ARRAYNUMBER(EdgeParametersMap))
FXIMPLEMENT(GNECreateEdgeFrame::LaneParameters,     FXGroupBox,     LaneParametersMap,  ARRAYNUMBER(LaneParametersMap))
FXIMPLEMENT(GNECreateEdgeFrame::EdgeTypeFile,       FXGroupBox,     EdgeTypeFileMap,    ARRAYNUMBER(EdgeTypeFileMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeSelector::EdgeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Template selector", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // default edge radio button
    myCreateDefaultEdge = new FXRadioButton(this, "Create default edge",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // use custom edge radio button
    myUseCustomEdge = new FXRadioButton(this, "Use edgeType/template",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // edge types combo box
    myEdgeTypesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameNewSaveDelete = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create new edge type button
    myNewEdgeTypeButton = new FXButton(horizontalFrameNewSaveDelete, 
        "add\t\add edge type", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE, GUIDesignButton);
    // create delete edge type button
    myDeleteEdgeTypeButton = new FXButton(horizontalFrameNewSaveDelete, 
        "delete\t\tdelete edge type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE, GUIDesignButton);
    // by default, create custom edge
    myCreateDefaultEdge->setCheck(TRUE);
}


GNECreateEdgeFrame::EdgeSelector::~EdgeSelector() {}


void
GNECreateEdgeFrame::EdgeSelector::refreshEdgeSelector() {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // get 
    const auto &edgeTypes = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdgeTypes();
    // check if there is template
    if (templateEditor->hasTemplate() || (edgeTypes.size() > 0)) {
        // enable both buttons
        myCreateDefaultEdge->enable();
        myUseCustomEdge->enable();
        // enable combo box
        myEdgeTypesComboBox->enable();
        // clear edge types
        myEdgeTypesComboBox->clearItems();
        // add template
        if (templateEditor->hasTemplate()) {
            myEdgeTypesComboBox->appendItem(("template: " + templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_ID)).c_str(), nullptr);
        }
        // add edge types
        for (const auto &edgeType : edgeTypes) {
            myEdgeTypesComboBox->appendItem(edgeType.second->getID().c_str(), nullptr);
        }
        // set num visible antes
        if (myEdgeTypesComboBox->getNumItems() <= 10) {
            myEdgeTypesComboBox->setNumVisible(myEdgeTypesComboBox->getNumItems());
        } else {
            myEdgeTypesComboBox->setNumVisible(10);
        }
    } else {
        // disable use custom edge
        myCreateDefaultEdge->enable();
        myUseCustomEdge->disable();
        // disable combo box
        myEdgeTypesComboBox->disable();
        // enable custom edge radio button
        myCreateDefaultEdge->setCheck(TRUE, FALSE);
    }
    // show editor parameter
    if (myUseCustomEdge->getCheck() == TRUE) {
        myCreateEdgeFrameParent->myEdgeParameters->showEdgeParameters();
        myCreateEdgeFrameParent->myLaneParameters->showLaneParameters();
    } else {
        myCreateEdgeFrameParent->myEdgeParameters->hideEdgeParameters();
        myCreateEdgeFrameParent->myLaneParameters->hideLaneParameters();
    }
    // recalc
    recalc();
}


bool 
GNECreateEdgeFrame::EdgeSelector::useEdgeTemplate() const {
    if (myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->hasTemplate()) {
        if (myUseCustomEdge->getCheck() == TRUE) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


long
GNECreateEdgeFrame::EdgeSelector::onCmdRadioButton(FXObject* obj, FXSelector, void*) {
    // check what object was pressed
    if (obj == myCreateDefaultEdge) {
        // update buttons
        myCreateDefaultEdge->setCheck(TRUE, FALSE);
        myUseCustomEdge->setCheck(FALSE, FALSE);
    } else {
        // update buttons
        myCreateDefaultEdge->setCheck(FALSE, FALSE);
        myUseCustomEdge->setCheck(TRUE, FALSE);
    }
    // refresh template selector
    refreshEdgeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeSelector::onCmdAddEdgeType(FXObject*, FXSelector, void*) {
    // create new edge type
    GNEEdgeType* edgeType = new GNEEdgeType(myCreateEdgeFrameParent->getViewNet()->getNet());
    // also create a new laneType
    GNELaneType *laneType = new GNELaneType(edgeType);
    // add it using undoList
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->p_begin("create new edge type");
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, true), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_LaneType(laneType, true), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->p_end();
    return 0;
}


long
GNECreateEdgeFrame::EdgeSelector::onCmdDeleteEdgeType(FXObject*, FXSelector, void*) {
    return 0;
}

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
GNECreateEdgeFrame::EdgeParameters::showEdgeParameters() {
    show();
}


void 
GNECreateEdgeFrame::EdgeParameters::hideEdgeParameters() {
    hide();
}


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
GNECreateEdgeFrame::EdgeParameters::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myNumLanes) {
        myCreateEdgeFrameParent->myLaneParameters->updateNumLanes(GNEAttributeCarrier::parse<int>(myNumLanes->getText().text()));
    }
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
    myEdgeAttributes[SUMO_ATTR_SPEED] = "13.89";
    mySpeed->setText("13.89");
    // set priority
    myEdgeAttributes[SUMO_ATTR_PRIORITY] = "-1";
    myPriority->setText("-1");
    // set numLanes
    myEdgeAttributes[SUMO_ATTR_NUMLANES] = "1";
    myNumLanes->setText("1");
    // set type
    myEdgeAttributes[SUMO_ATTR_TYPE] = "";
    myType->setText("");
    // set allow
    myEdgeAttributes[SUMO_ATTR_ALLOW] = "all";
    myAllow->setText("all");
    // set disallow
    myEdgeAttributes[SUMO_ATTR_DISALLOW] = "";
    myDisallow->setText("");
    // set spreadType
    myEdgeAttributes[SUMO_ATTR_SPREADTYPE] = "right";
    mySpreadType->setText("right");
    // set name
    myEdgeAttributes[SUMO_ATTR_NAME] = "";
    myName->setText("");
    // set width
    myEdgeAttributes[SUMO_ATTR_WIDTH] = "-1.00";
    myWidth->setText("-1.00");
    // set distance
    myEdgeAttributes[SUMO_ATTR_DISTANCE] = "0.00";
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
    // create ComboBox for spread type
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
        new FXLabel(horizontalFrameAttribute, "Lane index", nullptr, GUIDesignLabelAttribute);
    myLaneIndex = new FXComboBox(horizontalFrameAttribute, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
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
    fillDefaultParameters(0);
}


GNECreateEdgeFrame::LaneParameters::~LaneParameters() {}


void 
GNECreateEdgeFrame::LaneParameters::showLaneParameters() {
    show();
}


void
GNECreateEdgeFrame::LaneParameters::hideLaneParameters() {
    hide();
}

void 
GNECreateEdgeFrame::LaneParameters::setAttributes(GNEEdge* edge, GNEUndoList *undoList) const {
    // set speed
    edge->setAttribute(SUMO_ATTR_SPEED, toString(mySpeed->getText().text()), undoList);
    // set allow (no disallow)
    edge->setAttribute(SUMO_ATTR_ALLOW, toString(myAllow->getText().text()), undoList);
    // set witdth
    edge->setAttribute(SUMO_ATTR_WIDTH, toString(myWidth->getText().text()), undoList);
}


void 
GNECreateEdgeFrame::LaneParameters::updateNumLanes(int numLanes) {
    // First remove extra lanes
    while (myLaneIndex->getNumItems() > numLanes) {
        myLaneIndex->removeItem(myLaneIndex->getNumItems() - 1);
    }
    // now add lanes
    while (myLaneIndex->getNumItems() < numLanes) {
        myLaneIndex->insertItem(myLaneIndex->getNumItems(), toString(myLaneIndex->getNumItems()).c_str());
    }
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
GNECreateEdgeFrame::LaneParameters::fillDefaultParameters(int laneIndex) {
    // set speed
    mySpeed->setText("13.89");
    // set allow
    myAllow->setText("all");
    // set disallow
    myDisallow->setText("");
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeTypeFile - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeTypeFile::EdgeTypeFile(GNECreateEdgeFrame* createEdgeFrame) :
    FXGroupBox(createEdgeFrame->myContentFrame, "EdgeType File", GUIDesignGroupBoxFrame),
    myCreateEdgeFrame(createEdgeFrame) {
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create create tlDef button
    myLoadEdgeProgramButton = new FXButton(buttonsFrame, "Load\t\tLoad EdgeType from additional file", GUIIconSubSys::getIcon(GUIIcon::OPEN_CONFIG), this, MID_GNE_TLSFRAME_LOAD_PROGRAM, GUIDesignButton);
    // create create tlDef button
    mySaveEdgeProgramButton = new FXButton(buttonsFrame, "Save\t\tSave EdgeType to additional file", GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GNE_TLSFRAME_SAVE_PROGRAM, GUIDesignButton);
    // show EdgeTypeFile
    show();
}


GNECreateEdgeFrame::EdgeTypeFile::~EdgeTypeFile() {}


long
GNECreateEdgeFrame::EdgeTypeFile::onCmdLoadEdgeProgram(FXObject*, FXSelector, void*) {
    // open dialog
    FXFileDialog opendialog(this, "Load type file");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // declare number of edge types
        const int numEdgeTypes = myCreateEdgeFrame->getViewNet()->getNet()->getNetBuilder()->getTypeCont().size();
        // declare type handler
        NIXMLTypesHandler* handler = new NIXMLTypesHandler(myCreateEdgeFrame->getViewNet()->getNet()->getNetBuilder()->getTypeCont());
        // load edge types
        NITypeLoader::load(handler, {opendialog.getFilename().text()}, "types");
        // write information
        WRITE_MESSAGE("Loaded " + toString(myCreateEdgeFrame->getViewNet()->getNet()->getNetBuilder()->getTypeCont().size() - numEdgeTypes) + " edge types");
        // refresh template selector
        myCreateEdgeFrame->myEdgeSelector->refreshEdgeSelector();
    }
    return 0;
}


long
GNECreateEdgeFrame::EdgeTypeFile::onCmdSaveEdgeProgram(FXObject*, FXSelector, void*) {
    // open dialog
    FXString file = MFXUtils::getFilename2Write(this,
        "Save edge types Program as", ".xml",
        GUIIconSubSys::getIcon(GUIIcon::MODETLS),
        gCurrentFolder);
    if (file == "") {
        return 1;
    }
    // open device
    OutputDevice& device = OutputDevice::getDevice(file.text());
    // open tag
    device.openTag(SUMO_TAG_TYPE);
    // write edge types
    myCreateEdgeFrame->getViewNet()->getNet()->getNetBuilder()->getTypeCont().writeEdgeTypes(device);
    // close tag
    device.closeTag();
    // close device
    device.close();
    return 1;
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
    // create custom edge selector
    myEdgeSelector = new EdgeSelector(this);
    // create edge parameters
    myEdgeParameters = new EdgeParameters(this);
    // create lane parameters
    myLaneParameters = new LaneParameters(this);
    // create edge type file
    myEdgeTypeFile = new EdgeTypeFile(this);
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
            GNEEdge* newEdge = myViewNet->getNet()->createEdge(myCreateEdgeSource, junction, nullptr, myViewNet->getUndoList());
            // check if edge was sucesfully created
            if (newEdge) {
                // set parameters
                if (!myEdgeSelector->useEdgeTemplate()) {
                    myEdgeParameters->setAttributes(newEdge, myViewNet->getUndoList());
                }
                // create another edge, if create opposite edge is enabled
                if (oppositeEdge) {
                    GNEEdge* newOppositeEdge = myViewNet->getNet()->createEdge(junction, myCreateEdgeSource, nullptr,
                        myViewNet->getUndoList(), "-" + newEdge->getNBEdge()->getID());
                    // set parameters
                    if (!myEdgeSelector->useEdgeTemplate()) {
                        myEdgeParameters->setAttributes(newOppositeEdge, myViewNet->getUndoList());
                    }
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
    // refresh template selector
    myEdgeSelector->refreshEdgeSelector();
    // show frame
    GNEFrame::show();
}


void
GNECreateEdgeFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


GNECreateEdgeFrame::EdgeSelector* 
GNECreateEdgeFrame::getEdgeSelector() const {
    return myEdgeSelector;
}

/****************************************************************************/
