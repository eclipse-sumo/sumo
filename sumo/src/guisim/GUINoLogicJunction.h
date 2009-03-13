/****************************************************************************/
/// @file    GUINoLogicJunction.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// A MSNoLogicJunction with a graphical representation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUINoLogicJunction_h
#define GUINoLogicJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <bitset>
#include <utils/geom/Position2DVector.h>
#include <microsim/MSNoLogicJunction.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIJunctionWrapper;
class GUIGlObjectStorage;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUINoLogicJunction
 * This class extends the MSNoLogicJunction by a graphical representation and
 *  the ability to build a wrapper that displays this representation
 */
class GUINoLogicJunction
            : public MSNoLogicJunction {
public:
    /// Destructor.
    ~GUINoLogicJunction();

    /** Use this constructor only. */
    GUINoLogicJunction(const std::string &id, const Position2D &position,
                       LaneCont incoming,
#ifdef HAVE_INTERNAL_LANES
                       LaneCont internal,
#endif
                       const Position2DVector &shape);

    /// Builds the wrapper of the graphical representation of this junction
    GUIJunctionWrapper *buildJunctionWrapper(GUIGlObjectStorage &idStorage);

private:
    /// The shape of the junction
    Position2DVector myShape;

private:
    /// Invalidated copy constructor.
    GUINoLogicJunction(const GUINoLogicJunction&);

    /// Invalidated assignment operator.
    GUINoLogicJunction& operator=(const GUINoLogicJunction&);

};


#endif

/****************************************************************************/

