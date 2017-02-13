/****************************************************************************/
/// @file    GNESelectorFrame.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
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
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNESelectorFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
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
FXDEFMAP(GNESelectorFrame) GNESelectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_ELEMENTS,   GNESelectorFrame::onCmdSubset),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GNESelectorFrame::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GNESelectorFrame::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT,     GNESelectorFrame::onCmdInvert),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,      GNESelectorFrame::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_TAG,      GNESelectorFrame::onCmdSelMBTag),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_STRING,   GNESelectorFrame::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNESelectorFrame::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_SCALE,   GNESelectorFrame::onCmdScaleSelection)
};

// Object implementation
FXIMPLEMENT(GNESelectorFrame, FXVerticalFrame, GNESelectorFrameMap, ARRAYNUMBER(GNESelectorFrameMap))

// ===========================================================================
// method definitions
// ===========================================================================
GNESelectorFrame::GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, getStats().c_str()),
    mySetOperation(SET_ADD),
    mySetOperationTarget(mySetOperation),
    ALL_VCLASS_NAMES_MATCH_STRING("all " + joinToString(SumoVehicleClassStrings.getStrings(), " ")) {
    // selection modification mode
    FXGroupBox* selBox = new FXGroupBox(myContentFrame, "Modification Mode", GUIDesignGroupBoxFrame);
    // Create all options buttons
    new FXRadioButton(selBox, "add\t\tSelected objects are added to the previous selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_ADD, GUIDesignRadioButton);
    new FXRadioButton(selBox, "remove\t\tSelected objects are removed from the previous selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_SUB, GUIDesignRadioButton);
    new FXRadioButton(selBox, "keep\t\tRestrict previous selection by the current selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_RESTRICT, GUIDesignRadioButton);
    new FXRadioButton(selBox, "replace\t\tReplace previous selection by the current selection",
                      &mySetOperationTarget, FXDataTarget::ID_OPTION + SET_REPLACE, GUIDesignRadioButton);
    // Create groupBox for selection by expression matching (match box)
    FXGroupBox* elementBox = new FXGroupBox(myContentFrame, "type of element", GUIDesignGroupBoxFrame);
    // Create MatchTagBox for tags and fill it
    mySetComboBox = new FXComboBox(elementBox, GUIDesignComboBoxNCol, this, MID_CHOOSEN_ELEMENTS, GUIDesignComboBox);
    mySetComboBox->appendItem("Net Element");
    mySetComboBox->appendItem("Additional");
    mySetComboBox->setNumVisible(mySetComboBox->getNumItems());
    // Create groupBox fro selection by expression matching (match box)
    FXGroupBox* matchBox = new FXGroupBox(myContentFrame, "Match Attribute", GUIDesignGroupBoxFrame);
    // Create MatchTagBox for tags
    myMatchTagComboBox = new FXComboBox(matchBox, GUIDesignComboBoxNCol, this, MID_GNE_SELMB_TAG, GUIDesignComboBox);
    // Create listBox for Attributes
    myMatchAttrComboBox = new FXComboBox(matchBox, GUIDesignComboBoxNCol, NULL, 0, GUIDesignComboBox);
    // Set netElements as default tag
    mySetComboBox->setCurrentItem(0);
    // Fill list of sub-items
    onCmdSubset(0, 0, 0);
    // Set speed as default attribute
    myMatchAttrComboBox->setCurrentItem(3);
    // Create TextField for Match string
    myMatchString = new FXTextField(matchBox, GUIDesignTextFieldNCol, this, MID_GNE_SELMB_STRING, GUIDesignTextField);
    // Set default value for Match string
    myMatchString->setText(">10.0");
    // Create help button
    new FXButton(matchBox, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);
    // Create Groupbox for visual scalings
    FXGroupBox* selSizeBox = new FXGroupBox(myContentFrame, "Visual Scaling", GUIDesignGroupBoxFrame);
    // Create spin button and configure it
    mySelectionScaling = new FXRealSpinDial(selSizeBox, 7, this, MID_GNE_SELECT_SCALE, GUIDesignSpinDial);
    mySelectionScaling->setNumberFormat(1);
    mySelectionScaling->setIncrements(0.1, .5, 1);
    mySelectionScaling->setRange(1, 100);
    mySelectionScaling->setValue(1);
    mySelectionScaling->setHelpText("Enlarge selected objects");
    // Create groupbox for additional buttons
    FXGroupBox* additionalButtons = new FXGroupBox(myContentFrame, "Operations for selections", GUIDesignGroupBoxFrame);
    // Create "Clear List" Button
    new FXButton(additionalButtons, "Clear\t\t", 0, this, MID_CHOOSEN_CLEAR, GUIDesignButton);
    // Create "Invert" Button
    new FXButton(additionalButtons, "Invert\t\t", 0, this, MID_CHOOSEN_INVERT, GUIDesignButton);
    // Create "Save" Button
    new FXButton(additionalButtons, "Save\t\tSave ids of currently selected objects to a file.", 0, this, MID_CHOOSEN_SAVE, GUIDesignButton);
    // Create "Load" Button
    new FXButton(additionalButtons, "Load\t\tLoad ids from a file according to the current modfication mode.", 0, this, MID_CHOOSEN_LOAD, GUIDesignButton);
    // Create groupbox for information about selections
    FXGroupBox* selectionHintGroupBox = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    // Create Selection Hint
    new FXLabel(selectionHintGroupBox, " - Hold <SHIFT> for \n   rectangle selection.\n - Press <DEL> to\n   delete selected items.", 0, GUIDesignLabelLeft);
}


GNESelectorFrame::~GNESelectorFrame() {
    gSelected.remove2Update();
}


long
GNESelectorFrame::onCmdSubset(FXObject*, FXSelector, void*) {
    // Clear items of myMatchTagComboBox
    myMatchTagComboBox->clearItems();
    // Set items depending of current items
    const bool netElements = mySetComboBox->getCurrentItem() == 0;
    const std::vector<SumoXMLTag>& tags = GNEAttributeCarrier::allowedTags(netElements);
    for (std::vector<SumoXMLTag>::const_iterator it = tags.begin(); it != tags.end(); it++) {
        myMatchTagComboBox->appendItem(toString(*it).c_str());
    }
    myMatchTagComboBox->setCurrentItem(0); // edges
    myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
    // Fill attributes with the current element type
    onCmdSelMBTag(0, 0, 0);
    return 1;
}


long
GNESelectorFrame::onCmdLoad(FXObject*, FXSelector, void*) {
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
    myViewNet->update();
    return 1;
}


long
GNESelectorFrame::onCmdSave(FXObject*, FXSelector, void*) {
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
GNESelectorFrame::onCmdClear(FXObject*, FXSelector, void*) {
    myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    myViewNet->update();
    return 1;
}


long
GNESelectorFrame::onCmdInvert(FXObject*, FXSelector, void*) {
    std::set<GUIGlID> ids = myViewNet->getNet()->getGlIDs(GLO_JUNCTION);
    for (std::set<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        gSelected.toggleSelection(*it);
    }
    ids = myViewNet->getNet()->getGlIDs(myViewNet->selectEdges() ? GLO_EDGE : GLO_LANE);
    for (std::set<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        gSelected.toggleSelection(*it);
    }
    ids = myViewNet->getNet()->getGlIDs(GLO_ADDITIONAL);
    for (std::set<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        gSelected.toggleSelection(*it);
    }
    ids = myViewNet->getNet()->getGlIDs(GLO_CONNECTION);
    for (std::set<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        gSelected.toggleSelection(*it);
    }
    myViewNet->update();
    return 1;
}


long
GNESelectorFrame::onCmdSelMBTag(FXObject*, FXSelector, void*) {
    const bool netElements = mySetComboBox->getCurrentItem() == 0;
    const std::vector<SumoXMLTag>& tags = GNEAttributeCarrier::allowedTags(netElements);
    SumoXMLTag tag = tags[myMatchTagComboBox->getCurrentItem()];
    myMatchAttrComboBox->clearItems();
    const std::vector<std::pair <SumoXMLAttr, std::string> >& attrs = GNEAttributeCarrier::allowedAttributes(tag);
    for (std::vector<std::pair <SumoXMLAttr, std::string> >::const_iterator it = attrs.begin(); it != attrs.end(); it++) {
        myMatchAttrComboBox->appendItem(toString(it->first).c_str());
    }

    // @ToDo: Here can be placed a butto to set the default value
    myMatchAttrComboBox->setNumVisible(myMatchAttrComboBox->getNumItems());
    update();
    return 1;
}


long
GNESelectorFrame::onCmdSelMBString(FXObject*, FXSelector, void*) {
    const bool netElements = mySetComboBox->getCurrentItem() == 0;
    const std::vector<SumoXMLTag>& tags = GNEAttributeCarrier::allowedTags(netElements);
    SumoXMLTag tag = tags[myMatchTagComboBox->getCurrentItem()];
    const std::vector<std::pair <SumoXMLAttr, std::string> >& attrs = GNEAttributeCarrier::allowedAttributes(tag);
    SumoXMLAttr attr = attrs.at(myMatchAttrComboBox->getCurrentItem()).first;
    std::string expr(myMatchString->getText().text());
    bool valid = true;

    if (expr == "") {
        // the empty expression matches all objects
        handleIDs(getMatches(tag, attr, '@', 0, expr), false);
    } else if (GNEAttributeCarrier::isNumerical(tag, attr)) {
        // The expression must have the form
        //  <val matches if attr < val
        //  >val matches if attr > val
        //  =val matches if attr = val
        //  val matches if attr = val
        char compOp = expr[0];
        if (compOp == '<' || compOp == '>' || compOp == '=') {
            expr = expr.substr(1);
        } else {
            compOp = '=';
        }
        try {
            handleIDs(getMatches(tag, attr, compOp, TplConvert::_2SUMOReal(expr.c_str()), expr), false);
        } catch (EmptyData&) {
            valid = false;
        } catch (NumberFormatException&) {
            valid = false;
        }
    } else {
        // The expression must have the form
        //   =str: matches if <str> is an exact match
        //   !str: matches if <str> is not a substring
        //   ^str: matches if <str> is not an exact match
        //   str: matches if <str> is a substring (sends compOp '@')
        // Alternatively, if the expression is empty it matches all objects
        char compOp = expr[0];
        if (compOp == '=' || compOp == '!' || compOp == '^') {
            expr = expr.substr(1);
        } else {
            compOp = '@';
        }
        handleIDs(getMatches(tag, attr, compOp, 0, expr), false);
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
GNESelectorFrame::onCmdHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "Match Attribute Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << "The 'Match Attribute' controls allow to specify a set of objects which are then applied to the current selection "
            << "according to the current 'Modification Mode'.\n"
            << "1. Select an object type from the first input box\n"
            << "2. Select an attribute from the second input box\n"
            << "3. Enter a 'match expression' in the third input box and press <return>\n"
            << "\n"
            << "The empty expression matches all objects\n"
            << "For numerical attributes the match expression must consist of a comparison operator ('<', '>', '=') and a number.\n"
            << "An object matches if the comparison between its attribute and the given number by the given operator evaluates to 'true'\n"
            << "\n"
            << "For string attributes the match expression must consist of a comparison operator ('', '=', '!', '^') and a string.\n"
            << "  '' (no operator) matches if string is a substring of that object'ts attribute.\n"
            << "  '=' matches if string is an exact match.\n"
            << "  '!' matches if string is not a substring.\n"
            << "  '^' matches if string is not an exact match.\n"
            << "\n"
            << "Examples:\n"
            << "junction; id; 'foo' -> match all junctions that have 'foo' in their id\n"
            << "junction; type; '=priority' -> match all junctions of type 'priority', but not of type 'priority_stop'\n"
            << "edge; speed; '>10' -> match all edges with a speed above 10\n";
    new FXLabel(helpDialog, help.str().c_str(), 0, GUIDesignLabelLeft);
    // "OK"
    new FXButton(helpDialog, "OK\t\tSave modifications", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonDialog, 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}


long
GNESelectorFrame::onCmdScaleSelection(FXObject*, FXSelector, void*) {
    myViewNet->setSelectionScaling(mySelectionScaling->getValue());
    myViewNet->update();
    return 1;
}


void
GNESelectorFrame::show() {
    // selection may have changed due to deletions
    gSelected.add2Update(this);
    selectionUpdated();
    // Show frame
    GNEFrame::show();
}


void
GNESelectorFrame::hide() {
    // selection may have changed due to deletions
    gSelected.remove2Update();
    // hide frame
    GNEFrame::hide();
}


std::string
GNESelectorFrame::getStats() const {
    return "Selection:\n" +
           toString(gSelected.getSelected(GLO_JUNCTION).size()) + " Junctions\n" +
           toString(gSelected.getSelected(GLO_EDGE).size()) + " Edges\n" +
           toString(gSelected.getSelected(GLO_LANE).size()) + " Lanes\n" +
           toString(gSelected.getSelected(GLO_CONNECTION).size()) + " Connections\n" +
           toString(gSelected.getSelected(GLO_ADDITIONAL).size()) + " Additionals";
}


void
GNESelectorFrame::selectionUpdated() {
    myFrameHeaderLabel->setText(getStats().c_str());
    update();
}


void
GNESelectorFrame::handleIDs(std::vector<GUIGlID> ids, bool selectEdges, SetOperation setop) {
    const SetOperation setOperation = (setop == SET_DEFAULT ? (SetOperation)mySetOperation : setop);
    std::set<GUIGlID> previousSelection;
    myViewNet->getUndoList()->p_begin("change selection");
    if (setOperation == SET_REPLACE) {
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    } else if (setOperation == SET_RESTRICT) {
        previousSelection = gSelected.getSelected(); // have to make a copy
        myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    }
    // handle ids
    GUIGlObject* object;
    GUIGlObjectType type;
    std::set<GUIGlID> idsSet(ids.begin(), ids.end());
    std::set<GUIGlID> selected;
    std::set<GUIGlID> deselected;
    if (myViewNet->autoSelectNodes()) {
        for (std::vector<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
            GUIGlID id = *it;
            if (id > 0) { // net object?
                object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
                if (object->getType() == GLO_LANE && selectEdges) {
                    const GNEEdge& edge = (static_cast<GNELane*>(object))->getParentEdge();
                    idsSet.insert(edge.getGNEJunctionSource()->getGlID());
                    idsSet.insert(edge.getGNEJunctionDestiny()->getGlID());
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
                case GNESelectorFrame::SET_ADD:
                case GNESelectorFrame::SET_REPLACE:
                    selected.insert(id);
                    break;
                case GNESelectorFrame::SET_SUB:
                    deselected.insert(id);
                    break;
                case GNESelectorFrame::SET_RESTRICT:
                    if (previousSelection.count(id)) {
                        selected.insert(id);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), selected, deselected, true), true);
    myViewNet->getUndoList()->p_end();
    myViewNet->update();
}


std::vector<GUIGlID>
GNESelectorFrame::getMatches(SumoXMLTag tag, SumoXMLAttr attr, char compOp, SUMOReal val, const std::string& expr) {
    GUIGlObject* object;
    GNEAttributeCarrier* ac;
    std::vector<GUIGlID> result;
    const std::set<GUIGlID> allIDs = myViewNet->getNet()->getGlIDs();
    const bool numerical = GNEAttributeCarrier::isNumerical(tag, attr);
    for (std::set<GUIGlID>::const_iterator it = allIDs.begin(); it != allIDs.end(); it++) {
        GUIGlID id = *it;
        object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        if (!object) {
            throw ProcessError("Unkown object passed to GNESelectorFrame::getMatches (id=" + toString(id) + ").");
        }
        ac = dynamic_cast<GNEAttributeCarrier*>(object);
        if (ac && ac->getTag() == tag) { // not all objects need to be attribute carriers
            if (expr == "") {
                result.push_back(id);
            } else if (numerical) {
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
            } else {
                // string match
                std::string acVal = ac->getAttribute(attr);
                if ((attr == SUMO_ATTR_ALLOW || attr == SUMO_ATTR_DISALLOW) && acVal == "all") {
                    acVal = ALL_VCLASS_NAMES_MATCH_STRING;
                }
                switch (compOp) {
                    case '@':
                        if (acVal.find(expr) != std::string::npos) {
                            result.push_back(id);
                        }
                        break;
                    case '!':
                        if (acVal.find(expr) == std::string::npos) {
                            result.push_back(id);
                        }
                        break;
                    case '=':
                        if (acVal == expr) {
                            result.push_back(id);
                        }
                        break;
                    case '^':
                        if (acVal != expr) {
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
