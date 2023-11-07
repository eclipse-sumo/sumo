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
/// @file    POIIcons.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// Function that returns the icon for a certain POIIcon
/****************************************************************************/

#include "GUIIconSubSys.h"

#include "POIIcons.h"

// ===========================================================================
// member method definitions
// ===========================================================================

FXIcon*
POIIcons::getPOIIcon(POIIcon iconType) {
    switch (iconType) {
        case POIIcon::PIN:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_PIN);
        case POIIcon::NATURE:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_NATURE);
        case POIIcon::HOTEL:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_HOTEL);
        case POIIcon::FUEL:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_FUEL);
        case POIIcon::CHARGING_STATION:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_CHARGING_STATION);
        case POIIcon::NONE:
            return GUIIconSubSys::getIcon(GUIIcon::EMPTY);
        default:
            throw ProcessError("Invalid POIImage");
    }
}

/****************************************************************************/
