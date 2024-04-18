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
/// @file    GUIChargingStation.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @author  Mirko Barthauer
/// @date    20-12-13
///
// A lane area vehicles can halt at (gui-version)
/****************************************************************************/
#include <config.h>

#include <foreign/fontstash/fontstash.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSParkingArea.h>
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
#include "GUIPerson.h"
#include "GUIChargingStation.h"


// ===========================================================================
// method definitions
// ===========================================================================
GUIChargingStation::GUIChargingStation(const std::string& id, MSLane& lane, double frompos, double topos, const std::string& name,
                                       double chargingPower, double efficiency, bool chargeInTransit, SUMOTime chargeDelay,
                                       const std::string& chargeType, SUMOTime waitingTime) :
    MSChargingStation(id, lane, frompos, topos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, chargeType, waitingTime),
    GUIGlObject_AbstractAdd(GLO_CHARGING_STATION, id, GUIIconSubSys::getIcon(GUIIcon::CHARGINGSTATION)) {
    initAppearance(lane, frompos, topos);
}


GUIChargingStation::GUIChargingStation(const std::string& id, MSParkingArea* parkingArea, const std::string& name,
                                       double chargingPower, double efficiency, bool chargeInTransit, SUMOTime chargeDelay,
                                       const std::string& chargeType, SUMOTime waitingTime) :
    MSChargingStation(id, parkingArea, name, chargingPower, efficiency, chargeInTransit, chargeDelay, chargeType, waitingTime),
    GUIGlObject_AbstractAdd(GLO_CHARGING_STATION, id, GUIIconSubSys::getIcon(GUIIcon::CHARGINGSTATION)) {
    initAppearance(const_cast<MSLane&>(parkingArea->getLane()), parkingArea->getBeginLanePosition(), parkingArea->getEndLanePosition());
}


GUIChargingStation::~GUIChargingStation() {
}


GUIParameterTableWindow*
GUIChargingStation::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table items
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem(TL("name"), false, getMyName());
    ret->mkItem(TL("begin position [m]"), false, myBegPos);
    ret->mkItem(TL("end position [m]"), false, myEndPos);
    ret->mkItem(TL("stopped vehicles [#]"), true, new FunctionBinding<GUIChargingStation, int>(this, &MSStoppingPlace::getStoppedVehicleNumber));
    ret->mkItem(TL("last free pos [m]"), true, new FunctionBinding<GUIChargingStation, double>(this, &MSStoppingPlace::getLastFreePos));
    ret->mkItem(TL("charging power [W]"), false, myChargingPower);
    ret->mkItem(TL("charging efficiency [#]"), false, myEfficiency);
    ret->mkItem(TL("charge in transit [true/false]"), false, myChargeInTransit);
    ret->mkItem(TL("charge delay [s]"), false, myChargeDelay);
    ret->mkItem(TL("charge type"), false, myChargeType);
    ret->mkItem(TL("waiting time [s]"), false, myWaitingTime);
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
    buildPositionCopyEntry(ret, app);
    return ret;
}


double
GUIChargingStation::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
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
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();

    // draw the area depending if the vehicle is charging
    glTranslated(0, 0, getType());

    // set color depending if charging station is charging
    if (myChargingVehicle) {
        GLHelper::setColor(s.colorSettings.chargingStationColorCharge);
    } else {
        GLHelper::setColor(s.colorSettings.chargingStationColor);
    }
    const double exaggeration = getExaggeration(s);
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, MIN2(1.0, exaggeration));

    // draw details unless zoomed out to far
    if (s.drawDetail(10, exaggeration)) {

        // push charging power matrix
        GLHelper::pushMatrix();
        // translate and rotate
        glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
        glRotated(-myFGSignRot, 0, 0, 1);
        // draw charging power
        GLHelper::drawText((toString(myChargingPower) + " W").c_str(), Position(1.2, 0), .1, 1.f, s.colorSettings.chargingStationColor, 0, FONS_ALIGN_LEFT);
        // pop charging power matrix
        GLHelper::popMatrix();

        GLHelper::pushMatrix();
        // draw the sign
        glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }

        glScaled(exaggeration, exaggeration, 1);
        GLHelper::drawFilledCircle((double) 1.1, noPoints);
        glTranslated(0, 0, .1);

        GLHelper::setColor(s.colorSettings.chargingStationColorSign);
        GLHelper::drawFilledCircle((double) 0.9, noPoints);

        if (s.drawDetail(10, exaggeration)) {
            GLHelper::drawText("C", Position(), .1, 1.6, s.colorSettings.chargingStationColor, myFGSignRot);
        }

        glTranslated(5, 0, 0);
        GLHelper::popMatrix();

    }
    if (s.addFullName.show(this) && getMyName() != "") {
        GLHelper::drawTextSettings(s.addFullName, getMyName(), myFGSignPos, s.scale, s.getTextAngle(myFGSignRot), GLO_MAX - getType());
    }
    GLHelper::popMatrix();
    GLHelper::popName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName, s.angle);
}


void
GUIChargingStation::initAppearance(MSLane& lane, double frompos, double topos) {
    myFGShape = lane.getShape();
    myFGShape = myFGShape.getSubpart(
                    lane.interpolateLanePosToGeometryPos(frompos),
                    lane.interpolateLanePosToGeometryPos(topos));
    myFGShapeRotations.reserve(myFGShape.size() - 1);
    myFGShapeLengths.reserve(myFGShape.size() - 1);
    int e = (int)myFGShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myFGShape[i];
        const Position& s = myFGShape[i + 1];
        myFGShapeLengths.push_back(f.distanceTo(s));
        myFGShapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
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


const std::string
GUIChargingStation::getOptionalName() const {
    return myName;
}


/****************************************************************************/
