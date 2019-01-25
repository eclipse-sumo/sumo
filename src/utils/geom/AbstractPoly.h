/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AbstractPoly.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The base class for polygons
/****************************************************************************/
#ifndef AbstractPoly_h
#define AbstractPoly_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "Position.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class AbstractPoly {
public:
    /// @brief constructor
    AbstractPoly() { }

    /// @brief copy constructor
    AbstractPoly(const AbstractPoly&) { }

    /// @brief destructor
    virtual ~AbstractPoly() { }

    /// @brief Returns whether the AbstractPoly the given coordinate
    virtual bool around(const Position& p, double offset = 0) const = 0;

    /// @brief Returns whether the AbstractPoly overlaps with the given polygon
    virtual bool overlapsWith(const AbstractPoly& poly, double offset = 0) const = 0;

    /// @brief Returns whether the AbstractPoly is partially within the given polygon
    virtual bool partialWithin(const AbstractPoly& poly, double offset = 0) const = 0;

    /// @brief Returns whether the AbstractPoly crosses the given line
    virtual bool crosses(const Position& p1, const Position& p2) const = 0;
};


#endif

/****************************************************************************/

