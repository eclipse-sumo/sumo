/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEEdgeDataFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// The Widget for add edgeData elements
/****************************************************************************/

#include <cmath>
#include <unordered_map>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_GenericData.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <utils/common/StdDefs.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEEdgeDataFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEEdgeDataFrame::AreaDialog) AreaDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,        GNEEdgeDataFrame::AreaDialog::onCmdSetAttribute),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SET_ATTRIBUTE,        GNEEdgeDataFrame::AreaDialog::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE_BUTTON, GNEEdgeDataFrame::AreaDialog::onCmdSetAttribute),
};

FXIMPLEMENT(GNEEdgeDataFrame::AreaDialog, GNEDialog, AreaDialogMap, ARRAYNUMBER(AreaDialogMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEEdgeDataFrame::AreaDialog::AreaDialog(GNEApplicationWindow* applicationWindow, const Position& pos,
        const std::set<std::string>& attributes, const std::string& defaultAttribute) :
    GNEDialog(applicationWindow, TL("Change edgeData values in area"), GUIIcon::EDGEDATA, DialogType::DEFAULT,
              GNEDialog::Buttons::ACCEPT_CANCEL_RESET, GNEDialog::OpenType::MODAL, GNEDialog::ResizeMode::STATIC),
    myOriginalPosition(pos.x(), pos.y()),
    myOriginalAttribute(defaultAttribute),
    myEditedPosition(pos.x(), pos.y()),
    myAttribute(defaultAttribute) {
    auto tooltipMenu = applicationWindow->getStaticTooltipMenu();
    // Position (X,Y)
    FXHorizontalFrame* posFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(posFrame, TL("Position (X,Y)"), nullptr, GUIDesignLabelThickedFixed(120));
    myPositionTextField = new FXTextField(posFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myPositionTextField->setText((toString(myOriginalPosition.x()) + "," + toString(myOriginalPosition.y())).c_str());
    // Attribute
    FXHorizontalFrame* attrFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(attrFrame, TL("Attribute"), nullptr, GUIDesignLabelThickedFixed(120));
    myAttributeComboBox = new MFXComboBoxIcon(attrFrame, tooltipMenu, false, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    for (const auto& attr : attributes) {
        myAttributeComboBox->appendIconItem(attr.c_str());
    }
    myAttributeComboBox->setNumVisible(MAX2(1, MIN2(10, (int)attributes.size())));
    myAttributeComboBox->setText(defaultAttribute.c_str(), FALSE);
    if (!defaultAttribute.empty() && (myAttributeComboBox->findItem(defaultAttribute.c_str()) >= 0)) {
        myAttributeComboBox->setCurrentItem(defaultAttribute.c_str(), FALSE);
    }
    // Change at point
    FXHorizontalFrame* deltaFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(deltaFrame, TL("Change at point"), nullptr, GUIDesignLabelThickedFixed(120));
    myDeltaTextField = new FXTextField(deltaFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myDeltaTextField->setText("0");
    // Range to 0
    FXHorizontalFrame* rangeFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(rangeFrame, TL("Range to 0"), nullptr, GUIDesignLabelThickedFixed(120));
    myRangeTextField = new FXTextField(rangeFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myRangeTextField->setText("100");
    // Scale mode (linear cone vs smooth bell)
    FXHorizontalFrame* scaleFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(scaleFrame, TL("Scale"), nullptr, GUIDesignLabelThickedFixed(120));
    myLinearRadioButton = new FXRadioButton(scaleFrame, TL("Linear (cone)"), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonFix);
    mySmoothRadioButton = new FXRadioButton(scaleFrame, TL("Smooth (bell)"), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonFix);
    myLinearRadioButton->setCheck(TRUE);
    mySmoothRadioButton->setCheck(FALSE);
    // validate initial state
    validateFields();
    // open modal dialog
    openDialog(defaultAttribute.empty() ? static_cast<FXWindow*>(myAttributeComboBox) : static_cast<FXWindow*>(myDeltaTextField));
}


GNEEdgeDataFrame::AreaDialog::~AreaDialog() {}


bool
GNEEdgeDataFrame::AreaDialog::isValid() const {
    return myValid;
}


const Position&
GNEEdgeDataFrame::AreaDialog::getEditedPosition() const {
    return myEditedPosition;
}


const std::string&
GNEEdgeDataFrame::AreaDialog::getAttributeName() const {
    return myAttribute;
}


double
GNEEdgeDataFrame::AreaDialog::getDelta() const {
    return myDelta;
}


double
GNEEdgeDataFrame::AreaDialog::getRange() const {
    return myRange;
}


bool
GNEEdgeDataFrame::AreaDialog::isSmooth() const {
    return mySmooth;
}


void
GNEEdgeDataFrame::AreaDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEEdgeDataFrame::AreaDialog::onCmdSetAttribute(FXObject* sender, FXSelector, void*) {
    if (sender == myLinearRadioButton) {
        mySmooth = false;
        myLinearRadioButton->setCheck(TRUE);
        mySmoothRadioButton->setCheck(FALSE);
    } else if (sender == mySmoothRadioButton) {
        mySmooth = true;
        myLinearRadioButton->setCheck(FALSE);
        mySmoothRadioButton->setCheck(TRUE);
    }
    validateFields();
    return 1;
}


long
GNEEdgeDataFrame::AreaDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    validateFields();
    if (!myValid) {
        return 1;
    }
    return closeDialogAccepting();
}


long
GNEEdgeDataFrame::AreaDialog::onCmdReset(FXObject*, FXSelector, void*) {
    myPositionTextField->setText((toString(myOriginalPosition.x()) + "," + toString(myOriginalPosition.y())).c_str(), FALSE);
    myAttributeComboBox->setText(myOriginalAttribute.c_str(), FALSE);
    if (!myOriginalAttribute.empty() && (myAttributeComboBox->findItem(myOriginalAttribute.c_str()) >= 0)) {
        myAttributeComboBox->setCurrentItem(myOriginalAttribute.c_str(), FALSE);
    }
    myDeltaTextField->setText("0", FALSE);
    myRangeTextField->setText("100", FALSE);
    mySmooth = false;
    myLinearRadioButton->setCheck(TRUE);
    mySmoothRadioButton->setCheck(FALSE);
    validateFields();
    return 1;
}


void
GNEEdgeDataFrame::AreaDialog::validateFields() {
    bool valid = true;
    // validate X,Y position
    const std::string posStr = myPositionTextField->getText().text();
    if (GNEAttributeCarrier::canParse<Position>(posStr)) {
        const Position parsed = GNEAttributeCarrier::parse<Position>(posStr);
        myEditedPosition.set(parsed.x(), parsed.y(), 0.0);
        myPositionTextField->setTextColor(GUIDesignTextColorBlack);
    } else {
        myPositionTextField->setTextColor(GUIDesignTextColorRed);
        valid = false;
    }
    // validate attribute name
    myAttribute = myAttributeComboBox->getText().text();
    if (!myAttribute.empty() && SUMOXMLDefinitions::isValidParameterKey(myAttribute)) {
        myAttributeComboBox->setTextColor(GUIDesignTextColorBlack);
    } else {
        myAttributeComboBox->setTextColor(GUIDesignTextColorRed);
        valid = false;
    }
    // validate delta
    const std::string deltaStr = myDeltaTextField->getText().text();
    if (GNEAttributeCarrier::canParse<double>(deltaStr)) {
        myDelta = GNEAttributeCarrier::parse<double>(deltaStr);
        myDeltaTextField->setTextColor(GUIDesignTextColorBlack);
    } else {
        myDeltaTextField->setTextColor(GUIDesignTextColorRed);
        valid = false;
    }
    // validate range > 0
    const std::string rangeStr = myRangeTextField->getText().text();
    if (GNEAttributeCarrier::canParse<double>(rangeStr) && (GNEAttributeCarrier::parse<double>(rangeStr) > 0.0)) {
        myRange = GNEAttributeCarrier::parse<double>(rangeStr);
        myRangeTextField->setTextColor(GUIDesignTextColorBlack);
    } else {
        myRangeTextField->setTextColor(GUIDesignTextColorRed);
        valid = false;
    }
    myValid = valid;
    if (myAcceptButton) {
        if (myValid) {
            myAcceptButton->enable();
        } else {
            myAcceptButton->disable();
        }
    }
}

GNEEdgeDataFrame::GNEEdgeDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(viewParent, viewNet, GNE_TAG_EDGEREL_SINGLE, false) {
}


GNEEdgeDataFrame::~GNEEdgeDataFrame() {}


bool
GNEEdgeDataFrame::addEdgeData(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& /*mouseButtonKeyPressed*/) {
    // first check if we clicked over an edge
    if (viewObjects.getEdgeFront() && myDataSetSelector->getDataSet() && myIntervalSelector->getDataInterval()) {
        // check if exist already a edge rel single in the given edge
        if (myIntervalSelector->getDataInterval()->edgeRelSingleExists(viewObjects.getEdgeFront())) {
            // write warning
            WRITE_WARNINGF(TL("There is already a % in edge '%'"), toString(GNE_TAG_EDGEREL_SINGLE), viewObjects.getEdgeFront()->getID());
            // abort edge data creation
            return false;
        }
        // check if parameters are valid
        if (myGenericDataAttributesEditor->checkAttributes(true)) {
            GNEDataHandler dataHandler(myViewNet->getNet(), myDataSetSelector->getDataSet()->getFileBucket(),
                                       myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
            // create interval base object
            CommonXMLStructure::SumoBaseObject* intervalBaseObject = new CommonXMLStructure::SumoBaseObject(nullptr);
            intervalBaseObject->addStringAttribute(SUMO_ATTR_ID, myIntervalSelector->getDataInterval()->getID());
            intervalBaseObject->addDoubleAttribute(SUMO_ATTR_BEGIN, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_BEGIN));
            intervalBaseObject->addDoubleAttribute(SUMO_ATTR_END, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_END));
            // create genericData base object
            CommonXMLStructure::SumoBaseObject* genericDataBaseObject = new CommonXMLStructure::SumoBaseObject(intervalBaseObject);
            // obtain parameters
            myGenericDataAttributesEditor->fillSumoBaseObject(genericDataBaseObject);
            // create edgeData
            dataHandler.buildEdgeData(genericDataBaseObject, viewObjects.getEdgeFront()->getID(), genericDataBaseObject->getParameters());
            // delete data interval object (and child)
            delete intervalBaseObject;
            return true;
        } else {
            return false;
        }
    } else {
        // invalid parent parameters
        return false;
    }
}


void
GNEEdgeDataFrame::openAreaDialog(const Position& pos) {
    auto appWindow = myViewNet->getViewParent()->getGNEAppWindows();
    auto net = myViewNet->getNet();
    // resolve active dataInterval
    GNEDataInterval* dataInterval = myIntervalSelector->getDataInterval();
    if (dataInterval == nullptr) {
        GNEDataSet* dataSet = myDataSetSelector->getDataSet();
        if (dataSet && !dataSet->getDataIntervalChildren().empty()) {
            dataInterval = dataSet->getDataIntervalChildren().begin()->second;
        } else {
            for (const auto& ds : net->getAttributeCarriers()->getDataSets()) {
                if (!ds.second->getDataIntervalChildren().empty()) {
                    dataInterval = ds.second->getDataIntervalChildren().begin()->second;
                    break;
                }
            }
        }
    }
    if (dataInterval == nullptr) {
        GNEWarningBasicDialog(appWindow, TL("No data interval"), TL("Please create a dataSet and dataInterval first."));
        return;
    }
    // collect existing edgeData attributes in active interval (fallback to dataset)
    std::set<std::string> attributes = net->getAttributeCarriers()->retrieveGenericDataParameters(
                                           dataInterval->getDataSetParent()->getID(), toString(GNE_TAG_EDGEREL_SINGLE),
                                           dataInterval->getAttribute(SUMO_ATTR_BEGIN), dataInterval->getAttribute(SUMO_ATTR_END));
    if (attributes.empty()) {
        attributes = net->getAttributeCarriers()->retrieveGenericDataParameters(
                         dataInterval->getDataSetParent()->getID(), toString(GNE_TAG_EDGEREL_SINGLE), "", "");
    }
    std::string defaultAttribute = myAttributeSelector->getFilteredAttribute();
    if (defaultAttribute.empty()) {
        if (!attributes.empty()) {
            defaultAttribute = *attributes.begin();
        } else {
            const auto templateAC = net->getACTemplates()->getTemplateAC(GNE_TAG_EDGEREL_SINGLE);
            if (templateAC && templateAC->getParameters() && !templateAC->getParameters()->getParametersMap().empty()) {
                defaultAttribute = templateAC->getParameters()->getParametersMap().begin()->first;
            }
        }
    }
    // open modal dialog
    const AreaDialog dialog(appWindow, pos, attributes, defaultAttribute);
    if ((dialog.getResult() != GNEDialog::Result::ACCEPT) || !dialog.isValid()) {
        return;
    }
    const Position& center = dialog.getEditedPosition();
    const std::string& attribute = dialog.getAttributeName();
    const double delta = dialog.getDelta();
    const double range = dialog.getRange();
    const bool smooth = dialog.isSmooth();
    // index existing single-edge data elements in the target interval
    std::unordered_map<const GNEEdge*, GNEGenericData*> existingEdgeData;
    for (const auto& genericData : dataInterval->getGenericDataChildren()) {
        if ((genericData->getTagProperty()->getTag() == GNE_TAG_EDGEREL_SINGLE) && !genericData->getParentEdges().empty()) {
            existingEdgeData[genericData->getParentEdges().front()] = genericData;
        }
    }
    // find all edges within range and compute scaled changes
    std::vector<std::pair<GNEEdge*, double> > affectedEdges;
    for (const auto& edgePair : net->getAttributeCarriers()->getEdges()) {
        GNEEdge* edge = edgePair.second;
        const double dist = edge->getNBEdge()->getGeometry().distance2D(center);
        if ((dist >= 0.0) && (dist < range)) {
            const double u = dist / range;
            const double weight = smooth ? (0.5 * (1.0 + std::cos(M_PI * u))) : (1.0 - u);
            affectedEdges.push_back(std::make_pair(edge, delta * weight));
        }
    }
    if (affectedEdges.empty()) {
        return;
    }
    const bool allowUndoRedo = appWindow->isUndoRedoAllowed();
    auto undoList = net->getUndoList();
    if (allowUndoRedo) {
        undoList->begin(GUIIcon::EDGEDATA, TL("change edgeData values in area"));
    }
    for (const auto& item : affectedEdges) {
        GNEEdge* edge = item.first;
        const double scaledDelta = item.second;
        const auto it = existingEdgeData.find(edge);
        if (it != existingEdgeData.end()) {
            GNEGenericData* edgeData = it->second;
            double currentVal = 0.0;
            if (edgeData->hasParameter(attribute)) {
                const std::string curStr = edgeData->getParameter(attribute);
                if (!curStr.empty() && GNEAttributeCarrier::canParse<double>(curStr)) {
                    currentVal = GNEAttributeCarrier::parse<double>(curStr);
                } else if (!curStr.empty()) {
                    WRITE_WARNINGF(TL("Skipping edge '%' with non-numeric attribute '%' value '%'"), edge->getID(), attribute, curStr);
                    continue;
                }
            }
            Parameterised updatedParams(edgeData->getParametersMap());
            updatedParams.setParameter(attribute, toString(currentVal + scaledDelta));
            if (allowUndoRedo) {
                edgeData->setAttribute(GNE_ATTR_PARAMETERS, updatedParams.getParametersStr(), undoList);
            } else {
                edgeData->getParameters()->setParameter(attribute, toString(currentVal + scaledDelta));
            }
        } else {
            Parameterised::Map newParams;
            newParams[attribute] = toString(scaledDelta);
            GNEGenericData* newEdgeData = new GNEEdgeData(dataInterval, edge, newParams);
            if (allowUndoRedo) {
                undoList->add(new GNEChange_GenericData(newEdgeData, true), true);
            } else {
                dataInterval->addGenericDataChild(newEdgeData);
                edge->addChildElement(newEdgeData);
                newEdgeData->incRef("openAreaDialog");
            }
        }
    }
    if (allowUndoRedo) {
        undoList->end();
    } else {
        dataInterval->getDataSetParent()->updateAttributeColors();
    }
    myAttributeSelector->refreshAttributeSelector();
    myViewNet->getIntervalBar().markForUpdate();
    myViewNet->updateViewNet();
}


/****************************************************************************/
