#ifndef MSBuildCells_h
#define MSBuildCells_h
//---------------------------------------------------------------------------//
//                        MSBuildCells.h -
//  A class set each Lane to the network's cells
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jul 2006
//  copyright            : (C) 2006 by Danilo Boyom
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
// $Log: MSBuildCells.h,v
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <utils/geom/Boundary.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */

class GUINet;
class MSCell;
class GUIGlObject_AbstractAdd;
class MSEdge;
class MSLane;
class MSJunction;
class GUIEdge;
class MSEdgeControl;
class MSNet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSBuildCells
 */
class MSBuildCells {
public:
    /// Constructor
    MSBuildCells(MSNet &net, Boundary boundary);

    /// Destructor
    ~MSBuildCells();


    /// Builds the network cells
    void build();

	/// return the Cells Container
	std::vector<MSCell*>& getCellsCont(void);

private:

    /// The network the grid is assigned to
    MSNet &myNet;

	/// The networks boundary
    Boundary _boundary;

    /// Definition of a MSCell container
    typedef std::vector<MSCell*> Cont;

    /// The container saves the MSCell
    Cont _cellsCont;

    /// The Number of Cells in x- and y-Direction
    int _xsize, _ysize;

	/// The sizes of each cell
    SUMOReal _xcellsize, _ycellsize;


    /// Computes the boundary of the network
    //Boundary computeBoundary();

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

	/// algorithn to determine if two Cells are neighboring
	// bool isNeighbor(size_t i, size_t j);
	/// get all the neighbors from the cell with the index i
	std::vector<MSCell*> getNeighbors(size_t i);

	void closeBuilding();

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
