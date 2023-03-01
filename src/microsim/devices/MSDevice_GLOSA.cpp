/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_GLOSA.cpp
/// @author  Jakob Erdmann
/// @date    21.04.2021
///
// A device for Green Light Optimal Speed Advisory
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_GLOSA.h"

//#define DEBUG_GLOSA
#define DEBUG_COND (true)

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_GLOSA::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("GLOSA Device");
    insertDefaultAssignmentOptions("glosa", "GLOSA Device", oc);

    oc.doRegister("device.glosa.range", new Option_Float(100.0));
    oc.addDescription("device.glosa.range", "GLOSA Device", TL("The communication range to the traffic light"));

    oc.doRegister("device.glosa.max-speedfactor", new Option_Float(1.1));
    oc.addDescription("device.glosa.max-speedfactor", "GLOSA Device", TL("The maximum speed factor when approaching a green light"));

    oc.doRegister("device.glosa.min-speed", new Option_Float(5.0));
    oc.addDescription("device.glosa.min-speed", "GLOSA Device", TL("Minimum speed when coasting towards a red light"));
}


void
MSDevice_GLOSA::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (!MSGlobals::gUseMesoSim && equippedByDefaultAssignmentOptions(oc, "glosa", v, false)) {
        MSDevice_GLOSA* device = new MSDevice_GLOSA(v, "glosa_" + v.getID(),
                getFloatParam(v, OptionsCont::getOptions(), "glosa.min-speed", 5, true),
                getFloatParam(v, OptionsCont::getOptions(), "glosa.range", 100, true),
                getFloatParam(v, OptionsCont::getOptions(), "glosa.max-speedfactor", 1.1, true));
        into.push_back(device);
    }
}

void
MSDevice_GLOSA::cleanup() {
    // cleaning up global state (if any)
}

// ---------------------------------------------------------------------------
// MSDevice_GLOSA-methods
// ---------------------------------------------------------------------------
MSDevice_GLOSA::MSDevice_GLOSA(SUMOVehicle& holder, const std::string& id, double minSpeed, double range, double maxSpeedFactor) :
    MSVehicleDevice(holder, id),
    myVeh(dynamic_cast<MSVehicle&>(holder)),
    myNextTLSLink(nullptr),
    myDistance(0),
    myMinSpeed(minSpeed),
    myRange(range),
    myMaxSpeedFactor(maxSpeedFactor)

{
    myOriginalSpeedFactor = myVeh.getChosenSpeedFactor();
}


MSDevice_GLOSA::~MSDevice_GLOSA() {
}


bool
MSDevice_GLOSA::notifyMove(SUMOTrafficObject& /*tObject*/, double oldPos,
                           double newPos, double /*newSpeed*/) {
    myDistance -= (newPos - oldPos);
    if (myNextTLSLink != nullptr && myDistance <= myRange) {
        const double vMax = myVeh.getLane()->getVehicleMaxSpeed(&myVeh);
        const double timeToJunction = earliest_arrival(myDistance, vMax);
        const double timeToSwitch = getTimeToSwitch(myNextTLSLink);
#ifdef DEBUG_GLOSA
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << myVeh.getID() << " d=" << myDistance << " ttJ=" << timeToJunction << " ttS=" << timeToSwitch << "\n";
        }
#endif
        if (myNextTLSLink->haveGreen()) {
            if (timeToJunction > timeToSwitch) {
                if (myMaxSpeedFactor > myVeh.getChosenSpeedFactor()) {
                    const double vMax2 = vMax / myVeh.getChosenSpeedFactor() * myMaxSpeedFactor;
                    const double timetoJunction2 = earliest_arrival(myDistance, vMax2);
                    // reaching the signal at yellow might be sufficient
                    const double yellowSlack = myVeh.getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_DRIVE_AFTER_YELLOW_TIME, 0);
#ifdef DEBUG_GLOSA
                    if (DEBUG_COND) {
                        std::cout << "  vMax2=" << vMax2 << " ttJ2=" << timetoJunction2 << " yellowSlack=" << yellowSlack << "\n";
                    }
#endif
                    if (timetoJunction2 <= (timeToSwitch + yellowSlack)) {
                        // increase speed factor up to a maximum if necessary and useful
                        // XXX could compute optimal speed factor here
                        myVeh.setChosenSpeedFactor(myMaxSpeedFactor);
                    }
                }
            }
        } else if (myNextTLSLink->haveRed()) {
            adaptSpeed(myDistance, timeToJunction, timeToSwitch);
        }
    }
    return true; // keep the device
}


bool
MSDevice_GLOSA::notifyEnter(SUMOTrafficObject& /*veh*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    const MSLink* prevLink = myNextTLSLink;
    myNextTLSLink = nullptr;
    const MSLane* lane = myVeh.getLane();
    const std::vector<MSLane*>& bestLaneConts = myVeh.getBestLanesContinuation(lane);
    double seen = lane->getLength() - myVeh.getPositionOnLane();
    int view = 1;
    std::vector<MSLink*>::const_iterator linkIt = MSLane::succLinkSec(myVeh, view, *lane, bestLaneConts);
    while (!lane->isLinkEnd(linkIt)) {
        if (!lane->getEdge().isInternal()) {
            if ((*linkIt)->isTLSControlled()) {
                myNextTLSLink = *linkIt;
                myDistance = seen;
                break;
            }
        }
        lane = (*linkIt)->getViaLaneOrLane();
        if (!lane->getEdge().isInternal()) {
            view++;
        }
        seen += lane->getLength();
        linkIt = MSLane::succLinkSec(myVeh, view, *lane, bestLaneConts);
    }
    if (prevLink != nullptr && myNextTLSLink == nullptr) {
        // moved passt tls
        myVeh.setChosenSpeedFactor(myOriginalSpeedFactor);
    } else if (myNextTLSLink != nullptr && prevLink != myNextTLSLink) {
        // approaching new tls
        double tlsRange = 1e10;
        const std::string val = myNextTLSLink->getTLLogic()->getParameter("device.glosa.range", "1e10");
        try {
            tlsRange = StringUtils::toDouble(val);
        } catch (const NumberFormatException&) {
            WRITE_WARNINGF(TL("Invalid value '%' for parameter 'device.glosa.range' of traffic light '%'"),
                           val, myNextTLSLink->getTLLogic()->getID());
        }
        myRange = MIN2(getFloatParam(myVeh, OptionsCont::getOptions(), "glosa.range", 100, true), tlsRange);
    }

#ifdef DEBUG_GLOSA
    if (DEBUG_COND) std::cout << SIMTIME << " veh=" << myVeh.getID() << " enter=" << myVeh.getLane()->getID() << " hadTLS=" << hadTLS
                                  << " tls=" << (myNextTLSLink == nullptr ? "NULL" : myNextTLSLink->getTLLogic()->getID()) << " dist=" << myDistance << "\n";
#endif
    return true; // keep the device
}


double
MSDevice_GLOSA::getTimeToSwitch(const MSLink* tlsLink) {
    assert(tlsLink != nullptr);
    const MSTrafficLightLogic* const tl = tlsLink->getTLLogic();
    assert(tl != nullptr);
    const auto& phases = tl->getPhases();
    const int n = (int)phases.size();
    const int cur = tl->getCurrentPhaseIndex();
    SUMOTime result = tl->getNextSwitchTime() - SIMSTEP;
    for (int i = 1; i < n; i++) {
        const auto& phase = phases[(cur + i) % n];
        const char ls = phase->getState()[tlsLink->getTLIndex()];
        if ((tlsLink->haveRed() && (ls == 'g' || ls == 'G'))
                || (tlsLink->haveGreen() && ls != 'g' && ls != 'G')) {
            break;
        }
        result += phase->duration;
    }
    return STEPS2TIME(result);
}


double
MSDevice_GLOSA::earliest_arrival(double distance, double vMax) {
    // assume we keep acceleration until we hit maximum speed
    const double v = myVeh.getSpeed();
    const double a = myVeh.getCarFollowModel().getMaxAccel();
    const double accel_time = MIN2((vMax - v) / a, time_to_junction_at_continuous_accel(distance, v));
    const double remaining_dist = distance - distance_at_continuous_accel(v, accel_time);
    const double remaining_time = remaining_dist / vMax;
    return accel_time + remaining_time;
}


/*
double
MSDevice_GLOSA::latest_arrival(speed, distance, earliest) {
    // assume we keep current speed until within myRange and then decelerate to myMinSpeed
    speed = max(speed, GLOSA_MIN_SPEED)
    potential_decel_dist = min(distance, GLOSA_RANGE)
    decel_time = (speed - GLOSA_MIN_SPEED) / GLOSA_DECEL
    avg_decel_speed = (speed + GLOSA_MIN_SPEED) / 2.0
    decel_dist = decel_time * avg_decel_speed
    if decel_dist > potential_decel_dist:
        decel_dist = potential_decel_dist
        # XXX actually avg_decel_speed is higher in this case
        decel_time = decel_dist / avg_decel_speed
    slow_dist = potential_decel_dist - decel_dist
    fast_dist = distance - (decel_dist + slow_dist)
    result = fast_dist / speed + decel_time + slow_dist / GLOSA_MIN_SPEED
    if result < earliest:
        if (distance > 15):
            print("DEBUG: fixing latest arrival of %s to match earliest of %s" % (result, earliest))
        result = earliest
    return result
    return 0;
}
*/


double
MSDevice_GLOSA::distance_at_continuous_accel(double speed, double time) {
    const double v = speed;
    const double t = time;
    const double a = myVeh.getCarFollowModel().getMaxAccel();
    // integrated area composed of a rectangle and a triangle
    return v * t + a * t * t / 2;
}


double
MSDevice_GLOSA::time_to_junction_at_continuous_accel(double d, double v) {
    // see distance_at_continuous_accel
    // t^2 + (2v/a)t - 2d/a = 0
    const double a = myVeh.getCarFollowModel().getMaxAccel();
    const double p_half = v / a;
    const double t = -p_half + sqrt(p_half * p_half + 2 * d / a);
    return t;
}


void
MSDevice_GLOSA::adaptSpeed(double distance, double timeToJunction, double timeToSwitch) {
    // ensure that myVehicle arrives at the
    // junction with maximum speed when it switches to green
    // car performs a slowDown at time z to speed x for duration y
    // there are two basic strategies
    // a) maximize z -> this saves road space but leads to low x and thus excessive braking
    // b) maximize x -> this saves fuel but wastes road
    // c) compromise: b) but only when distance to junction is below a threshold

    const double vMax = myVeh.getLane()->getVehicleMaxSpeed(&myVeh);
    if (timeToJunction < timeToSwitch
            && myVeh.getSpeed() > myMinSpeed) {
        // need to start/continue maneuver
        const double t = timeToSwitch;
        const double a = myVeh.getCarFollowModel().getMaxAccel();
        const double d = myVeh.getCarFollowModel().getMaxDecel();
        const double u = myMinSpeed;
        const double w = vMax;
        const double s = distance;
        const double v = myVeh.getSpeed();
        // x : target speed
        // y : slow down duration
        // s is composed of 1 trapezoid (decel), 1 rectangle (maintain), 1 trapezoid (accel)
        // s = (v^2-x^2)/2d + x*(y-(v-x)/d) + (w^2-x^2)/2a
        // y = t - (w-x)/d
        // solution for x curtesy of mathomatic.org
        const double sign0 = -1; // XXX hack
        const double root_argument = a * d * ((2.0 * d * (s - (w * t))) - ((v - w) * (v - w)) + (a * ((d * (t * t)) + (2.0 * (s - (t * v))))));
        if (root_argument < 0) {
#ifdef DEBUG_GLOSA
            WRITE_WARNINGF("GLOSA error 1 root_argument=% s=% t=% v=%", root_argument, s, t, v);
#endif
            return;
        }
        const double x = (((a * (v - (d * t))) + (d * w) - sign0 * sqrt(root_argument)) / (d + a));
        const double y = t - (w - x) / d;
        if (!(x >= u && x <= w && y > 0 && y < t)) {
#ifdef DEBUG_GLOSA
            WRITE_WARNINGF("GLOSA error 2 x=% y=% s=% t=% v=%", x, y, s, t, v);
#endif
            return;
        }
        const double targetSpeed = x;
        const double duration = y;
#ifdef DEBUG_GLOSA
        if (DEBUG_COND) {
            std::cout << "  targetSpeed=" << targetSpeed << " duration=" << duration << "\n";
        }
#endif
        std::vector<std::pair<SUMOTime, double> > speedTimeLine;
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), myVeh.getSpeed()));
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(duration), targetSpeed));
        myVeh.getInfluencer().setSpeedTimeLine(speedTimeLine);
    } else {
        // end maneuver
        std::vector<std::pair<SUMOTime, double> > speedTimeLine;
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), myVeh.getSpeed()));
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), vMax));
        myVeh.getInfluencer().setSpeedTimeLine(speedTimeLine);
    }
}


void
MSDevice_GLOSA::generateOutput(OutputDevice* /*tripinfoOut*/) const {
    /*
    if (tripinfoOut != nullptr) {
        tripinfoOut->openTag("glosa_device");
        tripinfoOut->closeTag();
    }
    */
}

std::string
MSDevice_GLOSA::getParameter(const std::string& key) const {
    if (key == "minSpeed") {
        return toString(myMinSpeed);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_GLOSA::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = StringUtils::toDouble(value);
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    if (key == "minSpeed") {
        myMinSpeed = doubleValue;
    } else {
        throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}


/****************************************************************************/
