/****************************************************************************/
/// @file    MSBuildCells.cpp
/// @author  Danilot Teta Boyom
/// @date    Jul 2006
/// @version $Id$
///
// }
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <vector>
#include <algorithm>
#include "MSNet.h"
#include "MSEdge.h"
#include "MSLane.h"
#include <utils/geom/GeomHelper.h>

#include "MSBuildCells.h"
#include <microsim/MSEdgeControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <utils/geom/GeoConvHelper.h>
#include "MSCell.h"
#include <utils/iodevices/OutputDevice.h>

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
MSBuildCells::MSBuildCells(MSNet &net, Boundary boundary)
        : myNet(net), _boundary(boundary)
{
    _boundary = GeoConvHelper::getConvBoundary();
    _xsize = (size_t)((_boundary.xmax()-_boundary.xmin())/MSGlobals::gLANRange) + 1;
    _ysize = (size_t)((_boundary.ymax()-_boundary.ymin())/MSGlobals::gLANRange) + 1;
}


MSBuildCells::~MSBuildCells()
{
    size_t size = _xsize*_ysize;
    for (size_t i=0; i<size; i++) {
        delete _cellsCont[i];
    }
}


void
MSBuildCells::build()
{

    // allocate grid
    size_t size = _xsize*_ysize;
    // get the boundary
    if (_boundary.getHeight()==0||_boundary.getWidth()==0) {
        _boundary.add(_boundary.xmin()+1, _boundary.ymax()+1);
        _boundary.add(_boundary.xmin()-1, _boundary.ymax()-1);
    }
    // compute the cell size
    _xcellsize = MSGlobals::gLANRange;
    _ycellsize = MSGlobals::gLANRange;
    createCells(size);
    // divide Edges on grid
    divideOnGrid();
    setCellsNeighbors();
}


void
MSBuildCells::createCells(size_t size)
{
    for (size_t i=0; i<size; i++) {
        MSCell *cell = new MSCell(_xcellsize,_xcellsize);
        _cellsCont.push_back(cell);
    }
}



void
MSBuildCells::divideOnGrid()
{
    size_t index, sizeOfCont;
    sizeOfCont = myNet.getEdgeControl().getSingleLaneEdges().size();
    for (index=0; index<sizeOfCont; index++) {
        computeEdgeCells(index, (myNet.getEdgeControl().getSingleLaneEdges())[index]);
    }
    sizeOfCont = myNet.getEdgeControl().getMultiLaneEdges().size();
    for (index=0; index<sizeOfCont; index++) {
        computeEdgeCells(index, (myNet.getEdgeControl().getMultiLaneEdges())[index]);
    }
}


void
MSBuildCells::computeEdgeCells(size_t index, MSEdge *edge)
{
    //const MSEdge::LaneCont * const lanes = edge->getLanes();
    const std::vector<MSLane*> *lanes = edge->getLanes();
    for (size_t i=0; i<lanes->size(); i++) {
        MSLane *lane = (*lanes)[i];
        computeLaneCells(index, lane->getShape(), edge);
    }
}


void
MSBuildCells::computeLaneCells(size_t /*index !!!*/, const Position2DVector &lane, MSEdge *edge)
{
    // compute the outer and inner positions of the edge
    //  (meaning the real edge position and the position yielding from
    //  adding the offset of lanes)
    const Position2D &beg = lane.getBegin();
    const Position2D &end = lane.getEnd();
    SUMOReal length = GeomHelper::distance(beg, end);
    std::pair<SUMOReal, SUMOReal> offsets(0, 0);
    if (length!=0) {
        offsets = GeomHelper::getNormal90D_CW(beg, end, length,
                                              3.5 / 2.0);
    }
    SUMOReal x11 = beg.x() - offsets.first;
    SUMOReal y11 = beg.y() + offsets.second;
    SUMOReal x12 = end.x() - offsets.first;
    SUMOReal y12 = end.y() + offsets.second;

    SUMOReal x21 = beg.x() + offsets.first;
    SUMOReal y21 = beg.y() - offsets.second;
    SUMOReal x22 = end.x() + offsets.first;
    SUMOReal y22 = end.y() - offsets.second;

    Boundary bb1;
    for (size_t i=0; i<lane.size(); i++) {
        bb1.add(lane[i]);
    }
    // compute the cells the lane is going through
    for (int y=(int)(bb1.ymin()/_ycellsize); y<(int)((bb1.ymax()/_ycellsize)+1)&&y<(int) _ysize; y++) {
        SUMOReal ypos1 = SUMOReal(y) * _ycellsize;
        for (int x=(int)(bb1.xmin()/_xcellsize); x<(int)((bb1.xmax()/_xcellsize)+1)&&x<(int) _xsize; x++) {
            SUMOReal xpos1 = SUMOReal(x) * _xcellsize;

            size_t offset = _xsize * y + x;
            if (
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
            ) {
                _cellsCont[offset]->addEdge(edge);
            }
            _cellsCont[offset]->setIndex(offset); // Die Position ist schon die eigentliche Index
        }
    }
}


void MSBuildCells::setCellsNeighbors(void)
{
    for (size_t i=0; i < _cellsCont.size(); i++) {
        _cellsCont[i]->setCellNeighbors(getNeighbors(i));
        _cellsCont[i]->setEdgesNeighbors();
    }

}


std::vector<MSCell*>
MSBuildCells::getNeighbors(size_t i)
{
    std::vector<MSCell*> ret;
    ret.push_back(_cellsCont[i]); //I'am my own neighbor

    size_t x = i % _xsize;
    if (x==0) {
        ret.push_back(_cellsCont[i+1]);
        //	cout<<"-----Nachbarn1   "<<i+1<<endl;
    }
    if (x==_xsize-1) {
        ret.push_back(_cellsCont[i-1]);
        //cout<<"-----Nachbarn2   "<<i-1<<endl;
    }
    if ((x>0) && (x<_xsize-1)) {
        //	cout<<"-----Nachbarn3   "<<i+1<<endl;
        //	cout<<"-----Nachbarn4   "<<i-1<<endl;
        ret.push_back(_cellsCont[i-1]);
        ret.push_back(_cellsCont[i+1]);
    }

    size_t y = i / _xsize;
    if (y==0) {
        ret.push_back(_cellsCont[i+_xsize]);
        //	cout<<"-----Nachbarn5   "<<i+_xsize<<endl;
    }
    if (y==_ysize-1) {
        ret.push_back(_cellsCont[i-_xsize]);
        //	cout<<"-----Nachbarn6   "<<i-_xsize<<endl;
    }
    if ((y>0) && (y<_ysize-1)) {
        //	cout <<"i == "<<i<<"  y == "<<y<<endl;
        //	cout<<"-----Nachbarn7   "<<i+_xsize<<endl;
        //	cout<<"-----Nachbarn8   "<<i-_xsize<<endl;
        ret.push_back(_cellsCont[i+_xsize]);
        ret.push_back(_cellsCont[i-_xsize]);
    }

    if ((x>0) && (y>0)) {
        ret.push_back(_cellsCont[i-_xsize-1]);
        //	cout<<"-----Nachbarn9   "<<i-_xsize-1<<endl;
    }
    if ((x>0) && (y<_ysize-1)) {
        ret.push_back(_cellsCont[i+_xsize-1]);
        //	cout<<"-----Nachbarn10   "<<i+_xsize-1<<endl;
    }
    if ((x<_xsize-1) && (y<_ysize-1)) {
        ret.push_back(_cellsCont[i+_xsize+1]);
        //	cout<<"-----Nachbarn11   "<<i+_xsize+1<<endl;
    }
    if ((x<_xsize-1) && (y>0)) {
        ret.push_back(_cellsCont[i-_xsize+1]);
        //	cout<<"-----Nachbarn 12  "<<i-_xsize+1<<endl;
    }
    return ret;

}


void
MSBuildCells::writeNearEdges(OutputDevice *od)
{
    od->getOStream() << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << endl << "<edge-neighbors>" << endl;
    size_t index, sizeOfCont;
    sizeOfCont = myNet.getEdgeControl().getSingleLaneEdges().size();
    for (index=0; index<sizeOfCont; index++) {
        MSEdge *e = myNet.getEdgeControl().getSingleLaneEdges()[index];
        const std::vector<MSEdge*> &neighbors = e->getNeighborEdges();
        od->getOStream() << "   <edge id=\"" << e->getID()
        << "\" neighborNo=\"" << neighbors.size() << "\">";
        for (std::vector<MSEdge*>::const_iterator j=neighbors.begin(); j!=neighbors.end(); ++j) {
            if (j!=neighbors.begin()) {
                od->getOStream() << ' ';
            }
            od->getOStream() << (*j)->getID();
        }
        od->getOStream() << "</edge>" << endl;
    }
    sizeOfCont = myNet.getEdgeControl().getMultiLaneEdges().size();
    for (index=0; index<sizeOfCont; index++) {
        MSEdge *e = myNet.getEdgeControl().getMultiLaneEdges()[index];
        const std::vector<MSEdge*> &neighbors = e->getNeighborEdges();
        od->getOStream() << "   <edge id=\"" << e->getID()
        << "\" neighborNo=\"" << neighbors.size() << "\">";
        for (std::vector<MSEdge*>::const_iterator j=neighbors.begin(); j!=neighbors.end(); ++j) {
            if (j!=neighbors.begin()) {
                od->getOStream() << ' ';
            }
            od->getOStream() << (*j)->getID();
        }
        od->getOStream() << "</edge>" << endl;
    }
    od->getOStream() << "</edge-neighbors>" << endl;
}


/****************************************************************************/

