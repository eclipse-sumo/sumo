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
        : myNet(net), myBoundary(boundary)
{
    myBoundary = GeoConvHelper::getConvBoundary();
    myXsize = (size_t)((myBoundary.xmax()-myBoundary.xmin())/MSGlobals::gLANRange) + 1;
    myYsize = (size_t)((myBoundary.ymax()-myBoundary.ymin())/MSGlobals::gLANRange) + 1;
}


MSBuildCells::~MSBuildCells()
{
    size_t size = myXsize*myYsize;
    for (size_t i=0; i<size; i++) {
        delete myCellsCont[i];
    }
}


void
MSBuildCells::build()
{

    // allocate grid
    size_t size = myXsize*myYsize;
    // get the boundary
    if (myBoundary.getHeight()==0||myBoundary.getWidth()==0) {
        myBoundary.add(myBoundary.xmin()+1, myBoundary.ymax()+1);
        myBoundary.add(myBoundary.xmin()-1, myBoundary.ymax()-1);
    }
    // compute the cell size
    myXcellsize = MSGlobals::gLANRange;
    myYcellsize = MSGlobals::gLANRange;
    createCells(size);
    // divide Edges on grid
    divideOnGrid();
    setCellsNeighbors();
}


void
MSBuildCells::createCells(size_t size)
{
    for (size_t i=0; i<size; i++) {
        MSCell *cell = new MSCell(myXcellsize,myXcellsize);
        myCellsCont.push_back(cell);
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
    for (int y=(int)(bb1.ymin()/myYcellsize); y<(int)((bb1.ymax()/myYcellsize)+1)&&y<(int) myYsize; y++) {
        SUMOReal ypos1 = SUMOReal(y) * myYcellsize;
        for (int x=(int)(bb1.xmin()/myXcellsize); x<(int)((bb1.xmax()/myXcellsize)+1)&&x<(int) myXsize; x++) {
            SUMOReal xpos1 = SUMOReal(x) * myXcellsize;

            size_t offset = myXsize * y + x;
            if (
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1, xpos1+myXcellsize, ypos1) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1, xpos1, ypos1+myYcellsize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1+myYcellsize, xpos1+myXcellsize,
                                       ypos1+myYcellsize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1+myXcellsize, ypos1, xpos1+myXcellsize,
                                       ypos1+myYcellsize) ||

                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1, xpos1+myXcellsize, ypos1) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1, xpos1, ypos1+myYcellsize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1+myYcellsize, xpos1+myXcellsize,
                                       ypos1+myYcellsize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1+myXcellsize, ypos1, xpos1+myXcellsize,
                                       ypos1+myYcellsize) ||

                (x11>=xpos1&&x11<xpos1+myXcellsize&&y11>=ypos1&&y11<ypos1+myYcellsize) ||
                (x12>=xpos1&&x12<xpos1+myXcellsize&&y12>=ypos1&&y12<ypos1+myYcellsize) ||
                (x21>=xpos1&&x21<xpos1+myXcellsize&&y21>=ypos1&&y21<ypos1+myYcellsize) ||
                (x22>=xpos1&&x22<xpos1+myXcellsize&&y22>=ypos1&&y22<ypos1+myYcellsize)
            ) {
                myCellsCont[offset]->addEdge(edge);
            }
            myCellsCont[offset]->setIndex(offset); // Die Position ist schon die eigentliche Index
        }
    }
}


void MSBuildCells::setCellsNeighbors(void)
{
    for (size_t i=0; i < myCellsCont.size(); i++) {
        myCellsCont[i]->setCellNeighbors(getNeighbors(i));
        myCellsCont[i]->setEdgesNeighbors();
    }

}


std::vector<MSCell*>
MSBuildCells::getNeighbors(size_t i)
{
    std::vector<MSCell*> ret;
    ret.push_back(myCellsCont[i]); //I'am my own neighbor

    size_t x = i % myXsize;
    if (x==0) {
        ret.push_back(myCellsCont[i+1]);
        //	cout<<"-----Nachbarn1   "<<i+1<<endl;
    }
    if (x==myXsize-1) {
        ret.push_back(myCellsCont[i-1]);
        //cout<<"-----Nachbarn2   "<<i-1<<endl;
    }
    if ((x>0) && (x<myXsize-1)) {
        //	cout<<"-----Nachbarn3   "<<i+1<<endl;
        //	cout<<"-----Nachbarn4   "<<i-1<<endl;
        ret.push_back(myCellsCont[i-1]);
        ret.push_back(myCellsCont[i+1]);
    }

    size_t y = i / myXsize;
    if (y==0) {
        ret.push_back(myCellsCont[i+myXsize]);
        //	cout<<"-----Nachbarn5   "<<i+myXsize<<endl;
    }
    if (y==myYsize-1) {
        ret.push_back(myCellsCont[i-myXsize]);
        //	cout<<"-----Nachbarn6   "<<i-myXsize<<endl;
    }
    if ((y>0) && (y<myYsize-1)) {
        //	cout <<"i == "<<i<<"  y == "<<y<<endl;
        //	cout<<"-----Nachbarn7   "<<i+myXsize<<endl;
        //	cout<<"-----Nachbarn8   "<<i-myXsize<<endl;
        ret.push_back(myCellsCont[i+myXsize]);
        ret.push_back(myCellsCont[i-myXsize]);
    }

    if ((x>0) && (y>0)) {
        ret.push_back(myCellsCont[i-myXsize-1]);
        //	cout<<"-----Nachbarn9   "<<i-myXsize-1<<endl;
    }
    if ((x>0) && (y<myYsize-1)) {
        ret.push_back(myCellsCont[i+myXsize-1]);
        //	cout<<"-----Nachbarn10   "<<i+myXsize-1<<endl;
    }
    if ((x<myXsize-1) && (y<myYsize-1)) {
        ret.push_back(myCellsCont[i+myXsize+1]);
        //	cout<<"-----Nachbarn11   "<<i+myXsize+1<<endl;
    }
    if ((x<myXsize-1) && (y>0)) {
        ret.push_back(myCellsCont[i-myXsize+1]);
        //	cout<<"-----Nachbarn 12  "<<i-myXsize+1<<endl;
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

