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
#include <config.h>

#include "textures/POITextures/Tree.cpp"
#include "textures/POITextures/Hotel.cpp"

#include "POIImages.h"
#include "GUIIconSubSys.h"


// ===========================================================================
// static member definitions
// ===========================================================================

static StringBijection<POIImages::Images>::Entry imageStringsInitializer[] = {
    {"tree",    POIImages::Images::TREE},
    {"hotel",   POIImages::Images::HOTEL},
    {"",        POIImages::Images::NONE}
};


StringBijection<POIImages::Images> 
POIImages::imageStrings(imageStringsInitializer, POIImages::Images::NONE, false);

// ===========================================================================
// member method definitions
// ===========================================================================

FXIcon*
POIImages::getPOIIcon(const Images image) {
    switch (image) {
        case Images::TREE:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_TREE);
        case Images::HOTEL:
            return GUIIconSubSys::getIcon(GUIIcon::POIICON_HOTEL);
        case Images::NONE:
            return GUIIconSubSys::getIcon(GUIIcon::EMPTY);
        default:
            throw ProcessError("Invalid POIImage");
    }
}


const unsigned char*
POIImages::getPOITexture(const Images image) {
    switch (image) {
        case Images::TREE:
            return POITexture_Tree;
        case Images::HOTEL:
            return POITexture_Hotel;
        case Images::NONE:
            return nullptr;
        default:
            throw ProcessError("Invalid POIImage");
    }
}

/****************************************************************************/
