/****************************************************************************/
/// @file    MSParkingArea.h
/// @author  Mirco Sturari
/// @date    Tue, 19.01.2016
/// @version $Id: MSParkingArea.h 19388 2015-11-19 21:33:01Z behrisch $
///
// A area where vehicles can park next to the road (gui version)
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

    // if width and length of lot rectangle is not specified set to a default value
    if (width == 0) width = lane.getWidth();
    if (length == 0) length = getSpaceDim();
    
    myFGShape = lane.getShape();
    myFGWidth = width;
    myFGLength = length;
    myFGAngle = angle;
    myFGShape.move2side(lane.getWidth() / 2. + myFGWidth / 2.);
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
    tmp.move2side(lane.getWidth() + myFGWidth);
    myFGSignPos = tmp.getLineCenter();
    myFGSignRot = 0;
    if (tmp.length() != 0) {
        myFGSignRot = myFGShape.rotationDegreeAtOffset(SUMOReal((myFGShape.length() / 2.)));
        myFGSignRot -= 90;
    }
    // If not specified put angle and position relative to lane
    if (capacity > 0) {
        for (unsigned int i = 1; i <= capacity; ++i) {
            const Position& f = myFGShape.positionAtOffset(length * (i - 1));
            const Position& s = myFGShape.positionAtOffset(length * (i));
            SUMOReal lot_angle = ((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI) + angle;
            Position pos = myFGShape.positionAtOffset(length * ((double)i - 0.5));
            mySpaceOccupancies[i].myFGRotation = lot_angle;
            mySpaceOccupancies[i].myFGPosition = pos;
            mySpaceOccupancies[i].myFGLength = length;
            mySpaceOccupancies[i].myFGWidth = width;
        }
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
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, myFGWidth / 2.);
    // draw details unless zoomed out to far
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    if (s.scale * exaggeration >= 10) {
        // draw the lots
        glTranslated(0, 0, .1);
        std::map<unsigned int, LotSpaceDefinition > mySpaces = mySpaceOccupancies;
        std::map<unsigned int, LotSpaceDefinition >::iterator i;
        for (i = mySpaces.begin(); i != mySpaces.end(); i++) {
            glPushMatrix();
            glTranslated((*i).second.myFGPosition.x(), (*i).second.myFGPosition.y(), (*i).second.myFGPosition.z());
            glRotated((*i).second.myFGRotation, 0, 0, 1);
            Position pos = (*i).second.myFGPosition;
            PositionVector geom;
            SUMOReal w = (*i).second.myFGWidth / 2.;
            SUMOReal h = (*i).second.myFGLength / 2.;
            geom.push_back(Position(- w, + h, 0.));
            geom.push_back(Position(+ w, + h, 0.));
            geom.push_back(Position(+ w, - h, 0.));
            geom.push_back(Position(- w, - h, 0.));
            geom.push_back(Position(- w, + h, 0.));
            /*
            geom.push_back(Position(pos.x(), pos.y(), pos.z()));
            geom.push_back(Position(pos.x() + (*l).second.myFGWidth, pos.y(), pos.z()));
            geom.push_back(Position(pos.x() + (*l).second.myFGWidth, pos.y() - (*l).second.myFGLength, pos.z()));
            geom.push_back(Position(pos.x(), pos.y() - (*l).second.myFGLength, pos.z()));
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
            GLHelper::drawText("P", Position(), .1, 1.6 * exaggeration, blue, myFGSignRot);
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
    Boundary b = myFGShape.getBoxBoundary();
    b.grow(20);
    return b;
}



/****************************************************************************/

