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
// Revision 1.3  2003/08/21 12:49:02  dkrajzew
// lane2lane connection display added
//
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

#include "icons/arrows/p.xpm"
#include "icons/arrows/pl_1.xpm"
#include "icons/arrows/pl_2.xpm"
#include "icons/arrows/pl_3.xpm"
#include "icons/arrows/pr_1.xpm"
#include "icons/arrows/pr_2.xpm"
#include "icons/arrows/pr_3.xpm"




/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIBarROWRulesDrawer::GUIBarROWRulesDrawer(std::vector<GUIEdge*> &edges)
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
                        drawArrows(lane);
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
                        drawArrows(lane);
                    }
                }
            }
        }
    }
}


void
GUIBarROWRulesDrawer::initStep(/*const double & width*/)
{
    if(!myAmInitialised) {
//        myBla[0] = 10;
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
GUIBarROWRulesDrawer::initTexture(size_t no)
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

double oisOffset = 4.0;// !!!

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
        glVertex2f(x1-1.5, visLength+0.0+oisOffset);
        glVertex2f(x1-1.5, visLength+0.5+oisOffset);
        glVertex2f(x2-1.5, visLength+0.5+oisOffset);
        glVertex2f(x2-1.5, visLength+0.0+oisOffset);
        glEnd();
        x1 = x2;
        x2 += w;
    }
    glPopMatrix();
}


void
GUIBarROWRulesDrawer::drawArrows(const GUILaneWrapper &lane)
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
        MSLink::LinkDirection state = lane.getLinkDirection(i);
        glBindTexture(GL_TEXTURE_2D, myTextureIDs[state]);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0);
        glVertex2f(1.5, visLength+4.0+oisOffset);
        glTexCoord2f(0, 1);
        glVertex2f(1.5, visLength+1+oisOffset);
        glTexCoord2f(1, 0);
        glVertex2f(-1.5, visLength+4+oisOffset);
        glTexCoord2f(1, 1);
        glVertex2f(-1.5, visLength+1+oisOffset);
        glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
/*



    glVertex2f(0, 0);

    glVertex2f(0, 500);

    glVertex2f(500, 0);

    glVertex2f(500, 500);
    glEnd();*/
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIBarROWRulesDrawer.icc"
//#endif

// Local Variables:
// mode:C++
// End:


