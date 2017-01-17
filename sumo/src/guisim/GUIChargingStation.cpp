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
#include "GUIPerson.h"
#include "GUIChargingStation.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIChargingStation::GUIChargingStation(const std::string& id, MSLane& lane, SUMOReal frompos, SUMOReal topos,  SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, int chargeDelay) :
    MSChargingStation(id, lane, frompos, topos, chargingPower, efficiency, chargeInTransit, chargeDelay),
    GUIGlObject_AbstractAdd("chargingStation", GLO_TRIGGER, id) {
    myFGShape = lane.getShape();
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
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, exaggeration);

    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 10) {

        // push charging power matrix
        glPushMatrix();

        // Traslate End positionof signal
        glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);

        // Rotate 180 (Eje X -> Mirror)
        glRotated(180, 1, 0, 0);

        // Rotate again using myBlockIconRotation
        glRotated(myFGSignRot, 0, 0, 1);

        // Set poligon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // set polyfront position on 0,0
        pfSetPosition(0, 0);

        // Set polyfront scale to 1
        pfSetScale(1.f);

        // traslate matrix
        glTranslated(1.2, 0, 0);

        // draw charging power
        pfDrawString((toString(myChargingPower) + " W").c_str());

        // pop charging power matrix
        glPopMatrix();

        // draw the sign
        glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }

        glScaled(exaggeration, exaggeration, 1);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        glTranslated(0, 0, .1);

        GLHelper::setColor(yellow);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

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
