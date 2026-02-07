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
/// @file    GNEPathElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Class used for path elements (routes, trips, flows...)
/****************************************************************************/

#include "GNEPathElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEPathElement::GNEPathElement(const GNEPathElement::Options options) :
    myOptions(options) {
}


GNEPathElement::~GNEPathElement() {}


GNEPathElement::Options
GNEPathElement::getPathElementOptions() const {
    return myOptions;
}


bool
GNEPathElement::isNetworkElement() const {
    return (myOptions & GNEPathElement::Options::NETWORK_ELEMENT) != 0;
}


bool
GNEPathElement::isAdditionalElement() const {
    return (myOptions & GNEPathElement::Options::ADDITIONAL_ELEMENT) != 0;
}


bool
GNEPathElement::isDemandElement() const {
    return (myOptions & GNEPathElement::Options::DEMAND_ELEMENT) != 0;
}


bool
GNEPathElement::isDataElement() const {
    return (myOptions & GNEPathElement::Options::DATA_ELEMENT) != 0;
}


bool
GNEPathElement::isRoute() const {
    return (myOptions & GNEPathElement::Options::ROUTE) != 0;
}

/****************************************************************************/
