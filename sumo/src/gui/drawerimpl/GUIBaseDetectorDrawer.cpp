//---------------------------------------------------------------------------//
//                        GUIBaseDetectorDrawer.cpp -
//  Base class for detector drawing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 14.Jan 2004
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
// Revision 1.2  2004/02/16 13:54:39  dkrajzew
// tried to patch a sometimes occuring visualisation bug
//
// Revision 1.1  2004/01/26 06:39:41  dkrajzew
// visualisation of e3-detectors added; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <gui/GUISUMOAbstractView.h>
#include "GUIBaseDetectorDrawer.h"
#include <guisim/GUIDetectorWrapper.h>

#include <gui/icons/arrows/p.xpm>

#include <qimage.h>
#include <qgl.h>


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
GUIBaseDetectorDrawer::drawGLDetectors(size_t *which,
                                       size_t maxDetectors,
                                       double scale)
{
    initStep();
    myDrawGLDetectors(which, maxDetectors, scale);
}


void
GUIBaseDetectorDrawer::initStep()
{
    if(!myAmInitialised) {
        glGenTextures(1, myTextureIDs);
        QImage use = QGLWidget::convertToGLFormat(QImage(p_xpm));
        glBindTexture(GL_TEXTURE_2D, myTextureIDs[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            use.width(), use.height(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, use.bits() );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, 0);
        myAmInitialised = true;
    }
    glLineWidth(1);
}


void
GUIBaseDetectorDrawer::drawArrow(double size)
{
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
    glBindTexture(GL_TEXTURE_2D, myTextureIDs[0]);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 0);
    glVertex2f(size, size);
    glTexCoord2f(0, 1);
    glVertex2f(size, -size);
    glTexCoord2f(1, 0);
    glVertex2f(-size, size);
    glTexCoord2f(1, 1);
    glVertex2f(-size, -size);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
