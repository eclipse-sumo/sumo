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
#include "GNEPoly.h"
#include "GNEPOI.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNESelectorFrame) GNESelectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,      GNESelectorFrame::onCmdSelectOperation),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_ELEMENTS,       GNESelectorFrame::onCmdSubset),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,           GNESelectorFrame::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,           GNESelectorFrame::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT,         GNESelectorFrame::onCmdInvert),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,          GNESelectorFrame::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_TAG,          GNESelectorFrame::onCmdSelMBTag),
    FXMAPFUNC(SEL_COMMAND,  MID_GME_SELMB_ATTRIBUTE,    GNESelectorFrame::onCmdSelMBAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_STRING,       GNESelectorFrame::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                   GNESelectorFrame::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_SCALE,       GNESelectorFrame::onCmdScaleSelection)
};

// Object implementation
FXIMPLEMENT(GNESelectorFrame, FXVerticalFrame, GNESelectorFrameMap, ARRAYNUMBER(GNESelectorFrameMap))

// ===========================================================================
// method definitions
// ===========================================================================
GNESelectorFrame::GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Selection"),
    mySetOperation(SET_ADD),
    myCurrentTag(SUMO_TAG_NOTHING),
    myCurrentAttribute(SUMO_ATTR_NOTHING) {
    // create combo box and label for selected items
    FXGroupBox* mySelectedItemsComboBox = new FXGroupBox(myContentFrame, "Selected items", GUIDesignGroupBoxFrame);
    mySelectedItems = new FXLabel(mySelectedItemsComboBox, "", 0, GUIDesignLabelLeft);;
    // create combo box for selection modification mode
    FXGroupBox* selBox = new FXGroupBox(myContentFrame, "Modification Mode", GUIDesignGroupBoxFrame);
    // Create all options buttons
    myAddRadioButton = new FXRadioButton(selBox, "add\t\tSelected objects are added to the previous selection",
                                         this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myRemoveRadioButton = new FXRadioButton(selBox, "remove\t\tSelected objects are removed from the previous selection",
                                            this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myKeepRadioButton = new FXRadioButton(selBox, "keep\t\tRestrict previous selection by the current selection",
                                          this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myReplaceRadioButton = new FXRadioButton(selBox, "replace\t\tReplace previous selection by the current selection",
                                             this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myAddRadioButton->setCheck(true);
    // Create groupBox for selection by expression matching (match box)
    FXGroupBox* elementBox = new FXGroupBox(myContentFrame, "type of element", GUIDesignGroupBoxFrame);
    // Create MatchTagBox for tags and fill it
    mySetComboBox = new FXComboBox(elementBox, GUIDesignComboBoxNCol, this, MID_CHOOSEN_ELEMENTS, GUIDesignComboBox);
    mySetComboBox->appendItem("Net Element");
    mySetComboBox->appendItem("Additional");
    mySetComboBox->appendItem("Shape");
    mySetComboBox->setNumVisible(mySetComboBox->getNumItems());
    // Create groupBox fro selection by expression matching (match box)
    FXGroupBox* matchBox = new FXGroupBox(myContentFrame, "Match Attribute", GUIDesignGroupBoxFrame);
    // Create MatchTagBox for tags
    myMatchTagComboBox = new FXComboBox(matchBox, GUIDesignComboBoxNCol, this, MID_GNE_SELMB_TAG, GUIDesignComboBox);
    // Create listBox for Attributes
    myMatchAttrComboBox = new FXComboBox(matchBox, GUIDesignComboBoxNCol, this, MID_GME_SELMB_ATTRIBUTE, GUIDesignComboBox);
    // Create TextField for Match string
    myMatchString = new FXTextField(matchBox, GUIDesignTextFieldNCol, this, MID_GNE_SELMB_STRING, GUIDesignTextField);
    // Fill list of sub-items
    onCmdSubset(0, 0, 0);
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
GNESelectorFrame::onCmdSelectOperation(FXObject* obj, FXSelector, void*) {
    if (obj == myAddRadioButton) {
        mySetOperation = SET_ADD;
        myAddRadioButton->setCheck(true);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myRemoveRadioButton) {
        mySetOperation = SET_SUB;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(true);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myKeepRadioButton) {
        mySetOperation = SET_RESTRICT;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(true);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myReplaceRadioButton) {
        mySetOperation = SET_REPLACE;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(true);
        return 1;
    } else {
        return 0;
    }
}


long
GNESelectorFrame::onCmdSubset(FXObject*, FXSelector, void*) {
    if (mySetComboBox->getText() == "Net Element") {
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchTagComboBox->enable();
        myMatchAttrComboBox->enable();
        myMatchString->enable();
        // Clear items of myMatchTagComboBox
        myMatchTagComboBox->clearItems();
        // Set items depending of current items
        for (auto i : GNEAttributeCarrier::allowedNetElementsTags()) {
            myMatchTagComboBox->appendItem(toString(i).c_str());
        }
        myMatchTagComboBox->setCurrentItem(0); // edges
        myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
        // Fill attributes with the current element type
        onCmdSelMBTag(0, 0, 0);
    } else if (mySetComboBox->getText() == "Additional") {
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchTagComboBox->enable();
        myMatchAttrComboBox->enable();
        myMatchString->enable();
        // Clear items of myMatchTagComboBox
        myMatchTagComboBox->clearItems();
        // Set items depending of current items
        for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
            myMatchTagComboBox->appendItem(toString(i).c_str());
        }
        myMatchTagComboBox->setCurrentItem(0); // edges
        myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
        // Fill attributes with the current element type
        onCmdSelMBTag(0, 0, 0);
    } else if (mySetComboBox->getText() == "Shape") {
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchTagComboBox->enable();
        myMatchAttrComboBox->enable();
        myMatchString->enable();
        // Clear items of myMatchTagComboBox
        myMatchTagComboBox->clearItems();
        // Set items depending of current items
        for (auto i : GNEAttributeCarrier::allowedShapeTags()) {
            myMatchTagComboBox->appendItem(toString(i).c_str());
        }
        myMatchTagComboBox->setCurrentItem(0); // edges
        myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
        // Fill attributes with the current element type
        onCmdSelMBTag(0, 0, 0);
    } else {
        mySetComboBox->setTextColor(FXRGB(255, 0, 0));
        myMatchTagComboBox->disable();
        myMatchAttrComboBox->disable();
        myMatchString->disable();
    }
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
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox of type 'error'");
            }
            // open message box error
            FXMessageBox::error(this, MBOX_OK, "Errors while loading Selection", "%s", errors.c_str());
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox of type 'error' with 'OK'");
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
            WRITE_WARNING("Opening FXMessageBox of type 'error'");
        }
        // open message box error
        FXMessageBox::error(this, MBOX_OK, "Storing Selection failed", "%s", e.what());
        // write warning if netedit is running in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Closed FXMessageBox of type 'error' with 'OK'");
        }
    }
    return 1;
}


long
GNESelectorFrame::onCmdClear(FXObject*, FXSelector, void*) {
    myViewNet->getUndoList()->p_begin("clear selection");
    myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    myViewNet->getUndoList()->p_end();
    myViewNet->update();
    return 1;
}


long
GNESelectorFrame::onCmdInvert(FXObject*, FXSelector, void*) {
    // declare a set to save unselected elements
    std::set<GUIGlID> unselectedIDs;
    // iterate over all junctions to obtain unselected
    std::set<GUIGlID> ids = myViewNet->getNet()->getGlIDs(GLO_JUNCTION);
    for (auto it : ids) {
        if(gSelected.isSelected(GLO_JUNCTION, it) == false) {
            unselectedIDs.insert(it);
        }
        
    }
    // iterate over all edges or lanes (Depending of checkbox) to obtain unselected
    GUIGlObjectType currentEdgeOrLane = myViewNet->selectEdges() ? GLO_EDGE : GLO_LANE;
    ids = myViewNet->getNet()->getGlIDs(currentEdgeOrLane);
    for (auto it : ids) {
        if (gSelected.isSelected(currentEdgeOrLane, it) == false) {
            unselectedIDs.insert(it);
        }

    }
    // iterate over all additionals to obtain unselected
    ids = myViewNet->getNet()->getGlIDs(GLO_ADDITIONAL);
    for (auto it : ids) {
        if (gSelected.isSelected(GLO_ADDITIONAL, it) == false) {
            unselectedIDs.insert(it);
        }
    }
    // iterate over all connections to obtain unselected
    ids = myViewNet->getNet()->getGlIDs(GLO_CONNECTION);
    for (auto it : ids) {
        if (gSelected.isSelected(GLO_CONNECTION, it) == false) {
            unselectedIDs.insert(it);
        }
    }
    // iterate over all crossings to obtain unselected
    ids = myViewNet->getNet()->getGlIDs(GLO_CROSSING);
    for (auto it : ids) {
        if (gSelected.isSelected(GLO_CROSSING, it) == false) {
            unselectedIDs.insert(it);
        }
    }
    // iterate over all polygons to obtain unselected
    for (auto it : myViewNet->getNet()->getPolygons().getMyMap()) {
        GNEPoly* poly = dynamic_cast<GNEPoly*>(it.second);
        if (gSelected.isSelected(GLO_CROSSING, poly->getGlID()) == false) {
            unselectedIDs.insert(poly->getGlID());
        }
    }
    // iterate over all POIgons to obtain unselected
    for (auto it : myViewNet->getNet()->getPOIs().getMyMap()) {
        GNEPOI* POI = dynamic_cast<GNEPOI*>(it.second);
        if (gSelected.isSelected(GLO_CROSSING, POI->getGlID()) == false) {
            unselectedIDs.insert(POI->getGlID());
        }
    }
    // invert selection cleaning current selection and selecting set "unselectedIDs"
    myViewNet->getUndoList()->p_begin("invert selection");
    myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), std::set<GUIGlID>(), gSelected.getSelected(), true), true);
    myViewNet->getUndoList()->add(new GNEChange_Selection(myViewNet->getNet(), unselectedIDs, std::set<GUIGlID>(), true), true);
    myViewNet->getUndoList()->p_end();
    myViewNet->update();
    return 1;
}


long
GNESelectorFrame::onCmdSelMBTag(FXObject*, FXSelector, void*) {
    myCurrentTag = SUMO_TAG_NOTHING;
    // find current element tag
    if(mySetComboBox->getText() == "Net Element") {
        for (auto i : GNEAttributeCarrier::allowedNetElementsTags()) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else if(mySetComboBox->getText() == "Additional") {
        for (auto i : GNEAttributeCarrier::allowedAdditionalTags()) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else if(mySetComboBox->getText() == "Shape") {
        for (auto i : GNEAttributeCarrier::allowedShapeTags()) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else {
        throw ProcessError("Unkown set");
    }

    // check that typed by user value is correct
    if (myCurrentTag != SUMO_TAG_NOTHING) {
        // set color and enable items
        myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchAttrComboBox->enable();
        myMatchString->enable();
        myMatchAttrComboBox->clearItems();
        const std::vector<std::pair <SumoXMLAttr, std::string> >& attrs = GNEAttributeCarrier::allowedAttributes(myCurrentTag);
        for (std::vector<std::pair <SumoXMLAttr, std::string> >::const_iterator it = attrs.begin(); it != attrs.end(); it++) {
            myMatchAttrComboBox->appendItem(toString(it->first).c_str());
        }
        // @ToDo: Here can be placed a butto to set the default value
        myMatchAttrComboBox->setNumVisible(myMatchAttrComboBox->getNumItems());
        onCmdSelMBAttribute(0,0,0);
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
    const std::vector<std::pair <SumoXMLAttr, std::string> >& attrs = GNEAttributeCarrier::allowedAttributes(myCurrentTag);
    myCurrentAttribute = SUMO_ATTR_NOTHING;
    for (auto i : attrs) {
        if (toString(i.first) == myMatchAttrComboBox->getText().text()) {
            myCurrentAttribute = i.first;
        }
    }
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
    std::string expr(myMatchString->getText().text());
    bool valid = true;
    if (expr == "") {
        // the empty expression matches all objects
        handleIDs(getMatches(myCurrentTag, myCurrentAttribute, '@', 0, expr), false);
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
            handleIDs(getMatches(myCurrentTag, myCurrentAttribute, compOp, GNEAttributeCarrier::parse<double>(expr.c_str()), expr), false);
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
    new FXButton(helpDialog, "OK\t\tSave modifications", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
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

void
GNESelectorFrame::selectionUpdated() {
    // show extra information for tests
    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
        WRITE_WARNING("Current selection: " + 
            toString(gSelected.getSelected(GLO_JUNCTION).size()) + " Junctions, " +
            toString(gSelected.getSelected(GLO_EDGE).size()) + " Edges, " +
            toString(gSelected.getSelected(GLO_LANE).size()) + " Lanes, " +
            toString(gSelected.getSelected(GLO_CONNECTION).size()) + " connections, " +
            toString(gSelected.getSelected(GLO_ADDITIONAL).size()) + " Additionals, " +
            toString(gSelected.getSelected(GLO_CROSSING).size()) + " Crossings, " +
            toString(gSelected.getSelected(GLO_POLYGON).size()) + " Polygons, " +
            toString(gSelected.getSelected(GLO_POI).size()) + " POIs");
    }
    // update mySelectedItems label
    std::ostringstream selection;
    selection
        << toString(gSelected.getSelected(GLO_JUNCTION).size()) + " Junctions\n"
        << toString(gSelected.getSelected(GLO_EDGE).size()) + " Edges\n"
        << toString(gSelected.getSelected(GLO_LANE).size()) + " Lanes\n"
        << toString(gSelected.getSelected(GLO_CONNECTION).size()) + " Connections\n"
        << toString(gSelected.getSelected(GLO_ADDITIONAL).size()) + " Additionals\n"
        << toString(gSelected.getSelected(GLO_CROSSING).size()) + " Crossings\n"
        << toString(gSelected.getSelected(GLO_POLYGON).size()) + " Polygons\n"
        << toString(gSelected.getSelected(GLO_POI).size()) + " POIs";
    mySelectedItems->setText(selection.str().c_str());
    update();
}


void
GNESelectorFrame::handleIDs(std::vector<GUIGlID> ids, bool selectEdgesEnabled, SetOperation setop) {
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
        for (auto it : ids) {
            if (it > 0) { // net object?
                object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(it);
                if ((object->getType() == GLO_LANE) && selectEdgesEnabled) {
                    const GNEEdge& edge = (static_cast<GNELane*>(object))->getParentEdge();
                    idsSet.insert(edge.getGNEJunctionSource()->getGlID());
                    idsSet.insert(edge.getGNEJunctionDestiny()->getGlID());
                }
                GUIGlObjectStorage::gIDStorage.unblockObject(it);
            }
        }
    }
    for (auto it : idsSet) {
        if (it > 0) { // net object?
            object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(it);
            if (object == 0) {
                // in debug mode we would like to know about this.
                // It might be caused by a corrupted gl-name stack.
                // However, most cases of uninitizliaed values would go hidden since 0 is assumed to be the net object anyway
                assert(false);
                continue;
            }
            type = object->getType();
            GUIGlObjectStorage::gIDStorage.unblockObject(it);
            if ((type == GLO_LANE) && selectEdgesEnabled) {
                // @note edge may be selected/deselected multiple times but this shouldn't
                // hurt unless we add SET_TOGGLE
                it = (static_cast<GNELane*>(object))->getParentEdge().getGlID();
            }
            // doing the switch outside the loop requires functional techniques. this was deemed to ugly
            switch (setOperation) {
                case GNESelectorFrame::SET_ADD:
                case GNESelectorFrame::SET_REPLACE:
                    selected.insert(it);
                    break;
                case GNESelectorFrame::SET_SUB:
                    deselected.insert(it);
                    break;
                case GNESelectorFrame::SET_RESTRICT:
                    if (previousSelection.count(it)) {
                        selected.insert(it);
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
GNESelectorFrame::getMatches(SumoXMLTag tag, SumoXMLAttr attr, char compOp, double val, const std::string& expr) {
    GUIGlObject* object;
    GNEAttributeCarrier* ac;
    std::vector<GUIGlID> result;
    const std::set<GUIGlID> allIDs = myViewNet->getNet()->getGlIDs();
    const bool numerical = GNEAttributeCarrier::isNumerical(tag, attr);
    for (auto it : allIDs) {
        object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(it);
        if (!object) {
            throw ProcessError("Unkown object passed to GNESelectorFrame::getMatches (id=" + toString(it) + ").");
        }
        ac = dynamic_cast<GNEAttributeCarrier*>(object);
        if (ac && ac->getTag() == tag) { // not all objects need to be attribute carriers
            if (expr == "") {
                result.push_back(it);
            } else if (numerical) {
                double acVal;
                std::istringstream buf(ac->getAttribute(attr));
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
                std::string acVal = ac->getAttributeForSelection(attr);
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
        GUIGlObjectStorage::gIDStorage.unblockObject(it);
    }
    return result;
}

/****************************************************************************/
