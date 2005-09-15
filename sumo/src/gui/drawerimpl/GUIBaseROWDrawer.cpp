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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <string>
#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
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


GUIBaseROWDrawer::~GUIBaseROWDrawer()
{
}


void
GUIBaseROWDrawer::drawGLROWs(const GUINet &net, size_t *which,
                             size_t maxEdges, double width)
{
    if(width<1.0) {
        return;
    }
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
                    drawArrows(lane);
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


