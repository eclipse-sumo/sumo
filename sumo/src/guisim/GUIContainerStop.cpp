/****************************************************************************/
/// @file    GUIContainerStop.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Wed, 01.08.2014
/// @version $Id$
///
// A lane area vehicles can halt at (gui-version)
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

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/common/ToString.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUIContainer.h"
#include "GUIContainerStop.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUIContainerStop.h>
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIContainerStop::GUIContainerStop(const std::string& id, const std::vector<std::string>& lines, MSLane& lane,
                                   SUMOReal frompos, SUMOReal topos)
    : MSStoppingPlace(id, lines, lane, frompos, topos),
      GUIGlObject_AbstractAdd("containerStop", GLO_TRIGGER, id) {
    myFGShape = lane.getShape();
    myFGShape.move2side((SUMOReal) 1.65);
    myFGShape = myFGShape.getSubpart(frompos, topos);
    myFGShapeRotations.reserve(myFGShape.size() - 1);
    myFGShapeLengths.reserve(myFGShape.size() - 1);
    int e = (int) myFGShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myFGShape[i];
        const Position& s = myFGShape[i + 1];
        myFGShapeLengths.push_back(f.distanceTo(s));
        myFGShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
    }
    PositionVector tmp = myFGShape;
    tmp.move2side(1.5);
    myFGSignPos = tmp.getLineCenter();
    myFGSignRot = 0;
    if (tmp.length() != 0) {
        myFGSignRot = myFGShape.rotationDegreeAtOffset(SUMOReal((myFGShape.length() / 2.)));
        myFGSignRot -= 90;
    }
}


GUIContainerStop::~GUIContainerStop() {}


GUIGLObjectPopupMenu*
GUIContainerStop::getPopUpMenu(GUIMainWindow& app,
                               GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIContainerStop::getParameterWindow(GUIMainWindow& app,
                                     GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 4);
    // add items
    ret->mkItem("begin position [m]", false, myBegPos);
    ret->mkItem("end position [m]", false, myEndPos);
    ret->mkItem("container number [#]", true, new FunctionBinding<GUIContainerStop, int>(this, &MSStoppingPlace::getTransportableNumber));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIContainerStop::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    RGBColor grey(177, 184, 186, 171);
    RGBColor blue(83, 89, 172, 255);
    // draw the area
    int i;
    glTranslated(0, 0, getType());
    GLHelper::setColor(blue);
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, 1.0);
    // draw details unless zoomed out to far
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    if (s.scale * exaggeration >= 10) {
        // draw the lines
        for (i = 0; i != (int)myLines.size(); ++i) {
            glPushMatrix();
            glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
            glRotated(180, 1, 0, 0);
            glRotated(myFGSignRot, 0, 0, 1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            pfSetPosition(0, 0);
            pfSetScale(1.f);
            glScaled(exaggeration, exaggeration, 1);
            glTranslated(1.2, -(double)i, 0);
            pfDrawString(myLines[i].c_str());
            glPopMatrix();
        }
        // draw the sign
        glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }
        glScaled(exaggeration, exaggeration, 1);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        glTranslated(0, 0, .1);
        GLHelper::setColor(grey);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);
        if (s.scale * exaggeration >= 4.5) {
            GLHelper::drawText("C", Position(), .1, 1.6 * exaggeration, blue, myFGSignRot);
        }
    }
    glPopMatrix();
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    for (std::vector<MSTransportable*>::const_iterator i = myWaitingTransportables.begin(); i != myWaitingTransportables.end(); ++i) {
        glTranslated(0, 1, 0); // make multiple containers viewable
        static_cast<GUIContainer*>(*i)->drawGL(s);
    }
}


Boundary
GUIContainerStop::getCenteringBoundary() const {
    Boundary b = myFGShape.getBoxBoundary();
    b.grow(20);
    return b;
}



/****************************************************************************/

