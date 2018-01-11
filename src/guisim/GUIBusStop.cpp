/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIBusStop.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 07.12.2005
/// @version $Id$
///
// A lane area vehicles can halt at (gui-version)
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
#include "GUIPerson.h"
#include "GUIBusStop.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <foreign/fontstash/fontstash.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUIBusStop.h>
#include <utils/gui/globjects/GLIncludes.h>



// ===========================================================================
// method definitions
// ===========================================================================
GUIBusStop::GUIBusStop(const std::string& id, const std::vector<std::string>& lines, MSLane& lane,
                       double frompos, double topos, const std::string name) :
    MSStoppingPlace(id, lines, lane, frompos, topos, name),
    GUIGlObject_AbstractAdd("busStop", GLO_TRIGGER, id) {
    const double offsetSign = MSNet::getInstance()->lefthand() ? -1 : 1;
    myFGShape = lane.getShape();
    myFGShape.move2side(1.65 * offsetSign);
    myFGShape = myFGShape.getSubpart(
                    lane.interpolateLanePosToGeometryPos(frompos),
                    lane.interpolateLanePosToGeometryPos(topos));
    myFGShapeRotations.reserve(myFGShape.size() - 1);
    myFGShapeLengths.reserve(myFGShape.size() - 1);
    int e = (int) myFGShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myFGShape[i];
        const Position& s = myFGShape[i + 1];
        myFGShapeLengths.push_back(f.distanceTo(s));
        myFGShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double) M_PI);
    }
    PositionVector tmp = myFGShape;
    tmp.move2side(1.5 * offsetSign);
    myFGSignPos = tmp.getLineCenter();
    myFGSignRot = 0;
    if (tmp.length() != 0) {
        myFGSignRot = myFGShape.rotationDegreeAtOffset(double((myFGShape.length() / 2.)));
        myFGSignRot -= 90;
    }
}


GUIBusStop::~GUIBusStop() {}


void
GUIBusStop::addAccess(MSLane* lane, const double pos) {
    MSStoppingPlace::addAccess(lane, pos);
    myAccessCoords.push_back(lane->getShape().positionAtOffset(pos));
}


GUIGLObjectPopupMenu*
GUIBusStop::getPopUpMenu(GUIMainWindow& app,
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
GUIBusStop::getParameterWindow(GUIMainWindow& app,
                               GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 5);
    // add items
    ret->mkItem("name", false, getMyName());
    ret->mkItem("begin position [m]", false, myBegPos);
    ret->mkItem("end position [m]", false, myEndPos);
    ret->mkItem("person number [#]", true, new FunctionBinding<GUIBusStop, int>(this, &MSStoppingPlace::getTransportableNumber));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIBusStop::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    RGBColor green(76, 170, 50, 255);
    RGBColor yellow(255, 235, 0, 255);
    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(green);
    const double exaggeration = s.addSize.getExaggeration(s);
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, exaggeration);
    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 10) {
        glPushMatrix();
        // draw the lines
        const double rotSign = MSNet::getInstance()->lefthand() ? -1 : 1;
        // Iterate over every line
        for (int i = 0; i < (int)myLines.size(); ++i) {
            // push a new matrix for every line
            glPushMatrix();
            // traslate and rotate
            glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
            glRotated(rotSign * myFGSignRot, 0, 0, 1);
            // draw line
            GLHelper::drawText(myLines[i].c_str(), Position(1.2, (double)i), .1, 1.f, RGBColor(76, 170, 50), 0, FONS_ALIGN_LEFT);
            // pop matrix for every line
            glPopMatrix();
        }
        for (std::vector<Position>::const_iterator i = myAccessCoords.begin(); i != myAccessCoords.end(); ++i) {
            GLHelper::drawBoxLine(*i, RAD2DEG(myFGSignPos.angleTo2D(*i)) - 90, myFGSignPos.distanceTo2D(*i), .05);
        }
        // draw the sign
        glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }
        glScaled(exaggeration, exaggeration, 1);
        GLHelper::drawFilledCircle((double) 1.1, noPoints);
        glTranslated(0, 0, .1);
        GLHelper::setColor(yellow);
        GLHelper::drawFilledCircle((double) 0.9, noPoints);
        if (s.scale * exaggeration >= 4.5) {
            GLHelper::drawText("H", Position(), .1, 1.6, green, myFGSignRot);
        }
        glPopMatrix();
    }
    if (s.addFullName.show && getMyName() != "") {
        GLHelper::drawText(getMyName(), myFGSignPos, GLO_MAX - getType(), s.addFullName.size / s.scale, s.addFullName.color, myFGSignRot);
    }
    glPopMatrix();
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


Boundary
GUIBusStop::getCenteringBoundary() const {
    Boundary b = myFGShape.getBoxBoundary();
    b.grow(SUMO_const_laneWidth);
    return b;
}



/****************************************************************************/

