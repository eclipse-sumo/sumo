/****************************************************************************/
/// @file    GNESelector.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id: GNESelector.cpp 4378 2015-03-03 10:19:20Z erdm_ja $
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNESelector.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNESelector) GNESelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GNESelector::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GNESelector::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT,     GNESelector::onCmdInvert),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,      GNESelector::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_TAG,      GNESelector::onCmdSelMBTag),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_STRING,   GNESelector::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_HELP,           GNESelector::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_SCALE,   GNESelector::onCmdScaleSelection)
};

// Object implementation
FXIMPLEMENT(GNESelector, FXScrollWindow, GNESelectorMap, ARRAYNUMBER(GNESelectorMap))

// ===========================================================================
// static members
// ===========================================================================
const int GNESelector::WIDTH = 140;

// ===========================================================================
// method definitions
// ===========================================================================
GNESelector::GNESelector(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList):
    FXScrollWindow(parent, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0),
    myHeaderFont(new FXFont(getApp(), "Arial", 14, FXFont::Bold)),
    myUpdateTarget(updateTarget),
    mySetOperation(SET_ADD),
    mySetOperationTarget(mySetOperation),
    myUndoList(undoList)
{
    // stats
    myContentFrame = new FXVerticalFrame(this, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0);
    myStats = new FXLabel(myContentFrame, getStats().c_str(), 0, JUSTIFY_LEFT);
    myStats->setFont(myHeaderFont);

    // selection modification mode
    FXGroupBox* selBox = new FXGroupBox(myContentFrame, "Modification Mode",
                                        GROUPBOX_NORMAL | FRAME_GROOVE | LAYOUT_FILL_X, 2, 0, 0, 0, 4, 2, 2, 2);
    new FXRadioButton(selBox, "add\t\tSelected objects are added to the previous selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_ADD);
    new FXRadioButton(selBox, "remove\t\tSelected objects are removed from the previous selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_SUB);
    new FXRadioButton(selBox, "keep\t\tRestrict previous selection by the current selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_RESTRICT);
    new FXRadioButton(selBox, "replace\t\tReplace previous selection by the current selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_REPLACE);

    // selection by expression matching (match box)
    FXGroupBox* matchBox = new FXGroupBox(myContentFrame, "Match Attribute",
                                          GROUPBOX_NORMAL | FRAME_GROOVE | LAYOUT_FILL_X, 2, 0, 0, 0, 4, 2, 2, 2);
    myMatchTagBox = new FXListBox(matchBox, this, MID_GNE_SELMB_TAG);
    const std::vector<SumoXMLTag>& tags = GNEAttributeCarrier::allowedTags();
    for (std::vector<SumoXMLTag>::const_iterator it = tags.begin(); it != tags.end(); it++) {
        myMatchTagBox->appendItem(toString(*it).c_str());
    }
    myMatchTagBox->setCurrentItem(1); // edges
    myMatchTagBox->setNumVisible(myMatchTagBox->getNumItems());
    myMatchAttrBox = new FXListBox(matchBox);
    onCmdSelMBTag(0, 0, 0);
    myMatchAttrBox->setCurrentItem(1); // speed
    myMatchString = new FXTextField(matchBox, 12, this, MID_GNE_SELMB_STRING, TEXTFIELD_NORMAL, 0, 0, 0, 0, 4, 2, 0, 2);
    myMatchString->setText(">10.0");
    new FXButton(matchBox, "Help", 0, this, MID_GNE_HELP);

    FXGroupBox* selSizeBox = new FXGroupBox(myContentFrame, "Visual Scaling",
                                            GROUPBOX_NORMAL | FRAME_GROOVE | LAYOUT_FILL_X, 2, 0, 0, 0, 4, 2, 2, 2);
    mySelectionScaling =
        new FXRealSpinDial(selSizeBox, 7, this, MID_GNE_SELECT_SCALE,
                           LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_Y);
    mySelectionScaling->setNumberFormat(1);
    mySelectionScaling->setIncrements(0.1, .5, 1);
    mySelectionScaling->setRange(1, 100);
    mySelectionScaling->setValue(1);
    mySelectionScaling->setHelpText("Enlarge selected objects");

    // additional buttons
    // new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_FILL_X);
    // "Clear List"
    new FXButton(myContentFrame, "Clear\t\t", 0, this, MID_CHOOSEN_CLEAR,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Invert"
    new FXButton(myContentFrame, "Invert\t\t", 0, this, MID_CHOOSEN_INVERT,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "Save"
    new FXButton(myContentFrame, "Save\t\tSave ids of currently selected objects to a file.", 0, this, MID_CHOOSEN_SAVE,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);

    // "Load"
    new FXButton(myContentFrame, "Load\t\tLoad ids from a file according to the current modfication mode.", 0, this, MID_CHOOSEN_LOAD,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);


    // Selection Hint
    new FXLabel(myContentFrame, "Hold <SHIFT> for\nrectangle selection.\nPress <DEL> to\ndelete selected items.", 0, JUSTIFY_LEFT);
}


GNESelector::~GNESelector() {
    delete myHeaderFont;
    gSelected.remove2Update();
}


long
GNESelector::onCmdLoad(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open List of Selected Items");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Selection files (*.txt)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // @todo maybe rewrite so that mySetOperation also applies to loaded items?
        std::string errors;
        std::set<GUIGlID> ids = gSelected.loadIDs(file, errors);
        handleIDs(std::vector<GUIGlID>(ids.begin(), ids.end()), false);
        if (errors != "") {
            FXMessageBox::error(this, MBOX_OK, "Errors while loading Selection", "%s", errors.c_str());
        }
    }
    myUpdateTarget->update();
    return 1;
}


long
GNESelector::onCmdSave(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(
                        this, "Save List of selected Items", ".txt", GUIIconSubSys::getIcon(ICON_EMPTY), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        gSelected.save(file.text());
    } catch (IOError& e) {
        FXMessageBox::error(this, MBOX_OK, "Storing Selection failed", "%s", e.what());
    }
    return 1;
}


long
GNESelector::onCmdClear(FXObject*, FXSelector, void*) {
    myUndoList->add(new GNEChange_Selection(std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    myUpdateTarget->update();
    return 1;
}


long
GNESelector::onCmdInvert(FXObject*, FXSelector, void*) {
    std::set<GUIGlID> ids = myUpdateTarget->getNet()->getGlIDs(GLO_JUNCTION);
    for (std::set<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        gSelected.toggleSelection(*it);
    }
    ids = myUpdateTarget->getNet()->getGlIDs(myUpdateTarget->selectEdges() ? GLO_EDGE : GLO_LANE);
    for (std::set<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        gSelected.toggleSelection(*it);
    }
    myUpdateTarget->update();
    return 1;
}


long
GNESelector::onCmdSelMBTag(FXObject*, FXSelector, void*) {
    const std::vector<SumoXMLTag>& tags = GNEAttributeCarrier::allowedTags();
    SumoXMLTag tag = tags[myMatchTagBox->getCurrentItem()];
    myMatchAttrBox->clearItems();
    const std::vector<SumoXMLAttr>& attrs = GNEAttributeCarrier::allowedAttributes(tag);
    for (std::vector<SumoXMLAttr>::const_iterator it = attrs.begin(); it != attrs.end(); it++) {
        myMatchAttrBox->appendItem(toString(*it).c_str());
    }
    myMatchAttrBox->setNumVisible(myMatchAttrBox->getNumItems());
    update();
    return 1;
}


long
GNESelector::onCmdSelMBString(FXObject*, FXSelector, void*) {
    const std::vector<SumoXMLTag>& tags = GNEAttributeCarrier::allowedTags();
    SumoXMLTag tag = tags[myMatchTagBox->getCurrentItem()];
    const std::vector<SumoXMLAttr>& attrs = GNEAttributeCarrier::allowedAttributes(tag);
    SumoXMLAttr attr = attrs[myMatchAttrBox->getCurrentItem()];
    std::string expr(myMatchString->getText().text());
    bool valid = true;

    if (GNEAttributeCarrier::isNumerical(attr)) {
        if (expr == "") {
            handleIDs(getMatches(tag, attr, '@', 0, expr), false);
        } else if (expr.size() < 2) {
            valid = false;
        } else {
            // numerical attr must have the form [<,>,=]<nr>
            // or be completely empty (matches all)
            const char compOp = expr[0];
            expr = expr.substr(1);
            SUMOReal val;
            std::istringstream buf(expr);
            buf >> val;
            if (!buf.fail() && (size_t)buf.tellg() == expr.size()) {
                switch (compOp) {
                    case '<':
                    case '>':
                    case '=':
                        handleIDs(getMatches(tag, attr, compOp, val, ""), false);
                        break;
                    default:
                        valid = false;
                }
            } else {
                valid = false;
            }
        }
    } else {
        handleIDs(getMatches(tag, attr, '@', 0, expr), false);
    }
    if (valid) {
        myMatchString->setTextColor(FXRGB(0, 0, 0));
        myMatchString->killFocus();
    } else {
        myMatchString->setTextColor(FXRGB(255, 0, 0));
    }

    return 1;
}


long
GNESelector::onCmdHelp(FXObject*, FXSelector, void*) {
    std::ostringstream help;
    help << "The 'Match Attribute' controls are a flexible tool for modifying the current selection\n"
         << "It allows you to specify a set of objects which are then applied to the current selection "
         << "according to the current 'Modification Mode'.\n"
         << "To specify a set of objects you need to perform 3 steps:\n"
         << "1. Select an object type from the first input box\n"
         << "2. Select an attribute from the second input box\n"
         << "3. Enter a 'match expression' in the third input box and press <return>\n\n"
         << "For string attributes an object is selected if the match expression is a substring of that object's attribute.\n"
         << "For numerical attributes the match expression must consist of a comparison operator ('<', '>', '='') and a number.\n"
         << "An object matches if the comparison between its attribute and the given number by the given operator evaluates to 'true'\n"
         << "Examples:\n"
         << "junction; id; 'foo' -> match all junctions that have 'foo' in their id\n"
         << "edge; speed; '>10' -> match all edges with a speed above 10\n";
    FXMessageBox::question(this, MBOX_OK, "Selection Help", "%s", help.str().c_str());
    return 1;
}


long
GNESelector::onCmdScaleSelection(FXObject*, FXSelector, void*) {
    myUpdateTarget->setSelectionScaling(mySelectionScaling->getValue());
    myUpdateTarget->update();
    return 1;
}


void
GNESelector::show() {
    gSelected.add2Update(this);
    selectionUpdated(); // selection may have changed due to deletions
    FXScrollWindow::show();
}


void
GNESelector::hide() {
    gSelected.remove2Update();
    FXScrollWindow::hide();
}


std::string
GNESelector::getStats() const {
    return "Selection:\n" +
           toString(gSelected.getSelected(GLO_JUNCTION).size()) + " Junctions\n" +
           toString(gSelected.getSelected(GLO_EDGE).size()) + " Edges\n" +
           toString(gSelected.getSelected(GLO_LANE).size()) + " Lanes\n";
}


void
GNESelector::selectionUpdated() {
    myStats->setText(getStats().c_str());
    update();
}


void
GNESelector::handleIDs(std::vector<GUIGlID> ids, bool selectEdges, SetOperation setop) {
    const SetOperation setOperation = (setop == SET_DEFAULT ? (SetOperation)mySetOperation : setop);
    std::set<GUIGlID> previousSelection;
    myUndoList->p_begin("change selection");
    if (setOperation == SET_REPLACE) {
        myUndoList->add(new GNEChange_Selection(std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    } else if (setOperation == SET_RESTRICT) {
        previousSelection = gSelected.getSelected(); // have to make a copy
        myUndoList->add(new GNEChange_Selection(std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    }
    // handle ids
    GUIGlObject* object;
    GUIGlObjectType type;
    std::set<GUIGlID> idsSet(ids.begin(), ids.end());
    std::set<GUIGlID> selected;
    std::set<GUIGlID> deselected;
    if (myUpdateTarget->autoSelectNodes()) {
        for (std::vector<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
            GUIGlID id = *it;
            if (id > 0) { // net object?
                object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
                if (object->getType() == GLO_LANE && selectEdges) {
                    const GNEEdge& edge = (static_cast<GNELane*>(object))->getParentEdge();
                    idsSet.insert(edge.getSource()->getGlID());
                    idsSet.insert(edge.getDest()->getGlID());
                }
                GUIGlObjectStorage::gIDStorage.unblockObject(id);
            }
        }
    }
    for (std::set<GUIGlID>::const_iterator it = idsSet.begin(); it != idsSet.end(); it++) {
        GUIGlID id = *it;
        if (id > 0) { // net object?
            object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
            if (object == 0) {
                // in debug mode we would like to know about this.
                // It might be caused by a corrupted gl-name stack.
                // However, most cases of uninitizliaed values would go hidden since 0 is assumed to be the net object anyway
                assert(false);
                continue;
            }
            type = object->getType();
            GUIGlObjectStorage::gIDStorage.unblockObject(id);
            if (type == GLO_LANE && selectEdges) {
                // @note edge may be selected/deselected multiple times but this shouldn't
                // hurt unless we add SET_TOGGLE
                id = (static_cast<GNELane*>(object))->getParentEdge().getGlID();
            }
            // doing the switch outside the loop requires functional techniques. this was deemed to ugly
            switch (setOperation) {
                case GNESelector::SET_ADD:
                case GNESelector::SET_REPLACE:
                    selected.insert(id);
                    break;
                case GNESelector::SET_SUB:
                    deselected.insert(id);
                    break;
                case GNESelector::SET_RESTRICT:
                    if (previousSelection.count(id)) {
                        selected.insert(id);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    myUndoList->add(new GNEChange_Selection(selected, deselected, true), true);
    myUndoList->p_end();
    myUpdateTarget->update();
}


std::vector<GUIGlID>
GNESelector::getMatches(SumoXMLTag tag, SumoXMLAttr attr, char compOp, SUMOReal val, const std::string& expr) {
    GUIGlObject* object;
    GNEAttributeCarrier* ac;
    std::vector<GUIGlID> result;
    const std::set<GUIGlID> allIDs = myUpdateTarget->getNet()->getGlIDs();
    for (std::set<GUIGlID>::const_iterator it = allIDs.begin(); it != allIDs.end(); it++) {
        GUIGlID id = *it;
        object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        if (!object) {
            throw ProcessError("Unkown object passed to GNESelector::getMatches (id=" + toString(id) + ").");
        }
        ac = dynamic_cast<GNEAttributeCarrier*>(object);
        if (ac && ac->getTag() == tag) { // not all objects need to be attribute carriers
            if (compOp == '@') { // match against expr
                std::string acVal = ac->getAttribute(attr);
                if (acVal.find(expr) != std::string::npos) {
                    result.push_back(id);
                }
            } else { // match against val
                SUMOReal acVal;
                std::istringstream buf(ac->getAttribute(attr));
                buf >> acVal;
                switch (compOp) {
                    case '<':
                        if (acVal < val) {
                            result.push_back(id);
                        }
                        break;
                    case '>':
                        if (acVal > val) {
                            result.push_back(id);
                        }
                        break;
                    case '=':
                        if (acVal == val) {
                            result.push_back(id);
                        }
                        break;
                }
            }
        }
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
    }
    return result;
}

/****************************************************************************/
