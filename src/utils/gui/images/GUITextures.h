/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GUITextures.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
///
// An enumeration of gifs used by the gui applications
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum GUITexture
 * @brief An enumeration of gifs used by the gui applications
 */
enum class GUITexture {
    E3 = 0,
    E3_SELECTED,
    EMPTY,
    EMPTY_SELECTED,
    LOCK,
    LOCK_SELECTED,
    NOTMOVING,
    NOTMOVING_SELECTED,
    REROUTER,
    REROUTER_SELECTED,
    ROUTEPROBE,
    ROUTEPROBE_SELECTED,
    TLS,
    VAPORIZER,
    VAPORIZER_SELECTED,
    VARIABLESPEEDSIGN,
    VARIABLESPEEDSIGN_SELECTED,
    LANE_BIKE,
    LANE_BUS,
    LANE_PEDESTRIAN,
    STOP,
    STOP_SELECTED,
    PERSONSTOP,
    PERSONSTOP_SELECTED,
    CONTAINERSTOP,
    CONTAINERSTOP_SELECTED,
    TEXTURE_MAX
};
