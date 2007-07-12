/****************************************************************************/
/// @file    MSCell.h
/// @author  Danilot Teta Boyom
/// @date    Jul 2003
/// @version $Id$
///
// A class represents the cells
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
#ifndef MSCell_h
#define MSCell_h


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
class MSEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCell
 * This class represents the Cell, in which the Lane are saved.
 * Each cells have a amount of Lane in his container.
 */
class MSCell
{
public:
    /// Constructor
    MSCell(SUMOReal xcellsize, SUMOReal ycellsize);

    /// Destructor
    ~MSCell();

    void setIndex(size_t index);

    /// Adds an edge into the Container
    void addEdge(MSEdge *edge);

    /// set my Neighbors Cells
    //void setCellNeighbors(size_t pos, MSCell *cell);
    void setCellNeighbors(std::vector<MSCell*> ret);

    /// set my Edges Neighbors
    void setEdgesNeighbors(void);


private:
    /// The Index of this Cell in the Grid
    size_t myIndex;

    /// The sizes of the cells
    SUMOReal myXcellsize, myYcellsize;

    /// Definition of a Edges container
    typedef std::vector<MSEdge*> Cont;

    /// Definition of a Cells container
    typedef std::vector<MSCell*> CellCont;

    /// The container holding the Egdes
    Cont myEdgesCont;

    /// all the Neighbors of the Cell
    CellCont myNeighbors;

};


#endif

/****************************************************************************/

