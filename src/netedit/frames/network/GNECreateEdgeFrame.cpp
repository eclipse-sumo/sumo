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

FXDEFMAP(GNECreateEdgeFrame::TemplateSelector) TemplateSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON,  GNECreateEdgeFrame::TemplateSelector::onCmdRadioButton),
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
FXIMPLEMENT(GNECreateEdgeFrame::TemplateSelector,       FXGroupBox,     TemplateSelectorMap,    ARRAYNUMBER(TemplateSelectorMap))
FXIMPLEMENT(GNECreateEdgeFrame::EdgeParameters,         FXGroupBox,     EdgeParametersMap,      ARRAYNUMBER(EdgeParametersMap))
FXIMPLEMENT(GNECreateEdgeFrame::LaneParameters,         FXGroupBox,     LaneParametersMap,      ARRAYNUMBER(LaneParametersMap))
FXIMPLEMENT(GNECreateEdgeFrame::EdgeTypeFile,           FXGroupBox,     EdgeTypeFileMap,        ARRAYNUMBER(EdgeTypeFileMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::TemplateSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::TemplateSelector::TemplateSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Template selector", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // default edge radio button
    myUseTemplateRadioButton = new FXRadioButton(this, "Use edge template",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // custom edge radio button
    myCustomEdgeRadioButton = new FXRadioButton(this, "Create custom edge",
        this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // by default, create custom edge
    myCustomEdgeRadioButton->setCheck(TRUE);
}


GNECreateEdgeFrame::TemplateSelector::~TemplateSelector() {}


void
GNECreateEdgeFrame::TemplateSelector::refreshTemplateSelector() {
    // get template editor
    GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // check if there is template
    if (templateEditor->hasTemplate()) {
        // enable both buttons
        myUseTemplateRadioButton->enable();
        myCustomEdgeRadioButton->enable();
        // change text in myUseTemplateRadioButton
        myUseTemplateRadioButton->setText(("Use '" + templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_ID) + "' template").c_str());
    } else {
        // disable use template
        myUseTemplateRadioButton->disable();
        myCustomEdgeRadioButton->enable();
        // enable custom edge radio button
        myCustomEdgeRadioButton->setCheck(TRUE, FALSE);
        // change text in myUseTemplateRadioButton
        myUseTemplateRadioButton->setText("Use edge template");
    }
    // show editor parameter
    if (myCustomEdgeRadioButton->getCheck() == TRUE) {
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
GNECreateEdgeFrame::TemplateSelector::useEdgeTemplate() const {
    if (myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->hasTemplate()) {
        if (myUseTemplateRadioButton->getCheck() == TRUE) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


long
GNECreateEdgeFrame::TemplateSelector::onCmdRadioButton(FXObject* obj, FXSelector, void*) {
    // check what object was pressed
    if (obj == myUseTemplateRadioButton) {
        // update buttons
        myUseTemplateRadioButton->setCheck(TRUE, FALSE);
        myCustomEdgeRadioButton->setCheck(FALSE, FALSE);
    } else {
        // update buttons
        myUseTemplateRadioButton->setCheck(FALSE, FALSE);
        myCustomEdgeRadioButton->setCheck(TRUE, FALSE);
    }
    // refresh template selector
    refreshTemplateSelector();
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

GNECreateEdgeFrame::EdgeTypeFile::EdgeTypeFile(GNECreateEdgeFrame* TLSEditorParent) :
    FXGroupBox(TLSEditorParent->myContentFrame, "EdgeType File", GUIDesignGroupBoxFrame),
    myTLSEditorParent(TLSEditorParent) {
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
/*
    FXFileDialog opendialog(this, "Load TLS Program");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::MODETLS));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // run parser
        NBTrafficLightLogicCont tmpTLLCont;
        NIXMLTrafficLightsHandler tllHandler(tmpTLLCont, myTLSEditorParent->myViewNet->getNet()->getEdgeCont(), true);
        tmpTLLCont.insert(myTLSEditorParent->myEditedDef);
        XMLSubSys::runParser(tllHandler, opendialog.getFilename().text());

        NBLoadedSUMOTLDef* newDefSameProgram = nullptr;
        std::set<NBLoadedSUMOTLDef*> newDefsOtherProgram;
        for (auto item : tmpTLLCont.getPrograms(myTLSEditorParent->myEditedDef->getID())) {
            if (item.second != myTLSEditorParent->myEditedDef) {
                NBLoadedSUMOTLDef* sdef = dynamic_cast<NBLoadedSUMOTLDef*>(item.second);
                if (item.first == myTLSEditorParent->myEditedDef->getProgramID()) {
                    newDefSameProgram = sdef;
                } else {
                    newDefsOtherProgram.insert(sdef);
                }
            }
        }
        const int newPrograms = (int)newDefsOtherProgram.size();
        if (newPrograms > 0 || newDefSameProgram != nullptr) {
            std::vector<NBNode*> nodes = myTLSEditorParent->myEditedDef->getNodes();
            for (auto newProg : newDefsOtherProgram) {
                for (auto it_node : nodes) {
                    GNEJunction* junction = myTLSEditorParent->getViewNet()->getNet()->retrieveJunction(it_node->getID());
                    myTLSEditorParent->getViewNet()->getUndoList()->add(new GNEChange_TLS(junction, newProg, true), true);
                }
            }
            if (newPrograms > 0) {
                WRITE_MESSAGE("Loaded " + toString(newPrograms) + " new programs for tlLogic '" + myTLSEditorParent->myEditedDef->getID() + "'");
            }
            if (newDefSameProgram != nullptr) {
                // replace old program when loading the same program ID
                myTLSEditorParent->myEditedDef = newDefSameProgram;
                WRITE_MESSAGE("Updated program '" + newDefSameProgram->getProgramID() +  "' for tlLogic '" + myTLSEditorParent->myEditedDef->getID() + "'");
            }
        } else {
            myTLSEditorParent->getViewNet()->setStatusBarText("No programs found for traffic light '" + myTLSEditorParent->myEditedDef->getID() + "'");
        }

        // clean up temporary container to avoid deletion of defs when it's destruct is called
        for (NBTrafficLightDefinition* def : tmpTLLCont.getDefinitions()) {
            tmpTLLCont.removeProgram(def->getID(), def->getProgramID(), false);
        }

        myTLSEditorParent->myTLSPhases->initPhaseTable();
        myTLSEditorParent->myTLSModifications->setHaveModifications(true);
    }
*/
    return 0;
}


long
GNECreateEdgeFrame::EdgeTypeFile::onCmdSaveEdgeProgram(FXObject*, FXSelector, void*) {
/*
    FXString file = MFXUtils::getFilename2Write(this,
        "Save TLS Program as", ".xml",
        GUIIconSubSys::getIcon(GUIIcon::MODETLS),
        gCurrentFolder);
    if (file == "") {
        return 1;
    }
    OutputDevice& device = OutputDevice::getDevice(file.text());

    // save program
    device.writeXMLHeader("additional", "additional_file.xsd");
    device.openTag(SUMO_TAG_TLLOGIC);
    device.writeAttr(SUMO_ATTR_ID, myTLSEditorParent->myEditedDef->getLogic()->getID());
    device.writeAttr(SUMO_ATTR_TYPE, myTLSEditorParent->myEditedDef->getLogic()->getType());
    device.writeAttr(SUMO_ATTR_PROGRAMID, myTLSEditorParent->myEditedDef->getLogic()->getProgramID());
    device.writeAttr(SUMO_ATTR_OFFSET, writeSUMOTime(myTLSEditorParent->myEditedDef->getLogic()->getOffset()));
    // write the phases
    const bool varPhaseLength = myTLSEditorParent->myEditedDef->getLogic()->getType() != TrafficLightType::STATIC;
    const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = myTLSEditorParent->myEditedDef->getLogic()->getPhases();
    for (auto j : phases) {
        device.openTag(SUMO_TAG_PHASE);
        device.writeAttr(SUMO_ATTR_DURATION, writeSUMOTime(j.duration));
        device.writeAttr(SUMO_ATTR_STATE, j.state);
        if (varPhaseLength) {
            if (j.minDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                device.writeAttr(SUMO_ATTR_MINDURATION, writeSUMOTime(j.minDur));
            }
            if (j.maxDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                device.writeAttr(SUMO_ATTR_MAXDURATION, writeSUMOTime(j.maxDur));
            }
        }
        device.closeTag();
    }
    device.close();
*/
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
    myTemplateSelector = new TemplateSelector(this);
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
                if (!myTemplateSelector->useEdgeTemplate()) {
                    myEdgeParameters->setAttributes(newEdge, myViewNet->getUndoList());
                }
                // create another edge, if create opposite edge is enabled
                if (oppositeEdge) {
                    GNEEdge* newOppositeEdge = myViewNet->getNet()->createEdge(junction, myCreateEdgeSource, nullptr,
                        myViewNet->getUndoList(), "-" + newEdge->getNBEdge()->getID());
                    // set parameters
                    if (!myTemplateSelector->useEdgeTemplate()) {
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
    myTemplateSelector->refreshTemplateSelector();
    // show frame
    GNEFrame::show();
}


void
GNECreateEdgeFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


GNECreateEdgeFrame::TemplateSelector* 
GNECreateEdgeFrame::getTemplateSelector() const {
    return myTemplateSelector;
}

/****************************************************************************/
