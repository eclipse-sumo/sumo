//---------------------------------------------------------------------------//
//                        GUILaneDrawer_SG.cpp -
//  Class for drawing right of way - rules on a simple geometry
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
#include "GUIROWDrawer_SG.h"

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
GUIROWDrawer_SG::GUIROWDrawer_SG(std::vector<GUIEdge*> &edges)
    : GUIBaseROWDrawer(edges)
{
}


GUIROWDrawer_SG::~GUIROWDrawer_SG()
{
}


void
GUIROWDrawer_SG::drawLinkRules(const GUILaneWrapper &lane)
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
        glVertex2f(-1.5, visLength+.0);
        glVertex2f(-1.5, visLength+.5);
        glVertex2f(1.5, visLength+.5);
        glVertex2f(1.5, visLength+.0);
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


void
GUIROWDrawer_SG::drawArrows(const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
    if(noLinks==0) {
        return;
    }
    // draw all links
    double visLength = -lane.visLength();
    glPushMatrix();
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    const Position2D &beg = lane.getBegin();
    glTranslated(beg.x(), beg.y(), 0);
    glRotated( lane.getRotation(), 0, 0, 1 );
    for(size_t i=0; i<noLinks; i++) {
        MSLink::LinkDirection dir = lane.getLinkDirection(i);
        MSLink::LinkState state = lane.getLinkState(i);
        if(state==MSLink::LINKSTATE_TL_OFF_NOSIGNAL) {
            continue;
        }
        glBindTexture(GL_TEXTURE_2D, myTextureIDs[dir]);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0);
        glVertex2f(1.5, visLength+4.0);
        glTexCoord2f(0, 1);
        glVertex2f(1.5, visLength+1);
        glTexCoord2f(1, 0);
        glVertex2f(-1.5, visLength+4);
        glTexCoord2f(1, 1);
        glVertex2f(-1.5, visLength+1);
        glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIROWDrawer_SG.icc"
//#endif

// Local Variables:
// mode:C++
// End:


