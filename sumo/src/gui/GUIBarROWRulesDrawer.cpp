//---------------------------------------------------------------------------//
//                        GUIBarROWRulesDrawer.cpp -
//  A class for
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sun, 10 Aug 2003
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
// Revision 1.2  2003/08/20 11:58:04  dkrajzew
// cleaned up a bit
//
// Revision 1.1  2003/08/14 13:44:14  dkrajzew
// tls/row - drawer added
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
#include "GUIViewTraffic.h"
#include "GUIBarROWRulesDrawer.h"

#include <qgl.h>



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBarROWRulesDrawer::GUIBarROWRulesDrawer(std::vector<GUIEdge*> &edges)
    : GUIROWRulesDrawer(edges)
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


GUIBarROWRulesDrawer::~GUIBarROWRulesDrawer()
{
}


void
GUIBarROWRulesDrawer::drawGLROWs(size_t *which, size_t maxEdges,
                                 bool showToolTips, double width)
{
    if(width<1.0) {
        return;
    }
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
                        drawLinkRules(lane);
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
                        drawLinkRules(lane);
                    }
                }
            }
        }
    }
}

void
GUIBarROWRulesDrawer::initStep(/*const double & width*/)
{
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void
GUIBarROWRulesDrawer::drawLinkRules(const GUILaneWrapper &lane)
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
        glVertex2f(x1-1.5, visLength+0.0);
        glVertex2f(x1-1.5, visLength+0.5);
        glVertex2f(x2-1.5, visLength+0.5);
        glVertex2f(x2-1.5, visLength+0.0);
        glEnd();
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIBarROWRulesDrawer.icc"
//#endif

// Local Variables:
// mode:C++
// End:


