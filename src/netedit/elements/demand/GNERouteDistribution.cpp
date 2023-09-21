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
/// @file    GNERouteDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// Route distribution used in netedit
/****************************************************************************/

#include "GNERouteDistribution.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteDistribution::GNERouteDistribution(GNENet* net) :
    GNEDistribution(net, GLO_ROUTE, SUMO_TAG_ROUTE_DISTRIBUTION, GUIIcon::ROUTEDISTRIBUTION) {
}


GNERouteDistribution::GNERouteDistribution(GNENet* net, const std::string& ID) :
    GNEDistribution(net, GLO_ROUTE, SUMO_TAG_ROUTE_DISTRIBUTION, GUIIcon::ROUTEDISTRIBUTION, ID, -1) {
}


GNERouteDistribution::~GNERouteDistribution() {}


void
GNERouteDistribution::writeDemandElement(OutputDevice& device) const {
    // only save if there is distribution elements to save
    if (!isDistributionEmpty()) {
        // now write attributes
        device.openTag(getTagProperty().getTag());
        device.writeAttr(SUMO_ATTR_ID, getID());
        device.writeAttr(SUMO_ATTR_ROUTES, getAttributeDistributionKeys());
        device.writeAttr(SUMO_ATTR_PROBS, getAttributeDistributionValues());
        device.closeTag();
    }
}

/****************************************************************************/
