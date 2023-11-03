/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNEVTypeDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// VehicleType distribution used in netedit
/****************************************************************************/

#include "GNEVTypeDistribution.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVTypeDistribution::GNEVTypeDistribution(GNENet* net) :
    GNEDistribution(net, GLO_VTYPE, SUMO_TAG_VTYPE_DISTRIBUTION, GUIIcon::VTYPEDISTRIBUTION) {
}


GNEVTypeDistribution::GNEVTypeDistribution(GNENet* net, const std::string& ID, const int deterministic) :
    GNEDistribution(net, GLO_VTYPE, SUMO_TAG_VTYPE_DISTRIBUTION, GUIIcon::VTYPEDISTRIBUTION, ID, deterministic) {
}


GNEVTypeDistribution::~GNEVTypeDistribution() {}


void
GNEVTypeDistribution::writeDemandElement(OutputDevice& device) const {
    // only save if there is distribution elements to save
    if (!isDistributionEmpty()) {
        // now write attributes
        device.openTag(getTagProperty().getTag());
        device.writeAttr(SUMO_ATTR_ID, getID());
        if (myDeterministic >= 0) {
            device.writeAttr(SUMO_ATTR_DETERMINISTIC, myDeterministic);
        }
        device.writeAttr(SUMO_ATTR_VTYPES, getAttributeDistributionKeys());
        device.writeAttr(SUMO_ATTR_PROBS, getAttributeDistributionValues());
        device.closeTag();
    }
}

/****************************************************************************/
