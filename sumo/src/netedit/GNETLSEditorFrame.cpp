/****************************************************************************/
/// @file    GNETLSEditorFrame.cpp
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying traffic lights
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/common/TplConvert.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <netbuild/NBTrafficLightDefinition.h>
#include <netbuild/NBLoadedSUMOTLDef.h>

#include "GNETLSEditorFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEUndoList.h"
#include "GNEInternalLane.h"
#include "GNEChange_TLS.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNETLSEditorFrame) GNETLSEditorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,    MID_CANCEL,                 GNETLSEditorFrame::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,    MID_OK,                     GNETLSEditorFrame::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_CREATE,         GNETLSEditorFrame::onCmdDefCreate),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_DELETE,         GNETLSEditorFrame::onCmdDefDelete),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_SWITCH,         GNETLSEditorFrame::onCmdDefSwitch),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_OFFSET,         GNETLSEditorFrame::onCmdDefOffset),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_RENAME,         GNETLSEditorFrame::onCmdDefRename),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_SUBRENAME,      GNETLSEditorFrame::onCmdDefSubRename),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_ADDOFF,         GNETLSEditorFrame::onCmdDefAddOff),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_GUESS_PROGRAM,      GNETLSEditorFrame::onCmdGuess),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_PHASE_CREATE,       GNETLSEditorFrame::onCmdPhaseCreate),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_PHASE_DELETE,       GNETLSEditorFrame::onCmdPhaseDelete),
    FXMAPFUNC(SEL_SELECTED,   MID_GNE_PHASE_TABLE,        GNETLSEditorFrame::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_DESELECTED, MID_GNE_PHASE_TABLE,        GNETLSEditorFrame::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_CHANGED,    MID_GNE_PHASE_TABLE,        GNETLSEditorFrame::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_REPLACED,   MID_GNE_PHASE_TABLE,        GNETLSEditorFrame::onCmdPhaseEdit),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_CREATE,         GNETLSEditorFrame::onUpdDefCreate),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_DELETE,         GNETLSEditorFrame::onUpdDefSwitch),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_SWITCH,         GNETLSEditorFrame::onUpdDefSwitch),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_OFFSET,         GNETLSEditorFrame::onUpdNeedsDef),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_PHASE_CREATE,       GNETLSEditorFrame::onUpdNeedsDef),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_PHASE_DELETE,       GNETLSEditorFrame::onUpdNeedsDefAndPhase),
    FXMAPFUNC(SEL_UPDATE,     MID_CANCEL,                 GNETLSEditorFrame::onUpdModified),
    FXMAPFUNC(SEL_UPDATE,     MID_OK,                     GNETLSEditorFrame::onUpdModified),
};


// Object implementation
FXIMPLEMENT(GNETLSEditorFrame, FXVerticalFrame, GNETLSEditorFrameMap, ARRAYNUMBER(GNETLSEditorFrameMap))

// ===========================================================================
// method definitions
// ===========================================================================
GNETLSEditorFrame::GNETLSEditorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Edit Traffic Light"),
    myTableFont(new FXFont(getApp(), "Courier New", 9)),
    myCurrentJunction(0),
    myHaveModifications(false),
    myEditedDef(0) {
    // create groupbox for description
    myGroupBoxJunction = new FXGroupBox(myContentFrame, "Junction", GUIDesignGroupBoxFrame);

    // create description label
    myDescription = new FXLabel(myGroupBoxJunction, "No Junction Selected\n", 0, GUIDesignLabelLeft);

    // create groupbox for tl df
    myGroupBoxTLSDef = new FXGroupBox(myContentFrame, "Traffic lights definition", GUIDesignGroupBoxFrame);

    // create create tlDef button
    myNewTLProgram = new FXButton(myGroupBoxTLSDef, "Create TLS\t\tCreate a new traffic light program", 0, this, MID_GNE_DEF_CREATE, GUIDesignButton);

    // create delete tlDef button
    myDeleteTLProgram = new FXButton(myGroupBoxTLSDef, "Delete TLS\t\tDelete a traffic light program. If all programs are deleted the junction turns into a priority junction.", 0, this, MID_GNE_DEF_DELETE, GUIDesignButton);

    // create groupbox for phases
    myGroupBoxAttributes = new FXGroupBox(myContentFrame, "Attributes", GUIDesignGroupBoxFrame);

    // Create matrix
    myAttributeMatrix = new FXMatrix(myGroupBoxAttributes, 2, GUIDesignMatrixAttributes);

    // create label for name
    myNameLabel = new FXLabel(myAttributeMatrix, "Name", 0, GUIDesignLabelAttribute);

    // create text field for name
    myNameTextField = new FXTextField(myAttributeMatrix, GUIDesignTextFieldNCol, this, MID_GNE_DEF_SWITCH, GUIDesignTextFieldAttributeStr);

    // create label for program
    myProgramLabel = new FXLabel(myAttributeMatrix, "Program", 0, GUIDesignLabelAttribute);

    // create combo box for program
    myProgramComboBox = new FXComboBox(myAttributeMatrix, GUIDesignComboBoxNCol, this, MID_GNE_DEF_SWITCH, GUIDesignComboBoxAttribute);

    // create offset control
    myOffsetLabel = new FXLabel(myAttributeMatrix, "Offset", 0, GUIDesignLabelAttribute);
    myOffset = new FXTextField(myAttributeMatrix, GUIDesignTextFieldNCol, this, MID_GNE_DEF_OFFSET, GUIDesignTextFieldAttributeReal);

    // create groupbox for phases
    myGroupBoxPhases = new FXGroupBox(myContentFrame, "Phases", GUIDesignGroupBoxFrame);

    // create and configure phase table
    myPhaseTable = new FXTable(myGroupBoxPhases, this, MID_GNE_PHASE_TABLE, GUIDesignTableLimitedHeight);
    myPhaseTable->setColumnHeaderMode(LAYOUT_FIX_HEIGHT);
    myPhaseTable->setColumnHeaderHeight(0);
    myPhaseTable->setRowHeaderMode(LAYOUT_FIX_WIDTH);
    myPhaseTable->setRowHeaderWidth(0);
    myPhaseTable->hide();
    myPhaseTable->setFont(myTableFont);
    myPhaseTable->setHelpText("phase duration in seconds | phase state");

    // create total duration info label
    myCycleDuration = new FXLabel(myGroupBoxPhases, "", 0, GUIDesignLabelLeft);

    // create new phase button
    myInsertDuplicateButton = new FXButton(myGroupBoxPhases, "Copy Phase\t\tInsert duplicate phase after selected phase", 0, this, MID_GNE_PHASE_CREATE, GUIDesignButton);

    // create delete phase button
    myDeleteSelectedPhaseButton = new FXButton(myGroupBoxPhases, "Delete Phase\t\tDelete selected phase", 0, this, MID_GNE_PHASE_DELETE, GUIDesignButton);

    // create groupbox for modifications
    myGroupBoxModifications = new FXGroupBox(myContentFrame, "Modifications", GUIDesignGroupBoxFrame);

    // create discard modifications buttons
    myDiscardModificationsButtons = new FXButton(myGroupBoxModifications, "Cancel\t\tDiscard program modifications (Esc)", 0, this, MID_CANCEL, GUIDesignButton);

    // create save modifications button
    mySaveModificationsButtons = new FXButton(myGroupBoxModifications, "Save\t\tSave program modifications (Enter)", 0, this, MID_OK, GUIDesignButton);


    // "Add 'off' program"
    /*
    new FXButton(myContentFrame, "Add \"Off\"-Program\t\tAdds a program for switching off this traffic light",
            0, this, MID_GNE_DEF_ADDOFF, GUIDesignButton);
    */
}


GNETLSEditorFrame::~GNETLSEditorFrame() {
    delete myTableFont;
    cleanup();
}


void
GNETLSEditorFrame::editJunction(GNEJunction* junction) {
    if (myCurrentJunction == 0 || (!myHaveModifications && (junction != myCurrentJunction))) {
        onCmdCancel(0, 0, 0);
        myViewNet->getUndoList()->p_begin("modifying traffic light definition");
        myCurrentJunction = junction;
        myCurrentJunction->selectTLS(true);
        initDefinitions();
    } else {
        myViewNet->setStatusBarText("Unsaved modifications. Abort or Save");
    }
}


long
GNETLSEditorFrame::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myCurrentJunction != 0) {
        myViewNet->getUndoList()->p_abort();
        cleanup();
        myViewNet->update();
    }
    return 1;
}


long
GNETLSEditorFrame::onCmdOK(FXObject*, FXSelector, void*) {
    if (myCurrentJunction != 0) {
        if (myHaveModifications) {
            NBTrafficLightDefinition* old = myDefinitions[myProgramComboBox->getCurrentItem()];
            std::vector<NBNode*> nodes = old->getNodes();
            for (std::vector<NBNode*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                GNEJunction* junction = myViewNet->getNet()->retrieveJunction((*it)->getID());
                myViewNet->getUndoList()->add(new GNEChange_TLS(junction, old, false), true);
                myViewNet->getUndoList()->add(new GNEChange_TLS(junction, myEditedDef, true), true);
            }
            myEditedDef = 0;
            myViewNet->getUndoList()->p_end();
            cleanup();
            myViewNet->update();
        } else {
            onCmdCancel(0, 0, 0);
        }
    }
    return 1;
}


long
GNETLSEditorFrame::onCmdDefCreate(FXObject*, FXSelector, void*) {
    GNEJunction* junction = myCurrentJunction;
    onCmdCancel(0, 0, 0); // abort because we onCmdOk assumes we wish to save an edited definition
    if (junction->getAttribute(SUMO_ATTR_TYPE) != toString(NODETYPE_TRAFFIC_LIGHT)) {
        junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_TRAFFIC_LIGHT), myViewNet->getUndoList());
    } else {
        myViewNet->getUndoList()->add(new GNEChange_TLS(junction, 0, true, true), true);
    }
    editJunction(junction);
    return 1;
}


long
GNETLSEditorFrame::onCmdDefDelete(FXObject*, FXSelector, void*) {
    GNEJunction* junction = myCurrentJunction;
    const bool changeType = myDefinitions.size() == 1;
    onCmdCancel(0, 0, 0); // abort because onCmdOk assumes we wish to save an edited definition
    if (changeType) {
        junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), myViewNet->getUndoList());
    } else {
        NBTrafficLightDefinition* tlDef = myDefinitions[myProgramComboBox->getCurrentItem()];
        myViewNet->getUndoList()->add(new GNEChange_TLS(junction, tlDef, false), true);
    }
    return 1;
}


long
GNETLSEditorFrame::onCmdDefSwitch(FXObject*, FXSelector, void*) {
    assert(myCurrentJunction != 0);
    assert((int)myDefinitions.size() == myProgramComboBox->getNumItems());
    NBTrafficLightDefinition* tlDef = myDefinitions[myProgramComboBox->getCurrentItem()];
    // logic may not have been recomputed yet. recompute to be sure
    NBTrafficLightLogicCont& tllCont = myViewNet->getNet()->getTLLogicCont();
    myViewNet->getNet()->computeJunction(myCurrentJunction);
    NBTrafficLightLogic* tllogic = tllCont.getLogic(tlDef->getID(), tlDef->getProgramID());
    if (tllogic != 0) {
        // now we can be sure that the tlDef is up to date (i.e. re-guessed)
        buildIinternalLanes(tlDef);
        // create working copy from original def
        delete myEditedDef;
        myEditedDef = new NBLoadedSUMOTLDef(tlDef, tllogic);
        myOffset->setText(toString(STEPS2TIME(myEditedDef->getLogic()->getOffset())).c_str());
        initPhaseTable();
        updateCycleDuration();
    } else {
        // tlDef has no valid logic (probably because id does not control any links
        onCmdCancel(0, 0, 0);
        myViewNet->setStatusBarText("Traffic light does not control any links");
    }
    return 1;
}


long
GNETLSEditorFrame::onUpdDefSwitch(FXObject* o, FXSelector, void*) {
    const bool enable = myDefinitions.size() > 0 && !myHaveModifications;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditorFrame::onUpdNeedsDef(FXObject* o, FXSelector, void*) {
    const bool enable = myDefinitions.size() > 0;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditorFrame::onUpdNeedsDefAndPhase(FXObject* o, FXSelector, void*) {
    // do not delete the last phase
    const bool enable = myDefinitions.size() > 0 && myPhaseTable->getNumRows() > 1;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditorFrame::onUpdDefCreate(FXObject* o, FXSelector, void*) {
    const bool enable = myCurrentJunction != 0 && !myHaveModifications;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditorFrame::onUpdModified(FXObject* o, FXSelector, void*) {
    bool enable = myHaveModifications;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}



long
GNETLSEditorFrame::onCmdDefOffset(FXObject*, FXSelector, void*) {
    myHaveModifications = true;
    myEditedDef->setOffset(getSUMOTime(myOffset->getText()));
    return 1;
}


long
GNETLSEditorFrame::onCmdDefRename(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditorFrame::onCmdDefSubRename(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditorFrame::onCmdDefAddOff(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditorFrame::onCmdGuess(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditorFrame::onCmdPhaseSwitch(FXObject*, FXSelector, void*) {
    const int index = myPhaseTable->getCurrentRow();
    const NBTrafficLightLogic::PhaseDefinition& phase = getPhases()[index];
    myPhaseTable->selectRow(index);
    // need not hold since links could have been deleted somewhere else and indices may be reused
    // assert(phase.state.size() == myInternalLanes.size());
    for (TLIndexMap::iterator it = myInternalLanes.begin(); it != myInternalLanes.end(); it++) {
        int tlIndex = it->first;
        std::vector<GNEInternalLane*> lanes = it->second;
        assert(tlIndex >= 0);
        assert(tlIndex < (int)phase.state.size());
        for (std::vector<GNEInternalLane*>::iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
            (*it_lane)->setLinkState((LinkState)phase.state[tlIndex]);
        }
    }
    myViewNet->update();
    return 1;
}


long
GNETLSEditorFrame::onCmdPhaseCreate(FXObject*, FXSelector, void*) {
    myHaveModifications = true;
    // allows insertion at first position by deselecting via arrow keys
    int newIndex = myPhaseTable->getSelStartRow() + 1;
    int oldIndex = MAX2(0, myPhaseTable->getSelStartRow());
    // copy current row
    const SUMOTime duration = getSUMOTime(myPhaseTable->getItemText(oldIndex, 0));
    const std::string state = myPhaseTable->getItemText(oldIndex, 1).text();
    myEditedDef->getLogic()->addStep(duration, state, newIndex);
    myPhaseTable->setCurrentItem(newIndex, 0);
    initPhaseTable(newIndex);
    myPhaseTable->setFocus();
    return 1;
}


long
GNETLSEditorFrame::onCmdPhaseDelete(FXObject*, FXSelector, void*) {
    myHaveModifications = true;
    const int newRow = MAX2((int)0, (int)myPhaseTable->getCurrentRow() - 1);
    myEditedDef->getLogic()->deletePhase(myPhaseTable->getCurrentRow());
    initPhaseTable(newRow);
    myPhaseTable->setFocus();
    return 1;
}


long
GNETLSEditorFrame::onCmdPhaseEdit(FXObject*, FXSelector, void* ptr) {
    /* @note: there is a bug when copying/pasting rows: when this handler is
     * called the value of the cell is not yet updated. This means you have to
     * click inside the cell and hit enter to actually update the value */
    FXTablePos* tp = (FXTablePos*)ptr;
    FXString value = myPhaseTable->getItemText(tp->row, tp->col);
    if (tp->col == 0) {
        // duration edited
        if (GNEAttributeCarrier::canParse<SUMOReal>(value.text())) {
            SUMOTime duration = getSUMOTime(value);
            if (duration > 0) {
                myEditedDef->getLogic()->setPhaseDuration(tp->row, duration);
                myHaveModifications = true;
                updateCycleDuration();
                return 1;
            }
        }
        // input error, reset value
        myPhaseTable->setItemText(tp->row, 0, toString(STEPS2TIME(getPhases()[tp->row].duration)).c_str());
    } else {
        // state edited
        try {
            // insert phase with new step and delete the old phase
            myEditedDef->getLogic()->addStep(getPhases()[tp->row].duration, value.text(), tp->row);
            myEditedDef->getLogic()->deletePhase(tp->row + 1);
            myHaveModifications = true;
            onCmdPhaseSwitch(0, 0, 0);
        } catch (ProcessError) {
            // input error, reset value
            myPhaseTable->setItemText(tp->row, 1, getPhases()[tp->row].state.c_str());
        }
    }
    return 1;
}


void
GNETLSEditorFrame::updateDescription() const {
    std::string description;
    if (myCurrentJunction == 0) {
        description = "No Junction Selected\n";
    } else {
        NBNode* nbn = myCurrentJunction->getNBNode();
        description = "Current junction: " + nbn->getID() + "\n(";
        if (!nbn->isTLControlled()) {
            description += "uncontrolled, ";
        }
        description += (myHaveModifications ? "modified)" : "unmodified)");
    }
    myDescription->setText(description.c_str());
}


void
GNETLSEditorFrame::cleanup() {
    if (myCurrentJunction) {
        myCurrentJunction->selectTLS(false);
    }
    // clean data structures
    myCurrentJunction = 0;
    myHaveModifications = false;
    delete myEditedDef;
    myEditedDef = 0;
    buildIinternalLanes(0); // only clears
    // clean up controls
    myNameTextField->setText("");
    myOffset->setText("");
    myDefinitions.clear();
    myProgramComboBox->hide();
    initPhaseTable(); // only clears when there are no definitions
    myCycleDuration->hide();
    updateDescription();
}


void
GNETLSEditorFrame::buildIinternalLanes(NBTrafficLightDefinition* tlDef) {
    // clean up previous objects
    SUMORTree& rtree = myViewNet->getNet()->getVisualisationSpeedUp();
    for (TLIndexMap::iterator it = myInternalLanes.begin(); it != myInternalLanes.end(); it++) {
        std::vector<GNEInternalLane*> lanes = it->second;
        for (std::vector<GNEInternalLane*>::iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
            rtree.removeAdditionalGLObject(*it_lane);
            delete *it_lane;
        }
    }
    myInternalLanes.clear();
    if (tlDef != 0) {
        const int NUM_POINTS = 10;
        assert(myCurrentJunction);
        SUMORTree& rtree = myViewNet->getNet()->getVisualisationSpeedUp();
        NBNode* nbn = myCurrentJunction->getNBNode();
        std::string innerID = ":" + nbn->getID(); // see NWWriter_SUMO::writeInternalEdges
        const NBConnectionVector& links = tlDef->getControlledLinks();
        for (NBConnectionVector::const_iterator it = links.begin(); it != links.end(); it++) {
            int tlIndex = it->getTLIndex();
            PositionVector shape = nbn->computeInternalLaneShape(it->getFrom(), NBEdge::Connection(it->getFromLane(),
                                   it->getTo(), it->getToLane()), NUM_POINTS);
            GNEInternalLane* ilane = new GNEInternalLane(this, innerID + '_' + toString(tlIndex),  shape, tlIndex);
            rtree.addAdditionalGLObject(ilane);
            myInternalLanes[tlIndex].push_back(ilane);
        }
        const std::vector<NBNode::Crossing>& crossings = nbn->getCrossings();
        for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
            const NBNode::Crossing& c = *it;
            GNEInternalLane* ilane = new GNEInternalLane(this, c.id, c.shape, c.tlLinkNo);
            rtree.addAdditionalGLObject(ilane);
            myInternalLanes[c.tlLinkNo].push_back(ilane);
        }
    }
}


void
GNETLSEditorFrame::initDefinitions() {
    myDefinitions.clear();
    myNameTextField->setText("");
    myProgramComboBox->clearItems();
    assert(myCurrentJunction);
    NBNode* nbn = myCurrentJunction->getNBNode();
    std::set<NBTrafficLightDefinition*> tldefs = nbn->getControllingTLS();
    for (std::set<NBTrafficLightDefinition*>::iterator it = tldefs.begin(); it != tldefs.end(); it++) {
        myDefinitions.push_back(*it);
        myNameTextField->setText((*it)->getID().c_str());
        myProgramComboBox->appendItem((*it)->getProgramID().c_str());
    }
    if (myDefinitions.size() > 0) {
        myProgramComboBox->setCurrentItem(0);
        myProgramComboBox->setNumVisible(myProgramComboBox->getNumItems());
        myProgramComboBox->show();
        onCmdDefSwitch(0, 0, 0);
    }
    updateDescription();
}


void
GNETLSEditorFrame::initPhaseTable(int index) {
    myPhaseTable->setVisibleRows(1);
    myPhaseTable->setVisibleColumns(2);
    myPhaseTable->hide();
    if (myDefinitions.size() > 0) {
        const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = getPhases();
        myPhaseTable->setTableSize((int)phases.size(), 2);
        myPhaseTable->setVisibleRows((int)phases.size());
        myPhaseTable->setVisibleColumns(2);
        for (int row = 0; row < (int)phases.size(); row++) {
            myPhaseTable->setItemText(row, 0, toString(STEPS2TIME(phases[row].duration)).c_str());
            myPhaseTable->setItemText(row, 1, phases[row].state.c_str());
            myPhaseTable->getItem(row, 1)->setJustify(FXTableItem::LEFT);
        }
        myPhaseTable->fitColumnsToContents(0, 2);
        myPhaseTable->setHeight((int)phases.size() * 21); // experimental
        myPhaseTable->setCurrentItem(index, 0);
        myPhaseTable->selectRow(index, true);
        myPhaseTable->show();
        myPhaseTable->setFocus();
    }
    update();
}


const std::vector<NBTrafficLightLogic::PhaseDefinition>&
GNETLSEditorFrame::getPhases() {
    return myEditedDef->getLogic()->getPhases();
}


void
GNETLSEditorFrame::handleChange(GNEInternalLane* lane) {
    myHaveModifications = true;
    if (myViewNet->changeAllPhases()) {
        const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = getPhases();
        for (int row = 0; row < (int)phases.size(); row++) {
            myEditedDef->getLogic()->setPhaseState(row, lane->getTLIndex(), lane->getLinkState());
        }
    } else {
        myEditedDef->getLogic()->setPhaseState(myPhaseTable->getCurrentRow(), lane->getTLIndex(), lane->getLinkState());
    }
    initPhaseTable(myPhaseTable->getCurrentRow());
    myPhaseTable->setFocus();
}


void
GNETLSEditorFrame::handleMultiChange(GNELane* lane, FXObject* obj, FXSelector sel, void* data) {
    if (myEditedDef != 0) {
        myHaveModifications = true;
        const NBConnectionVector& links = myEditedDef->getControlledLinks();
        std::set<std::string> fromIDs;
        fromIDs.insert(lane->getMicrosimID());
        GNEEdge& edge = lane->getParentEdge();
        // if neither the lane nor its edge are selected, apply changes to the whole edge
        if (!gSelected.isSelected(GLO_EDGE, edge.getGlID()) && !gSelected.isSelected(GLO_LANE, lane->getGlID())) {
            for (GNEEdge::LaneVector::const_iterator it_lane = edge.getLanes().begin(); it_lane != edge.getLanes().end(); it_lane++) {
                fromIDs.insert((*it_lane)->getMicrosimID());
            }
        } else {
            // if the edge is selected, apply changes to all lanes of all selected edges
            if (gSelected.isSelected(GLO_EDGE, edge.getGlID())) {
                std::vector<GNEEdge*> edges = myViewNet->getNet()->retrieveEdges(true);
                for (std::vector<GNEEdge*>::iterator it = edges.begin(); it != edges.end(); it++) {
                    for (GNEEdge::LaneVector::const_iterator it_lane = (*it)->getLanes().begin(); it_lane != (*it)->getLanes().end(); it_lane++) {
                        fromIDs.insert((*it_lane)->getMicrosimID());
                    }
                }
            }
            // if the lane is selected, apply changes to all selected lanes
            if (gSelected.isSelected(GLO_LANE, lane->getGlID())) {
                std::vector<GNELane*> lanes = myViewNet->getNet()->retrieveLanes(true);
                for (std::vector<GNELane*>::iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
                    fromIDs.insert((*it_lane)->getMicrosimID());
                }
            }

        }
        // set new state for all connections from the chosen lane IDs
        for (NBConnectionVector::const_iterator it = links.begin(); it != links.end(); it++) {
            const NBConnection& c = *it;
            if (fromIDs.count(c.getFrom()->getLaneID(c.getFromLane())) > 0) {
                std::vector<GNEInternalLane*> lanes = myInternalLanes[c.getTLIndex()];
                for (std::vector<GNEInternalLane*>::iterator it_lane = lanes.begin(); it_lane != lanes.end(); it_lane++) {
                    (*it_lane)->onDefault(obj, sel, data);
                }
            }
        }
    }
}


bool
GNETLSEditorFrame::controlsEdge(GNEEdge& edge) const {
    if (myEditedDef != 0) {
        const NBConnectionVector& links = myEditedDef->getControlledLinks();
        for (NBConnectionVector::const_iterator it = links.begin(); it != links.end(); it++) {
            if ((*it).getFrom()->getID() == edge.getMicrosimID()) {
                return true;
            }
        }
    }
    return false;
}


SUMOTime
GNETLSEditorFrame::getSUMOTime(const FXString& string) {
    assert(GNEAttributeCarrier::canParse<SUMOReal>(string.text()));
    return TIME2STEPS(GNEAttributeCarrier::parse<SUMOReal>(string.text()));
}


void
GNETLSEditorFrame::updateCycleDuration() {
    SUMOTime cycleDuration = 0;
    for (std::vector<NBTrafficLightLogic::PhaseDefinition>::const_iterator it = getPhases().begin(); it != getPhases().end(); it++) {
        cycleDuration += it->duration;
    }
    std::string text = "Cycle time: " + toString(STEPS2TIME(cycleDuration));
    myCycleDuration->setText(text.c_str());
}


/****************************************************************************/
