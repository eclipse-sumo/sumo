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
FXIMPLEMENT(GNESelectorFrame::ModificationMode,                     FXGroupBoxModul,     ModificationModeMap,            ARRAYNUMBER(ModificationModeMap))
FXIMPLEMENT(GNESelectorFrame::VisualScaling,                        FXGroupBoxModul,     VisualScalingMap,               ARRAYNUMBER(VisualScalingMap))
FXIMPLEMENT(GNESelectorFrame::SelectionOperation,                   FXGroupBoxModul,     SelectionOperationMap,          ARRAYNUMBER(SelectionOperationMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// ModificationMode::SelectionInformation - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::SelectionInformation::SelectionInformation(GNESelectorFrame* selectorFrameParent) :
    FXGroupBoxModul(selectorFrameParent->myContentFrame, "Selection information"),
    mySelectorFrameParent(selectorFrameParent) {
    // information label
    myInformationLabel = new FXLabel(this, "", nullptr, GUIDesignLabelFrameInformation);
}


GNESelectorFrame::SelectionInformation::~SelectionInformation() {}


void
GNESelectorFrame::SelectionInformation::updateInformationLabel() {
    // first clear information
    myInformation.clear();
    // get attribute carriers
    const auto ACs = mySelectorFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // continue depending of supermode
    if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        updateInformationLabel("Junctions", ACs->getNumberOfSelectedJunctions());
        updateInformationLabel("Edges", ACs->getNumberOfSelectedEdges());
        updateInformationLabel("Lanes", ACs->getNumberOfSelectedLanes());
        updateInformationLabel("Connections", ACs->getNumberOfSelectedConnections());
        updateInformationLabel("Crossings", ACs->getNumberOfSelectedCrossings());
        updateInformationLabel("Additionals", ACs->getNumberOfSelectedAdditionals());
        updateInformationLabel("TAZs", ACs->getNumberOfSelectedTAZs());
        updateInformationLabel("Polygon", ACs->getNumberOfSelectedPolygons());
        updateInformationLabel("POIs", ACs->getNumberOfSelectedPOIs());
    } else if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        updateInformationLabel("Routes", ACs->getNumberOfSelectedRoutes());
        updateInformationLabel("Vehicles", ACs->getNumberOfSelectedVehicles());
        updateInformationLabel("Persons", ACs->getNumberOfSelectedPersons());
        updateInformationLabel("Person trips", ACs->getNumberOfSelectedPersonTrips());
        updateInformationLabel("Walks", ACs->getNumberOfSelectedWalks());
        updateInformationLabel("Rides", ACs->getNumberOfSelectedRides());
        updateInformationLabel("Containers", ACs->getNumberOfSelectedContainers());
        updateInformationLabel("Transport", ACs->getNumberOfSelectedTransport());
        updateInformationLabel("Tranships", ACs->getNumberOfSelectedTranships());
        updateInformationLabel("Stops", ACs->getNumberOfSelectedStops());
    } else if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        updateInformationLabel("EdgeDatas", ACs->getNumberOfSelectedEdgeDatas());
        updateInformationLabel("EdgeRelDatas", ACs->getNumberOfSelectedEdgeRelDatas());
        updateInformationLabel("EdgeTAZRel", ACs->getNumberOfSelectedEdgeTAZRel());
    }
    // adjust format
    const auto numberLines = std::count(myInformation.begin(), myInformation.end(), ':');
    if (numberLines == 0) {
        myInformation.append(" \n \n");
    } else if (numberLines > 1) {
        myInformation.pop_back();
    }
    // set label
    myInformationLabel->setText(myInformation.c_str());
}


void
GNESelectorFrame::SelectionInformation::updateInformationLabel(const std::string& element, int number) {
    // check number
    if (number > 0) {
        myInformation.append(element + ": " + toString(number) + "\n");
    }
}

// ---------------------------------------------------------------------------
// ModificationMode::ModificationMode - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::ModificationMode::ModificationMode(GNESelectorFrame* selectorFrameParent) :
    FXGroupBoxModul(selectorFrameParent->myContentFrame, "Modification Mode"),
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
    FXGroupBoxModul(selectorFrameParent->myContentFrame, "Visual Scaling"),
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
    FXGroupBoxModul(selectorFrameParent->myContentFrame, "Operations for selections"),
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
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_CONFIG));
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
                if ((object != nullptr) && !mySelectorFrameParent->getViewNet()->getLockManager().isObjectLocked(object->getType(), false)) {
                    // obtain GNEAttributeCarrier
                    GNEAttributeCarrier* AC = mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->retrieveAttributeCarrier(object->getGlID(), false);
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
            mySelectorFrameParent->myViewNet->getUndoList()->begin(GUIIcon::MODESELECT, "load selection");
            mySelectorFrameParent->handleIDs(loadedACs);
            mySelectorFrameParent->myViewNet->getUndoList()->end();
        }
    }
    mySelectorFrameParent->myViewNet->updateViewNet();
    return 1;
}


long
GNESelectorFrame::SelectionOperation::onCmdSave(FXObject*, FXSelector, void*) {
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save List of selected Items", ".txt",
                    GUIIconSubSys::getIcon(GUIIcon::SAVE), gCurrentFolder);
    if (file == "") {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(file.text());
        // get selected attribute carriers
        const auto selectedACs = mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getSelectedAttributeCarriers(false);
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
    bool ignoreLocking = false;
    // only continue if there is element for selecting
    if ((mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork() && processNetworkElementSelection(true, false, ignoreLocking)) ||
            (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand() &&  processDemandElementSelection(true, false, ignoreLocking)) ||
            (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData() && processDataElementSelection(true, false, ignoreLocking))) {
        // for invert selection, first clean current selection and next select elements of set "unselectedElements"
        mySelectorFrameParent->myViewNet->getUndoList()->begin(GUIIcon::MODESELECT, "invert selection");
        // invert selection of elements depending of current supermode
        if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            processNetworkElementSelection(false, true, ignoreLocking);
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand()) {
            processDemandElementSelection(false, true, ignoreLocking);
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData()) {
            processDataElementSelection(false, true, ignoreLocking);
        }
        // finish selection operation
        mySelectorFrameParent->myViewNet->getUndoList()->end();
    }
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
    bool ignoreLocking = false;
    // only continue if there is element for selecting
    if ((mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork() && processNetworkElementSelection(true, false, ignoreLocking)) ||
            (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand() &&  processDemandElementSelection(true, false, ignoreLocking)) ||
            (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData() && processDataElementSelection(true, false, ignoreLocking))) {
        // for invert selection, first clean current selection and next select elements of set "unselectedElements"
        mySelectorFrameParent->myViewNet->getUndoList()->begin(GUIIcon::MODESELECT, "invert selection");
        // invert selection of elements depending of current supermode
        if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            // invert network elements
            processNetworkElementSelection(false, false, ignoreLocking);
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand()) {
            // invert demand elements
            processDemandElementSelection(false, false, ignoreLocking);
        } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData()) {
            // invert data elements
            processDataElementSelection(false, false, ignoreLocking);
        }
        // finish selection operation
        mySelectorFrameParent->myViewNet->getUndoList()->end();
    }
    return 1;
}


bool
GNESelectorFrame::SelectionOperation::processNetworkElementSelection(const bool onlyCount, const bool onlyUnselect, bool& ignoreLocking) {
    // obtan locks (only for improve code legibly)
    const auto& locks = mySelectorFrameParent->getViewNet()->getLockManager();
    // get attribute carriers (only for improve code legibly)
    const auto& ACs = mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers();
    // obtain undoList (only for improve code legibly)
    GNEUndoList* undoList = mySelectorFrameParent->myViewNet->getUndoList();
    // iterate over junctions
    for (const auto& junction : ACs->getJunctions()) {
        // check if junction selection is locked
        if (ignoreLocking || !locks.isObjectLocked(GLO_JUNCTION, false)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || junction.second->isAttributeCarrierSelected()) {
                junction.second->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                junction.second->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        } else if (onlyCount) {
            ignoreLocking = askContinueIfLock();
            return true;
        }
        // due we iterate over all junctions, only it's neccesary iterate over incoming edges
        for (const auto& incomingEdge : junction.second->getGNEIncomingEdges()) {
            // special case for clear
            if (onlyUnselect) {
                // check if edge selection is locked
                if (ignoreLocking || !locks.isObjectLocked(GLO_EDGE, false)) {
                    if (onlyCount) {
                        return true;
                    } else {
                        incomingEdge->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    }
                } else if (onlyCount) {
                    ignoreLocking = askContinueIfLock();
                    return true;
                }
                // check if lane selection is locked
                if (ignoreLocking || !locks.isObjectLocked(GLO_LANE, false)) {
                    for (const auto& lane : incomingEdge->getLanes()) {
                        if (onlyCount) {
                            return true;
                        } else {
                            lane->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                        }
                    }
                } else if (onlyCount) {
                    ignoreLocking = askContinueIfLock();
                    return true;
                }
            } else if (mySelectorFrameParent->myViewNet->getNetworkViewOptions().selectEdges()) {
                // check if edge selection is locked
                if (ignoreLocking || !locks.isObjectLocked(GLO_EDGE, false)) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || incomingEdge->isAttributeCarrierSelected()) {
                        incomingEdge->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        incomingEdge->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                } else if (onlyCount) {
                    ignoreLocking = askContinueIfLock();
                    return true;
                }
            } else {
                // check if lane selection is locked
                if (ignoreLocking || !locks.isObjectLocked(GLO_LANE, false)) {
                    for (const auto& lane : incomingEdge->getLanes()) {
                        if (onlyCount) {
                            return true;
                        } else if (onlyUnselect || lane->isAttributeCarrierSelected()) {
                            lane->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                        } else {
                            lane->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                        }
                    }
                } else if (onlyCount) {
                    ignoreLocking = askContinueIfLock();
                    return true;
                }
            }
            // check if connection selection is locked
            if (ignoreLocking || !locks.isObjectLocked(GLO_CONNECTION, false)) {
                for (const auto& connection : incomingEdge->getGNEConnections()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || connection->isAttributeCarrierSelected()) {
                        connection->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        connection->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            } else if (onlyCount) {
                ignoreLocking = askContinueIfLock();
                return true;
            }
        }
        // check if crossing selection is locked
        if (ignoreLocking || !locks.isObjectLocked(GLO_CROSSING, false)) {
            for (const auto& crossing : junction.second->getGNECrossings()) {
                if (onlyCount) {
                    return true;
                } else if (onlyUnselect || crossing->isAttributeCarrierSelected()) {
                    crossing->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                } else {
                    crossing->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                }
            }
        } else if (onlyCount) {
            ignoreLocking = askContinueIfLock();
            return true;
        }
    }
    // check if additionals selection is locked
    if (ignoreLocking || !locks.isObjectLocked(GLO_ADDITIONALELEMENT, false)) {
        for (const auto& additionalTag : ACs->getAdditionals()) {
            // first check if additional is selectable
            if (GNEAttributeCarrier::getTagProperty(additionalTag.first).isSelectable()) {
                for (const auto& additional : additionalTag.second) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || additional->isAttributeCarrierSelected()) {
                        additional->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        additional->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert polygons
    if (ignoreLocking || !locks.isObjectLocked(GLO_POLYGON, false)) {
        for (const auto& polygon : ACs->getShapes().at(SUMO_TAG_POLY)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || polygon->isAttributeCarrierSelected()) {
                polygon->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                polygon->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert TAZs
    if (ignoreLocking || !locks.isObjectLocked(GLO_TAZ, false)) {
        for (const auto& TAZ : ACs->getTAZElements().at(SUMO_TAG_TAZ)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || TAZ->isAttributeCarrierSelected()) {
                TAZ->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                TAZ->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert POIs and POILanes
    if (ignoreLocking || !locks.isObjectLocked(GLO_POI, false)) {
        for (const auto& shapeTag : ACs->getShapes()) {
            if (shapeTag.first != SUMO_TAG_POLY) {
                for (const auto& POI : shapeTag.second) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || POI->isAttributeCarrierSelected()) {
                        POI->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        POI->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            } else if (onlyCount) {
                ignoreLocking = askContinueIfLock();
                return true;
            }
        }
    }
    return false;
}


bool
GNESelectorFrame::SelectionOperation::processDemandElementSelection(const bool onlyCount, const bool onlyUnselect, bool& ignoreLocking) {
    // obtan locks (only for improve code legibly)
    const auto& locks = mySelectorFrameParent->getViewNet()->getLockManager();
    // obtain undoList (only for improve code legibly)
    GNEUndoList* undoList = mySelectorFrameParent->myViewNet->getUndoList();
    // get demand elements
    const auto& demandElements = mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getDemandElements();
    // invert routes
    if (ignoreLocking || !locks.isObjectLocked(GLO_ROUTE, false)) {
        for (const auto& route : demandElements.at(SUMO_TAG_ROUTE)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || route->isAttributeCarrierSelected()) {
                route->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                route->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        // iterate over all embedded routes
        for (const auto& vehicle : demandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || vehicle->getChildDemandElements().front()->isAttributeCarrierSelected()) {
                vehicle->getChildDemandElements().front()->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                vehicle->getChildDemandElements().front()->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& routeFlow : demandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || routeFlow->getChildDemandElements().front()->isAttributeCarrierSelected()) {
                routeFlow->getChildDemandElements().front()->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                routeFlow->getChildDemandElements().front()->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert vehicles
    if (ignoreLocking || !locks.isObjectLocked(GLO_VEHICLE, false)) {
        for (const auto& vehicle : demandElements.at(SUMO_TAG_VEHICLE)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || vehicle->isAttributeCarrierSelected()) {
                vehicle->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                vehicle->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& vehicle : demandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || vehicle->isAttributeCarrierSelected()) {
                vehicle->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                vehicle->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& trip : demandElements.at(SUMO_TAG_TRIP)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || trip->isAttributeCarrierSelected()) {
                trip->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                trip->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& flow : demandElements.at(SUMO_TAG_FLOW)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || flow->isAttributeCarrierSelected()) {
                flow->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                flow->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& routeFlow : demandElements.at(GNE_TAG_FLOW_ROUTE)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || routeFlow->isAttributeCarrierSelected()) {
                routeFlow->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                routeFlow->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& routeFlow : demandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || routeFlow->isAttributeCarrierSelected()) {
                routeFlow->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                routeFlow->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert persons
    if (ignoreLocking || !locks.isObjectLocked(GLO_PERSON, false)) {
        for (const auto& person : demandElements.at(SUMO_TAG_PERSON)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || person->isAttributeCarrierSelected()) {
                person->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                person->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& personFlow : demandElements.at(SUMO_TAG_PERSONFLOW)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || personFlow->isAttributeCarrierSelected()) {
                personFlow->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                personFlow->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert person trip
    if (ignoreLocking || !locks.isObjectLocked(GLO_PERSONTRIP, false)) {
        for (const auto& person : demandElements.at(SUMO_TAG_PERSON)) {
            for (const auto& personPlan : person->getChildDemandElements()) {
                if (onlyCount) {
                    return true;
                } else if (personPlan->getTagProperty().isPersonTrip()) {
                    if (onlyUnselect || personPlan->isAttributeCarrierSelected()) {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
        for (const auto& personFlow : demandElements.at(SUMO_TAG_PERSONFLOW)) {
            for (const auto& personPlan : personFlow->getChildDemandElements()) {
                if (onlyCount) {
                    return true;
                } else if (personPlan->getTagProperty().isPersonTrip()) {
                    if (onlyUnselect || personPlan->isAttributeCarrierSelected()) {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert ride
    if (ignoreLocking || !locks.isObjectLocked(GLO_PERSONTRIP, false)) {
        for (const auto& person : demandElements.at(SUMO_TAG_PERSON)) {
            for (const auto& personPlan : person->getChildDemandElements()) {
                if (personPlan->getTagProperty().isRide()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || personPlan->isAttributeCarrierSelected()) {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
        for (const auto& personFlow : demandElements.at(SUMO_TAG_PERSONFLOW)) {
            for (const auto& personPlan : personFlow->getChildDemandElements()) {
                if (personPlan->getTagProperty().isRide()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || personPlan->isAttributeCarrierSelected()) {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert walks
    if (ignoreLocking || !locks.isObjectLocked(GLO_PERSONTRIP, false)) {
        for (const auto& person : demandElements.at(SUMO_TAG_PERSON)) {
            for (const auto& personPlan : person->getChildDemandElements()) {
                if (personPlan->getTagProperty().isWalk()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || personPlan->isAttributeCarrierSelected()) {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
        for (const auto& personFlow : demandElements.at(SUMO_TAG_PERSONFLOW)) {
            for (const auto& personPlan : personFlow->getChildDemandElements()) {
                if (personPlan->getTagProperty().isWalk()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || personPlan->isAttributeCarrierSelected()) {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        personPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert containers
    if (ignoreLocking || !locks.isObjectLocked(GLO_CONTAINER, false)) {
        for (const auto& container : demandElements.at(SUMO_TAG_CONTAINER)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || container->isAttributeCarrierSelected()) {
                container->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                container->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
        for (const auto& containerFlow : demandElements.at(SUMO_TAG_CONTAINERFLOW)) {
            if (onlyCount) {
                return true;
            } else if (onlyUnselect || containerFlow->isAttributeCarrierSelected()) {
                containerFlow->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
            } else {
                containerFlow->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert container
    if (ignoreLocking || !locks.isObjectLocked(GLO_TRANSPORT, false)) {
        for (const auto& container : demandElements.at(SUMO_TAG_CONTAINER)) {
            for (const auto& containerPlan : container->getChildDemandElements()) {
                if (containerPlan->getTagProperty().isTransportPlan()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || containerPlan->isAttributeCarrierSelected()) {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
        for (const auto& containerFlow : demandElements.at(SUMO_TAG_CONTAINERFLOW)) {
            for (const auto& containerPlan : containerFlow->getChildDemandElements()) {
                if (containerPlan->getTagProperty().isTransportPlan()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || containerPlan->isAttributeCarrierSelected()) {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert ride
    if (ignoreLocking || !locks.isObjectLocked(GLO_TRANSHIP, false)) {
        for (const auto& container : demandElements.at(SUMO_TAG_CONTAINER)) {
            for (const auto& containerPlan : container->getChildDemandElements()) {
                if (containerPlan->getTagProperty().isTranshipPlan()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || containerPlan->isAttributeCarrierSelected()) {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
        for (const auto& containerFlow : demandElements.at(SUMO_TAG_CONTAINERFLOW)) {
            for (const auto& containerPlan : containerFlow->getChildDemandElements()) {
                if (containerPlan->getTagProperty().isTranshipPlan()) {
                    if (onlyCount) {
                        return true;
                    } else if (onlyUnselect || containerPlan->isAttributeCarrierSelected()) {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                    } else {
                        containerPlan->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                    }
                }
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    // invert stops
    if (ignoreLocking || !locks.isObjectLocked(GLO_STOP, false)) {
        for (const auto& demandElementTag : demandElements) {
            for (const auto& demandElement : demandElementTag.second) {
                // avoid vTypes
                if (!demandElement->getTagProperty().isVehicleType()) {
                    // iterate over every child
                    for (const auto& stop : demandElement->getChildDemandElements()) {
                        if (stop->getTagProperty().isStop() || stop->getTagProperty().isStopPerson() || stop->getTagProperty().isStopContainer()) {
                            if (onlyCount) {
                                return true;
                            } else if (onlyUnselect || stop->isAttributeCarrierSelected()) {
                                stop->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                            } else {
                                stop->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                            }
                        } else {
                            // special case for embedded routes
                            for (const auto& stopEmbeddedRoute : stop->getChildDemandElements()) {
                                if (stopEmbeddedRoute->getTagProperty().isStop() ||
                                        stopEmbeddedRoute->getTagProperty().isStopPerson() ||
                                        stopEmbeddedRoute->getTagProperty().isStopContainer()) {
                                    if (onlyCount) {
                                        return true;
                                    } else if (onlyUnselect || stopEmbeddedRoute->isAttributeCarrierSelected()) {
                                        stopEmbeddedRoute->setAttribute(GNE_ATTR_SELECTED, "false", undoList);
                                    } else {
                                        stopEmbeddedRoute->setAttribute(GNE_ATTR_SELECTED, "true", undoList);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (onlyCount) {
        ignoreLocking = askContinueIfLock();
        return true;
    }
    return false;
}


bool
GNESelectorFrame::SelectionOperation::processDataElementSelection(const bool onlyCount, const bool onlyUnselect, bool& ignoreLocking) {
    // obtan locks (only for improve code legibly)
    const auto& locks = mySelectorFrameParent->getViewNet()->getLockManager();
    // invert dataSets
    for (const auto& genericDataTag : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->getGenericDatas()) {
        for (const auto& genericData : genericDataTag.second) {
            if (onlyCount && locks.isObjectLocked(genericData->getType(), false)) {
                ignoreLocking = askContinueIfLock();
                return true;
            } else if ((ignoreLocking || (!locks.isObjectLocked(GLO_EDGEDATA, false) && genericData->getType() == GLO_EDGEDATA)) ||
                       (ignoreLocking || (!locks.isObjectLocked(GLO_EDGERELDATA, false) && genericData->getType() == GLO_EDGERELDATA)) ||
                       (ignoreLocking || (!locks.isObjectLocked(GLO_TAZRELDATA, false) && genericData->getType() == GLO_TAZRELDATA))) {
                if (onlyCount) {
                    return true;
                } else if (onlyUnselect || genericData->isAttributeCarrierSelected()) {
                    genericData->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                } else {
                    genericData->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                }
            }
        }
    }
    return false;
}


bool
GNESelectorFrame::SelectionOperation::askContinueIfLock() const {
    WRITE_DEBUG("Opening FXMessageBox 'merge junctions'");
    // open question box
    const FXuint answer = FXMessageBox::question(mySelectorFrameParent->getViewNet()->getApp(),
                          MBOX_YES_NO, "Confirm selection operation", "There are locked elements in currentselection.\nApply operation to locked elements?");
    if (answer != 1) { //1:yes, 2:no, 4:esc
        // write warning if netedit is running in testing mode
        if (answer == 2) {
            WRITE_DEBUG("Closed FXMessageBox 'merge junctions' with 'No'");
        } else if (answer == 4) {
            WRITE_DEBUG("Closed FXMessageBox 'merge junctions' with 'ESC'");
        }
        return false;
    } else {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'merge junctions' with 'Yes'");
        return true;
    }
}

// ---------------------------------------------------------------------------
// GNECrossingFrame::Legend - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::Information::Information(GNESelectorFrame* selectorFrameParent) :
    FXGroupBoxModul(selectorFrameParent->myContentFrame, "Information") {
    // Create Selection Hint
    new FXLabel(this, " - Hold <SHIFT> for \n   rectangle selection.\n - Press <DEL> to\n   delete selected objects.", nullptr, GUIDesignLabelFrameInformation);
}


GNESelectorFrame::Information::~Information() {}

// ---------------------------------------------------------------------------
// GNESelectorFrame - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Selection") {
    // create selection information
    mySelectionInformation = new SelectionInformation(this);
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
    // create Information modul
    myInformation = new Information(this);
}


GNESelectorFrame::~GNESelectorFrame() {}


void
GNESelectorFrame::show() {
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
    // update information label
    mySelectionInformation->updateInformationLabel();
    // Show frame
    GNEFrame::show();
}


void
GNESelectorFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


void
GNESelectorFrame::updateFrameAfterUndoRedo() {
    // update information label
    mySelectionInformation->updateInformationLabel();
}


void
GNESelectorFrame::clearCurrentSelection() const {
    mySelectionOperation->onCmdClear(nullptr, 0, nullptr);
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
        std::vector<GNEAttributeCarrier*> selectedACs = myViewNet->getNet()->getAttributeCarriers()->getSelectedAttributeCarriers(false);
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
                edgesToSelect.insert(myViewNet->getNet()->getAttributeCarriers()->retrieveEdge(AC.second->getID()));
            }
        }
        // iterate over extracted edges
        for (const auto& edgeToSelect : edgesToSelect) {
            // select junction source and all connections and crossings
            ACsToSelect.insert(std::make_pair(edgeToSelect->getFromJunction()->getID(), edgeToSelect->getFromJunction()));
            for (const auto& connectionToSelect : edgeToSelect->getFromJunction()->getGNEConnections()) {
                ACsToSelect.insert(std::make_pair(connectionToSelect->getID(), connectionToSelect));
            }
            for (const auto& crossingToSelect : edgeToSelect->getFromJunction()->getGNECrossings()) {
                ACsToSelect.insert(std::make_pair(crossingToSelect->getID(), crossingToSelect));
            }
            // select junction destiny and all connections and crossings
            ACsToSelect.insert(std::make_pair(edgeToSelect->getToJunction()->getID(), edgeToSelect->getToJunction()));
            for (const auto& connectionToSelect : edgeToSelect->getToJunction()->getGNEConnections()) {
                ACsToSelect.insert(std::make_pair(connectionToSelect->getID(), connectionToSelect));
            }
            for (const auto& crossingToSelect : edgeToSelect->getToJunction()->getGNECrossings()) {
                ACsToSelect.insert(std::make_pair(crossingToSelect->getID(), crossingToSelect));
            }
        }
    }
    // only continue if there is ACs to select or unselect
    if ((ACsToSelect.size() + ACsToUnselect.size()) > 0) {
        // first unselect AC of ACsToUnselect and then selects AC of ACsToSelect
        myViewNet->getUndoList()->begin(GUIIcon::MODESELECT, "selection");
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
        myViewNet->getUndoList()->end();
    }
}


std::vector<GNEAttributeCarrier*>
GNESelectorFrame::getMatches(const SumoXMLTag ACTag, const SumoXMLAttr ACAttr, const char compOp, const double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    // first retrieve all ACs using ACTag
    const auto allACbyTag = myViewNet->getNet()->getAttributeCarriers()->retrieveAttributeCarriers(ACTag);
    // get Tag value
    const auto& tagValue = GNEAttributeCarrier::getTagProperty(ACTag);
    // iterate over all ACs
    for (const auto& AC : allACbyTag) {
        if (expr == "" && compOp == '@') {
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
        if (expr == "" && compOp == '@') {
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


FXVerticalFrame*
GNESelectorFrame::getContentFrame() const {
    return myContentFrame;
}


GNESelectorFrame::ModificationMode*
GNESelectorFrame::getModificationModeModul() const {
    return myModificationMode;
}


GNESelectorFrame::SelectionInformation*
GNESelectorFrame::getSelectionInformation() const {
    return mySelectionInformation;
}

/****************************************************************************/
