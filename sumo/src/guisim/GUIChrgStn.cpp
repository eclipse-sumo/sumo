/****************************************************************************/
/// @file    GUIChrgStn.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez López
/// @date    20-12-13
/// @version $Id$
///
// A lane area vehicles can halt at (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
#include <utils/geom/Line.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/common/ToString.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUIPerson.h"
#include "GUIChrgStn.h"
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
GUIChrgStn::GUIChrgStn(const std::string& id, const std::vector<std::string>& lines, MSLane& lane,
                       SUMOReal frompos, SUMOReal topos, SUMOReal new_chrgpower, SUMOReal new_efficiency, SUMOReal new_ChargeInTransit, SUMOReal new_ChargeDelay)
    : MSChrgStn(id, lines, lane, frompos, topos, new_chrgpower, new_efficiency, new_ChargeInTransit, new_ChargeDelay),
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

    chrgpower = new_chrgpower;
    efficiency = new_efficiency;
    chargeInTransit = new_ChargeInTransit;
    chargeDelay = new_ChargeDelay;
}


GUIChrgStn::~GUIChrgStn()
{}


GUIParameterTableWindow*
GUIChrgStn::getParameterWindow(GUIMainWindow& app,
                               GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 6);

    // add items
    ret->mkItem("begin position [m]", false, myBegPos);
    ret->mkItem("end position [m]", false, myEndPos);
    ret->mkItem("charging power [W]", false, chrgpower);
    ret->mkItem("charging efficiency []", false, efficiency);
    ret->mkItem("charge in transit [0/1]", false, chargeInTransit);
    ret->mkItem("charge delay [s]", false, chargeDelay);

    // close building
    ret->closeBuilding();
    return ret;
}


GUIGLObjectPopupMenu*
GUIChrgStn::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GUIChrgStn::getCenteringBoundary() const {
    Boundary b = myFGShape.getBoxBoundary();
    b.grow(20);
    return b;
}


void
GUIChrgStn::drawGL(const GUIVisualizationSettings& s) const {

    glPushName(getGlID());
    glPushMatrix();
    RGBColor blue(114, 210, 252, 255);
    RGBColor green(76, 170, 50, 255);
    RGBColor yellow(255, 235, 0, 255);
    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(blue);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, exaggeration);

    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 10) {
        // draw the sign
        glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }

        glScaled(exaggeration, exaggeration, 1);
        GLHelper::setColor(blue);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        glTranslated(0, 0, .1);

        GLHelper::setColor(yellow);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

        if (s.scale * exaggeration >= 4.5) {
            GLHelper::drawText("C", Position(), .1, 1.6, green, myFGSignRot);
        }

        /** draw the Sen function IGNORED

        //glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);

        if (myFGSignRot == 0)
            glTranslated(0, -1.5, 10);
        else
            glTranslated(0, 1.5, 10);
        //glTranslated(0, (-1)*myFGSignPos.y(), .1);
        RGBColor green(76, 170, 50, 255);



        for (double X = 0; X < 5; X+=0.01)
        {
            double Y = sin((4*X)-(PI/2))*0.8;
            glTranslated(0.01, Y, 0);
            GLHelper::drawFilledCircle(0.05, 9);
            glTranslated(0, Y*-1, 0);
        }

        glTranslated(-5, 0, 0);

        for (double X = 0; X > -5; X-=0.01)
        {
            double Y = sin((4*X)-(PI/2))*0.8;
            glTranslated(-0.01, Y, 0);
            GLHelper::drawFilledCircle(0.05, 9);
            glTranslated(0, Y*-1, 0);
        }
        */
        glTranslated(5, 0, 0);

    }
    glPopMatrix();
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    // there should be no persons on a chargingStation
    //for (std::vector<MSPerson*>::const_iterator i = myWaitingPersons.begin(); i != myWaitingPersons.end(); ++i) {
    //    glTranslated(0, 1, 0); // make multiple persons viewable
    //    static_cast<GUIPerson*>(*i)->drawGL(s);
    //}

}

/****************************************************************************/
