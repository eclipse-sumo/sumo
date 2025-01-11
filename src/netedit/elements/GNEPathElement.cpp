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

GNEPathElement::GNEPathElement(GUIGlObjectType type, const std::string& microsimID, FXIcon* icon, const int options) :
    GUIGlObject(type, microsimID, icon),
    myOption(options) {
}


GNEPathElement::~GNEPathElement() {}


bool
GNEPathElement::isNetworkElement() const {
    return (myOption & GNEPathElement::Options::NETWORK_ELEMENT) != 0;
}


bool
GNEPathElement::isAdditionalElement() const {
    return (myOption & GNEPathElement::Options::ADDITIONAL_ELEMENT) != 0;
}


bool
GNEPathElement::isDemandElement() const {
    return (myOption & GNEPathElement::Options::DEMAND_ELEMENT) != 0;
}


bool
GNEPathElement::isDataElement() const {
    return (myOption & GNEPathElement::Options::DATA_ELEMENT) != 0;
}


bool
GNEPathElement::isRoute() const {
    return (myOption & GNEPathElement::Options::ROUTE) != 0;
}

/****************************************************************************/
