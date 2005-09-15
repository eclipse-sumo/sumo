#ifndef GUIGridBuilder_h
#define GUIGridBuilder_h
//---------------------------------------------------------------------------//
//                        GUIGridBuilder.h -
//  A class dividing the network in rectangular cells
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jul 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 07:59:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.2  2004/10/29 06:01:54  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:49:18  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.4  2004/07/02 08:41:40  dkrajzew
// detector drawer are now also responsible for other additional items
//
// Revision 1.3  2003/12/09 11:27:15  dkrajzew
// documentation added
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

#include <vector>
#include <utils/geom/Boundary.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;
class GUIGrid;
class GUIGlObject_AbstractAdd;
class GUIEdge;
class GUILaneWrapper;
class GUIJunctionWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIGridBuilder
 */
class GUIGridBuilder {
public:
    /// Constructor
    GUIGridBuilder(GUINet &net, GUIGrid &grid);

    /// Destructor
    ~GUIGridBuilder();

    /// Builds the network grid
    void build();

private:
    /// Computes the boundary of the network
    Boundary computeBoundary();

    /// Divides the artifacts onto the grid (builds the grid)
    void divideOnGrid();

    /// Computes the information which cells do contain the given edge and stores it
    void computeEdgeCells(size_t index, GUIEdge *edge);

    /// Computes the information which cells do contain the given lane and stores it
    void computeLaneCells(size_t index, GUILaneWrapper &lane);

    /// Computes the information which cells do contain the given junction and stores it
    void setJunction(size_t index, GUIJunctionWrapper *junction);

    /// Computes the information which cells do contain the given additional structures and stores it
    void setAdditional(size_t index, GUIGlObject_AbstractAdd *add);

private:
    /// The network the grid is assigned to
    GUINet &myNet;

    /// The network the grid is assigned to
    GUIGrid &myGrid;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
