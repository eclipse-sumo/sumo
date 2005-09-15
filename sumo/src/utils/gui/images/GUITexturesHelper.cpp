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
// Revision 1.3  2005/09/15 12:19:55  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:51  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.3  2004/04/23 12:34:01  dkrajzew
// some debugging of the texture-disabling option (still in work)
//
// Revision 1.2  2004/04/02 11:00:34  dkrajzew
// first try to implement an option for diabling textures
//
// Revision 1.1  2004/03/19 12:41:53  dkrajzew
// porting to FOX
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <cassert>
#include <iostream>
#include <fx.h>
#include <fx3d.h>
#include "GUITexturesHelper.h"

#include "p.xpm"
#include "pl_1.xpm"
#include "pl_2.xpm"
#include "pl_3.xpm"
#include "pr_1.xpm"
#include "pr_2.xpm"
#include "pr_3.xpm"
#include "GUIImageGlobals.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


using namespace std;

/* =========================================================================
 * static member variable definitions
 * ======================================================================= */
bool GUITexturesHelper::myWasInitialised = false;
FXApp* GUITexturesHelper::myApp = 0;
size_t GUITexturesHelper::myTextureIDs[TEXTURE_MAX];
FXImage *GUITexturesHelper::myTextures[TEXTURE_MAX];


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
GUITexturesHelper::init(FXApp *a)
{
    myApp = a;
    myWasInitialised = false;
}


void
GUITexturesHelper::assignTextures()
{
    if(myWasInitialised) {
        return;
    }
    // check whether other textures shall be used
	myWasInitialised = true;
    if(!gAllowTextures) {
        return;
    }
    // build texture images
    glGenTextures(6, myTextureIDs);
    myTextures[MSLink::LINKDIR_STRAIGHT] =
        new FXXPMImage(myApp, p_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_TURN] =
        new FXXPMImage(myApp, pl_3_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_LEFT] =
        new FXXPMImage(myApp, pl_2_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_RIGHT] =
        new FXXPMImage(myApp, pr_2_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_PARTLEFT] =
        new FXXPMImage(myApp, pl_1_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_PARTRIGHT] =
        new FXXPMImage(myApp, pr_1_xpm, IMAGE_KEEP);
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
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, myTextureIDs[which]);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 1);
    glVertex2d(sizeX1, sizeY1);
    glTexCoord2f(0, 0);
    glVertex2d(sizeX1, sizeY2);
    glTexCoord2f(1, 1);
    glVertex2d(sizeX2, sizeY1);
    glTexCoord2f(1, 0);
    glVertex2d(sizeX2, sizeY2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
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




