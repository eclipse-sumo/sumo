//---------------------------------------------------------------------------//
//                        GUIGrid.h -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2006/01/31 11:03:20  dkrajzew
// debugging the grid
//
// Revision 1.5  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:11:14  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/02/17 10:33:42  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.1  2004/11/23 10:38:32  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:01:55  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.7  2004/07/02 08:41:40  dkrajzew
// detector drawer are now also responsible for other additional items
//
// Revision 1.6  2003/12/09 11:27:15  dkrajzew
// documentation added
//
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
#include <algorithm>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GUIGrid.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUIGrid::Set - methods
 * ----------------------------------------------------------------------- */
GUIGrid::Set::Set()
{
}


GUIGrid::Set::~Set()
{
}


void
GUIGrid::Set::add(size_t no)
{
    size_t index = no >> 5;
    size_t pos = no - (index<<5);
    // check whether an entry with the same index exists
    Cont::iterator i = find_if(_cont.begin(), _cont.end(),
        index_finder(index));
    if(i==_cont.end()) {
        // if not, build a new index
        SubSet ss;
        ss.myIndex = index;
        ss.mySet = 1<<pos;
        // ... and add it
        _cont.push_back(ss);
    } else {
        // add the information to the index otherwise
        (*i).mySet |= 1<<pos;
    }
}


void
GUIGrid::Set::remove(size_t no)
{
    size_t index = no >> 5;
    size_t pos = no - (index<<5);
    // check whether an entry with the index exists
    Cont::iterator i = find_if(_cont.begin(), _cont.end(),
        index_finder(index));
    if(i!=_cont.end()) {
        // if yes, clear the information
        (*i).mySet &=~ (1<<pos);
        // check whether the whoel item can be removed
        if((*i).mySet==0) {
            _cont.erase(i);
        }
    }
}


void
GUIGrid::Set::removeIfIn(const Set &other)
{
    for(Cont::const_iterator j=other._cont.begin(); j!=other._cont.end(); j++) {
        // check whether an entry with the index exists
        Cont::iterator i = find_if(_cont.begin(), _cont.end(),
            index_finder((*j).myIndex));
        if(i!=_cont.end()) {
            // if yes, clear the information
            (*i).mySet &=~ (*j).mySet;
            // check whether the whoel item can be removed
            if((*i).mySet==0) {
                _cont.erase(i);
            }
        }
    }
}


void
GUIGrid::Set::setInto(size_t *into) const
{
    for(Cont::const_iterator j=_cont.begin(); j!=_cont.end(); j++) {
        into[(*j).myIndex] |= (*j).mySet;
    }
}



/* -------------------------------------------------------------------------
 * GUIGrid::GridCell - methods
 * ----------------------------------------------------------------------- */
GUIGrid::GridCell::GridCell()
{
}


GUIGrid::GridCell::~GridCell()
{
}


void
GUIGrid::GridCell::addJunction(size_t no)
{
    myJunctions.add(no);
}


void
GUIGrid::GridCell::addEdge(size_t no)
{
    myEdges.add(no);
}


void
GUIGrid::GridCell::addAdditional(size_t no)
{
    myAdditional.add(no);
}


void
GUIGrid::GridCell::setJunctions(size_t *into) const
{
    myJunctions.setInto(into);
}


void
GUIGrid::GridCell::setEdges(size_t *into) const
{
    myEdges.setInto(into);
}


void
GUIGrid::GridCell::setAdditional(size_t *into) const
{
    myAdditional.setInto(into);
}


void
GUIGrid::GridCell::removeIfIn(const GridCell &other)
{
    myJunctions.removeIfIn(other.myJunctions);
    myEdges.removeIfIn(other.myEdges);
    myAdditional.removeIfIn(other.myAdditional);
}


/* -------------------------------------------------------------------------
 * GUIGrid - methods
 * ----------------------------------------------------------------------- */
GUIGrid::GUIGrid(size_t noXCells, size_t noYCells)
    : myXCellSize(0), myYCellSize(0), myBoundary(), myGrid(0),
    myXSize(noXCells), myYSize(noYCells)
{
    for(size_t i=0; i<3; i++) {
        myRelations[i] = 0;
    }
	myVisHelper = new PaintState[myXSize*myYSize];
}


GUIGrid::~GUIGrid()
{
    delete[] myGrid;
    for(size_t i=0; i<3; i++) {
        delete[] myRelations[i];
    }
	delete[] myVisHelper;
}


const Boundary &
GUIGrid::getBoundary() const
{
    return myBoundary;
}


std::vector<size_t>
GUIGrid::getCellsContaining(Boundary boundary)
{
    std::vector<size_t> cells;
    // compute the cells the boundary is going through
    for(int y=0; y<myYSize; y++) {
        SUMOReal ypos1 = SUMOReal(y) * myYCellSize + myBoundary.ymin();
        for(int x=0; x<myXSize; x++) {
            SUMOReal xpos1 = SUMOReal(x) * myXCellSize + myBoundary.xmin();
            Boundary cellBounds;
            cellBounds.add(xpos1, ypos1);
            cellBounds.add(xpos1+myXCellSize, ypos1+myYCellSize);
            if( boundary.partialWithin(cellBounds) ) {
                int offset = myXSize * y + x;
                cells.push_back(offset);
            }
        }
    }
    return cells;
}


void
GUIGrid::buildRelationships()
{
    for(int y=0; y<myYSize; y++) {
        for(int x=0; x<myXSize; x++) {
            size_t pos = y*myXSize + x;
            // build cont without upper
            GridCell cont = myGrid[pos];
            removeFrom(cont, x, y-1);
            myRelations[0][pos] = cont;
            // build cont without left
            cont = myGrid[pos];
            removeFrom(cont, x-1, y);
            myRelations[1][pos] = cont;
            // build cont without left and upper
            cont = myGrid[pos];
            removeFrom(cont, x-1, y);
            removeFrom(cont, x, y-1);
            myRelations[2][pos] = cont;
        }
    }
}


void
GUIGrid::removeFrom(GridCell &cont, int x, int y)
{
    if(x<0||y<0||x>=myXSize||y>=myYSize) {
        return;
    }
    // get the list of edges to remove
    size_t pos = y*myXSize + x;
    GridCell &from = myGrid[pos];
    cont.removeIfIn(from);
}


void
GUIGrid::get(int what,
             SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax,
             size_t *setEdges, size_t *setJunctions, size_t *setAdditional) const
{
	xmax += myXCellSize;
	ymax += myYCellSize;
    // compute bounderies
	/*
    SUMOReal xmin = x - xoff - myXCellSize;
    SUMOReal xmax = x + xoff + myXCellSize;
    SUMOReal ymin = y - yoff - myYCellSize;
    SUMOReal ymax = y + yoff + myYCellSize;
	*/
	for(int i=0; i<myXSize*myYSize; i++) {
		myVisHelper[i] = GPS_NOT_DRAWN;// = new bool[myXSize*myYSize];
	}

    // loop over bounderies
    SUMOReal yrun= ymin;//(yur >= 0 ? yur : 0);
    int ypos = (int) ((ymin/*+myBoundary.ymin()*/)/myYCellSize);
    if(ypos<0) {
        ypos = 0;
    }
    size_t yidx = 0;
    for(; yrun<ymax&&ypos<myYSize; yrun+=myYCellSize, ypos++, yidx++) {
        SUMOReal xrun=xmin;//(xur >= 0 ? xur : 0);
        int xpos = (int) ((xmin/*+myBoundary.xmin()*/)/myXCellSize);
        if(xpos<0) {
            xpos = 0;
        }
        size_t xidx = 0;
        for(; xrun<xmax&&xpos<myXSize; xrun+=myXCellSize, xpos++, xidx++) {
            int offs = ypos*myXSize+xpos;
            if(xidx==0&&yidx==0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    myGrid[offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    myGrid[offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    myGrid[offs].setAdditional(setAdditional);
                }
				myVisHelper[offs] = GPS_FULL_DRAWN;
            } else if(yidx!=0 && xidx!=0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    myRelations[2][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    myRelations[2][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    myRelations[2][offs].setAdditional(setAdditional);
                }
				myVisHelper[offs] = GPS_ADD_DRAWN;
            } else if(yidx==0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    myRelations[1][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    myRelations[1][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    myRelations[1][offs].setAdditional(setAdditional);
                }
				myVisHelper[offs] = GPS_ADD_DRAWN;
            } else {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    myRelations[0][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    myRelations[0][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    myRelations[0][offs].setAdditional(setAdditional);
                }
				myVisHelper[offs] = GPS_ADD_DRAWN;
            }
        }
    }
}


int
GUIGrid::getNoXCells() const
{
    return myXSize;
}


int
GUIGrid::getNoYCells() const
{
    return myYSize;
}


SUMOReal
GUIGrid::getXCellSize() const
{
    return myXCellSize;
}


SUMOReal
GUIGrid::getYCellSize() const
{
    return myYCellSize;
}


void
GUIGrid::closeBuilding()
{
    // ok, we now have a grid, but we don not want to draw edges more
    //  than once; build the relationship matrix next
	size_t size = getNoXCells() * getNoYCells();
    for(size_t i=0; i<3; i++) {
		myRelations[i] = new GUIGrid::GridCell[size];
    }
    buildRelationships();
}


void
GUIGrid::setBoundary(const Boundary &b)
{
	myBoundary = b;
}


GUIGrid::PaintState
GUIGrid::getPaintState(size_t x, size_t y)
{
	return myVisHelper[myXSize*y+x];
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




