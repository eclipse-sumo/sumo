/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEAttributesEditorType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Row used for edit attributes
/****************************************************************************/

#include <netedit/dialogs/elements/GNECalibratorDialog.h>
#include <netedit/dialogs/elements/GNERerouterDialog.h>
#include <netedit/dialogs/elements/GNEVariableSpeedSignDialog.h>
#include <netedit/dialogs/GNEParametersDialog.h>
#include <netedit/elements/additional/GNECalibrator.h>
#include <netedit/elements/additional/GNERerouter.h>
#include <netedit/elements/additional/GNEVariableSpeedSign.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEInternalTest.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEAttributesEditor.h"
#include "GNEAttributesEditorType.h"
#include "GNEAttributesEditorRow.h"

// ===========================================================================
// static members
// ===========================================================================

GNEAttributesEditorType::AttributesEditorRows GNEAttributesEditorType::myFirstSingletonAttributesEditorRows = {};
GNEAttributesEditorType::AttributesEditorRows GNEAttributesEditorType::mySecondSingletonAttributesEditorRows = {};

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAttributesEditorType) GNEAttributeTableMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_FRONT,         GNEAttributesEditorType::onCmdMarkAsFront),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_DIALOG,        GNEAttributesEditorType::onCmdOpenElementDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_EXTENDED,      GNEAttributesEditorType::onCmdOpenExtendedAttributesDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_PARAMETERS,    GNEAttributesEditorType::onCmdOpenEditParametersDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_HELP,          GNEAttributesEditorType::onCmdAttributesEditorHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_RESET,         GNEAttributesEditorType::onCmdAttributesEditorReset)
};

// Object implementation
FXIMPLEMENT(GNEAttributesEditorType,  MFXGroupBoxModule,  GNEAttributeTableMap,   ARRAYNUMBER(GNEAttributeTableMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditorType::GNEAttributesEditorType(GNEFrame* frameParent, GNEAttributesEditor* attributesEditorParent,
        const std::string attributesEditorName, EditorType editorType, AttributeType attributeType) :
    MFXGroupBoxModule(frameParent, attributesEditorName.c_str()),
    myFrameParent(frameParent),
    myAttributesEditorParent(attributesEditorParent),
    myEditorType(editorType),
    myAttributeType(attributeType) {
    // create netedit especific buttons (before row)
    if (attributeType == AttributeType::NETEDIT) {
        // create netedit editor buttons
        myFrontButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Front element"), "", "", GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), this, MID_GNE_ATTRIBUTESEDITOR_FRONT, GUIDesignButton);
        myFrontButton->hide();
        myOpenDialogButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Open element dialog"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_DIALOG, GUIDesignButton);
        myOpenDialogButton->hide();
        // Create buttons
        myFrameNeteditButtons = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
        GUIDesigns::buildFXButton(myFrameNeteditButtons, TL("Help"), TL("Open help attributes dialog"), TL("Open help attributes dialog"), nullptr,
                                  this, MID_GNE_ATTRIBUTESEDITOR_HELP, GUIDesignButtonRectangular);
        GUIDesigns::buildFXButton(myFrameNeteditButtons, "", TL("Reset attributes"), TL("Reset attributes"), GUIIconSubSys::getIcon(GUIIcon::RESET),
                                  this, MID_GNE_ATTRIBUTESEDITOR_RESET, GUIDesignButtonIcon);
    }
    // build rows
    buildRows(this);
    // create specific buttons for extended and parameteres
    if (myAttributeType == AttributeType::EXTENDED) {
        // create extended attributes (always shown)
        myOpenExtendedAttributesButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit extended attributes"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_EXTENDED, GUIDesignButton);
    } else if (myAttributeType == AttributeType::PARAMETERS) {
        // create generic attributes editor button (always shown)
        myOpenGenericParametersEditorButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit parameters"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_PARAMETERS, GUIDesignButton);
    }
}


GNEAttributesEditorType::~GNEAttributesEditorType() {
    // drow singletons (important if we're reloading)
    myFirstSingletonAttributesEditorRows.clear();
    mySecondSingletonAttributesEditorRows.clear();
}


GNEFrame*
GNEAttributesEditorType::getFrameParent() const {
    return myFrameParent;
}


bool
GNEAttributesEditorType::isEditorTypeCreator() const {
    return myEditorType == EditorType::CREATOR;
}


bool
GNEAttributesEditorType::isEditorTypeEditor() const {
    return myEditorType == EditorType::EDITOR;
}


const std::vector<GNEAttributeCarrier*>&
GNEAttributesEditorType::getEditedAttributeCarriers() const {
    return myEditedACs;
}


void
GNEAttributesEditorType::showAttributesEditor(GNEAttributeCarrier* AC, const bool primaryAttributeEditor) {
    // clean previous rows and ACs
    myEditedACs.clear();
    myAttributesEditorRows.clear();
    // set new ACs and Rows
    myEditedACs.push_back(AC);
    if (primaryAttributeEditor) {
        myAttributesEditorRows = myFirstSingletonAttributesEditorRows[myAttributeType];
    } else {
        myAttributesEditorRows = mySecondSingletonAttributesEditorRows[myAttributeType];
    }
    refreshAttributesEditor();
}


void
GNEAttributesEditorType::showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs, const bool primaryAttributeEditor) {
    // clean previous rows and ACs
    myEditedACs.clear();
    myAttributesEditorRows.clear();
    // set new ACs and rows
    for (const auto& AC : ACs) {
        myEditedACs.push_back(AC);
    }
    if (primaryAttributeEditor) {
        myAttributesEditorRows = myFirstSingletonAttributesEditorRows[myAttributeType];
    } else {
        myAttributesEditorRows = mySecondSingletonAttributesEditorRows[myAttributeType];
    }
    refreshAttributesEditor();
}


void
GNEAttributesEditorType::hideAttributesEditor() {
    myEditedACs.clear();
    myAttributesEditorRows.clear();
    // hide all rows before hidding table
    for (const auto& row : myAttributesEditorRows) {
        row->hideAttributeRow();
    }
    hide();
}


void
GNEAttributesEditorType::refreshAttributesEditor() {
    bool showButtons = false;
    int rowIndex = 0;
    if (myEditedACs.size() > 0) {
        const auto tagProperty = myEditedACs.front()->getTagProperty();
        // check if show netedit attributes (only in edit mode)
        if (myAttributeType == AttributeType::NETEDIT && isEditorTypeEditor()) {
            // front button
            if (tagProperty->isDrawable()) {
                myFrontButton->show();
                // disable if we're reparenting
                if (isReparenting()) {
                    myOpenDialogButton->disable();
                } else {
                    myOpenDialogButton->enable();
                }
                showButtons = true;
            } else {
                myFrontButton->hide();
            }
            // specific for single edited attributes
            if ((myEditedACs.size() == 1) && tagProperty->hasDialog()) {
                // update and show edit dialog
                myOpenDialogButton->setText(TLF("Open % dialog", tagProperty->getTagStr()).c_str());
                myOpenDialogButton->setIcon(GUIIconSubSys::getIcon(tagProperty->getGUIIcon()));
                myOpenDialogButton->show();
                // disable if we're reparenting
                if (isReparenting()) {
                    myOpenDialogButton->disable();
                } else {
                    myOpenDialogButton->enable();
                }
                showButtons = true;
            } else {
                myOpenDialogButton->hide();
            }
        }
        // continue depending of attribute type
        if (myAttributeType == AttributeType::EXTENDED) {
            // only show extended attributes button (already created)
            if (tagProperty->hasExtendedAttributes()) {
                showButtons = true;
            }
        } else if (myAttributeType == AttributeType::PARAMETERS) {
            if (tagProperty->hasParameters()) {
                // only show parameters row
                myAttributesEditorRows[rowIndex]->showAttributeRow(this, tagProperty->getAttributeProperties(GNE_ATTR_PARAMETERS), isReparenting());
                // set parameters button at the end
                myOpenGenericParametersEditorButton->reparent(this);
                // only show open parameters editor
                showButtons = true;
            }
        } else {
            // Iterate over tag property of first AC and show row for every attribute
            for (const auto& attrProperty : tagProperty->getAttributeProperties()) {
                // filter editor type
                bool validEditorType = false;
                if (isEditorTypeCreator() && attrProperty->isCreateMode()) {
                    validEditorType = true;
                }
                if (isEditorTypeEditor() && attrProperty->isEditMode()) {
                    validEditorType = true;
                }
                // filter types
                bool validAttributeType = true;
                if ((myAttributeType == AttributeType::BASIC) && !attrProperty->isBasicEditor()) {
                    validAttributeType = false;
                }
                if ((myAttributeType == AttributeType::FLOW) && !attrProperty->isFlowEditor()) {
                    validAttributeType = false;
                }
                if ((myAttributeType == AttributeType::GEO) && !attrProperty->isGeoEditor()) {
                    validAttributeType = false;
                }
                if ((myAttributeType == AttributeType::NETEDIT) && !attrProperty->isNeteditEditor()) {
                    validAttributeType = false;
                }
                if (attrProperty->isExtendedEditor()) {
                    validAttributeType = false;
                }
                if (attrProperty->getAttr() == GNE_ATTR_PARAMETERS) {
                    validAttributeType = false;
                }
                if (validEditorType && validAttributeType) {
                    if (rowIndex < (int)myAttributesEditorRows.size()) {
                        // only update if row was show successfully
                        if (myAttributesEditorRows[rowIndex]->showAttributeRow(this, attrProperty, isReparenting())) {
                            rowIndex++;
                        }
                    } else {
                        throw ProcessError("Invalid maximum number of rows");
                    }
                }
            }
            // hide rest of rows before showing table
            for (int i = rowIndex; i < (int)myAttributesEditorRows.size(); i++) {
                myAttributesEditorRows[i]->hideAttributeRow();
            }
        }
    }
    // check if show row
    if ((rowIndex == 0) && !showButtons) {
        hideAttributesEditor();
    } else {
        if (myFrameNeteditButtons) {
            myFrameNeteditButtons->reparent(this);
        }
        show();
    }
}


void
GNEAttributesEditorType::disableAttributesEditor() {
    for (const auto& row : myAttributesEditorRows) {
        row->disable();
    }
}


bool
GNEAttributesEditorType::checkAttributes(const bool showWarning) {
    // iterate over all rows and check if values are valid
    for (const auto& row : myAttributesEditorRows) {
        if (!row->isValueValid()) {
            if (showWarning) {
                const std::string errorMessage = TLF("Invalid value '%' in attribute %", row->getCurrentValue(), row->getAttrProperty()->getAttrStr());
                // show warning
                WRITE_WARNING(errorMessage);
                // set message in status bar
                myFrameParent->getViewNet()->setStatusBarText(errorMessage);
                return false;
            }
        }
    }
    return true;
}


SumoXMLAttr
GNEAttributesEditorType::fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject, const bool insertDefaultValues) const {
    // iterate over every attribute row and stop if there was an error
    for (const auto& row : myAttributesEditorRows) {
        if (row->isAttributeRowShown()) {
            const auto fillResult = row->fillSumoBaseObject(baseObject, insertDefaultValues);
            if (fillResult != SUMO_ATTR_NOTHING) {
                return fillResult;
            }
        }
    }
    // handle special case for elemnt with start-end position over lanes
    fillStartEndAttributes(baseObject);
    // all ok, then return nothing
    return SUMO_ATTR_NOTHING;
}


bool
GNEAttributesEditorType::isReparenting() const {
    return myReparentTag != SUMO_TAG_NOTHING;
}


bool
GNEAttributesEditorType::checkNewParent(const GNEAttributeCarrier* AC) const {
    return AC->getTagProperty()->getTag() == myReparentTag;
}


void
GNEAttributesEditorType::setNewParent(const GNEAttributeCarrier* AC) {
    myEditedACs.front()->setAttribute(GNE_ATTR_PARENT, AC->getID(), myFrameParent->getViewNet()->getUndoList());
    abortReparenting();
}


void
GNEAttributesEditorType::abortReparenting() {
    myReparentTag = SUMO_TAG_NOTHING;
    refreshAttributesEditor();
}


long
GNEAttributesEditorType::onCmdMarkAsFront(FXObject*, FXSelector, void*) {
    // check if all element are selectd
    bool allSelected = true;
    for (auto& AC : myEditedACs) {
        if (!AC->isMarkedForDrawingFront()) {
            allSelected = false;
            break;
        }
    }
    // only unfronto if all element are front
    for (auto& AC : myEditedACs) {
        if (allSelected) {
            AC->unmarkForDrawingFront();
        } else {
            AC->markForDrawingFront();
        }
    }
    return 1;
}


long
GNEAttributesEditorType::onCmdOpenElementDialog(FXObject*, FXSelector, void*) {
    // check number of inspected elements
    if (myEditedACs.size() == 1) {
        const auto editedTag = myEditedACs.front()->getTagProperty()->getTag();
        // check AC
        if (editedTag == SUMO_TAG_REROUTER) {
            // Open rerouter dialog
            GNERerouterDialog(dynamic_cast<GNERerouter*>(myEditedACs.front()));
        } else if ((editedTag == SUMO_TAG_CALIBRATOR) || (editedTag == GNE_TAG_CALIBRATOR_LANE)) {
            // Open calibrator dialog
            GNECalibratorDialog(dynamic_cast<GNECalibrator*>(myEditedACs.front()));
        } else if (editedTag == SUMO_TAG_VSS) {
            // Open VSS dialog
            GNEVariableSpeedSignDialog(dynamic_cast<GNEVariableSpeedSign*>(myEditedACs.front()));
        }
    }
    return 1;
}


long
GNEAttributesEditorType::onCmdOpenExtendedAttributesDialog(FXObject*, FXSelector, void*) {
    // obtain edited AC (temporal), until unification of
    auto demandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(myEditedACs.front()->getTagProperty()->getTag(), myEditedACs.front()->getID(), false);
    // open vehicle type dialog
    if (demandElement) {
        // open dialog
        const auto vTypeDialog = GNEVehicleTypeDialog(demandElement);
        if (vTypeDialog.getResult() == GNEDialog::Result::ACCEPT) {
            refreshAttributesEditor();
        }
    }
    return 1;
}


long
GNEAttributesEditorType::onCmdOpenEditParametersDialog(FXObject*, FXSelector, void*) {
    // create parameters dialog
    const auto singleParametersDialog = GNEParametersDialog(myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows(), myEditedACs.front()->getParameters()->getParametersMap());
    // continue depending of result
    if (singleParametersDialog.getResult() == GNEDialog::Result::ACCEPT) {
        if (isEditorTypeCreator()) {
            // Set new value of attribute in all edited ACs without undo-redo
            for (const auto& editedAC : myEditedACs) {
                editedAC->setACParameters(singleParametersDialog.getEditedParameters());
            }
        } else if (isEditorTypeEditor()) {
            const auto undoList = myFrameParent->getViewNet()->getUndoList();
            const auto tagProperty = myEditedACs.front()->getTagProperty();
            // first check if we're editing a single attribute or an ID
            if (myEditedACs.size() > 1) {
                undoList->begin(tagProperty->getGUIIcon(), TLF("change multiple % attributes", tagProperty->getTagStr()));
            }
            // Set new value of attribute in all edited ACs
            for (const auto& editedAC : myEditedACs) {
                editedAC->setACParameters(singleParametersDialog.getEditedParameters(), undoList);
            }
            // finish change multiple attributes or ID Attributes
            if (myEditedACs.size() > 1) {
                undoList->end();
            }
        }
        refreshAttributesEditor();
    }
    return 1;
}


long
GNEAttributesEditorType::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    if (myEditedACs.size() > 0) {
        myFrameParent->openHelpAttributesDialog(myEditedACs.front());
    }
    return 1;
}


long
GNEAttributesEditorType::onCmdAttributesEditorReset(FXObject*, FXSelector, void*) {
    if (myEditedACs.size() > 0) {
        // continue depending if we're creating or inspecting (undo-redo)
        if (myEditorType == EditorType::CREATOR) {
            for (auto& AC : myEditedACs) {
                AC->resetDefaultValues(false);
            }
        } else {
            myFrameParent->getViewNet()->getUndoList()->begin(myEditedACs.front()->getTagProperty()->getGUIIcon(), TLF("reset %", myEditedACs.front()->getTagProperty()->getTagStr()));
            for (auto& AC : myEditedACs) {
                AC->resetDefaultValues(true);
            }
            myFrameParent->getViewNet()->getUndoList()->end();
        }
        // refresh all attributes editor types
        myAttributesEditorParent->refreshAttributesEditor();
    }
    return 1;
}


void
GNEAttributesEditorType::setAttribute(SumoXMLAttr attr, const std::string& value) {
    const auto undoList = myFrameParent->getViewNet()->getUndoList();
    const auto tagProperty = myEditedACs.front()->getTagProperty();
    // continue depending if we're creating or inspecting
    if (isEditorTypeCreator()) {
        // Set new value of attribute in all edited ACs without undo-redo
        for (const auto& editedAC : myEditedACs) {
            editedAC->setAttribute(attr, value);
        }
    } else if (isEditorTypeEditor()) {
        // first check if we're editing a single attribute or an ID
        if (myEditedACs.size() > 1) {
            undoList->begin(tagProperty->getGUIIcon(), TLF("change multiple % attributes", tagProperty->getTagStr()));
        } else if (attr == SUMO_ATTR_ID) {
            // IDs attribute has to be encapsulated because implies multiple changes in different additionals (due references)
            undoList->begin(tagProperty->getGUIIcon(), TLF("change % attribute", tagProperty->getTagStr()));
        }
        // Set new value of attribute in all edited ACs
        for (const auto& editedAC : myEditedACs) {
            editedAC->setAttribute(attr, value, undoList);
        }
        // finish change multiple attributes or ID Attributes
        if ((myEditedACs.size() > 1) || (attr == SUMO_ATTR_ID)) {
            undoList->end();
        }
    }
    refreshAttributesEditor();
    // update frame parent (needed to update other attribute tables)
    myFrameParent->attributeUpdated(attr);
}


void
GNEAttributesEditorType::toggleEnableAttribute(SumoXMLAttr attr, const bool value) {
    const auto undoList = myFrameParent->getViewNet()->getUndoList();
    const auto tagProperty = myEditedACs.front()->getTagProperty();
    // continue depending if we're creating or inspecting
    if (isEditorTypeCreator()) {
        // Set new value of attribute in all edited ACs without undo-redo
        for (const auto& editedAC : myEditedACs) {
            editedAC->toggleAttribute(attr, value);
        }
    } else if (isEditorTypeEditor()) {
        // first check if we're editing a single attribute
        if (myEditedACs.size() > 1) {
            undoList->begin(tagProperty->getGUIIcon(), TLF("change multiple % attributes", tagProperty->getTagStr()));
        }
        // Set new value of attribute in all edited ACs
        for (const auto& editedAC : myEditedACs) {
            if (value) {
                editedAC->enableAttribute(attr, undoList);
            } else {
                editedAC->disableAttribute(attr, undoList);
            }
        }
        // finish change multiple attributes or ID Attributes
        if ((myEditedACs.size() > 1) || (attr == SUMO_ATTR_ID)) {
            undoList->end();
        }
    }
    refreshAttributesEditor();
    // update frame parent (needed to update other attribute tables)
    myFrameParent->attributeUpdated(attr);
}


void
GNEAttributesEditorType::enableReparent() {
    if (myEditedACs.front()->getTagProperty()->getXMLParentTags().size() > 0) {
        myReparentTag = myEditedACs.front()->getTagProperty()->getXMLParentTags().front();
        refreshAttributesEditor();
        myFrameParent->getViewNet()->update();
    }
}


void
GNEAttributesEditorType::inspectParent() {
    const auto type = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myEditedACs.front()->getAttribute(SUMO_ATTR_TYPE), false);
    if (type) {
        myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectElement(type, myEditedACs.front());
    }
}


void
GNEAttributesEditorType::moveLaneUp() {
    const auto lane = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(myEditedACs.front()->getAttribute(SUMO_ATTR_LANE), false);
    if (lane) {
        // set next lane
        setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getChildLanes().at(lane->getIndex() + 1)->getID());
    }
}


void
GNEAttributesEditorType::moveLaneDown() {
    const auto lane = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(myEditedACs.front()->getAttribute(SUMO_ATTR_LANE), false);
    if (lane) {
        // set previous lane
        setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getChildLanes().at(lane->getIndex() - 1)->getID());
    }
}


void
GNEAttributesEditorType::fillStartEndAttributes(CommonXMLStructure::SumoBaseObject* baseObject) const {
    if (baseObject->hasDoubleAttribute(SUMO_ATTR_POSITION) && baseObject->hasDoubleAttribute(GNE_ATTR_SIZE) &&
            baseObject->hasDoubleAttribute(GNE_ATTR_LANELENGTH) && baseObject->hasBoolAttribute(GNE_ATTR_FORCESIZE) &&
            baseObject->hasStringAttribute(GNE_ATTR_REFERENCE)) {
        // extract parameters
        const double centerPosition = baseObject->getDoubleAttribute(SUMO_ATTR_POSITION);
        const double size = baseObject->getDoubleAttribute(GNE_ATTR_SIZE);
        const double laneLength = baseObject->getDoubleAttribute(GNE_ATTR_LANELENGTH);
        const bool forceSize = baseObject->getBoolAttribute(GNE_ATTR_FORCESIZE);
        const auto reference = baseObject->getStringAttribute(GNE_ATTR_REFERENCE);
        // we fill startPos and endPos using the existent parameters
        double startPos = centerPosition - (size * 0.5);
        double endPos = centerPosition + (size * 0.5);
        if (reference == SUMOXMLDefinitions::ReferencePositions.getString(ReferencePosition::LEFT)) {
            startPos = centerPosition - size;
            endPos = centerPosition;
        } else if (reference == SUMOXMLDefinitions::ReferencePositions.getString(ReferencePosition::RIGHT)) {
            startPos = centerPosition;
            endPos = centerPosition + size;
        }
        // adjust values
        if (startPos < 0) {
            startPos = 0;
            if (forceSize) {
                endPos = size;
            }
        }
        if (endPos > laneLength) {
            endPos = laneLength;
            if (forceSize) {
                startPos = laneLength - size;
            }
        }
        if (startPos < 0) {
            startPos = 0;
        }
        if (endPos > laneLength) {
            endPos = laneLength;
        }
        // add it in baseObject
        baseObject->addDoubleAttribute(SUMO_ATTR_STARTPOS, startPos);
        baseObject->addDoubleAttribute(SUMO_ATTR_ENDPOS, endPos);
    }
}


void
GNEAttributesEditorType::buildRows(GNEAttributesEditorType* editorParent) {
    // only build one time
    if (myFirstSingletonAttributesEditorRows.empty()) {
        const auto tagPropertiesDatabase = editorParent->getFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase();
        // declare vector of types with rows
        const std::vector<AttributeType> types = {AttributeType::BASIC, AttributeType::FLOW, AttributeType::GEO, AttributeType::NETEDIT, AttributeType::PARAMETERS};
        // iterate over all types and create their correspond rows
        for (const auto type : types) {
            int maxNumberOfRows = 0;
            // get max number of rows
            if (type == AttributeType::BASIC) {
                maxNumberOfRows = tagPropertiesDatabase->getMaxNumberOfEditableAttributeRows();
            } else if (type == AttributeType::FLOW) {
                maxNumberOfRows = tagPropertiesDatabase->getMaxNumberOfFlowAttributeRows();
            } else if (type == AttributeType::GEO) {
                maxNumberOfRows = tagPropertiesDatabase->getMaxNumberOfGeoAttributeRows();
            } else if (type == AttributeType::NETEDIT) {
                maxNumberOfRows = tagPropertiesDatabase->getMaxNumberOfNeteditAttributesRows();
            } else if (type == AttributeType::PARAMETERS) {
                maxNumberOfRows = 1;
            } else {
                throw ProcessError("Invalid editor option");
            }
            // resize myAttributesEditorRows and fill it with attribute rows
            myFirstSingletonAttributesEditorRows[type].resize(maxNumberOfRows);
            mySecondSingletonAttributesEditorRows[type].resize(maxNumberOfRows);
            for (int i = 0; i < (int)myFirstSingletonAttributesEditorRows[type].size(); i++) {
                myFirstSingletonAttributesEditorRows[type][i] = new GNEAttributesEditorRow(editorParent);
                mySecondSingletonAttributesEditorRows[type][i] = new GNEAttributesEditorRow(editorParent);
            }
        }

    }

}

/****************************************************************************/
