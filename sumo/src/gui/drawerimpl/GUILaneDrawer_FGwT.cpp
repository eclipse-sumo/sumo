//---------------------------------------------------------------------------//
//                        GUILaneDrawer_FGwT.cpp -
//  A class for vehicle drawing; Vehicles are drawn as triangles
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
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
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream> // !!!
#include <string> // !!!
#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
#include "GUILaneDrawer_FGwT.h"
#include <utils/geom/GeomHelper.h>

#include <qgl.h>



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUILaneDrawer_FGwT::GUILaneDrawer_FGwT(std::vector<GUIEdge*> &edges)
    : GUIBaseLaneDrawer(edges)
{
}


GUILaneDrawer_FGwT::~GUILaneDrawer_FGwT()
{
}


void
GUILaneDrawer_FGwT::drawLane(const GUILaneWrapper &lane,
                             GUISUMOAbstractView::LaneColoringScheme scheme, 
                             double width)
{
    setLaneColor(lane, scheme);
    glPushName(lane.getGlID());
    const DoubleVector &rots = lane.getShapeRotations();
    const DoubleVector &lengths = lane.getShapeLengths();
    const Position2DVector &geom = lane.getShape();
    if(width>1.0) {
        for(size_t i=0; i<geom.size()-1; i++) {
            drawBoxLine(geom.at(i), rots[i], lengths[i]);
        }
    } else {
        for(size_t i=0; i<geom.size()-1; i++) {
            drawLine(geom.at(i), rots[i], lengths[i]);
        }
    }
    glPopName();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUILaneDrawer_FGwT.icc"
//#endif

// Local Variables:
// mode:C++
// End:


