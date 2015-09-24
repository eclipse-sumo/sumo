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

#include <cmath>
#include <vector>
#include <string>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/StringUtils.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Person.h>
#include <microsim/devices/MSDevice_Container.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include "GUIVehicle.h"
#include "GUIPerson.h"
#include "GUIContainer.h"
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUILane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation


/* -------------------------------------------------------------------------
 * GUIVehicle - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                       const MSVehicleType* type, const SUMOReal speedFactor) :
    MSVehicle(pars, route, type, speedFactor),
    GUIBaseVehicle((MSBaseVehicle&)*this) {
    mySeatPositions.push_back(Position(0, 0)); // ensure length 1
}


GUIVehicle::~GUIVehicle() {
}


GUIParameterTableWindow*
GUIVehicle::getParameterWindow(GUIMainWindow& app,
                               GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 40);
    // add items
    ret->mkItem("lane [id]", false, myLane->getID());
    ret->mkItem("position [m]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getSpeed));
    ret->mkItem("angle [degree]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getAngle));
    if (getChosenSpeedFactor() != 1) {
        ret->mkItem("speed factor", false, getChosenSpeedFactor());
    }
    ret->mkItem("time gap [s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getTimeGap));
    ret->mkItem("waiting time [s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getWaitingSeconds));
    ret->mkItem("impatience", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &MSVehicle::getImpatience));
    ret->mkItem("last lane change [s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, time2string(getParameter().depart));
    if (getParameter().repetitionNumber < INT_MAX) {
        ret->mkItem("remaining [#]", false, (unsigned int) getParameter().repetitionNumber - getParameter().repetitionsDone);
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
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getCO2Emissions));
    ret->mkItem("CO [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getCOEmissions));
    ret->mkItem("HC [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHCEmissions));
    ret->mkItem("NOx [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getNOxEmissions));
    ret->mkItem("PMx [mg/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getPMxEmissions));
    ret->mkItem("fuel [ml/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getFuelConsumption));
    ret->mkItem("noise (Harmonoise) [dB]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHarmonoise_NoiseEmissions));
    std::ostringstream str;
    for (std::vector<MSDevice*>::const_iterator i = myDevices.begin(); i != myDevices.end(); ++i) {
        if (i != myDevices.begin()) {
            str << ' ';
        }
        str << (*i)->getID().substr(0, (*i)->getID().find(getID()));
    }
    ret->mkItem("devices", false, str.str());
    ret->mkItem("persons", true,
                new FunctionBinding<GUIVehicle, unsigned int>(this, &GUIVehicle::getPersonNumber));
    ret->mkItem("containers", true,
                new FunctionBinding<GUIVehicle, unsigned int>(this, &GUIVehicle::getContainerNumber));

    ret->mkItem("parameters [key:val]", false, toString(getParameter().getMap()));
    ret->mkItem("", false, "");
    ret->mkItem("Type Information:", false, "");
    ret->mkItem("type [id]", false, myType->getID());
    ret->mkItem("length", false, myType->getLength());
    ret->mkItem("minGap", false, myType->getMinGap());
    ret->mkItem("vehicle class", false, SumoVehicleClassStrings.getString(myType->getVehicleClass()));
    ret->mkItem("emission class", false, PollutantsInterface::getName(myType->getEmissionClass()));
    ret->mkItem("maximum speed [m/s]", false, getMaxSpeed());
    ret->mkItem("maximum acceleration [m/s^2]", false, getCarFollowModel().getMaxAccel());
    ret->mkItem("maximum deceleration [m/s^2]", false, getCarFollowModel().getMaxDecel());
    ret->mkItem("imperfection (sigma)", false, getCarFollowModel().getImperfection());
    ret->mkItem("reaction time (tau)", false, getCarFollowModel().getHeadwayTime());
    ret->mkItem("person capacity", false, myType->getPersonCapacity());
    ret->mkItem("container capacity", false, myType->getContainerCapacity());

    ret->mkItem("type parameters [key:val]", false, toString(myType->getParameter().getMap()));
    // close building
    ret->closeBuilding();
    return ret;
}


void 
GUIVehicle::drawAction_drawPersonsAndContainers(const GUIVisualizationSettings& s) const {
    if (myPersonDevice != 0) {
        const std::vector<MSTransportable*>& ps = myPersonDevice->getPersons();
        size_t personIndex = 0;
        for (std::vector<MSTransportable*>::const_iterator i = ps.begin(); i != ps.end(); ++i) {
            GUIPerson* person = dynamic_cast<GUIPerson*>(*i);
            assert(person != 0);
            person->setPositionInVehicle(getSeatPosition(personIndex++));
            person->drawGL(s);
        }
    }
    if (myContainerDevice != 0) {
        const std::vector<MSTransportable*>& cs = myContainerDevice->getContainers();
        size_t containerIndex = 0;
        for (std::vector<MSTransportable*>::const_iterator i = cs.begin(); i != cs.end(); ++i) {
            GUIContainer* container = dynamic_cast<GUIContainer*>(*i);
            assert(container != 0);
            container->setPositionInVehicle(getSeatPosition(containerIndex++));
            container->drawGL(s);
        }
    }
}


void 
GUIVehicle::drawAction_drawLinkItems(const GUIVisualizationSettings& s) const {
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
                    (*i).myArrivalTime, (*i).myArrivalSpeed, (*i).getLeaveSpeed(), getVehicleType().getLengthWithGap());
            drawLinkItem(p, (*i).myArrivalTime, leaveTime, s.vehicleSize.getExaggeration(s));
            // the time slot that ego vehicle uses when checking opened may
            // differ from the one it requests in setApproaching
            MSLink::ApproachingVehicleInformation avi = (*i).myLink->getApproaching(this);
            assert(avi.arrivalTime == (*i).myArrivalTime && avi.leavingTime == leaveTime);
            UNUSED_PARAMETER(avi); // only used for assertion
        }
    }
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
drawAction_drawBlinker(double dir, SUMOReal length) {
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
GUIVehicle::drawAction_drawVehicleBlinker(SUMOReal length) const {
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
GUIVehicle::drawAction_drawVehicleBrakeLight(SUMOReal length) const {
    if (!signalSet(MSVehicle::VEH_SIGNAL_BRAKELIGHT)) {
        return;
    }
    glColor3f(1.f, .2f, 0);
    glPushMatrix();
    glTranslated(-getVehicleType().getWidth() * 0.5, length, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslated(getVehicleType().getWidth() * 0.5, length, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
}


SUMOReal
GUIVehicle::getColorValue(size_t activeScheme) const {
    switch (activeScheme) {
        case 8:
            return getSpeed();
        case 9:
            return getWaitingSeconds();
        case 10:
            return getLastLaneChangeOffset();
        case 11:
            return getLane()->getVehicleMaxSpeed(this);
        case 12:
            return getCO2Emissions();
        case 13:
            return getCOEmissions();
        case 14:
            return getPMxEmissions();
        case 15:
            return getNOxEmissions();
        case 16:
            return getHCEmissions();
        case 17:
            return getFuelConsumption();
        case 18:
            return getHarmonoise_NoiseEmissions();
        case 19: // !!! unused!?
            if (getNumberReroutes() == 0) {
                return -1;
            }
            return getNumberReroutes();
        case 20:
            return gSelected.isSelected(GLO_VEHICLE, getGlID());
        case 21:
            return getBestLaneOffset();
        case 22:
            return getAcceleration();
        case 23:
            return getTimeGap();
    }
    return 0;
}


void
GUIVehicle::drawBestLanes() const {
    myLock.lock();
    std::vector<std::vector<MSVehicle::LaneQ> > bestLanes = myBestLanes;
    myLock.unlock();
    SUMOReal width = 0.5;
    for (std::vector<std::vector<MSVehicle::LaneQ> >::iterator j = bestLanes.begin(); j != bestLanes.end(); ++j) {
        std::vector<MSVehicle::LaneQ>& lanes = *j;
        SUMOReal gmax = -1;
        SUMOReal rmax = -1;
        for (std::vector<MSVehicle::LaneQ>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            gmax = MAX2((*i).length, gmax);
            rmax = MAX2((*i).occupation, rmax);
        }
        for (std::vector<MSVehicle::LaneQ>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            const PositionVector& shape = (*i).lane->getShape();
            SUMOReal g = (*i).length / gmax;
            SUMOReal r = (*i).occupation / rmax;
            glColor3d(r, g, 0);
            GLHelper::drawBoxLines(shape, width);

            PositionVector s1 = shape;
            s1.move2side((SUMOReal) .1);
            glColor3d(r, 0, 0);
            GLHelper::drawLine(s1);
            s1.move2side((SUMOReal) - .2);
            glColor3d(0, g, 0);
            GLHelper::drawLine(s1);

            glColor3d(r, g, 0);
            Position lastPos = shape[-1];
        }
        width = .2;
    }
}


void
GUIVehicle::drawRouteHelper(const MSRoute& r, SUMOReal exaggeration) const {
    MSRouteIterator i = r.begin();
    const std::vector<MSLane*>& bestLaneConts = getBestLanesContinuation();
    // draw continuation lanes when drawing the current route where available
    size_t bestLaneIndex = (&r == myRoute ? 0 : bestLaneConts.size());
    for (; i != r.end(); ++i) {
        const GUILane* lane;
        if (bestLaneIndex < bestLaneConts.size() && bestLaneConts[bestLaneIndex] != 0 && (*i) == &(bestLaneConts[bestLaneIndex]->getEdge())) {
            lane = static_cast<GUILane*>(bestLaneConts[bestLaneIndex]);
            ++bestLaneIndex;
        } else {
            lane = static_cast<GUILane*>((*i)->getLanes()[0]);
        }
        GLHelper::drawBoxLines(lane->getShape(), lane->getShapeRotations(), lane->getShapeLengths(), exaggeration);
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
GUIVehicle::drawAction_drawRailCarriages(const GUIVisualizationSettings& s, SUMOReal defaultLength, SUMOReal carriageGap, int firstPassengerCarriage, bool asImage) const {
    RGBColor current = GLHelper::getColor();
    RGBColor darker = current.changedBrightness(-51);
    const SUMOReal exaggeration = s.vehicleSize.getExaggeration(s);
    defaultLength *= exaggeration;
    if (exaggeration == 0) {
        return;
    }
    carriageGap *= exaggeration;
    const SUMOReal length = getVehicleType().getLength() * exaggeration;
    const SUMOReal halfWidth = getVehicleType().getWidth() / 2.0 * exaggeration;
    glPopMatrix(); // undo scaling and 90 degree rotation
    glPopMatrix(); // undo initial translation and rotation
    GLHelper::setColor(darker);
    const SUMOReal xCornerCut = 0.3 * exaggeration;
    const SUMOReal yCornerCut = 0.4 * exaggeration;
    // round to closest integer
    const int numCarriages = floor(length / (defaultLength + carriageGap) + 0.5);
    assert(numCarriages > 0);
    const SUMOReal carriageLengthWithGap = length / numCarriages;
    const SUMOReal carriageLength = carriageLengthWithGap - carriageGap;
    // lane on which the carriage front is situated
    MSLane* lane = myLane;
    int furtherIndex = 0;
    // lane on which the carriage back is situated
    MSLane* backLane = myLane;
    int backFurtherIndex = furtherIndex;
    // offsets of front and back
    SUMOReal carriageOffset = myState.pos();
    SUMOReal carriageBackOffset = myState.pos() - carriageLength;
    // handle seats
    int requiredSeats = getNumPassengers();
    if (requiredSeats > 0) {
        mySeatPositions.clear();
    }
    Position front, back;
    SUMOReal angle = 0.;
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
        front = lane->getShape().positionAtOffset2D(carriageOffset);
        back = backLane->getShape().positionAtOffset2D(carriageBackOffset);
        if (front == back) {
            // no place for drawing available
            continue;
        }
        angle = atan2((front.x() - back.x()), (back.y() - front.y())) * (SUMOReal) 180.0 / (SUMOReal) PI;
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
            glVertex2d(-halfWidth, carriageLength - yCornerCut);
            glVertex2d(-halfWidth + xCornerCut, carriageLength);
            glVertex2d(halfWidth - xCornerCut, carriageLength);
            glVertex2d(halfWidth, carriageLength - yCornerCut);
            glVertex2d(halfWidth, yCornerCut);
            glVertex2d(halfWidth - xCornerCut, 0);
            glEnd();
        }
        glPopMatrix();
        carriageOffset -= carriageLengthWithGap;
        carriageBackOffset -= carriageLengthWithGap;
        GLHelper::setColor(current);
    }
    myCarriageLength = front.distanceTo2D(back);
    // restore matrices
    glPushMatrix();
    glTranslated(front.x(), front.y(), getType());
    glRotated(angle, 0, 0, 1);
    glPushMatrix();
}


const Position&
GUIVehicle::getSeatPosition(size_t personIndex) const {
    /// if there are not enough seats in the vehicle people have to squeeze onto the last seat
    return mySeatPositions[(int)MIN2(personIndex, mySeatPositions.size() - 1)];
}


int
GUIVehicle::getNumPassengers() const {
    if (myPersonDevice != 0) {
        return (int)myPersonDevice->getPersons().size();
    }
    return 0;
}


void
GUIVehicle::computeSeats(const Position& front, const Position& back, int& requiredSeats) const {
    if (requiredSeats <= 0) {
        return; // save some work
    }
    const Line l(front, back);
    const SUMOReal length = l.length2D();
    if (length < 4) {
        // small vehicle, sit at the center
        mySeatPositions.push_back(l.getPositionAtDistance2D(length / 2));
        requiredSeats--;
    } else {
        for (SUMOReal p = 2; p <= length - 1; p += 1) {
            mySeatPositions.push_back(l.getPositionAtDistance2D(p));
            requiredSeats--;
        }
    }
}


SUMOReal
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
    } else {
        return "";
    }
    if (myStops.front().triggered) {
        result += ", triggered";
    } else if (myStops.front().containerTriggered) {
        result += ", containerTriggered";
    } else {
        result += ", duration=" + time2string(myStops.front().duration);
    }
    return result;
}


void
GUIVehicle::selectBlockingFoes() const {
    SUMOReal dist = myLane->getLength() - getPositionOnLane();
    for (DriveItemVector::const_iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
        const DriveProcessItem& dpi = *i;
        if (dpi.myLink == 0) {
            continue;
        }
        std::vector<const SUMOVehicle*> blockingFoes;
        std::vector<const MSPerson*> blockingPersons;
        dpi.myLink->opened(dpi.myArrivalTime, dpi.myArrivalSpeed, dpi.getLeaveSpeed(), getVehicleType().getLength(),
                           getImpatience(), getCarFollowModel().getMaxDecel(), getWaitingTime(), &blockingFoes);
        for (std::vector<const SUMOVehicle*>::const_iterator it = blockingFoes.begin(); it != blockingFoes.end(); ++it) {
            gSelected.select(static_cast<const GUIVehicle*>(*it)->getGlID());
        }
#ifdef HAVE_INTERNAL_LANES
        const MSLink::LinkLeaders linkLeaders = (dpi.myLink)->getLeaderInfo(dist, getVehicleType().getMinGap(), &blockingPersons);
        for (MSLink::LinkLeaders::const_iterator it = linkLeaders.begin(); it != linkLeaders.end(); ++it) {
            // the vehicle to enter the junction first has priority
            const GUIVehicle* leader = dynamic_cast<const GUIVehicle*>(it->vehAndGap.first);
            if (leader != 0) {
                if (leader->myLinkLeaders[dpi.myLink->getJunction()].count(getID()) == 0) {
                    // leader isn't already following us, now we follow it
                    gSelected.select(leader->getGlID());
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
#endif
        dist += dpi.myLink->getViaLaneOrLane()->getLength();
    }
}

/****************************************************************************/

