/****************************************************************************/
/// @file    GUIJunctionDrawer.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 02.09.2003
/// @version $Id$
///
// Base class for drawing junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <guisim/GUIJunctionWrapper.h>
#include <utils/glutils/GLHelper.h>
#include "GUIJunctionDrawer.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <foreign/polyfonts/polyfonts.h>
#include <microsim/MSJunction.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


// ===========================================================================
// member method definitions
// ===========================================================================
GUIJunctionDrawer::GUIJunctionDrawer(
    std::vector<GUIJunctionWrapper*> &junctions)
        : myJunctions(junctions)
{}


GUIJunctionDrawer::~GUIJunctionDrawer()
{}


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
    for (size_t i=0; i<maxJunctions; ++i) {
        if (which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for (size_t j=0; j<32; j++, pos<<=1) {
            if ((which[i]&pos)!=0) {
                if (myShowToolTips) {
                    glPushName(myJunctions[j+(i<<5)]->getGlID());
                }
                glColor3f(0, 0, 0);
                GLHelper::drawFilledPoly(myJunctions[j+(i<<5)]->getShape(), true);
                if (myShowToolTips) {
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
    for (size_t i=0; i<maxJunctions; ++i) {
        if (which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for (size_t j=0; j<32; j++, pos<<=1) {
            if ((which[i]&pos)!=0) {
                glPushMatrix();
                Position2D p = myJunctions[j+(i<<5)]->getJunction().getPosition();
                glTranslated(p.x(), p.y(), 0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                pfSetPosition(0, 0);
                pfSetScale(settings.junctionNameSize / scale);
                glColor3f(settings.junctionNameColor.red(), settings.junctionNameColor.green(), settings.junctionNameColor.blue());
                SUMOReal w = pfdkGetStringWidth(myJunctions[j+(i<<5)]->microsimID().c_str());
                glRotated(180, 1, 0, 0);
                glTranslated(-w/2., 0.4, 0);
                pfDrawString(myJunctions[j+(i<<5)]->microsimID().c_str());
                glPopMatrix();
            }
        }
    }
}



/****************************************************************************/

