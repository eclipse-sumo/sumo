/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIBusStop.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Johannes Rummel
/// @date    Wed, 07.12.2005
///
// A lane area vehicles can halt at (gui-version)
/****************************************************************************/
#include <config.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/common/ToString.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSStageDriving.h>
#include "GUINet.h"
#include "GUIEdge.h"
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
GUIBusStop::GUIBusStop(const std::string& id, SumoXMLTag element, const std::vector<std::string>& lines, MSLane& lane,
                       double frompos, double topos, const std::string name, int personCapacity,
                       double parkingLength, const RGBColor& color) :
    MSStoppingPlace(id, element, lines, lane, frompos, topos, name, personCapacity, parkingLength, color),
    GUIGlObject_AbstractAdd(GLO_BUS_STOP, id) {
    const double offsetSign = MSGlobals::gLefthand ? -1 : 1;
    // see MSVehicleControl defContainerType
    myWidth = MAX2(1.0, ceil((double)personCapacity / getTransportablesAbreast()) * myTransportableDepth);
    myFGShape = lane.getShape();
    myFGShape = myFGShape.getSubpart(
                    lane.interpolateLanePosToGeometryPos(frompos),
                    lane.interpolateLanePosToGeometryPos(topos));
    myFGShape.move2side((lane.getWidth() + myWidth) * 0.45 * offsetSign);
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
    tmp.move2side(myWidth / 2 * offsetSign);
    myFGSignPos = tmp.getLineCenter();
    myFGSignRot = 0;
    if (tmp.length() != 0) {
        myFGSignRot = myFGShape.rotationDegreeAtOffset(double((myFGShape.length() / 2.)));
        myFGSignRot -= 90;
    }
}


GUIBusStop::~GUIBusStop() {}


bool
GUIBusStop::addAccess(MSLane* lane, const double pos, double length) {
    const bool added = MSStoppingPlace::addAccess(lane, pos, length);
    if (added) {
        myAccessCoords.push_back(lane->geometryPositionAtOffset(pos));
    }
    return added;
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
        new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem("name", false, getMyName());
    ret->mkItem("begin position [m]", false, myBegPos);
    ret->mkItem("end position [m]", false, myEndPos);
    ret->mkItem("lines", false, joinToString(myLines, " "));
    ret->mkItem("parking length [m]", false, (myEndPos - myBegPos) / myParkingFactor);
    const std::string transportable = (myElement == SUMO_TAG_CONTAINER_STOP ? "container" : "person");
    ret->mkItem((transportable + " capacity [#]").c_str(), false, myTransportableCapacity);
    ret->mkItem((transportable + " number [#]").c_str(), true, new FunctionBinding<GUIBusStop, int>(this, &MSStoppingPlace::getTransportableNumber));
    ret->mkItem("stopped vehicles[#]", true, new FunctionBinding<GUIBusStop, int>(this, &MSStoppingPlace::getStoppedVehicleNumber));
    ret->mkItem("last free pos[m]", true, new FunctionBinding<GUIBusStop, double>(this, &MSStoppingPlace::getLastFreePos));
    // rides-being-waited-on statistic
    std::map<std::string, int> stats;
    for (const MSTransportable* t : getTransportables()) {
        MSStageDriving* s = dynamic_cast<MSStageDriving*>(t->getCurrentStage());
        if (s != nullptr) {
            if (s->getIntendedVehicleID() != "") {
                stats[s->getIntendedVehicleID()] += 1;
            } else {
                stats[joinToString(s->getLines(), " ")] += 1;
            }
        }
    }
    if (stats.size() > 0) {
        ret->mkItem("waiting for:", false, "[#]");
        for (auto item : stats) {
            ret->mkItem(item.first.c_str(), false, toString(item.second));
        }
    }

    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIBusStop::drawGL(const GUIVisualizationSettings& s) const {
    // get colors
    RGBColor color, colorSign;
    if (myElement == SUMO_TAG_CONTAINER_STOP) {
        color = s.colorSettings.containerStopColor;
        colorSign = s.colorSettings.containerStopColorSign;
    } else if (myElement == SUMO_TAG_TRAIN_STOP) {
        color = s.colorSettings.trainStopColor;
        colorSign = s.colorSettings.trainStopColorSign;
    } else {
        color = s.colorSettings.busStopColor;
        colorSign = s.colorSettings.busStopColorSign;
    }
    // set color
    if (getColor() != RGBColor::INVISIBLE) {
        color = getColor();
    }
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(color);
    const double exaggeration = getExaggeration(s);
    const double offset = myWidth * 0.5 * MAX2(0.0, exaggeration - 1);
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, myWidth * 0.5 * exaggeration, 0, offset);
    // draw details unless zoomed out to far
    if (s.drawDetail(s.detailSettings.stoppingPlaceDetails, exaggeration)) {
        GLHelper::pushMatrix();
        // draw the lines
        const double rotSign = MSGlobals::gLefthand ? 1 : -1;
        // Iterate over every line
        const double lineAngle = s.getTextAngle(rotSign * myFGSignRot);
        RGBColor lineColor = color.changedBrightness(-51);
        const double textOffset = s.flippedTextAngle(rotSign * myFGSignRot) ? -1 : 1;
        const double textOffset2 = s.flippedTextAngle(rotSign * myFGSignRot) ? -1 : 0.3;
        for (int i = 0; i < (int)myLines.size(); ++i) {
            // push a new matrix for every line
            GLHelper::pushMatrix();
            // traslate and rotate
            glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
            glRotated(lineAngle, 0, 0, 1);
            // draw line
            GLHelper::drawText(myLines[i].c_str(), Position(1.2, i * textOffset + textOffset2), .1, 1.f, lineColor, 0, FONS_ALIGN_LEFT);
            // pop matrix for every line
            GLHelper::popMatrix();
        }
        GLHelper::setColor(color);
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
        GLHelper::setColor(colorSign);
        GLHelper::drawFilledCircle((double) 0.9, noPoints);
        if (s.drawDetail(s.detailSettings.stoppingPlaceText, exaggeration)) {
            if (myElement == SUMO_TAG_CONTAINER_STOP) {
                GLHelper::drawText("C", Position(), .1, 1.6, color, myFGSignRot);
            } else if (myElement == SUMO_TAG_TRAIN_STOP) {
                GLHelper::drawText("T", Position(), .1, 1.6, color, myFGSignRot);
            } else {
                GLHelper::drawText("H", Position(), .1, 1.6, color, myFGSignRot);
            }
        }
        GLHelper::popMatrix();
    }
    if (s.addFullName.show(this) && getMyName() != "") {
        GLHelper::drawTextSettings(s.addFullName, getMyName(), myFGSignPos, s.scale, s.getTextAngle(myFGSignRot), GLO_MAX - getType());
    }
    GLHelper::popMatrix();
    GLHelper::popName();
    drawName(myFGSignPos, s.scale, s.addName, s.angle);
}


double
GUIBusStop::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GUIBusStop::getCenteringBoundary() const {
    Boundary b = myFGShape.getBoxBoundary();
    b.grow(myWidth);
    for (const Position& p : myAccessCoords) {
        b.add(p);
    }
    return b;
}

const std::string
GUIBusStop::getOptionalName() const {
    return myName;
}

/****************************************************************************/
