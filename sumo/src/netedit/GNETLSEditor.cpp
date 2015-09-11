/****************************************************************************/
/// @file    GNETLSEditor.cpp
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying traffic lights
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <netbuild/NBTrafficLightDefinition.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include "GNETLSEditor.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNEInternalLane.h"
#include "GNEChange_TLS.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNETLSEditor) GNETLSEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,    MID_CANCEL,                 GNETLSEditor::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,    MID_OK,                     GNETLSEditor::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_CREATE,         GNETLSEditor::onCmdDefCreate),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_DELETE,         GNETLSEditor::onCmdDefDelete),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_SWITCH,         GNETLSEditor::onCmdDefSwitch),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_OFFSET,         GNETLSEditor::onCmdDefOffset),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_RENAME,         GNETLSEditor::onCmdDefRename),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_SUBRENAME,      GNETLSEditor::onCmdDefSubRename),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_DEF_ADDOFF,         GNETLSEditor::onCmdDefAddOff),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_GUESS_PROGRAM,      GNETLSEditor::onCmdGuess),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_PHASE_CREATE,       GNETLSEditor::onCmdPhaseCreate),
    FXMAPFUNC(SEL_COMMAND,    MID_GNE_PHASE_DELETE,       GNETLSEditor::onCmdPhaseDelete),
    FXMAPFUNC(SEL_SELECTED,   MID_GNE_PHASE_TABLE,        GNETLSEditor::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_DESELECTED, MID_GNE_PHASE_TABLE,        GNETLSEditor::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_CHANGED,    MID_GNE_PHASE_TABLE,        GNETLSEditor::onCmdPhaseSwitch),
    FXMAPFUNC(SEL_REPLACED,   MID_GNE_PHASE_TABLE,        GNETLSEditor::onCmdPhaseEdit),

    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_CREATE,         GNETLSEditor::onUpdDefCreate),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_DELETE,         GNETLSEditor::onUpdDefSwitch),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_SWITCH,         GNETLSEditor::onUpdDefSwitch),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_DEF_OFFSET,         GNETLSEditor::onUpdNeedsDef),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_PHASE_CREATE,       GNETLSEditor::onUpdNeedsDef),
    FXMAPFUNC(SEL_UPDATE,     MID_GNE_PHASE_DELETE,       GNETLSEditor::onUpdNeedsDef),
    FXMAPFUNC(SEL_UPDATE,     MID_CANCEL,                 GNETLSEditor::onUpdModified),
    FXMAPFUNC(SEL_UPDATE,     MID_OK,                     GNETLSEditor::onUpdModified),
};


// Object implementation
FXIMPLEMENT(GNETLSEditor, FXScrollWindow, GNETLSEditorMap, ARRAYNUMBER(GNETLSEditorMap))

// ===========================================================================
// static members
// ===========================================================================
const int GNETLSEditor::WIDTH = 140;

// ===========================================================================
// method definitions
// ===========================================================================
GNETLSEditor::GNETLSEditor(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList):
    FXScrollWindow(parent, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0),
    myHeaderFont(new FXFont(getApp(), "Arial", 11, FXFont::Bold)),
    myTableFont(new FXFont(getApp(), "Courier New", 9)),
    myCurrentJunction(0),
    myUndoList(undoList),
    myUpdateTarget(updateTarget),
    myHaveModifications(false),
    myEditedDef(0) {
    // heading
    myContentFrame = new FXVerticalFrame(this, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0);
    FXLabel* heading = new FXLabel(myContentFrame, "Edit Traffic Light", 0, JUSTIFY_LEFT);
    heading->setFont(myHeaderFont);
    myDescription = new FXLabel(myContentFrame, "", 0, JUSTIFY_LEFT);
    new FXHorizontalSeparator(myContentFrame, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);

    // create tlDef button
    new FXButton(myContentFrame, "Create TLS\t\tCreate a new traffic light program", 0, this, MID_GNE_DEF_CREATE,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    // delete tlDef button
    new FXButton(myContentFrame,
                 "Delete TLS\t\tDelete a traffic light program. If all programs are deleted the junction turns into a priority junction.",
                 0, this, MID_GNE_DEF_DELETE, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    // definitions list
    new FXLabel(myContentFrame, "Name, Program");
    myDefBox = new FXListBox(myContentFrame, this, MID_GNE_DEF_SWITCH,
                             FRAME_SUNKEN | FRAME_THICK | LISTBOX_NORMAL | LAYOUT_FIX_WIDTH, 0, 0, WIDTH - 4);

    // offset control
    new FXLabel(myContentFrame, "Offset");
    myOffset = new FXTextField(myContentFrame, 6,
                               this, MID_GNE_DEF_OFFSET, TEXTFIELD_NORMAL | TEXTFIELD_REAL, 0, 0, 0, 0, 4, 2, 0, 2);

    new FXHorizontalSeparator(myContentFrame, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);

    // phase table
    new FXLabel(myContentFrame, "Phases");
    myPhaseTable = new FXTable(myContentFrame, this, MID_GNE_PHASE_TABLE, LAYOUT_FIX_HEIGHT | LAYOUT_FIX_WIDTH);
    myPhaseTable->setColumnHeaderMode(LAYOUT_FIX_HEIGHT);
    myPhaseTable->setColumnHeaderHeight(0);
    myPhaseTable->setRowHeaderMode(LAYOUT_FIX_WIDTH);
    myPhaseTable->setRowHeaderWidth(0);
    myPhaseTable->hide();
    myPhaseTable->setFont(myTableFont);
    myPhaseTable->setHelpText("phase duration in seconds | phase state");

    // total duration info
    myCycleDuration = new FXLabel(myContentFrame, "");

    // insert new phase button
    new FXButton(myContentFrame, "Insert Phase\t\tInsert Phase after selected phase", 0, this, MID_GNE_PHASE_CREATE,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    // delete phase button
    new FXButton(myContentFrame, "Delete Phase\t\tDelete selected phase", 0, this, MID_GNE_PHASE_DELETE,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(myContentFrame, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
    // buttons
    // "Cancel"
    new FXButton(myContentFrame, "Cancel\t\tDiscard program modifications (Esc)", 0, this, MID_CANCEL,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "OK"
    new FXButton(myContentFrame, "Save\t\tSave program modifications (Enter)", 0, this, MID_OK,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    new FXHorizontalSeparator(myContentFrame, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
    // "Add 'off' program"
    /*
    new FXButton(myContentFrame, "Add \"Off\"-Program\t\tAdds a program for switching off this traffic light",
            0, this, MID_GNE_DEF_ADDOFF,
            ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
            0, 0, 0, 0, 4, 4, 3, 3);
    */
}


GNETLSEditor::~GNETLSEditor() {
    delete myHeaderFont;
    delete myTableFont;
    cleanup();
}


void
GNETLSEditor::editJunction(GNEJunction* junction) {
    if (myCurrentJunction == 0 || (!myHaveModifications && (junction != myCurrentJunction))) {
        onCmdCancel(0, 0, 0);
        myUndoList->p_begin("modifying traffic light definition");
        myCurrentJunction = junction;
        myCurrentJunction->selectTLS(true);
        initDefinitions();
    } else {
        myUpdateTarget->setStatusBarText("Unsaved modifications. Abort or Save");
    }
}


long
GNETLSEditor::onCmdCancel(FXObject*, FXSelector, void*) {
    if (myCurrentJunction != 0) {
        myUndoList->p_abort();
        cleanup();
        myUpdateTarget->update();
    }
    return 1;
}


long
GNETLSEditor::onCmdOK(FXObject*, FXSelector, void*) {
    if (myCurrentJunction != 0) {
        if (myHaveModifications) {
            NBTrafficLightDefinition* old = myDefinitions[myDefBox->getCurrentItem()];
            myUndoList->add(new GNEChange_TLS(myCurrentJunction, old, false), true);
            myUndoList->add(new GNEChange_TLS(myCurrentJunction, myEditedDef, true), true);
            myEditedDef = 0;
            myUndoList->p_end();
            cleanup();
            myUpdateTarget->update();
        } else {
            onCmdCancel(0, 0, 0);
        }
    }
    return 1;
}


long
GNETLSEditor::onCmdDefCreate(FXObject*, FXSelector, void*) {
    GNEJunction* junction = myCurrentJunction;
    onCmdCancel(0, 0, 0); // abort because we onCmdOk assumes we wish to save an edited definition
    if (junction->getAttribute(SUMO_ATTR_TYPE) != toString(NODETYPE_TRAFFIC_LIGHT)) {
        junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_TRAFFIC_LIGHT), myUndoList);
    } else {
        myUndoList->add(new GNEChange_TLS(junction, 0, true, true), true);
    }
    return 1;
}


long
GNETLSEditor::onCmdDefDelete(FXObject*, FXSelector, void*) {
    GNEJunction* junction = myCurrentJunction;
    const bool changeType = myDefinitions.size() == 1;
    onCmdCancel(0, 0, 0); // abort because onCmdOk assumes we wish to save an edited definition
    if (changeType) {
        junction->setAttribute(SUMO_ATTR_TYPE, toString(NODETYPE_PRIORITY), myUndoList);
    } else {
        NBTrafficLightDefinition* tlDef = myDefinitions[myDefBox->getCurrentItem()];
        myUndoList->add(new GNEChange_TLS(junction, tlDef, false), true);
    }
    return 1;
}


long
GNETLSEditor::onCmdDefSwitch(FXObject*, FXSelector, void*) {
    assert(myCurrentJunction != 0);
    assert((int)myDefinitions.size() == myDefBox->getNumItems());
    NBTrafficLightDefinition* tlDef = myDefinitions[myDefBox->getCurrentItem()];
    // logic may not have been recomputed yet. recompute to be sure
    NBTrafficLightLogicCont& tllCont = myUpdateTarget->getNet()->getTLLogicCont();
    myUpdateTarget->getNet()->computeJunction(myCurrentJunction);
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
        myUpdateTarget->setStatusBarText("Traffic light does not control any links");
    }
    return 1;
}


long
GNETLSEditor::onUpdDefSwitch(FXObject* o, FXSelector, void*) {
    const bool enable = myDefinitions.size() > 0 && !myHaveModifications;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditor::onUpdNeedsDef(FXObject* o, FXSelector, void*) {
    const bool enable = myDefinitions.size() > 0;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditor::onUpdDefCreate(FXObject* o, FXSelector, void*) {
    const bool enable = myCurrentJunction != 0 && !myHaveModifications;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}


long
GNETLSEditor::onUpdModified(FXObject* o, FXSelector, void*) {
    bool enable = myHaveModifications;
    o->handle(this, FXSEL(SEL_COMMAND, enable ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE), 0);
    return 1;
}



long
GNETLSEditor::onCmdDefOffset(FXObject*, FXSelector, void*) {
    myHaveModifications = true;
    myEditedDef->setOffset(getSUMOTime(myOffset->getText()));
    return 1;
}


long
GNETLSEditor::onCmdDefRename(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditor::onCmdDefSubRename(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditor::onCmdDefAddOff(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditor::onCmdGuess(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNETLSEditor::onCmdPhaseSwitch(FXObject*, FXSelector, void*) {
    const unsigned int index = myPhaseTable->getCurrentRow();
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
    myUpdateTarget->update();
    return 1;
}


long
GNETLSEditor::onCmdPhaseCreate(FXObject*, FXSelector, void*) {
    myHaveModifications = true;
    const unsigned int numLinks = myEditedDef->getLogic()->getNumLinks();
    // allows insertion at first position by deselecting via arrow keys
    unsigned int newIndex = myPhaseTable->getSelStartRow() + 1;
    myEditedDef->getLogic()->addStep(TIME2STEPS(3), std::string(numLinks, LINKSTATE_TL_RED), newIndex);
    myPhaseTable->setCurrentItem(newIndex, 0);
    initPhaseTable(newIndex);
    myPhaseTable->setFocus();
    return 1;
}


long
GNETLSEditor::onCmdPhaseDelete(FXObject*, FXSelector, void*) {
    myHaveModifications = true;
    myEditedDef->getLogic()->deletePhase(myPhaseTable->getCurrentRow());
    initPhaseTable(0);
    myPhaseTable->setFocus();
    return 1;
}


long
GNETLSEditor::onCmdPhaseEdit(FXObject*, FXSelector, void* ptr) {
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
GNETLSEditor::updateDescription() const {
    std::string description;
    if (myCurrentJunction == 0) {
        description = "No Junction Selected\n";
    } else {
        NBNode* nbn = myCurrentJunction->getNBNode();
        description = "Junction '" + nbn->getID() + "'\n(";
        if (!nbn->isTLControlled()) {
            description += "uncontrolled, ";
        }
        description += (myHaveModifications ? "modified)" : "unmodified)");
    }
    myDescription->setText(description.c_str());
}


void
GNETLSEditor::cleanup() {
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
    myOffset->setText("");
    myDefinitions.clear();
    myDefBox->hide();
    initPhaseTable(); // only clears when there are no definitions
    myCycleDuration->hide();
    updateDescription();
}


void
GNETLSEditor::buildIinternalLanes(NBTrafficLightDefinition* tlDef) {
    // clean up previous objects
    SUMORTree& rtree = myUpdateTarget->getNet()->getVisualisationSpeedUp();
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
        SUMORTree& rtree = myUpdateTarget->getNet()->getVisualisationSpeedUp();
        NBNode* nbn = myCurrentJunction->getNBNode();
        std::string innerID = ":" + nbn->getID(); // see NWWriter_SUMO::writeInternalEdges
        const NBConnectionVector& links = tlDef->getControlledLinks();
        for (NBConnectionVector::const_iterator it = links.begin(); it != links.end(); it++) {
            int tlIndex = it->getTLIndex();
            PositionVector shape = nbn->computeInternalLaneShape(it->getFrom(), NBEdge::Connection(it->getFromLane(),
                                   it->getTo(), it->getToLane()), NUM_POINTS);
            GNEInternalLane* ilane = new GNEInternalLane(this, innerID + '_' + toString(tlIndex) , shape, tlIndex);
            rtree.addAdditionalGLObject(ilane);
            myInternalLanes[tlIndex].push_back(ilane);
        }
    }
}


void
GNETLSEditor::initDefinitions() {
    myDefinitions.clear();
    myDefBox->clearItems();
    assert(myCurrentJunction);
    NBNode* nbn = myCurrentJunction->getNBNode();
    std::set<NBTrafficLightDefinition*> tldefs = nbn->getControllingTLS();
    for (std::set<NBTrafficLightDefinition*>::iterator it = tldefs.begin(); it != tldefs.end(); it++) {
        myDefinitions.push_back(*it);
        std::string item = (*it)->getID() + ", " + (*it)->getProgramID();
        myDefBox->appendItem(item.c_str());
    }
    if (myDefinitions.size() > 0) {
        myDefBox->setCurrentItem(0);
        myDefBox->setNumVisible(myDefBox->getNumItems());
        myDefBox->show();
        onCmdDefSwitch(0, 0, 0);
    }
    updateDescription();
}


void
GNETLSEditor::initPhaseTable(unsigned int index) {
    myPhaseTable->setVisibleRows(1);
    myPhaseTable->setVisibleColumns(2);
    myPhaseTable->hide();
    if (myDefinitions.size() > 0) {
        const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = getPhases();
        myPhaseTable->setTableSize((int)phases.size(), 2);
        myPhaseTable->setVisibleRows((int)phases.size());
        myPhaseTable->setVisibleColumns(2);
        for (unsigned int row = 0; row < phases.size(); row++) {
            myPhaseTable->setItemText(row, 0, toString(STEPS2TIME(phases[row].duration)).c_str());
            myPhaseTable->setItemText(row, 1, phases[row].state.c_str());
            myPhaseTable->getItem(row, 1)->setJustify(FXTableItem::LEFT);
        }
        myPhaseTable->fitColumnsToContents(0, 2);
        const int maxWidth = WIDTH - 4;
        int desiredWidth = myPhaseTable->getColumnWidth(0) +
                           myPhaseTable->getColumnWidth(1) + 3;
        int spaceForScrollBar = desiredWidth > maxWidth ? 15 : 0;
        myPhaseTable->setHeight((int)phases.size() * 21 + spaceForScrollBar); // experimental
        myPhaseTable->setWidth(MIN2(desiredWidth, maxWidth));
        myPhaseTable->setCurrentItem(index, 0);
        myPhaseTable->selectRow(index, true);
        myPhaseTable->show();
        myPhaseTable->setFocus();
    }
    update();
}


const std::vector<NBTrafficLightLogic::PhaseDefinition>&
GNETLSEditor::getPhases() {
    return myEditedDef->getLogic()->getPhases();
}


void
GNETLSEditor::handleChange(GNEInternalLane* lane) {
    myHaveModifications = true;
    myEditedDef->getLogic()->setPhaseState(myPhaseTable->getCurrentRow(), lane->getTlIndex(), lane->getLinkState());
    initPhaseTable(myPhaseTable->getCurrentRow());
    myPhaseTable->setFocus();
}


SUMOTime
GNETLSEditor::getSUMOTime(const FXString& string) {
    assert(GNEAttributeCarrier::canParse<SUMOReal>(string.text()));
    return TIME2STEPS(GNEAttributeCarrier::parse<SUMOReal>(string.text()));
}


void
GNETLSEditor::updateCycleDuration() {
    SUMOTime cycleDuration = 0;
    for (std::vector<NBTrafficLightLogic::PhaseDefinition>::const_iterator it = getPhases().begin(); it != getPhases().end(); it++) {
        cycleDuration += it->duration;
    }
    std::string text = "Cycle time: " + toString(STEPS2TIME(cycleDuration));
    myCycleDuration->setText(text.c_str());
}


/****************************************************************************/
