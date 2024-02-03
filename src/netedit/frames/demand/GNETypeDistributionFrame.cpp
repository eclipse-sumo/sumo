/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNETypeDistributionFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit type distribution elements
/****************************************************************************/


#include "GNETypeDistributionFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNETypeDistributionFrame::GNETypeDistributionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Type Distributions")) {

    /// @brief type editor
    myDistributionEditor = new GNEDistributionFrame::DistributionEditor(this, SUMO_TAG_VTYPE_DISTRIBUTION, GUIIcon::VTYPEDISTRIBUTION);

    /// @brief type distribution selector
    myDistributionSelector = new GNEDistributionFrame::DistributionSelector(this);

    /// @brief distribution attributes editor
    myAttributesEditor = new GNEFrameAttributeModules::AttributesEditor(this);

    // Create type distribution attributes editor
    myDistributionValuesEditor = new GNEDistributionFrame::DistributionValuesEditor(this, myDistributionEditor, myDistributionSelector, myAttributesEditor, SUMO_TAG_VTYPE);
}


GNETypeDistributionFrame::~GNETypeDistributionFrame() {}


void
GNETypeDistributionFrame::show() {
    // refresh type selector
    myDistributionSelector->refreshDistributionSelector();
    // show frame
    GNEFrame::show();
}


GNEDistributionFrame::DistributionSelector*
GNETypeDistributionFrame::getDistributionSelector() const {
    return myDistributionSelector;
}


void
GNETypeDistributionFrame::attributeUpdated(SumoXMLAttr attribute) {
    if (attribute == SUMO_ATTR_ID) {
        // refresh distribution selector IDs
        myDistributionSelector->refreshDistributionIDs();
    }
}

/****************************************************************************/
