/****************************************************************************/
/// @file    MSCells.h
/// @author  Danilot Teta Boyom, Michael Behrisch
/// @date    Jul 2006
/// @version $Id$
///
// A class set each Lane to the network's cells
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCells_h
#define MSCells_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSDevice_C2C;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCells
 */
class MSCells {
public:
    /// Constructor
    MSCells(SUMOReal cellSize);

    /// Destructor
    ~MSCells();

    /// Update the network cell contents
    void update();

    typedef std::vector<MSDevice_C2C*> Cell;

    typedef std::vector<Cell*>::const_iterator CellsIterator;

    /// Get iterator over the cells
    CellsIterator begin();

    CellsIterator end();

    /// Add a device
    void add(MSDevice_C2C* device);

    /// Remove a device
    void remove(MSDevice_C2C* device);

    /// Get the neighbor cell in the specified direction
    Cell* getNeighbor(std::vector<Cell*>::const_iterator center, int xOffset, int yOffset);

private:

    /// Get the (target) cell for a device
    Cell* getCellByPosition(MSDevice_C2C* device);

    /// The container saves the cells
    std::vector<Cell*> myCellsCont;

    /// The container saving Devices for which a cell could not be determined
    Cell myUndetermined;

    /// The number of Cells in x- and y-direction
    unsigned myXsize, myYsize;

    /// The size of each cell
    SUMOReal myXcellsize, myYcellsize;

};


#endif

/****************************************************************************/

