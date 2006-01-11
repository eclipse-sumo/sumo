//---------------------------------------------------------------------------//
//                        GUIBaseROWDrawer.h -
//  Base class for drawing right of way-rules
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
// Revision 1.10  2006/01/11 11:54:35  dkrajzew
// reworked possible link states; new link coloring
//
// Revision 1.9  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.8  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/11/12 13:45:25  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <string>
#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUIBaseROWDrawer.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBaseROWDrawer::GUIBaseROWDrawer(std::vector<GUIEdge*> &edges)
    : myEdges(edges)
{
    myLinkColors[MSLink::LINKSTATE_TL_GREEN] = RGBColor(0, 1, 0);
    myLinkColors[MSLink::LINKSTATE_TL_RED] = RGBColor(1, 0, 0);
    myLinkColors[MSLink::LINKSTATE_TL_YELLOW] = RGBColor(1, 1, 0);
    myLinkColors[MSLink::LINKSTATE_TL_OFF_BLINKING] = RGBColor(1, 1, 0);
    myLinkColors[MSLink::LINKSTATE_TL_OFF_NOSIGNAL] = RGBColor(0, 1, 1);
    myLinkColors[MSLink::LINKSTATE_MAJOR] = RGBColor(1, 1, 1);
    myLinkColors[MSLink::LINKSTATE_MINOR] = RGBColor((SUMOReal) .2, (SUMOReal) .2, (SUMOReal) .2);
    myLinkColors[MSLink::LINKSTATE_EQUAL] = RGBColor(0.5, 0.5, .5);
    myLinkColors[MSLink::LINKSTATE_DEADEND] = RGBColor(0, 0, 0);
}


GUIBaseROWDrawer::~GUIBaseROWDrawer()
{
}


void
GUIBaseROWDrawer::drawGLROWs(const GUINet &net, size_t *which,
                             size_t maxEdges, SUMOReal width,
                             bool showLane2Lane, bool withArrows)
{
    if(width<1.0) {
        return;
    }
    if(showLane2Lane) {
        drawGLROWs_WithConnections(net, which, maxEdges, width, withArrows);
    } else {
        drawGLROWs_Only(net, which, maxEdges, width, withArrows);
    }
}


void
GUIBaseROWDrawer::drawGLROWs_Only(const GUINet &net, size_t *which,
                                  size_t maxEdges, SUMOReal width,
                                  bool withArrows)
{
    // initialise drawing
    initStep();
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
                    if(lane.getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
                        continue;
                    }
                    drawLinkRules(net, lane);
                    if(withArrows) {
                        drawArrows(lane);
                    }
                }
            }
        }
    }
}


void
GUIBaseROWDrawer::drawGLROWs_WithConnections(const GUINet &net,
                                             size_t *which,
                                             size_t maxEdges,
                                             SUMOReal width,
                                             bool withArrows)
{
    // initialise drawing
    initStep();
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
                    if(lane.getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
                        continue;
                    }
                    drawLinkRules(net, lane);
                    if(withArrows) {
                        drawArrows(lane);
                    }
                    // this should be independent to the geometry:
                    //  draw from end of first to the begin of second
                    size_t noLinks = lane.getLinkNumber();
                    for(size_t i=0; i<noLinks; i++) {
                        MSLink::LinkState state = lane.getLinkState(i);
                        const RGBColor &color = myLinkColors.find(state)->second;
                        glColor3d(color.red(), color.green(), color.blue());
                        const MSLane *connected = lane.getLinkLane(i);
                        if(connected!=0) {
                            glBegin(GL_LINES);
                            const Position2D &p1 = lane.getShape().at(-1);
                            const Position2D &p2 = connected->getShape().at(0);
                            glVertex2f(p1.x(), p1.y());
                            glVertex2f(p2.x(), p2.y());
                            glEnd();
                            GLHelper::drawTriangleAtEnd(Line2D(p1, p2), (SUMOReal) .4, (SUMOReal) .2);
                        }
                    }
                }
            }
        }
    }
}


void
GUIBaseROWDrawer::initStep()
{
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


