#ifndef GUINoLogicJunction_H
#define GUINoLogicJunction_H
//---------------------------------------------------------------------------//
//                        GUINoLogicJunction.h -
//  A MSNoLogicJunction with a graphical representation
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
// Revision 1.4  2004/08/02 11:58:14  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.3  2003/12/04 13:38:16  dkrajzew
// usage of internal links added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <bitset>
#include <utils/geom/Position2DVector.h>
#include <microsim/MSNoLogicJunction.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIJunctionWrapper;
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUINoLogicJunction
 * This class extends the MSNoLogicJunction by a graphical representation and
 *  the ability to build a wrapper that displays this representation
 */
class GUINoLogicJunction
    : public MSNoLogicJunction
{
public:
    /// Destructor.
    ~GUINoLogicJunction();

    /** Use this constructor only. */
    GUINoLogicJunction( const std::string &id, const Position2D &position,
        LaneCont incoming, LaneCont internal,
        const Position2DVector &shape);

    /// Builds the wrapper of the graphical representation of this junction
    GUIJunctionWrapper *buildJunctionWrapper(GUIGlObjectStorage &idStorage);

private:
    /// The shape of the junction
    Position2DVector myShape;

private:
    /// Invalidated copy constructor.
    GUINoLogicJunction( const GUINoLogicJunction& );

    /// Invalidated assignment operator.
    GUINoLogicJunction& operator=( const GUINoLogicJunction& );

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
