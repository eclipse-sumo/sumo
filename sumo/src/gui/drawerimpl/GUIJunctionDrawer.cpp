//---------------------------------------------------------------------------//
//                        GUIJunctionDrawer.cpp -
//  BBase class for drawing junctions
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
// Revision 1.5  2007/01/09 12:05:51  dkrajzew
// names are now drawn after all lanes and junctions have been drawn
//
// Revision 1.4  2007/01/09 11:12:01  dkrajzew
// the names of nodes, additional structures, vehicles, edges, pois may now be shown
//
// Revision 1.3  2006/12/18 14:42:00  dkrajzew
// debugging nvwa usage
//
// Revision 1.2  2006/12/13 07:01:50  dkrajzew
// debugging building
//
// Revision 1.1  2006/12/12 12:10:40  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.7  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
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
// Revision 1.3  2003/11/26 09:36:01  dkrajzew
// documentation added
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

#include <vector>
#include <guisim/GUIJunctionWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUIJunctionDrawer.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/glutils/polyfonts.h>
#include <microsim/MSJunction.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIJunctionDrawer::GUIJunctionDrawer(
        std::vector<GUIJunctionWrapper*> &junctions)
    : myJunctions(junctions)
{
}


GUIJunctionDrawer::~GUIJunctionDrawer()
{
}


void
GUIJunctionDrawer::setGLID(bool val)
{
    myShowToolTips = val;
}


void
GUIJunctionDrawer::drawGLJunctions(size_t *which, size_t maxJunctions,
                                   GUISUMOAbstractView::JunctionColoringScheme,
                                   GUISUMOAbstractView::VisualizationSettings &settings)
{
    glLineWidth(1);
    // go through edges
    for(size_t i=0; i<maxJunctions; i++ ) {
        if(which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for(size_t j=0; j<32; j++, pos<<=1) {
            if((which[i]&pos)!=0) {
                if(myShowToolTips) {
                    glPushName(myJunctions[j+(i<<5)]->getGlID());
                }
                glColor3f(0, 0, 0);
                GLHelper::drawFilledPoly(myJunctions[j+(i<<5)]->getShape(), true);
                if(myShowToolTips) {
                    glPopName();
                }
            }
        }
    }
}

void
GUIJunctionDrawer::drawGLJunctionNames(
        size_t *which, size_t maxJunctions, SUMOReal scale,
        GUISUMOAbstractView::JunctionColoringScheme,
        GUISUMOAbstractView::VisualizationSettings &settings)
{
    glLineWidth(1);
    // go through edges
    for(size_t i=0; i<maxJunctions; i++ ) {
        if(which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for(size_t j=0; j<32; j++, pos<<=1) {
            if((which[i]&pos)!=0) {
                glPushMatrix();
                Position2D p = myJunctions[j+(i<<5)]->getJunction().getPosition();
                glTranslated(p.x(), p.y(), 0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                pfSetPosition(0, 0);
                pfSetScale(settings.junctionNameSize / scale);
                glColor3d(0, 1, 0);
                SUMOReal w = pfdkGetStringWidth(myJunctions[j+(i<<5)]->microsimID().c_str());
                glRotated(180, 1, 0, 0);
                glTranslated(-w/2., 0.4, 0);
                pfDrawString(myJunctions[j+(i<<5)]->microsimID().c_str());
                glPopMatrix();
            }
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


