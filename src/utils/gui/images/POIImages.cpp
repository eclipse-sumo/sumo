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
/// @file    POIImages.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// Class for grouping POI images and textures
/****************************************************************************/

#include "GUIIconSubSys.h"

#include "POIImages.h"

// ===========================================================================
// member method definitions
// ===========================================================================

FXIcon*
POIImages::getPOIIcon(POIIcon POIIcon) {
    switch (POIIcon) {
        case POIIcon::TREE:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_TREE);
        case POIIcon::HOTEL:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_HOTEL);
        case POIIcon::NONE:
            return GUIIconSubSys::getIcon(GUIIcon::EMPTY);
        default:
            throw ProcessError("Invalid POIImage");
    }
}

/****************************************************************************/
