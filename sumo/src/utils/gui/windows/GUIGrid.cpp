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
#include <vector>
#include <algorithm>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GUIGrid.h"


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
        (*i).mySet &= (unsigned(4294967295) - (1<<pos));
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
            (*i).mySet &= (unsigned(4294967295) - (*j).mySet);
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
GUIGrid::GUIGrid(int noXCells, int noYCells)
    : _xcellsize(0), _ycellsize(0), _boundary(), _grid(0),
    _xsize(noXCells), _ysize(noYCells)
{
    for(size_t i=0; i<3; i++) {
        _relations[i] = 0;
    }
}


GUIGrid::~GUIGrid()
{
    delete[] _grid;
    for(size_t i=0; i<3; i++) {
        delete[] _relations[i];
    }
}


const Boundary &
GUIGrid::getBoundary() const
{
    return _boundary;
}


std::vector<size_t>
GUIGrid::getCellsContaining(Boundary boundary)
{
    std::vector<size_t> cells;
    // compute the cells the lae is going through
    for(int y=0; y<_ysize; y++) {
        double ypos1 = double(y) * _ycellsize;
        for(int x=0; x<_xsize; x++) {
            double xpos1 = double(x) * _xcellsize;
            Boundary cellBounds;
            cellBounds.add(xpos1, ypos1);
            cellBounds.add(xpos1+_xcellsize, ypos1+_ycellsize);
            if( boundary.partialWithin(cellBounds) ) {
                int offset = _xsize * y + x;
                cells.push_back(offset);
            }
        }
    }
    return cells;
}


void
GUIGrid::buildRelationships()
{
    for(int y=0; y<_ysize; y++) {
        for(int x=0; x<_xsize; x++) {
            size_t pos = y*_xsize + x;
            // build cont without upper
            GridCell cont = _grid[pos];
            removeFrom(cont, x, y-1);
            _relations[0][pos] = cont;
            // build cont without left
            cont = _grid[pos];
            removeFrom(cont, x-1, y);
            _relations[1][pos] = cont;
            // build cont without left and upper
            cont = _grid[pos];
            removeFrom(cont, x-1, y);
            removeFrom(cont, x, y-1);
            _relations[2][pos] = cont;
        }
    }
}


void
GUIGrid::removeFrom(GridCell &cont, int x, int y)
{
    if(x<0||y<0||x>=_xsize||y>=_ysize) {
        return;
    }
    // get the list of edges to remove
    size_t pos = y*_xsize + x;
    GridCell &from = _grid[pos];
    cont.removeIfIn(from);
}


void
GUIGrid::get(int what,
             double x, double y, double xoff, double yoff,
             size_t *setEdges, size_t *setJunctions, size_t *setAdditional) const
{
    // compute bounderies
    double xur = x - xoff - _xcellsize;
    double xdl = x + xoff + _xcellsize;
    double yur = y - yoff - _ycellsize;
    double ydl = y + yoff + _ycellsize;

    // loop over bounderies
    double yrun=(yur >= 0 ? yur : 0);
    int ypos = (int) (yur/_ycellsize);
    if(ypos<0) {
        ypos = 0;
    }
    size_t yidx = 0;
    for(; yrun<ydl&&ypos<_ysize; yrun+=_ycellsize, ypos++, yidx++) {
        double xrun=(xur >= 0 ? xur : 0);
        int xpos = (int) (xur/_xcellsize);
        if(xpos<0) {
            xpos = 0;
        }
        size_t xidx = 0;
        for(; xrun<xdl&&xpos<_xsize; xrun+=_xcellsize, xpos++, xidx++) {
            int offs = ypos*_xsize+xpos;
            if(xidx==0&&yidx==0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    _grid[offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    _grid[offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    _grid[offs].setAdditional(setAdditional);
                }
            } else if(yidx!=0 && xidx!=0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    _relations[2][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    _relations[2][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    _relations[2][offs].setAdditional(setAdditional);
                }
            } else if(yidx==0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    _relations[1][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    _relations[1][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    _relations[1][offs].setAdditional(setAdditional);
                }
            } else {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    _relations[0][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    _relations[0][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    _relations[0][offs].setAdditional(setAdditional);
                }
            }
        }
    }
}


int
GUIGrid::getNoXCells() const
{
    return _xsize;
}


int
GUIGrid::getNoYCells() const
{
    return _ysize;
}


double
GUIGrid::getXCellSize() const
{
    return _xcellsize;
}


double
GUIGrid::getYCellSize() const
{
    return _ycellsize;
}


void
GUIGrid::closeBuilding()
{
    // ok, we now have a grid, but we don not want to draw edges more
    //  than once; build the relationship matrix next
    size_t size = getNoXCells() * getNoYCells();
    for(size_t i=0; i<3; i++) {
        _relations[i] = new GUIGrid::GridCell[size];
    }
    buildRelationships();
}


void
GUIGrid::setBoundary(const Boundary &b)
{
    _boundary = b;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




