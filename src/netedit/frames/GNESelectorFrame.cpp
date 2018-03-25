/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNESelectorFrame.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
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
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNENet.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEAttributeCarrier.h>

#include "GNESelectorFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNESelectorFrame::ModificationMode) ModificationModeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNESelectorFrame::ModificationMode::onCmdSelectModificationMode),
};

FXDEFMAP(GNESelectorFrame::ElementSet) ElementSetMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_ELEMENTS,   GNESelectorFrame::ElementSet::onCmdSelectElementSet),
};

FXDEFMAP(GNESelectorFrame) SelectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTTAG,        GNESelectorFrame::onCmdSelMBTag),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,                       GNESelectorFrame::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,                       GNESelectorFrame::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT,                     GNESelectorFrame::onCmdInvert),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,                      GNESelectorFrame::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,  GNESelectorFrame::onCmdSelMBAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_PROCESSSTRING,    GNESelectorFrame::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTSCALE,      GNESelectorFrame::onCmdScaleSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                               GNESelectorFrame::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNESelectorFrame, FXVerticalFrame, SelectorFrameMap, ARRAYNUMBER(SelectorFrameMap))
FXIMPLEMENT(GNESelectorFrame::ModificationMode, FXGroupBox, ModificationModeMap, ARRAYNUMBER(ModificationModeMap))
FXIMPLEMENT(GNESelectorFrame::ElementSet, FXGroupBox, ElementSetMap, ARRAYNUMBER(ElementSetMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNESelectorFrame::ObjectTypeEntry::ObjectTypeEntry(FXMatrix* parent, const std::string& label, const std::string& label2) {
    count = new FXLabel(parent, "0", 0, GUIDesignLabelLeft);
    typeName = new FXLabel(parent, label.c_str(), 0, GUIDesignLabelLeft);
    locked = new FXMenuCheck(parent, ("lock\t\tLock " + label2 + " selection").c_str(), 0, 0, LAYOUT_FILL_X | LAYOUT_RIGHT);
}


GNESelectorFrame::GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Selection"),
    myCurrentAttribute(SUMO_ATTR_NOTHING) {
    // create combo box and labels for selected items
    FXGroupBox* mySelectedItemsComboBox = new FXGroupBox(myContentFrame, "Selected items", 0, GUIDesignGroupBoxFrame);
    FXMatrix* mSelectedItems = new FXMatrix(mySelectedItemsComboBox, 3, (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    // create typeEntries for the different elements
    myTypeEntries[GLO_JUNCTION] = ObjectTypeEntry(mSelectedItems, "Junctions", "junction");
    myTypeEntries[GLO_EDGE] = ObjectTypeEntry(mSelectedItems, "Edges", "edge");
    myTypeEntries[GLO_LANE] = ObjectTypeEntry(mSelectedItems, "Lanes", "lane");
    myTypeEntries[GLO_CONNECTION] = ObjectTypeEntry(mSelectedItems, "Connections", "connection");
    myTypeEntries[GLO_ADDITIONAL] = ObjectTypeEntry(mSelectedItems, "Additionals", "additional");
    myTypeEntries[GLO_CROSSING] = ObjectTypeEntry(mSelectedItems, "Crossings", "crossing");
    myTypeEntries[GLO_POLYGON] = ObjectTypeEntry(mSelectedItems, "Polygons", "polygon");
    myTypeEntries[GLO_POI] = ObjectTypeEntry(mSelectedItems, "POIs", "POI");
    // create Modification Mode selector
    myModificationMode = new ModificationMode(this);
    // create ElementSet selector
    myElementSet = new ElementSet(this);
    // Create groupBox fro selection by expression matching (match box)
    FXGroupBox* matchBox = new FXGroupBox(myContentFrame, "Match Attribute", GUIDesignGroupBoxFrame);
    // Create MatchTagBox for tags
    myMatchTagComboBox = new FXComboBox(matchBox, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTTAG, GUIDesignComboBox);
    // Create listBox for Attributes
    myMatchAttrComboBox = new FXComboBox(matchBox, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTATTRIBUTE, GUIDesignComboBox);
    // Create TextField for Match string
    myMatchString = new FXTextField(matchBox, GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignTextField);
    // Fill list of sub-items
    myElementSet->onCmdSelectElementSet(0, 0, 0);
    // Set speed of edge as default attribute
    myCurrentTag = SUMO_TAG_EDGE;
    myMatchAttrComboBox->setCurrentItem(3);
    myCurrentAttribute = SUMO_ATTR_SPEED;
    // Set default value for Match string
    myMatchString->setText(">10.0");
    // Create help button
    new FXButton(matchBox, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Create Groupbox for visual scalings
    FXGroupBox* selSizeBox = new FXGroupBox(myContentFrame, "Visual Scaling", GUIDesignGroupBoxFrame);
    // Create spin button and configure it
    mySelectionScaling = new FXRealSpinner(selSizeBox, 7, this, MID_GNE_SELECTORFRAME_SELECTSCALE, GUIDesignSpinDial);
    //mySelectionScaling->setNumberFormat(1);
    //mySelectionScaling->setIncrements(0.1, .5, 1);
    mySelectionScaling->setIncrement(0.5);
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
    new FXLabel(selectionHintGroupBox, " - Hold <SHIFT> for \n   rectangle selection.\n - Press <DEL> to\n   delete selected items.", 0, GUIDesignLabelFrameInformation);
}


GNESelectorFrame::~GNESelectorFrame() {}


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
        std::vector<GNEAttributeCarrier*> ACs;
        for(auto i : ids) {
            ACs.push_back(myViewNet->getNet()->retrieveAttributeCarrier(i, false));
        }
        handleIDs(ACs, false);
        if (errors != "") {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox 'error loading selection'");
            }
            // open message box error
            FXMessageBox::error(this, MBOX_OK, "Errors while loading Selection", "%s", errors.c_str());
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox 'error loading selection' with 'OK'");
            }
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
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Opening FXMessageBox 'error storing selection'");
        }
        // open message box error
        FXMessageBox::error(this, MBOX_OK, "Storing Selection failed", "%s", e.what());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox 'error storing selection' with 'OK'");
        }
    }
    return 1;
}


long
GNESelectorFrame::onCmdClear(FXObject*, FXSelector, void*) {
    // for clear selection, simply change all GNE_ATTR_SELECTED attribute of current selected elements
    myViewNet->getUndoList()->p_begin("clear selection");
    std::vector<GNEAttributeCarrier*> selectedAC = myViewNet->getNet()->getSelectedAttributeCarriers();
    // change attribute GNE_ATTR_SELECTED of all selected items to false
    for (auto i : selectedAC) {
        i->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
    }
    // finish clear selection
    myViewNet->getUndoList()->p_end();
    // update view
    myViewNet->update();
    return 1;
}


long
GNESelectorFrame::onCmdInvert(FXObject*, FXSelector, void*) {
    // first make a copy of current selected elements
    std::vector<GNEAttributeCarrier*> copyOfSelectedAC = myViewNet->getNet()->getSelectedAttributeCarriers();
    // for invert selection, first clean current selection and next select elements of set "unselectedElements"
    myViewNet->getUndoList()->p_begin("invert selection");
    // select junctions, edges, lanes connections and crossings
    std::vector<GNEJunction*> junctions = myViewNet->getNet()->retrieveJunctions();
    for (auto i : junctions) {
        i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
        // due we iterate over all junctions, only it's neccesary iterate over incoming edges
        for (auto j : i->getGNEIncomingEdges()) {
            // only select edges if "select edges" flag is enabled. In other case, select only lanes
            if(myViewNet->selectEdges()) {
                j->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            } else {
                for (auto k : j->getLanes()) {
                    k->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
                }
            }
            // select connections
            for (auto k : j->getGNEConnections()) {
                k->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
        }
        // select crossings
        for (auto j : i->getGNECrossings()) {
            j->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
        }
    }
    // select additionals
    std::vector<GNEAdditional*> additionals = myViewNet->getNet()->getAdditionals();
    for (auto i : additionals) {
        i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
    }
    // select polygons
    for (auto i : myViewNet->getNet()->getPolygons()) {
        dynamic_cast<GNEPoly*>(i.second)->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
    }
    // select POIs
    for (auto i : myViewNet->getNet()->getPOIs()) {
        dynamic_cast<GNEPOI*>(i.second)->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
    }
    // now iterate over all elements of "copyOfSelectedAC" and undselect it 
    for (auto i : copyOfSelectedAC) {
        i->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
    }
    // finish selection operation
    myViewNet->getUndoList()->p_end();
    // update view
    myViewNet->update();
    return 1;
}


long
GNESelectorFrame::onCmdSelMBTag(FXObject*, FXSelector, void*) {
    // First check what type of elementes is being selected
    myCurrentTag = SUMO_TAG_NOTHING;
    // find current element tag
    if (myElementSet->getElementSet() == ElementSet::ELEMENTSET_NETELEMENT) {
        for (auto i : GNEAttributeCarrier::allowedNetElementsTags()) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else if (myElementSet->getElementSet() == ElementSet::ELEMENTSET_ADDITIONAL) {
        for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else if (myElementSet->getElementSet() == ElementSet::ELEMENTSET_SHAPE) {
        for (auto i : GNEAttributeCarrier::allowedShapeTags()) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else {
        throw ProcessError("Unkown set");
    }
    // check that typed-by-user value is correct
    if (myCurrentTag != SUMO_TAG_NOTHING) {
        // set color and enable items
        myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchAttrComboBox->enable();
        myMatchString->enable();
        myMatchAttrComboBox->clearItems();
        // fill attribute combo box
        for (auto it : GNEAttributeCarrier::allowedAttributes(myCurrentTag)) {
            myMatchAttrComboBox->appendItem(toString(it.first).c_str());
        }
        // check if item can block movement
        if(GNEAttributeCarrier::canBlockMovement(myCurrentTag)) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_BLOCK_MOVEMENT).c_str());
        }
        // check if item can block shape
        if(GNEAttributeCarrier::canBlockShape(myCurrentTag)) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_BLOCK_SHAPE).c_str());
        }
        // check if item can close shape
        if(GNEAttributeCarrier::canCloseShape(myCurrentTag)) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_CLOSE_SHAPE).c_str());
        }
        // check if item can have parent
        if(GNEAttributeCarrier::canHaveParent(myCurrentTag)) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_PARENT).c_str());
        }
        // @ToDo: Here can be placed a button to set the default value
        myMatchAttrComboBox->setNumVisible(myMatchAttrComboBox->getNumItems());
        onCmdSelMBAttribute(0, 0, 0);
    } else {
        // change color to red and disable items
        myMatchTagComboBox->setTextColor(FXRGB(255, 0, 0));
        myMatchAttrComboBox->disable();
        myMatchString->disable();
    }
    update();
    return 1;
}


long
GNESelectorFrame::onCmdSelMBAttribute(FXObject*, FXSelector, void*) {
    // first obtain all item attributes vinculated with current tag
    std::vector<std::pair <SumoXMLAttr, std::string> > itemAttrs = GNEAttributeCarrier::allowedAttributes(myCurrentTag);
    // add extra attribute if item can block movement
    if(GNEAttributeCarrier::canBlockMovement(myCurrentTag)) {
        itemAttrs.push_back(std::pair<SumoXMLAttr, std::string>(GNE_ATTR_BLOCK_MOVEMENT, "false"));
    }
    // add extra attribute if item can block shape
    if(GNEAttributeCarrier::canBlockShape(myCurrentTag)) {
        itemAttrs.push_back(std::pair<SumoXMLAttr, std::string>(GNE_ATTR_BLOCK_SHAPE, "false"));
    }
    // add extra attribute if item can close shape
    if(GNEAttributeCarrier::canCloseShape(myCurrentTag)) {
        itemAttrs.push_back(std::pair<SumoXMLAttr, std::string>(GNE_ATTR_CLOSE_SHAPE, "true"));
    }
    // add extra attribute if item can have parent
    if(GNEAttributeCarrier::canHaveParent(myCurrentTag)) {
        itemAttrs.push_back(std::pair<SumoXMLAttr, std::string>(GNE_ATTR_PARENT, ""));
    }
    // set current selected attribute
    myCurrentAttribute = SUMO_ATTR_NOTHING;
    for (auto i : itemAttrs) {
        if (toString(i.first) == myMatchAttrComboBox->getText().text()) {
            myCurrentAttribute = i.first;
        }
    }
    // check if selected attribute is valid
    if (myCurrentAttribute != SUMO_ATTR_NOTHING) {
        myMatchAttrComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchString->enable();
    } else {
        myMatchAttrComboBox->setTextColor(FXRGB(255, 0, 0));
        myMatchString->disable();
    }
    return 1;
}


long
GNESelectorFrame::onCmdSelMBString(FXObject*, FXSelector, void*) {
    // obtain expresion
    std::string expr(myMatchString->getText().text());
    bool valid = true;
    if (expr == "") {
        // the empty expression matches all objects
        handleIDs(getMatches(myCurrentTag, myCurrentAttribute, '@', 0, expr), myViewNet->selectEdges());
    } else if (GNEAttributeCarrier::isNumerical(myCurrentTag, myCurrentAttribute)) {
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
            handleIDs(getMatches(myCurrentTag, myCurrentAttribute, compOp, GNEAttributeCarrier::parse<double>(expr.c_str()), expr), myViewNet->selectEdges());
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
        handleIDs(getMatches(myCurrentTag, myCurrentAttribute, compOp, 0, expr), false);
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
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
        << "- The 'Match Attribute' controls allow to specify a set of objects which are then applied to the current selection\n"
        << "  according to the current 'Modification Mode'.\n"
        << "     1. Select an object type from the first input box\n"
        << "     2. Select an attribute from the second input box\n"
        << "     3. Enter a 'match expression' in the third input box and press <return>\n"
        << "\n"
        << "- The empty expression matches all objects\n"
        << "- For numerical attributes the match expression must consist of a comparison operator ('<', '>', '=') and a number.\n"
        << "- An object matches if the comparison between its attribute and the given number by the given operator evaluates to 'true'\n"
        << "\n"
        << "- For string attributes the match expression must consist of a comparison operator ('', '=', '!', '^') and a string.\n"
        << "     '' (no operator) matches if string is a substring of that object'ts attribute.\n"
        << "     '=' matches if string is an exact match.\n"
        << "     '!' matches if string is not a substring.\n"
        << "     '^' matches if string is not an exact match.\n"
        << "\n"
        << "- Examples:\n"
        << "     junction; id; 'foo' -> match all junctions that have 'foo' in their id\n"
        << "     junction; type; '=priority' -> match all junctions of type 'priority', but not of type 'priority_stop'\n"
        << "     edge; speed; '>10' -> match all edges with a speed above 10\n";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Opening help dialog of selector frame");
    }
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Close help dialog of selector frame");
    }
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
    // update label
    selectionUpdated();
    // Show frame
    GNEFrame::show();
}


void
GNESelectorFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


void
GNESelectorFrame::selectionUpdated() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Current selection: " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_JUNCTION).size()) + " Junctions, " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_EDGE).size()) + " Edges, " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_LANE).size()) + " Lanes, " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_CONNECTION).size()) + " connections, " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_ADDITIONAL).size()) + " Additionals, " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_CROSSING).size()) + " Crossings, " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_POLYGON).size()) + " Polygons, " +
                      toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_POI).size()) + " POIs");
    }
    // update labels
    myTypeEntries[GLO_JUNCTION].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_JUNCTION).size()).c_str());
    myTypeEntries[GLO_EDGE].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_EDGE).size()).c_str());
    myTypeEntries[GLO_LANE].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_LANE).size()).c_str());
    myTypeEntries[GLO_CONNECTION].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_CONNECTION).size()).c_str());
    myTypeEntries[GLO_ADDITIONAL].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_ADDITIONAL).size()).c_str());
    myTypeEntries[GLO_CROSSING].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_CROSSING).size()).c_str());
    myTypeEntries[GLO_POLYGON].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_POLYGON).size()).c_str());
    myTypeEntries[GLO_POI].count->setText(toString(myViewNet->getNet()->getSelectedAttributeCarriers(GLO_POI).size()).c_str());
    update();
}


void
GNESelectorFrame::handleIDs(std::vector<GNEAttributeCarrier*> ACs, bool selectEdgesEnabled, ModificationMode::SetOperation setop) {
    const ModificationMode::SetOperation setOperation = ((setop == ModificationMode::SET_DEFAULT) ? myModificationMode->getModificationMode() : setop);
    // first save previous selection
    std::vector<GNEAttributeCarrier*> previousACSelection = myViewNet->getNet()->getSelectedAttributeCarriers();
    // start change selection operation
    myViewNet->getUndoList()->p_begin("change selection");
    // if we're replacing or restricting, we need to unselect all previous selected attribute carrier
    if ((setOperation == ModificationMode::SET_REPLACE) || (setOperation == ModificationMode::SET_RESTRICT)) {
        // change attribute GNE_ATTR_SELECTED of all selected items to false
        for (auto i : previousACSelection) {
            i->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
        }
    }
    // declare set to keep ACs to select and undselect
    std::vector<GNEAttributeCarrier*> selectACs;
    std::vector<GNEAttributeCarrier*> unselectACs;
    // handle ids
    for (auto i : ACs) {
        // doing the switch outside the loop requires functional techniques. this was deemed to ugly
        switch (setOperation) {
            case GNESelectorFrame::ModificationMode::SET_ADD:
            case GNESelectorFrame::ModificationMode::SET_REPLACE:
                selectACs.push_back(i);
                break;
            case GNESelectorFrame::ModificationMode::SET_SUB:
                unselectACs.push_back(i);
                break;
            case GNESelectorFrame::ModificationMode::SET_RESTRICT:
                if (std::find(previousACSelection.begin(), previousACSelection.end(), i) != previousACSelection.end()) {
                    selectACs.push_back(i);
                } else {
                    unselectACs.push_back(i);
                }
                break;
            default:
                break;
        }
    }
    // change attribute selection of selectACs and unselectACs
    for (auto i : selectACs) {
        i->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
    }
    for (auto i : unselectACs) {
        i->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
    }
    // finish operation
    myViewNet->getUndoList()->p_end();
    // update view
    myViewNet->update();
}


std::vector<GNEAttributeCarrier*>
GNESelectorFrame::getMatches(SumoXMLTag ACTag, SumoXMLAttr ACAttr, char compOp, double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    std::vector<GNEAttributeCarrier*> allACbyTag = myViewNet->getNet()->retrieveAttributeCarriers(ACTag);
    const bool numerical = GNEAttributeCarrier::isNumerical(ACTag, ACAttr);
    for (auto it : allACbyTag) {
        if (expr == "") {
            result.push_back(it);
        } else if (numerical) {
            double acVal;
            std::istringstream buf(it->getAttribute(ACAttr));
            buf >> acVal;
            switch (compOp) {
                case '<':
                    if (acVal < val) {
                        result.push_back(it);
                    }
                    break;
                case '>':
                    if (acVal > val) {
                        result.push_back(it);
                    }
                    break;
                case '=':
                    if (acVal == val) {
                        result.push_back(it);
                    }
                    break;
            }
        } else {
            // string match
            std::string acVal = it->getAttributeForSelection(ACAttr);
            switch (compOp) {
                case '@':
                    if (acVal.find(expr) != std::string::npos) {
                        result.push_back(it);
                    }
                    break;
                case '!':
                    if (acVal.find(expr) == std::string::npos) {
                        result.push_back(it);
                    }
                    break;
                case '=':
                    if (acVal == expr) {
                        result.push_back(it);
                    }
                    break;
                case '^':
                    if (acVal != expr) {
                        result.push_back(it);
                    }
                    break;
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// ModificationMode::ModificationMode - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::ModificationMode::ModificationMode(GNESelectorFrame *selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Modification Mode", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent),
    myModificationModeType(SET_ADD) {
    // Create all options buttons
    myAddRadioButton = new FXRadioButton(this, "add\t\tSelected objects are added to the previous selection",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myRemoveRadioButton = new FXRadioButton(this, "remove\t\tSelected objects are removed from the previous selection",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myKeepRadioButton = new FXRadioButton(this, "keep\t\tRestrict previous selection by the current selection",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myReplaceRadioButton = new FXRadioButton(this, "replace\t\tReplace previous selection by the current selection",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myAddRadioButton->setCheck(true);
}


GNESelectorFrame::ModificationMode::~ModificationMode() {}


GNESelectorFrame::ModificationMode::SetOperation 
GNESelectorFrame::ModificationMode::getModificationMode() const {
    return myModificationModeType;
}


long
GNESelectorFrame::ModificationMode::onCmdSelectModificationMode(FXObject* obj, FXSelector, void*) {
    if (obj == myAddRadioButton) {
        myModificationModeType = SET_ADD;
        myAddRadioButton->setCheck(true);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myRemoveRadioButton) {
        myModificationModeType = SET_SUB;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(true);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myKeepRadioButton) {
        myModificationModeType = SET_RESTRICT;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(true);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myReplaceRadioButton) {
        myModificationModeType = SET_REPLACE;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(true);
        return 1;
    } else {
        return 0;
    }
}

// ---------------------------------------------------------------------------
// ModificationMode::MachTag - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::ElementSet::ElementSet(GNESelectorFrame *selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Element Set", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent),
    myCurrentElementSet(ELEMENTSET_NETELEMENT) {
    // Create MatchTagBox for tags and fill it
    mySetComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_CHOOSEN_ELEMENTS, GUIDesignComboBox);
    mySetComboBox->appendItem("Net Element");
    mySetComboBox->appendItem("Additional");
    mySetComboBox->appendItem("Shape");
    mySetComboBox->setNumVisible(mySetComboBox->getNumItems());
}


GNESelectorFrame::ElementSet::~ElementSet() {}


GNESelectorFrame::ElementSet::ElementSetType
GNESelectorFrame::ElementSet::getElementSet() const {
    return myCurrentElementSet;
}


long
GNESelectorFrame::ElementSet::onCmdSelectElementSet(FXObject*, FXSelector, void*) {
    if (mySetComboBox->getText() == "Net Element") {
        myCurrentElementSet = ELEMENTSET_NETELEMENT;
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        mySelectorFrameParent->myMatchTagComboBox->enable();
        mySelectorFrameParent->myMatchAttrComboBox->enable();
        mySelectorFrameParent->myMatchString->enable();
        // Clear items of myMatchTagComboBox
        mySelectorFrameParent->myMatchTagComboBox->clearItems();
        // Set items depending of current items
        for (auto i : GNEAttributeCarrier::allowedNetElementsTags()) {
            mySelectorFrameParent->myMatchTagComboBox->appendItem(toString(i).c_str());
        }
        mySelectorFrameParent->myMatchTagComboBox->setCurrentItem(0); // edges
        mySelectorFrameParent->myMatchTagComboBox->setNumVisible(mySelectorFrameParent->myMatchTagComboBox->getNumItems());
        // Fill attributes with the current element type
        mySelectorFrameParent->onCmdSelMBTag(0, 0, 0);
    } else if (mySetComboBox->getText() == "Additional") {
        myCurrentElementSet = ELEMENTSET_ADDITIONAL;
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        mySelectorFrameParent->myMatchTagComboBox->enable();
        mySelectorFrameParent->myMatchAttrComboBox->enable();
        mySelectorFrameParent->myMatchString->enable();
        // Clear items of myMatchTagComboBox
        mySelectorFrameParent->myMatchTagComboBox->clearItems();
        // Set items depending of current items
        for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
            mySelectorFrameParent->myMatchTagComboBox->appendItem(toString(i).c_str());
        }
        mySelectorFrameParent->myMatchTagComboBox->setCurrentItem(0); // edges
        mySelectorFrameParent->myMatchTagComboBox->setNumVisible(mySelectorFrameParent->myMatchTagComboBox->getNumItems());
        // Fill attributes with the current element type
        mySelectorFrameParent->onCmdSelMBTag(0, 0, 0);
    } else if (mySetComboBox->getText() == "Shape") {
        myCurrentElementSet = ELEMENTSET_SHAPE;
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        mySelectorFrameParent->myMatchTagComboBox->enable();
        mySelectorFrameParent->myMatchAttrComboBox->enable();
        mySelectorFrameParent->myMatchString->enable();
        // Clear items of myMatchTagComboBox
        mySelectorFrameParent->myMatchTagComboBox->clearItems();
        // Set items depending of current items
        for (auto i : GNEAttributeCarrier::allowedShapeTags()) {
            mySelectorFrameParent->myMatchTagComboBox->appendItem(toString(i).c_str());
        }
        mySelectorFrameParent->myMatchTagComboBox->setCurrentItem(0); // edges
        mySelectorFrameParent->myMatchTagComboBox->setNumVisible(mySelectorFrameParent->myMatchTagComboBox->getNumItems());
        // Fill attributes with the current element type
        mySelectorFrameParent->onCmdSelMBTag(0, 0, 0);
    } else {
        myCurrentElementSet = ELEMENTSET_INVALID;
        mySetComboBox->setTextColor(FXRGB(255, 0, 0));
        mySelectorFrameParent->myMatchTagComboBox->disable();
        mySelectorFrameParent->myMatchAttrComboBox->disable();
        mySelectorFrameParent->myMatchString->disable();
    }
    return 1;
}


/****************************************************************************/
