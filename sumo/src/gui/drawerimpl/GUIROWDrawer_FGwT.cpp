//---------------------------------------------------------------------------//
//                        GUIROWDrawer_FGwT.cpp -
//  Class for drawing right of way-rules with full geometry
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
// Revision 1.14  2006/01/31 10:52:01  dkrajzew
// unneeded inclusions removed
//
// Revision 1.13  2006/01/11 11:54:35  dkrajzew
// reworked possible link states; new link coloring
//
// Revision 1.12  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.11  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 11:05:29  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.7  2004/11/23 10:05:22  dkrajzew
// removed some warnings and adapted the new class hierarchy
//
// Revision 1.6  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.5  2004/02/16 13:54:39  dkrajzew
// tried to patch a sometimes occuring visualisation bug
//
// Revision 1.4  2004/01/26 06:42:07  dkrajzew
// non-setting tooltip bug patched
//
// Revision 1.3  2003/12/04 13:22:05  dkrajzew
// unused file inclusion removed
//
// Revision 1.2  2003/11/12 13:45:25  dkrajzew
// visualisation of tl-logics added
//
// Revision 1.1  2003/10/15 11:35:06  dkrajzew
// old row-drawer replaced by new ones; setting of name information seems to
//  be necessary
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

#include <microsim/MSEdge.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILaneWrapper.h>
#include "GUIROWDrawer_FGwT.h"
#include <utils/geom/Position2DVector.h>
#include <utils/gui/images/GUITexturesHelper.h>

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
GUIROWDrawer_FGwT::GUIROWDrawer_FGwT(std::vector<GUIEdge*> &edges)
    : GUIBaseROWDrawer(edges)
{
}


GUIROWDrawer_FGwT::~GUIROWDrawer_FGwT()
{
}


void
GUIROWDrawer_FGwT::drawLinkRules(const GUINet &net,
                                 const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
    SUMOReal visLength = -lane.visLength();
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g.at(lane.getShape().size()-2);
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    if(noLinks==0) {
        glPushName(lane.getGlID());
        // draw a grey bar if no links are on the street
        glColor3f(0.5, 0.5, 0.5);
        glPushMatrix();
        glTranslated(end.x(), end.y(), 0);
        glRotated( rot, 0, 0, 1 );
        glBegin( GL_QUADS );
        glVertex2d(-SUMO_const_halfLaneWidth, 0.0);
        glVertex2d(-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(SUMO_const_halfLaneWidth, 0.0);
        glEnd();
        glPopMatrix();
        glPopName();
        return;
    }
    // draw all links
    SUMOReal w = SUMO_const_laneWidth / (SUMOReal) noLinks;
    SUMOReal x1 = 0;
    glPushMatrix();
    glTranslated(end.x(), end.y(), 0);
    glRotated( rot, 0, 0, 1 );
    for(size_t i=0; i<noLinks; i++) {
        SUMOReal x2 = x1 + w;
        MSLink::LinkState state = lane.getLinkState(i);
        switch(state) {
        case MSLink::LINKSTATE_TL_GREEN:
        case MSLink::LINKSTATE_TL_RED:
        case MSLink::LINKSTATE_TL_YELLOW:
        case MSLink::LINKSTATE_TL_OFF_BLINKING:
            glPushName(lane.getLinkTLID(net, i));
            break;
        case MSLink::LINKSTATE_MAJOR:
        case MSLink::LINKSTATE_MINOR:
        case MSLink::LINKSTATE_EQUAL:
        case MSLink::LINKSTATE_TL_OFF_NOSIGNAL:
        default:
            glPushName(lane.getGlID());
            break;
        }
        const RGBColor &color = myLinkColors.find(state)->second;
        glColor3d(color.red(), color.green(), color.blue());
        glBegin( GL_QUADS );
        glVertex2d(x1-SUMO_const_halfLaneWidth, 0.0);
        glVertex2d(x1-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(x2-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(x2-SUMO_const_halfLaneWidth,0.0);
        glEnd();
        glPopName();
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}


void
GUIROWDrawer_FGwT::drawArrows(const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
    if(noLinks==0) {
        return;
    }
    // draw all links
    const Position2D &end = lane.getShape().getEnd();
    const Position2D &f = lane.getShape().at(lane.getShape().size()-2);
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    glPushMatrix();
    glPushName(lane.getGlID());
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTranslated(end.x(), end.y(), 0);
    glRotated( rot, 0, 0, 1 );
    for(size_t i=0; i<noLinks; i++) {
        MSLink::LinkDirection dir = lane.getLinkDirection(i);
        MSLink::LinkState state = lane.getLinkState(i);
        if(state==MSLink::LINKSTATE_TL_OFF_NOSIGNAL||dir==MSLink::LINKDIR_NODIR) {
            continue;
        }
        GUITexturesHelper::drawDirectionArrow((GUITexture) dir,
            1.5, 4.0, -1.5, 1);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopName();
    glPopMatrix();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


