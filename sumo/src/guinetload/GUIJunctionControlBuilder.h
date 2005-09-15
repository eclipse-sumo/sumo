#ifndef GUIJunctionControlBuilder_h
#define GUIJunctionControlBuilder_h
//---------------------------------------------------------------------------//
//                        GUIJunctionControlBuilder.h -
//  A MSJunctionControlBuilder that builds GUIJunctions instead of MSJunctions
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
// Revision 1.5  2005/09/15 11:06:03  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 07:56:43  dkrajzew
// level 3 warnings removed
//
// Revision 1.3  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.2  2003/12/04 13:25:52  dkrajzew
// handling of internal links added; documentation added; some dead code
//  removed
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

#include <netload/NLJunctionControlBuilder.h>
#include <utils/geom/Position2DVector.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class Position2DVector;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIJunctionControlBuilder
 * A class that build junctions with their graphical representation
 */
class GUIJunctionControlBuilder
    : public NLJunctionControlBuilder {
public:
    /// Constructor
    GUIJunctionControlBuilder(OptionsCont &oc);

    /// Destructor
    ~GUIJunctionControlBuilder();

    /// Stores temporary the information about the current junction's shape
    void addJunctionShape(const Position2DVector &shape);

protected:
    /** builds a junction that does not use a logic */
    virtual MSJunction *buildNoLogicJunction();

    /** builds a junction with a logic */
    virtual MSJunction *buildLogicJunction();

private:
    /// The shape of the current junction
    Position2DVector myShape;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
