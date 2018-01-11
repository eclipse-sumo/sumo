/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETLSEditorFrame.cpp
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying traffic lights
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

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNETLSEditorFrame) GNETLSEditorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,    MID_CANCEL,                       GNETLSEditorFrame::onCmdCancel),
    FXMAPFUNC(SEL_UPDATE,     MID_CANCEL,                       GNETLSEditorFrame::onUpdModified),
    FXMAPFUNC(SEL_COMMAND,    MID_OK,                           GNETLSEditorFrame::onCmdOK),
    FXMAPFUNC(SEL_UPDATE,     MID_OK,                           GNETLSEditorFrame::onUpdModified),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_CREATE,          GNETLSEditorFrame::onCmdDefCreate),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_DELETE,          GNETLSEditorFrame::onCmdDefDelete),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_SWITCH,          GNETLSEditorFrame::onCmdDefSwitch),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_OFFSET,          GNETLSEditorFrame::onCmdDefOffset),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_RENAME,          GNETLSEditorFrame::onCmdDefRename),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_SUBRENAME,       GNETLSEditorFrame::onCmdDefSubRename),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_ADDOFF,          GNETLSEditorFrame::onCmdDefAddOff),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_GUESSPROGRAM,    GNETLSEditorFrame::onCmdGuess),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_PHASE_CREATE,    GNETLSEditorFrame::onCmdPhaseCreate),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_TLSFRAME_PHASE_CREATE,    GNETLSEditorFrame::onUpdNeedsDef),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_PHASE_DELETE,    GNETLSEditorFrame::onCmdPhaseDelete),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_TLSFRAME_CLEANUP,         GNETLSEditorFrame::onCmdCleanup),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_TLSFRAME_PHASE_DELETE,    GNETLSEditorFrame::onUpdNeedsDefAndPhase),
    FXMAPFUNC(SEL_SELECTED,   MID_GNE_TLSFRAME_PHASE_TABLE,     GNETLSEditorFrame::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_DESELECTED, MID_GNE_TLSFRAME_PHASE_TABLE,     GNETLSEditorFrame::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_CHANGED,    MID_GNE_TLSFRAME_PHASE_TABLE,     GNETLSEditorFrame::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_REPLACED,   MID_GNE_TLSFRAME_PHASE_TABLE,     GNETLSEditorFrame::onCmdPhaseEdit),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_TLSFRAME_CREATE,          GNETLSEditorFrame::onUpdDefCreate),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_TLSFRAME_DELETE,          GNETLSEditorFrame::onUpdDefSwitch),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_TLSFRAME_SWITCH,          GNETLSEditorFrame::onUpdDefSwitch),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_TLSFRAME_OFFSET,          GNETLSEditorFrame::onUpdNeedsDef),



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
    // Create frame for junction ID
    FXHorizontalFrame* junctionIDFrame = new FXHorizontalFrame(myGroupBoxJunction, GUIDesignAuxiliarHorizontalFrame);
    myLabelJunctionID = new FXLabel(junctionIDFrame, "Junction ID", 0, GUIDesignLabelAttribute);
    myTextFieldJunctionID = new FXTextField(junctionIDFrame, GUIDesignTextFieldNCol, this, MID_GNE_TLSFRAME_SELECT_JUNCTION, GUIDesignTextField);
    myTextFieldJunctionID->setEditable(false);
    // create frame for junction status
    FXHorizontalFrame* junctionIDStatus = new FXHorizontalFrame(myGroupBoxJunction, GUIDesignAuxiliarHorizontalFrame);
    myLabelJunctionStatus = new FXLabel(junctionIDStatus, "Status", 0, GUIDesignLabelAttribute);
    myTextFieldJunctionStatus = new FXTextField(junctionIDStatus, GUIDesignTextFieldNCol, this, MID_GNE_TLSFRAME_UPDATE_STATUS, GUIDesignTextField);
    myTextFieldJunctionStatus->setEditable(false);

    // create groupbox for tl df
    myGroupBoxTLSDef = new FXGroupBox(myContentFrame, "Traffic lights definition", GUIDesignGroupBoxFrame);

    // create create tlDef button
    myNewTLProgram = new FXButton(myGroupBoxTLSDef, "Create TLS\t\tCreate a new traffic light program", 0, this, MID_GNE_TLSFRAME_CREATE, GUIDesignButton);

    // create delete tlDef button
    myDeleteTLProgram = new FXButton(myGroupBoxTLSDef, "Delete TLS\t\tDelete a traffic light program. If all programs are deleted the junction turns into a priority junction.", 0, this, MID_GNE_TLSFRAME_DELETE, GUIDesignButton);

    // create TLS attributes
    myTLSAttributes = new TLSAttributes(myContentFrame, this);

    // create groupbox for phases
    myGroupBoxPhases = new FXGroupBox(myContentFrame, "Phases", GUIDesignGroupBoxFrame);

    // create and configure phase table
    myTableScroll = new FXScrollWindow(myGroupBoxPhases, LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT);
    myPhaseTable = new FXTable(myTableScroll, this, MID_GNE_TLSFRAME_PHASE_TABLE, GUIDesignTableLimitedHeight);
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
    myInsertDuplicateButton = new FXButton(myGroupBoxPhases, "Copy Phase\t\tInsert duplicate phase after selected phase", 0, this, MID_GNE_TLSFRAME_PHASE_CREATE, GUIDesignButton);

    // create delete phase button
    myDeleteSelectedPhaseButton = new FXButton(myGroupBoxPhases, "Delete Phase\t\tDelete selected phase", 0, this, MID_GNE_TLSFRAME_PHASE_DELETE, GUIDesignButton);
    new FXButton(myGroupBoxPhases, "Cleanup States\t\tClean unused states from all phase.", 0, this, MID_GNE_TLSFRAME_CLEANUP, GUIDesignButton);

    // create groupbox for modifications
    myGroupBoxModifications = new FXGroupBox(myContentFrame, "Modifications", GUIDesignGroupBoxFrame);

    // create discard modifications buttons
    myDiscardModificationsButtons = new FXButton(myGroupBoxModifications, "Cancel\t\tDiscard program modifications (Esc)", 0, this, MID_CANCEL, GUIDesignButton);

    // create save modifications button
    mySaveModificationsButtons = new FXButton(myGroupBoxModifications, "Save\t\tSave program modifications (Enter)", 0, this, MID_OK, GUIDesignButton);


    // "Add 'off' program"
    /*
    new FXButton(myContentFrame, "Add \"Off\"-Program\t\tAdds a program for switching off this traffic light",
            0, this, MID_GNE_TLSFRAME_ADDOFF, GUIDesignButton);
    */

    updateDescription();
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
        myTLSAttributes->initTLSAttributes(myCurrentJunction);
        updateDescription();
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
            NBTrafficLightDefinition* oldDefinition = myTLSAttributes->getCurrentTLSDefinition();
            std::vector<NBNode*> nodes = oldDefinition->getNodes();
            for (auto it : nodes) {
                GNEJunction* junction = myViewNet->getNet()->retrieveJunction(it->getID());
                myViewNet->getUndoList()->add(new GNEChange_TLS(junction, oldDefinition, false), true);
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
    const bool changeType = myTLSAttributes->getNumberOfTLSDefinitions() == 1;
    onCmdCancel(0, 0, 0); // abort because onCmdOk assumes we wish to save an edited definition
    if (changeType) {
        junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), myViewNet->getUndoList());
    } else {
        NBTrafficLightDefinition* tlDef = myTLSAttributes->getCurrentTLSDefinition();
        myViewNet->getUndoList()->add(new GNEChange_TLS(junction, tlDef, false), true);
    }
    return 1;
}


long
GNETLSEditorFrame::onCmdDefSwitch(FXObject*, FXSelector, void*) {
    assert(myCurrentJunction != 0);
    assert(myTLSAttributes->getNumberOfTLSDefinitions() == myTLSAttributes->getNumberOfPrograms());
    NBTrafficLightDefinition* tlDef = myTLSAttributes->getCurrentTLSDefinition();
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

        myTLSAttributes->setOffset(myEditedDef->getLogic()->getOffset());
        initPhaseTable();
        updateCycleDuration();
        myCycleDuration->show();
    } else {
        // tlDef has no valid logic (probably because id does not control any links
        onCmdCancel(0, 0, 0);
        myViewNet->setStatusBarText("Traffic light does not control any links");
    }
    return 1;
}


long
GNETLSEditorFrame::onUpdDefSwitch(FXObject* o, FXSelector, void*) {
    const bool enable = myTLSAttributes->getNumberOfTLSDefinitions() > 0 && !myHaveModifications;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditorFrame::onUpdNeedsDef(FXObject* o, FXSelector, void*) {
    const bool enable = myTLSAttributes->getNumberOfTLSDefinitions() > 0;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditorFrame::onUpdNeedsDefAndPhase(FXObject* o, FXSelector, void*) {
    // do not delete the last phase
    const bool enable = myTLSAttributes->getNumberOfTLSDefinitions() > 0 && myPhaseTable->getNumRows() > 1;
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
    myEditedDef->setOffset(myTLSAttributes->getOffset());
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
    for (auto it : myInternalLanes) {
        int tlIndex = it.first;
        std::vector<GNEInternalLane*> lanes = it.second;
        assert(tlIndex >= 0);
        assert(tlIndex < (int)phase.state.size());
        for (auto it_lane : lanes) {
            it_lane->setLinkState((LinkState)phase.state[tlIndex]);
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
    const bool fixed = myEditedDef->getType() == TLTYPE_STATIC;
    const SUMOTime duration = getSUMOTime(myPhaseTable->getItemText(oldIndex, 0));
    const std::string state = myPhaseTable->getItemText(oldIndex, fixed ? 1 : 3).text();
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
GNETLSEditorFrame::onCmdCleanup(FXObject*, FXSelector, void*) {
    myHaveModifications = myEditedDef->cleanupStates();
    initPhaseTable(0);
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
    const bool fixed = myEditedDef->getType() == TLTYPE_STATIC;
    if (tp->col == 0) {
        // duration edited
        if (GNEAttributeCarrier::canParse<double>(value.text())) {
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
    } else if (!fixed && tp->col == 1) {
        // minDur edited
        if (GNEAttributeCarrier::canParse<double>(value.text())) {
            SUMOTime minDur = getSUMOTime(value);
            if (minDur > 0) {
                myEditedDef->getLogic()->setPhaseMinDuration(tp->row, minDur);
                myHaveModifications = true;
                return 1;
            }
        } else if (StringUtils::prune(value.text()).empty()) {
            myEditedDef->getLogic()->setPhaseMinDuration(tp->row, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
            myHaveModifications = true;
            return 1;
        }
        // input error, reset value
        myPhaseTable->setItemText(tp->row, 1, varDurString(getPhases()[tp->row].minDur).c_str());
    } else if (!fixed && tp->col == 2) {
        // minDur edited
        if (GNEAttributeCarrier::canParse<double>(value.text())) {
            SUMOTime maxDur = getSUMOTime(value);
            if (maxDur > 0) {
                myEditedDef->getLogic()->setPhaseMaxDuration(tp->row, maxDur);
                myHaveModifications = true;
                return 1;
            }
        } else if (StringUtils::prune(value.text()).empty()) {
            myEditedDef->getLogic()->setPhaseMaxDuration(tp->row, NBTrafficLightDefinition::UNSPECIFIED_DURATION);
            myHaveModifications = true;
            return 1;
        }
        // input error, reset value
        myPhaseTable->setItemText(tp->row, 2, varDurString(getPhases()[tp->row].maxDur).c_str());
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
    if (myCurrentJunction == 0) {
        myTextFieldJunctionID->setText("");
        myTextFieldJunctionStatus->setText("");
    } else {
        NBNode* nbn = myCurrentJunction->getNBNode();
        myTextFieldJunctionID->setText(nbn->getID().c_str());
        if (!nbn->isTLControlled()) {
            myTextFieldJunctionStatus->setText("uncontrolled");
        } else {
            myTextFieldJunctionStatus->setText(myHaveModifications ? "modified" : "unmodified");
        }
    }
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
    myTLSAttributes->clearTLSAttributes();
    initPhaseTable(); // only clears when there are no definitions
    myCycleDuration->hide();
    updateDescription();
}


void
GNETLSEditorFrame::buildIinternalLanes(NBTrafficLightDefinition* tlDef) {
    SUMORTree& rtree = myViewNet->getNet()->getVisualisationSpeedUp();
    // clean up previous objects
    for (auto it : myInternalLanes) {
        for (auto it_intLanes : it.second) {
            rtree.removeAdditionalGLObject(it_intLanes);
            delete it_intLanes;
        }
    }
    myInternalLanes.clear();
    // create new internal lanes
    if (tlDef != 0) {
        const int NUM_POINTS = 10;
        assert(myCurrentJunction);
        NBNode* nbn = myCurrentJunction->getNBNode();
        std::string innerID = ":" + nbn->getID(); // see NWWriter_SUMO::writeInternalEdges
        const NBConnectionVector& links = tlDef->getControlledLinks();
        for (auto it : links) {
            int tlIndex = it.getTLIndex();
            PositionVector shape = nbn->computeInternalLaneShape(it.getFrom(), NBEdge::Connection(it.getFromLane(),
                                   it.getTo(), it.getToLane()), NUM_POINTS);
            GNEInternalLane* ilane = new GNEInternalLane(this, innerID + '_' + toString(tlIndex),  shape, tlIndex);
            rtree.addAdditionalGLObject(ilane);
            myInternalLanes[tlIndex].push_back(ilane);
        }
        for (auto c : nbn->getCrossings()) {
            GNEInternalLane* ilane = new GNEInternalLane(this, c->id, c->shape, c->tlLinkNo);
            rtree.addAdditionalGLObject(ilane);
            myInternalLanes[c->tlLinkNo].push_back(ilane);
        }
    }
}


std::string
GNETLSEditorFrame::varDurString(SUMOTime dur) {
    return dur == NBTrafficLightDefinition::UNSPECIFIED_DURATION ? "   " : toString(STEPS2TIME(dur));
}

void
GNETLSEditorFrame::initPhaseTable(int index) {
    myPhaseTable->setVisibleRows(1);
    myPhaseTable->setVisibleColumns(2);
    myPhaseTable->hide();
    if (myTLSAttributes->getNumberOfTLSDefinitions() > 0) {
        const bool fixed = myEditedDef->getType() == TLTYPE_STATIC;
        const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = getPhases();
        myPhaseTable->setTableSize((int)phases.size(), fixed ? 2 : 4);
        myPhaseTable->setVisibleRows((int)phases.size());
        myPhaseTable->setVisibleColumns(fixed ? 2 : 4);
        for (int row = 0; row < (int)phases.size(); row++) {
            myPhaseTable->setItemText(row, 0, toString(STEPS2TIME(phases[row].duration)).c_str());
            if (!fixed) {
                myPhaseTable->setItemText(row, 1, varDurString(phases[row].minDur).c_str());
                myPhaseTable->setItemText(row, 2, varDurString(phases[row].maxDur).c_str());
            }
            myPhaseTable->setItemText(row, fixed ? 1 : 3, phases[row].state.c_str());
            myPhaseTable->getItem(row, 1)->setJustify(FXTableItem::LEFT);
        }
        myPhaseTable->fitColumnsToContents(0, fixed ? 2 : 4);
        myPhaseTable->setHeight((int)phases.size() * 21); // experimental
        myPhaseTable->setCurrentItem(index, 0);
        myPhaseTable->selectRow(index, true);
        myPhaseTable->show();
        myPhaseTable->setFocus();

        myTableScroll->setHeight(myPhaseTable->getHeight() + 10);
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
GNETLSEditorFrame::handleMultiChange(GNELane* lane, FXObject* obj, FXSelector sel, void* eventData) {
    if (myEditedDef != 0) {
        myHaveModifications = true;
        const NBConnectionVector& links = myEditedDef->getControlledLinks();
        std::set<std::string> fromIDs;
        fromIDs.insert(lane->getMicrosimID());
        GNEEdge& edge = lane->getParentEdge();
        // if neither the lane nor its edge are selected, apply changes to the whole edge
        if (!gSelected.isSelected(GLO_EDGE, edge.getGlID()) && !gSelected.isSelected(GLO_LANE, lane->getGlID())) {
            for (auto it_lane : edge.getLanes()) {
                fromIDs.insert(it_lane->getMicrosimID());
            }
        } else {
            // if the edge is selected, apply changes to all lanes of all selected edges
            if (gSelected.isSelected(GLO_EDGE, edge.getGlID())) {
                std::vector<GNEEdge*> edges = myViewNet->getNet()->retrieveEdges(true);
                for (auto it : edges) {
                    for (auto it_lane : it->getLanes()) {
                        fromIDs.insert(it_lane->getMicrosimID());
                    }
                }
            }
            // if the lane is selected, apply changes to all selected lanes
            if (gSelected.isSelected(GLO_LANE, lane->getGlID())) {
                std::vector<GNELane*> lanes = myViewNet->getNet()->retrieveLanes(true);
                for (auto it_lane : lanes) {
                    fromIDs.insert(it_lane->getMicrosimID());
                }
            }

        }
        // set new state for all connections from the chosen lane IDs
        for (auto it : links) {
            if (fromIDs.count(it.getFrom()->getLaneID(it.getFromLane())) > 0) {
                std::vector<GNEInternalLane*> lanes = myInternalLanes[it.getTLIndex()];
                for (auto it_lane : lanes) {
                    it_lane->onDefault(obj, sel, eventData);
                }
            }
        }
    }
}


bool
GNETLSEditorFrame::controlsEdge(GNEEdge& edge) const {
    if (myEditedDef != 0) {
        const NBConnectionVector& links = myEditedDef->getControlledLinks();
        for (auto it : links) {
            if (it.getFrom()->getID() == edge.getMicrosimID()) {
                return true;
            }
        }
    }
    return false;
}


SUMOTime
GNETLSEditorFrame::getSUMOTime(const FXString& string) {
    assert(GNEAttributeCarrier::canParse<double>(string.text()));
    return TIME2STEPS(GNEAttributeCarrier::parse<double>(string.text()));
}


void
GNETLSEditorFrame::updateCycleDuration() {
    SUMOTime cycleDuration = 0;
    for (auto it : getPhases()) {
        cycleDuration += it.duration;
    }
    std::string text = "Cycle time: " + toString(STEPS2TIME(cycleDuration));
    myCycleDuration->setText(text.c_str());
}


// ---------------------------------------------------------------------------
// GNETLSEditorFrame::TLSAttributes - methods
// ---------------------------------------------------------------------------

GNETLSEditorFrame::TLSAttributes::TLSAttributes(FXComposite* parent, GNETLSEditorFrame* TLSEditorParent) :
    FXGroupBox(parent, "Traffic light Attributes", GUIDesignGroupBoxFrame),
    myTLSEditorParent(TLSEditorParent) {

    // create frame, label and textfield for name (By default disabled)
    FXHorizontalFrame* nameFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myNameLabel = new FXLabel(nameFrame, "ID", 0, GUIDesignLabelAttribute);
    myNameTextField = new FXTextField(nameFrame, GUIDesignTextFieldNCol, myTLSEditorParent, MID_GNE_TLSFRAME_SWITCH, GUIDesignTextField);
    myNameTextField->disable();

    // create frame, label and comboBox for Program (By default hidden)
    FXHorizontalFrame* programFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myProgramLabel = new FXLabel(programFrame, "Program", 0, GUIDesignLabelAttribute);
    myProgramComboBox = new FXComboBox(programFrame, GUIDesignComboBoxNCol, myTLSEditorParent, MID_GNE_TLSFRAME_SWITCH, GUIDesignComboBoxAttribute);
    myProgramComboBox->disable();

    // create frame, label and TextField for Offset (By default disabled)
    FXHorizontalFrame* offsetFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myOffsetLabel = new FXLabel(offsetFrame, "Offset", 0, GUIDesignLabelAttribute);
    myOffsetTextField = new FXTextField(offsetFrame, GUIDesignTextFieldNCol, myTLSEditorParent, MID_GNE_TLSFRAME_OFFSET, GUIDesignTextFieldReal);
    myOffsetTextField->disable();
}


GNETLSEditorFrame::TLSAttributes::~TLSAttributes() {}


void
GNETLSEditorFrame::TLSAttributes::initTLSAttributes(GNEJunction* junction) {
    assert(junction);
    myTLSDefinitions.clear();
    // enable name TextField
    myNameTextField->enable();
    // enable Offset
    myOffsetTextField->enable();
    // obtain TLSs
    for (auto it : junction->getNBNode()->getControllingTLS()) {
        myTLSDefinitions.push_back(it);
        myNameTextField->setText(it->getID().c_str());
        myNameTextField->enable();
        myProgramComboBox->appendItem(it->getProgramID().c_str());
    }
    if (myTLSDefinitions.size() > 0) {
        myProgramComboBox->enable();
        myProgramComboBox->setCurrentItem(0);
        myProgramComboBox->setNumVisible(myProgramComboBox->getNumItems());
        myTLSEditorParent->onCmdDefSwitch(0, 0, 0);
    }
}


void
GNETLSEditorFrame::TLSAttributes::clearTLSAttributes() {
    // clear definitions
    myTLSDefinitions.clear();
    // clear and disable name TextField
    myNameTextField->setText("");
    myNameTextField->disable();
    // clear and disable myProgramComboBox
    myProgramComboBox->clearItems();
    myProgramComboBox->disable();
    // clear and disable Offset TextField
    myOffsetTextField->setText("");
    myOffsetTextField->disable();
}


NBTrafficLightDefinition*
GNETLSEditorFrame::TLSAttributes::getCurrentTLSDefinition() const {
    return myTLSDefinitions.at(myProgramComboBox->getCurrentItem());
}


int
GNETLSEditorFrame::TLSAttributes::getNumberOfTLSDefinitions() const {
    return (int)myTLSDefinitions.size();
}


int
GNETLSEditorFrame::TLSAttributes::getNumberOfPrograms() const {
    return myProgramComboBox->getNumItems();
}


SUMOTime
GNETLSEditorFrame::TLSAttributes::getOffset() const {
    return getSUMOTime(myOffsetTextField->getText());
}


void
GNETLSEditorFrame::TLSAttributes::setOffset(SUMOTime offset) {
    myOffsetTextField->setText(toString(STEPS2TIME(offset)).c_str());
}

/****************************************************************************/
