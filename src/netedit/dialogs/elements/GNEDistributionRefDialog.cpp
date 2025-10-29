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
    GNEDialog(distributionParent->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
              TLF("Add % reference", distributionParent->getTagStr()), distributionParent->getTagProperty()->getGUIIcon(),
              DialogType::DISTRIBUTION_REF, GNEDialog::Buttons::ACCEPT_CANCEL, OpenType::MODAL, ResizeMode::STATIC),
    myDistributionParent(distributionParent) {
    auto tooltipMenu = distributionParent->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // create reference elements
    FXHorizontalFrame* referenceFrames = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    auto referenceLabel = new FXLabel(referenceFrames, "reference", nullptr, GUIDesignLabelThickedFixed(100));
    myReferencesComboBox = new MFXComboBoxIcon(referenceFrames, tooltipMenu, true, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_DISTRIBUTIONDIALOG_REFERENCE, GUIDesignComboBox);
    // create probability elements
    FXHorizontalFrame* probabilityFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(probabilityFrame, toString(SUMO_ATTR_PROB).c_str(), nullptr, GUIDesignLabelThickedFixed(100));
    myProbabilityTextField = new MFXTextFieldIcon(probabilityFrame, tooltipMenu, GUIIcon::EMPTY, this,
            MID_GNE_DISTRIBUTIONDIALOG_PROBABILITY, GUIDesignTextField);
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
                myCandidateIDs.insert(vType.second->getID());
            }
        }
        // insert it in comboBox
        for (const auto& vTypeID : myCandidateIDs) {
            myReferencesComboBox->appendIconItem(vTypeID.c_str());
        }
        // set default probability
        myDefaultprobability = distributionParent->getNet()->getTagPropertiesDatabase()->getTagProperty(GNE_TAG_VTYPEREF, true)->getAttributeProperties(SUMO_ATTR_PROB)->getDefaultStringValue();
        myProbabilityTextField->setText(myDefaultprobability.c_str());
    } else if (distributionParent->getTagProperty()->getTag() == SUMO_TAG_ROUTE_DISTRIBUTION) {
        referenceLabel->setText(toString(SUMO_TAG_ROUTE).c_str());
        const auto& routes = distributionParent->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE);
        // obtain candidate IDs
        for (const auto& route : routes) {
            if (referenceIDs.count(route.second->getID()) == 0) {
                myCandidateIDs.insert(route.second->getID());
            }
        }
        // insert it in comboBox
        for (const auto& routeID : myCandidateIDs) {
            myReferencesComboBox->appendIconItem(routeID.c_str());
        }
        // set default probability
        myDefaultprobability = distributionParent->getNet()->getTagPropertiesDatabase()->getTagProperty(GNE_TAG_ROUTEREF, true)->getAttributeProperties(SUMO_ATTR_PROB)->getDefaultStringValue();
        myProbabilityTextField->setText(myDefaultprobability.c_str());
    }
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
    if ((myReferencesComboBox->getTextColor() == BLACK_COLOR) &&
            (myProbabilityTextField->getTextColor() == BLACK_COLOR)) {
        // declare referenced element
        GNEDemandElement* reference = nullptr;
        GNEDemandElement* referencedElement = nullptr;
        const double probability = GNEAttributeCarrier::parse<double>(myProbabilityTextField->getText().text());
        GNEDemandElement* distribution = myDistributionParent->getNet()->getAttributeCarriers()->retrieveDemandElement(myDistributionParent->getGUIGlObject());
        auto undoList = myDistributionParent->getNet()->getViewNet()->getUndoList();
        // create a routeRef o a vTypeRef
        if (distribution->getTagProperty()->getTag() == SUMO_TAG_VTYPE_DISTRIBUTION) {
            referencedElement = myDistributionParent->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myReferencesComboBox->getText().text());
            reference = new GNEVTypeRef(distribution, referencedElement, probability);
        } else if (distribution->getTagProperty()->getTag() == SUMO_TAG_ROUTE_DISTRIBUTION) {
            referencedElement = myDistributionParent->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, myReferencesComboBox->getText().text());
            reference = new GNERouteRef(distribution, referencedElement, probability);
        }
        // continue depending if allow/disallow is enabled
        if (myDistributionParent->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed()) {
            undoList->begin(referencedElement, TLF("add % in '%'", referencedElement->getTagStr(), distribution->getID()));
            undoList->add(new GNEChange_DemandElement(reference, true), true);
            undoList->end();
        } else {
            myDistributionParent->getNet()->getAttributeCarriers()->insertDemandElement(reference);
            distribution->addChildElement(reference);
            referencedElement->addChildElement(reference);
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
    bool found = false;
    for (const auto& candidateID : myCandidateIDs) {
        if (myReferencesComboBox->getText().text() == candidateID) {
            found = true;
            break;
        }
    }
    // continue depending if selected candidate exist
    if (found) {
        myReferencesComboBox->setTextColor(BLACK_COLOR);
    } else {
        myReferencesComboBox->setTextColor(RED_COLOR);
    }
    // check if enable or disable accept button
    if ((myReferencesComboBox->getTextColor() == BLACK_COLOR) &&
            (myProbabilityTextField->getTextColor() == BLACK_COLOR)) {
        myAcceptButton->enable();
    } else {
        myAcceptButton->disable();
    }
    return 1;
}


long
GNEDistributionRefDialog::onCmdSetProbability(FXObject*, FXSelector, void*) {
    // first check if set default probability
    if (myProbabilityTextField->getText().empty()) {
        myProbabilityTextField->setText(myDefaultprobability.c_str(), TRUE);
    }
    // check if value can be parsed to double
    if (GNEAttributeCarrier::canParse<double>(myProbabilityTextField->getText().text())) {
        myProbabilityTextField->setTextColor(BLACK_COLOR);
    } else {
        myProbabilityTextField->setTextColor(RED_COLOR);
    }
    // check if enable or disable accept button
    if ((myReferencesComboBox->getTextColor() == BLACK_COLOR) &&
            (myProbabilityTextField->getTextColor() == BLACK_COLOR)) {
        myAcceptButton->enable();
    } else {
        myAcceptButton->disable();
    }
    return 1;
}

/****************************************************************************/
