/****************************************************************************/
/// @file    MSBuildCells.h
/// @author  Danilot Teta Boyom
/// @date    Jul 2006
/// @version $Id$
///
// A class set each Lane to the network's cells
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
#ifndef MSBuildCells_h
#define MSBuildCells_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
class MSCell;
class GUIGlObject_AbstractAdd;
class MSEdge;
class MSLane;
class MSJunction;
class GUIEdge;
class MSEdgeControl;
class MSNet;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBuildCells
 */
class MSBuildCells
{
public:
    /// Constructor
    MSBuildCells(MSNet &net, Boundary boundary);

    /// Destructor
    ~MSBuildCells();


    /// Builds the network cells
    void build();

    /// writes information about edge neighbors
    void writeNearEdges(OutputDevice &od);

private:

    /// The network the grid is assigned to
    MSNet &myNet;

    /// The networks boundary
    Boundary myBoundary;

    /// Definition of a MSCell container
    typedef std::vector<MSCell*> Cont;

    /// The container saves the MSCell
    Cont myCellsCont;

    /// The Number of Cells in x- and y-Direction
    size_t myXsize, myYsize;

    /// The sizes of each cell
    SUMOReal myXcellsize, myYcellsize;

    /// creates size's amount of new Cells
    void createCells(size_t size);

    /// Divides the artifacts onto the grid (builds the grid)
    void divideOnGrid();

    /// Computes the information which cells do contain the given edge and stores it
    void computeEdgeCells(size_t index, MSEdge *edge);

    /// Computes the information which cells do contain the given lane and stores it
    void computeLaneCells(size_t index, const Position2DVector &lane, MSEdge *edge);

    /// set the Neighbours of all cells
    void setCellsNeighbors(void);

    /// get all the neighbors from the cell with the index i
    std::vector<MSCell*> getNeighbors(size_t i);

};


#endif

/****************************************************************************/

