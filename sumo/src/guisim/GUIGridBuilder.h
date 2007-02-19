/****************************************************************************/
/// @file    GUIGridBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Jul 2003
/// @version $Id$
///
// A class dividing the network in rectangular cells
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIGridBuilder_h
#define GUIGridBuilder_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/geom/Boundary.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUIGrid;
class GUIGlObject_AbstractAdd;
class GUIEdge;
class GUILaneWrapper;
class GUIJunctionWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGridBuilder
 */
class GUIGridBuilder
{
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


#endif

/****************************************************************************/

