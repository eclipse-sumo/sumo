/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/demandelements/GNERoute.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNESelectorFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry) ObjectTypeEntryMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::onCmdSetCheckBox)
};

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
FXIMPLEMENT(GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry,   FXObject,       ObjectTypeEntryMap,     ARRAYNUMBER(ObjectTypeEntryMap))
FXIMPLEMENT(GNESelectorFrame::ModificationMode,                     FXGroupBox,     ModificationModeMap,    ARRAYNUMBER(ModificationModeMap))
FXIMPLEMENT(GNESelectorFrame::ElementSet,                           FXGroupBox,     ElementSetMap,          ARRAYNUMBER(ElementSetMap))
FXIMPLEMENT(GNESelectorFrame::MatchAttribute,                       FXGroupBox,     MatchAttributeMap,      ARRAYNUMBER(MatchAttributeMap))
FXIMPLEMENT(GNESelectorFrame::VisualScaling,                        FXGroupBox,     VisualScalingMap,       ARRAYNUMBER(VisualScalingMap))
FXIMPLEMENT(GNESelectorFrame::SelectionOperation,                   FXGroupBox,     SelectionOperationMap,  ARRAYNUMBER(SelectionOperationMap))

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
    new FXLabel(selectionHintGroupBox, " - Hold <SHIFT> for \n   rectangle selection.\n - Press <DEL> to\n   delete selected items.", nullptr, GUIDesignLabelFrameInformation);

}


GNESelectorFrame::~GNESelectorFrame() {}


void
GNESelectorFrame::show() {
    // show Type Entries depending of current supermode
    myLockGLObjectTypes->showTypeEntries();
    // refresh element set
    myElementSet->refreshElementSet();
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
        if (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
            // iterate over junctions
            for (const auto& i : myViewNet->getNet()->getAttributeCarriers().junctions) {
                // check if junction selection is locked
                if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_JUNCTION)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                // due we iterate over all junctions, only it's neccesary iterate over incoming edges
                for (const auto& j : i.second->getGNEIncomingEdges()) {
                    // check if edge selection is locked
                    if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_EDGE)) {
                        if (j->isAttributeCarrierSelected()) {
                            j->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                    // check if lane selection is locked
                    if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_LANE)) {
                        for (auto k : j->getLanes()) {
                            if (k->isAttributeCarrierSelected()) {
                                k->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                            }
                        }
                    }
                    // check if connection selection is locked
                    if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_CONNECTION)) {
                        for (const auto& k : j->getGNEConnections()) {
                            if (k->isAttributeCarrierSelected()) {
                                k->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                            }
                        }
                    }
                }
                // check if crossing selection is locked
                if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_CROSSING)) {
                    for (const auto& j : i.second->getGNECrossings()) {
                        if (j->isAttributeCarrierSelected()) {
                            j->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                        }
                    }
                }
            }
            // check if additionals selection is locked
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ADDITIONAL)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().additionals) {
                    // first check if additional is selectable
                    if (GNEAttributeCarrier::getTagProperties(i.first).isSelectable()) {
                        for (const auto& j : i.second) {
                            if (j.second->isAttributeCarrierSelected()) {
                                j.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                            }
                        }
                    }
                }
            }
            // select polygons
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POLYGON)) {
                for (const auto& i : myViewNet->getNet()->getPolygons()) {
                    GNEShape* shape = dynamic_cast<GNEShape*>(i.second);
                    if (shape->isAttributeCarrierSelected()) {
                        shape->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select POIs
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POI)) {
                for (const auto& i : myViewNet->getNet()->getPOIs()) {
                    GNEShape* shape = dynamic_cast<GNEShape*>(i.second);
                    if (shape->isAttributeCarrierSelected()) {
                        shape->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
        } else {
            // select routes
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTE)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select embedded route
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_EMBEDDEDROUTE)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_EMBEDDEDROUTE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select vehicles
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_VEHICLE)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VEHICLE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select trips
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_TRIP)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_TRIP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select flows
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_FLOW)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_FLOW)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select route flows
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTEFLOW)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTEFLOW)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select stops
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_STOP)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_LANE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CONTAINERSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CHARGINGSTATION)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_PARKINGAREA)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select person
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSON)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSON)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select person flows
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONFLOW)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONFLOW)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select person trips
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONTRIP)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONTRIP_FROMTO)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONTRIP_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select ride
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_RIDE)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_RIDE_FROMTO)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_RIDE_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select walks
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_WALK)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_FROMTO)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_ROUTE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
            // select person stops
            if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONSTOP)) {
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONSTOP_LANE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
                for (const auto& i : myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONSTOP_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
                    }
                }
            }
        }
        // finish selection operation
        myViewNet->getUndoList()->p_end();
        // update view
        myViewNet->update();
    }
}


void
GNESelectorFrame::handleIDs(const std::vector<GNEAttributeCarrier*>& ACs, ModificationMode::SetOperation setop) {
    const ModificationMode::SetOperation setOperation = ((setop == ModificationMode::SET_DEFAULT) ? myModificationMode->getModificationMode() : setop);
    // declare two sets of attribute carriers, one for select and another for unselect
    std::set<std::pair<std::string, GNEAttributeCarrier*> > ACToSelect;
    std::set<std::pair<std::string, GNEAttributeCarrier*> > ACToUnselect;
    // in restrict AND replace mode all current selected attribute carriers will be unselected
    if ((setOperation == ModificationMode::SET_REPLACE) || (setOperation == ModificationMode::SET_RESTRICT)) {
        // obtain selected ACs depending of current supermode
        std::vector<GNEAttributeCarrier*> selectedAC = myViewNet->getNet()->getSelectedAttributeCarriers(false);
        // add id into ACs to unselect
        for (auto i : selectedAC) {
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
            if (i.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
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
            if (i.second->getTagProperty().isSelectable()) {
                i.second->setAttribute(GNE_ATTR_SELECTED, "false", myViewNet->getUndoList());
            }
        }
        for (auto i : ACToSelect) {
            if (i.second->getTagProperty().isSelectable()) {
                i.second->setAttribute(GNE_ATTR_SELECTED, "true", myViewNet->getUndoList());
            }
        }
        // finish operation
        myViewNet->getUndoList()->p_end();
    }
    // update view
    myViewNet->update();
}


GNESelectorFrame::ModificationMode*
GNESelectorFrame::getModificationModeModul() const {
    return myModificationMode;
}


GNESelectorFrame::LockGLObjectTypes*
GNESelectorFrame::getLockGLObjectTypes() const {
    return myLockGLObjectTypes;
}


std::vector<GNEAttributeCarrier*>
GNESelectorFrame::getMatches(SumoXMLTag ACTag, SumoXMLAttr ACAttr, char compOp, double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    std::vector<GNEAttributeCarrier*> allACbyTag = myViewNet->getNet()->retrieveAttributeCarriers(ACTag);
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(ACTag);
    for (auto it : allACbyTag) {
        if (expr == "") {
            result.push_back(it);
        } else if (tagValue.hasAttribute(ACAttr) && tagValue.getAttributeProperties(ACAttr).isNumerical()) {
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
    FXGroupBox(selectorFrameParent->myContentFrame, "Locked selected items", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent) {
    // create a matrix for TypeEntries
    FXMatrix* matrixLockGLObjectTypes = new FXMatrix(this, 3, GUIDesignMatrixLockGLTypes);
    // create typeEntries for the different Network elements
    myTypeEntries[GLO_JUNCTION] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "Junctions"));
    myTypeEntries[GLO_EDGE] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "Edges"));
    myTypeEntries[GLO_LANE] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "Lanes"));
    myTypeEntries[GLO_CONNECTION] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "Connections"));
    myTypeEntries[GLO_ADDITIONAL] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "Additionals"));
    myTypeEntries[GLO_CROSSING] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "Crossings"));
    myTypeEntries[GLO_POLYGON] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "Polygons"));
    myTypeEntries[GLO_POI] = std::make_pair(Supermode::GNE_SUPERMODE_NETWORK, new ObjectTypeEntry(matrixLockGLObjectTypes, "POIs"));
    // create typeEntries for the different Demand elements
    myTypeEntries[GLO_ROUTE] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Routes"));
    myTypeEntries[GLO_EMBEDDEDROUTE] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Emb.Routes"));
    myTypeEntries[GLO_VEHICLE] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Vehicles"));
    myTypeEntries[GLO_ROUTEFLOW] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Flows"));
    myTypeEntries[GLO_TRIP] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Trips"));
    myTypeEntries[GLO_FLOW] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Flow"));
    myTypeEntries[GLO_STOP] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Stops"));
    myTypeEntries[GLO_PERSON] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Person"));
    myTypeEntries[GLO_PERSONFLOW] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "PersonFlow"));
    myTypeEntries[GLO_PERSONTRIP] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "PersonTrip"));
    myTypeEntries[GLO_RIDE] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Ride"));
    myTypeEntries[GLO_WALK] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Walk"));
    myTypeEntries[GLO_PERSONSTOP] = std::make_pair(Supermode::GNE_SUPERMODE_DEMAND, new ObjectTypeEntry(matrixLockGLObjectTypes, "Personstop"));
}


GNESelectorFrame::LockGLObjectTypes::~LockGLObjectTypes() {
    // remove all type entries
    for (const auto& i : myTypeEntries) {
        delete i.second.second;
    }
}


void
GNESelectorFrame::LockGLObjectTypes::addedLockedObject(const GUIGlObjectType type) {
    myTypeEntries.at(type).second->counterUp();
}


void
GNESelectorFrame::LockGLObjectTypes::removeLockedObject(const GUIGlObjectType type) {
    myTypeEntries.at(type).second->counterDown();
}


bool
GNESelectorFrame::LockGLObjectTypes::IsObjectTypeLocked(const GUIGlObjectType type) const {
    if ((type >= 100) && (type < 199)) {
        return myTypeEntries.at(GLO_ADDITIONAL).second->isGLTypeLocked();
    } else {
        return myTypeEntries.at(type).second->isGLTypeLocked();
    }
}


void
GNESelectorFrame::LockGLObjectTypes::showTypeEntries() {
    for (const auto& i : myTypeEntries) {
        // showr or hidde type entries depending of current supermode
        if (i.second.first == mySelectorFrameParent->myViewNet->getEditModes().currentSupermode) {
            i.second.second->showObjectTypeEntry();
        } else {
            i.second.second->hideObjectTypeEntry();
        }
    }
    // recalc frame parent
    recalc();
}


GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry::ObjectTypeEntry(FXMatrix* matrixParent, const std::string& label) :
    FXObject(),
    myCounter(0) {
    // create elements
    myLabelCounter = new FXLabel(matrixParent, "0", nullptr, GUIDesignLabelLeft);
    myLabelTypeName = new FXLabel(matrixParent, (label + " ").c_str(), nullptr, GUIDesignLabelLeft);
    myCheckBoxLocked = new FXCheckButton(matrixParent, "unlocked", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
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

// ---------------------------------------------------------------------------
// ModificationMode::ModificationMode - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::ModificationMode::ModificationMode(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Modification Mode", GUIDesignGroupBoxFrame),
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
}


GNESelectorFrame::ElementSet::~ElementSet() {}


GNESelectorFrame::ElementSet::ElementSetType
GNESelectorFrame::ElementSet::getElementSet() const {
    return myCurrentElementSet;
}


void
GNESelectorFrame::ElementSet::refreshElementSet() {
    // first clear item
    mySetComboBox->clearItems();
    // now fill elements depending of supermode
    if (mySelectorFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
        mySetComboBox->appendItem("Net Element");
        mySetComboBox->appendItem("Additional");
        mySetComboBox->appendItem("Shape");
    } else {
        mySetComboBox->appendItem("Demand Element");
    }
    mySetComboBox->setNumVisible(mySetComboBox->getNumItems());
    // update rest of elements
    onCmdSelectElementSet(0, 0, 0);
}


long
GNESelectorFrame::ElementSet::onCmdSelectElementSet(FXObject*, FXSelector, void*) {
    // check depending of current supermode
    if (mySelectorFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
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
    } else {
        if (mySetComboBox->getText() == "Demand Element") {
            myCurrentElementSet = ELEMENTSET_DEMANDELEMENT;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
            // enable match attribute
            mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
        } else {
            myCurrentElementSet = ELEMENTSET_INVALID;
            mySetComboBox->setTextColor(FXRGB(255, 0, 0));
            // disable match attribute
            mySelectorFrameParent->myMatchAttribute->disableMatchAttribute();
        }
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
    new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
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
    std::vector<SumoXMLTag> listOfTags;
    if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_NETELEMENT) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_NETELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_ADDITIONAL) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL | GNEAttributeCarrier::TagType::TAGTYPE_TAZ, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_SHAPE) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_SHAPE, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_DEMANDELEMENT) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT | GNEAttributeCarrier::TagType::TAGTYPE_STOP, true);
    } else {
        throw ProcessError("Invalid element set");
    }
    // fill combo box
    for (auto i : listOfTags) {
        myMatchTagComboBox->appendItem(toString(i).c_str());
    }
    // set first item as current item
    myMatchTagComboBox->setCurrentItem(0);
    myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
    // Fill attributes with the current element type
    onCmdSelMBTag(nullptr, 0, nullptr);
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
    std::vector<SumoXMLTag> listOfTags;
    if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_NETELEMENT) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_NETELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_ADDITIONAL) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL | GNEAttributeCarrier::TagType::TAGTYPE_TAZ, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_SHAPE) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_SHAPE, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == ElementSet::ELEMENTSET_DEMANDELEMENT) {
        listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT | GNEAttributeCarrier::TagType::TAGTYPE_STOP, true);
    } else {
        throw ProcessError("Unkown set");
    }
    // fill myMatchTagComboBox
    for (auto i : listOfTags) {
        if (toString(i) == myMatchTagComboBox->getText().text()) {
            myCurrentTag = i;
        }
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
            myMatchAttrComboBox->appendItem(it.getAttrStr().c_str());
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
        onCmdSelMBAttribute(nullptr, 0, nullptr);
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
    GNEAttributeCarrier::AttributeProperties extraAttrProperty;
    extraAttrProperty = GNEAttributeCarrier::AttributeProperties(GNE_ATTR_GENERIC,
                        GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_STRING,
                        "Generic Parameters");
    tagPropertiesCopy.addAttribute(extraAttrProperty);
    // add extra attribute if item can block movement
    if (tagValue.canBlockMovement()) {
        // add an extra AttributeValues to allow select ACs using as criterium "block movement"
        extraAttrProperty = GNEAttributeCarrier::AttributeProperties(GNE_ATTR_BLOCK_MOVEMENT,
                            GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_BOOL | GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_DEFAULTVALUESTATIC,
                            "Block movement",
                            "false");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // add extra attribute if item can block shape
    if (tagValue.canBlockShape()) {
        // add an extra AttributeValues to allow select ACs using as criterium "block shape"
        extraAttrProperty = GNEAttributeCarrier::AttributeProperties(GNE_ATTR_BLOCK_SHAPE,
                            GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_BOOL | GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_DEFAULTVALUESTATIC,
                            "Block shape",
                            "false");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // add extra attribute if item can close shape
    if (tagValue.canCloseShape()) {
        // add an extra AttributeValues to allow select ACs using as criterium "close shape"
        extraAttrProperty = GNEAttributeCarrier::AttributeProperties(GNE_ATTR_CLOSE_SHAPE,
                            GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_BOOL | GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_DEFAULTVALUESTATIC,
                            "Close shape",
                            "true");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // add extra attribute if item can have parent
    if (tagValue.hasParent()) {
        // add an extra AttributeValues to allow select ACs using as criterium "parent"
        extraAttrProperty = GNEAttributeCarrier::AttributeProperties(GNE_ATTR_PARENT,
                            GNEAttributeCarrier::AttrProperty::ATTRPROPERTY_STRING,
                            "Parent element");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // set current selected attribute
    myCurrentAttribute = SUMO_ATTR_NOTHING;
    for (const auto& i : tagPropertiesCopy) {
        if (i.getAttrStr() == myMatchAttrComboBox->getText().text()) {
            myCurrentAttribute = i.getAttr();
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
    } else if (tagValue.hasAttribute(myCurrentAttribute) && tagValue.getAttributeProperties(myCurrentAttribute).isNumerical()) {
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
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
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
    mySelectionScaling->setRange(1, 100000);
    mySelectionScaling->setValue(1);
    mySelectionScaling->setHelpText("Enlarge selected objects");
}


GNESelectorFrame::VisualScaling::~VisualScaling() {}


long
GNESelectorFrame::VisualScaling::onCmdScaleSelection(FXObject*, FXSelector, void*) {
    // set scale in viewnet
    mySelectorFrameParent->myViewNet->setSelectionScaling(mySelectionScaling->getValue());
    mySelectorFrameParent->myViewNet->update();
    return 1;
}

// ---------------------------------------------------------------------------
// ModificationMode::SelectionOperation - methods
// ---------------------------------------------------------------------------

GNESelectorFrame::SelectionOperation::SelectionOperation(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Operations for selections", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent) {
    // Create "Clear List" Button
    new FXButton(this, "Clear\t\t", nullptr, this, MID_CHOOSEN_CLEAR, GUIDesignButton);
    // Create "Invert" Button
    new FXButton(this, "Invert\t\t", nullptr, this, MID_CHOOSEN_INVERT, GUIDesignButton);
    // Create "Save" Button
    new FXButton(this, "Save\t\tSave ids of currently selected objects to a file.", nullptr, this, MID_CHOOSEN_SAVE, GUIDesignButton);
    // Create "Load" Button
    new FXButton(this, "Load\t\tLoad ids from a file according to the current modfication mode.", nullptr, this, MID_CHOOSEN_LOAD, GUIDesignButton);
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
                    GNEAttributeCarrier* AC = mySelectorFrameParent->myViewNet->getNet()->retrieveAttributeCarrier(object->getGlID(), false);
                    // check if AC exist and if is selectable
                    if (AC && AC->getTagProperty().isSelectable())
                        // now check if we're in the correct supermode to load this element
                        if (((mySelectorFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && !AC->getTagProperty().isDemandElement()) ||
                                ((mySelectorFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && AC->getTagProperty().isDemandElement())) {
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
    mySelectorFrameParent->myViewNet->update();
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
        for (auto i : mySelectorFrameParent->myViewNet->getNet()->getSelectedAttributeCarriers(false)) {
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
    // only continue if there is element for selecting
    if (mySelectorFrameParent->ACsToSelected()) {
        // obtan locks (only for improve code legibly)
        LockGLObjectTypes* locks = mySelectorFrameParent->getLockGLObjectTypes();
        // for invert selection, first clean current selection and next select elements of set "unselectedElements"
        mySelectorFrameParent->myViewNet->getUndoList()->p_begin("invert selection");
        // invert selection of elements depending of current supermode
        if (mySelectorFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
            // iterate over junctions
            for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().junctions) {
                // check if junction selection is locked
                if (!locks->IsObjectTypeLocked(GLO_JUNCTION)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                // due we iterate over all junctions, only it's neccesary iterate over incoming edges
                for (const auto& j : i.second->getGNEIncomingEdges()) {
                    // only select edges if "select edges" flag is enabled. In other case, select only lanes
                    if (mySelectorFrameParent->myViewNet->getNetworkViewOptions().selectEdges()) {
                        // check if edge selection is locked
                        if (!locks->IsObjectTypeLocked(GLO_EDGE)) {
                            if (j->isAttributeCarrierSelected()) {
                                j->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                            } else {
                                j->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                            }
                        }
                    } else {
                        // check if lane selection is locked
                        if (!locks->IsObjectTypeLocked(GLO_LANE)) {
                            for (auto k : j->getLanes()) {
                                if (k->isAttributeCarrierSelected()) {
                                    k->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                                } else {
                                    k->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                                }
                            }
                        }
                    }
                    // check if connection selection is locked
                    if (!locks->IsObjectTypeLocked(GLO_CONNECTION)) {
                        for (const auto& k : j->getGNEConnections()) {
                            if (k->isAttributeCarrierSelected()) {
                                k->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                            } else {
                                k->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                            }
                        }
                    }
                }
                // check if crossing selection is locked
                if (!locks->IsObjectTypeLocked(GLO_CROSSING)) {
                    for (const auto& j : i.second->getGNECrossings()) {
                        if (j->isAttributeCarrierSelected()) {
                            j->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                        } else {
                            j->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                        }
                    }
                }
            }
            // check if additionals selection is locked
            if (!locks->IsObjectTypeLocked(GLO_ADDITIONAL)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().additionals) {
                    // first check if additional is selectable
                    if (GNEAttributeCarrier::getTagProperties(i.first).isSelectable()) {
                        for (const auto& j : i.second) {
                            if (j.second->isAttributeCarrierSelected()) {
                                j.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                            } else {
                                j.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                            }
                        }
                    }
                }
            }
            // select polygons
            if (!locks->IsObjectTypeLocked(GLO_POLYGON)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getPolygons()) {
                    GNEShape* shape = dynamic_cast<GNEShape*>(i.second);
                    if (shape->isAttributeCarrierSelected()) {
                        shape->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        shape->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select POIs
            if (!locks->IsObjectTypeLocked(GLO_POI)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getPOIs()) {
                    GNEShape* shape = dynamic_cast<GNEShape*>(i.second);
                    if (shape->isAttributeCarrierSelected()) {
                        shape->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        shape->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
        } else {
            // select routes
            if (!locks->IsObjectTypeLocked(GLO_ROUTE)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select embedded routes
            if (!locks->IsObjectTypeLocked(GLO_EMBEDDEDROUTE)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_EMBEDDEDROUTE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select vehicles
            if (!locks->IsObjectTypeLocked(GLO_VEHICLE)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VEHICLE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select trips
            if (!locks->IsObjectTypeLocked(GLO_TRIP)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_TRIP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select flows
            if (!locks->IsObjectTypeLocked(GLO_FLOW)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_FLOW)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select route flows
            if (!locks->IsObjectTypeLocked(GLO_ROUTEFLOW)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTEFLOW)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select stops
            if (!locks->IsObjectTypeLocked(GLO_STOP)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_LANE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CONTAINERSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CHARGINGSTATION)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_PARKINGAREA)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select person
            if (!locks->IsObjectTypeLocked(GLO_PERSON)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSON)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select person flow
            if (!locks->IsObjectTypeLocked(GLO_PERSONFLOW)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONFLOW)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select person trip
            if (!locks->IsObjectTypeLocked(GLO_PERSONTRIP)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONTRIP_FROMTO)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONTRIP_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select ride
            if (!locks->IsObjectTypeLocked(GLO_RIDE)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_RIDE_FROMTO)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_RIDE_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select walks
            if (!locks->IsObjectTypeLocked(GLO_WALK)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_FROMTO)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_ROUTE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
            // select person stops
            if (!locks->IsObjectTypeLocked(GLO_PERSONSTOP)) {
                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONSTOP_LANE)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }

                for (const auto& i : mySelectorFrameParent->myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONSTOP_BUSSTOP)) {
                    if (i.second->isAttributeCarrierSelected()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "false", mySelectorFrameParent->myViewNet->getUndoList());
                    } else {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", mySelectorFrameParent->myViewNet->getUndoList());
                    }
                }
            }
        }
        // finish selection operation
        mySelectorFrameParent->myViewNet->getUndoList()->p_end();
        // update view
        mySelectorFrameParent->myViewNet->update();
    }
    return 1;
}


bool
GNESelectorFrame::ACsToSelected() const {
    if (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) {
        // check if exist junction and edges
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_JUNCTION) && (myViewNet->getNet()->getAttributeCarriers().junctions.size() > 0)) {
            return true;
        }
        if ((!myLockGLObjectTypes->IsObjectTypeLocked(GLO_JUNCTION) || !myLockGLObjectTypes->IsObjectTypeLocked(GLO_JUNCTION)) && (myViewNet->getNet()->getAttributeCarriers().edges.size() > 0)) {
            return true;
        }
        // check if additionals selection is locked
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ADDITIONAL)) {
            for (const auto& i : myViewNet->getNet()->getAttributeCarriers().additionals) {
                // first check if additional is selectable
                if (GNEAttributeCarrier::getTagProperties(i.first).isSelectable() && (myViewNet->getNet()->getAttributeCarriers().additionals.at(i.first).size() > 0)) {
                    return true;
                }
            }
        }
        // select polygons
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POLYGON) && (myViewNet->getNet()->getPolygons().size() > 0)) {
            return true;
        }
        // select POIs
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_POI) && (myViewNet->getNet()->getPOIs().size() > 0)) {
            return true;
        }
    } else {
        // select routes
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTE) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTE).size() > 0)) {
            return true;
        }
        // select embedded routes
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_EMBEDDEDROUTE) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_EMBEDDEDROUTE).size() > 0)) {
            return true;
        }
        // select vehicles
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_VEHICLE) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VEHICLE).size() > 0)) {
            return true;
        }
        // select trips
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_TRIP) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_TRIP).size() > 0)) {
            return true;
        }
        // select flows
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_FLOW) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_FLOW).size() > 0)) {
            return true;
        }
        // select route flows
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_ROUTEFLOW) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_ROUTEFLOW).size() > 0)) {
            return true;
        }
        // select stops
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_STOP)) {
            if ((myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_LANE).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_BUSSTOP).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CONTAINERSTOP).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_STOP_CHARGINGSTATION).size() > 0)) {
                return true;
            }
        }
        // select person
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSON) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSON).size() > 0)) {
            return true;
        }
        // select person flows
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONFLOW) && (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONFLOW).size() > 0)) {
            return true;
        }
        // select persontrips
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONTRIP)) {
            if ((myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONTRIP_FROMTO).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONTRIP_BUSSTOP).size() > 0)) {
                return true;
            }
        }
        // select ride
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_RIDE)) {
            if ((myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_RIDE_FROMTO).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_RIDE_BUSSTOP).size() > 0)) {
                return true;
            }
        }
        // select walks
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_WALK)) {
            if ((myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_FROMTO).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_BUSSTOP).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_WALK_ROUTE).size() > 0)) {
                return true;
            }
        }
        // select person stops
        if (!myLockGLObjectTypes->IsObjectTypeLocked(GLO_PERSONSTOP)) {
            if ((myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONSTOP_LANE).size() > 0) ||
                    (myViewNet->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_PERSONSTOP_BUSSTOP).size() > 0)) {
                return true;
            }
        }
    }
    // nothing to select
    return false;
}

/****************************************************************************/
