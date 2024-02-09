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
/// @file    GUIVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
#include <config.h>

#include <cmath>
#include <vector>
#include <string>
#include <bitset>
#include <utils/common/MsgHandler.h>
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
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSStop.h>
#include <microsim/MSParkingArea.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSRoutingEngine.h>
#include <microsim/devices/MSDevice_Transportable.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <microsim/devices/MSDevice_ElecHybrid.h>
#include <microsim/devices/MSDevice_Battery.h>
#include <microsim/transportables/MSTrainHelper.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include "GUIVehicle.h"
#include "GUIPerson.h"
#include "GUIContainer.h"
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUILane.h"

#define SPEEDMODE_DEFAULT 31
#define LANECHANGEMODE_DEFAULT 1621

//#define DEBUG_FOES

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation


/* -------------------------------------------------------------------------
 * GUIVehicle - methods
 * ----------------------------------------------------------------------- */

GUIVehicle::GUIVehicle(SUMOVehicleParameter* pars, ConstMSRoutePtr route,
                       MSVehicleType* type, const double speedFactor) :
    MSVehicle(pars, route, type, speedFactor),
    GUIBaseVehicle((MSBaseVehicle&) * this) {
}


GUIVehicle::~GUIVehicle() {
}


GUIParameterTableWindow*
GUIVehicle::getParameterWindow(GUIMainWindow& app,
                               GUISUMOAbstractView&) {
    const bool isElecHybrid = getDevice(typeid(MSDevice_ElecHybrid)) != nullptr ? true : false;
    const bool hasBattery = getDevice(typeid(MSDevice_Battery)) != nullptr;
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem(TL("lane [id]"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getLaneID));
    if (MSGlobals::gSublane) {
        ret->mkItem(TL("shadow lane [id]"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getShadowLaneID));
    }
    if (MSGlobals::gLateralResolution > 0) {
        ret->mkItem(TL("target lane [id]"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getTargetLaneID));
    }
    if (isSelected()) {
        ret->mkItem(TL("back lanes [id,..]"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getBackLaneIDs));
    }
    ret->mkItem(TL("position [m]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getPositionOnLane));
    ret->mkItem(TL("lateral offset [m]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getLateralPositionOnLane));
    ret->mkItem(TL("speed [m/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getSpeed));
    ret->mkItem(TL("lateral speed [m/s]"), true,
                new FunctionBinding<MSAbstractLaneChangeModel, double>(&getLaneChangeModel(), &MSAbstractLaneChangeModel::getSpeedLat));
    ret->mkItem(TL("acceleration [m/s^2]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getAcceleration));
    ret->mkItem(TL("angle [degree]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &GUIBaseVehicle::getNaviDegree));
    ret->mkItem(TL("slope [degree]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getSlope));
    ret->mkItem(TL("speed factor"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getChosenSpeedFactor));
    ret->mkItem(TL("time gap on lane [s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getTimeGapOnLane));
    ret->mkItem(TL("waiting time [s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getWaitingSeconds));
    ret->mkItem(TLF("waiting time (accumulated, % s) [s]", time2string(MSGlobals::gWaitingTimeMemory)).c_str(), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getAccumulatedWaitingSeconds));
    ret->mkItem(TL("time since startup [s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getTimeSinceStartupSeconds));
    ret->mkItem(TL("time loss [s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getTimeLossSeconds));
    ret->mkItem(TL("impatience"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getImpatience));
    ret->mkItem(TL("last lane change [s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem(TL("desired depart [s]"), false, time2string(getParameter().depart));
    ret->mkItem(TL("depart delay [s]"), false, time2string(getDepartDelay()));
    ret->mkItem(TL("odometer [m]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSBaseVehicle::getOdometer));
    if (getParameter().repetitionNumber < std::numeric_limits<int>::max()) {
        ret->mkItem(TL("remaining [#]"), false, (int) getParameter().repetitionNumber - getParameter().repetitionsDone);
    }
    if (getParameter().repetitionOffset > 0) {
        ret->mkItem(TL("insertion period [s]"), false, time2string(getParameter().repetitionOffset));
    }
    if (getParameter().repetitionProbability > 0) {
        ret->mkItem(TL("insertion probability"), false, getParameter().repetitionProbability);
    }
    ret->mkItem(TL("stop info"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getStopInfo));
    ret->mkItem(TL("line"), false, myParameter->line);
    ret->mkItem(TL("CO2 [mg/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getEmissions<PollutantsInterface::CO2>));
    ret->mkItem(TL("CO [mg/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getEmissions<PollutantsInterface::CO>));
    ret->mkItem(TL("HC [mg/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getEmissions<PollutantsInterface::HC>));
    ret->mkItem(TL("NOx [mg/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getEmissions<PollutantsInterface::NO_X>));
    ret->mkItem(TL("PMx [mg/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getEmissions<PollutantsInterface::PM_X>));
    ret->mkItem(TL("fuel [mg/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getEmissions<PollutantsInterface::FUEL>));
    ret->mkItem(TL("electricity [Wh/s]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getEmissions<PollutantsInterface::ELEC>));
    ret->mkItem(TL("noise (Harmonoise) [dB]"), true,
                new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getHarmonoise_NoiseEmissions));
    ret->mkItem(TL("devices"), false, getDeviceDescription());
    ret->mkItem(TL("persons"), true,
                new FunctionBinding<GUIVehicle, int>(this, &MSVehicle::getPersonNumber));
    ret->mkItem(TL("containers"), true,
                new FunctionBinding<GUIVehicle, int>(this, &MSVehicle::getContainerNumber));
    ret->mkItem(TL("lcState right"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getLCStateRight));
    ret->mkItem(TL("lcState left"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getLCStateLeft));
    // close building
    if (MSGlobals::gLateralResolution > 0) {
        ret->mkItem(TL("lcState center"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getLCStateCenter));
        ret->mkItem(TL("right side on edge [m]"), true, new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getRightSideOnEdge2));
        ret->mkItem(TL("left side on edge [m]"), true, new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getLeftSideOnEdge));
        ret->mkItem(TL("rightmost edge sublane [#]"), true, new FunctionBinding<GUIVehicle, int>(this, &GUIVehicle::getRightSublaneOnEdge));
        ret->mkItem(TL("leftmost edge sublane [#]"), true, new FunctionBinding<GUIVehicle, int>(this, &GUIVehicle::getLeftSublaneOnEdge));
        ret->mkItem(TL("lane change maneuver distance [m]"), true, new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::getManeuverDist));
    }
    if (hasBattery || isElecHybrid) {
        ret->mkItem(TL("present state of charge [Wh]"), true,
                    new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getStateOfCharge));
    }
    if (hasBattery) {
        ret->mkItem(TL("relative state of charge (SoC) [-]"), true,
                    new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getRelativeStateOfCharge));
        ret->mkItem(TL("currently charging [Wh]"), true,
                    new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getChargedEnergy));
    }
    if (isElecHybrid) {
        ret->mkItem(TL("present electric current [A]"), true,
                    new FunctionBinding<GUIVehicle, double>(this, &MSVehicle::getElecHybridCurrent));
    }
    if (hasInfluencer()) {
        if (getInfluencer().getSpeedMode() != SPEEDMODE_DEFAULT) {
            ret->mkItem(TL("speed mode"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getSpeedMode));
        }
        if (getInfluencer().getLaneChangeMode() != LANECHANGEMODE_DEFAULT) {
            ret->mkItem(TL("lane change mode"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getLaneChangeMode));
        }
    }
    ret->closeBuilding(&getParameter());
    return ret;
}


GUIParameterTableWindow*
GUIVehicle::getTypeParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, "vType:" + myType->getID());
    ret->mkItem(TL("length [m]"), false, myType->getLength());
    ret->mkItem(TL("width [m]"), false, myType->getWidth());
    ret->mkItem(TL("height [m]"), false, myType->getHeight());
    ret->mkItem(TL("minGap [m]"), false, myType->getMinGap());
    ret->mkItem(TL("vehicle class"), false, SumoVehicleClassStrings.getString(myType->getVehicleClass()));
    ret->mkItem(TL("emission class"), false, PollutantsInterface::getName(myType->getEmissionClass()));
    ret->mkItem(TL("mass [kg]"), false, myType->getMass());
    ret->mkItem(TL("car-following model"), false, SUMOXMLDefinitions::CarFollowModels.getString((SumoXMLTag)getCarFollowModel().getModelID()));
    ret->mkItem(TL("lane-change model"), false, SUMOXMLDefinitions::LaneChangeModels.getString(getLaneChangeModel().getModelID()));
    ret->mkItem(TL("guiShape"), false, getVehicleShapeName(myType->getGuiShape()));
    ret->mkItem(TL("maximum speed [m/s]"), false, getVehicleType().getMaxSpeed());
    ret->mkItem(TL("desired maximum speed [m/s]"), false, getVehicleType().getDesiredMaxSpeed());
    ret->mkItem(TL("maximum acceleration [m/s^2]"), false, getCarFollowModel().getMaxAccel());
    ret->mkItem(TL("maximum deceleration [m/s^2]"), false, getCarFollowModel().getMaxDecel());
    ret->mkItem(TL("emergency deceleration [m/s^2]"), false, getCarFollowModel().getEmergencyDecel());
    ret->mkItem(TL("apparent deceleration [m/s^2]"), false, getCarFollowModel().getApparentDecel());
    ret->mkItem(TL("imperfection (sigma)"), false, getCarFollowModel().getImperfection());
    ret->mkItem(TL("desired headway (tau) [s]"), false, getCarFollowModel().getHeadwayTime());
    ret->mkItem(TL("speedfactor"), false, myType->getParameter().speedFactor.toStr(gPrecision));
    if (myType->getParameter().wasSet(VTYPEPARS_ACTIONSTEPLENGTH_SET)) {
        ret->mkItem(TL("action step length [s]"), false, myType->getActionStepLengthSecs());
    }
    ret->mkItem(TL("person capacity"), false, myType->getPersonCapacity());
    ret->mkItem(TL("boarding time [s]"), false, STEPS2TIME(myType->getLoadingDuration(true)));
    ret->mkItem(TL("container capacity"), false, myType->getContainerCapacity());
    ret->mkItem(TL("loading time [s]"), false, STEPS2TIME(myType->getLoadingDuration(false)));
    if (MSGlobals::gLateralResolution > 0) {
        ret->mkItem(TL("minGapLat [m]"), false, myType->getMinGapLat());
        ret->mkItem(TL("maxSpeedLat [m/s]"), false, myType->getMaxSpeedLat());
        ret->mkItem(TL("latAlignment"), true, new FunctionBindingString<GUIVehicle>(this, &GUIVehicle::getDynamicAlignment));
    } else if (MSGlobals::gLaneChangeDuration > 0) {
        ret->mkItem(TL("maxSpeedLat [m/s]"), false, myType->getMaxSpeedLat());
    }
    for (auto item : myType->getParameter().lcParameter) {
        ret->mkItem(toString(item.first).c_str(), false, toString(item.second));
    }
    for (auto item : myType->getParameter().jmParameter) {
        ret->mkItem(toString(item.first).c_str(), false, toString(item.second));
    }
    if (MSGlobals::gModelParkingManoeuver) {
        ret->mkItem(TL("manoeuver Angle vs Times"), false, myType->getParameter().getManoeuverAngleTimesS());
    }
    ret->closeBuilding(&(myType->getParameter()));
    return ret;
}


std::string
GUIVehicle::getDynamicAlignment() const {
    std::string align = myType->getPreferredLateralAlignment() == LatAlignmentDefinition::GIVEN
                        ? toString(myType->getPreferredLateralAlignmentOffset())
                        : toString(myType->getPreferredLateralAlignment());
    std::string align2 = toString(getLaneChangeModel().getDesiredAlignment());
    if (align2 != align) {
        align = align2 + " (default: " + align + ")";
    }
    return align;
}

void
GUIVehicle::drawAction_drawLinkItems(const GUIVisualizationSettings& s) const {
    glTranslated(0, 0, getType() + .2); // draw on top of cars
    for (DriveItemVector::const_iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
        if ((*i).myLink == nullptr) {
            continue;
        }
        MSLink* link = (*i).myLink;
        MSLane* via = link->getViaLaneOrLane();
        if (via != nullptr) {
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


void
GUIVehicle::drawAction_drawCarriageClass(const GUIVisualizationSettings& s, bool asImage) const {
    RGBColor current = GLHelper::getColor();
    RGBColor darker = current.changedBrightness(-51);
    const double exaggeration = (s.vehicleSize.getExaggeration(s, this)
                                 * s.vehicleScaler.getScheme().getColor(getScaleValue(s, s.vehicleScaler.getActive())));
    if (exaggeration == 0) {
        return;
    }
    // bool reversed = 
    MSTrainHelper trainHelper(this, isReversed() && s.drawReversed, s.secondaryShape);
    int numCarriages = trainHelper.getNumCarriages();
    const int firstPassengerCarriage = trainHelper.getDefaultLength() == trainHelper.getLocomotiveLength() || numCarriages == 1
                                       || (getVClass() & (SVC_RAIL_ELECTRIC | SVC_RAIL_FAST | SVC_RAIL)) == 0 ? 0 : 1;
    const int noPersonsBackCarriages = (getVehicleType().getGuiShape() == SUMOVehicleShape::TRUCK_SEMITRAILER || getVehicleType().getGuiShape() == SUMOVehicleShape::TRUCK_1TRAILER) && numCarriages > 1 ? 1 : 0;
    const int firstContainerCarriage = numCarriages == 1 || getVehicleType().getGuiShape() == SUMOVehicleShape::TRUCK_1TRAILER ? 0 : 1;
    const int seatsPerCarriage = (int)ceil(getVType().getPersonCapacity() / (numCarriages - firstPassengerCarriage - noPersonsBackCarriages));
    const int containersPerCarriage = (int)ceil(getVType().getContainerCapacity() / (numCarriages - firstContainerCarriage));
    // Handle seats.
    int requiredSeats = getNumPassengers();
    int requiredPositions = getNumContainers();
    if (requiredSeats > 0) {
        mySeatPositions.clear();
    }
    if (requiredPositions > 0) {
        myContainerPositions.clear();
    }
    GLHelper::popMatrix(); // undo initial translation and rotation
    const double xCornerCut = 0.3 * exaggeration;
    const double yCornerCut = 0.4 * exaggeration;
    Position front, back;
    double angle = 0.0;
    double curCLength = trainHelper.getFirstCarriageLength();
    std::vector<MSTrainHelper::Carriage*> carriages = trainHelper.getCarriages();
    for (int i = 0; i < numCarriages; ++i) {
        front = carriages[i]->front;
        back = carriages[i]->back;
        if (front == back) {
            // No place for drawing available.
            continue;
        }
        const double drawnCarriageLength = front.distanceTo2D(back);
        angle = atan2((front.x() - back.x()), (back.y() - front.y())) * (double) 180.0 / (double) M_PI;
        // if we are in reverse 'first' carriages are drawn last so the >= test doesn't work
        const bool reversed = trainHelper.isReversed();
        if (reversed) {
            if (i <= numCarriages - firstPassengerCarriage) {
                computeSeats(back, front, SUMO_const_waitingPersonWidth, seatsPerCarriage, exaggeration, requiredSeats, mySeatPositions);
            }
            if (i <= numCarriages - firstContainerCarriage) {
                computeSeats(front, back, SUMO_const_waitingContainerWidth, containersPerCarriage, exaggeration, requiredPositions, myContainerPositions);
            }
        } else {
            if (i >= firstPassengerCarriage) {
                computeSeats(front, back, SUMO_const_waitingPersonWidth, seatsPerCarriage, exaggeration, requiredSeats, mySeatPositions);
            }
            if (i >= firstContainerCarriage) {
                computeSeats(front, back, SUMO_const_waitingContainerWidth, containersPerCarriage, exaggeration, requiredPositions, myContainerPositions);
            }
        }
        curCLength = (i == trainHelper.getFirstCarriageNo() ? trainHelper.getFirstCarriageLength() : trainHelper.getCarriageLength());
        GLHelper::pushMatrix();
        glTranslated(front.x(), front.y(), getType());
        glRotated(angle, 0, 0, 1);
        double halfWidth = trainHelper.getHalfWidth();
        if (!asImage || !GUIBaseVehicleHelper::drawAction_drawVehicleAsImage(s, getVType().getImgFile(), this, getVType().getWidth() * exaggeration, curCLength)) {
            switch (getVType().getGuiShape()) {
                case SUMOVehicleShape::TRUCK_SEMITRAILER:
                case SUMOVehicleShape::TRUCK_1TRAILER:
                    if (i == trainHelper.getFirstCarriageNo()) {  // at the moment amReversed is only ever set for rail - so has no impact in this call
                        GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, getVType().getGuiShape(), getVType().getWidth() * exaggeration, curCLength, 0, false, reversed);
                    } else {
                        GLHelper::setColor(current);
                        GLHelper::drawBoxLine(Position(0, 0), 180, curCLength, halfWidth);
                    }
                    break;
                default: {
                    if (i == trainHelper.getFirstCarriageNo()) {
                        GLHelper::setColor(darker);
                    } else {
                        GLHelper::setColor(current);
                    }
                    // generic rail carriage
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
                    // indicate front of the head of the train
                    if (i == trainHelper.getFirstCarriageNo()) {
                        glTranslated(0, 0, 0.1);
                        glColor3d(0, 0, 0);
                        glBegin(GL_TRIANGLE_FAN);
                        if (reversed) {  // not quite correct as its drawing at the wrong end of the locomotive - however useful as visual indicator of reverse?
                            glVertex2d(-halfWidth + xCornerCut, yCornerCut);
                            glVertex2d(-halfWidth + 2 * xCornerCut, 3 * yCornerCut);
                            glVertex2d(halfWidth - 2 * xCornerCut, 3 * yCornerCut);
                            glVertex2d(halfWidth - xCornerCut, yCornerCut);
                        } else {
                            glVertex2d(-halfWidth + 2 * xCornerCut, yCornerCut);
                            glVertex2d(-halfWidth + xCornerCut, 3 * yCornerCut);
                            glVertex2d(halfWidth - xCornerCut, 3 * yCornerCut);
                            glVertex2d(halfWidth - 2 * xCornerCut, yCornerCut);
                        }
                        glEnd();
                        glTranslated(0, 0, -0.1);
                    }
                }
            }
        }
        GLHelper::popMatrix();
    }
    if (getVType().getGuiShape() == SUMOVehicleShape::RAIL_CAR) {
        GLHelper::pushMatrix();
        glTranslated(front.x(), front.y(), getType());
        glRotated(angle, 0, 0, 1);
        drawAction_drawVehicleBlinker(curCLength);
        drawAction_drawVehicleBrakeLight(curCLength);
        GLHelper::popMatrix();
    }
    // restore matrix
    GLHelper::pushMatrix();
    front = getPosition();
    glTranslated(front.x(), front.y(), getType());
    const double degAngle = RAD2DEG(getAngle() + M_PI / 2.);
    glRotated(degAngle, 0, 0, 1);
    glScaled(exaggeration, trainHelper.getUpscaleLength(), 1);
    if (mySeatPositions.size() == 0) {
        mySeatPositions.push_back(Seat(back, DEG2RAD(angle)));
    }
    if (myContainerPositions.size() == 0) {
        myContainerPositions.push_back(Seat(back, DEG2RAD(angle)));
    }
}

#define BLINKER_POS_FRONT .5
#define BLINKER_POS_BACK .5

inline void
drawAction_drawBlinker(double dir, double length) {
    glColor3d(1.f, .8f, 0);
    GLHelper::pushMatrix();
    glTranslated(dir, BLINKER_POS_FRONT, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    GLHelper::popMatrix();
    GLHelper::pushMatrix();
    glTranslated(dir, length - BLINKER_POS_BACK, -0.1);
    GLHelper::drawFilledCircle(.5, 6);
    GLHelper::popMatrix();
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
        drawAction_drawBlinker(offset, length);
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
    GLHelper::pushMatrix();
    if (onlyOne) {
        glTranslated(0, length, -0.1);
        GLHelper::drawFilledCircle(.5, 6);
    } else {
        glTranslated(-getVehicleType().getWidth() * 0.5, length, -0.1);
        GLHelper::drawFilledCircle(.5, 6);
        GLHelper::popMatrix();
        GLHelper::pushMatrix();
        glTranslated(getVehicleType().getWidth() * 0.5, length, -0.1);
        GLHelper::drawFilledCircle(.5, 6);
    }
    GLHelper::popMatrix();
}

inline void
GUIVehicle::drawAction_drawVehicleBlueLight() const {
    if (signalSet(MSVehicle::VEH_SIGNAL_EMERGENCY_BLUE)) {
        GLHelper::pushMatrix();
        glTranslated(0, 2.5, .5);
        glColor3f(0, 0, 1);
        GLHelper::drawFilledCircle(.5, 6);
        GLHelper::popMatrix();
    }
}


double
GUIVehicle::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
    switch (activeScheme) {
        case 8:
            if (isStopped()) {
                return isParking() ? -2 : -1;
            }
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
            return getEmissions<PollutantsInterface::CO2>();
        case 15:
            return getEmissions<PollutantsInterface::CO>();
        case 16:
            return getEmissions<PollutantsInterface::PM_X>();
        case 17:
            return getEmissions<PollutantsInterface::NO_X>();
        case 18:
            return getEmissions<PollutantsInterface::HC>();
        case 19:
            return getEmissions<PollutantsInterface::FUEL>();
        case 20:
            return getHarmonoise_NoiseEmissions();
        case 21:
            return getNumberReroutes();
        case 22:
            return gSelected.isSelected(GLO_VEHICLE, getGlID());
        case 23:
            return getLaneChangeModel().isOpposite() ? -100 : getBestLaneOffset();
        case 24:
            return getAcceleration();
        case 25:
            return getTimeGapOnLane();
        case 26:
            return STEPS2TIME(getDepartDelay());
        case 27:
            return getEmissions<PollutantsInterface::ELEC>();
        case 28:
            return getRelativeStateOfCharge();
        case 29:
            return getChargedEnergy();
        case 30:
            return getTimeLossSeconds();
        case 31:
            return getStopDelay();
        case 32:
            return getStopArrivalDelay();
        case 33:
            return getLaneChangeModel().getSpeedLat();
        case 34: // by numerical param value
            std::string error;
            std::string val = getPrefixedParameter(s.vehicleParam, error);
            try {
                if (val == "") {
                    return GUIVisualizationSettings::MISSING_DATA;
                } else {
                    return StringUtils::toDouble(val);
                }
            } catch (NumberFormatException&) {
                try {
                    return StringUtils::toBool(val);
                } catch (BoolFormatException&) {
                    WRITE_WARNINGF(TL("Vehicle parameter '%' key '%' is not a number for vehicle '%'."),
                                   myParameter->getParameter(s.vehicleParam, "0"), s.vehicleParam, getID());
                    return GUIVisualizationSettings::MISSING_DATA;
                }
            }
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
GUIVehicle::drawRouteHelper(const GUIVisualizationSettings& s, ConstMSRoutePtr r, bool future, bool noLoop, const RGBColor& col) const {
    const double exaggeration = s.vehicleSize.getExaggeration(s, this) * (s.gaming ? 0.5 : 1);
    MSRouteIterator start = future ? myCurrEdge : r->begin();
    MSRouteIterator i = start;
    const std::vector<MSLane*>& bestLaneConts = getBestLanesContinuation();
    // draw continuation lanes when drawing the current route where available
    int bestLaneIndex = (r == myRoute ? 0 : (int)bestLaneConts.size());
    std::map<const MSLane*, int> repeatLane; // count repeated occurrences of the same edge
    const double textSize = s.vehicleName.size / s.scale;
    const GUILane* prevLane = nullptr;
    int reversalIndex = 0;
    const int indexDigits = (int)toString(r->size()).size();
    if (!isOnRoad() && !isParking()) {
        // simulation time has already advanced so isRemoteControlled is always false
        const std::string offRoadLabel = hasInfluencer() && getInfluencer()->isRemoteAffected(SIMSTEP) ? "offRoad" : "teleporting";
        GLHelper::drawTextSettings(s.vehicleValue, offRoadLabel, getPosition(), s.scale, s.angle, 1.0);
    } else if (myLane->isInternal()) {
        bestLaneIndex++;
    }
    const bool s2 = s.secondaryShape;
    for (; i != r->end(); ++i) {
        const GUILane* lane;
        if (bestLaneIndex < (int)bestLaneConts.size() && bestLaneConts[bestLaneIndex] != 0 && (*i) == &(bestLaneConts[bestLaneIndex]->getEdge())) {
            lane = static_cast<GUILane*>(bestLaneConts[bestLaneIndex]);
            ++bestLaneIndex;
        } else {
            const std::vector<MSLane*>* allowed = (*i)->allowedLanes(getVClass());
            if (allowed != nullptr && allowed->size() != 0) {
                lane = static_cast<GUILane*>((*allowed)[0]);
            } else {
                lane = static_cast<GUILane*>((*i)->getLanes()[0]);
            }
        }
        GLHelper::setColor(col);
        GLHelper::drawBoxLines(lane->getShape(s2), lane->getShapeRotations(s2), lane->getShapeLengths(s2), exaggeration);
        if (prevLane != nullptr && lane->getBidiLane() == prevLane) {
            // indicate train reversal
            std::string label = "reverse:" + toString(reversalIndex++);
            if (s.showRouteIndex) {
                label += "@r" + toString((int)(i - myCurrEdge));
            }
            Position pos = lane->geometryPositionAtOffset(lane->getLength() / 2) - Position(0, textSize * repeatLane[lane]);
            GLHelper::drawTextSettings(s.vehicleValue, label, pos, s.scale, s.angle, 1.0);
        }
        if (s.showRouteIndex) {
            std::string label = toString((int)(i - myCurrEdge));
            const double laneAngle = lane->getShape(s2).angleAt2D(0);
            Position pos = lane->getShape(s2).front() - Position(0, textSize * repeatLane[lane]) + Position(
                               (laneAngle >= -0.25 * M_PI && laneAngle < 0.75 * M_PI ? 1 : -1) * 0.4 * indexDigits * textSize, 0);
            //GLHelper::drawText(label, pos, 1.0, textSize, s.vehicleName.color);
            GLHelper::drawTextSettings(s.vehicleName, label, pos, s.scale, s.angle, 1.0);
        }
        repeatLane[lane]++;
        prevLane = lane;
        if (noLoop && i != start && (*i) == (*start)) {
            break;
        }
    }
    drawStopLabels(s, noLoop, col);
    drawParkingInfo(s, col);
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
    } else if (myStops.front().pars.collision) {
        result += ", collision";
    } else if (myStops.front().pars.until != -1) {
        result += ", until=" + time2string(myStops.front().pars.until);
    } else {
        result += ", duration=" + time2string(myStops.front().duration);
    }
    if (myStops.front().pars.actType != "") {
        result += ", actType=" + myStops.front().pars.actType;
    }
    return result;
}


void
GUIVehicle::selectBlockingFoes() const {
    double dist = myLane->getLength() - getPositionOnLane();
#ifdef DEBUG_FOES
    std::cout << SIMTIME << " selectBlockingFoes veh=" << getID() << " dist=" << dist << " numLinks=" << myLFLinkLanes.size() << "\n";
#endif
    for (DriveItemVector::const_iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
        const DriveProcessItem& dpi = *i;
        if (dpi.myLink == nullptr) {
            /// XXX if the vehicle intends to stop on an intersection, there could be a relevant exitLink (see #4299)
            continue;
        }
        MSLink::BlockingFoes blockingFoes;
        std::vector<const MSPerson*> blockingPersons;
#ifdef DEBUG_FOES
        std::cout << "   foeLink=" << dpi.myLink->getViaLaneOrLane()->getID() << "\n";
        const bool isOpen =
#endif
            dpi.myLink->opened(dpi.myArrivalTime, dpi.myArrivalSpeed, dpi.getLeaveSpeed(), getVehicleType().getLength(),
                               getImpatience(), getCarFollowModel().getMaxDecel(), getWaitingTime(), getLateralPositionOnLane(), &blockingFoes, false, this);
#ifdef DEBUG_FOES
        if (!isOpen) {
            std::cout << "     closed due to:\n";
            for (const auto& item : blockingFoes) {
                std::cout << "   " << item->getID() << "\n";
            }
        }
#endif
        if (getLaneChangeModel().getShadowLane() != nullptr) {
            MSLink* parallelLink = dpi.myLink->getParallelLink(getLaneChangeModel().getShadowDirection());
            if (parallelLink != nullptr) {
                const double shadowLatPos = getLateralPositionOnLane() - getLaneChangeModel().getShadowDirection() * 0.5 * (
                                                myLane->getWidth() + getLaneChangeModel().getShadowLane()->getWidth());
#ifdef DEBUG_FOES
                const bool isShadowOpen =
#endif
                    parallelLink->opened(dpi.myArrivalTime, dpi.myArrivalSpeed, dpi.getLeaveSpeed(),
                                         getVehicleType().getLength(), getImpatience(),
                                         getCarFollowModel().getMaxDecel(),
                                         getWaitingTime(), shadowLatPos, &blockingFoes, false, this);
#ifdef DEBUG_FOES
                if (!isShadowOpen) {
                    std::cout <<  "    foes at shadow link=" << parallelLink->getViaLaneOrLane()->getID() << ":\n";
                    for (const auto& item : blockingFoes) {
                        std::cout << "   " << item->getID() << "\n";
                    }
                }
#endif
            }
        }
        for (const auto& item : blockingFoes) {
            gSelected.select(static_cast<const GUIVehicle*>(item)->getGlID());
        }
#ifdef DEBUG_FOES
        gDebugFlag1 = true;
#endif
        const MSLink::LinkLeaders linkLeaders = (dpi.myLink)->getLeaderInfo(this, dist, &blockingPersons);
#ifdef DEBUG_FOES
        gDebugFlag1 = false;
#endif
        for (MSLink::LinkLeaders::const_iterator it = linkLeaders.begin(); it != linkLeaders.end(); ++it) {
            // the vehicle to enter the junction first has priority
            const GUIVehicle* leader = dynamic_cast<const GUIVehicle*>(it->vehAndGap.first);
            if (leader != nullptr) {
                if (isLeader(dpi.myLink, leader, it->vehAndGap.second) || it->inTheWay()) {
                    gSelected.select(leader->getGlID());
#ifdef DEBUG_FOES
                    std::cout << "      linkLeader=" << leader->getID() << "\n";
#endif
                }
            } else {
                for (std::vector<const MSPerson*>::iterator it_p = blockingPersons.begin(); it_p != blockingPersons.end(); ++it_p) {
                    const GUIPerson* foe = dynamic_cast<const GUIPerson*>(*it_p);
                    if (foe != nullptr) {
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
    if (view != nullptr) {
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
    return (int)sublaneSides.size() - 1;
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


std::string
GUIVehicle::getLCStateRight() const {
    return toString((LaneChangeAction)getLaneChangeModel().getSavedState(-1).second);
}

std::string
GUIVehicle::getLCStateLeft() const {
    return toString((LaneChangeAction)getLaneChangeModel().getSavedState(1).second);
}

std::string
GUIVehicle::getLCStateCenter() const {
    return toString((LaneChangeAction)getLaneChangeModel().getSavedState(0).second);
}

std::string
GUIVehicle::getLaneID() const {
    return Named::getIDSecure(myLane, "n/a");
}

std::string
GUIVehicle::getBackLaneIDs() const {
    return toString(myFurtherLanes);
}

std::string
GUIVehicle::getShadowLaneID() const {
    return Named::getIDSecure(getLaneChangeModel().getShadowLane(), "");
}

std::string
GUIVehicle::getTargetLaneID() const {
    return Named::getIDSecure(getLaneChangeModel().getTargetLane(), "");
}

double
GUIVehicle::getManeuverDist() const {
    return getLaneChangeModel().getManeuverDist();
}

std::string
GUIVehicle::getSpeedMode() const {
    return std::bitset<6>(getInfluencer()->getSpeedMode()).to_string();
}

std::string
GUIVehicle::getLaneChangeMode() const {
    return std::bitset<12>(getInfluencer()->getLaneChangeMode()).to_string();
}

void
GUIVehicle::rerouteDRTStop(MSStoppingPlace* busStop) {
    SUMOTime intermediateDuration = TIME2STEPS(20);
    SUMOTime finalDuration = SUMOTime_MAX;
    if (myParameter->stops.size() >= 2) {
        // copy durations from the original stops
        intermediateDuration = myParameter->stops.front().duration;
        finalDuration = myParameter->stops.back().duration;
    }
    // if the stop is already in the list of stops, cancel all stops that come
    // after it and set the stop duration
    std::string line = "";
    int destinations = 0;
    bool add = true;
    for (auto it = myStops.begin(); it != myStops.end(); it++) {
        if (!it->reached && destinations < 2 && it->busstop != nullptr) {
            line += it->busstop->getID();
            destinations++;
        }
        if (it->busstop == busStop) {
            it->duration = finalDuration;
            myStops.erase(++it, myStops.end());
            add = false;
            break;
        } else {
            it->duration = MIN2(it->duration, intermediateDuration);
        }
    }
    if (destinations < 2) {
        line += busStop->getID();
    }
    if (add) {
        // create new stop
        SUMOVehicleParameter::Stop stopPar;
        stopPar.busstop = busStop->getID();
        stopPar.lane = busStop->getLane().getID();
        stopPar.startPos = busStop->getBeginLanePosition();
        stopPar.endPos = busStop->getEndLanePosition();
        stopPar.duration = finalDuration;
        stopPar.until = -1;
        stopPar.triggered = false;
        stopPar.containerTriggered = false;
        stopPar.parking = ParkingType::ONROAD;
        stopPar.index = STOP_INDEX_FIT;
        stopPar.parametersSet = STOP_START_SET | STOP_END_SET;
        // clean up prior route to improve visualisation, ensure that the stop can be added immediately
        ConstMSEdgeVector edges = myRoute->getEdges();
        edges.erase(edges.begin(), edges.begin() + getRoutePosition());
        edges.push_back(&busStop->getLane().getEdge());
        replaceRouteEdges(edges, -1, 0, "DRT.tmp", false, false, false);
        std::string errorMsg;
        // add stop
        addStop(stopPar, errorMsg);
    }
    const bool hasReroutingDevice = getDevice(typeid(MSDevice_Routing)) != nullptr;
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = hasReroutingDevice
            ? MSRoutingEngine::getRouterTT(getRNGIndex(), getVClass())
            : MSNet::getInstance()->getRouterTT(getRNGIndex());
    // reroute to ensure the new stop is reached
    reroute(MSNet::getInstance()->getCurrentTimeStep(), "DRT", router);
    myParameter->line = line;
    assert(haveValidStopEdges());
}

Position
GUIVehicle::getVisualPosition(bool s2, const double offset) const {
    if (s2) {
        // see MSVehicle::getPosition
        if (myLane == nullptr) {
            return Position::INVALID;
        }
        if (isParking()) {
            if (myStops.begin()->parkingarea != nullptr) {
                return myStops.begin()->parkingarea->getVehiclePosition(*this);
            } else {
                // position beside the road
                PositionVector shp = myLane->getEdge().getLanes()[0]->getShape(s2);
                shp.move2side(SUMO_const_laneWidth * (MSGlobals::gLefthand ? -1 : 1));
                return shp.positionAtOffset((getPositionOnLane() + offset) * myLane->getLengthGeometryFactor(s2));
            }
        }
        const PositionVector& shape = myLane->getShape(s2);
        const double posLat = (MSGlobals::gLefthand ? 1 : -1) * getLateralPositionOnLane();
        return shape.positionAtOffset((getPositionOnLane() + offset) * myLane->getLengthGeometryFactor(s2), posLat);
    } else {
        return getPosition(offset);
    }
}


double
GUIVehicle::getVisualAngle(bool s2) const {
    if (s2) {
        // see MSVehicle::computeAngle
        const PositionVector& shape = myLane->getShape(s2);
        if (isParking()) {
            if (myStops.begin()->parkingarea != nullptr) {
                return myStops.begin()->parkingarea->getVehicleAngle(*this);
            } else {
                return shape.rotationAtOffset(getPositionOnLane() * myLane->getLengthGeometryFactor(s2));
            }
        }
        // if (myLaneChangeModel->isChangingLanes()) {
        const double lefthandSign = (MSGlobals::gLefthand ? -1 : 1);
        Position p1 = getVisualPosition(s2);
        Position p2 = getVisualPosition(s2, MAX2(0.0, -myType->getLength()));
        double result = (p1 != p2 ? p2.angleTo2D(p1) :
                         shape.rotationAtOffset(getPositionOnLane() * myLane->getLengthGeometryFactor(s2)));
        if (myLaneChangeModel->isChangingLanes()) {
            result += lefthandSign * DEG2RAD(myLaneChangeModel->getAngleOffset());
        }
        return result;
    } else {
        return getAngle();
    }
}
/****************************************************************************/
