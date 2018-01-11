/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cmath>
#include <vector>
#include <string>
#include <utils/common/StringUtils.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Transportable.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include "GUIVehicle.h"
#include "GUIPerson.h"
#include "GUIContainer.h"
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUILane.h"

//#define DRAW_BOUNDING_BOX
//#define DEBUG_FOES

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation


/* -------------------------------------------------------------------------
 * GUIVehicle - methods
 * ----------------------------------------------------------------------- */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif
GUIVehicle::GUIVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                       MSVehicleType* type, const double speedFactor) :
    MSVehicle(pars, route, type, speedFactor),
    GUIBaseVehicle((MSBaseVehicle&) * this) {
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif


GUIVehicle::~GUIVehicle() {
}


GUIParameterTableWindow*
GUIVehicle::getParameterWindow(GUIMainWindow& app,
                               GUISUMOAbstractView&) {
    const int sublaneParams = MSGlobals::gLateralResolution > 0 ? 4 : 0;
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 37 + sublaneParams + (int)getParameter().getMap().size());
    // add items
    ret->mkItem("lane [id]", false, Named::getIDSecure(myLane, "n/a"));
    if (MSGlobals::gLaneChangeDuration > 0 || MSGlobals::gLateralResolution > 0) {
        const MSLane* shadowLane = getLaneChangeModel().getShadowLane();
        ret->mkItem("shadow lane [id]", false, shadowLane == 0 ? "" : shadowLane->getID());
    }
    if (MSGlobals::gLateralResolution > 0) {
        const MSLane* targetLane = getLaneChangeModel().getTargetLane();
        ret->mkItem("target lane [id]", false, targetLane == 0 ? "" : targetLane->getID());
    }
    ret->mkItem("position [m]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getPositionOnLane));
    ret->mkItem("lateral offset [m]", true,
                new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getLateralPositionOnLane));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getSpeed));
    ret->mkItem("lateral speed [m/s]", true,
                new FunctionBinding<MSAbstractLaneChangeModel, double>(&getLaneChangeModel(), &MSAbstractLaneChangeModel::getSpeedLat));
    ret->mkItem("acceleration [m/s^2]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getAcceleration));
    ret->mkItem("angle [degree]", true,
                new FunctionBinding<GUIVehicle, double>(this, &GUIBaseVehicle::getNaviDegree));
    ret->mkItem("slope [degree]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getSlope));
    ret->mkItem("speed factor", false, getChosenSpeedFactor());
    ret->mkItem("time gap on lane [s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getTimeGapOnLane));
    ret->mkItem("waiting time [s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getWaitingSeconds));
    ret->mkItem(("waiting time (accumulated, " + time2string(MSGlobals::gWaitingTimeMemory) + "s) [s]").c_str(), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getAccumulatedWaitingSeconds));
    ret->mkItem("time loss [s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getTimeLossSeconds));
    ret->mkItem("impatience", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getImpatience));
    ret->mkItem("last lane change [s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, time2string(getParameter().depart));
    ret->mkItem("depart delay [s]", false, time2string(getDepartDelay()));
    if (getParameter().repetitionNumber < std::numeric_limits<int>::max()) {
        ret->mkItem("remaining [#]", false, (int) getParameter().repetitionNumber - getParameter().repetitionsDone);
    }
    if (getParameter().repetitionOffset > 0) {
        ret->mkItem("insertion period [s]", false, time2string(getParameter().repetitionOffset));
    }
    if (getParameter().repetitionProbability > 0) {
        ret->mkItem("insertion probability", false, getParameter().repetitionProbability);
    }
    ret->mkItem("stop info", false, getStopInfo());
    ret->mkItem("line", false, myParameter->line);
    ret->mkItem("CO2 [mg/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getCO2Emissions));
    ret->mkItem("CO [mg/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getCOEmissions));
    ret->mkItem("HC [mg/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getHCEmissions));
    ret->mkItem("NOx [mg/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getNOxEmissions));
    ret->mkItem("PMx [mg/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getPMxEmissions));
    ret->mkItem("fuel [ml/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getFuelConsumption));
    ret->mkItem("electricity [Wh/s]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getElectricityConsumption));
    ret->mkItem("noise (Harmonoise) [dB]", true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getHarmonoise_NoiseEmissions));
    std::ostringstream str;
    for (std::vector<MSDevice*>::const_iterator i = myDevices.begin(); i != myDevices.end(); ++i) {
        if (i != myDevices.begin()) {
            str << ' ';
        }
        str << (*i)->getID().substr(0, (*i)->getID().find(getID()));
    }
    ret->mkItem("devices", false, str.str());
    ret->mkItem("persons", true,
                new FunctionBinding<GUIVehicle, int>(this, &MSVehicle::getPersonNumber));
    ret->mkItem("containers", true,
                new FunctionBinding<GUIVehicle, int>(this, &MSVehicle::getContainerNumber));
    // close building
    if (MSGlobals::gLateralResolution > 0) {
        ret->mkItem("right side on edge [m]", true, new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getRightSideOnEdge2));
        ret->mkItem("left side on edge [m]", true, new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getLeftSideOnEdge));
        ret->mkItem("rightmost edge sublane [#]", true, new FunctionBinding<GUIVehicle, int>(this, &GUIVehicle::getRightSublaneOnEdge));
        ret->mkItem("leftmost edge sublane [#]", true, new FunctionBinding<GUIVehicle, int>(this, &GUIVehicle::getLeftSublaneOnEdge));
        ret->mkItem("lane change maneuver distance [m]", true, new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getManeuverDist));
    }
    ret->closeBuilding(&getParameter());
    return ret;
}


GUIParameterTableWindow*
GUIVehicle::getTypeParameterWindow(GUIMainWindow& app,
                                   GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 25 
                + (int)myType->getParameter().getMap().size()
                + (int)myType->getParameter().lcParameter.size()
                + (int)myType->getParameter().jmParameter.size());
    // add items
    ret->mkItem("Type Information:", false, "");
    ret->mkItem("type [id]", false, myType->getID());
    ret->mkItem("length", false, myType->getLength());
    ret->mkItem("width", false, myType->getWidth());
    ret->mkItem("height", false, myType->getHeight());
    ret->mkItem("minGap", false, myType->getMinGap());
    ret->mkItem("vehicle class", false, SumoVehicleClassStrings.getString(myType->getVehicleClass()));
    ret->mkItem("emission class", false, PollutantsInterface::getName(myType->getEmissionClass()));
    ret->mkItem("carFollowModel", false, SUMOXMLDefinitions::CarFollowModels.getString((SumoXMLTag)getCarFollowModel().getModelID()));
    ret->mkItem("LaneChangeModel", false, SUMOXMLDefinitions::LaneChangeModels.getString(getLaneChangeModel().getModelID()));
    ret->mkItem("maximum speed [m/s]", false, getMaxSpeed());
    ret->mkItem("maximum acceleration [m/s^2]", false, getCarFollowModel().getMaxAccel());
    ret->mkItem("maximum deceleration [m/s^2]", false, getCarFollowModel().getMaxDecel());
    ret->mkItem("emergency deceleration [m/s^2]", false, getCarFollowModel().getEmergencyDecel());
    ret->mkItem("apparent deceleration [m/s^2]", false, getCarFollowModel().getApparentDecel());
    ret->mkItem("imperfection (sigma)", false, getCarFollowModel().getImperfection());
    ret->mkItem("desired headway (tau)", false, getCarFollowModel().getHeadwayTime());
    ret->mkItem("person capacity", false, myType->getPersonCapacity());
    ret->mkItem("boarding time", false, STEPS2TIME(myType->getBoardingDuration()));
    ret->mkItem("container capacity", false, myType->getContainerCapacity());
    ret->mkItem("loading time", false, STEPS2TIME(myType->getLoadingDuration()));
    if (MSGlobals::gLateralResolution > 0) {
        ret->mkItem("minGapLat", false, myType->getMinGapLat());
        ret->mkItem("maxSpeedLat", false, myType->getMaxSpeedLat());
        ret->mkItem("latAlignment", false, toString(myType->getPreferredLateralAlignment()));
    } else if (MSGlobals::gLaneChangeDuration > 0) {
        ret->mkItem("maxSpeedLat", false, myType->getMaxSpeedLat());
    }
    for (auto item : myType->getParameter().lcParameter) {
        ret->mkItem(toString(item.first).c_str(), false, toString(item.second));
    }
    for (auto item : myType->getParameter().jmParameter) {
        ret->mkItem(toString(item.first).c_str(), false, toString(item.second));
    }

    // close building
    ret->closeBuilding(&(myType->getParameter()));
    return ret;
}




void
GUIVehicle::drawAction_drawPersonsAndContainers(const GUIVisualizationSettings& s) const {
    if (myPersonDevice != 0) {
        const std::vector<MSTransportable*>& ps = myPersonDevice->getTransportables();
        int personIndex = 0;
        for (std::vector<MSTransportable*>::const_iterator i = ps.begin(); i != ps.end(); ++i) {
            GUIPerson* person = dynamic_cast<GUIPerson*>(*i);
            assert(person != 0);
            person->setPositionInVehicle(getSeatPosition(personIndex++));
            person->drawGL(s);
        }
    }
    if (myContainerDevice != 0) {
        const std::vector<MSTransportable*>& cs = myContainerDevice->getTransportables();
        int containerIndex = 0;
        for (std::vector<MSTransportable*>::const_iterator i = cs.begin(); i != cs.end(); ++i) {
            GUIContainer* container = dynamic_cast<GUIContainer*>(*i);
            assert(container != 0);
            container->setPositionInVehicle(getSeatPosition(containerIndex++));
            container->drawGL(s);
        }
    }
#ifdef DRAW_BOUNDING_BOX
    glPushName(getGlID());
    glPushMatrix();
    glTranslated(0, 0, getType());
    PositionVector boundingBox = getBoundingBox();
    boundingBox.push_back(boundingBox.front());
    PositionVector smallBB = getBoundingPoly();
    glColor3d(0, .8, 0);
    GLHelper::drawLine(boundingBox);
    glColor3d(0.5, .8, 0);
    GLHelper::drawLine(smallBB);
    //GLHelper::drawBoxLines(getBoundingBox(), 0.5);
    glPopMatrix();
    glPopName();
#endif
}


void
GUIVehicle::drawAction_drawLinkItems(const GUIVisualizationSettings& s) const {
    glTranslated(0, 0, getType() + .2); // draw on top of cars
    for (DriveItemVector::const_iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
        if ((*i).myLink == 0) {
            continue;
        }
        MSLink* link = (*i).myLink;
        MSLane* via = link->getViaLaneOrLane();
        if (via != 0) {
            Position p = via->getShape()[0];
            if ((*i).mySetRequest) {
                glColor3d(0, .8, 0);
            } else {
                glColor3d(.8, 0, 0);
            }
            const SUMOTime leaveTime = (*i).myLink->getLeaveTime(
                                           (*i).myArrivalTime, (*i).myArrivalSpeed, (*i).getLeaveSpeed(), getVehicleType().getLength());
            drawLinkItem(p, (*i).myArrivalTime, leaveTime, s.vehicleName.size / s.scale);
            // the time slot that ego vehicle uses when checking opened may
            // differ from the one it requests in setApproaching
            MSLink::ApproachingVehicleInformation avi = (*i).myLink->getApproaching(this);
            assert(avi.arrivalTime == (*i).myArrivalTime && avi.leavingTime == leaveTime);
            UNUSED_PARAMETER(avi); // only used for assertion
        }
    }
    glTranslated(0, 0, getType() - .2); // draw on top of cars
}


bool
GUIVehicle::drawAction_drawCarriageClass(const GUIVisualizationSettings& s, SUMOVehicleShape guiShape, bool asImage) const {
    switch (guiShape) {
        case SVS_BUS_FLEXIBLE:
            drawAction_drawRailCarriages(s, 8.25, 0, 0, asImage); // 16.5 overall, 2 modules http://de.wikipedia.org/wiki/Ikarus_180
            break;
        case SVS_RAIL:
            drawAction_drawRailCarriages(s, 24.5, 1, 1, asImage); // http://de.wikipedia.org/wiki/UIC-Y-Wagen_%28DR%29
            break;
        case SVS_RAIL_CAR:
            drawAction_drawRailCarriages(s, 16.85, 1, 0, asImage); // 67.4m overall, 4 carriages http://de.wikipedia.org/wiki/DB-Baureihe_423
//            drawAction_drawRailCarriages(s, 5.71, 0, 0, asImage); // 40.0m overall, 7 modules http://de.wikipedia.org/wiki/Bombardier_Flexity_Berlin
//            drawAction_drawRailCarriages(s, 9.44, 1, 1, asImage); // actually length of the locomotive http://de.wikipedia.org/wiki/KJI_Nr._20_und_21
//            drawAction_drawRailCarriages(s, 24.775, 0, 0, asImage); // http://de.wikipedia.org/wiki/ICE_3
            break;
        case SVS_RAIL_CARGO:
            drawAction_drawRailCarriages(s, 13.86, 1, 0, asImage); // UIC 571-1 http://de.wikipedia.org/wiki/Flachwagen
            break;
        default:
            return false;
    }
    return true;
}

#define BLINKER_POS_FRONT .5
#define BLINKER_POS_BACK .5

inline void
drawAction_drawBlinker(double dir, double length) {
    glColor3d(1.f, .8f, 0);
    glPushMatrix();
    glTranslated(dir, BLINKER_POS_FRONT, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslated(dir, length - BLINKER_POS_BACK, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
}


void
GUIVehicle::drawAction_drawVehicleBlinker(double length) const {
    if (!signalSet(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT | MSVehicle::VEH_SIGNAL_BLINKER_LEFT | MSVehicle::VEH_SIGNAL_BLINKER_EMERGENCY)) {
        return;
    }
    const double offset = MAX2(.5 * getVehicleType().getWidth(), .4);
    if (signalSet(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT)) {
        drawAction_drawBlinker(-offset, length);
    }
    if (signalSet(MSVehicle::VEH_SIGNAL_BLINKER_LEFT)) {
        drawAction_drawBlinker(offset, length);;
    }
    if (signalSet(MSVehicle::VEH_SIGNAL_BLINKER_EMERGENCY)) {
        drawAction_drawBlinker(-offset, length);
        drawAction_drawBlinker(offset, length);
    }
}


inline void
GUIVehicle::drawAction_drawVehicleBrakeLight(double length, bool onlyOne) const {
    if (!signalSet(MSVehicle::VEH_SIGNAL_BRAKELIGHT)) {
        return;
    }
    glColor3f(1.f, .2f, 0);
    glPushMatrix();
    if (onlyOne) {
        glTranslated(0, length, -0.1);
        GLHelper::drawFilledCircle(.5, 6);
    } else {
        glTranslated(-getVehicleType().getWidth() * 0.5, length, -0.1);
        GLHelper::drawFilledCircle(.5, 6);
        glPopMatrix();
        glPushMatrix();
        glTranslated(getVehicleType().getWidth() * 0.5, length, -0.1);
        GLHelper::drawFilledCircle(.5, 6);
    }
    glPopMatrix();
}

inline void
GUIVehicle::drawAction_drawVehicleBlueLight() const {
    if (signalSet(MSVehicle::VEH_SIGNAL_EMERGENCY_BLUE)) {
        glPushMatrix();
        glTranslated(0, 2.5, .5);
        glColor3f(0, 0, 1);
        GLHelper::drawFilledCircle(.5, 6);
        glPopMatrix();
    }
}


double
GUIVehicle::getColorValue(int activeScheme) const {
    switch (activeScheme) {
        case 8:
            return getSpeed();
        case 9:
            // color by action step
            if (isActionStep(SIMSTEP)) {
                // Upcoming simstep is actionstep (t was already increased before drawing)
                return 1.;
            } else if (isActive()) {
                // Completed simstep was actionstep
                return 2.;
            } else {
                // not active
                return 0.;
            }
        case 10:
            return getWaitingSeconds();
        case 11:
            return getAccumulatedWaitingSeconds();
        case 12:
            return getLastLaneChangeOffset();
        case 13:
            return getLane()->getVehicleMaxSpeed(this);
        case 14:
            return getCO2Emissions();
        case 15:
            return getCOEmissions();
        case 16:
            return getPMxEmissions();
        case 17:
            return getNOxEmissions();
        case 18:
            return getHCEmissions();
        case 19:
            return getFuelConsumption();
        case 20:
            return getHarmonoise_NoiseEmissions();
        case 21:
            if (getNumberReroutes() == 0) {
                return -1;
            }
            return getNumberReroutes();
        case 22:
            return gSelected.isSelected(GLO_VEHICLE, getGlID());
        case 23:
            return getBestLaneOffset();
        case 24:
            return getAcceleration();
        case 25:
            return getTimeGapOnLane();
        case 26:
            return STEPS2TIME(getDepartDelay());
        case 27:
            return getElectricityConsumption();
        case 28:
            return getTimeLossSeconds();
    }
    return 0;
}


void
GUIVehicle::drawBestLanes() const {
    myLock.lock();
    std::vector<std::vector<MSVehicle::LaneQ> > bestLanes = myBestLanes;
    myLock.unlock();
    for (std::vector<std::vector<MSVehicle::LaneQ> >::iterator j = bestLanes.begin(); j != bestLanes.end(); ++j) {
        std::vector<MSVehicle::LaneQ>& lanes = *j;
        double gmax = -1;
        double rmax = -1;
        for (std::vector<MSVehicle::LaneQ>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            gmax = MAX2((*i).length, gmax);
            rmax = MAX2((*i).occupation, rmax);
        }
        for (std::vector<MSVehicle::LaneQ>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            const PositionVector& shape = (*i).lane->getShape();
            double g = (*i).length / gmax;
            double r = (*i).occupation / rmax;
            glColor3d(r, g, 0);
            double width = 0.5 / (1 + abs((*i).bestLaneOffset));
            GLHelper::drawBoxLines(shape, width);

            PositionVector s1 = shape;
            s1.move2side((double) .1);
            glColor3d(r, 0, 0);
            GLHelper::drawLine(s1);
            s1.move2side((double) - .2);
            glColor3d(0, g, 0);
            GLHelper::drawLine(s1);

            glColor3d(r, g, 0);
        }
    }
}


void
GUIVehicle::drawRouteHelper(const GUIVisualizationSettings& s, const MSRoute& r) const {
    const double exaggeration = s.vehicleSize.getExaggeration(s);
    MSRouteIterator i = r.begin();
    const std::vector<MSLane*>& bestLaneConts = getBestLanesContinuation();
    // draw continuation lanes when drawing the current route where available
    int bestLaneIndex = (&r == myRoute ? 0 : (int)bestLaneConts.size());
    for (; i != r.end(); ++i) {
        const GUILane* lane;
        if (bestLaneIndex < (int)bestLaneConts.size() && bestLaneConts[bestLaneIndex] != 0 && (*i) == &(bestLaneConts[bestLaneIndex]->getEdge())) {
            lane = static_cast<GUILane*>(bestLaneConts[bestLaneIndex]);
            ++bestLaneIndex;
        } else {
            const std::vector<MSLane*>* allowed = (*i)->allowedLanes(getVClass());
            if (allowed != 0 && allowed->size() != 0) {
                lane = static_cast<GUILane*>((*allowed)[0]);
            } else {
                lane = static_cast<GUILane*>((*i)->getLanes()[0]);
            }
        }
        GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), exaggeration);
    }
    int stopIndex = 0;
    for (const Stop& stop : myStops) {
        Position pos = stop.lane->geometryPositionAtOffset(stop.getEndPos(*this));
        GLHelper::drawBoxLines(stop.lane->getShape().getOrthogonal(pos, 10, true, stop.lane->getWidth()), 0.1);
        std::string label = "stop " + toString(stopIndex);
        if (stop.pars.until >= 0) {
            label += " until:" + time2string(stop.pars.until);
        }
        if (stop.duration >= 0) {
            label += " duration:" + time2string(stop.duration);
        }
        GLHelper::drawText(label, pos, 1.0, s.vehicleName.size / s.scale, s.vehicleName.color);
        stopIndex++;
    }
}



MSLane*
GUIVehicle::getPreviousLane(MSLane* current, int& furtherIndex) const {
    if (furtherIndex < (int)myFurtherLanes.size()) {
        return myFurtherLanes[furtherIndex++];
    } else {
        return current;
    }
}


void
GUIVehicle::drawAction_drawRailCarriages(const GUIVisualizationSettings& s, double defaultLength, double carriageGap, int firstPassengerCarriage, bool asImage) const {
    RGBColor current = GLHelper::getColor();
    RGBColor darker = current.changedBrightness(-51);
    const double exaggeration = s.vehicleSize.getExaggeration(s);
    defaultLength *= exaggeration;
    if (exaggeration == 0) {
        return;
    }
    carriageGap *= exaggeration;
    const double length = getVehicleType().getLength() * exaggeration;
    const double halfWidth = getVehicleType().getWidth() / 2.0 * exaggeration;
    glPopMatrix(); // undo scaling and 90 degree rotation
    glPopMatrix(); // undo initial translation and rotation
    GLHelper::setColor(darker);
    const double xCornerCut = 0.3 * exaggeration;
    const double yCornerCut = 0.4 * exaggeration;
    // round to closest integer
    const int numCarriages = (int)(length / (defaultLength + carriageGap) + 0.5);
    assert(numCarriages > 0);
    const double carriageLengthWithGap = length / numCarriages;
    const double carriageLength = carriageLengthWithGap - carriageGap;
    // lane on which the carriage front is situated
    MSLane* lane = myLane;
    int furtherIndex = 0;
    // lane on which the carriage back is situated
    MSLane* backLane = myLane;
    int backFurtherIndex = furtherIndex;
    // offsets of front and back
    double carriageOffset = myState.pos();
    double carriageBackOffset = myState.pos() - carriageLength;
    // handle seats
    int requiredSeats = getNumPassengers();
    if (requiredSeats > 0) {
        mySeatPositions.clear();
    }
    Position front, back;
    double angle = 0.;
    // draw individual carriages
    for (int i = 0; i < numCarriages; ++i) {
        while (carriageOffset < 0) {
            MSLane* prev = getPreviousLane(lane, furtherIndex);
            if (prev != lane) {
                carriageOffset += prev->getLength();
            } else {
                // no lane available for drawing.
                carriageOffset = 0;
            }
            lane = prev;
        }
        while (carriageBackOffset < 0) {
            MSLane* prev = getPreviousLane(backLane, backFurtherIndex);
            if (prev != backLane) {
                carriageBackOffset += prev->getLength();
            } else {
                // no lane available for drawing.
                carriageBackOffset = 0;
            }
            backLane = prev;
        }
        front = lane->geometryPositionAtOffset(carriageOffset);
        back = backLane->geometryPositionAtOffset(carriageBackOffset);
        if (front == back) {
            // no place for drawing available
            continue;
        }
        const double drawnCarriageLength = front.distanceTo2D(back);
        angle = atan2((front.x() - back.x()), (back.y() - front.y())) * (double) 180.0 / (double) M_PI;
        if (i >= firstPassengerCarriage) {
            computeSeats(front, back, requiredSeats);
        }
        glPushMatrix();
        glTranslated(front.x(), front.y(), getType());
        glRotated(angle, 0, 0, 1);
        if (!asImage || !drawAction_drawVehicleAsImage(s, carriageLength)) {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(-halfWidth + xCornerCut, 0);
            glVertex2d(-halfWidth, yCornerCut);
            glVertex2d(-halfWidth, drawnCarriageLength - yCornerCut);
            glVertex2d(-halfWidth + xCornerCut, drawnCarriageLength);
            glVertex2d(halfWidth - xCornerCut, drawnCarriageLength);
            glVertex2d(halfWidth, drawnCarriageLength - yCornerCut);
            glVertex2d(halfWidth, yCornerCut);
            glVertex2d(halfWidth - xCornerCut, 0);
            glEnd();
        }
        glPopMatrix();
        carriageOffset -= carriageLengthWithGap;
        carriageBackOffset -= carriageLengthWithGap;
        GLHelper::setColor(current);
    }
    if (getVType().getGuiShape() == SVS_RAIL_CAR) {
        glPushMatrix();
        glTranslated(front.x(), front.y(), getType());
        glRotated(angle, 0, 0, 1);
        drawAction_drawVehicleBlinker(carriageLength);
        drawAction_drawVehicleBrakeLight(carriageLength);
        glPopMatrix();
    }
    // restore matrices
    glPushMatrix();
    front = getPosition();
    glTranslated(front.x(), front.y(), getType());
    glRotated(angle, 0, 0, 1);
    const double upscale = s.vehicleSize.getExaggeration(s);
    glScaled(upscale, upscale, 1);
    glPushMatrix();
}


int
GUIVehicle::getNumPassengers() const {
    if (myPersonDevice != 0) {
        return (int)myPersonDevice->size();
    }
    return 0;
}


void
GUIVehicle::computeSeats(const Position& front, const Position& back, int& requiredSeats) const {
    if (requiredSeats <= 0) {
        return; // save some work
    }
    const double length = front.distanceTo2D(back);
    if (length < 4) {
        // small vehicle, sit at the center
        mySeatPositions.push_back(PositionVector::positionAtOffset2D(front, back, length / 2));
        requiredSeats--;
    } else {
        for (double p = 2; p <= length - 1; p += 1) {
            mySeatPositions.push_back(PositionVector::positionAtOffset2D(front, back, p));
            requiredSeats--;
        }
    }
}


double
GUIVehicle::getLastLaneChangeOffset() const {
    return STEPS2TIME(getLaneChangeModel().getLastLaneChangeOffset());
}


std::string
GUIVehicle::getStopInfo() const {
    std::string result = "";
    if (isParking()) {
        result += "parking";
    } else if (isStopped()) {
        result += "stopped";
    } else if (hasStops()) {
        return "next: " + myStops.front().getDescription();
    } else {
        return "";
    }
    if (myStops.front().pars.triggered) {
        result += ", triggered";
    } else if (myStops.front().pars.containerTriggered) {
        result += ", containerTriggered";
    } else if (myStops.front().collision) {
        result += ", collision";
    } else {
        result += ", duration=" + time2string(myStops.front().duration);
    }
    return result;
}


void
GUIVehicle::selectBlockingFoes() const {
    double dist = myLane->getLength() - getPositionOnLane();
    for (DriveItemVector::const_iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
        const DriveProcessItem& dpi = *i;
        if (dpi.myLink == 0) {
            continue;
        }
        std::vector<const SUMOVehicle*> blockingFoes;
        std::vector<const MSPerson*> blockingPersons;
#ifdef DEBUG_FOES
        const bool isOpen =
#endif
            dpi.myLink->opened(dpi.myArrivalTime, dpi.myArrivalSpeed, dpi.getLeaveSpeed(), getVehicleType().getLength(),
                               getImpatience(), getCarFollowModel().getMaxDecel(), getWaitingTime(), getLateralPositionOnLane(), &blockingFoes);
#ifdef DEBUG_FOES
        if (!isOpen) {
            std::cout << SIMTIME << " veh=" << getID() << " foes at link=" << dpi.myLink->getViaLaneOrLane()->getID() << ":\n";
            for (std::vector<const SUMOVehicle*>::const_iterator it = blockingFoes.begin(); it != blockingFoes.end(); ++it) {
                std::cout << "   " << (*it)->getID() << "\n";
            }
        }
#endif
        if (getLaneChangeModel().getShadowLane() != 0) {
            MSLink* parallelLink = dpi.myLink->getParallelLink(getLaneChangeModel().getShadowDirection());
            if (parallelLink != 0) {
                const double shadowLatPos = getLateralPositionOnLane() - getLaneChangeModel().getShadowDirection() * 0.5 * (
                                                myLane->getWidth() + getLaneChangeModel().getShadowLane()->getWidth());
#ifdef DEBUG_FOES
                const bool isShadowOpen =
#endif
                    parallelLink->opened(dpi.myArrivalTime, dpi.myArrivalSpeed, dpi.getLeaveSpeed(),
                                         getVehicleType().getLength(), getImpatience(),
                                         getCarFollowModel().getMaxDecel(),
                                         getWaitingTime(), shadowLatPos, &blockingFoes);
#ifdef DEBUG_FOES
                if (!isShadowOpen) {
                    std::cout << SIMTIME << " veh=" << getID() << " foes at shadow link=" << parallelLink->getViaLaneOrLane()->getID() << ":\n";
                    for (std::vector<const SUMOVehicle*>::const_iterator it = blockingFoes.begin(); it != blockingFoes.end(); ++it) {
                        std::cout << "   " << (*it)->getID() << "\n";
                    }
                }
#endif
            }
        }
        for (std::vector<const SUMOVehicle*>::const_iterator it = blockingFoes.begin(); it != blockingFoes.end(); ++it) {
            gSelected.select(static_cast<const GUIVehicle*>(*it)->getGlID());
        }
        const MSLink::LinkLeaders linkLeaders = (dpi.myLink)->getLeaderInfo(this, dist, &blockingPersons);
        for (MSLink::LinkLeaders::const_iterator it = linkLeaders.begin(); it != linkLeaders.end(); ++it) {
            // the vehicle to enter the junction first has priority
            const GUIVehicle* leader = dynamic_cast<const GUIVehicle*>(it->vehAndGap.first);
            if (leader != 0) {
                if (dpi.myLink->isLeader(this, leader)) {
                    gSelected.select(leader->getGlID());
#ifdef DEBUG_FOES
                    std::cout << SIMTIME << " veh=" << getID() << " linkLeader at link=" << dpi.myLink->getViaLaneOrLane()->getID() << " foe=" << leader->getID() << "\n";
#endif
                }
            } else {
                for (std::vector<const MSPerson*>::iterator it_p = blockingPersons.begin(); it_p != blockingPersons.end(); ++it_p) {
                    const GUIPerson* foe = dynamic_cast<const GUIPerson*>(*it_p);
                    if (foe != 0) {
                        gSelected.select(foe->getGlID());
                        //std::cout << SIMTIME << " veh=" << getID() << " is blocked on link " << dpi.myLink->getRespondIndex() << " to " << dpi.myLink->getViaLaneOrLane()->getID() << " by pedestrian. dist=" << it->second << "\n";
                    }
                }
            }
        }
        dist += dpi.myLink->getViaLaneOrLane()->getLength();
    }
}


void
GUIVehicle::drawOutsideNetwork(bool add) {
    GUIMainWindow* mw = GUIMainWindow::getInstance();
    GUISUMOAbstractView* view = mw->getActiveView();
    if (view != 0) {
        if (add) {
            if ((myAdditionalVisualizations[view] & VO_DRAW_OUTSIDE_NETWORK) == 0) {
                myAdditionalVisualizations[view] |= VO_DRAW_OUTSIDE_NETWORK;
                view->addAdditionalGLVisualisation(this);
            }
        } else {
            view->removeAdditionalGLVisualisation(this);
            myAdditionalVisualizations[view] &= ~VO_DRAW_OUTSIDE_NETWORK;
        }
    }
}

bool
GUIVehicle::isSelected() const {
    return gSelected.isSelected(GLO_VEHICLE, getGlID());
}

int
GUIVehicle::getRightSublaneOnEdge() const {
    const double rightSide = getRightSideOnEdge();
    const std::vector<double>& sublaneSides = myLane->getEdge().getSubLaneSides();
    for (int i = 0; i < (int)sublaneSides.size(); ++i) {
        if (sublaneSides[i] > rightSide) {
            return MAX2(i - 1, 0);
        }
    }
    return -1;
}

int
GUIVehicle::getLeftSublaneOnEdge() const {
    const double leftSide = getLeftSideOnEdge();
    const std::vector<double>& sublaneSides = myLane->getEdge().getSubLaneSides();
    for (int i = (int)sublaneSides.size() - 1; i >= 0; --i) {
        if (sublaneSides[i] < leftSide) {
            return i;
        }
    }
    return -1;
}

double
GUIVehicle::getManeuverDist() const {
    return getLaneChangeModel().getManeuverDist();
}

/****************************************************************************/

