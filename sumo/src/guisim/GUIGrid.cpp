#include <vector>
#include <algorithm>
#include "GUINet.h"
#include "GUIEdge.h"
#include <utils/geom/GeomHelper.h>
#include "GUIGrid.h"
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIJunctionWrapper.h>
#include <guisim/GUIDetectorWrapper.h>
#include <guisim/GUIEmitterWrapper.h>

using namespace std;

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
GUIGrid::GridCell::addDetector(size_t no)
{
    myDetectors.add(no);
}


void
GUIGrid::GridCell::addEmitter(size_t no)
{
    myEmitter.add(no);
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
GUIGrid::GridCell::setDetectors(size_t *into) const
{
    myDetectors.setInto(into);
}


void
GUIGrid::GridCell::setEmitters(size_t *into) const
{
    myEmitter.setInto(into);
}


void
GUIGrid::GridCell::removeIfIn(const GridCell &other)
{
    myJunctions.removeIfIn(other.myJunctions);
    myEdges.removeIfIn(other.myEdges);
    myDetectors.removeIfIn(other.myDetectors);
    myEmitter.removeIfIn(other.myEmitter);
}








GUIGrid::GUIGrid(GUINet &net, int noXCells, int noYCells)
    : _xcellsize(0), _ycellsize(0), _boundery(), _grid(0),
    _xsize(noXCells), _ysize(noYCells), _net(net)
{
}


GUIGrid::~GUIGrid()
{
}



void
GUIGrid::init()
{
	// allocate grid
    size_t size = _xsize*_ysize;
    _grid = new GUIGrid::GridCell[size];
	// get the boundery
	_boundery = computeBoundery();
	// assert that the boundery is not zero in neither dimension
    if(_boundery.getHeight()==0||_boundery.getWidth()==0) {
        _boundery.add(_boundery.xmin()+1, _boundery.ymax()+1);
		_boundery.add(_boundery.xmin()-1, _boundery.ymax()-1);
    }
	// compute the cell size
	_xcellsize =
		(_boundery.xmax()-_boundery.xmin()) / _xsize;
	_ycellsize =
		(_boundery.ymax()-_boundery.ymin()) / _ysize;
	// divide Edges on grid
	divideOnGrid();
    // ok, we now have a grid, but we don not want to draw edges more
    //  than once; build the relationship matrix next
    for(size_t i=0; i<3; i++) {
        _relations[i] = new GridCell[size];
    }
    buildRelationships();
}


Boundery
GUIGrid::computeBoundery()
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
GUIGrid::getBoundery() const
{
    return _boundery;
}


void
GUIGrid::divideOnGrid()
{
    size_t index;
    for(index=0; index<_net.myEdgeWrapper.size(); index++) {
        computeEdgeCells(index, _net.myEdgeWrapper[index]);
	}
    for(index=0; index<_net.myJunctionWrapper.size(); index++) {
        setJunction(index, _net.myJunctionWrapper[index]);
	}
    for(index=0; index<_net.myDetectorWrapper.size(); index++) {
        setDetector(index, _net.myDetectorWrapper[index]);
	}
    for(index=0; index<_net.myEmitter.size(); index++) {
        setEmitter(index, _net.myEmitter[index]);
	}
}


void
GUIGrid::computeEdgeCells(size_t index, GUIEdge *edge)
{
    for(size_t i=0; i<edge->nLanes(); i++) {
        GUILaneWrapper &lane = edge->getLaneGeometry(i);
        computeLaneCells(index, lane);
    }
}


void
GUIGrid::computeLaneCells(size_t index, GUILaneWrapper &lane)
{
    // compute the outer and inner positions of the edge
    //  (meaning the real edge position and the position yielding from
    //  adding the offset of lanes)
    const Position2D &beg = lane.getBegin();
    const Position2D &end = lane.getEnd();
    double length = GeomHelper::distance(beg, end);
    std::pair<double, double> offsets =
        GeomHelper::getNormal90D_CW(beg, end, length,
            3.5 / 2.0);
    double x11 = beg.x() - offsets.first;
    double y11 = beg.y() + offsets.second;
    double x12 = end.x() - offsets.first;
    double y12 = end.y() + offsets.second;

    double x21 = beg.x() + offsets.first;
    double y21 = beg.y() - offsets.second;
    double x22 = end.x() + offsets.first;
    double y22 = end.y() - offsets.second;

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
                _grid[offset].addEdge(index);
            }
        }
    }
/*
    for(size_t y1=0; y1<_ysize; y1++) {
        for(size_t x1=0; x1<_xsize; x1++) {
            size_t offset = _xsize * y1 + x1;
            if(_grid[offset].myEdges._cont.size()>0) {
                cout << _grid[offset].myEdges._cont[0].mySet << ", ";
            } else {
                cout << 0 << ", ";
            }
        }
        cout << endl;
    }
    */
}


void
GUIGrid::setJunction(size_t index, GUIJunctionWrapper *junction)
{
    std::vector<size_t> cells = getCellsContaining(junction->getBoundery());
    for(std::vector<size_t>::iterator i=cells.begin(); i!=cells.end(); i++) {
        _grid[*i].addJunction(index);
    }
}


void
GUIGrid::setDetector(size_t index, GUIDetectorWrapper *detector)
{
    std::vector<size_t> cells = getCellsContaining(detector->getBoundery());
    for(std::vector<size_t>::iterator i=cells.begin(); i!=cells.end(); i++) {
        _grid[*i].addDetector(index);
    }
}


void
GUIGrid::setEmitter(size_t index, GUIEmitterWrapper *emitter)
{
    std::vector<size_t> cells = getCellsContaining(emitter->getBoundery());
    for(std::vector<size_t>::iterator i=cells.begin(); i!=cells.end(); i++) {
        _grid[*i].addEmitter(index);
    }
}

std::vector<size_t>
GUIGrid::getCellsContaining(Boundery boundery)
{
    std::vector<size_t> cells;
    // compute the cells the lae is going through
    for(size_t y=0; y<_ysize; y++) {
        double ypos1 = double(y) * _ycellsize;
        for(int x=0; x<_xsize; x++) {
            double xpos1 = double(x) * _xcellsize;
            Boundery cellBounds;
            cellBounds.add(xpos1, ypos1);
            cellBounds.add(xpos1+_xcellsize, ypos1+_ycellsize);
            if( boundery.partialWithin(cellBounds) ) {
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
GUIGrid::get(/*GridReader &reader, */int what,
             double x, double y, double xoff, double yoff,
             size_t *setEdges, size_t *setJunctions, size_t *setDetectors,
             size_t *setEmitter)
{
    // compute bounderies
    double xur = x - xoff - _xcellsize;
    double xdl = x + xoff + _xcellsize;
    double yur = y - yoff - _ycellsize;
    double ydl = y + yoff + _ycellsize;

    // prepare container to return
//    into.clear();
    // loop over bounderies
    double yrun=(yur >= 0 ? yur : 0);
    int ypos = (int) (yur/_ycellsize);
    if(ypos<0) {
        ypos = 0;
    }
    size_t yidx = 0;
//    size_t max = into.max_size();
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
                    _grid[offs].setDetectors(setDetectors);
                }
                if((what&GLO_EMITTER)!=0) {
                    _grid[offs].setEmitters(setEmitter);
                }
/*                into.insert(
                    _grid[offs].begin(),
                    _grid[offs].end());*/
            } else if(yidx!=0 && xidx!=0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    _relations[2][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    _relations[2][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    _relations[2][offs].setDetectors(setDetectors);
                }
                if((what&GLO_EMITTER)!=0) {
                    _relations[2][offs].setEmitters(setEmitter);
                }
/*                into.insert(
                    _relations[2][offs].begin(),
                    _relations[2][offs].end());*/
            } else if(yidx==0) {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    _relations[1][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    _relations[1][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    _relations[1][offs].setDetectors(setDetectors);
                }
                if((what&GLO_EMITTER)!=0) {
                    _relations[1][offs].setEmitters(setEmitter);
                }
/*                into.insert(
                    _relations[1][offs].begin(),
                    _relations[1][offs].end());*/
            } else {
                if((what&GLO_LANE)!=0||(what&GLO_EDGE)!=0) {
                    _relations[0][offs].setEdges(setEdges);
                }
                if((what&GLO_JUNCTION)!=0) {
                    _relations[0][offs].setJunctions(setJunctions);
                }
                if((what&GLO_DETECTOR)!=0) {
                    _relations[0][offs].setDetectors(setDetectors);
                }
                if((what&GLO_EMITTER)!=0) {
                    _relations[0][offs].setEmitters(setEmitter);
                }
/*                into.insert(
                    _relations[0][offs].begin(),
                    _relations[0][offs].end());*/
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






