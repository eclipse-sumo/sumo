//---------------------------------------------------------------------------//
//                        GUILaneDrawer_SGwT.cpp -
//  Class for drawing lanes with simple geom and tooltip information
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
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
// Revision 1.4  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.3  2004/01/26 06:41:26  dkrajzew
// code style changes
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <string>
#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
#include "GUILaneDrawer_SGwT.h"
#include <utils/geom/GeomHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUILaneDrawer_SGwT::GUILaneDrawer_SGwT(std::vector<GUIEdge*> &edges)
    : GUIBaseLaneDrawer(edges)
{
}


GUILaneDrawer_SGwT::~GUILaneDrawer_SGwT()
{
}


void
GUILaneDrawer_SGwT::drawLane(const GUILaneWrapper &lane,
                             GUISUMOAbstractView::LaneColoringScheme scheme,
                             double width)
{
    setLaneColor(lane, scheme);
    glPushName(lane.getGlID());
    if(width>1.0) {
        glPushMatrix();
        const Position2D &beg = lane.getBegin();
        glTranslated(beg.x(), beg.y(), 0);
        glRotated( lane.getRotation(), 0, 0, 1 );
        double visLength = -lane.visLength();
        glBegin( GL_QUADS );
        glVertex2f(0-1.5, 0);
        glVertex2f(-1.5, visLength);
        glVertex2f(1.5, visLength);
        glVertex2f(1.5, 0);
        glEnd();
        glBegin( GL_LINES);
        // without the substracted offsets, lines are partially longer
        //  than the boxes
        glVertex2f(0, 0-.1);
        glVertex2f(0, visLength-.1);
        glEnd();
        glPopMatrix();
    } else {
        const Position2D &begin = lane.getBegin();
        const Position2D &end = lane.getEnd();
        glBegin( GL_LINES);
        glVertex2f(begin.x(), begin.y());
        glVertex2f(end.x(), end.y());
        glEnd();
    }
    glPopName();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


