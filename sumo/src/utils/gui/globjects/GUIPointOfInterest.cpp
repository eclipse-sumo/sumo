/****************************************************************************/
/// @file    GUIPointOfInterest.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    June 2006
/// @version $Id$
///
// The GUI-version of a point of interest
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/div/GLHelper.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/gui/globjects/GLIncludes.h>
#include "GUIPointOfInterest.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIPointOfInterest::GUIPointOfInterest(const std::string& id, const std::string& type,
                                       const RGBColor& color, const Position& pos,
                                       SUMOReal layer, SUMOReal angle, const std::string& imgFile,
                                       SUMOReal width, SUMOReal height) :
    PointOfInterest(id, type, color, pos, layer, angle, imgFile, width, height),
    GUIGlObject_AbstractAdd("poi", GLO_POI, id) {
}


GUIPointOfInterest::~GUIPointOfInterest() {}


GUIGLObjectPopupMenu*
GUIPointOfInterest::getPopUpMenu(GUIMainWindow& app,
                                 GUISUMOAbstractView& parent) {

    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app, false);
    FXString t(myType.c_str());
    new FXMenuCommand(ret, "(" + t + ")", 0, 0, 0);
    new FXMenuSeparator(ret);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIPointOfInterest::getParameterWindow(GUIMainWindow&,
                                       GUISUMOAbstractView&) {
    return 0;
}


Boundary
GUIPointOfInterest::getCenteringBoundary() const {
    Boundary b;
    b.add(x(), y());
    if (myImgFile != DEFAULT_IMG_FILE) {
        b.growWidth(myHalfImgWidth);
        b.growHeight(myHalfImgHeight);
    } else {
        b.grow(3);
    }
    return b;
}


void
GUIPointOfInterest::drawGL(const GUIVisualizationSettings& s) const {
    const SUMOReal exaggeration = s.poiSize.getExaggeration(s);
    if (s.scale * (1.3 / 3.0) *exaggeration < s.poiSize.minSize) {
        return;
    }
    glPushName(getGlID());
    glPushMatrix();
    GLHelper::setColor(getColor());
    glTranslated(x(), y(), getLayer());
    glRotated(-getNaviDegree(), 0, 0, 1);

    if (myImgFile != DEFAULT_IMG_FILE) {
        int textureID = GUITexturesHelper::getTextureID(myImgFile);
        if (textureID > 0) {
            GUITexturesHelper::drawTexturedBox(textureID,
                                               -myHalfImgWidth * exaggeration, -myHalfImgHeight * exaggeration,
                                               myHalfImgWidth * exaggeration,  myHalfImgHeight * exaggeration);
        }
    } else {
        // fallback if no image is defined
        GLHelper::drawFilledCircle((SUMOReal) 1.3 * exaggeration, 16);
    }
    glPopMatrix();
    const Position namePos = Position(x() + 1.32 * exaggeration, y() + 1.32 * exaggeration);
    drawName(namePos, s.scale, s.poiName);
    if (s.poiType.show) {
        GLHelper::drawText(myType, namePos + Position(0, -0.6 * s.poiType.size / s.scale),
                           GLO_MAX, s.poiType.size / s.scale, s.poiType.color);
    }
    glPopName();
}
/****************************************************************************/

