/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
