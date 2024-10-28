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
/// @file    GUIMEVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
#include <config.h>

#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/devices/MSVehicleDevice.h>
#include <guisim/GUILane.h>

#include "GUIMEVehicle.h"


// ===========================================================================
// method definitions
// ===========================================================================

GUIMEVehicle::GUIMEVehicle(SUMOVehicleParameter* pars, ConstMSRoutePtr route,
                           MSVehicleType* type, const double speedFactor) :
    MEVehicle(pars, route, type, speedFactor),
    GUIBaseVehicle((MSBaseVehicle&) * this) {
}


GUIMEVehicle::~GUIMEVehicle() { }


GUIParameterTableWindow*
GUIMEVehicle::getParameterWindow(GUIMainWindow& app,
                                 GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem("edge [id]", true, new FunctionBindingString<GUIMEVehicle>(this, &GUIMEVehicle::getEdgeID));
    ret->mkItem("segment [#]", true,  new FunctionBinding<GUIMEVehicle, int>(this, &GUIMEVehicle::getSegmentIndex));
    ret->mkItem("queue [#]", true,  new FunctionBinding<GUIMEVehicle, int>(this, &GUIMEVehicle::getQueIndex));
    ret->mkItem("position [m]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getSpeed));
    ret->mkItem("angle [degree]", true, new FunctionBinding<GUIMEVehicle, double>(this, &GUIBaseVehicle::getNaviDegree));
    ret->mkItem("waiting time [s]", true,
                new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getWaitingSeconds));
    ret->mkItem("speed factor", false, getChosenSpeedFactor());
    //ret->mkItem("time gap [s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &MSVehicle::getTimeGap));
    //ret->mkItem("waiting time [s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &MSVehicle::getWaitingSeconds));
    //ret->mkItem("impatience", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &MSVehicle::getImpatience));
    //ret->mkItem("last lane change [s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, time2string(getParameter().depart));
    ret->mkItem("depart delay [s]", false, time2string(getDepartDelay()));
    ret->mkItem("odometer [m]", true,
                new FunctionBinding<GUIMEVehicle, double>(this, &MSBaseVehicle::getOdometer));
    if (getParameter().repetitionNumber < std::numeric_limits<int>::max()) {
        ret->mkItem("remaining [#]", false, (int) getParameter().repetitionNumber - getParameter().repetitionsDone);
    }
    if (getParameter().repetitionOffset > 0) {
        ret->mkItem("insertion period [s]", false, time2string(getParameter().repetitionOffset));
    }
    if (getParameter().repetitionProbability > 0) {
        ret->mkItem("insertion probability", false, getParameter().repetitionProbability);
    }
    if (getParameter().poissonRate > 0) {
        ret->mkItem(TL("poisson rate"), false, getParameter().poissonRate);
    }
    //ret->mkItem("stop info", false, getStopInfo());
    ret->mkItem("line", false, myParameter->line);
    //ret->mkItem("CO2 [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getCO2Emissions));
    //ret->mkItem("CO [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getCOEmissions));
    //ret->mkItem("HC [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getHCEmissions));
    //ret->mkItem("NOx [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getNOxEmissions));
    //ret->mkItem("PMx [mg/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getPMxEmissions));
    //ret->mkItem("fuel [ml/s]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getFuelConsumption));
    //ret->mkItem("noise (Harmonoise) [dB]", true,
    //            new FunctionBinding<GUIMEVehicle, double>(this, &GUIMEVehicle::getHarmonoise_NoiseEmissions));
    ret->mkItem("devices", false, getDeviceDescription());
    //ret->mkItem("persons", true,
    //            new FunctionBinding<GUIMEVehicle, int>(this, &GUIMEVehicle::getPersonNumber));
    //ret->mkItem("containers", true,
    //            new FunctionBinding<GUIMEVehicle, int>(this, &GUIMEVehicle::getContainerNumber));
    // meso specific values
    ret->mkItem("event time [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getEventTimeSeconds));
    ret->mkItem("entry time [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getLastEntryTimeSeconds));
    ret->mkItem("block time [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getBlockTimeSeconds));
    ret->mkItem("link penalty [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getCurrentLinkPenaltySeconds));
    ret->mkItem("stop time [s]", true, new FunctionBinding<GUIMEVehicle, double>(this, &MEVehicle::getCurrentStoppingTimeSeconds));
    // close building
    ret->closeBuilding(&getParameter());
    return ret;
}


GUIParameterTableWindow*
GUIMEVehicle::getTypeParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, "vType:" + myType->getID());
    ret->mkItem("length [m]", false, myType->getLength());
    ret->mkItem("width [m]", false, myType->getWidth());
    ret->mkItem("height [m]", false, myType->getHeight());
    ret->mkItem("minGap [m]", false, myType->getMinGap());
    ret->mkItem("vehicle class", false, SumoVehicleClassStrings.getString(myType->getVehicleClass()));
    ret->mkItem("emission class", false, PollutantsInterface::getName(myType->getEmissionClass()));
    ret->mkItem("mass [kg]", false, myType->getMass());
    ret->mkItem("guiShape", false, getVehicleShapeName(myType->getGuiShape()));
    ret->mkItem("maximum speed [m/s]", false, getMaxSpeed());
    ret->mkItem("speedFactor", false, myType->getParameter().speedFactor.toStr(gPrecision));
    ret->mkItem("person capacity", false, myType->getPersonCapacity());
    ret->mkItem("container capacity", false, myType->getContainerCapacity());
    ret->closeBuilding(&(myType->getParameter()));
    return ret;
}


void
GUIMEVehicle::drawAction_drawCarriageClass(const GUIVisualizationSettings& /* s */, bool /* asImage */) const {
    GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(getVType().getWidth(), getVType().getLength());
}


double
GUIMEVehicle::getColorValue(const GUIVisualizationSettings& /* s */, int activeScheme) const {
    switch (activeScheme) {
        case 8:
            return getSpeed();
        case 9:
            return 0; // by actionStep
        case 10:
            return getWaitingSeconds();
        case 11:
            return 0; // getAccumulatedWaitingSeconds
        case 12:
            return 0; // invalid getLastLaneChangeOffset();
        case 13:
            return getSegment()->getEdge().getVehicleMaxSpeed(this);
        case 14:
            return 0; // invalid getEmissions<PollutantsInterface::CO2>();
        case 15:
            return 0; // invalid getEmissions<PollutantsInterface::CO>();
        case 16:
            return 0; // invalid getEmissions<PollutantsInterface::PM_X>();
        case 17:
            return 0; // invalid getEmissions<PollutantsInterface::NO_X>();
        case 18:
            return 0; // invalid getEmissions<PollutantsInterface::HC>();
        case 19:
            return 0; // invalid getEmissions<PollutantsInterface::FUEL>();
        case 20:
            return 0; // invalid getHarmonoise_NoiseEmissions();
        case 21: // reroute number
            if (getNumberReroutes() == 0) {
                return -1;
            }
            return getNumberReroutes();
        case 22:
            return gSelected.isSelected(GLO_VEHICLE, getGlID());
        case 23:
            return 0; // invalid getBestLaneOffset();
        case 24:
            return 0; // invalid getAcceleration();
        case 25:
            return 0; // invalid getTimeGapOnLane();
        case 26:
            return STEPS2TIME(getDepartDelay());
        case 27:
            return 0; // electricityConsumption
        case 28:
            return 0; // timeLossSeconds
        case 29:
            return 0; // getSpeedLat
    }
    return 0;
}



void
GUIMEVehicle::drawRouteHelper(const GUIVisualizationSettings& s, ConstMSRoutePtr r, bool future, bool noLoop, const RGBColor& col) const {
    const double exaggeration = getExaggeration(s);
    MSRouteIterator start = future ? myCurrEdge : r->begin();
    MSRouteIterator i = start;
    std::map<const MSLane*, int> repeatLane; // count repeated occurrences of the same edge
    const double textSize = s.vehicleName.size / s.scale;
    const int indexDigits = (int)toString(r->size()).size();
    const bool s2 = s.secondaryShape;
    for (; i != r->end(); ++i) {
        const GUILane* lane = static_cast<GUILane*>((*i)->getLanes()[0]);
        GLHelper::drawBoxLines(lane->getShape(s2), lane->getShapeRotations(s2), lane->getShapeLengths(s2), exaggeration);
        if (s.showRouteIndex) {
            std::string label = toString((int)(i - myCurrEdge));
            const double laneAngle = lane->getShape(s2).angleAt2D(0);
            Position pos = lane->getShape(s2).front() - Position(0, textSize * repeatLane[lane]) + Position(
                               (laneAngle >= -0.25 * M_PI && laneAngle < 0.75 * M_PI ? 1 : -1) * 0.4 * indexDigits * textSize, 0);
            //GLHelper::drawText(label, pos, 1.0, textSize, s.vehicleName.color);
            GLHelper::drawTextSettings(s.vehicleName, label, pos, s.scale, s.angle, 1.0);
        }
        if (noLoop && i != start && (*i) == (*start)) {
            break;
        }
        repeatLane[lane]++;
    }
    drawStopLabels(s, noLoop, col);
    drawParkingInfo(s);
}


double
GUIMEVehicle::getLastLaneChangeOffset() const {
    // @todo possibly we could compute something reasonable here
    return 0;
}


std::string
GUIMEVehicle::getStopInfo() const {
    std::string result = "";
    if (isParking()) {
        result += "parking";
    } else if (isStopped()) {
        result += "stopped";
    } else {
        return "";
    }
    return result;
}

std::string
GUIMEVehicle::getEdgeID() const {
    return getEdge()->getID();
}

int
GUIMEVehicle::getSegmentIndex() const {
    return getSegment() != nullptr ? getSegment()->getIndex() : -1;
}


void
GUIMEVehicle::selectBlockingFoes() const {
    // @todo possibly we could compute something reasonable here
}


double
GUIMEVehicle::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.vehicleSize.getExaggeration(s, this);
}


Boundary
GUIMEVehicle::getCenteringBoundary() const {
    // getPosition returns the start of the first lane, so we do not use it here
    getEdge()->lock();
    const double curTime = SIMTIME;
    double vehiclePosition = 0.;
    const MESegment* const segment = getSegment();
    const int queIdx = getQueIndex();
    if (segment != nullptr && queIdx != MESegment::PARKING_QUEUE) {
        vehiclePosition = segment->getLength();
        const std::vector<MEVehicle*>& queue = segment->getQueue(queIdx);
        for (auto it = queue.rbegin(); it != queue.rend(); ++it) {
            const MEVehicle* const v = *it;
            const double intendedLeave = MIN2(v->getEventTimeSeconds(), v->getBlockTimeSeconds());
            const double entry = v->getLastEntryTimeSeconds();
            const double offset = segment->getLength() * (curTime - entry) / (intendedLeave - entry);
            if (offset < vehiclePosition) {
                vehiclePosition = offset;
            }
            if (v == this) {
                break;
            }
            vehiclePosition -= v->getVehicleType().getLengthWithGap();
        }
    }
    Boundary b;
    const MSLane* const lane = getEdge()->getLanes()[queIdx == MESegment::PARKING_QUEUE ? 0 : queIdx];
    b.add(lane->geometryPositionAtOffset(getPositionOnLane() + vehiclePosition));
    b.grow(getVehicleType().getLength());
    getEdge()->unlock();
    return b;
}


/****************************************************************************/
