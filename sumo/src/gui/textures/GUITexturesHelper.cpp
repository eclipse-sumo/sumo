//                        GUITexturesHelper.cpp -
//  Helper for texture loading and usage
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 08.03.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2004/04/02 11:00:34  dkrajzew
// first try to implement an option for diabling textures
//
// Revision 1.1  2004/03/19 12:41:53  dkrajzew
// porting to FOX
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include <fx.h>
#include <fx3d.h>
#include "GUITexturesHelper.h"
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>

#include "p.xpm"
#include "pl_1.xpm"
#include "pl_2.xpm"
#include "pl_3.xpm"
#include "pr_1.xpm"
#include "pr_2.xpm"
#include "pr_3.xpm"
#include <utils/fonts/arial11.h>
#include <utils/fonts/arial10.h>
#include <utils/fonts/arial9.h>
#include <utils/fonts/arial8.h>
#include <utils/fonts/arial7.h>
#include <utils/fonts/arial6.h>
#include <utils/fonts/arial5.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * static member variable definitions
 * ======================================================================= */
bool GUITexturesHelper::myWasInitialised = false;
GUIApplicationWindow *GUITexturesHelper::myApp = 0;
size_t GUITexturesHelper::myTextureIDs[TEXTURE_MAX];
FXImage *GUITexturesHelper::myTextures[TEXTURE_MAX];
FontStorage GUITexturesHelper::myFonts;
LFontRenderer GUITexturesHelper::myFontRenderer;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
GUITexturesHelper::init(GUIApplicationWindow *a)
{
    myApp = a;
    myWasInitialised = false;
}


void
GUITexturesHelper::assignTextures()
{
    if(myWasInitialised||!gAllowTextures) {
        return;
    }
    // initialise font drawing
    myFonts.add("std11", arial11);
    myFonts.add("std10", arial10);
    myFonts.add("std9", arial9);
    myFonts.add("std8", arial8);
    myFonts.add("std7", arial7);
    myFonts.add("std6", arial6);
    myFonts.add("std5", arial5);
    myFontRenderer.add(myFonts.get("std11"));
    myFontRenderer.add(myFonts.get("std10"));
    myFontRenderer.add(myFonts.get("std9"));
    myFontRenderer.add(myFonts.get("std8"));
    myFontRenderer.add(myFonts.get("std7"));
    myFontRenderer.add(myFonts.get("std6"));
    myFontRenderer.add(myFonts.get("std5"));
    // build texture images
    glGenTextures(6, myTextureIDs);
    myTextures[MSLink::LINKDIR_STRAIGHT] =
        new FXXPMImage(myApp->getApp(), p_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_TURN] =
        new FXXPMImage(myApp->getApp(), pl_3_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_LEFT] =
        new FXXPMImage(myApp->getApp(), pl_2_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_RIGHT] =
        new FXXPMImage(myApp->getApp(), pr_2_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_PARTLEFT] =
        new FXXPMImage(myApp->getApp(), pl_1_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_PARTRIGHT] =
        new FXXPMImage(myApp->getApp(), pr_1_xpm, IMAGE_KEEP);
    // allocate in gl (bind)
    for(size_t i=0; i<TEXTURE_MAX; i++) {
        glBindTexture(GL_TEXTURE_2D,myTextureIDs[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            myTextures[i]->getWidth(), myTextures[i]->getHeight(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, myTextures[i]->getData() );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    myWasInitialised = true;
}


void
GUITexturesHelper::drawTexturedBox(GUITexture which, double size)
{
    drawTexturedBox(which, size, size, -size, -size);
}


void
GUITexturesHelper::drawTexturedBox(GUITexture which,
                                   double sizeX1, double sizeY1,
                                   double sizeX2, double sizeY2)
{
    if(!gAllowTextures) {
        return;
    }
    if(!myWasInitialised) {
        assignTextures();
    }
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
    glBindTexture(GL_TEXTURE_2D, myTextureIDs[which]);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 1);
    glVertex2f(sizeX1, sizeY1);
    glTexCoord2f(0, 0);
    glVertex2f(sizeX1, sizeY2);
    glTexCoord2f(1, 1);
    glVertex2f(sizeX2, sizeY1);
    glTexCoord2f(1, 0);
    glVertex2f(sizeX2, sizeY2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}


LFontRenderer &
GUITexturesHelper::getFontRenderer()
{
    if(!myWasInitialised) {
        assignTextures();
    }
    return myFontRenderer;
}


void
GUITexturesHelper::close()
{
    if(!myWasInitialised) {
        // nothing to do
        return;
    }
    for(size_t i=0; i<TEXTURE_MAX; i++) {
        delete myTextures[i];
    }

}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




