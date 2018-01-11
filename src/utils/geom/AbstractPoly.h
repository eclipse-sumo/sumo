/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Position.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class AbstractPoly {
public:
    AbstractPoly() { }
    AbstractPoly(const AbstractPoly&) { }
    virtual ~AbstractPoly() { }
    virtual bool around(const Position& p, double offset = 0) const = 0;
    virtual bool overlapsWith(const AbstractPoly& poly, double offset = 0) const = 0;
    virtual bool partialWithin(const AbstractPoly& poly, double offset = 0) const = 0;
    virtual bool crosses(const Position& p1,
                         const Position& p2) const = 0;
};


#endif

/****************************************************************************/

