/****************************************************************************/
/// @file    GUITexturesHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 08.03.2004
/// @version $Id$
///
// Global storage for textures; manages and draws them
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <iostream>
#include <fx.h>
#include <fx3d.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/common/MsgHandler.h>
#include "GUITexturesHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definition of static variables
// ===========================================================================
std::map<std::string, int> GUITexturesHelper::myTextures;
bool GUITexturesHelper::myAllowTextures = true;


// ===========================================================================
// method definitions
// ===========================================================================
int
GUITexturesHelper::getMaxTextureSize() {
    int max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    return max;
}


GUIGlID
GUITexturesHelper::add(FXImage* i) {
    GUIGlID id;
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
GUITexturesHelper::drawTexturedBox(unsigned int which, SUMOReal size) {
    drawTexturedBox(which, size, size, -size, -size);
}


void
GUITexturesHelper::drawTexturedBox(unsigned int which,
                                   SUMOReal sizeX1, SUMOReal sizeY1,
                                   SUMOReal sizeX2, SUMOReal sizeY2) {
    if (!myAllowTextures) {
        return;
    }
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST); // without DEPTH_TEST vehicles may be drawn below roads
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


int
GUITexturesHelper::getTextureID(const std::string& filename) {
    if (myTextures.count(filename) == 0) {
        try {
            FXImage* i = MFXImageHelper::loadImage(GUIMainWindow::getInstance()->getApp(), filename);
            if (MFXImageHelper::scalePower2(i, getMaxTextureSize())) {
                WRITE_WARNING("Scaling '" + filename + "'.");
            }
            GUIGlID id = add(i);
            delete i;
            myTextures[filename] = (int)id;
        } catch (InvalidArgument& e) {
            WRITE_ERROR("Could not load '" + filename + "'.\n" + e.what());
            myTextures[filename] = -1;
        }
    }
    return myTextures[filename];
}


void
GUITexturesHelper::clearTextures() {
    myTextures.clear();
}

/****************************************************************************/
