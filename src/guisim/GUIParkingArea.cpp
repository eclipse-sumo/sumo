/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUIParkingArea.cpp
/// @author  Mirco Sturari
/// @author  Jakob Erdmann
/// @author  Mirko Barthauer
/// @date    Tue, 19.01.2016
///
// A area where vehicles can park next to the road (gui version)
/****************************************************************************/
#include <config.h>

#include <string>
#include <foreign/fontstash/fontstash.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <guisim/GUIParkingArea.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GUINet.h"
#include "GUIEdge.h"
#include "GUIContainer.h"
#include "GUIParkingArea.h"


// ===========================================================================
// method definitions
// ===========================================================================
GUIParkingArea::GUIParkingArea(const std::string& id, const std::vector<std::string>& lines,
                               const std::vector<std::string>& badges, MSLane& lane, double frompos, double topos,
                               unsigned int capacity, double width, double length, double angle, const std::string& name,
                               bool onRoad,
                               const std::string& departPos,
                               bool lefthand) :
    MSParkingArea(id, lines, badges, lane, frompos, topos, capacity, width, length, angle, name, onRoad, departPos, lefthand),
    GUIGlObject_AbstractAdd(GLO_PARKING_AREA, id, GUIIconSubSys::getIcon(GUIIcon::PARKINGAREA)) {
    const double offsetSign = (MSGlobals::gLefthand || lefthand) ? -1 : 1;
    myShapeRotations.reserve(myShape.size() - 1);
    myShapeLengths.reserve(myShape.size() - 1);
    int e = (int) myShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myShape[i];
        const Position& s = myShape[i + 1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double) M_PI);
    }
    PositionVector tmp = myShape;
    tmp.move2side((lane.getWidth() + myWidth) * offsetSign);
    mySignPos = tmp.getLineCenter();
    mySignRot = 0;
    if (tmp.length() != 0) {
        mySignRot = myShape.rotationDegreeAtOffset(double((myShape.length() / 2.)));
        const double rotSign = MSGlobals::gLefthand ? -1 : 1;
        mySignRot -= 90 * rotSign;
    }
    myBoundary = myShape.getBoxBoundary();
    myBoundary.grow(20);
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
    buildPositionCopyEntry(ret, app);
    return ret;
}


GUIParameterTableWindow*
GUIParkingArea::getParameterWindow(GUIMainWindow& app,
                                   GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem(TL("name"), false, getMyName());
    ret->mkItem(TL("begin position [m]"), false, myBegPos);
    ret->mkItem(TL("end position [m]"), false, myEndPos);
    ret->mkItem(TL("occupancy [#]"), true, getOccupancy());
    ret->mkItem(TL("capacity [#]"), false, getCapacity());
    ret->mkItem(TL("alternatives [#]"), false, getNumAlternatives());
    ret->mkItem(TL("access badges"), false, joinToString(myAcceptedBadges, " "));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIParkingArea::drawGL(const GUIVisualizationSettings& s) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    RGBColor grey(177, 184, 186, 171);
    RGBColor blue(83, 89, 172, 255);
    RGBColor red(255, 0, 0, 255);
    RGBColor green(0, 255, 0, 255);
    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(blue);
    const double exaggeration = getExaggeration(s);
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, myWidth / 2. * MIN2(1.0, exaggeration));
    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 1) {
        // draw the lots
        glTranslated(0, 0, .1);
        // calculate shape lengt
        double ShapeLength = 0;
        for (const auto& length : myShapeLengths) {
            ShapeLength += length;
        }
        // calculate index Updater
        int indexUpdater = (int)((double)mySpaceOccupancies.size() / ShapeLength);
        // check if indexUpdater is 0
        if (indexUpdater == 0 || (myCapacity != myRoadSideCapacity)) {
            indexUpdater = 1;
        }
        // draw spaceOccupancies
        for (int i = 0; i < (int)mySpaceOccupancies.size(); i += indexUpdater) {
            GLHelper::drawSpaceOccupancies(exaggeration, mySpaceOccupancies.at(i).position, mySpaceOccupancies.at(i).rotation,
                                           mySpaceOccupancies.at(i).width, mySpaceOccupancies.at(i).length, mySpaceOccupancies.at(i).vehicle ? true : false);
        }
        GLHelper::setColor(blue);
        // draw the lines
        for (size_t i = 0; i != myLines.size(); ++i) {
            // push a new matrix for every line
            GLHelper::pushMatrix();
            // traslate and rotate
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            glRotated(180, 1, 0, 0);
            glRotated(mySignRot, 0, 0, 1);
            // draw line
            GLHelper::drawText(myLines[i].c_str(), Position(1.2, (double)i), .1, 1.f, RGBColor(76, 170, 50), 0, FONS_ALIGN_LEFT);
            // pop matrix for every line
            GLHelper::popMatrix();

        }
        // draw the sign
        glTranslated(mySignPos.x(), mySignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }
        glScaled(exaggeration, exaggeration, 1);
        GLHelper::drawFilledCircle((double) 1.1, noPoints);
        glTranslated(0, 0, .1);
        GLHelper::setColor(grey);
        GLHelper::drawFilledCircle((double) 0.9, noPoints);
        if (s.drawDetail(10, exaggeration)) {
            GLHelper::drawText("P", Position(), .1, 1.6, blue, mySignRot);
        }
    }
    GLHelper::popMatrix();
    if (s.addFullName.show(this) && getMyName() != "") {
        GLHelper::drawTextSettings(s.addFullName, getMyName(), mySignPos, s.scale, s.getTextAngle(mySignRot), GLO_MAX - getType());
    }
    GLHelper::popName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName, s.angle);
    if (myCapacity != myRoadSideCapacity) {
        // draw parking vehicles (their lane might not be within drawing range. if it is, they are drawn twice)
        myLane.getVehiclesSecure();
        for (const MSBaseVehicle* const v : myLane.getParkingVehicles()) {
            static_cast<const GUIVehicle*>(v)->drawGL(s);
        }
        myLane.releaseVehicles();
    }
}

void
GUIParkingArea::addLotEntry(double x, double y, double z,
                            double width, double length,
                            double angle, double slope) {
    MSParkingArea::addLotEntry(x, y, z, width, length, angle, slope);
    Boundary b;
    b.add(Position(x, y));
    b.grow(MAX2(width, length) + 5);
    myBoundary.add(b);
}


double
GUIParkingArea::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GUIParkingArea::getCenteringBoundary() const {
    return myBoundary;
}


const std::string
GUIParkingArea::getOptionalName() const {
    return myName;
}


/****************************************************************************/
