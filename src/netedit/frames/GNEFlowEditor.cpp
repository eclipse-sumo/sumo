/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEFlowEditor.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
/// Flow editor
/****************************************************************************/
#include <config.h>

#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFlowEditor.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFlowEditor) FlowEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEFlowEditor::onCmdSetFlowAttribute),
};

// Object implementation
FXIMPLEMENT(GNEFlowEditor,                   MFXGroupBoxModule,       FlowEditorMap,                  ARRAYNUMBER(FlowEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEFlowEditor::GNEFlowEditor(GNEViewNet* viewNet, GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Flow attributes")),
    myViewNet(viewNet) {
    // create comboBox for option A
    FXHorizontalFrame* auxiliarHorizontalFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    auto terminatelabel = new FXLabel(auxiliarHorizontalFrame, "terminate", nullptr, GUIDesignLabelThickedFixed(100));
    terminatelabel->setTipText("Terminate attribute");
    myTerminateComboBox = new FXComboBox(auxiliarHorizontalFrame, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // create comboBox for spacing
    mySpacingFrameComboBox = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    auto spacingAttribute = new FXLabel(mySpacingFrameComboBox, "spacing", nullptr, GUIDesignLabelThickedFixed(100));
    spacingAttribute->setTipText("Terminate attribute");
    mySpacingComboBox = new FXComboBox(mySpacingFrameComboBox, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // create textField for option A
    myTerminateFrameTextField = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    myTerminateLabel = new MFXLabelTooltip(myTerminateFrameTextField,
                                           frameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(), "A", nullptr, GUIDesignLabelThickedFixed(100));
    myTerminateTextField = new FXTextField(myTerminateFrameTextField, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for spacing
    mySpacingFrameTextField = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    mySpacingLabel = new MFXLabelTooltip(mySpacingFrameTextField, frameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                         "B", nullptr, GUIDesignLabelThickedFixed(100));
    mySpacingTextField = new FXTextField(mySpacingFrameTextField, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // fill terminate
    myTerminateComboBox->appendItem(toString(SUMO_ATTR_END).c_str());
    myTerminateComboBox->appendItem(toString(SUMO_ATTR_NUMBER).c_str());
    myTerminateComboBox->appendItem((toString(SUMO_ATTR_END) + "-" + toString(SUMO_ATTR_NUMBER)).c_str());
    myTerminateComboBox->setNumVisible(3);
    // fill comboBox B
    mySpacingComboBox->appendItem(toString(SUMO_ATTR_VEHSPERHOUR).c_str());
    mySpacingComboBox->appendItem(toString(SUMO_ATTR_PERIOD).c_str());
    mySpacingComboBox->appendItem(toString(GNE_ATTR_POISSON).c_str());
    mySpacingComboBox->appendItem(toString(SUMO_ATTR_PROB).c_str());
    mySpacingComboBox->setNumVisible(4);
}


GNEFlowEditor::~GNEFlowEditor() {}


void
GNEFlowEditor::showFlowEditor(const std::vector<GNEAttributeCarrier*> editedFlows) {
    // update flows
    myEditedFlows = editedFlows;
    // check number of flows
    if (myEditedFlows.size() > 0) {
        // update per hour attr
        if (myEditedFlows.front()->getTagProperty().hasAttribute(SUMO_ATTR_PERSONSPERHOUR)) {
            myPerHourAttr = SUMO_ATTR_PERSONSPERHOUR;
        } else if (myEditedFlows.front()->getTagProperty().hasAttribute(SUMO_ATTR_CONTAINERSPERHOUR)) {
            myPerHourAttr = SUMO_ATTR_CONTAINERSPERHOUR;
        } else {
            myPerHourAttr = SUMO_ATTR_VEHSPERHOUR;
        }
        // clear and update comboBoxB
        mySpacingComboBox->clearItems();
        mySpacingComboBox->appendItem(toString(myPerHourAttr).c_str());
        mySpacingComboBox->appendItem(toString(SUMO_ATTR_PERIOD).c_str());
        mySpacingComboBox->appendItem(toString(GNE_ATTR_POISSON).c_str());
        mySpacingComboBox->appendItem(toString(SUMO_ATTR_PROB).c_str());
        mySpacingComboBox->setNumVisible(4);
        // refresh
        refreshFlowEditor();
        // show
        show();
    }
}


void
GNEFlowEditor::hideFlowEditor() {
    hide();
}


bool
GNEFlowEditor::shownFlowEditor() const {
    return shown();
}


void
GNEFlowEditor::refreshFlowEditor() {
    // show both attributes
    myTerminateFrameTextField->show();
    mySpacingFrameTextField->show();
    // reset colors
    myTerminateTextField->setTextColor(FXRGB(0, 0, 0));
    myTerminateTextField->killFocus();
    mySpacingTextField->setTextColor(FXRGB(0, 0, 0));
    mySpacingTextField->killFocus();
    // continue depending of number of flow
    if (myEditedFlows.size() == 1) {
        refreshSingleFlow();
    } else if (myEditedFlows.size() > 1) {
        refreshMultipleFlows();
    }
    // recalc
    recalc();
}


void
GNEFlowEditor::getFlowAttributes(CommonXMLStructure::SumoBaseObject* baseObject) {
    // case end-number
    if (myTerminateLabel->getText().text() == toString(SUMO_ATTR_END)) {
        baseObject->addDoubleAttribute(SUMO_ATTR_END, GNEAttributeCarrier::parse<double>(myTerminateTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(SUMO_ATTR_NUMBER)) {
        baseObject->addIntAttribute(SUMO_ATTR_NUMBER, GNEAttributeCarrier::parse<int>(mySpacingTextField->getText().text()));
    }
    // other cases
    if (myTerminateLabel->getText().text() == toString(SUMO_ATTR_NUMBER)) {
        baseObject->addIntAttribute(SUMO_ATTR_NUMBER, GNEAttributeCarrier::parse<int>(myTerminateTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(myPerHourAttr)) {
        baseObject->addDoubleAttribute(myPerHourAttr, GNEAttributeCarrier::parse<double>(mySpacingTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(SUMO_ATTR_PERIOD)) {
        baseObject->addDoubleAttribute(SUMO_ATTR_PERIOD, GNEAttributeCarrier::parse<double>(mySpacingTextField->getText().text()));
    }
    if (mySpacingLabel->getText() == TL("rate")) {
        baseObject->addDoubleAttribute(GNE_ATTR_POISSON, GNEAttributeCarrier::parse<double>(mySpacingTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(SUMO_ATTR_PROB)) {
        baseObject->addDoubleAttribute(SUMO_ATTR_PROB, GNEAttributeCarrier::parse<double>(mySpacingTextField->getText().text()));
    }
}


bool
GNEFlowEditor::areFlowValuesValid() const {
    // check text fields
    if (myTerminateFrameTextField->shown() && (myTerminateTextField->getTextColor() == FXRGB(0, 0, 0)) &&
            mySpacingFrameTextField->shown() && (mySpacingTextField->getTextColor() == FXRGB(0, 0, 0))) {
        return true;
    } else {
        return false;
    }
}


long
GNEFlowEditor::onCmdSetFlowAttribute(FXObject* obj, FXSelector, void*) {
    // check number of flows
    if (myEditedFlows.front()) {
        // declare vectors for enable/disable attributes
        std::vector<SumoXMLAttr> enableAttrs, disableAttrs;
        // check if all spacing attributes are disabled
        const bool spacingEnabled = myEditedFlows.front()->isAttributeEnabled(myPerHourAttr) ||
                                    myEditedFlows.front()->isAttributeEnabled(SUMO_ATTR_PERIOD) ||
                                    myEditedFlows.front()->isAttributeEnabled(GNE_ATTR_POISSON) ||
                                    myEditedFlows.front()->isAttributeEnabled(SUMO_ATTR_PROB);
        // get special case endNumber
        const bool endNumber = (myTerminateComboBox->getText().text() == (toString(SUMO_ATTR_END) + "-" + toString(SUMO_ATTR_NUMBER)));
        // get terminate attribute
        SumoXMLAttr terminateAttribute = SUMO_ATTR_NOTHING;
        if (myTerminateComboBox->getText().text() == toString(SUMO_ATTR_END) || endNumber) {
            terminateAttribute = SUMO_ATTR_END;
        } else if (myTerminateComboBox->getText().text() == toString(SUMO_ATTR_NUMBER)) {
            terminateAttribute = SUMO_ATTR_NUMBER;
        }
        // get spacing attribute
        SumoXMLAttr spacingAttribute = SUMO_ATTR_NOTHING;
        if (endNumber) {
            spacingAttribute = SUMO_ATTR_NUMBER;
        } else if (mySpacingComboBox->getText().text() == toString(myPerHourAttr)) {
            spacingAttribute = myPerHourAttr;
        } else if (mySpacingComboBox->getText().text() == toString(SUMO_ATTR_PERIOD)) {
            spacingAttribute = SUMO_ATTR_PERIOD;
        } else if (mySpacingComboBox->getText().text() == toString(GNE_ATTR_POISSON)) {
            spacingAttribute = GNE_ATTR_POISSON;
        } else if (mySpacingComboBox->getText().text() == toString(SUMO_ATTR_PROB)) {
            spacingAttribute = SUMO_ATTR_PROB;
        }
        // check if obj is a comboBox or a text field
        if (obj == myTerminateComboBox) {
            if (endNumber) {
                enableAttrs.push_back(SUMO_ATTR_END);
                enableAttrs.push_back(SUMO_ATTR_NUMBER);
                // disable others
                disableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(GNE_ATTR_POISSON);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                myTerminateComboBox->setTextColor(FXRGB(0, 0, 0));
                myTerminateComboBox->killFocus();
            } else if (terminateAttribute == SUMO_ATTR_END) {
                enableAttrs.push_back(SUMO_ATTR_END);
                disableAttrs.push_back(SUMO_ATTR_NUMBER);
                // at least enable one spacing attribute
                if (!spacingEnabled) {
                    enableAttrs.push_back(myPerHourAttr);
                }
                // reset color
                myTerminateComboBox->setTextColor(FXRGB(0, 0, 0));
                myTerminateComboBox->killFocus();
            } else if (terminateAttribute == SUMO_ATTR_NUMBER) {
                disableAttrs.push_back(SUMO_ATTR_END);
                enableAttrs.push_back(SUMO_ATTR_NUMBER);
                // at least enable one spacing attribute
                if (!spacingEnabled) {
                    enableAttrs.push_back(myPerHourAttr);
                }
                // reset color
                myTerminateComboBox->setTextColor(FXRGB(0, 0, 0));
                myTerminateComboBox->killFocus();
            } else {
                // disable both
                disableAttrs.push_back(SUMO_ATTR_END);
                disableAttrs.push_back(SUMO_ATTR_NUMBER);
                // set invalid color
                myTerminateComboBox->setTextColor(FXRGB(255, 0, 0));
            }
        } else if (obj == mySpacingComboBox) {
            if (spacingAttribute == myPerHourAttr) {
                enableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(GNE_ATTR_POISSON);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                mySpacingComboBox->setTextColor(FXRGB(0, 0, 0));
                mySpacingComboBox->killFocus();
            } else if (spacingAttribute == SUMO_ATTR_PERIOD) {
                disableAttrs.push_back(myPerHourAttr);
                enableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(GNE_ATTR_POISSON);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                mySpacingComboBox->setTextColor(FXRGB(0, 0, 0));
                mySpacingComboBox->killFocus();
            } else if (spacingAttribute == GNE_ATTR_POISSON) {
                disableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                enableAttrs.push_back(GNE_ATTR_POISSON);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                mySpacingComboBox->setTextColor(FXRGB(0, 0, 0));
                mySpacingComboBox->killFocus();
            } else if (spacingAttribute == SUMO_ATTR_PROB) {
                disableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(GNE_ATTR_POISSON);
                enableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                mySpacingComboBox->setTextColor(FXRGB(0, 0, 0));
                mySpacingComboBox->killFocus();
            } else {
                // disable all
                disableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(GNE_ATTR_POISSON);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // set invalid color
                mySpacingComboBox->setTextColor(FXRGB(255, 0, 0));
            }
        } else if ((obj == myTerminateTextField) && (terminateAttribute != SUMO_ATTR_NOTHING)) {
            if (myEditedFlows.front()->isValid(terminateAttribute, myTerminateTextField->getText().text())) {
                // continue depending of flow
                if (myEditedFlows.front()->isTemplate()) {
                    // change attribute directly
                    myEditedFlows.front()->setAttribute(terminateAttribute, myTerminateTextField->getText().text());
                } else if (myEditedFlows.size() == 1) {
                    // change using undoList
                    myEditedFlows.front()->setAttribute(terminateAttribute, myTerminateTextField->getText().text(), myViewNet->getUndoList());
                } else {
                    // change all flows using undoList
                    myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), "change multiple flow attributes");
                    for (const auto& flow : myEditedFlows) {
                        flow->setAttribute(terminateAttribute, myTerminateTextField->getText().text(), myViewNet->getUndoList());
                    }
                    myViewNet->getUndoList()->end();
                }
                // reset color
                myTerminateTextField->setTextColor(FXRGB(0, 0, 0));
                myTerminateTextField->killFocus();
            } else {
                // set invalid color
                myTerminateTextField->setTextColor(FXRGB(255, 0, 0));
                // stop
                return 1;
            }
        } else if ((obj == mySpacingTextField) && (spacingAttribute != SUMO_ATTR_NOTHING)) {
            if (myEditedFlows.front()->isValid(spacingAttribute, mySpacingTextField->getText().text())) {
                // continue depending of flow
                if (myEditedFlows.front()->isTemplate()) {
                    // change attribute directly
                    myEditedFlows.front()->setAttribute(spacingAttribute, mySpacingTextField->getText().text());
                } else if (myEditedFlows.size() == 1) {
                    // change using undoList
                    myEditedFlows.front()->setAttribute(spacingAttribute, mySpacingTextField->getText().text(), myViewNet->getUndoList());
                } else {
                    // change all flows using undoList
                    myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), TL("change multiple flow attributes"));
                    for (const auto& flow : myEditedFlows) {
                        flow->setAttribute(spacingAttribute, mySpacingTextField->getText().text(), myViewNet->getUndoList());
                    }
                    myViewNet->getUndoList()->end();
                }
                // reset color
                mySpacingTextField->setTextColor(FXRGB(0, 0, 0));
                mySpacingTextField->killFocus();
            } else {
                // set invalid color
                mySpacingTextField->setTextColor(FXRGB(255, 0, 0));
                // stop
                return 1;
            }
        }
        // enable and disable attributes
        for (const auto& attr : enableAttrs) {
            if (myEditedFlows.front()->isTemplate()) {
                // enable directly
                myEditedFlows.front()->toggleAttribute(attr, true);
            } else if (myEditedFlows.size() == 1) {
                // enable using undoList
                myEditedFlows.front()->enableAttribute(attr, myViewNet->getUndoList());
            } else {
                // enable in all flow using undoList
                myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), TL("enable multiple flow attributes"));
                for (const auto& flow : myEditedFlows) {
                    flow->enableAttribute(attr, myViewNet->getUndoList());
                }
                myViewNet->getUndoList()->end();
            }
        }
        for (const auto& attr : disableAttrs) {
            if (myEditedFlows.front()->isTemplate()) {
                // disable directly
                myEditedFlows.front()->toggleAttribute(attr, false);
            } else if (myEditedFlows.size() == 1) {
                // disable using undoList
                myEditedFlows.front()->disableAttribute(attr, myViewNet->getUndoList());
            } else {
                // disable in all flow using undoList
                myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), TL("disable multiple flow attributes"));
                for (const auto& flow : myEditedFlows) {
                    flow->disableAttribute(attr, myViewNet->getUndoList());
                }
                myViewNet->getUndoList()->end();
            }
        }
        // refresh flow editor
        refreshFlowEditor();
    }
    return 1;
}


void
GNEFlowEditor::refreshSingleFlow() {
    // get flow (only for code legibly)
    const auto flow = myEditedFlows.front();
    // continue depending of combinations
    if (flow->isAttributeEnabled(SUMO_ATTR_END) && flow->isAttributeEnabled(SUMO_ATTR_NUMBER)) {
        // set first comboBox
        myTerminateComboBox->setCurrentItem(2),
                            // hide second comboBox
                            mySpacingFrameComboBox->hide();
        // set label
        myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
        myTerminateLabel->setTipText(flow->getTagProperty().getAttributeProperties(SUMO_ATTR_END).getDefinition().c_str());
        mySpacingLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
        mySpacingLabel->setTipText(flow->getTagProperty().getAttributeProperties(SUMO_ATTR_NUMBER).getDefinition().c_str());
        // set text fields
        myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
        mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
    } else {
        // show second comboBox
        mySpacingFrameComboBox->show();
        // set first attribute
        if (myTerminateComboBox->getTextColor() == FXRGB(255, 0, 0)) {
            // invalid combination, disable text field
            myTerminateFrameTextField->hide();
        } else if (flow->isAttributeEnabled(SUMO_ATTR_END)) {
            // set first comboBox
            myTerminateComboBox->setCurrentItem(0);
            // set label
            myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
            // set definition
            myTerminateLabel->setTipText(flow->getTagProperty().getAttributeProperties(SUMO_ATTR_END).getDefinition().c_str());
            // set text fields
            myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
        } else if (flow->isAttributeEnabled(SUMO_ATTR_NUMBER)) {
            // set first comboBox
            myTerminateComboBox->setCurrentItem(1);
            // set label
            myTerminateLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
            // set definition
            myTerminateLabel->setTipText(flow->getTagProperty().getAttributeProperties(SUMO_ATTR_NUMBER).getDefinition().c_str());
            // set text fields
            myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
        }
        // set second attribute
        if (mySpacingComboBox->getTextColor() == FXRGB(255, 0, 0)) {
            // invalid combination, disable text field
            mySpacingFrameTextField->hide();
        } else if (flow->isAttributeEnabled(myPerHourAttr)) {
            // set first comboBox
            mySpacingComboBox->setCurrentItem(0),
                              // set label
                              mySpacingLabel->setText(toString(myPerHourAttr).c_str());
            // set tip text
            mySpacingLabel->setTipText(flow->getTagProperty().getAttributeProperties(myPerHourAttr).getDefinition().c_str());
            // set text fields
            mySpacingTextField->setText(getFlowAttribute(myPerHourAttr).c_str());
        } else if (flow->isAttributeEnabled(SUMO_ATTR_PERIOD)) {
            // set first comboBox
            mySpacingComboBox->setCurrentItem(1);
            // set label
            mySpacingLabel->setText(toString(SUMO_ATTR_PERIOD).c_str());
            // set tip text
            mySpacingLabel->setTipText(flow->getTagProperty().getAttributeProperties(SUMO_ATTR_PERIOD).getDefinition().c_str());
            // set text fields
            mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PERIOD).c_str());
        } else if (flow->isAttributeEnabled(GNE_ATTR_POISSON)) {
            // set first comboBox
            mySpacingComboBox->setCurrentItem(2);
            // set label
            mySpacingLabel->setText(TL("rate"));
            // set definition
            mySpacingLabel->setTipText(flow->getTagProperty().getAttributeProperties(GNE_ATTR_POISSON).getDefinition().c_str());
            // set text fields
            mySpacingTextField->setText(getFlowAttribute(GNE_ATTR_POISSON).c_str());
        } else if (flow->isAttributeEnabled(SUMO_ATTR_PROB)) {
            // set first comboBox
            mySpacingComboBox->setCurrentItem(3);
            // set label
            mySpacingLabel->setText(toString(SUMO_ATTR_PROB).c_str());
            // set tip text
            mySpacingLabel->setTipText(flow->getTagProperty().getAttributeProperties(SUMO_ATTR_PROB).getDefinition().c_str());
            // set text fields
            mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PROB).c_str());
        }
    }
}


void
GNEFlowEditor::refreshMultipleFlows() {
    // get first flow (only for code legibly)
    const auto editedFlow = myEditedFlows.front();
    // get values of first flow
    const bool end = editedFlow->isAttributeEnabled(SUMO_ATTR_END);
    const bool number = editedFlow->isAttributeEnabled(SUMO_ATTR_NUMBER);
    const bool perhour = editedFlow->isAttributeEnabled(myPerHourAttr);
    const bool period = editedFlow->isAttributeEnabled(SUMO_ATTR_PERIOD);
    const bool poisson = editedFlow->isAttributeEnabled(GNE_ATTR_POISSON);
    const bool probability = editedFlow->isAttributeEnabled(SUMO_ATTR_PROB);
    // we need to check if attributes are defined differents in flows
    std::vector<std::string> terminateDifferent;
    std::vector<std::string> spacingDifferent;
    // iterate over all flows
    for (const auto& flow : myEditedFlows) {
        if (flow->isAttributeEnabled(SUMO_ATTR_END) != end) {
            terminateDifferent.push_back(toString(SUMO_ATTR_END));
        }
        if (flow->isAttributeEnabled(SUMO_ATTR_NUMBER) != number) {
            terminateDifferent.push_back(toString(SUMO_ATTR_NUMBER));
        }
        if (flow->isAttributeEnabled(myPerHourAttr) != perhour) {
            spacingDifferent.push_back(toString(myPerHourAttr));
        }
        if (flow->isAttributeEnabled(SUMO_ATTR_PERIOD) != period) {
            spacingDifferent.push_back(toString(SUMO_ATTR_PERIOD));
        }
        if (flow->isAttributeEnabled(GNE_ATTR_POISSON) != poisson) {
            spacingDifferent.push_back(toString(GNE_ATTR_POISSON));
        }
        if (flow->isAttributeEnabled(SUMO_ATTR_PROB) != probability) {
            spacingDifferent.push_back(toString(SUMO_ATTR_PROB));
        }
    }
    // special case for end and number
    if (end && number && terminateDifferent.empty() && spacingDifferent.empty()) {
        // set first comboBox
        myTerminateComboBox->setCurrentItem(2),
                            // hide second comboBox
                            mySpacingFrameComboBox->hide();
        // set label
        myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
        mySpacingLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
        // set text fields
        myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
        mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
    } else {
        // show second comboBox
        mySpacingFrameComboBox->show();
        // check terminateDifferent
        if (terminateDifferent.size() > 0) {
            myTerminateComboBox->setText((TL("different: ") + terminateDifferent.front() + " " + terminateDifferent.back()).c_str());
            // hide textField
            myTerminateFrameTextField->hide();
        } else {
            // show textField
            myTerminateFrameTextField->show();
            // set first attribute
            if (myTerminateComboBox->getTextColor() == FXRGB(255, 0, 0)) {
                // invalid combination, disable text field
                myTerminateFrameTextField->hide();
            } else if (end) {
                // set first comboBox
                myTerminateComboBox->setCurrentItem(0),
                                    // set label
                                    myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
                // set text fields
                myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
            } else if (number) {
                // set first comboBox
                myTerminateComboBox->setCurrentItem(1),
                                    // set label
                                    myTerminateLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
                // set text fields
                myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
            }
        }
        // check terminateDifferent
        if (spacingDifferent.size() > 0) {
            mySpacingComboBox->setText((TL("different: ") + spacingDifferent.front() + " " + spacingDifferent.back()).c_str());
            // hide textField
            mySpacingFrameTextField->hide();
        } else {
            // show textField
            mySpacingFrameTextField->show();
            // set second attribute
            if (mySpacingComboBox->getTextColor() == FXRGB(255, 0, 0)) {
                // invalid combination, disable text field
                mySpacingFrameTextField->hide();
            } else if (perhour) {
                // set first comboBox
                mySpacingComboBox->setCurrentItem(0),
                                  // set label
                                  mySpacingLabel->setText(toString(myPerHourAttr).c_str());
                // set text fields
                mySpacingTextField->setText(getFlowAttribute(myPerHourAttr).c_str());
            } else if (period) {
                // set first comboBox
                mySpacingComboBox->setCurrentItem(1),
                                  // set label
                                  mySpacingLabel->setText(toString(SUMO_ATTR_PERIOD).c_str());
                // set text fields
                mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PERIOD).c_str());
            } else if (poisson) {
                // set first comboBox
                mySpacingComboBox->setCurrentItem(2),
                                  // set label
                                  mySpacingLabel->setText(TL("rate"));
                // set text fields
                mySpacingTextField->setText(getFlowAttribute(GNE_ATTR_POISSON).c_str());
            } else if (probability) {
                // set first comboBox
                mySpacingComboBox->setCurrentItem(3),
                                  // set label
                                  mySpacingLabel->setText(toString(SUMO_ATTR_PROB).c_str());
                // set text fields
                mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PROB).c_str());
            }
        }
    }
}


const std::string
GNEFlowEditor::getFlowAttribute(SumoXMLAttr attr) {
    if (myEditedFlows.size() == 1) {
        return myEditedFlows.front()->getAttribute(attr);
    } else {
        std::string solution;
        std::set<std::string> values;
        // extract all values (avoiding duplicated)
        for (const auto& flow : myEditedFlows) {
            values.insert(flow->getAttribute(attr));
        }
        // insert value and space
        for (const auto& value : values) {
            solution.append(value + " ");
        }
        // remove last space
        if (solution.size() > 0) {
            solution.pop_back();
        }
        return solution;
    }
}


/****************************************************************************/
