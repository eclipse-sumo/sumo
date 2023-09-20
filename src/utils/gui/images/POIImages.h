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
/// @file    POIImages.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// Class for grouping POI images and textures
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/shapes/PointOfInterest.h>
#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class POIImages
 * @brief Utility function for getting the images associated to a POI.
 */
class POIImages {

public:
    /// @brief returns icon associated to the given POI image
    static FXIcon* getPOIIcon(PointOfInterest::Icon POIIcon);

    /// @brief returns texture associated to the given POI image
    static const unsigned char* getPOITexture(PointOfInterest::Icon POIIcon);
};
