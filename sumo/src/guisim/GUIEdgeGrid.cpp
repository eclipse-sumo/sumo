#include <utility>
#include <vector>
#include <algorithm>
#include <utils/geom/Boundery.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUIEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEdge.h>
#include "GUIEdgeCont.h"
#include "GUIEdgeGrid.h"

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
}*/


GUIEdgeGrid::GUIEdgeGrid(size_t xsize, size_t ysize)
	: _xsize(xsize), _ysize(ysize)
{
}


GUIEdgeGrid::~GUIEdgeGrid()
{
	delete[] _grid;
}


void
GUIEdgeGrid::init() {
	// allocate grid
    size_t size = _xsize*_ysize;
	_grid = new GUIEdgeCont[size];
	// get the boundery
	_boundery = computeBoundery();
	// compute the cell size
	_xcellsize =
		(_boundery.xmax()-_boundery.xmin()) / _xsize;
	_ycellsize =
		(_boundery.ymax()-_boundery.ymin()) / _ysize;
	// divide Edges on grid
	divideOnGrid(_xcellsize, _ycellsize, _boundery.xmin());
}


GUIEdgeCont &
GUIEdgeGrid::get(double x, double y)
{
	size_t xpos = (size_t) (x/_xcellsize);
	size_t ypos = (size_t) (y/_ycellsize);
	return _grid[ypos*_xsize+xpos];
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
GUIEdgeGrid::divideOnGrid(double xcellsize, double ycellsize,
                          size_t xsize)
{
	MSEdge::DictType::iterator j;
	for(j=MSEdge::myDict.begin(); j!=MSEdge::myDict.end(); j++) {
		GUIEdge *edge = static_cast<GUIEdge*>((*j).second);
		computeEdgeCells(xcellsize, ycellsize, edge, xsize);
	}
}

void
GUIEdgeGrid::computeEdgeCells(double xcellsize, double ycellsize,
                              GUIEdge *edge, size_t xsize)
{
/*	double x1 = edge->toXPos();
	double x2 = edge->fromXPos();
	double y1 = edge->toYPos();
	double y2 = edge->fromYPos();
    double ymin = y1 < y2 ? y1 : y2;//min(y1, y2);
	double xmin = x1 < x2 ? x1 : x2;//min(x1, x2);
	double ymax = y1 > y2 ? y1 : y2;//max(y1, y2);
	double xmax = x1 > x2 ? x1 : x2;//max(x1, x2);*/
return; // !!!!
    CellPosVector cells;
    cells.push_back(getInitCellPos(edge->fromXPos(), edge->fromYPos()));
    while(cells.size()>0) {
        CellPosVector next_cells;
        for(CellPosVector::iterator i=cells.begin(); i!=cells.end(); i++) {
            set(edge, *i);
            CellPosVector next = getNextCells(edge, *i);
            copy(next.begin(), next.end(), back_inserter(next_cells));
        }
        cells = next_cells;
    }
}


GUIEdgeGrid::CellPos
GUIEdgeGrid::getInitCellPos(double x, double y)
{
    x = x - _boundery.xmin();
    y = y - _boundery.ymin();
    return CellPos(x/_xcellsize, y/_ycellsize);
}

void
GUIEdgeGrid::set(GUIEdge *edge, CellPos pos)
{
	size_t offset = _xsize * pos.second + pos.first;
	if(_grid[offset].size()==0) {
		_grid[offset].push_back(edge);
	} else {
        if(_grid[offset].at(_grid[offset].size()-1)!=edge) {
			_grid[offset].push_back(edge);
        }
	}
}

GUIEdgeGrid::CellPosVector
GUIEdgeGrid::getNextCells(GUIEdge *edge, CellPos pos)
{
    CellPosVector ret;
    unsigned int nolanes = edge->nLanes();
    double offset = nolanes * 3.5; // !!!
    // compute the corners of the cell
    double cxul = pos.first * _xcellsize - offset + _boundery.xmin();
    double cxlr = (pos.first+1) * _xcellsize + offset + _boundery.xmin();
    double cyul = pos.second * _ycellsize - offset + _boundery.ymin();
    double cylr = (pos.second+1) * _ycellsize + offset + _boundery.ymin();
    // check whether the upper cell shall be appended
    if( checkPossible(edge, pos.first, pos.second-1) &&
        checkIntersection(edge, cxul, cyul, cxlr, cyul) ) {
        ret.push_back(CellPos(pos.first, pos.second-1));
    }
    // check whether the lower cell shall be appended
    if( checkPossible(edge, pos.first, pos.second+1) &&
        checkIntersection(edge, cxul, cylr, cxlr, cylr) ) {
        ret.push_back(CellPos(pos.first, pos.second+1));
    }
    // check whether the left cell shall be appended
    if( checkPossible(edge, pos.first-1, pos.second) &&
        checkIntersection(edge, cxul, cyul, cxul, cylr) ) {
        ret.push_back(CellPos(pos.first-1, pos.second));
    }
    // check whether the right cell shall be appended
    if( checkPossible(edge, pos.first+1, pos.second) &&
        checkIntersection(edge, cxlr, cyul, cxlr, cylr) ) {
        ret.push_back(CellPos(pos.first+1, pos.second));
    }
    return ret;
}

bool
GUIEdgeGrid::checkPossible(GUIEdge *edge, int x, int y)
{
    if(!(x>=0 && y>=0 && x<_xsize && y<_ysize)) {
        return false;
    }
    size_t offset = _xsize * y + x;
    return (_grid[offset].size()==0 ||
        _grid[offset].at(_grid[offset].size()-1)!=edge);
}

bool
GUIEdgeGrid::checkIntersection(GUIEdge *edge,
                               double cx1, double cy1, double cx2, double cy2)
{
    return GeomHelper::intersects(edge->fromXPos(), edge->fromYPos(),
        edge->toXPos(), edge->toYPos(), cx1, cy1, cx2, cy2);
}


