#ifndef GUIEdgeGrid_h
#define GUIEdgeGrid_h
//---------------------------------------------------------------------------//
//                        GUIEdgeGrid.h -
//  A grid of edges for faster drawing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.3  2003/03/06 17:17:32  dkrajzew
// y-direction flipped
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utility>
#include <utils/geom/Boundery.h>
#include <vector>
#include <set>
//#include <utils/geom/Bresenham.h>
#include "GUIEdgeCont.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This not fully implemented class is meant to be used as a container of edges
 * to allow a faster access on painting, when only a part of the network is
 * shown.
 * In this case, only the edges which are stored in this grid's cells that are
 * visible shall be redrawn.
 */
class GUIEdgeGrid
{
public:
    /// constructor
    GUIEdgeGrid(size_t xsize, size_t ysize);

    /// destructor
    ~GUIEdgeGrid();

    /// the definition of a set of edges to draw
    typedef std::set<GUIEdge*> GUIEdgeSet;

    /// initialises the edge grid
    void init();

    /** returns the edges lying within the cell that has the specified
        cooridnates
        !!! this is not enough; a boundery should be given */
    void get(GUIEdgeSet &into,
        double x, double y, double xoff, double yoff);

    /// returns the boundery of the net
    const Boundery &getBoundery() const;

    /// returns the x-dimension of the grid
    size_t getNoXCells() const;

    /// returns the y-dimension of the grid
    size_t getNoYCells() const;

    /// returns the x-size of a grid cell
    const double &getXCellSize() const;

    /// returns the y-size of a grid cell
    const double &getYCellSize() const;

private:
    /// definition of a cell position
    typedef std::pair<int, int> CellPos;

    /// definition of a list of cell positions
    typedef std::vector<CellPos> CellPosVector;

private:
    /// computes the boundery of the net
    Boundery computeBoundery();

    /// divides the edges onto the grid
    void divideOnGrid(double xcellsize, double ycellsize);

    /// computes in which cells an edge lyes
    void computeEdgeCells(double xcellsize, double ycellsize,
                          GUIEdge *edge);


        /*
    class EdgeAssigner : public Bresenham::BresenhamCallBack {
	private:
		double _xcellsize, _ycellsize;
		GUIEdgeCont *_grid;
		GUIEdge *_edge;
		double _xmin, _ymin;
		size_t _xsize;
	public:
		EdgeAssigner(double xcellsize, double ycellsize,
            GUIEdgeCont *grid, GUIEdge *edge, size_t xsize);
		~EdgeAssigner();
		void execute(double x, double y);
	};*/

    /// Builds the relationship-grid
    void buildRelationships();

    /// removes items being in _grid at position(x, y) from the given container
    void removeFrom(GUIEdgeCont &cont, int x, int y);

private:
    /// the number of cells in x- and y-directions
    size_t _xsize, _ysize;

    /// the size of the cells in m
    double _xcellsize, _ycellsize;

    /// the grid of edge lists (two dimensional)
    GUIEdgeCont *_grid;

    /// counted: u, l, ul
    GUIEdgeCont **_relations;


    /// the default empty list of edges
    GUIEdgeSet _empty;

    /// the net's boundery
    Boundery _boundery;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIEdgeGrid.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

