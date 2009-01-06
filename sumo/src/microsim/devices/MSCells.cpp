/****************************************************************************/
/// @file    MSCells.cpp
/// @author  Danilot Teta Boyom, Michael Behrisch
/// @date    Jul 2006
/// @version $Id$
///
// Storing vehicles in cells to speed up distance checking
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/geom/GeoConvHelper.h>
#include "MSDevice_C2C.h"
#include "MSCells.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

// ===========================================================================
// member method definitions
// ===========================================================================
MSCells::MSCells(SUMOReal cellSize)
{
    myXcellsize = cellSize;
    myYcellsize = cellSize;
    Boundary boundary = GeoConvHelper::getConvBoundary();
    myXsize = (unsigned)((boundary.xmax()-boundary.xmin())/myXcellsize) + 1;
    myYsize = (unsigned)((boundary.ymax()-boundary.ymin())/myXcellsize) + 1;
    for (unsigned i=0; i<myXsize*myYsize; ++i) {
        myCellsCont.push_back(new Cell());
    }
}

MSCells::~MSCells()
{
    for (unsigned i=0; i<myXsize*myYsize; ++i) {
        delete myCellsCont[i];
    }
}

MSCells::CellsIterator MSCells::begin()
{
    return myCellsCont.begin();
}

MSCells::CellsIterator MSCells::end()
{
    return myCellsCont.end();
}

void MSCells::update()
{
    for (vector<Cell*>::const_iterator cell = myCellsCont.begin();
            cell != myCellsCont.end(); ++cell) {
        for (MSCells::Cell::iterator device = (*cell)->begin();
                device != (*cell)->end();) {
            MSCells::Cell* target = getCellByPosition(*device);
            if (target != *cell) {
                device = (*cell)->erase(device);
            } else {
                add(*device);
                ++device;
            }
        }
    }
}

void MSCells::add(MSDevice_C2C* device)
{
    MSCells::Cell* cell = getCellByPosition(device);
    if (cell == 0) {
        myUndetermined.push_back(device);
    } else {
        cell->push_back(device);
    }
}

void MSCells::remove(MSDevice_C2C* device)
{
    MSCells::Cell* cell = getCellByPosition(device);
    if (cell == 0) {
        myUndetermined.erase(find(myUndetermined.begin(), myUndetermined.end(),
                                  device));
    } else {
        Cell::iterator contain = find(cell->begin(), cell->end(), device);
        if (contain != cell->end()) {
            cell->erase(contain);
        }
    }
}

MSCells::Cell* MSCells::getNeighbor(std::vector<Cell*>::const_iterator center,
                                    int xOffset, int yOffset)
{
    int offset = yOffset*myXsize + xOffset;
    if (offset < 0 && distance(begin(), center) < -offset) {
        return 0;
    }
    if (distance(center, end()) < offset) {
        return 0;
    }
    return *(center + offset);
}

MSCells::Cell* MSCells::getCellByPosition(MSDevice_C2C* device)
{
    const Position2D &pos = device->getHolder().getPosition();
    Boundary boundary = GeoConvHelper::getConvBoundary();
    int xIndex = (int)((pos.x() - boundary.xmin()) / myXcellsize);
    int yIndex = (int)((pos.y() - boundary.ymin()) / myYcellsize);
    if ((xIndex < 0) || (xIndex >= myXsize) ||
            (yIndex < 0) || (yIndex >= myYsize)) {
        return 0;
    }
    return myCellsCont[yIndex*myXsize+xIndex];
}

/****************************************************************************/
