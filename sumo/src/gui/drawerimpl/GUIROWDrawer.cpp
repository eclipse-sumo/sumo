//---------------------------------------------------------------------------//
//                        GUIROWDrawer.h -
//  Draws links (mainly their right-of-way)
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
// Revision 1.3  2007/01/12 13:57:28  dkrajzew
// warnings removed
//
// Revision 1.2  2006/12/21 13:23:54  dkrajzew
// added visualization of tls/junction link indices
//
// Revision 1.1  2006/12/12 12:10:40  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.12  2006/11/16 10:50:43  dkrajzew
// warnings removed
//
// Revision 1.11  2006/03/20 07:22:56  dkrajzew
// added missing files
//
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
#include "GUIROWDrawer.h"
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/glutils/polyfonts.h>

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
GUIROWDrawer::GUIROWDrawer(std::vector<GUIEdge*> &edges)
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


GUIROWDrawer::~GUIROWDrawer()
{
}


void
GUIROWDrawer::setGLID(bool val)
{
    myShowToolTips = val;
}


void
ROWdrawAction_drawLinkNo(const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
    if(noLinks==0) {
        return;
    }

    // draw all links
    SUMOReal w = SUMO_const_laneWidth / (SUMOReal) noLinks;
    SUMOReal x1 = SUMO_const_laneWidth / (SUMOReal) 2.;
    glPushMatrix();
    glColor3d(1, 0, 0);
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    glTranslated(end.x(), end.y(), 0);
    glRotated( rot, 0, 0, 1 );
    for(size_t i=0; i<noLinks; i++) {
        SUMOReal x2 = x1 - w/2.;
        int linkNo = lane.getLinkRespondIndex(i);
        glPushMatrix();
        //glTranslated(0, veh.getLength() / 2., 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(1);
        glColor3d(1, 0, 0);
        SUMOReal tw = pfdkGetStringWidth(toString(linkNo).c_str());
        glRotated(180, 0, 1, 0);
        glTranslated(x2-tw/2., 0.5, 0);
        pfDrawString(toString(linkNo).c_str());
        glPopMatrix();
        x1 -= w;
    }
    glPopMatrix();
}


void
ROWdrawAction_drawTLSLinkNo(const GUINet &net, const GUILaneWrapper &lane)
{
    size_t noLinks = lane.getLinkNumber();
    if(noLinks==0) {
        return;
    }

    // draw all links
    SUMOReal w = SUMO_const_laneWidth / (SUMOReal) noLinks;
    SUMOReal x1 = SUMO_const_laneWidth / 2.;
    glPushMatrix();
    glColor3d(1, 0, 0);
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    glTranslated(end.x(), end.y(), 0);
    glRotated( rot, 0, 0, 1 );
    for(size_t i=0; i<noLinks; i++) {
        SUMOReal x2 = x1 - w/2.;
        int linkNo = lane.getLinkTLIndex(net, i);
        if(linkNo<0) {
            continue;
        }
        glPushMatrix();
        //glTranslated(0, veh.getLength() / 2., 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(1);
        glColor3d(1, 0, 0);
        SUMOReal tw = pfdkGetStringWidth(toString(linkNo).c_str());
        glRotated(180, 0, 1, 0);
        glTranslated(x2-tw/2., 0.5, 0);
        pfDrawString(toString(linkNo).c_str());
        glPopMatrix();
        x1 -= w;
    }
    glPopMatrix();
}


void
ROWdrawAction_drawLinkRules(const GUINet &net, const GUILaneWrapper &lane,
                            bool showToolTips, const GUIROWDrawer::LinkColorMap &lc)
{
    size_t noLinks = lane.getLinkNumber();
    const Position2DVector &g = lane.getShape();
    const Position2D &end = g.getEnd();
    const Position2D &f = g[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    if(noLinks==0) {
        if(showToolTips) {
            glPushName(lane.getGlID());
        }
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
        if(showToolTips) {
            glPopName();
        }
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
        if(showToolTips) {
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
        }
        const RGBColor &color = lc.find(state)->second;
        glColor3d(color.red(), color.green(), color.blue());
        glBegin( GL_QUADS );
        glVertex2d(x1-SUMO_const_halfLaneWidth, 0.0);
        glVertex2d(x1-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(x2-SUMO_const_halfLaneWidth, 0.5);
        glVertex2d(x2-SUMO_const_halfLaneWidth,0.0);
        glEnd();
        if(showToolTips) {
            glPopName();
        }
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}


void
ROWdrawAction_drawArrows(const GUILaneWrapper &lane, bool showToolTips)
{
    size_t noLinks = lane.getLinkNumber();
    if(noLinks==0) {
        return;
    }
    // draw all links
    const Position2D &end = lane.getShape().getEnd();
    const Position2D &f = lane.getShape()[-2];
    const Position2D &s = end;
    SUMOReal rot = (SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265;
    glPushMatrix();
    if(showToolTips) {
        glPushName(lane.getGlID());
    }
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
    if(showToolTips) {
        glPopName();
    }
    glPopMatrix();
}


void
ROWdrawAction_drawLane2LaneConnections(const GUILaneWrapper &lane,
                                       const GUIROWDrawer::LinkColorMap &lc)
{
    size_t noLinks = lane.getLinkNumber();
    for(size_t i=0; i<noLinks; i++) {
        MSLink::LinkState state = lane.getLinkState(i);
        const RGBColor &color = lc.find(state)->second;
        glColor3d(color.red(), color.green(), color.blue());
        const MSLane *connected = lane.getLinkLane(i);
        if(connected!=0) {
            glBegin(GL_LINES);
            const Position2D &p1 = lane.getShape()[-1];
            const Position2D &p2 = connected->getShape()[0];
            glVertex2f(p1.x(), p1.y());
            glVertex2f(p2.x(), p2.y());
            glEnd();
            GLHelper::drawTriangleAtEnd(Line2D(p1, p2), (SUMOReal) .4, (SUMOReal) .2);
        }
    }
}



void
GUIROWDrawer::drawGLROWs(const GUINet &net, size_t *which,
                         size_t maxEdges, SUMOReal width,
                         GUISUMOAbstractView::VisualizationSettings &settings)
{
    if(width<1.0) {
        return;
    }
    bool showLane2Lane = settings.showLane2Lane;
    bool withArrows = settings.showLinkDecals;
    bool withJunctionIndex = settings.drawLinkJunctionIndex;
    bool withTLSIndex = settings.drawLinkTLIndex;
    // initialise drawing
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
                    ROWdrawAction_drawLinkRules(net, lane, myShowToolTips, myLinkColors);
                    if(withArrows) {
                        ROWdrawAction_drawArrows(lane, myShowToolTips);
                    }
                    if(showLane2Lane) {
                        // this should be independent to the geometry:
                        //  draw from end of first to the begin of second
                        ROWdrawAction_drawLane2LaneConnections(lane, myLinkColors);
                    }
                    if(withJunctionIndex) {
                        ROWdrawAction_drawLinkNo(lane);
                    }
                    if(withTLSIndex) {
                        ROWdrawAction_drawTLSLinkNo(net, lane);
                    }
                }
            }
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


