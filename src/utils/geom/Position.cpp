/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Position.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A position in the 2D- or 3D-world
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <limits>
#include "Position.h"


// ===========================================================================
// static member definitions
// ===========================================================================

// Position 1Mio km below the surface should suffice for signaling invalidity inside the solar system
const Position Position::INVALID(
    - 1024 * 1024 * 1024,
    - 1024 * 1024 * 1024,
    - 1024 * 1024 * 1024);


Position
Position::rotateAround2D(double rad, const Position& origin) {
    const double s = sin(rad);
    const double c = cos(rad);
    Position p = (*this) - origin;
    return Position(
               p.x() * c - p.y() * s,
               p.x() * s + p.y() * c) + origin;

}
