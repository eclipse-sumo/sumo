/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNESelectorFrame.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// The Widget for modifying selections of network-elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNESelectorFrame.h"
#include "GNEElementSet.h"
#include "GNEMatchAttribute.h"
#include "GNEMatchGenericDataAttribute.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry) ObjectTypeEntryMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::onCmdSetCheckBox)
};

FXDEFMAP(GNESelectorFrame::ModificationMode) ModificationModeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNESelectorFrame::ModificationMode::onCmdSelectModificationMode)
};

FXDEFMAP(GNESelectorFrame::VisualScaling) VisualScalingMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTSCALE,      GNESelectorFrame::VisualScaling::onCmdScaleSelection)
};

FXDEFMAP(GNESelectorFrame::SelectionOperation) SelectionOperationMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,   GNESelectorFrame::SelectionOperation::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,   GNESelectorFrame::SelectionOperation::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT, GNESelectorFrame::SelectionOperation::onCmdInvert),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,  GNESelectorFrame::SelectionOperation::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_DELETE, GNESelectorFrame::SelectionOperation::onCmdDelete)
};

// Object implementation
FXIMPLEMENT(GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry,   FXObject,       ObjectTypeEntryMap,             ARRAYNUMBER(ObjectTypeEntryMap))
FXIMPLEMENT(GNESelectorFrame::ModificationMode,                     FXGroupBox,     ModificationModeMap,            ARRAYNUMBER(ModificationModeMap))
FXIMPLEMENT(GNESelectorFrame::VisualScaling,                        FXGroupBox,     VisualScalingMap,               ARRAYNUMBER(VisualScalingMap))
FXIMPLEMENT(GNESelectorFrame::SelectionOperation,                   FXGroupBox,     SelectionOperationMap,          ARRAYNUMBER(SelectionOperationMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// ModificationMode::LockGLObjectTypes - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::LockGLObjectTypes::LockGLObjectTypes(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Locked selected items", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent) {
    // create a matrix for TypeEntries
    FXMatrix* matrixLockGLObjectTypes = new FXMatrix(this, 3, GUIDesignMatrixLockGLTypes);
    // create typeEntries for Network elements
    myTypeEntries[GLO_JUNCTION] =           new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "Junctions");
    myTypeEntries[GLO_EDGE] =               new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "Edges");
    myTypeEntries[GLO_LANE] =               new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "Lanes");
    myTypeEntries[GLO_CONNECTION] =         new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "Connections");
    myTypeEntries[GLO_CROSSING] =           new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "Crossings");
    myTypeEntries[GLO_ADDITIONALELEMENT] =  new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "Additionals");
    myTypeEntries[GLO_TAZ] =                new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "TAZs");
    myTypeEntries[GLO_POLYGON] =            new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "Polygons");
    myTypeEntries[GLO_POI] =                new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::NETWORK, "POIs");
    // create typeEntries for Demand elements
    myTypeEntries[GLO_ROUTE] =              new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Routes");
    myTypeEntries[GLO_VEHICLE] =            new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Vehicles");
    myTypeEntries[GLO_ROUTEFLOW] =          new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Flows");
    myTypeEntries[GLO_TRIP] =               new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Trips");
    myTypeEntries[GLO_FLOW] =               new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Flow");
    myTypeEntries[GLO_STOP] =               new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Stops");
    myTypeEntries[GLO_PERSON] =             new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Persons");
    myTypeEntries[GLO_PERSONFLOW] =         new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "PersonFlows");
    myTypeEntries[GLO_PERSONTRIP] =         new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "PersonTrips");
    myTypeEntries[GLO_RIDE] =               new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Rides");
    myTypeEntries[GLO_WALK] =               new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "Walks");
    myTypeEntries[GLO_STOP_PERSON] =         new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DEMAND, "StopPersons");
    // create typeEntries for Data elements
    myTypeEntries[GLO_EDGEDATA] =           new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DATA, "EdgeDatas");
    myTypeEntries[GLO_EDGERELDATA] =        new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DATA, "EdgeRelDatas");
    myTypeEntries[GLO_TAZRELDATA] =        new ObjectTypeEntry(matrixLockGLObjectTypes, Supermode::DATA, "EdgeRelDatas");
}


GNESelectorFrame::LockGLObjectTypes::~LockGLObjectTypes() {
    // remove all type entries
    for (const auto& typeEntry : myTypeEntries) {
        delete typeEntry.second;
    }
}


void
GNESelectorFrame::LockGLObjectTypes::addedLockedObject(const GUIGlObjectType type) {
    if ((type >= 100) && (type < 199)) {
        myTypeEntries.at(GLO_ADDITIONALELEMENT)->counterUp();
    } else {
        myTypeEntries.at(type)->counterUp();
    }
}


void
GNESelectorFrame::LockGLObjectTypes::removeLockedObject(const GUIGlObjectType type) {
    if ((type >= 100) && (type < 199)) {
        myTypeEntries.at(GLO_ADDITIONALELEMENT)->counterDown();
    } else {
        myTypeEntries.at(type)->counterDown();
    }
}


bool
GNESelectorFrame::LockGLObjectTypes::IsObjectTypeLocked(const GUIGlObjectType type) const {
    if ((type >= 100) && (type < 199)) {
        return myTypeEntries.at(GLO_ADDITIONALELEMENT)->isGLTypeLocked();
    } else {
        return myTypeEntries.at(type)->isGLTypeLocked();
    }
}


void
GNESelectorFrame::LockGLObjectTypes::showTypeEntries() {
    for (const auto& typeEntry : myTypeEntries) {
        // show or hidde type entries depending of current supermode
        if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork() && (typeEntry.second->getSupermode() == Supermode::NETWORK)) {
            typeEntry.second->showObjectTypeEntry();
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand() && (typeEntry.second->getSupermode() == Supermode::DEMAND)) {
            typeEntry.second->showObjectTypeEntry();
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData() && (typeEntry.second->getSupermode() == Supermode::DATA)) {
            typeEntry.second->showObjectTypeEntry();
        } else {
            typeEntry.second->hideObjectTypeEntry();
        }
    }
    // recalc frame parent
    recalc();
}


GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::ObjectTypeEntry(FXMatrix* matrixParent, const Supermode supermode, const std::string& label) :
    FXObject(),
    mySupermode(supermode),
    myLabelCounter(nullptr),
    myLabelTypeName(nullptr),
    myCheckBoxLocked(nullptr),
    myCounter(0) {
    // create elements
    myLabelCounter = new FXLabel(matrixParent, "0", nullptr, GUIDesignLabelLeft);
    myLabelTypeName = new FXLabel(matrixParent, (label + " ").c_str(), nullptr, GUIDesignLabelLeft);
    myCheckBoxLocked = new FXCheckButton(matrixParent, "unlocked", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
}


Supermode
GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::getSupermode() const {
    return mySupermode;
}


void
GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::showObjectTypeEntry() {
    myLabelCounter->show();
    myLabelTypeName->show();
    myCheckBoxLocked->show();
}


void
GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::hideObjectTypeEntry() {
    myLabelCounter->hide();
    myLabelTypeName->hide();
    myCheckBoxLocked->hide();
}


void
GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::counterUp() {
    myCounter++;
    myLabelCounter->setText(toString(myCounter).c_str());
}


void
GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::counterDown() {
    myCounter--;
    myLabelCounter->setText(toString(myCounter).c_str());
}


bool
GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::isGLTypeLocked() const {
    return (myCheckBoxLocked->getCheck() == TRUE);
}


long
GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::onCmdSetCheckBox(FXObject*, FXSelector, void*) {
    if (myCheckBoxLocked->getCheck() == TRUE) {
        myCheckBoxLocked->setText("locked");
    } else {
        myCheckBoxLocked->setText("unlocked");
    }
    return 1;
}


GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::ObjectTypeEntry() :
    FXObject(),
    mySupermode(Supermode::NETWORK),
    myLabelCounter(nullptr),
    myLabelTypeName(nullptr),
    myCheckBoxLocked(nullptr),
    myCounter(0) {
}

// ---------------------------------------------------------------------------
// ModificationMode::ModificationMode - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::ModificationMode::ModificationMode(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Modification Mode", GUIDesignGroupBoxFrame),
    myModificationModeType(Operation::ADD) {
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


GNESelectorFrame::ModificationMode::Operation
GNESelectorFrame::ModificationMode::getModificationMode() const {
    return myModificationModeType;
}


long
GNESelectorFrame::ModificationMode::onCmdSelectModificationMode(FXObject* obj, FXSelector, void*) {
    if (obj == myAddRadioButton) {
        myModificationModeType = Operation::ADD;
        myAddRadioButton->setCheck(true);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myRemoveRadioButton) {
        myModificationModeType = Operation::SUB;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(true);
        myKeepRadioButton->setCheck(false);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myKeepRadioButton) {
        myModificationModeType = Operation::RESTRICT;
        myAddRadioButton->setCheck(false);
        myRemoveRadioButton->setCheck(false);
        myKeepRadioButton->setCheck(true);
        myReplaceRadioButton->setCheck(false);
        return 1;
    } else if (obj == myReplaceRadioButton) {
        myModificationModeType = Operation::REPLACE;
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
    mySelectionScaling->setRange(1, 100000);
    mySelectionScaling->setValue(1);
    mySelectionScaling->setHelpText("Enlarge selected objects");
}


GNESelectorFrame::VisualScaling::~VisualScaling() {}


long
GNESelectorFrame::VisualScaling::onCmdScaleSelection(FXObject*, FXSelector, void*) {
    // set scale in viewnet
    mySelectorFrameParent->myViewNet->setSelectorFrameScale(mySelectionScaling->getValue());
    mySelectorFrameParent->myViewNet->updateViewNet();
    return 1;
}

// ---------------------------------------------------------------------------
// ModificationMode::SelectionOperation - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::SelectionOperation::SelectionOperation(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Operations for selections", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent) {
    // tabular buttons, see GNETLSEditorFrame

    FXHorizontalFrame* selectionButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    FXVerticalFrame* col1 = new FXVerticalFrame(selectionButtons, LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // left button columm
    FXVerticalFrame* col2 = new FXVerticalFrame(selectionButtons, LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // right button column

    // Create "Clear List" Button
    new FXButton(col1, "Clear\t\tDeselect all objects (hotkey: ESC)", nullptr, this, MID_CHOOSEN_CLEAR, GUIDesignButton);
    // Create "Invert" Button
    new FXButton(col2, "Invert\t\tInvert selection status of all objects", nullptr, this, MID_CHOOSEN_INVERT, GUIDesignButton);
    // Create "Save" Button
    new FXButton(col1, "Save\t\tSave ids of currently selected objects to a file.", nullptr, this, MID_CHOOSEN_SAVE, GUIDesignButton);
    // Create "Load" Button
    new FXButton(col2, "Load\t\tLoad ids from a file according to the current modfication mode.", nullptr, this, MID_CHOOSEN_LOAD, GUIDesignButton);
    // Create "Delete" Button
    new FXButton(col1, "Delete\t\tDelete all selected objects (hotkey: DEL)", nullptr, this, MID_CHOOSEN_DELETE, GUIDesignButton);
}


GNESelectorFrame::SelectionOperation::~SelectionOperation() {}


long
GNESelectorFrame::SelectionOperation::onCmdLoad(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open List of Selected Items");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
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
                    GNEAttributeCarrier* AC = mySelectorFrameParent->myViewNet->getNet()->retrieveAttributeCarrier(object->getGlID(), false);
                    // check if AC exist and if is selectable
                    if (AC && AC->getTagProperty().isSelectable())
                        // now check if we're in the correct supermode to load this element
                        if (((mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) && !AC->getTagProperty().isDemandElement()) ||
                                ((mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand()) && AC->getTagProperty().isDemandElement())) {
                            loadedACs.push_back(AC);
                        }
                }
            }
        }
        // change selected attribute in loaded ACs allowing undo/redo
        if (loadedACs.size() > 0) {
            mySelectorFrameParent->myViewNet->getUndoList()->p_begin("load selection");
            mySelectorFrameParent->handleIDs(loadedACs);
            mySelectorFrameParent->myViewNet->getUndoList()->p_end();
        }
    }
    mySelectorFrameParent->myViewNet->updateViewNet();
    return 1;
}


long
GNESelectorFrame::SelectionOperation::onCmdSave(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save List of selected Items", ".txt",
                    GUIIconSubSys::getIcon(GUIIcon::EMPTY), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        // get selected attribute carriers
        const auto selectedACs = mySelectorFrameParent->myViewNet->getNet()->getSelectedAttributeCarriers(false);
        for (const auto& selectedAC : selectedACs) {
            GUIGlObject* object = dynamic_cast<GUIGlObject*>(selectedAC);
            if (object) {
                dev << GUIGlObject::TypeNames.getString(object->getType()) << ":" << selectedAC->getID() << "\n";
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
GNESelectorFrame::SelectionOperation::onCmdDelete(FXObject*, FXSelector, void*) {
    // acts like the 'del' hotkey
    mySelectorFrameParent->getViewNet()->hotkeyDel();
    return 1;
}


long
GNESelectorFrame::SelectionOperation::onCmdInvert(FXObject*, FXSelector, void*) {
    // only continue if there is element for selecting
    if (mySelectorFrameParent->ACsToSelected()) {
        // obtan locks (only for improve code legibly)
        LockGLObjectTypes* locks = mySelectorFrameParent->getLockGLObjectTypes();
        // obtain undoList (only for improve code legibly)
        GNEUndoList* undoList = mySelectorFrameParent->myViewNet->getUndoList();
        // for invert selection, first clean current selection and next select elements of set "unselectedElements"
        undoList->p_begin("invert selection");
        // invert selection of elements depending of current supermode
        if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            // iterate over junctions
            for (const auto& junction : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getJunctions()) {
                // check if junction selection is locked
                if (!locks->IsObjectTypeLocked(GLO_JUNCTION)) {
                    if (junction.second->isAttributeCarrierSelected()) {
                        junction.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        junction.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                // due we iterate over all junctions, only it's neccesary iterate over incoming edges
                for (const auto& incomingEdge : junction.second->getGNEIncomingEdges()) {
                    // only select edges if "select edges" flag is enabled. In other case, select only lanes
                    if (mySelectorFrameParent->myViewNet->getNetworkViewOptions().selectEdges()) {
                        // check if edge selection is locked
                        if (!locks->IsObjectTypeLocked(GLO_EDGE)) {
                            if (incomingEdge->isAttributeCarrierSelected()) {
                                incomingEdge->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                            } else {
                                incomingEdge->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                            }
                        }
                    } else {
                        // check if lane selection is locked
                        if (!locks->IsObjectTypeLocked(GLO_LANE)) {
                            for (const auto& lane : incomingEdge->getLanes()) {
                                if (lane->isAttributeCarrierSelected()) {
                                    lane->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                                } else {
                                    lane->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                                }
                            }
                        }
                    }
                    // check if connection selection is locked
                    if (!locks->IsObjectTypeLocked(GLO_CONNECTION)) {
                        for (const auto& connection : incomingEdge->getGNEConnections()) {
                            if (connection->isAttributeCarrierSelected()) {
                                connection->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                            } else {
                                connection->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                            }
                        }
                    }
                }
                // check if crossing selection is locked
                if (!locks->IsObjectTypeLocked(GLO_CROSSING)) {
                    for (const auto& crossing : junction.second->getGNECrossings()) {
                        if (crossing->isAttributeCarrierSelected()) {
                            crossing->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                        } else {
                            crossing->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                        }
                    }
                }
            }
            // check if additionals selection is locked
            if (!locks->IsObjectTypeLocked(GLO_ADDITIONALELEMENT)) {
                for (const auto& additionals : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getAdditionals()) {
                    // first check if additional is selectable
                    if (GNEAttributeCarrier::getTagProperties(additionals.first).isSelectable()) {
                        for (const auto& additional : additionals.second) {
                            if (additional.second->isAttributeCarrierSelected()) {
                                additional.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                            } else {
                                additional.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                            }
                            // now iterate over additional children
                            for (const auto& additionalChild : additional.second->getChildAdditionals()) {
                                // first check if additional child is selectable
                                if (additionalChild->getTagProperty().isSelectable()) {
                                    if (additionalChild->isAttributeCarrierSelected()) {
                                        additionalChild->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                                    } else {
                                        additionalChild->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // invert polygons
            if (!locks->IsObjectTypeLocked(GLO_POLYGON)) {
                for (const auto& polygon : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POLY)) {
                    if (polygon.second->isAttributeCarrierSelected()) {
                        polygon.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        polygon.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert TAZs
            if (!locks->IsObjectTypeLocked(GLO_TAZ)) {
                for (const auto& polygon : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getTAZElements().at(SUMO_TAG_TAZ)) {
                    if (polygon.second->isAttributeCarrierSelected()) {
                        polygon.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        polygon.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert POIs and POILanes
            if (!locks->IsObjectTypeLocked(GLO_POI)) {
                for (const auto& POI : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POI)) {
                    if (POI.second->isAttributeCarrierSelected()) {
                        POI.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        POI.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand()) {
            // invert routes
            if (!locks->IsObjectTypeLocked(GLO_ROUTE)) {
                for (const auto& route : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
                    if (route.second->isAttributeCarrierSelected()) {
                        route.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        route.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& embeddedRoute : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_ROUTE_EMBEDDED)) {
                    if (embeddedRoute.second->isAttributeCarrierSelected()) {
                        embeddedRoute.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        embeddedRoute.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert vehicles
            if (!locks->IsObjectTypeLocked(GLO_VEHICLE)) {
                for (const auto& vehicle : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VEHICLE)) {
                    if (vehicle.second->isAttributeCarrierSelected()) {
                        vehicle.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        vehicle.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& vehicle : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_VEHICLE_WITHROUTE)) {
                    if (vehicle.second->isAttributeCarrierSelected()) {
                        vehicle.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        vehicle.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert trips
            if (!locks->IsObjectTypeLocked(GLO_TRIP)) {
                for (const auto& trip : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_TRIP)) {
                    if (trip.second->isAttributeCarrierSelected()) {
                        trip.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        trip.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert flows
            if (!locks->IsObjectTypeLocked(GLO_FLOW)) {
                for (const auto& flow : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_FLOW)) {
                    if (flow.second->isAttributeCarrierSelected()) {
                        flow.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        flow.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert route flows
            if (!locks->IsObjectTypeLocked(GLO_ROUTEFLOW)) {
                for (const auto& routeFlow : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_FLOW_ROUTE)) {
                    if (routeFlow.second->isAttributeCarrierSelected()) {
                        routeFlow.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        routeFlow.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& routeFlow : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_FLOW_WITHROUTE)) {
                    if (routeFlow.second->isAttributeCarrierSelected()) {
                        routeFlow.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        routeFlow.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert stops
            if (!locks->IsObjectTypeLocked(GLO_STOP)) {
                for (const auto& demandElementTag : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements()) {
                    for (const auto& demandElement : demandElementTag.second) {
                        for (const auto& stop : demandElement.second->getChildDemandElements()) {
                            if (stop->getTagProperty().isStop()) {
                                if (stop->isAttributeCarrierSelected()) {
                                    stop->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                                } else {
                                    stop->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                                }
                            }
                        }
                    }
                }
            }
            // invert person
            if (!locks->IsObjectTypeLocked(GLO_PERSON)) {
                for (const auto& person : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON)) {
                    if (person.second->isAttributeCarrierSelected()) {
                        person.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        person.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert person flow
            if (!locks->IsObjectTypeLocked(GLO_PERSONFLOW)) {
                for (const auto& personFlow : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW)) {
                    if (personFlow.second->isAttributeCarrierSelected()) {
                        personFlow.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personFlow.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert person trip
            if (!locks->IsObjectTypeLocked(GLO_PERSONTRIP)) {
                for (const auto& personTripFromTo : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_PERSONTRIP_EDGE)) {
                    if (personTripFromTo.second->isAttributeCarrierSelected()) {
                        personTripFromTo.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personTripFromTo.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& personTripBusStop : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_PERSONTRIP_BUSSTOP)) {
                    if (personTripBusStop.second->isAttributeCarrierSelected()) {
                        personTripBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personTripBusStop.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert ride
            if (!locks->IsObjectTypeLocked(GLO_RIDE)) {
                for (const auto& rideFromTo : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_RIDE_EDGE)) {
                    if (rideFromTo.second->isAttributeCarrierSelected()) {
                        rideFromTo.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        rideFromTo.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& rideBusStop : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_RIDE_BUSSTOP)) {
                    if (rideBusStop.second->isAttributeCarrierSelected()) {
                        rideBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        rideBusStop.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert walks
            if (!locks->IsObjectTypeLocked(GLO_WALK)) {
                for (const auto& walkFromTo : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_EDGE)) {
                    if (walkFromTo.second->isAttributeCarrierSelected()) {
                        walkFromTo.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        walkFromTo.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& walkBusStop : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_BUSSTOP)) {
                    if (walkBusStop.second->isAttributeCarrierSelected()) {
                        walkBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        walkBusStop.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& walkEdges : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_EDGES)) {
                    if (walkEdges.second->isAttributeCarrierSelected()) {
                        walkEdges.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        walkEdges.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& walkRoute : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_ROUTE)) {
                    if (walkRoute.second->isAttributeCarrierSelected()) {
                        walkRoute.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        walkRoute.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
            // invert person stops
            if (!locks->IsObjectTypeLocked(GLO_STOP_PERSON)) {
                for (const auto& stopPersonLane : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_STOPPERSON_EDGE)) {
                    if (stopPersonLane.second->isAttributeCarrierSelected()) {
                        stopPersonLane.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        stopPersonLane.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
                for (const auto& stopPersonBusStop : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_STOPPERSON_BUSSTOP)) {
                    if (stopPersonBusStop.second->isAttributeCarrierSelected()) {
                        stopPersonBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        stopPersonBusStop.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData()) {
            // invert dataSets
            for (const auto& dataSet : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDataSets()) {
                for (const auto& dataInterval : dataSet.second->getDataIntervalChildren()) {
                    for (const auto& genericData : dataInterval.second->getGenericDataChildren()) {
                        if ((!locks->IsObjectTypeLocked(GLO_EDGEDATA) && (genericData->getType() == GLO_EDGEDATA)) ||
                                (!locks->IsObjectTypeLocked(GLO_EDGERELDATA) && (genericData->getType() == GLO_EDGERELDATA))) {
                            if (genericData->isAttributeCarrierSelected()) {
                                genericData->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                            } else {
                                genericData->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                            }
                        }
                    }
                }
            }
        }
        // finish selection operation
        undoList->p_end();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNESelectorFrame - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Selection") {
    // create selectedItems modul
    myLockGLObjectTypes = new LockGLObjectTypes(this);
    // create Modification Mode modul
    myModificationMode = new ModificationMode(this);
    // create ElementSet modul
    myNetworkElementSet = new GNEElementSet(this, Supermode::NETWORK, SUMO_TAG_EDGE, SUMO_ATTR_SPEED, ">10.0");
    myDemandElementSet = new GNEElementSet(this, Supermode::DEMAND, SUMO_TAG_VEHICLE, SUMO_ATTR_ID, "");
    myDataElementSet = new GNEElementSet(this, Supermode::DATA, SUMO_TAG_MEANDATA_EDGE, GNE_ATTR_PARAMETERS, "key=value");
    // create VisualScaling modul
    myVisualScaling = new VisualScaling(this);
    // create SelectionOperation modul
    mySelectionOperation = new SelectionOperation(this);
    // Create groupbox for information about selections
    FXGroupBox* selectionHintGroupBox = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    // Create Selection Hint
    new FXLabel(selectionHintGroupBox, " - Hold <SHIFT> for \n   rectangle selection.\n - Press <DEL> to\n   delete selected objects.", nullptr, GUIDesignLabelFrameInformation);

}


GNESelectorFrame::~GNESelectorFrame() {}


void
GNESelectorFrame::show() {
    // show Type Entries depending of current supermode
    myLockGLObjectTypes->showTypeEntries();
    // refresh element set
    if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        // only show network element set
        myNetworkElementSet->showElementSet();
        myDemandElementSet->hideElementSet();
        myDataElementSet->hideElementSet();
    } else if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        // only show demand element set
        myNetworkElementSet->hideElementSet();
        myDemandElementSet->showElementSet();
        myDataElementSet->hideElementSet();
    } else if (myViewNet->getEditModes().isCurrentSupermodeData()) {
        // only show data element set
        myNetworkElementSet->hideElementSet();
        myDemandElementSet->hideElementSet();
        myDataElementSet->showElementSet();
    }
    // Show frame
    GNEFrame::show();
}


void
GNESelectorFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


void
GNESelectorFrame::clearCurrentSelection() const {
    // only continue if there is element for selecting
    if (ACsToSelected()) {
        // for invert selection, first clean current selection and next select elements of set "unselectedElements"
        myViewNet->getUndoList()->p_begin("invert selection");
        // invert selection of elements depending of current supermode
        if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            // iterate over junctions
            for (const auto& junction : myViewNet->getNet()->getAttributeCarriers()->getJunctions()) {
                // check if junction selection is locked
                if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_JUNCTION)) {
                    if (junction.second->isAttributeCarrierSelected()) {
                        junction.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                // due we iterate over all junctions, only it's neccesary iterate over incoming edges
                for (const auto& edge : junction.second->getGNEIncomingEdges()) {
                    // check if edge selection is locked
                    if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_EDGE)) {
                        if (edge->isAttributeCarrierSelected()) {
                            edge->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                    // check if lane selection is locked
                    if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_LANE)) {
                        for (const auto& lane : edge->getLanes()) {
                            if (lane->isAttributeCarrierSelected()) {
                                lane->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                            }
                        }
                    }
                    // check if connection selection is locked
                    if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_CONNECTION)) {
                        for (const auto& connection : edge->getGNEConnections()) {
                            if (connection->isAttributeCarrierSelected()) {
                                connection->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                            }
                        }
                    }
                }
                // check if crossing selection is locked
                if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_CROSSING)) {
                    for (const auto& crossing : junction.second->getGNECrossings()) {
                        if (crossing->isAttributeCarrierSelected()) {
                            crossing->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                }
            }
            // check if additionals selection is locked
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ADDITIONALELEMENT)) {
                for (const auto& additionals : myViewNet->getNet()->getAttributeCarriers()->getAdditionals()) {
                    // first check if additional is selectable
                    if (GNEAttributeCarrier::getTagProperties(additionals.first).isSelectable()) {
                        for (const auto& additional : additionals.second) {
                            if (additional.second->isAttributeCarrierSelected()) {
                                additional.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                            }
                            // now iterate over additional children
                            for (const auto& additionalChild : additional.second->getChildAdditionals()) {
                                // first check if additional child is selectable
                                if (additionalChild->getTagProperty().isSelectable() && additionalChild->isAttributeCarrierSelected()) {
                                    additionalChild->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                                }
                            }
                        }
                    }
                }
            }
            // unselect polygons
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POLYGON)) {
                for (const auto& polygon : myViewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POLY)) {
                    if (polygon.second->isAttributeCarrierSelected()) {
                        polygon.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect TAZs
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_TAZ)) {
                for (const auto& polygon : myViewNet->getNet()->getAttributeCarriers()->getTAZElements().at(SUMO_TAG_TAZ)) {
                    if (polygon.second->isAttributeCarrierSelected()) {
                        polygon.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect POIs and POILanes
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POI)) {
                for (const auto& POI : myViewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POI)) {
                    if (POI.second->isAttributeCarrierSelected()) {
                        POI.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
        } else if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
            // unselect routes
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTE)) {
                for (const auto& route : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
                    if (route.second->isAttributeCarrierSelected()) {
                        route.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect vehicles
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_VEHICLE)) {
                for (const auto& vehicle : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VEHICLE)) {
                    if (vehicle.second->isAttributeCarrierSelected()) {
                        vehicle.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& vehicle : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_VEHICLE_WITHROUTE)) {
                    if (vehicle.second->isAttributeCarrierSelected()) {
                        vehicle.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                    for (const auto& embeddedRoute : vehicle.second->getChildDemandElements()) {
                        if (embeddedRoute->isAttributeCarrierSelected()) {
                            embeddedRoute->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                }
            }
            // unselect trips
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_TRIP)) {
                for (const auto& trip : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_TRIP)) {
                    if (trip.second->isAttributeCarrierSelected()) {
                        trip.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect flows
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_FLOW)) {
                for (const auto& flow : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_FLOW)) {
                    if (flow.second->isAttributeCarrierSelected()) {
                        flow.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect route flows
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTEFLOW)) {
                for (const auto& routeFlow : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_FLOW_ROUTE)) {
                    if (routeFlow.second->isAttributeCarrierSelected()) {
                        routeFlow.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& routeFlow : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_FLOW_WITHROUTE)) {
                    if (routeFlow.second->isAttributeCarrierSelected()) {
                        routeFlow.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                    for (const auto& embeddedRoute : routeFlow.second->getChildDemandElements()) {
                        if (embeddedRoute->isAttributeCarrierSelected()) {
                            embeddedRoute->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                }
            }
            // unselect stops
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_STOP)) {
                for (const auto& demandElementTag : myViewNet->getNet()->getAttributeCarriers()->getDemandElements()) {
                    for (const auto& demandElement : demandElementTag.second) {
                        for (const auto& stop : demandElement.second->getChildDemandElements()) {
                            if (stop->getTagProperty().isStop()) {
                                if (stop->isAttributeCarrierSelected()) {
                                    stop->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                                }
                            }
                        }
                    }
                }
            }
            // unselect person
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSON)) {
                for (const auto& person : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON)) {
                    if (person.second->isAttributeCarrierSelected()) {
                        person.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect person flows
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONFLOW)) {
                for (const auto& personFlow : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW)) {
                    if (personFlow.second->isAttributeCarrierSelected()) {
                        personFlow.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect person trips
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONTRIP)) {
                for (const auto& personTripBusStop : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_PERSONTRIP_EDGE)) {
                    if (personTripBusStop.second->isAttributeCarrierSelected()) {
                        personTripBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& personTripBusStop : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_PERSONTRIP_BUSSTOP)) {
                    if (personTripBusStop.second->isAttributeCarrierSelected()) {
                        personTripBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect ride
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_RIDE)) {
                for (const auto& rideFromTo : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_RIDE_EDGE)) {
                    if (rideFromTo.second->isAttributeCarrierSelected()) {
                        rideFromTo.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& rideBusStop : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_RIDE_BUSSTOP)) {
                    if (rideBusStop.second->isAttributeCarrierSelected()) {
                        rideBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect walks
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_WALK)) {
                if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_WALK)) {
                    for (const auto& walkBusStop : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_EDGE)) {
                        if (walkBusStop.second->isAttributeCarrierSelected()) {
                            walkBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                    for (const auto& walkBusStop : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_BUSSTOP)) {
                        if (walkBusStop.second->isAttributeCarrierSelected()) {
                            walkBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                }
                for (const auto& walkEdges : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_EDGES)) {
                    if (walkEdges.second->isAttributeCarrierSelected()) {
                        walkEdges.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& walkRoute : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_WALK_ROUTE)) {
                    if (walkRoute.second->isAttributeCarrierSelected()) {
                        walkRoute.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // unselect person stops
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_STOP_PERSON)) {
                for (const auto& stopPersonLane : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_STOPPERSON_EDGE)) {
                    if (stopPersonLane.second->isAttributeCarrierSelected()) {
                        stopPersonLane.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& stopPersonBusStop : myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(GNE_TAG_STOPPERSON_BUSSTOP)) {
                    if (stopPersonBusStop.second->isAttributeCarrierSelected()) {
                        stopPersonBusStop.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
        } else if (myViewNet->getEditModes().isCurrentSupermodeData()) {
            for (const auto& dataSet : myViewNet->getNet()->getAttributeCarriers()->getDataSets()) {
                for (const auto& dataInterval : dataSet.second->getDataIntervalChildren()) {
                    for (const auto& genericData : dataInterval.second->getGenericDataChildren()) {
                        if (genericData->isAttributeCarrierSelected()) {
                            genericData->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                }
            }
        }
        // finish selection operation
        myViewNet->getUndoList()->p_end();
    }
}


void
GNESelectorFrame::handleIDs(const std::vector<GNEAttributeCarrier*>& ACs, const ModificationMode::Operation setop) {
    // declare set operation
    const ModificationMode::Operation setOperation = ((setop == ModificationMode::Operation::DEFAULT) ? myModificationMode->getModificationMode() : setop);
    // declare two sets of attribute carriers, one for select and another for unselect
    std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsToSelect, ACsToUnselect;
    // in restrict AND replace mode all current selected attribute carriers will be unselected
    if ((setOperation == ModificationMode::Operation::REPLACE) || (setOperation == ModificationMode::Operation::RESTRICT)) {
        // obtain selected ACs depending of current supermode
        std::vector<GNEAttributeCarrier*> selectedACs = myViewNet->getNet()->getSelectedAttributeCarriers(false);
        // add id into ACs to unselect
        for (const auto& selectedAC : selectedACs) {
            ACsToUnselect.insert(std::make_pair(selectedAC->getID(), selectedAC));
        }
    }
    // handle ids
    for (const auto& AC : ACs) {
        // iterate over AtributeCarriers an place it in ACsToSelect or ACsToUnselect
        switch (setOperation) {
            case GNESelectorFrame::ModificationMode::Operation::SUB:
                ACsToUnselect.insert(std::make_pair(AC->getID(), AC));
                break;
            case GNESelectorFrame::ModificationMode::Operation::RESTRICT:
                if (ACsToUnselect.find(std::make_pair(AC->getID(), AC)) != ACsToUnselect.end()) {
                    ACsToSelect.insert(std::make_pair(AC->getID(), AC));
                }
                break;
            default:
                ACsToSelect.insert(std::make_pair(AC->getID(), AC));
                break;
        }
    }
    // select junctions and their connections if Auto select junctions is enabled (note: only for "add mode")
    if (myViewNet->autoSelectNodes() && (setop == ModificationMode::Operation::ADD)) {
        std::set<GNEEdge*> edgesToSelect;
        // iterate over ACsToSelect and extract edges
        for (const auto& AC : ACsToSelect) {
            if (AC.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                edgesToSelect.insert(myViewNet->getNet()->retrieveEdge(AC.second->getID()));
            }
        }
        // iterate over extracted edges
        for (const auto& edgeToSelect : edgesToSelect) {
            // select junction source and all connections and crossings
            ACsToSelect.insert(std::make_pair(edgeToSelect->getParentJunctions().front()->getID(), edgeToSelect->getParentJunctions().front()));
            for (const auto& connectionToSelect : edgeToSelect->getParentJunctions().front()->getGNEConnections()) {
                ACsToSelect.insert(std::make_pair(connectionToSelect->getID(), connectionToSelect));
            }
            for (const auto& crossingToSelect : edgeToSelect->getParentJunctions().front()->getGNECrossings()) {
                ACsToSelect.insert(std::make_pair(crossingToSelect->getID(), crossingToSelect));
            }
            // select junction destiny and all connections and crossings
            ACsToSelect.insert(std::make_pair(edgeToSelect->getParentJunctions().back()->getID(), edgeToSelect->getParentJunctions().back()));
            for (const auto& connectionToSelect : edgeToSelect->getParentJunctions().back()->getGNEConnections()) {
                ACsToSelect.insert(std::make_pair(connectionToSelect->getID(), connectionToSelect));
            }
            for (const auto& crossingToSelect : edgeToSelect->getParentJunctions().back()->getGNECrossings()) {
                ACsToSelect.insert(std::make_pair(crossingToSelect->getID(), crossingToSelect));
            }
        }
    }
    // only continue if there is ACs to select or unselect
    if ((ACsToSelect.size() + ACsToUnselect.size()) > 0) {
        // first unselect AC of ACsToUnselect and then selects AC of ACsToSelect
        myViewNet->getUndoList()->p_begin("selection");
        for (const auto& ACToUnselect : ACsToUnselect) {
            if (ACToUnselect.second->getTagProperty().isSelectable()) {
                ACToUnselect.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
            }
        }
        for (const auto& ACToSelect : ACsToSelect) {
            if (ACToSelect.second->getTagProperty().isSelectable()) {
                ACToSelect.second->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
        }
        // finish operation
        myViewNet->getUndoList()->p_end();
    }
}


std::vector<GNEAttributeCarrier*>
GNESelectorFrame::getMatches(const SumoXMLTag ACTag, const SumoXMLAttr ACAttr, const char compOp, const double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    // first retrieve all ACs using ACTag
    std::vector<GNEAttributeCarrier*> allACbyTag = myViewNet->getNet()->retrieveAttributeCarriers(ACTag);
    // get Tag value
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(ACTag);
    // iterate over all ACs
    for (const auto& AC : allACbyTag) {
        if (expr == "") {
            result.push_back(AC);
        } else if (tagValue.hasAttribute(ACAttr) && tagValue.getAttributeProperties(ACAttr).isNumerical()) {
            double acVal;
            std::istringstream buf(AC->getAttribute(ACAttr));
            buf >> acVal;
            switch (compOp) {
                case '<':
                    if (acVal < val) {
                        result.push_back(AC);
                    }
                    break;
                case '>':
                    if (acVal > val) {
                        result.push_back(AC);
                    }
                    break;
                case '=':
                    if (acVal == val) {
                        result.push_back(AC);
                    }
                    break;
            }
        } else {
            // string match
            std::string acVal = AC->getAttributeForSelection(ACAttr);
            switch (compOp) {
                case '@':
                    if (acVal.find(expr) != std::string::npos) {
                        result.push_back(AC);
                    }
                    break;
                case '!':
                    if (acVal.find(expr) == std::string::npos) {
                        result.push_back(AC);
                    }
                    break;
                case '=':
                    if (acVal == expr) {
                        result.push_back(AC);
                    }
                    break;
                case '^':
                    if (acVal != expr) {
                        result.push_back(AC);
                    }
                    break;
            }
        }
    }
    return result;
}


std::vector<GNEAttributeCarrier*>
GNESelectorFrame::getGenericMatches(const std::vector<GNEGenericData*>& genericDatas, const std::string& attr, const char compOp, const double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    // iterate over generic datas
    for (const auto& genericData : genericDatas) {
        if (expr == "") {
            result.push_back(genericData);
        } else if (attr != toString(GNE_ATTR_PARENT)) {
            double acVal;
            std::istringstream buf(genericData->getParameter(attr, "0"));
            buf >> acVal;
            switch (compOp) {
                case '<':
                    if (acVal < val) {
                        result.push_back(genericData);
                    }
                    break;
                case '>':
                    if (acVal > val) {
                        result.push_back(genericData);
                    }
                    break;
                case '=':
                    if (acVal == val) {
                        result.push_back(genericData);
                    }
                    break;
            }
        } else {
            // string match
            std::string acVal = genericData->getAttributeForSelection(GNE_ATTR_PARENT);
            switch (compOp) {
                case '@':
                    if (acVal.find(expr) != std::string::npos) {
                        result.push_back(genericData);
                    }
                    break;
                case '!':
                    if (acVal.find(expr) == std::string::npos) {
                        result.push_back(genericData);
                    }
                    break;
                case '=':
                    if (acVal == expr) {
                        result.push_back(genericData);
                    }
                    break;
                case '^':
                    if (acVal != expr) {
                        result.push_back(genericData);
                    }
                    break;
            }
        }
    }
    return result;
}


GNESelectorFrame::ModificationMode*
GNESelectorFrame::getModificationModeModul() const {
    return myModificationMode;
}


FXVerticalFrame*
GNESelectorFrame::getContentFrame() const {
    return myContentFrame;
}


GNESelectorFrame::LockGLObjectTypes*
GNESelectorFrame::getLockGLObjectTypes() const {
    return myLockGLObjectTypes;
}


bool
GNESelectorFrame::ACsToSelected() const {
    if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        // check if exist junction and edges
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_JUNCTION) && (myViewNet->getNet()->getAttributeCarriers()->getJunctions().size() > 0)) {
            return true;
        }
        // check if exist connections
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_CONNECTION)) {
            for (const auto& junction : myViewNet->getNet()->getAttributeCarriers()->getJunctions()) {
                if (junction.second->getGNEConnections().size() > 0) {
                    return true;
                }
            }
        }
        // check if exist crossings
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_CROSSING)) {
            for (const auto& junction : myViewNet->getNet()->getAttributeCarriers()->getJunctions()) {
                if (junction.second->getGNECrossings().size() > 0) {
                    return true;
                }
            }
        }
        // check edges and lanes
        if ((!myLockGLObjectTypes->IsObjectTypeLocked(GLO_EDGE) || !myLockGLObjectTypes->IsObjectTypeLocked(GLO_LANE)) && (myViewNet->getNet()->getAttributeCarriers()->getEdges().size() > 0)) {
            return true;
        }
        // check if additionals selection is locked
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ADDITIONALELEMENT)) {
            for (const auto& additionalTag : myViewNet->getNet()->getAttributeCarriers()->getAdditionals()) {
                // first check if additional is selectable
                if (GNEAttributeCarrier::getTagProperties(additionalTag.first).isSelectable() &&
                        (myViewNet->getNet()->getAttributeCarriers()->getAdditionals().at(additionalTag.first).size() > 0)) {
                    return true;
                }
            }
        }
        // check polygons
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POLYGON) &&
            (myViewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POLY).size() > 0)) {
            return true;
        }
        // check POIs
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POI) &&
            (myViewNet->getNet()->getAttributeCarriers()->getShapes().at(SUMO_TAG_POI).size() > 0)) {
            return true;
        }
    } else if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        // get demand elements map
        const std::map<SumoXMLTag, std::map<std::string, GNEDemandElement*> >& demandElementsMap = myViewNet->getNet()->getAttributeCarriers()->getDemandElements();
        // check routes
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTE) && ((demandElementsMap.at(SUMO_TAG_ROUTE).size() + demandElementsMap.at(GNE_TAG_ROUTE_EMBEDDED).size()) > 0)) {
            return true;
        }
        // check vehicles
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_VEHICLE) && ((demandElementsMap.at(SUMO_TAG_VEHICLE).size() + demandElementsMap.at(GNE_TAG_VEHICLE_WITHROUTE).size()) > 0)) {
            return true;
        }
        // check trips
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_TRIP) && (demandElementsMap.at(SUMO_TAG_TRIP).size() > 0)) {
            return true;
        }
        // check flows
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_FLOW) && (demandElementsMap.at(SUMO_TAG_FLOW).size() > 0)) {
            return true;
        }
        // check route flows
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTEFLOW) && ((demandElementsMap.at(GNE_TAG_FLOW_ROUTE).size() + demandElementsMap.at(GNE_TAG_FLOW_WITHROUTE).size()) > 0)) {
            return true;
        }
        // check stops
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_STOP)) {
            for (const auto& demandElementTag : demandElementsMap) {
                for (const auto& demandElement : demandElementTag.second) {
                    for (const auto& stop : demandElement.second->getChildDemandElements()) {
                        if (stop->getTagProperty().isStop()) {
                            return true;
                        }
                    }
                }
            }
        }
        // check person
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSON) && ((demandElementsMap.at(SUMO_TAG_PERSON).size()) > 0)) {
            return true;
        }
        // check person flows
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONFLOW) && (demandElementsMap.at(SUMO_TAG_PERSONFLOW).size() > 0)) {
            return true;
        }
        // check persontrips
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONTRIP)) {
            if ((demandElementsMap.at(GNE_TAG_PERSONTRIP_EDGE).size() > 0) ||
                    (demandElementsMap.at(GNE_TAG_PERSONTRIP_BUSSTOP).size() > 0)) {
                return true;
            }
        }
        // check ride
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_RIDE)) {
            if ((demandElementsMap.at(GNE_TAG_RIDE_EDGE).size() > 0) ||
                    (demandElementsMap.at(GNE_TAG_RIDE_BUSSTOP).size() > 0)) {
                return true;
            }
        }
        // check walks
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_WALK)) {
            if ((demandElementsMap.at(GNE_TAG_WALK_EDGE).size() > 0) ||
                    (demandElementsMap.at(GNE_TAG_WALK_BUSSTOP).size() > 0) ||
                    (demandElementsMap.at(GNE_TAG_WALK_EDGES).size() > 0) ||
                    (demandElementsMap.at(GNE_TAG_WALK_ROUTE).size() > 0)) {
                return true;
            }
        }
        // check person stops
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_STOP_PERSON)) {
            if ((demandElementsMap.at(GNE_TAG_STOPPERSON_EDGE).size() > 0) ||
                    (demandElementsMap.at(GNE_TAG_STOPPERSON_BUSSTOP).size() > 0)) {
                return true;
            }
        }
    } else if (myViewNet->getEditModes().isCurrentSupermodeData()) {
        for (const auto& dataSet : myViewNet->getNet()->getAttributeCarriers()->getDataSets()) {
            for (const auto& dataInterval : dataSet.second->getDataIntervalChildren()) {
                for (const auto& genericData : dataInterval.second->getGenericDataChildren()) {
                    if ((!myLockGLObjectTypes->IsObjectTypeLocked(GLO_EDGEDATA) && (genericData->getType() == GLO_EDGEDATA)) ||
                            (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_EDGERELDATA) && (genericData->getType() == GLO_EDGERELDATA))) {
                        return true;
                    }
                }
            }
        }
    }
    // nothing to select
    return false;
}

/****************************************************************************/
