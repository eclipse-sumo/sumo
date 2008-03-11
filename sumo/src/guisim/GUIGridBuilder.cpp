/****************************************************************************/
/// @file    GUIGridBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Jul 2003
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
#include "GUINet.h"
#include "GUIEdge.h"
#include <utils/geom/GeomHelper.h>
#include "GUIGridBuilder.h"
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIJunctionWrapper.h>

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
GUIGridBuilder::GUIGridBuilder(GUINet &net, GUIGrid &grid)
        : myNet(net), myGrid(grid)
{}


GUIGridBuilder::~GUIGridBuilder()
{}



void
GUIGridBuilder::build()
{
    // allocate grid
    size_t size = myGrid.getNoXCells()*myGrid.getNoYCells();
    myGrid.myGrid = new GUIGrid::GridCell[size];
    // get the boundary
    myGrid.myBoundary = computeBoundary();
    // assert that the boundary is not zero in neither dimension
    if (myGrid.myBoundary.getHeight()==0||myGrid.myBoundary.getWidth()==0) {
        myGrid.myBoundary.add(myGrid.myBoundary.xmin()+1, myGrid.myBoundary.ymax()+1);
        myGrid.myBoundary.add(myGrid.myBoundary.xmin()-1, myGrid.myBoundary.ymax()-1);
    }
    // compute the cell size
    myGrid.myXCellSize =
        (myGrid.myBoundary.xmax()-myGrid.myBoundary.xmin()) / myGrid.getNoXCells();
    myGrid.myYCellSize =
        (myGrid.myBoundary.ymax()-myGrid.myBoundary.ymin()) / myGrid.getNoYCells();
    // divide Edges on grid
    divideOnGrid();
    myGrid.closeBuilding();
}


Boundary
GUIGridBuilder::computeBoundary()
{
    Boundary ret;
    {
        // use the junctions to compute the boundaries
        for (size_t index=0; index<myNet.myJunctionWrapper.size(); index++) {
            if (myNet.myJunctionWrapper[index]->getShape().size()>0) {
                ret.add(myNet.myJunctionWrapper[index]->getBoundary());
            } else {
                ret.add(myNet.myJunctionWrapper[index]->getJunction().getPosition());
            }
        }
    }
    {
        // use the lanes to compute the boundaries
        for (size_t index=0; index<myNet.myEdgeWrapper.size(); index++) {
            ret.add(myNet.myEdgeWrapper[index]->getBoundary());
        }
    }
    return ret;
}


void
GUIGridBuilder::divideOnGrid()
{
    size_t index;
    for (index=0; index<myNet.myEdgeWrapper.size(); index++) {
        computeEdgeCells(index, myNet.myEdgeWrapper[index]);
    }
    for (index=0; index<myNet.myJunctionWrapper.size(); index++) {
        setJunction(index, myNet.myJunctionWrapper[index]);
    }
    const std::vector<GUIGlObject_AbstractAdd*> &add =
        GUIGlObject_AbstractAdd::getObjectList();
    for (index=0; index<add.size(); index++) {
        setAdditional(index, add[index]);
    }
}


void
GUIGridBuilder::computeEdgeCells(size_t index, GUIEdge *edge)
{
    for (size_t i=0; i<edge->nLanes(); ++i) {
        GUILaneWrapper &lane = edge->getLaneGeometry(i);
        computeLaneCells(index, lane);
    }
}


void
GUIGridBuilder::computeLaneCells(size_t index, GUILaneWrapper &lane)
{
    // compute the outer and inner positions of the edge
    //  (meaning the real edge position and the position yielding from
    //  adding the offset of lanes)
    const Position2D &beg = lane.getShape()[0];
    const Position2D &end = lane.getShape()[-1];
    SUMOReal length = GeomHelper::distance(beg, end);
    std::pair<SUMOReal, SUMOReal> offsets(0, 0);
    if (length!=0) {
        offsets = GeomHelper::getNormal90D_CW(beg, end, length, 3.5 / 2.0);
    }
    SUMOReal x11 = beg.x() - offsets.first;
    SUMOReal y11 = beg.y() + offsets.second;
    SUMOReal x12 = end.x() - offsets.first;
    SUMOReal y12 = end.y() + offsets.second;

    SUMOReal x21 = beg.x() + offsets.first;
    SUMOReal y21 = beg.y() - offsets.second;
    SUMOReal x22 = end.x() + offsets.first;
    SUMOReal y22 = end.y() - offsets.second;

    // compute the cells the lae is going through
    for (int y=0; y<(int) myGrid.myYSize; y++) {
        SUMOReal ypos1 = SUMOReal(y) * myGrid.myYCellSize;
        for (int x=0; x<(int) myGrid.myXSize; x++) {
            SUMOReal xpos1 = SUMOReal(x) * myGrid.myXCellSize;
            if (
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1, xpos1+myGrid.myXCellSize, ypos1) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1, xpos1, ypos1+myGrid.myYCellSize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1+myGrid.myYCellSize, xpos1+myGrid.myXCellSize,
                                       ypos1+myGrid.myYCellSize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1+myGrid.myXCellSize, ypos1, xpos1+myGrid.myXCellSize,
                                       ypos1+myGrid.myYCellSize) ||

                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1, xpos1+myGrid.myXCellSize, ypos1) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1, xpos1, ypos1+myGrid.myYCellSize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1+myGrid.myYCellSize, xpos1+myGrid.myXCellSize,
                                       ypos1+myGrid.myYCellSize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1+myGrid.myXCellSize, ypos1, xpos1+myGrid.myXCellSize,
                                       ypos1+myGrid.myYCellSize) ||

                (x11>=xpos1&&x11<xpos1+myGrid.myXCellSize&&y11>=ypos1&&y11<ypos1+myGrid.myYCellSize) ||
                (x12>=xpos1&&x12<xpos1+myGrid.myXCellSize&&y12>=ypos1&&y12<ypos1+myGrid.myYCellSize) ||
                (x21>=xpos1&&x21<xpos1+myGrid.myXCellSize&&y21>=ypos1&&y21<ypos1+myGrid.myYCellSize) ||
                (x22>=xpos1&&x22<xpos1+myGrid.myXCellSize&&y22>=ypos1&&y22<ypos1+myGrid.myYCellSize)
            ) {
                size_t offset = myGrid.myXSize * y + x;
                myGrid.myGrid[offset].addEdge(index);
            }
        }
    }
}


void
GUIGridBuilder::setJunction(size_t index, GUIJunctionWrapper *junction)
{
    std::vector<size_t> cells = myGrid.getCellsContaining(junction->getBoundary());
    for (std::vector<size_t>::iterator i=cells.begin(); i!=cells.end(); ++i) {
        myGrid.myGrid[*i].addJunction(index);
    }
}


void
GUIGridBuilder::setAdditional(size_t index, GUIGlObject_AbstractAdd *add)
{
    std::vector<size_t> cells = myGrid.getCellsContaining(add->getCenteringBoundary());
    for (std::vector<size_t>::iterator i=cells.begin(); i!=cells.end(); ++i) {
        myGrid.myGrid[*i].addAdditional(index);
    }
}



/****************************************************************************/

