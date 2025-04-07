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
/// @file    GNERouteDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2022
///
// Route distribution used in netedit
/****************************************************************************/

#include <netedit/GNETagProperties.h>

#include "GNERouteDistribution.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteDistribution::GNERouteDistribution(GNENet* net) :
    GNEDistribution(net, GLO_ROUTE, SUMO_TAG_ROUTE_DISTRIBUTION) {
}


GNERouteDistribution::GNERouteDistribution(const std::string& ID, GNENet* net, const std::string& filename) :
    GNEDistribution(ID, net, filename, GLO_ROUTE, SUMO_TAG_ROUTE_DISTRIBUTION, -1) {
}


GNERouteDistribution::~GNERouteDistribution() {}


void
GNERouteDistribution::writeDemandElement(OutputDevice& device) const {
    // write attributes
    device.openTag(getTagProperty()->getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    // check if write route or refs)
    for (const auto& refChild : getChildDemandElements()) {
        int numReferences = 0;
        for (const auto& routeChild : refChild->getParentDemandElements().at(1)->getChildDemandElements()) {
            if (routeChild->getTagProperty()->getTag() == GNE_TAG_ROUTEREF) {
                numReferences++;
            }
        }
        if (numReferences == 1) {
            refChild->getParentDemandElements().at(1)->writeDemandElement(device);
        } else {
            refChild->writeDemandElement(device);
        }
    }
    device.closeTag();
}

/****************************************************************************/
