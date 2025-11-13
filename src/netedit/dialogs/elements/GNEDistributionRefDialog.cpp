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
/// @file    GNEDistributionRefDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog for edit attribute carriers
/****************************************************************************/

#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/demand/GNERouteRef.h>
#include <netedit/elements/demand/GNEVTypeRef.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDistributionRefDialog.h"

// ===========================================================================
// Defines
// ===========================================================================

#define BLACK_COLOR MFXUtils::getFXColor(RGBColor::BLACK)
#define BLUE_COLOR MFXUtils::getFXColor(RGBColor::BLUE)
#define RED_COLOR MFXUtils::getFXColor(RGBColor::RED)

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionRefDialog) GNEDistributionRefDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DISTRIBUTIONDIALOG_REFERENCE,   GNEDistributionRefDialog::onCmdSetReference),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DISTRIBUTIONDIALOG_PROBABILITY, GNEDistributionRefDialog::onCmdSetProbability)
};

// Object implementation
FXIMPLEMENT(GNEDistributionRefDialog, GNEDialog, GNEDistributionRefDialogMap, ARRAYNUMBER(GNEDistributionRefDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDistributionRefDialog::GNEDistributionRefDialog(GNEAttributeCarrier* distributionParent) :
    GNEDialog(distributionParent->getNet()->getGNEApplicationWindow(),
              TLF("Add % reference", distributionParent->getTagStr()), distributionParent->getTagProperty()->getGUIIcon(),
              DialogType::DISTRIBUTION_REF, GNEDialog::Buttons::ACCEPT_CANCEL, OpenType::MODAL, ResizeMode::STATIC),
    myDistributionParent(distributionParent) {
    auto tooltipMenu = distributionParent->getNet()->getGNEApplicationWindow()->getStaticTooltipMenu();
    // create reference elements
    FXHorizontalFrame* referenceFrames = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    auto referenceLabel = new FXLabel(referenceFrames, "reference", nullptr, GUIDesignLabelThickedFixed(100));
    myReferencesComboBox = new MFXComboBoxIcon(referenceFrames, tooltipMenu, true, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_DISTRIBUTIONDIALOG_REFERENCE, GUIDesignComboBox);
    // create probability elements
    FXHorizontalFrame* probabilityFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(probabilityFrame, toString(SUMO_ATTR_PROB).c_str(), nullptr, GUIDesignLabelThickedFixed(100));
    myProbabilityTextField = new MFXTextFieldIcon(probabilityFrame, tooltipMenu, GUIIcon::EMPTY, this,
            MID_GNE_DISTRIBUTIONDIALOG_PROBABILITY, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    // extract all references
    std::set<std::string> referenceIDs;
    for (const auto& reference : distributionParent->getHierarchicalElement()->getChildDemandElements()) {
        if (reference->getTagProperty()->isDistributionReference()) {
            referenceIDs.insert(reference->getParentDemandElements().at(1)->getID());
        }
    }
    // continue depending on distributionParent
    if (distributionParent->getTagProperty()->getTag() == SUMO_TAG_VTYPE_DISTRIBUTION) {
        referenceLabel->setText(toString(SUMO_TAG_VTYPE).c_str());
        const auto& vTypes = distributionParent->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE);
        // obtain candidate IDs
        for (const auto& vType : vTypes) {
            if (referenceIDs.count(vType.second->getID()) == 0) {
                myCandidates[vType.second->getID()] = vType.second;
            }
        }
        // insert it in comboBox
        for (const auto& vTypeID : myCandidates) {
            myReferencesComboBox->appendIconItem(vTypeID.first.c_str());
        }
    } else if (distributionParent->getTagProperty()->getTag() == SUMO_TAG_ROUTE_DISTRIBUTION) {
        referenceLabel->setText(toString(SUMO_TAG_ROUTE).c_str());
        const auto& routes = distributionParent->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE);
        // obtain candidate IDs
        for (const auto& route : routes) {
            if (referenceIDs.count(route.second->getID()) == 0) {
                myCandidates[route.second->getID()] = route.second;
            }
        }
        // insert it in comboBox
        for (const auto& routeID : myCandidates) {
            myReferencesComboBox->appendIconItem(routeID.first.c_str());
        }
    }
    // set current item (for update probability)
    myReferencesComboBox->setCurrentItem(0, TRUE);
    // open dialog
    openDialog();
}


GNEDistributionRefDialog::~GNEDistributionRefDialog() {}


void
GNEDistributionRefDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEDistributionRefDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if ((myReferencesComboBox->getTextColor() != RED_COLOR) &&
            (myProbabilityTextField->getTextColor() != RED_COLOR)) {
        // declare referenced element
        GNEDemandElement* reference = nullptr;
        const double probability = GNEAttributeCarrier::parse<double>(myProbabilityTextField->getText().text());
        GNEDemandElement* distribution = myDistributionParent->getNet()->getAttributeCarriers()->retrieveDemandElement(myDistributionParent->getGUIGlObject());
        auto undoList = myDistributionParent->getNet()->getUndoList();
        // create a routeRef o a vTypeRef
        if (distribution->getTagProperty()->getTag() == SUMO_TAG_VTYPE_DISTRIBUTION) {
            if (myProbabilityTextField->getTextColor() == BLUE_COLOR) {
                reference = new GNEVTypeRef(distribution, myReferencedElement);
            } else {
                reference = new GNEVTypeRef(distribution, myReferencedElement, probability);
            }
        } else if (distribution->getTagProperty()->getTag() == SUMO_TAG_ROUTE_DISTRIBUTION) {
            if (myProbabilityTextField->getTextColor() == BLUE_COLOR) {
                reference = new GNERouteRef(distribution, myReferencedElement);
            } else {
                reference = new GNERouteRef(distribution, myReferencedElement, probability);
            }
        }
        // continue depending if allow/disallow is enabled
        if (myDistributionParent->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
            undoList->begin(myReferencedElement, TLF("add % in '%'", myReferencedElement->getTagStr(), distribution->getID()));
            undoList->add(new GNEChange_DemandElement(reference, true), true);
            undoList->end();
        } else {
            myDistributionParent->getNet()->getAttributeCarriers()->insertDemandElement(reference);
            distribution->addChildElement(reference);
            myReferencedElement->addChildElement(reference);
            reference->incRef("GNEDistributionRefDialog");
        }
        return closeDialogAccepting();
    } else {
        return closeDialogCanceling();
    }
}


long
GNEDistributionRefDialog::onCmdSetReference(FXObject*, FXSelector, void*) {
    // check if candidateID exist in list of candidates
    GNEDemandElement* reference = nullptr;
    for (const auto& candidate : myCandidates) {
        if (myReferencesComboBox->getText().text() == candidate.first) {
            reference = candidate.second;
            break;
        }
    }
    // continue depending if selected candidate exist
    if (reference == nullptr) {
        myReferencesComboBox->setTextColor(RED_COLOR);
    } else {
        myReferencesComboBox->setTextColor(BLACK_COLOR);
        myReferencedElement = reference;
        // set default probability
        myProbabilityTextField->setText(myReferencedElement->getAttribute(SUMO_ATTR_PROB).c_str(), TRUE);
        // check if enable or disable accept button
        if ((myReferencesComboBox->getTextColor() != RED_COLOR) &&
                (myProbabilityTextField->getTextColor() != RED_COLOR)) {
            myAcceptButton->enable();
        } else {
            myAcceptButton->disable();
        }
    }
    return 1;
}


long
GNEDistributionRefDialog::onCmdSetProbability(FXObject*, FXSelector, void*) {
    // first check if set default probability
    if (myProbabilityTextField->getText().empty()) {
        myProbabilityTextField->setText(myReferencedElement->getAttribute(SUMO_ATTR_PROB).c_str(), TRUE);
    }
    // check if value can be parsed to double
    if (GNEAttributeCarrier::canParse<double>(myProbabilityTextField->getText().text())) {
        if (myProbabilityTextField->getText().text() == myReferencedElement->getAttribute(SUMO_ATTR_PROB)) {
            myProbabilityTextField->setTextColor(BLUE_COLOR);
        } else {
            myProbabilityTextField->setTextColor(BLACK_COLOR);
        }
    } else {
        myProbabilityTextField->setTextColor(RED_COLOR);
    }
    // check if enable or disable accept button
    if ((myReferencesComboBox->getTextColor() != RED_COLOR) &&
            (myProbabilityTextField->getTextColor() != RED_COLOR)) {
        myAcceptButton->enable();
    } else {
        myAcceptButton->disable();
    }
    return 1;
}

/****************************************************************************/
