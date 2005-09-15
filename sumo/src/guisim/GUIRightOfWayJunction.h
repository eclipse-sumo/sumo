#ifndef GUIRightOfWayJunction_H
#define GUIRightOfWayJunction_H
//---------------------------------------------------------------------------//
//                        GUIRightOfWayJunction.h -
//  A MSRightOfWayJunction with a graphical representation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 1 Jul 2003
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:02:54  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/08/02 11:58:14  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.3  2003/12/04 13:38:16  dkrajzew
// usage of internal links added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <microsim/MSRightOfWayJunction.h>
#include <utils/geom/Position2DVector.h>
#include <bitset>
#include <vector>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIJunctionWrapper;
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIRightOfWayJunction
 * This class extends the MSRightOfWayJunction by a graphical representation
 *  and the ability to build a wrapper that displays this representation
 */
class GUIRightOfWayJunction
    : public MSRightOfWayJunction
{
public:
    /** Use this constructor only. */
    GUIRightOfWayJunction( const std::string &id, const Position2D &position,
        LaneCont incoming, LaneCont internal, MSJunctionLogic* logic,
        const Position2DVector &myShape);

    /// Destructor.
    virtual ~GUIRightOfWayJunction();

    /// Builds the wrapper of the graphical representation of this junction
    GUIJunctionWrapper *buildJunctionWrapper(GUIGlObjectStorage &idStorage);


protected:
    /// The shape of the junction
    Position2DVector myShape;

private:
    /// Invalidated copy constructor.
    GUIRightOfWayJunction( const GUIRightOfWayJunction& );

    /// Invalidated assignment operator.
    GUIRightOfWayJunction& operator=( const GUIRightOfWayJunction& );

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
