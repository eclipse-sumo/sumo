/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIChargingStation.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
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
#include "GUIChargingStation.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <foreign/fontstash/fontstash.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// method definitions
// ===========================================================================
GUIChargingStation::GUIChargingStation(const std::string& id, MSLane& lane, double frompos, double topos,  double chargingPower, double efficiency, bool chargeInTransit, double chargeDelay) :
    MSChargingStation(id, lane, frompos, topos, chargingPower, efficiency, chargeInTransit, chargeDelay),
    GUIGlObject_AbstractAdd("chargingStation", GLO_TRIGGER, id) {
    myFGShape = lane.getShape();
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
    tmp.move2side(1.5);
    myFGSignPos = tmp.getLineCenter();
    myFGSignRot = 0;
    if (tmp.length() != 0) {
        myFGSignRot = myFGShape.rotationDegreeAtOffset(double((myFGShape.length() / 2.)));
        myFGSignRot -= 90;
    }
}


GUIChargingStation::~GUIChargingStation() {
}


GUIParameterTableWindow*
GUIChargingStation::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table items
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 6);

    // add items
    ret->mkItem("begin position [m]", false, myBegPos);
    ret->mkItem("end position [m]", false, myEndPos);
    ret->mkItem("charging power [W]", false, myChargingPower);
    ret->mkItem("charging myEfficiency []", false, myEfficiency);
    ret->mkItem("charge in transit [true/false]", false, myChargeInTransit);
    ret->mkItem("charge delay [s]", false, myChargeDelay);

    // close building
    ret->closeBuilding();
    return ret;
}


GUIGLObjectPopupMenu*
GUIChargingStation::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}

Boundary
GUIChargingStation::getCenteringBoundary() const {
    Boundary b = myFGShape.getBoxBoundary();
    b.grow(20);
    return b;
}


void
GUIChargingStation::drawGL(const GUIVisualizationSettings& s) const {
    // Draw Charging Station
    glPushName(getGlID());
    glPushMatrix();
    RGBColor blue(114, 210, 252, 255);
    RGBColor green(76, 170, 50, 255);
    RGBColor yellow(255, 235, 0, 255);
    RGBColor yellowCharge(255, 180, 0, 255);

    // draw the area depending if the vehicle is charging
    glTranslated(0, 0, getType());

    if (myChargingVehicle == true) {
        GLHelper::setColor(yellowCharge);
    } else {
        GLHelper::setColor(blue);
    }
    const double exaggeration = s.addSize.getExaggeration(s);
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, exaggeration);

    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 10) {

        // push charging power matrix
        glPushMatrix();
        // draw charging power
        GLHelper::drawText((toString(myChargingPower) + " W").c_str(), myFGSignPos + Position(1.2, 0), .1, 1.f, RGBColor(114, 210, 252), myFGSignRot, FONS_ALIGN_LEFT);
        // pop charging power matrix
        glPopMatrix();

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
            GLHelper::drawText("C", Position(), .1, 1.6, blue, myFGSignRot);
        }

        glTranslated(5, 0, 0);

    }
    glPopMatrix();
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}

/****************************************************************************/
