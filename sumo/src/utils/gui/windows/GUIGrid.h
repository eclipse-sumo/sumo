/****************************************************************************/
/// @file    GUIGrid.h
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
#ifndef GUIGrid_h
#define GUIGrid_h


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
// class definitions
// ===========================================================================
/**
 * @class GUIGrid
 * This class lays a grid over a network in order to allow the drawing of
 *  only a part of the network: the viewport of a view on the simulation is
 *  used to determine the streets, junctions, vehicles etc. actually
 *  visible (not outside the viewport). The information about which streets
 *  are visible are copied into a container which should be supplied by the
 *  view that wants to know which parts shall be visualised.
 * It is obvious, that the view should call this method only if the viewport
 *  has been changed.
 * The grid itself is made of cells. Each cell holds a small part of the area
 *  and the number of cells to use horizontally and vertically may be specified
 *  (although no tests have been done and the value 10 is used for both
 *  directions). Each cell contains a set of items which do lie within the cell
 *  and each item is represented as a bit. This allows to store the values in
 *  a memory saving way and allow a fast access, though.
 */
class GUIGrid
{
public:
    enum PaintState {
        GPS_NOT_DRAWN,
        GPS_FULL_DRAWN,
        GPS_ADD_DRAWN
    };


    /// Constructor
    GUIGrid(int noXCells, int noYCells);

    /// Destructor
    ~GUIGrid();

    /// Builds the network grid
    //void init();

    /// Returns the network's boundary
    const Boundary &getBoundary() const;

    ///
    void setBoundary(const Boundary &b);

    /** @brief Fills the given containers with the wished information
        The offsets and x/y-positions are used to determine the viewport.
        The first int supplied ("what") determines the information about which
        artifact shall be computed. They are filled into the given arrays.
        The arrays must have the proper size (equals to the number of the
        simulation's objects of each type) */
    void get(int what,
             SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax,
             size_t *setEdges, size_t *setJunctions, size_t *setAdditional) const;

    /// returns the number of cells in x-direction
    int getNoXCells() const;

    /// returns the number of cells in y-direction
    int getNoYCells() const;

    /// returns the size of each cell in x-direction
    SUMOReal getXCellSize() const;

    /// returns the size of each cell in y-direction
    SUMOReal getYCellSize() const;

    PaintState getPaintState(size_t x, size_t y);

    friend class GUIGridBuilder;
    friend class GUIRouterGridBuilder;
    friend class VGridBuilder;

private:
    /**
     * @struct SubSet
     * This item holds information whether at least one artifact that belong
     *  to the set described by the mySet-value belongs to a Set. The
     *  mySet-value is an index for the global table that stores all instances
     *  of the current artifact divided by 32. This division and the usage of
     *  the myIndex-value allows the computation of the artifact's real index
     *  as it is: mySet * 32 + myIndex;
     */
    struct SubSet {
        /// The index of the artifact within this sub set
        size_t   myIndex;

        /// The sub set number
        size_t   mySet;

    };

    /**
     * @class Set
     * This class stores the list of a certain artifact instances that lie
     *  within a certain cell of the network via storing their indices.
     * The indices are not stored directly but within subsets. If more than a
     *  single artifact with the same mySet-value exist, they are stored within
     *  the same subset.
     */
    class Set
    {
    public:
        /// Constructor
        Set();

        /// Destructor
        ~Set();

        /// Adds an index of an artifact to the set
        void add(size_t no);

        /// Removes an index of an artifact from the set
        void remove(size_t no);

        /// Removes all indices the given set contains
        void removeIfIn(const Set &other);

        /// Sets the information about the stored artifact indices to the given array
        void setInto(size_t *into) const;

//        friend class GUIGrid; // !!!
    private:
        /// Definition of a subset container
        typedef std::vector<SubSet> Cont;

        /// The container holding the subsets
        Cont myCont;

    };

    /**
     * @class GridCell
     * A single cell within the grid. Holds the information about junctions,
     *  edges, detectors and emitters using a set for each of these.
     */
    class GridCell
    {
    public:
        /// Constructor
        GridCell();

        /// Destructor
        ~GridCell();

        /// Adds a junction to the set
        void addJunction(size_t no);

        /// Adds an edge to the set
        void addEdge(size_t no);

        /// Adds an additional structure to the set
        void addAdditional(size_t no);

        /// Sets the information which junctions are stored lie within the cell
        void setJunctions(size_t *into) const;

        /// Sets the information which edges are stored lie within the cell
        void setEdges(size_t *into) const;

        /// Sets the information which additional structures are stored lie within the cell
        void setAdditional(size_t *into) const;

        /// Removes all artifacts from which are stored within the given cell
        void removeIfIn(const GridCell &other);

    public:

        /// The set of junctions
        Set myJunctions;

        /// The set of edges
        Set myEdges;

        /// The set of additional structures
        Set myAdditional;

    };

    /**
     * @class index_finder
     * A class which searches for a certain set which has the given index
     */
    class index_finder
    {
    public:
        /// constructor
        explicit index_finder(size_t index)
                : myIndex(index) { }

        /// operator
        bool operator()(const SubSet &s) const {
            return s.myIndex==myIndex;
        }
    private:
        /// The index to search for
        size_t myIndex;

    };

private:

    /// Computes the boundary of the network
    //Boundary computeBoundary();

    /// Returns the list of cells containing an object with the given boundary
    std::vector<size_t> getCellsContaining(Boundary boundary);

    /// Builds the addition relationships
    void buildRelationships();

    /// Removes items from the cell at x/y which are included in the given cell
    void removeFrom(GridCell &cont, int x, int y);

    void closeBuilding();

private:
    /// The sizes of the cells
    SUMOReal myXCellSize, myYCellSize;

    /// The networks boundary
    Boundary myBoundary;

    /// The build grid
    GridCell *myGrid;

    /// The build relations
    GridCell *myRelations[3];

    /// The size of the grid in cells in x- and y-direction
    int myXSize, myYSize;

    PaintState *myVisHelper;

};


#endif

/****************************************************************************/

