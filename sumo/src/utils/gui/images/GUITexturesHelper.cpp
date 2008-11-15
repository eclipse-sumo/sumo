/****************************************************************************/
/// @file    GUITexturesHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// Global storage for textures; manages and draws them
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
#include "GUIImageGlobals.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member variable definitions
// ===========================================================================
bool GUITexturesHelper::myWasInitialised = false;
FXApp* GUITexturesHelper::myApp = 0;
GLuint GUITexturesHelper::myTextureIDs[TEXTURE_MAX];
FXImage *GUITexturesHelper::myTextures[TEXTURE_MAX];


// ===========================================================================
// method definitions
// ===========================================================================
void
GUITexturesHelper::init(FXApp *a)
{
    myApp = a;
    myWasInitialised = false;
}


void
GUITexturesHelper::assignTextures()
{
    if (myWasInitialised) {
        return;
    }
    // check whether other textures shall be used
    myWasInitialised = true;
    if (!gAllowTextures) {
        return;
    }
    // build texture images
    glGenTextures(6, myTextureIDs);
    myTextures[MSLink::LINKDIR_STRAIGHT] = new FXXPMImage(myApp, p_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_TURN] = new FXXPMImage(myApp, pl_3_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_LEFT] = new FXXPMImage(myApp, pl_2_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_RIGHT] = new FXXPMImage(myApp, pr_2_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_PARTLEFT] = new FXXPMImage(myApp, pl_1_xpm, IMAGE_KEEP);
    myTextures[MSLink::LINKDIR_PARTRIGHT] = new FXXPMImage(myApp, pr_1_xpm, IMAGE_KEEP);
    // allocate in gl (bind)
    for (size_t i=0; i<TEXTURE_MAX; i++) {
        glBindTexture(GL_TEXTURE_2D,myTextureIDs[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     myTextures[i]->getWidth(), myTextures[i]->getHeight(), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, myTextures[i]->getData());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

unsigned int
GUITexturesHelper::add(FXImage *i)
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 i->getWidth(), i->getHeight(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, i->getData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}


void
GUITexturesHelper::drawDirectionArrow(GUITexture which, SUMOReal size)
{
    drawTexturedBox(myTextureIDs[which], size, size, -size, -size);
}


void
GUITexturesHelper::drawDirectionArrow(unsigned int which,
                                      SUMOReal sizeX1, SUMOReal sizeY1,
                                      SUMOReal sizeX2, SUMOReal sizeY2)
{
    drawTexturedBox(myTextureIDs[which], sizeX1, sizeY1, sizeX2, sizeY2);
}

void
GUITexturesHelper::drawTexturedBox(unsigned int which, SUMOReal size)
{
    drawTexturedBox(which, size, size, -size, -size);
}


void
GUITexturesHelper::drawTexturedBox(unsigned int which,
                                   SUMOReal sizeX1, SUMOReal sizeY1,
                                   SUMOReal sizeX2, SUMOReal sizeY2)
{
    if (!gAllowTextures) {
        return;
    }
    if (!myWasInitialised) {
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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, which);
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
    glEnable(GL_DEPTH_TEST);
}


void
GUITexturesHelper::close()
{
    if (!myWasInitialised) {
        // nothing to do
        return;
    }
    for (size_t i=0; i<TEXTURE_MAX; i++) {
        delete myTextures[i];
    }

}



/****************************************************************************/

