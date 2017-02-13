/****************************************************************************/
/// @file    GUIParkingArea.h
/// @author  Mirco Sturari
/// @author  Jakob Erdmann
/// @date    Tue, 19.01.2016
/// @version $Id$
///
// A area where vehicles can park next to the road (gui version)
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
#include "GUIParkingArea.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/GeomHelper.h>
#include <guisim/GUIParkingArea.h>
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIParkingArea::GUIParkingArea(const std::string& id, const std::vector<std::string>& lines, MSLane& lane,
                               SUMOReal frompos, SUMOReal topos, unsigned int capacity,
                               SUMOReal width, SUMOReal length, SUMOReal angle)
    : MSParkingArea(id, lines, lane, frompos, topos, capacity, width, length, angle),
      GUIGlObject_AbstractAdd("parkingArea", GLO_TRIGGER, id) {

    myShapeRotations.reserve(myShape.size() - 1);
    myShapeLengths.reserve(myShape.size() - 1);
    int e = (int) myShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myShape[i];
        const Position& s = myShape[i + 1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
    }
    PositionVector tmp = myShape;
    tmp.move2side(lane.getWidth() + myWidth);
    mySignPos = tmp.getLineCenter();
    mySignRot = 0;
    if (tmp.length() != 0) {
        mySignRot = myShape.rotationDegreeAtOffset(SUMOReal((myShape.length() / 2.)));
        mySignRot -= 90;
    }
}

GUIParkingArea::~GUIParkingArea() {}


GUIGLObjectPopupMenu*
GUIParkingArea::getPopUpMenu(GUIMainWindow& app,
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
GUIParkingArea::getParameterWindow(GUIMainWindow& app,
                                   GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 4);
    // add items
    ret->mkItem("begin position [m]", false, myBegPos);
    ret->mkItem("end position [m]", false, myEndPos);
    ret->mkItem("occupancy [#]", true, getOccupancy());
    ret->mkItem("capacity [#]", false, getCapacity());
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIParkingArea::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    RGBColor grey(177, 184, 186, 171);
    RGBColor blue(83, 89, 172, 255);
    RGBColor red(255, 0, 0, 255);
    RGBColor green(0, 255, 0, 255);
    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(blue);
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, myWidth / 2.);
    // draw details unless zoomed out to far
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    if (s.scale * exaggeration >= 10) {
        // draw the lots
        glTranslated(0, 0, .1);
        std::map<unsigned int, LotSpaceDefinition >::const_iterator i;
        for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
            glPushMatrix();
            glTranslated((*i).second.myPosition.x(), (*i).second.myPosition.y(), (*i).second.myPosition.z());
            glRotated((*i).second.myRotation, 0, 0, 1);
            Position pos = (*i).second.myPosition;
            PositionVector geom;
            SUMOReal w = (*i).second.myWidth / 2.;
            SUMOReal h = (*i).second.myLength;
            geom.push_back(Position(- w, + 0, 0.));
            geom.push_back(Position(+ w, + 0, 0.));
            geom.push_back(Position(+ w, + h, 0.));
            geom.push_back(Position(- w, + h, 0.));
            geom.push_back(Position(- w, + 0, 0.));
            /*
            geom.push_back(Position(pos.x(), pos.y(), pos.z()));
            geom.push_back(Position(pos.x() + (*l).second.myWidth, pos.y(), pos.z()));
            geom.push_back(Position(pos.x() + (*l).second.myWidth, pos.y() - (*l).second.myLength, pos.z()));
            geom.push_back(Position(pos.x(), pos.y() - (*l).second.myLength, pos.z()));
            geom.push_back(Position(pos.x(), pos.y(), pos.z()));
            */
            GLHelper::setColor((*i).second.vehicle == 0 ? green : red);
            GLHelper::drawBoxLines(geom, 0.1);
            glPopMatrix();
        }
        GLHelper::setColor(blue);
        // draw the lines
        for (size_t i = 0; i != myLines.size(); ++i) {
            glPushMatrix();
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            glRotated(180, 1, 0, 0);
            glRotated(mySignRot, 0, 0, 1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            pfSetPosition(0, 0);
            pfSetScale(1.f);
            glScaled(exaggeration, exaggeration, 1);
            glTranslated(1.2, -(double)i, 0);
            pfDrawString(myLines[i].c_str());
            glPopMatrix();
        }
        // draw the sign
        glTranslated(mySignPos.x(), mySignPos.y(), 0);
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
            GLHelper::drawText("P", Position(), .1, 1.6 * exaggeration, blue, mySignRot);
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
GUIParkingArea::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);
    return b;
}



/****************************************************************************/

