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
// Revision 1.11  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.10  2003/07/16 15:18:23  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.9  2003/06/06 10:32:20  dkrajzew
// got rid of glut
//
// Revision 1.8  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.7  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.6  2003/04/07 10:15:11  dkrajzew
// glut reinserted
//
// Revision 1.5  2003/04/04 15:11:10  roessel
// Commented out #include <glut.h>
//
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
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
#include "GUIViewTraffic.h"
#include "GUISimpleLaneDrawer.h"

#include <qgl.h>



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUISimpleLaneDrawer::GUISimpleLaneDrawer(std::vector<GUIEdge*> &edges)
    : GUILaneDrawer(edges)
{
    myLinkColors[MSLink::LINKSTATE_ABSTRACT_TL] = RGBColor(0, 0, 1);
    myLinkColors[MSLink::LINKSTATE_TL_GREEN] = RGBColor(0, 1, 0);
    myLinkColors[MSLink::LINKSTATE_TL_RED] = RGBColor(1, 0, 0);
    myLinkColors[MSLink::LINKSTATE_TL_YELLOW] = RGBColor(1, 1, 0);
    myLinkColors[MSLink::LINKSTATE_TL_OFF_BLINKING] = RGBColor(1, 1, 0);
    myLinkColors[MSLink::LINKSTATE_TL_OFF_NOSIGNAL] = RGBColor(0, 1, 1);
    myLinkColors[MSLink::LINKSTATE_MAJOR] = RGBColor(1, 0, 1);
    myLinkColors[MSLink::LINKSTATE_MINOR] = RGBColor(1, 1, 1);
    myLinkColors[MSLink::LINKSTATE_EQUAL] = RGBColor(0.5, 0.5, .5);
}


GUISimpleLaneDrawer::~GUISimpleLaneDrawer()
{
}


void
GUISimpleLaneDrawer::drawGLLanes(size_t *which, size_t maxEdges,
                                 bool showToolTips, double width,
                                 GUIViewTraffic::LaneColoringScheme scheme)
{
    // initialise drawing
    initStep(/*width*/);
    // check whether tool-tip information shall be generated
    if(showToolTips) {
        // go through edges
        for(size_t i=0; i<maxEdges; i++ ) {
            if(which[i]==0) {
                continue;
            }
            size_t pos = 1;
            for(size_t j=0; j<32; j++, pos<<=1) {
                if((which[i]&pos)!=0) {
                    GUIEdge *edge = static_cast<GUIEdge*>(myEdges[j+(i<<5)]);
                    size_t noLanes = edge->nLanes();
                    // go through the current edge's lanes
                    for(size_t k=0; k<noLanes; k++) {
                        const GUILaneWrapper &lane = edge->getLaneGeometry(k);
                        drawLaneWithTooltips(lane, scheme, width);
                        if(width>1.0) {
                            drawLinkRules(lane);
                        }
                    }
                }
            }
        }
    } else {
        // go through edges
        for(size_t i=0; i<maxEdges; i++ ) {
            if(which[i]==0) {
                continue;
            }
            size_t pos = 1;
            for(size_t j=0; j<32; j++, pos<<=1) {
                if((which[i]&pos)!=0) {
                    GUIEdge *edge = static_cast<GUIEdge*>(myEdges[j+(i<<5)]);
                    size_t noLanes = edge->nLanes();
                    // go through the current edge's lanes
                    for(size_t k=0; k<noLanes; k++) {
                        const GUILaneWrapper &lane = edge->getLaneGeometry(k);
                        drawLaneNoTooltips(lane, scheme, width);
                        if(width>1.0) {
                            drawLinkRules(lane);
                        }
                    }
                }
            }
        }
    }
}

void
GUISimpleLaneDrawer::initStep(/*const double & width*/)
{
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // !!!
/*    if(width<1) {
        _drawLines = true;
    } else {
        _drawLines = false;
    }*/
    glColor3f(0, 0, 0);
}


void
GUISimpleLaneDrawer::drawLinkRules(const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
    double visLength = -lane.visLength();
    if(noLinks==0) {
        // draw a grey bar if no links are on the street
        glColor3f(0.5, 0.5, 0.5);
        glPushMatrix();
        const Position2D &beg = lane.getBegin();
        glTranslated(beg.x(), beg.y(), 0);
        glRotated( lane.getRotation(), 0, 0, 1 );
        glBegin( GL_QUADS );
        glVertex2f(-1.5, visLength+4.0);
        glVertex2f(-1.5, visLength+4.5);
        glVertex2f(1.5, visLength+4.5);
        glVertex2f(1.5, visLength+4.0);
        glEnd();
        glPopMatrix();
        return;
    }
    // draw all links
    float w = 3.0 / (float) noLinks;
    float x1 = 0;
    glPushMatrix();
    const Position2D &beg = lane.getBegin();
    glTranslated(beg.x(), beg.y(), 0);
    glRotated( lane.getRotation(), 0, 0, 1 );
    for(size_t i=0; i<noLinks; i++) {
        float x2 = x1 + w;
        MSLink::LinkState state = lane.getLinkState(i);
        const RGBColor &color = myLinkColors.find(state)->second;
        glColor3f(color.red(), color.green(), color.blue());
        glBegin( GL_QUADS );
        glVertex2f(x1-1.5, visLength+4.0);
        glVertex2f(x1-1.5, visLength+4.5);
        glVertex2f(x2-1.5, visLength+4.5);
        glVertex2f(x2-1.5, visLength+4.0);
        glEnd();
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}




void
GUISimpleLaneDrawer::drawLaneNoTooltips(const GUILaneWrapper &lane,
            GUIViewTraffic::LaneColoringScheme scheme, double width)
{
    setLaneColor(lane, scheme);
    if(width>1.0) {
        glPushMatrix();
        const Position2D &beg = lane.getBegin();
        glTranslated(beg.x(), beg.y(), 0);
        glRotated( lane.getRotation(), 0, 0, 1 );
        double visLength = -lane.visLength();
        glBegin( GL_QUADS );
        glVertex2f(-1.5, 0);
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
            GUIViewTraffic::LaneColoringScheme scheme, double width)
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


void
GUISimpleLaneDrawer::setLaneColor(const GUILaneWrapper &lane,
                                  GUIViewTraffic::LaneColoringScheme scheme)
{
    switch(scheme) {
    case GUIViewTraffic::LCS_BLACK:
        glColor3f(0, 0, 0);
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
        case MSEdge::EDGEFUNCTION_UNKNOWN:
        default:
            throw 1;
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


