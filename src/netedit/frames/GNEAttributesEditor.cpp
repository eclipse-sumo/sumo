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
/// @file    GNEAttributesEditor.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Row used for edit attributes
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/GNECalibratorDialog.h>
#include <netedit/dialogs/GNERerouterDialog.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <netedit/dialogs/GNEVariableSpeedSignDialog.h>
#include <netedit/elements/additional/GNECalibrator.h>
#include <netedit/elements/additional/GNERerouter.h>
#include <netedit/elements/additional/GNEVariableSpeedSign.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEAttributesEditor.h"
#include "GNEAttributesEditorRow.h"

// ===========================================================================
// static members
// ===========================================================================

GNEAttributesEditor::AttributesEditorRows GNEAttributesEditor::mySingletonAttributesEditorRows = {};

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAttributesEditor) GNEAttributeTableMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_FRONT,         GNEAttributesEditor::onCmdMarkAsFront),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_DIALOG,        GNEAttributesEditor::onCmdOpenElementDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_EXTENDED,      GNEAttributesEditor::onCmdOpenExtendedAttributesDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_PARAMETERS,    GNEAttributesEditor::onCmdOpenEditParametersDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_HELP,          GNEAttributesEditor::onCmdAttributesEditorHelp)
};

// Object implementation
FXIMPLEMENT(GNEAttributesEditor,  MFXGroupBoxModule,  GNEAttributeTableMap,   ARRAYNUMBER(GNEAttributeTableMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditor::GNEAttributesEditor(GNEFrame* frameParent, const std::string attributesEditorName, EditorType editorType, AttributeType attributeType) :
    MFXGroupBoxModule(frameParent, attributesEditorName.c_str()),
    myFrameParent(frameParent),
    myEditorType(editorType),
    myAttributeType(attributeType) {
    // create netedit especific buttons (before row)
    if (attributeType == AttributeType::NETEDIT) {
        // create netedit editor buttons
        myFrontButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Front element"), "", "", GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), this, MID_GNE_ATTRIBUTESEDITOR_FRONT, GUIDesignButton);
        myFrontButton->hide();
        myOpenDialogButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Open element dialog"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_DIALOG, GUIDesignButton);
        myOpenDialogButton->hide();
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
    // Create help button
    myHelpButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_HELP, GUIDesignButtonRectangular);
}


GNEFrame*
GNEAttributesEditor::getFrameParent() const {
    return myFrameParent;
}


const std::vector<GNEAttributeCarrier*>&
GNEAttributesEditor::getEditedAttributeCarriers() const {
    return myEditedACs;
}


void
GNEAttributesEditor::showAttributesEditor(GNEAttributeCarrier* AC) {
    // clean previous rows and ACs
    myEditedACs.clear();
    myAttributesEditorRows.clear();
    // set new ACs and Rows
    myEditedACs.push_back(AC);
    myAttributesEditorRows = mySingletonAttributesEditorRows[myAttributeType];
    refreshAttributesEditor();
}


void
GNEAttributesEditor::showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs) {
    // clean previous rows and ACs
    myEditedACs.clear();
    myAttributesEditorRows.clear();
    // set new ACs and rows
    for (const auto& AC : ACs) {
        myEditedACs.push_back(AC);
    }
    myAttributesEditorRows = mySingletonAttributesEditorRows[myAttributeType];
    refreshAttributesEditor();
}


void
GNEAttributesEditor::hideAttributesEditor() {
    myEditedACs.clear();
    myAttributesEditorRows.clear();
    // hide all rows before hidding table
    for (const auto& row : myAttributesEditorRows) {
        row->hideAttributeRow();
    }
    hide();
}


void
GNEAttributesEditor::refreshAttributesEditor() {
    if (myEditedACs.size() > 0) {
        const auto tagProperty = myEditedACs.front()->getTagProperty();
        bool showButtons = false;
        int itRows = 0;
        // check if show netedit attributes (only in edit mode)
        if (myAttributeType == AttributeType::NETEDIT && (myEditorType == EditorType::EDITOR)) {
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
            if (myEditedACs.size() == 1) {
                // edit dialog
                if (tagProperty->hasDialog()) {
                    // set text and icon
                    myOpenDialogButton->setText(TLF("Open % dialog", tagProperty->getTagStr()).c_str());
                    myOpenDialogButton->setIcon(GUIIconSubSys::getIcon(tagProperty->getGUIIcon()));
                    // disable if we're reparenting
                    if (isReparenting()) {
                        myOpenDialogButton->disable();
                    } else {
                        myOpenDialogButton->enable();
                    }
                    myOpenDialogButton->show();
                    showButtons = true;
                } else {
                    myOpenDialogButton->hide();
                }

            }
        }
        // continue depending of attribute type
        if (myAttributeType == AttributeType::EXTENDED) {
            // only show extended attributes button (already created)
            showButtons = true;
        } else if (myAttributeType == AttributeType::PARAMETERS) {
            // only show parameters row
            myAttributesEditorRows[itRows]->showAttributeRow(this, tagProperty->getAttributeProperties(GNE_ATTR_PARAMETERS), isReparenting());
            // set parameters button at the end
            myOpenGenericParametersEditorButton->reparent(this);
            // only show open parameters editor
            showButtons = true;
        } else {
            // Iterate over tag property of first AC and show row for every attribute
            for (const auto& attrProperty : tagProperty->getAttributeProperties()) {
                bool showAttributeRow = true;
                // check show conditions
                if (attrProperty->isExtended()) {
                    showAttributeRow = false;
                }
                // filter editor type
                if ((myEditorType != EditorType::CREATOR) && (myEditorType != EditorType::EDITOR)) {
                    showAttributeRow = false;
                }
                if ((myEditorType == EditorType::CREATOR) && !attrProperty->isCreateMode()) {
                    showAttributeRow = false;
                }
                if ((myEditorType == EditorType::EDITOR) && !attrProperty->isEditMode()) {
                    showAttributeRow = false;
                }
                // filter types
                if ((myAttributeType == AttributeType::CHILD) && !attrProperty->isChild()) {
                    showAttributeRow = false;
                }
                if ((myAttributeType == AttributeType::FLOW) && !attrProperty->isFlow()) {
                    showAttributeRow = false;
                }
                if ((myAttributeType == AttributeType::GEO) && !attrProperty->isGEO()) {
                    showAttributeRow = false;
                }
                if ((myAttributeType == AttributeType::NETEDIT) && !attrProperty->isNetedit()) {
                    showAttributeRow = false;
                }
                if (attrProperty->getAttr() == GNE_ATTR_PARAMETERS) {
                    showAttributeRow = false;
                }
                if (showAttributeRow) {
                    if (itRows < (int)myAttributesEditorRows.size()) {
                        // only update if row was show successfully
                        if (myAttributesEditorRows[itRows]->showAttributeRow(this, attrProperty, isReparenting())) {
                            itRows++;
                        }
                    } else {
                        throw ProcessError("Invalid maximum number of rows");
                    }
                }
            }
            // hide rest of rows before showing table
            for (int i = itRows; i < (int)myAttributesEditorRows.size(); i++) {
                myAttributesEditorRows[i]->hideAttributeRow();
            }
        }
        // only show if at least one row or button was shown
        if ((itRows == 0) && !showButtons) {
            hideAttributesEditor();
        } else {
            if (itRows > 0) {
                // reparent before show to put it at the end of the list
                myHelpButton->reparent(this);
                myHelpButton->show();
            } else {
                myHelpButton->hide();
            }
            show();
        }
    } else {
        hideAttributesEditor();
    }
}


void
GNEAttributesEditor::disableAttributesEditor() {
    for (const auto& row : myAttributesEditorRows) {
        row->disable();
    }
}


bool
GNEAttributesEditor::checkAttributes(const bool showWarning) {
    for (const auto& row : myAttributesEditorRows) {
        if (!row->isValueValid())
            if (showWarning) {
                const std::string errorMessage = TLF("Invalid value % of attribute %", row->getCurrentValue(), row->getAttrProperty()->getAttrStr());
                // show warning
                WRITE_WARNING(errorMessage);
                // set message in status bar
                myFrameParent->getViewNet()->setStatusBarText(errorMessage);
                return false;
            }
    }
    return true;
}


SumoXMLAttr
GNEAttributesEditor::fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject) const {
    // iterate over every attribute row and stop if there was an error
    for (const auto& row : myAttributesEditorRows) {
        if (row->isAttributeRowShown()) {
            const auto fillResult = row->fillSumoBaseObject(baseObject);
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
GNEAttributesEditor::isReparenting() const {
    return myReparentTag != SUMO_TAG_NOTHING;
}


bool
GNEAttributesEditor::checkNewParent(const GNEAttributeCarrier* AC) const {
    return AC->getTagProperty()->getTag() == myReparentTag;
}


void
GNEAttributesEditor::setNewParent(const GNEAttributeCarrier* AC) {
    myEditedACs.front()->setAttribute(GNE_ATTR_PARENT, AC->getID(), myFrameParent->getViewNet()->getUndoList());
    abortReparenting();
}


void
GNEAttributesEditor::abortReparenting() {
    myReparentTag = SUMO_TAG_NOTHING;
    refreshAttributesEditor();
}


long
GNEAttributesEditor::onCmdMarkAsFront(FXObject*, FXSelector, void*) {
    // front all edited ACs
    for (auto& AC : myEditedACs) {
        AC->markForDrawingFront();
    }
    return 1;
}


long
GNEAttributesEditor::onCmdOpenElementDialog(FXObject*, FXSelector, void*) {
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
GNEAttributesEditor::onCmdOpenExtendedAttributesDialog(FXObject*, FXSelector, void*) {
    // obtain edited AC (temporal), until unification of
    const auto demandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(myEditedACs.front()->getTagProperty()->getTag(), myEditedACs.front()->getID(), false);
    // open vehicle type dialog
    if (demandElement) {
        GNEVehicleTypeDialog(demandElement, true);  // NOSONAR, constructor returns after dialog has been closed
        refreshAttributesEditor();
    }
    return 1;
}


long
GNEAttributesEditor::onCmdOpenEditParametersDialog(FXObject*, FXSelector, void*) {
    if (GNESingleParametersDialog(this).execute()) {
        refreshAttributesEditor();
    }
    return 1;
}


long
GNEAttributesEditor::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    if (myEditedACs.size() > 0) {
        myFrameParent->openHelpAttributesDialog(myEditedACs.front());
    }
    return 1;
}


void
GNEAttributesEditor::setAttribute(SumoXMLAttr attr, const std::string& value) {
    const auto undoList = myFrameParent->getViewNet()->getUndoList();
    const auto tagProperty = myEditedACs.front()->getTagProperty();
    // continue depending if we're creating or inspecting
    if (myEditorType == EditorType::CREATOR) {
        // Set new value of attribute in all edited ACs without undo-redo
        for (const auto& editedAC : myEditedACs) {
            editedAC->setAttribute(attr, value);
        }
    } else if (myEditorType == EditorType::EDITOR) {
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
GNEAttributesEditor::toggleEnableAttribute(SumoXMLAttr attr, const bool value) {
    const auto undoList = myFrameParent->getViewNet()->getUndoList();
    const auto tagProperty = myEditedACs.front()->getTagProperty();
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
    refreshAttributesEditor();
    // update frame parent (needed to update other attribute tables)
    myFrameParent->attributeUpdated(attr);
}


void
GNEAttributesEditor::enableReparent() {
    if (myEditedACs.front()->getTagProperty()->getParentTags().size() > 0) {
        myReparentTag = myEditedACs.front()->getTagProperty()->getParentTags().front();
        refreshAttributesEditor();
        myFrameParent->getViewNet()->update();
    }
}


void
GNEAttributesEditor::inspectParent() {
    const auto type = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myEditedACs.front()->getAttribute(SUMO_ATTR_TYPE), false);
    if (type) {
        myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectElement(type, myEditedACs.front());
    }
}


void
GNEAttributesEditor::moveLaneUp() {
    const auto lane = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(myEditedACs.front()->getAttribute(SUMO_ATTR_LANE), false);
    if (lane) {
        // set next lane
        setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getChildLanes().at(lane->getIndex() + 1)->getID());
    }
}


void
GNEAttributesEditor::moveLaneDown() {
    const auto lane = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(myEditedACs.front()->getAttribute(SUMO_ATTR_LANE), false);
    if (lane) {
        // set previous lane
        setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getChildLanes().at(lane->getIndex() - 1)->getID());
    }
}


void
GNEAttributesEditor::fillStartEndAttributes(CommonXMLStructure::SumoBaseObject* baseObject) const {
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
GNEAttributesEditor::buildRows(GNEAttributesEditor* editorParent) {
    // only build one time
    if (mySingletonAttributesEditorRows.empty()) {
        const auto tagPropertiesDatabase = editorParent->getFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase();
        // declare vector of types with rows
        const std::vector<AttributeType> types = {AttributeType::BASIC, AttributeType::CHILD, AttributeType::FLOW, AttributeType::GEO, AttributeType::NETEDIT, AttributeType::PARAMETERS};
        // iterate over all types and create their correspond rows
        for (const auto type : types) {
            int maxNumberOfRows = 0;
            // get max number of rows
            if (type == AttributeType::BASIC) {
                maxNumberOfRows = tagPropertiesDatabase->getMaxNumberOfEditableAttributeRows();
            } else if (type == AttributeType::CHILD) {
                maxNumberOfRows = tagPropertiesDatabase->getMaxNumberOfChildAttributeRows();
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
            mySingletonAttributesEditorRows[type].resize(maxNumberOfRows);
            for (int i = 0; i < (int)mySingletonAttributesEditorRows[type].size(); i++) {
                mySingletonAttributesEditorRows[type][i] = new GNEAttributesEditorRow(editorParent);
            }
        }

    }

}

/****************************************************************************/
