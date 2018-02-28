/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <utils/gui/globjects/GLIncludes.h>
#include "GUIPointOfInterest.h"


// ===========================================================================
// method definitions
// ===========================================================================

GUIPointOfInterest::GUIPointOfInterest(const std::string& id, const std::string& type,
                                       const RGBColor& color, const Position& pos, bool geo,
                                       const std::string& lane, double posOverLane, double posLat,
                                       double layer, double angle, const std::string& imgFile,
                                       bool relativePath, double width, double height) :
    PointOfInterest(id, type, color, pos, geo, lane, posOverLane, posLat, layer, angle, imgFile, relativePath, width, height),
    GUIGlObject_AbstractAdd("poi", GLO_POI, id) {
}


GUIPointOfInterest::~GUIPointOfInterest() {}


GUIGLObjectPopupMenu*
GUIPointOfInterest::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build shape header
    buildShapePopupOptions(app, ret, getShapeType());
    return ret;
}


GUIParameterTableWindow*
GUIPointOfInterest::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 3 + (int)getMap().size());
    // add items
    ret->mkItem("type", false, getShapeType());
    ret->mkItem("layer", false, getShapeLayer());
    ret->closeBuilding(this);
    return ret;
}


Boundary
GUIPointOfInterest::getCenteringBoundary() const {
    Boundary b;
    b.add(x(), y());
    if (getShapeImgFile() != DEFAULT_IMG_FILE) {
        b.growWidth(myHalfImgWidth);
        b.growHeight(myHalfImgHeight);
    } else {
        b.grow(3);
    }
    return b;
}


void
GUIPointOfInterest::drawGL(const GUIVisualizationSettings& s) const {
    const double exaggeration = s.poiSize.getExaggeration(s);
    if (s.scale * (1.3 / 3.0) *exaggeration < s.poiSize.minSize) {
        return;
    }
    glPushName(getGlID());
    glPushMatrix();
    // set color depending of selection
    if (gSelected.isSelected(GLO_POI, getGlID())) {
        GLHelper::setColor(RGBColor(0, 0, 204));
    } else {
        GLHelper::setColor(getShapeColor());
    }
    glTranslated(x(), y(), getShapeLayer());
    glRotated(-getShapeNaviDegree(), 0, 0, 1);

    if (getShapeImgFile() != DEFAULT_IMG_FILE) {
        int textureID = GUITexturesHelper::getTextureID(getShapeImgFile());
        if (textureID > 0) {
            GUITexturesHelper::drawTexturedBox(textureID,
                                               -myHalfImgWidth * exaggeration, -myHalfImgHeight * exaggeration,
                                               myHalfImgWidth * exaggeration,  myHalfImgHeight * exaggeration);
        }
    } else {
        // fallback if no image is defined
        GLHelper::drawFilledCircle((double) 1.3 * exaggeration, 16);
    }
    glPopMatrix();
    const Position namePos = *this;
    drawName(namePos, s.scale, s.poiName);
    if (s.poiType.show) {
        GLHelper::drawText(getShapeType(), namePos + Position(0, -0.6 * s.poiType.size / s.scale),
                           GLO_MAX, s.poiType.size / s.scale, s.poiType.color);
    }
    glPopName();
}

/****************************************************************************/

