//---------------------------------------------------------------------------//
//                        GUIEdgeGrid.cpp -
//  A container for gui-edges
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2003/05/21 15:15:40  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.6  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.5  2003/03/06 17:17:32  dkrajzew
// y-direction flipped
//
// Revision 1.4  2003/02/07 10:39:17  dkrajzew
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
#include <vector>
#include <algorithm>
#include <utils/geom/Boundery.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEdge.h>
#include "GUIEdgeCont.h"
#include "GUIEdgeGrid.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/*
GUIEdgeGrid::EdgeAssigner::EdgeAssigner(double xcellsize,
                                        double ycellsize,
                                        GUIEdgeCont *grid,
                                        GUIEdge *edge, size_t xsize)
	: _xcellsize(xcellsize), _ycellsize(ycellsize),
	_grid(grid), _edge(edge),
	_xsize(xsize)
{
}

GUIEdgeGrid::EdgeAssigner::~EdgeAssigner()
{
}

void
GUIEdgeGrid::EdgeAssigner::execute(double x, double y)
{
	size_t xcell = (size_t) (x / _xcellsize);
	size_t ycell = (size_t) (y / _ycellsize);
	size_t offset = _xsize * ycell + xcell;
cout << x << ", " << y << ", " << xcell << ", " << ycell << ", " << offset << endl;
	if(_grid[offset].size()==0) {
		_grid[offset].push_back(_edge);
	} else {
		if(_grid[offset].at(_grid[offset].size()-1)!=_edge)
			_grid[offset].push_back(_edge);
	}
}
*/

GUIEdgeGrid::GUIEdgeGrid(size_t xsize, size_t ysize)
	: _xsize(xsize), _ysize(ysize),
    _grid(0), _relations(0)
{
}


GUIEdgeGrid::~GUIEdgeGrid()
{
	delete[] _grid;
    for(int i=0; i<3; i++) {
        delete[] _relations[i];
        _relations[i] = 0;
    }
    delete[] _relations;
}


void
GUIEdgeGrid::init() {
	// allocate grid
    size_t size = _xsize*_ysize;
	_grid = new GUIEdgeCont[size];
	// get the boundery
	_boundery = computeBoundery();
	// assert that the boundery is not zero in neither dimension
    if(_boundery.getHeight()==0) {
        _boundery.add(_boundery.xmin()+1, _boundery.ymax()+1);
		_boundery.add(_boundery.xmin()-1, _boundery.ymax()-1);
    }
	// compute the cell size
	_xcellsize =
		(_boundery.xmax()-_boundery.xmin()) / _xsize;
	_ycellsize =
		(_boundery.ymax()-_boundery.ymin()) / _ysize;
	// divide Edges on grid
	divideOnGrid(_xcellsize, _ycellsize);
    // ok, we now have a grid, but we don not want to draw edges more
    //  than once; build the relationship matrix next
    _relations = new GUIEdgeCont*[3];
    for(size_t i=0; i<3; i++) {
        _relations[i] = new GUIEdgeCont[size];
    }
    buildRelationships();

/*
    for(size_t a1=0; a1<_ysize; a1++) {
        for(size_t b1=0; b1<_xsize; b1++) {
            size_t offset = a1*_xsize + b1;
            cout << char(_grid[offset].size()+'a');
        }
        cout << endl;
    }
    cout << endl;
    for(size_t z=0; z<3; z++) {
        for(size_t a1=0; a1<_ysize; a1++) {
            for(size_t b1=0; b1<_xsize; b1++) {
                size_t offset = a1*_xsize + b1;
                cout << char(_relations[z][offset].size()+'a');
            }
            cout << endl;
        }
        cout << endl;
    }*/
}


void
GUIEdgeGrid::get(GUIEdgeGrid::GUIEdgeSet &into,
                 double x, double y, double xoff, double yoff)
{
    into.clear();
    // compute bounderies
    double xur = x - xoff - _xcellsize;
    double xdl = x + xoff + _xcellsize;
    double yur = y - yoff - _ycellsize;
    double ydl = y + yoff + _ycellsize;

    // prepare container to return
    into.clear();
    // loop over bounderies
    double yrun=(yur >= 0 ? yur : 0);
    int ypos = (int) (yur/_ycellsize);
    if(ypos<0) {
        ypos = 0;
    }
    size_t yidx = 0;
    size_t max = into.max_size();
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
                into.insert(
                    _grid[offs].begin(),
                    _grid[offs].end());
            } else if(yidx!=0 && xidx!=0) {
                into.insert(
                    _relations[2][offs].begin(),
                    _relations[2][offs].end());
            } else if(yidx==0) {
                into.insert(
                    _relations[1][offs].begin(),
                    _relations[1][offs].end());
            } else {
                into.insert(
                    _relations[0][offs].begin(),
                    _relations[0][offs].end());
            }
        }
    }
}


Boundery
GUIEdgeGrid::computeBoundery()
{
	Boundery ret;
	MSJunction::DictType::iterator i;
	// get the bounderies of the network
	for(i=MSJunction::myDict.begin(); i!=MSJunction::myDict.end(); i++) {
		MSJunction *junction = ((*i).second);
		ret.add(junction->getXCoordinate(), junction->getYCoordinate());
	}
	return ret;
}


const Boundery &
GUIEdgeGrid::getBoundery() const
{
    return _boundery;
}


void
GUIEdgeGrid::divideOnGrid(double xcellsize, double ycellsize)
{
	MSEdge::DictType::iterator j;
	for(j=MSEdge::myDict.begin(); j!=MSEdge::myDict.end(); j++) {
		GUIEdge *edge = static_cast<GUIEdge*>((*j).second);
		computeEdgeCells(xcellsize, ycellsize, edge);
	}
}


void
GUIEdgeGrid::computeEdgeCells(double xcellsize, double ycellsize,
                              GUIEdge *edge)
{
    // compute the outer and inner positions of the edge
    //  (meaning the real edge position and the position yielding from
    //  adding the offset of lanes)
    double x11 = edge->fromXPos();
    double y11 = edge->fromYPos();
    double x12 = edge->toXPos();
    double y12 = edge->toYPos();
    double length = sqrt( (x11-x12)*(x11-x12) + (y11-y12)*(y11-y12) );
    std::pair<double, double> offsets =
        GUIEdge::getLaneOffsets(x11, y11, x12, y12, length,
            3.5 * double(edge->nLanes()));
    double x21 = x11 + offsets.first;
    double y21 = y11 - offsets.second;
    double x22 = x12 + offsets.first;
    double y22 = y12 - offsets.second;

    // compute the cells the lae is going through
    for(size_t y=0; y<_ysize; y++) {
        double ypos1 = double(y) * _ycellsize;
        for(size_t x=0; x<_xsize; x++) {
            double xpos1 = double(x) * _xcellsize;
            if(
                GeomHelper::intersects(x11, y11, x12, y12,
                    xpos1, ypos1, xpos1+_xcellsize, ypos1) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                    xpos1, ypos1, xpos1, ypos1+_ycellsize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                    xpos1, ypos1+_ycellsize, xpos1+_xcellsize,
                    ypos1+_ycellsize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                    xpos1+_xcellsize, ypos1, xpos1+_xcellsize,
                    ypos1+_ycellsize) ||

                GeomHelper::intersects(x21, y21, x22, y22,
                    xpos1, ypos1, xpos1+_xcellsize, ypos1) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                    xpos1, ypos1, xpos1, ypos1+_ycellsize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                    xpos1, ypos1+_ycellsize, xpos1+_xcellsize,
                    ypos1+_ycellsize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                    xpos1+_xcellsize, ypos1, xpos1+_xcellsize,
                    ypos1+_ycellsize) ||

                (x11>=xpos1&&x11<xpos1+_xcellsize&&y11>=ypos1&&y11<ypos1+_ycellsize) ||
                (x12>=xpos1&&x12<xpos1+_xcellsize&&y12>=ypos1&&y12<ypos1+_ycellsize) ||
                (x21>=xpos1&&x21<xpos1+_xcellsize&&y21>=ypos1&&y21<ypos1+_ycellsize) ||
                (x22>=xpos1&&x22<xpos1+_xcellsize&&y22>=ypos1&&y22<ypos1+_ycellsize)
                    )
            {
            	size_t offset = _xsize * y + x;
                _grid[offset].push_back(edge);
            }
        }
    }
}


void
GUIEdgeGrid::buildRelationships()
{
    for(int y=0; y<_ysize; y++) {
        for(int x=0; x<_xsize; x++) {
            size_t pos = y*_xsize + x;
            // build cont without upper
            GUIEdgeCont cont = _grid[pos];
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
GUIEdgeGrid::removeFrom(GUIEdgeCont &cont, int x, int y)
{
    if(x<0||y<0||x>=_xsize||y>=_ysize) {
        return;
    }
    // get the list of edges to remove
    size_t pos = y*_xsize + x;
    GUIEdgeCont &from = _grid[pos];
    // remove
    for(GUIEdgeCont::reverse_iterator i=from.rbegin(); i!=from.rend(); i++) {
        GUIEdge *edge = (*i);
        GUIEdgeCont::iterator j = find(cont.begin(), cont.end(), edge);
        if(j!=cont.end()) {
            cont.erase(j);
        }
    }
}


size_t
GUIEdgeGrid::getNoXCells() const
{
    return _xsize;
}


size_t
GUIEdgeGrid::getNoYCells() const
{
    return _ysize;
}


const double &
GUIEdgeGrid::getXCellSize() const
{
    return _xcellsize;
}


const double &
GUIEdgeGrid::getYCellSize() const
{
    return _ycellsize;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIEdgeGrid.icc"
//#endif

// Local Variables:
// mode:C++
// End:


