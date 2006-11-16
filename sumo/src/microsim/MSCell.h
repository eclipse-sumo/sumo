#ifndef MSCell_h
#define MSCell_h
//---------------------------------------------------------------------------//
//                        MSCell.h -
//  A class represents the cells
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
// Revision 1.3  2006/11/16 10:50:44  dkrajzew
// warnings removed
//
// Revision 1.2  2006/11/16 07:02:17  dkrajzew
// warnings removed
//
// Revision 1.1  2006/09/18 09:58:02  dkrajzew
// removed deprecated c2c functions, added new made by Danilot Boyom
//
// Revision 1.6  2005/10/06 13:39:50  dksumo
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

class GUIEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSCell
 * This class represents the Cell, in which the Lane are saved.
 * Each cells have a amount of Lane in his container.
 */
class MSCell {
public:
    /// Constructor
    MSCell(SUMOReal xcellsize, SUMOReal ycellsize);

    /// Destructor
    ~MSCell();

	void setIndex(size_t index);

	 /// Adds an edge into the Container
    void addEdge(MSEdge *edge);

	/// Removes an edge from the Container
    void removeEdge(MSEdge *edge);

	/// set my Neighbors Cells
	//void setCellNeighbors(size_t pos, MSCell *cell);
    void setCellNeighbors(std::vector<MSCell*> ret);

	/// get the NeighborsCells Container
    std::vector<MSCell*>& getCellNeighbors(void);

	/// get the Edge Container of this Cell
    std::vector<MSEdge*>& getEdges(void);

	/// set my Edges Neighbors
	void setEdgesNeighbors(void);

	// return the Index
	size_t getIndex(void);


private:
	/// The Index of this Cell in the Grid
	size_t myIndex;

    /// The sizes of the cells
    SUMOReal _xcellsize, _ycellsize;

    /// Definition of a Edges container
    typedef std::vector<MSEdge*> Cont;

	/// Definition of a Cells container
	typedef std::vector<MSCell*> CellCont;

    /// The container holding the Egdes
    Cont _edgesCont;

	/// all the Neighbors of the Cell
	CellCont _neighbors;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
