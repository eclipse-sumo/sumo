//---------------------------------------------------------------------------//
//                        GUISimpleLaneDrawer.cpp -
//  A class for vehicle drawing; Vehicles are drawn as triangles
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
// Revision 1.4  2003/04/04 08:37:51  dkrajzew
// view centering now applies net size; closing problems debugged; comments added; tootip button added
//
// Revision 1.3  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/06 17:17:28  dkrajzew
// y-direction flipped
//
// Revision 1.1  2003/02/07 10:34:14  dkrajzew
// files updated
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
#include <guisim/GUILaneWrapper.h>
#include <glut.h>
#include "GUIViewTraffic.h"
#include "GUISimpleLaneDrawer.h"


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUISimpleLaneDrawer::GUISimpleLaneDrawer()
{
}


GUISimpleLaneDrawer::~GUISimpleLaneDrawer()
{
}


void
GUISimpleLaneDrawer::initStep(const double & width)
{
    glLineWidth(1);
    if(width<1) {
        _drawLines = true;
    } else {
        _drawLines = false;
    }
    glColor3f(0, 0, 0);
}


void
GUISimpleLaneDrawer::drawLaneNoTooltips(const GUILaneWrapper &lane,
            GUIViewTraffic::LaneColoringScheme scheme)
{
    setLaneColor(lane, scheme);
    if(!_drawLines) {
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
        glVertex2f(0, 0);
        glVertex2f(0, visLength);
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
}


void
GUISimpleLaneDrawer::drawLaneWithTooltips(const GUILaneWrapper &lane,
            GUIViewTraffic::LaneColoringScheme scheme)
{
    setLaneColor(lane, scheme);
    glPushName(lane.getGlID());
    if(!_drawLines) {
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
        glVertex2f(0, 0);
        glVertex2f(0, visLength);
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


void
GUISimpleLaneDrawer::closeStep()
{
}


void
GUISimpleLaneDrawer::setLaneColor(const GUILaneWrapper &lane,
                                  GUIViewTraffic::LaneColoringScheme scheme)
{
    switch(scheme) {
    case GUIViewTraffic::LCS_BLACK:
        break;
    case GUIViewTraffic::LCS_BY_PURPOSE:
        switch(lane.getPurpose()) {
        case MSEdge::EDGEFUNCTION_NORMAL:
            glColor3f(0, 0, 0);
            break;
        case MSEdge::EDGEFUNCTION_SOURCE:
            glColor3f(0, 1, 0);
            break;
        case MSEdge::EDGEFUNCTION_SINK:
            glColor3f(1, 0, 0);
            break;
        }
        break;
    case GUIViewTraffic::LCS_BY_SPEED:
        {
            double speed = lane.maxSpeed();
            double maxSpeed = GUILaneWrapper::getOverallMaxSpeed();
            double fact = speed / maxSpeed / 2.0;
            glColor3f(1.0-fact, 0.5, 0.5+fact);
        }
        break;
    default:
        throw 1;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUISimpleLaneDrawer.icc"
//#endif

// Local Variables:
// mode:C++
// End:


