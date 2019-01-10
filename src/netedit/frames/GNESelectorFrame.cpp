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
#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
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
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNESelectorFrame::ModificationMode::onCmdSelectModificationMode)
};

FXDEFMAP(GNESelectorFrame::ElementSet) ElementSetMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_ELEMENTS,   GNESelectorFrame::ElementSet::onCmdSelectElementSet)
};

FXDEFMAP(GNESelectorFrame::MatchAttribute) MatchAttributeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTTAG,        GNESelectorFrame::MatchAttribute::onCmdSelMBTag),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,  GNESelectorFrame::MatchAttribute::onCmdSelMBAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_PROCESSSTRING,    GNESelectorFrame::MatchAttribute::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                               GNESelectorFrame::MatchAttribute::onCmdHelp)
};

FXDEFMAP(GNESelectorFrame::VisualScaling) VisualScalingMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTSCALE,      GNESelectorFrame::VisualScaling::onCmdScaleSelection)
};

FXDEFMAP(GNESelectorFrame::SelectionOperation) SelectionOperationMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,   GNESelectorFrame::SelectionOperation::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,   GNESelectorFrame::SelectionOperation::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT, GNESelectorFrame::SelectionOperation::onCmdInvert),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,  GNESelectorFrame::SelectionOperation::onCmdClear)
};

// Object implementation
FXIMPLEMENT(GNESelectorFrame::ModificationMode,     FXGroupBox,     ModificationModeMap,    ARRAYNUMBER(ModificationModeMap))
FXIMPLEMENT(GNESelectorFrame::ElementSet,           FXGroupBox,     ElementSetMap,          ARRAYNUMBER(ElementSetMap))
FXIMPLEMENT(GNESelectorFrame::MatchAttribute,       FXGroupBox,     MatchAttributeMap,      ARRAYNUMBER(MatchAttributeMap))
FXIMPLEMENT(GNESelectorFrame::VisualScaling,        FXGroupBox,     VisualScalingMap,       ARRAYNUMBER(VisualScalingMap))
FXIMPLEMENT(GNESelectorFrame::SelectionOperation,   FXGroupBox,     SelectionOperationMap,  ARRAYNUMBER(SelectionOperationMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNESelectorFrame::GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Selection") {
    // create selectedItems modul
    myLockGLObjectTypes = new LockGLObjectTypes(this);
    // create Modification Mode modul
    myModificationMode = new ModificationMode(this);
    // create ElementSet modul
    myElementSet = new ElementSet(this);
    // create MatchAttribute modul
    myMatchAttribute = new MatchAttribute(this);
    // create VisualScaling modul
    myVisualScaling = new VisualScaling(this);
    // create SelectionOperation modul
    mySelectionOperation = new SelectionOperation(this);
    // Create groupbox for information about selections
    FXGroupBox* selectionHintGroupBox = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    // Create Selection Hint
    new FXLabel(selectionHintGroupBox, " - Hold <SHIFT> for \n   rectangle selection.\n - Press <DEL> to\n   delete selected items.", 0, GUIDesignLabelFrameInformation);
}


GNESelectorFrame::~GNESelectorFrame() {}


void
GNESelectorFrame::show() {
    // update selected items
    myLockGLObjectTypes->updateLockGLObjectTypes();
    // Show frame
    GNEFrame::show();
}


void
GNESelectorFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


GNESelectorFrame::LockGLObjectTypes*
GNESelectorFrame::getLockGLObjectTypes() const {
    return myLockGLObjectTypes;
}


void
GNESelectorFrame::clearCurrentSelection() const {
    // for clear selection, simply change all GNE_ATTR_SELECTED attribute of current selected elements
    myViewNet->getUndoList()->p_begin("clear selection");
    std::vector<GNEAttributeCarrier*> selectedAC = myViewNet->getNet()->getSelectedAttributeCarriers();
    // change attribute GNE_ATTR_SELECTED of all selected items to false
    for (auto i : selectedAC) {
        i->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
    }
    // finish clear selection
    myViewNet->getUndoList()->p_end();
    // update current selected items
    myLockGLObjectTypes->updateLockGLObjectTypes();
    // update view
    myViewNet->update();
}


void
GNESelectorFrame::handleIDs(const std::vector<GNEAttributeCarrier*>& ACs, ModificationMode::SetOperation setop) {
    const ModificationMode::SetOperation setOperation = ((setop == ModificationMode::SET_DEFAULT) ? myModificationMode->getModificationMode() : setop);
    // declare two sets of attribute carriers, one for select and another for unselect
    std::set<std::pair<std::string, GNEAttributeCarrier*> > ACToSelect;
    std::set<std::pair<std::string, GNEAttributeCarrier*> > ACToUnselect;
    // in restrict AND replace mode all current selected attribute carriers will be unselected
    if ((setOperation == ModificationMode::SET_REPLACE) || (setOperation == ModificationMode::SET_RESTRICT)) {
        for (auto i : myViewNet->getNet()->getSelectedAttributeCarriers()) {
            ACToUnselect.insert(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i));
        }
    }
    // handle ids
    for (auto i : ACs) {
        // iterate over AtributeCarriers an place it in ACToSelect or ACToUnselect
        switch (setOperation) {
            case GNESelectorFrame::ModificationMode::SET_SUB:
                ACToUnselect.insert(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i));
                break;
            case GNESelectorFrame::ModificationMode::SET_RESTRICT:
                if (ACToUnselect.find(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i)) != ACToUnselect.end()) {
                    ACToSelect.insert(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i));
                }
                break;
            default:
                ACToSelect.insert(std::pair<std::string, GNEAttributeCarrier*>(i->getID(), i));
                break;
        }
    }
    // select junctions and their connections if Auto select junctions is enabled (note: only for "add mode")
    if (myViewNet->autoSelectNodes() && GNESelectorFrame::ModificationMode::SET_ADD) {
        std::vector<GNEEdge*> edgesToSelect;
        // iterate over ACToSelect and extract edges
        for (auto i : ACToSelect) {
            if (i.second->getTag() == SUMO_TAG_EDGE) {
                edgesToSelect.push_back(dynamic_cast<GNEEdge*>(i.second));
            }
        }
        // iterate over extracted edges
        for (auto i : edgesToSelect) {
            // select junction source and all their connections and crossings
            ACToSelect.insert(std::make_pair(i->getGNEJunctionSource()->getID(), i->getGNEJunctionSource()));
            for (auto j : i->getGNEJunctionSource()->getGNEConnections()) {
                ACToSelect.insert(std::make_pair(j->getID(), j));
            }
            for (auto j : i->getGNEJunctionSource()->getGNECrossings()) {
                ACToSelect.insert(std::make_pair(j->getID(), j));
            }
            // select junction destiny and all their connections crossings
            ACToSelect.insert(std::make_pair(i->getGNEJunctionDestiny()->getID(), i->getGNEJunctionDestiny()));
            for (auto j : i->getGNEJunctionDestiny()->getGNEConnections()) {
                ACToSelect.insert(std::make_pair(j->getID(), j));
            }
            for (auto j : i->getGNEJunctionDestiny()->getGNECrossings()) {
                ACToSelect.insert(std::make_pair(j->getID(), j));
            }
        }
    }
    // only continue if there is ACs to select or unselect
    if ((ACToSelect.size() + ACToUnselect.size()) > 0) {
        // first unselect AC of ACToUnselect and then selects AC of ACToSelect
        myViewNet->getUndoList()->p_begin("selection using rectangle");
        for (auto i : ACToUnselect) {
            if (GNEAttributeCarrier::getTagProperties(i.second->getTag()).isSelectable()) {
                i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
            }
        }
        for (auto i : ACToSelect) {
            if (GNEAttributeCarrier::getTagProperties(i.second->getTag()).isSelectable()) {
                i.second->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
        }
        // finish operation
        myViewNet->getUndoList()->p_end();
    }
    // update current selected items
    myLockGLObjectTypes->updateLockGLObjectTypes();
    // update view
    myViewNet->update();
}


GNESelectorFrame::ModificationMode*
GNESelectorFrame::getModificationModeModul() const {
    return myModificationMode;
}


std::vector<GNEAttributeCarrier*>
GNESelectorFrame::getMatches(SumoXMLTag ACTag, SumoXMLAttr ACAttr, char compOp, double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    std::vector<GNEAttributeCarrier*> allACbyTag = myViewNet->getNet()->retrieveAttributeCarriers(ACTag);
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(ACTag);
    for (auto it : allACbyTag) {
        if (expr == "") {
            result.push_back(it);
        } else if (tagValue.hasAttribute(ACAttr) && tagValue.getAttribute(ACAttr).isNumerical()) {
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
// ModificationMode::LockGLObjectTypes - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::LockGLObjectTypes::LockGLObjectTypes(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Selected items", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent) {
    // create combo box and labels for selected items
    FXMatrix* mLockGLObjectTypes = new FXMatrix(this, 3, (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    // create typeEntries for the different elements
    myTypeEntries[GLO_JUNCTION] = ObjectTypeEntry(mLockGLObjectTypes, "Junctions", "junction");
    myTypeEntries[GLO_EDGE] = ObjectTypeEntry(mLockGLObjectTypes, "Edges", "edge");
    myTypeEntries[GLO_LANE] = ObjectTypeEntry(mLockGLObjectTypes, "Lanes", "lane");
    myTypeEntries[GLO_CONNECTION] = ObjectTypeEntry(mLockGLObjectTypes, "Connections", "connection");
    myTypeEntries[GLO_ADDITIONAL] = ObjectTypeEntry(mLockGLObjectTypes, "Additionals", "additional");
    myTypeEntries[GLO_CROSSING] = ObjectTypeEntry(mLockGLObjectTypes, "Crossings", "crossing");
    myTypeEntries[GLO_POLYGON] = ObjectTypeEntry(mLockGLObjectTypes, "Polygons", "polygon");
    myTypeEntries[GLO_POI] = ObjectTypeEntry(mLockGLObjectTypes, "POIs", "POI");
}


GNESelectorFrame::LockGLObjectTypes::~LockGLObjectTypes() {}


void
GNESelectorFrame::LockGLObjectTypes::updateLockGLObjectTypes() {
    myTypeEntries[GLO_JUNCTION].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveJunctions(true).size()).c_str());
    myTypeEntries[GLO_EDGE].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveEdges(true).size()).c_str());
    myTypeEntries[GLO_LANE].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveLanes(true).size()).c_str());
    myTypeEntries[GLO_CONNECTION].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveConnections(true).size()).c_str());
    myTypeEntries[GLO_ADDITIONAL].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveAdditionals(true).size()).c_str());
    myTypeEntries[GLO_CROSSING].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveCrossings(true).size()).c_str());
    myTypeEntries[GLO_POLYGON].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveShapes(SUMO_TAG_POLY, true).size()).c_str());
    myTypeEntries[GLO_POI].count->setText(toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveShapes(SUMO_TAG_POI, true).size()).c_str());
    // show information in debug mode
    WRITE_DEBUG("Current selection: " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveJunctions(true).size()) + " Junctions, " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveEdges(true).size()) + " Edges, " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveLanes(true).size()) + " Lanes, " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveConnections(true).size()) + " connections, " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveAdditionals(true).size()) + " Additionals, " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveCrossings(true).size()) + " Crossings, " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveShapes(SUMO_TAG_POLY, true).size()) + " Polygons, " +
                toString(mySelectorFrameParent->getViewNet()->getNet()->retrieveShapes(SUMO_TAG_POI, true).size()) + " POIs");
}


bool
GNESelectorFrame::LockGLObjectTypes::IsObjectTypeLocked(GUIGlObjectType type) const {
    if ((type >= 100) && (type < 199)) {
        return myTypeEntries.at(GLO_ADDITIONAL).locked->getCheck() != FALSE;
    } else {
        return myTypeEntries.at(type).locked->getCheck() != FALSE;
    }
}


GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::ObjectTypeEntry(FXMatrix* parent, const std::string& label, const std::string& label2) {
    count = new FXLabel(parent, "0", 0, GUIDesignLabelLeft);
    typeName = new FXLabel(parent, label.c_str(), 0, GUIDesignLabelLeft);
    locked = new FXMenuCheck(parent, ("lock\t\tLock " + label2 + " selection").c_str(), 0, 0, LAYOUT_FILL_X | LAYOUT_RIGHT);
}

// ---------------------------------------------------------------------------
// ModificationMode::ModificationMode - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::ModificationMode::ModificationMode(GNESelectorFrame* selectorFrameParent) :
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
// ModificationMode::ElementSet - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::ElementSet::ElementSet(GNESelectorFrame* selectorFrameParent) :
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
        // enable match attribute
        mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
    } else if (mySetComboBox->getText() == "Additional") {
        myCurrentElementSet = ELEMENTSET_ADDITIONAL;
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        // enable match attribute
        mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
    } else if (mySetComboBox->getText() == "Shape") {
        myCurrentElementSet = ELEMENTSET_SHAPE;
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        // enable match attribute
        mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
    } else {
        myCurrentElementSet = ELEMENTSET_INVALID;
        mySetComboBox->setTextColor(FXRGB(255, 0, 0));
        // disable match attribute
        mySelectorFrameParent->myMatchAttribute->disableMatchAttribute();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// ModificationMode::MatchAttribute - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::MatchAttribute::MatchAttribute(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Match Attribute", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent),
    myCurrentTag(SUMO_TAG_EDGE),
    myCurrentAttribute(SUMO_ATTR_ID) {
    // Create MatchTagBox for tags
    myMatchTagComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTTAG, GUIDesignComboBox);
    // Create listBox for Attributes
    myMatchAttrComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTATTRIBUTE, GUIDesignComboBox);
    // Create TextField for Match string
    myMatchString = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignTextField);
    // Create help button
    new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Fill list of sub-items (first element will be "edge")
    enableMatchAttribute();
    // Set speed of edge as default attribute
    myMatchAttrComboBox->setText("speed");
    myCurrentAttribute = SUMO_ATTR_SPEED;
    // Set default value for Match string
    myMatchString->setText(">10.0");
}


GNESelectorFrame::MatchAttribute::~MatchAttribute() {}


void
GNESelectorFrame::MatchAttribute::enableMatchAttribute() {
    // enable comboboxes and text field
    myMatchTagComboBox->enable();
    myMatchAttrComboBox->enable();
    myMatchString->enable();
    // Clear items of myMatchTagComboBox
    myMatchTagComboBox->clearItems();
    // Set items depending of current item set
    if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_NETELEMENT) {
        auto listOfTags = GNEAttributeCarrier::allowedNetElementsTags(true);
        for (auto i : listOfTags) {
            myMatchTagComboBox->appendItem(toString(i).c_str());
        }
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_ADDITIONAL) {
        auto listOfTags = GNEAttributeCarrier::allowedAdditionalTags(true);
        for (auto i : listOfTags) {
            myMatchTagComboBox->appendItem(toString(i).c_str());
        }
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_SHAPE) {
        auto listOfTags = GNEAttributeCarrier::allowedShapeTags(true);
        for (auto i : listOfTags) {
            myMatchTagComboBox->appendItem(toString(i).c_str());
        }
    } else {
        throw ProcessError("Invalid element set");
    }
    // set first item as current item
    myMatchTagComboBox->setCurrentItem(0);
    myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
    // Fill attributes with the current element type
    onCmdSelMBTag(0, 0, 0);
}


void
GNESelectorFrame::MatchAttribute::disableMatchAttribute() {
    // disable comboboxes and text field
    myMatchTagComboBox->disable();
    myMatchAttrComboBox->disable();
    myMatchString->disable();
    // change colors to black (even if there are invalid values)
    myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
    myMatchAttrComboBox->setTextColor(FXRGB(0, 0, 0));
    myMatchString->setTextColor(FXRGB(0, 0, 0));
}


long
GNESelectorFrame::MatchAttribute::onCmdSelMBTag(FXObject*, FXSelector, void*) {
    // First check what type of elementes is being selected
    myCurrentTag = SUMO_TAG_NOTHING;
    // find current element tag
    if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_NETELEMENT) {
        auto listOfTags = GNEAttributeCarrier::allowedNetElementsTags(true);
        for (auto i : listOfTags) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_ADDITIONAL) {
        auto listOfTags = GNEAttributeCarrier::allowedAdditionalTags(true);
        for (auto i : listOfTags) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_SHAPE) {
        auto listOfTags = GNEAttributeCarrier::allowedShapeTags(true);
        for (auto i : listOfTags) {
            if (toString(i) == myMatchTagComboBox->getText().text()) {
                myCurrentTag = i;
            }
        }
    } else {
        throw ProcessError("Unkown set");
    }
    // check that typed-by-user value is correct
    if (myCurrentTag != SUMO_TAG_NOTHING) {
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentTag);
        // set color and enable items
        myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchAttrComboBox->enable();
        myMatchString->enable();
        myMatchAttrComboBox->clearItems();
        // fill attribute combo box
        for (auto it : tagValue) {
            myMatchAttrComboBox->appendItem(toString(it.first).c_str());
        }
        // Add extra attribute "generic"
        myMatchAttrComboBox->appendItem(toString(GNE_ATTR_GENERIC).c_str());
        // check if item can block movement
        if (tagValue.canBlockMovement()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_BLOCK_MOVEMENT).c_str());
        }
        // check if item can block shape
        if (tagValue.canBlockShape()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_BLOCK_SHAPE).c_str());
        }
        // check if item can close shape
        if (tagValue.canCloseShape()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_CLOSE_SHAPE).c_str());
        }
        // check if item can have parent
        if (tagValue.hasParent()) {
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
GNESelectorFrame::MatchAttribute::onCmdSelMBAttribute(FXObject*, FXSelector, void*) {
    // first obtain a copy of item attributes vinculated with current tag
    auto tagPropertiesCopy = GNEAttributeCarrier::getTagProperties(myCurrentTag);
    // obtain tag property (only for improve code legibility)
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentTag);
    // add an extra AttributeValues to allow select ACs using as criterium "generic parameters"
    tagPropertiesCopy.addAttribute(GNE_ATTR_GENERIC, GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_STRING, "", "");
    // add extra attribute if item can block movement
    if (tagValue.canBlockMovement()) {
        // add an extra AttributeValues to allow select ACs using as criterium "block movement"
        tagPropertiesCopy.addAttribute(GNE_ATTR_BLOCK_MOVEMENT, GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_BOOL, "", "false");
    }
    // add extra attribute if item can block shape
    if (tagValue.canBlockShape()) {
        // add an extra AttributeValues to allow select ACs using as criterium "block shape"
        tagPropertiesCopy.addAttribute(GNE_ATTR_BLOCK_SHAPE, GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_BOOL, "", "false");
    }
    // add extra attribute if item can close shape
    if (tagValue.canCloseShape()) {
        // add an extra AttributeValues to allow select ACs using as criterium "close shape"
        tagPropertiesCopy.addAttribute(GNE_ATTR_CLOSE_SHAPE, GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_BOOL, "", "true");
    }
    // add extra attribute if item can have parent
    if (tagValue.hasParent()) {
        // add an extra AttributeValues to allow select ACs using as criterium "parent"
        tagPropertiesCopy.addAttribute(GNE_ATTR_PARENT, GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_STRING, "", "");
    }
    // set current selected attribute
    myCurrentAttribute = SUMO_ATTR_NOTHING;
    for (auto i : tagPropertiesCopy) {
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
GNESelectorFrame::MatchAttribute::onCmdSelMBString(FXObject*, FXSelector, void*) {
    // obtain expresion
    std::string expr(myMatchString->getText().text());
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentTag);
    bool valid = true;
    if (expr == "") {
        // the empty expression matches all objects
        mySelectorFrameParent->handleIDs(mySelectorFrameParent->getMatches(myCurrentTag, myCurrentAttribute, '@', 0, expr));
    } else if (tagValue.hasAttribute(myCurrentAttribute) && tagValue.getAttribute(myCurrentAttribute).isNumerical()) {
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
        // check if value can be parsed to double
        if (GNEAttributeCarrier::canParse<double>(expr.c_str())) {
            mySelectorFrameParent->handleIDs(mySelectorFrameParent->getMatches(myCurrentTag, myCurrentAttribute, compOp, GNEAttributeCarrier::parse<double>(expr.c_str()), expr));
        } else {
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
        mySelectorFrameParent->handleIDs(mySelectorFrameParent->getMatches(myCurrentTag, myCurrentAttribute, compOp, 0, expr));
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
GNESelectorFrame::MatchAttribute::onCmdHelp(FXObject*, FXSelector, void*) {
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
    WRITE_DEBUG("Opening help dialog of selector frame");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Close help dialog of selector frame");
    return 1;
}

// ---------------------------------------------------------------------------
// ModificationMode::VisualScaling - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::VisualScaling::VisualScaling(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Visual Scaling", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent) {
    // Create spin button and configure it
    mySelectionScaling = new FXRealSpinner(this, 7, this, MID_GNE_SELECTORFRAME_SELECTSCALE, GUIDesignSpinDial);
    //mySelectionScaling->setNumberFormat(1);
    //mySelectionScaling->setIncrements(0.1, .5, 1);
    mySelectionScaling->setIncrement(0.5);
    mySelectionScaling->setRange(1, 100);
    mySelectionScaling->setValue(1);
    mySelectionScaling->setHelpText("Enlarge selected objects");
}


GNESelectorFrame::VisualScaling::~VisualScaling() {}


long
GNESelectorFrame::VisualScaling::onCmdScaleSelection(FXObject*, FXSelector, void*) {
    // set scale in viewnet
    mySelectorFrameParent->getViewNet()->setSelectionScaling(mySelectionScaling->getValue());
    mySelectorFrameParent->getViewNet()->update();
    return 1;
}

// ---------------------------------------------------------------------------
// ModificationMode::SelectionOperation - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::SelectionOperation::SelectionOperation(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Operations for selections", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent) {
    // Create "Clear List" Button
    new FXButton(this, "Clear\t\t", 0, this, MID_CHOOSEN_CLEAR, GUIDesignButton);
    // Create "Invert" Button
    new FXButton(this, "Invert\t\t", 0, this, MID_CHOOSEN_INVERT, GUIDesignButton);
    // Create "Save" Button
    new FXButton(this, "Save\t\tSave ids of currently selected objects to a file.", 0, this, MID_CHOOSEN_SAVE, GUIDesignButton);
    // Create "Load" Button
    new FXButton(this, "Load\t\tLoad ids from a file according to the current modfication mode.", 0, this, MID_CHOOSEN_LOAD, GUIDesignButton);
}


GNESelectorFrame::SelectionOperation::~SelectionOperation() {}


long
GNESelectorFrame::SelectionOperation::onCmdLoad(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open List of Selected Items");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Selection files (*.txt)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        std::vector<GNEAttributeCarrier*> loadedACs;
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        std::ostringstream msg;
        std::ifstream strm(file.c_str());
        // check if file can be opened
        if (!strm.good()) {
            WRITE_ERROR("Could not open '" + file + "'.");
            return 0;
        }
        while (strm.good()) {
            std::string line;
            strm >> line;
            // check if line isn't empty
            if (line.length() != 0) {
                // obtain GLObject
                GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(line);
                // check if GUIGlObject exist and their  their GL type isn't blocked
                if ((object != nullptr) && !mySelectorFrameParent->myLockGLObjectTypes->IsObjectTypeLocked(object->getType())) {
                    // obtain GNEAttributeCarrier
                    GNEAttributeCarrier* AC = mySelectorFrameParent->getViewNet()->getNet()->retrieveAttributeCarrier(object->getGlID(), false);
                    // check if AC exist and if is selectable
                    if (AC != nullptr) {
                        loadedACs.push_back(AC);
                    }
                }
            }
        }
        // change selected attribute in loaded ACs allowing undo/redo
        if (loadedACs.size() > 0) {
            mySelectorFrameParent->getViewNet()->getUndoList()->p_begin("load selection");
            mySelectorFrameParent->handleIDs(loadedACs);
            mySelectorFrameParent->getViewNet()->getUndoList()->p_end();
        }
        // update list of current selected items
        mySelectorFrameParent->myLockGLObjectTypes->updateLockGLObjectTypes();
    }
    mySelectorFrameParent->getViewNet()->update();
    return 1;
}


long
GNESelectorFrame::SelectionOperation::onCmdSave(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(
                        this, "Save List of selected Items", ".txt", GUIIconSubSys::getIcon(ICON_EMPTY), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        for (auto i : mySelectorFrameParent->myViewNet->getNet()->getSelectedAttributeCarriers()) {
            GUIGlObject* object = dynamic_cast<GUIGlObject*>(i);
            if (object) {
                dev << GUIGlObject::TypeNames.getString(object->getType()) << ":" << i->getID() << "\n";
            }
        }
        dev.close();
    } catch (IOError& e) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'error storing selection'");
        // open message box error
        FXMessageBox::error(this, MBOX_OK, "Storing Selection failed", "%s", e.what());
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'error storing selection' with 'OK'");
    }
    return 1;
}


long
GNESelectorFrame::SelectionOperation::onCmdClear(FXObject*, FXSelector, void*) {
    // clear current selection
    mySelectorFrameParent->clearCurrentSelection();
    return 1;
}


long
GNESelectorFrame::SelectionOperation::onCmdInvert(FXObject*, FXSelector, void*) {
    // first make a copy of current selected elements
    std::vector<GNEAttributeCarrier*> copyOfSelectedAC = mySelectorFrameParent->getViewNet()->getNet()->getSelectedAttributeCarriers();
    // for invert selection, first clean current selection and next select elements of set "unselectedElements"
    mySelectorFrameParent->getViewNet()->getUndoList()->p_begin("invert selection");
    // select junctions, edges, lanes connections and crossings
    std::vector<GNEJunction*> junctions = mySelectorFrameParent->getViewNet()->getNet()->retrieveJunctions();
    for (auto i : junctions) {
        i->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
        // due we iterate over all junctions, only it's neccesary iterate over incoming edges
        for (auto j : i->getGNEIncomingEdges()) {
            // only select edges if "select edges" flag is enabled. In other case, select only lanes
            if (mySelectorFrameParent->getViewNet()->selectEdges()) {
                j->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
            } else {
                for (auto k : j->getLanes()) {
                    k->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
                }
            }
            // select connections
            for (auto k : j->getGNEConnections()) {
                k->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
            }
        }
        // select crossings
        for (auto j : i->getGNECrossings()) {
            j->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
        }
    }
    // select additionals
    std::vector<GNEAdditional*> additionals = mySelectorFrameParent->getViewNet()->getNet()->retrieveAdditionals();
    for (auto i : additionals) {
        if (GNEAttributeCarrier::getTagProperties(i->getTag()).isSelectable()) {
            i->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
        }
    }
    // select polygons
    for (auto i : mySelectorFrameParent->getViewNet()->getNet()->getPolygons()) {
        dynamic_cast<GNEPoly*>(i.second)->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
    }
    // select POIs
    for (auto i : mySelectorFrameParent->getViewNet()->getNet()->getPOIs()) {
        dynamic_cast<GNEPOI*>(i.second)->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->getViewNet()->getUndoList());
    }
    // now iterate over all elements of "copyOfSelectedAC" and undselect it
    for (auto i : copyOfSelectedAC) {
        i->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->getViewNet()->getUndoList());
    }
    // finish selection operation
    mySelectorFrameParent->getViewNet()->getUndoList()->p_end();
    // update current selected items
    mySelectorFrameParent->myLockGLObjectTypes->updateLockGLObjectTypes();
    // update view
    mySelectorFrameParent->getViewNet()->update();
    return 1;
}


/****************************************************************************/
