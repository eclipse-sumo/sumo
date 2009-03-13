/****************************************************************************/
/// @file    GUIRightOfWayJunction.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// A MSRightOfWayJunction with a graphical representation
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
#ifndef GUIRightOfWayJunction_h
#define GUIRightOfWayJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSRightOfWayJunction.h>
#include <utils/geom/Position2DVector.h>
#include <bitset>
#include <vector>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIJunctionWrapper;
class GUIGlObjectStorage;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIRightOfWayJunction
 * This class extends the MSRightOfWayJunction by a graphical representation
 *  and the ability to build a wrapper that displays this representation
 */
class GUIRightOfWayJunction
            : public MSRightOfWayJunction {
public:
    /** Use this constructor only. */
    GUIRightOfWayJunction(const std::string &id, const Position2D &position,
                          LaneCont incoming,
#ifdef HAVE_INTERNAL_LANES
                          LaneCont internal,
#endif
                          MSJunctionLogic* logic, const Position2DVector &myShape);

    /// Destructor.
    virtual ~GUIRightOfWayJunction();

    /// Builds the wrapper of the graphical representation of this junction
    GUIJunctionWrapper *buildJunctionWrapper(GUIGlObjectStorage &idStorage);


protected:
    /// The shape of the junction
    Position2DVector myShape;

private:
    /// Invalidated copy constructor.
    GUIRightOfWayJunction(const GUIRightOfWayJunction&);

    /// Invalidated assignment operator.
    GUIRightOfWayJunction& operator=(const GUIRightOfWayJunction&);

};


#endif

/****************************************************************************/

