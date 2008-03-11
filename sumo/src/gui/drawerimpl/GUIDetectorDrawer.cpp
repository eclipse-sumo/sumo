/****************************************************************************/
/// @file    GUIDetectorDrawer.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 14.Jan 2004
/// @version $Id$
///
// Base class for detector drawing
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

#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUIDetectorDrawer.h"
#include <guisim/GUIDetectorWrapper.h>
#include <foreign/polyfonts/polyfonts.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIDetectorDrawer::GUIDetectorDrawer(
    const std::vector<GUIGlObject_AbstractAdd*> &additionals)
        : myAdditionals(additionals)
{}


GUIDetectorDrawer::~GUIDetectorDrawer()
{}


void
GUIDetectorDrawer::setGLID(bool val)
{
    myShowToolTips = val;
}


void
GUIDetectorDrawer::drawGLDetectors(size_t *which,
                                   size_t maxDetectors,
                                   SUMOReal scale,
                                   GUISUMOAbstractView::VisualizationSettings &settings)
{
    SUMOReal upscale = settings.addExaggeration;
    initStep();
    for (size_t i=0; i<maxDetectors; ++i) {
        if (which[i]==0) {
            continue;
        }
        size_t pos = 1;
        for (size_t j=0; j<32; j++, pos<<=1) {
            if ((which[i]&pos)!=0) {
                if (myShowToolTips) {
                    glPushName(myAdditionals[j+(i<<5)]->getGlID());
                }
                myAdditionals[j+(i<<5)]->drawGL(scale, upscale);
                // draw name if wished
                if (settings.drawAddName) {
                    glPushMatrix();
                    Position2D p = myAdditionals[j+(i<<5)]->getCenteringBoundary().getCenter();
                    glTranslated(p.x(), p.y(), 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    pfSetPosition(0, 0);
                    pfSetScale(settings.addNameSize / scale);
                    SUMOReal w = pfdkGetStringWidth(myAdditionals[j+(i<<5)]->microsimID().c_str());
                    glRotated(180, 1, 0, 0);
                    glTranslated(-w/2., 0.4, 0);
                    pfDrawString(myAdditionals[j+(i<<5)]->microsimID().c_str());
                    glPopMatrix();
                }
                if (myShowToolTips) {
                    glPopName();
                }
            }
        }
    }
}


void
GUIDetectorDrawer::initStep()
{
    glLineWidth(1);
}



/****************************************************************************/

