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
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
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
#include "GUIBaseROWDrawer.h"

#include <qgl.h>

#include <gui/icons/arrows/p.xpm>
#include <gui/icons/arrows/pl_1.xpm>
#include <gui/icons/arrows/pl_2.xpm>
#include <gui/icons/arrows/pl_3.xpm>
#include <gui/icons/arrows/pr_1.xpm>
#include <gui/icons/arrows/pr_2.xpm>
#include <gui/icons/arrows/pr_3.xpm>




/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBaseROWDrawer::GUIBaseROWDrawer(std::vector<GUIEdge*> &edges)
    : GUIROWRulesDrawer(edges), myAmInitialised(false)
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
GUIBaseROWDrawer::drawGLROWs(size_t *which, size_t maxEdges, double width)
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
                    drawLinkRules(lane);
                    drawArrows(lane);
                }
            }
        }
    }
}


void
GUIBaseROWDrawer::initStep()
{
    if(!myAmInitialised) {
        glGenTextures(6, myTextureIDs);
        myTextures[MSLink::LINKDIR_STRAIGHT] = QImage(p_xpm);
        myTextures[MSLink::LINKDIR_TURN] = QImage(pl_3_xpm);
        myTextures[MSLink::LINKDIR_LEFT] = QImage(pl_2_xpm);
        myTextures[MSLink::LINKDIR_RIGHT] = QImage(pr_2_xpm);
        myTextures[MSLink::LINKDIR_PARTLEFT] = QImage(pl_1_xpm);
        myTextures[MSLink::LINKDIR_PARTRIGHT] = QImage(pr_1_xpm);
        for(size_t i=0; i<6; i++) {
            initTexture(i);
        }
        myAmInitialised = true;
    }
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void
GUIBaseROWDrawer::initTexture(size_t no)
{
    QImage use = QGLWidget::convertToGLFormat(myTextures[no]);
    glBindTexture(GL_TEXTURE_2D, myTextureIDs[no]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        use.width(), use.height(), 0,
        GL_RGBA, GL_UNSIGNED_BYTE, use.bits() );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIBaseROWDrawer.icc"
//#endif

// Local Variables:
// mode:C++
// End:


