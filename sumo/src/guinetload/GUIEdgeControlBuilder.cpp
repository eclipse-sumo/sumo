//---------------------------------------------------------------------------//
//                        GUIEdgeControlBuilder.cpp -
//  A builder for edges during the loading derived from the
//      NLEdgeControlBuilder
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
// Revision 1.11  2005/07/12 12:15:41  dkrajzew
// new loading of edges implemented
//
// Revision 1.10  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.9  2004/11/23 10:12:27  dkrajzew
// new detectors usage applied
//
// Revision 1.8  2004/07/02 08:39:12  dkrajzew
// using global selection storage
//
// Revision 1.7  2004/04/02 11:15:25  dkrajzew
// changes due to the visualisation of the selection status
//
// Revision 1.6  2004/03/19 12:56:48  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/11/12 14:05:18  dkrajzew
// access to the id storage in MSNet is now secure
//
// Revision 1.4  2003/09/05 14:57:12  dkrajzew
// first steps for reading of internal lanes
//
// Revision 1.3  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.2  2003/02/07 10:38:19  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUILane.h>
#include <guisim/GUISourceLane.h>
#include <guisim/GUIInternalLane.h>
#include <microsim/MSJunction.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <netload/NLNetBuilder.h>
#include "GUIEdgeControlBuilder.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIEdgeControlBuilder::GUIEdgeControlBuilder(
        GUIGlObjectStorage &glObjectIDStorage, unsigned int storageSize)
    : NLEdgeControlBuilder(storageSize),
    myGlObjectIDStorage(glObjectIDStorage)
{
}

GUIEdgeControlBuilder::~GUIEdgeControlBuilder()
{
}


MSEdge *
GUIEdgeControlBuilder::addEdge(const string &id)
{
    MSEdge *edge = new GUIEdge(id, myGlObjectIDStorage);
    if(!MSEdge::dictionary(id, edge)) {
        throw XMLIdAlreadyUsedException("Edge", id);
    }
    m_pEdges->push_back(edge);
    return edge;
}


void
GUIEdgeControlBuilder::addSrcDestInfo(const std::string &id,
                                      MSJunction *from, MSJunction *to)
{
    GUIEdge *edge = static_cast<GUIEdge*>(MSEdge::dictionary(id));
    if(edge==0) {
        throw XMLIdNotKnownException("edge", id);
    }
    edge->initJunctions(from, to, gIDStorage);
}


MSLane *
GUIEdgeControlBuilder::addLane(MSNet &net, const std::string &id,
                               double maxSpeed, double length, bool isDepart,
                               const Position2DVector &shape)
{
    // checks if the depart lane was set before
    if(isDepart&&m_pDepartLane!=0) {
      throw XMLDepartLaneDuplicationException();
    }
    MSLane *lane = 0;
    switch(m_Function) {
    case MSEdge::EDGEFUNCTION_SOURCE:
        lane = new GUISourceLane(net, id, maxSpeed, length, m_pActiveEdge,
            myCurrentNumericalLaneID++, shape);
        break;
    case MSEdge::EDGEFUNCTION_INTERNAL:
        lane = new GUIInternalLane(net, id, maxSpeed, length, m_pActiveEdge,
            myCurrentNumericalLaneID++, shape);
        break;
    case MSEdge::EDGEFUNCTION_NORMAL:
    case MSEdge::EDGEFUNCTION_SINK:
        lane = new GUILane(net, id, maxSpeed, length, m_pActiveEdge,
            myCurrentNumericalLaneID++, shape);
        break;
    default:
        throw 1;
    }
    m_pLaneStorage->push_back(lane);
    if(isDepart) {
        m_pDepartLane = lane;
    }
    return lane;
}

/*
void
GUIEdgeControlBuilder::addLaneShape(const Position2DVector &shape)
{
    myLaneShape = shape;
}
*/

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

