/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2026 German Aerospace Center (DLR) and others.
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
/// @author  Mirko Barthauer
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
//#define DEBUG_QUEUE
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

    oc.doRegister("device.glosa.add-switchtime", new Option_Float(0.0));
    oc.addDescription("device.glosa.add-switchtime", "GLOSA Device", TL("Additional time the vehicle shall need to reach the intersection after the signal turns green"));

    oc.doRegister("device.glosa.use-queue", new Option_Bool(false));
    oc.addDescription("device.glosa.use-queue", "GLOSA Device", TL("Use queue in front of the tls for GLOSA calculation"));

    oc.doRegister("device.glosa.override-safety", new Option_Bool(false));
    oc.addDescription("device.glosa.override-safety", "GLOSA Device", TL("Override safety features - ignore the current light state, always follow GLOSA's predicted state"));

    oc.doRegister("device.glosa.ignore-cfmodel", new Option_Bool(false));
    oc.addDescription("device.glosa.ignore-cfmodel", "GLOSA Device", TL("Vehicles follow a perfect speed calculation - ignore speed calculations from the CF model if not safety critical"));
}


void
MSDevice_GLOSA::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (!MSGlobals::gUseMesoSim && equippedByDefaultAssignmentOptions(oc, "glosa", v, false)) {
        MSDevice_GLOSA* device = new MSDevice_GLOSA(v, "glosa_" + v.getID(),
                v.getFloatParam("device.glosa.min-speed", true),
                v.getFloatParam("device.glosa.range", true),
                v.getFloatParam("device.glosa.max-speedfactor", true),
                v.getFloatParam("device.glosa.add-switchtime", true),
                v.getBoolParam("device.glosa.use-queue", true),
                v.getBoolParam("device.glosa.override-safety", true),
                v.getBoolParam("device.glosa.ignore-cfmodel", true));
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
MSDevice_GLOSA::MSDevice_GLOSA(SUMOVehicle& holder, const std::string& id, double minSpeed, double range, double maxSpeedFactor,
                               double addSwitchTime, bool useQueue, bool overrideSafety, bool ignoreCFModel) :
    MSVehicleDevice(holder, id),
    myVeh(dynamic_cast<MSVehicle&>(holder)),
    myNextTLSLink(nullptr),
    myDistance(0),
    myMinSpeed(minSpeed),
    myRange(range),
    myMaxSpeedFactor(maxSpeedFactor),
    myAddSwitchTime(addSwitchTime),
    myOverrideSafety(overrideSafety),
    myIgnoreCFModel(ignoreCFModel),
    mySpeedAdviceActive(false),
    myUseQueue(useQueue)

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
        double timeToJunction = earliest_arrival(myDistance, vMax);
        int countOld = 0;
        // calculate "first" next phase, all coming Phases are calculated via "getTimeToNextSwitch"
        double timeToSwitch = getTimeToSwitch(myNextTLSLink, countOld);
        bool currentPhaseGreen = false;
        bool currentPhaseStop = false;
        bool solved = false;
        double nextSwitch = 0;
        nextSwitch = timeToSwitch;
        double QueueLength = 0;
        double greenTime = 0;
        double additionalJunctionTime = 0;
        // It takes factor [seconds/per meter queue] to dissolve the queue at drive off
        // experimental value from calibrated drone data
        double factor = 0.21;
        // Additional time (offset) for the leading vehicle to accelerate
        // experimental value from calibrated drone data
        double addition = 3;
        // Vehicles are often not able to drive with their desired speed vMax.
        // The further away from the junction, the more uncertain the arrival prediction becomes.
        // For a vehicle trying to reach the begin/end of a green phase, this is critical.
        // The vehicle should then rather opt for the next phase (trade travel time for traffic flow)
        int switchOffset = 0;

        if (myNextTLSLink->haveGreen()) {
            currentPhaseGreen = true;
        } else if (myNextTLSLink->haveRed() || myNextTLSLink->haveYellow()) {
            currentPhaseStop = true;
        }
        // else if any other phase, GLOSA does not interfere
#if defined DEBUG_GLOSA || defined DEBUG_QUEUE
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << myVeh.getID() << " d=" << myDistance << " ttJ=" << timeToJunction << " ttS=" << timeToSwitch << "\n";
        }
#endif
        if (myUseQueue) {
            // Detect queue length at tls
            QueueLength = myNextTLSLink->getTLLogic()->getTLQueueLength(myNextTLSLink->getLaneBefore()->getID());
#ifdef DEBUG_QUEUE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " veh=" << myVeh.getID() << " Queuelength=" << QueueLength << "\n";
            }
#endif
            if (currentPhaseGreen) {
                // how long has it already been green in this phase
                greenTime = timeGreen(myNextTLSLink);
                additionalJunctionTime = (QueueLength * factor + addition) - greenTime;
                if ((additionalJunctionTime > 0) && (additionalJunctionTime < nextSwitch)) {
                    // do note use queue system if queue is to long
                    timeToJunction += additionalJunctionTime;
                } else {
                    // important for case: "speed can be increased to arrive at tls while green"
                    additionalJunctionTime = 0;
                }
#ifdef DEBUG_QUEUE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " veh=" << myVeh.getID() << " Additonaljunctiontime=" << additionalJunctionTime << " Greentime=" << greenTime << " TimetoJunction(GreenQueue)=" << timeToJunction << "\n";
                }
#endif
            }

            if (currentPhaseStop) {
                nextSwitch += QueueLength * factor + addition;
#ifdef DEBUG_QUEUE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " veh=" << myVeh.getID() << " Nextswitch(RedQueue)=" << nextSwitch << "\n";
                }
#endif
            }
        }

        // Search for the next passable phase, maximum 10 Phases
        for (int countwhile = 1; countwhile < 11; countwhile++) {
            if (currentPhaseGreen) {
                // reset nextSwitch because the queue matters only for the current Phase
                if (countwhile == 2 && myUseQueue) {
                    nextSwitch -= QueueLength * factor + addition;
                }
                if (mySpeedAdviceActive && myOriginalSpeedFactor > myVeh.getChosenSpeedFactor()) {
                    myVeh.setChosenSpeedFactor(myOriginalSpeedFactor);
                    mySpeedAdviceActive = false;
                }
#ifdef DEBUG_GLOSA
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " veh=" << myVeh.getID() << " traffic light will be/is green" << "\n";
                    std::cout << SIMTIME << " veh=" << myVeh.getID()
                              << " timeToJunction=" << timeToJunction
                              << " nextSwitch=" << nextSwitch
                              << " myDistance=" << myDistance << "\n";
                }
#endif
                // if we arrive at the tls after it switched to red (else do nothing)
                if (timeToJunction > nextSwitch) {
                    // if speed can be increased to arrive at tls while green (else look for next phases)
                    if (myMaxSpeedFactor > myOriginalSpeedFactor) {
                        const double vMax2 = vMax / myVeh.getChosenSpeedFactor() * myMaxSpeedFactor;
                        const double timetoJunction2 = earliest_arrival(myDistance, vMax2) + additionalJunctionTime;
                        // reaching the signal at yellow might be sufficient
                        const double yellowSlack = myVeh.getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_DRIVE_AFTER_YELLOW_TIME, 0);
#ifdef DEBUG_GLOSA
                        if (DEBUG_COND) {
                            std::cout << SIMTIME << " veh=" << myVeh.getID()
                                      << " vMax2=" << vMax2
                                      << " timetoJunction2=" << timetoJunction2
                                      << " yellowSlack=" << yellowSlack << "\n";
                        }
#endif
                        // if increased speed is fast enough to arrive at tls while green (else look for next phases)
                        if (timetoJunction2 <= (nextSwitch + yellowSlack)) {
                            // increase speed factor up to a maximum if necessary and useful
                            // XXX could compute optimal speed factor here
                            myVeh.setChosenSpeedFactor(myMaxSpeedFactor);
                            mySpeedAdviceActive = true;
                            break; // solved
                        } else {
                            // speed can not be increased to arrive at tls while green
                            // next switch is calculated at the end of the for-loop
                        }
                    } else {
                        // speed increase is not enough to reach tls while green
                        // next switch is calculated at the end of the for-loop
                    }
                } else {
                    // vehicle will arrive at tls while green
                    break; // solved
                }
            } else if (currentPhaseStop) {
                // tls is red at the moment
#ifdef DEBUG_GLOSA
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " veh=" << myVeh.getID() << " traffic light will be/is red" << "\n";
                    std::cout << SIMTIME << " veh=" << myVeh.getID()
                              << " timeToJunction=" << timeToJunction
                              << " nextSwitch=" << nextSwitch
                              << " myDistance=" << myDistance << "\n";
                }
#endif
                if (countwhile == 2 && myUseQueue) {
                    // take queue into account if current green phase can not be passed and next red phase is used instead
                    nextSwitch += QueueLength * factor + addition;
#ifdef DEBUG_QUEUE
                    if (DEBUG_COND) {
                        std::cout << SIMTIME << " veh=" << myVeh.getID() << " nextSwitch(redadditon): " << nextSwitch << "\n";
                    }
#endif
                }
                adaptSpeed(myDistance, timeToJunction, nextSwitch + myAddSwitchTime, solved);
                if (countwhile == 2 && myUseQueue) {
                    // reset queue because following phases should not calculate with current queue
                    nextSwitch -= QueueLength * factor + addition;
                    timeToJunction -= additionalJunctionTime;
                }
                if (solved) {
                    break; // solved
                }
            }
            // calculate next Phase
            nextSwitch += getTimeToNextSwitch(myNextTLSLink, currentPhaseGreen, currentPhaseStop, countOld);
            // For vehicles far away from the junction we add an offset
            if (nextSwitch > 80.) {
                nextSwitch += (double)switchOffset;
                switchOffset = 6;
                nextSwitch -= (double)switchOffset;
            } else if (nextSwitch > 60.) {
                nextSwitch += (double)switchOffset;
                switchOffset = 4;
                nextSwitch -= (double)switchOffset;
            } else if (nextSwitch > 40.) {
                nextSwitch += (double)switchOffset;
                switchOffset = 3;
                nextSwitch -= (double)switchOffset;
            } else if (nextSwitch > 20.) {
                nextSwitch += (double)switchOffset;
                switchOffset = 2;
                nextSwitch -= (double)switchOffset;
            }
        }
    }
    return true; // keep the device
}


bool
MSDevice_GLOSA::notifyEnter(SUMOTrafficObject& /*veh*/, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    const MSLink* prevLink = myNextTLSLink;
    myNextTLSLink = nullptr;
    const MSLane* lane = myVeh.getLane();
    if (myVeh.getDeparture() < SIMSTEP) {
        // no need to call at insertion
        myVeh.updateBestLanes();
    }
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
        // moved pass tls
        myVeh.setChosenSpeedFactor(myOriginalSpeedFactor);
        mySpeedAdviceActive = false;
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
        myRange = MIN2(myVeh.getFloatParam("device.glosa.range", true), tlsRange);
    }

#ifdef DEBUG_GLOSA
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << myVeh.getID() << " enter=" << myVeh.getLane()->getID() << " tls=" << (myNextTLSLink == nullptr ? "NULL" : myNextTLSLink->getTLLogic()->getID()) << " dist=" << myDistance << "\n";
    }
#endif
    return true; // keep the device
}


double
MSDevice_GLOSA::getTimeToSwitch(const MSLink* tlsLink, int& countOld) {
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
        if (((tlsLink->haveRed() || tlsLink->haveYellow()) && (ls == 'g' || ls == 'G'))
                || (tlsLink->haveGreen() && ls != 'g' && ls != 'G')) {
            countOld = cur + i;
            break;
        }
        result += phase->duration;
    }
    return STEPS2TIME(result);
}


double
MSDevice_GLOSA::getTimeToNextSwitch(const MSLink* tlsLink, bool& currentPhaseGreen, bool& currentPhaseStop, int& countOld) {
    // get time till the tls switches to a phase the vehicle can reach
    assert(tlsLink != nullptr);
    const MSTrafficLightLogic* const tl = tlsLink->getTLLogic();
    assert(tl != nullptr);
    const auto& phases = tl->getPhases();
    const int n = (int)phases.size();
    const int cur = countOld;
    SUMOTime result = 0;

    for (int i = 0; i < n; i++) {
        const auto& phase = phases[(cur + i) % n];
        const char ls = phase->getState()[tlsLink->getTLIndex()];
        if (currentPhaseGreen && (ls == 'g' || ls == 'G')) {
            countOld = (cur + i) % n;
            break;
        }
        if (currentPhaseStop && (ls != 'g' && ls != 'G')) {
            countOld = (cur + i) % n;
            break;
        }
        result += phase->duration;
    }
    currentPhaseGreen = !currentPhaseGreen;
    currentPhaseStop = !currentPhaseStop;
    return STEPS2TIME(result);
}

double
MSDevice_GLOSA::timeGreen(const MSLink* tlsLink) {
    assert(tlsLink != nullptr);
    const MSTrafficLightLogic* const tl = tlsLink->getTLLogic();
    assert(tl != nullptr);
    const auto& phases = tl->getPhases();
    const int n = (int)phases.size();
    const int cur = tl->getCurrentPhaseIndex();
    // As there are multiple "microphases G" in one Greenphase this function only gives back
    // the already spent time in the current microphase
    SUMOTime result = tl->getSpentDuration();

    for (int i = 1; i < n; i++) {
        const auto& phase = phases[(cur - i) % n];
        const char ls = phase->getState()[tlsLink->getTLIndex()];
        if (ls != 'g' && ls != 'G') {
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
MSDevice_GLOSA::adaptSpeed(double distance, double /*timeToJunction*/, double timeToSwitch, bool& solved) {
    // ensure that myVehicle arrives at the
    // junction with maximum speed when it switches to green
    // car performs a slowDown at time z to speed x for duration y
    // there are two basic strategies
    // a) maximize z -> this saves road space but leads to low x and thus excessive braking
    // b) maximize x -> this saves fuel but wastes road
    // c) compromise: b) but only when distance to junction is below a threshold

    const double vMax = myVeh.getLane()->getSpeedLimit() * myOriginalSpeedFactor;

    // need to start/continue maneuver
    const double t = timeToSwitch;
    const double a = myVeh.getCarFollowModel().getMaxAccel();
    const double u = myMinSpeed;
    const double w = vMax;
    const double s = distance;
    const double v = myVeh.getSpeed();
    // x : target speed
    // y : slow down duration
    // s is composed of 1 trapezoid (decel), 1 rectangle (maintain), 1 trapezoid (accel)
    // s = (v^2-x^2)/2d + x*(t-y-(w-x)/a) + (w^2-x^2)/2a
    // y = (v-x)/d
    // solution for x curtesy of mathomatic.org

    // First, we calculate targetSpeed assuming we are driving that speed already (v=x)
    // If this results in targetSpeed < currentSpeed, then we need to decelerate (and vice versa)
    const double rootConst = a * a * t * t - 2.0 * a * w * t + 2 * a * s;
    double vConst = 0;
    if (rootConst >= 0) {
        vConst = sqrt(rootConst) - a * t + w;
    }
    double d = myVeh.getCarFollowModel().getMaxDecel();
    if (v < vConst) {
        d = a;
    }

    // Second, we calculate the correct targetSpeed, knowing if we need to accelerate or decelerate
    const double sign0 = -1; // quadratic formula solution x1 (choose solution with higher speed)
    const double root_argument = a * d * ((2.0 * d * (s - (w * t))) - ((v - w) * (v - w)) + (a * ((d * (t * t)) + (2.0 * (s - (t * v))))));
    if (root_argument < 0) {
#ifdef DEBUG_GLOSA
        WRITE_WARNINGF("GLOSA error 1 root_argument=% s=% t=% v=%", root_argument, s, t, v);
#endif
        // no reset of speedFactor because in this case, current speed can be kept
        solved = true;

        return;
    }
    const double x = (((a * (v - (d * t))) + (d * w) - sign0 * sqrt(root_argument)) / (d + a));
    double y = (v - x) / d;
    if (v < x) {
        y = (x - v) / d;
    }
    if (s < (w * w - x * x) / 2.0 / a) {
        // end maneuver
        if (myIgnoreCFModel) {
            std::vector<std::pair<SUMOTime, double> > speedTimeLine;
            speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), myVeh.getSpeed()));
            speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(w - x / a), vMax));
            myVeh.getInfluencer().setSpeedTimeLine(speedTimeLine);
        } else {
            myVeh.setChosenSpeedFactor(myOriginalSpeedFactor);
        }
        mySpeedAdviceActive = false;
        return;
    }
    if (!(x >= u && x <= w && y < t)) {
#ifdef DEBUG_GLOSA
        WRITE_WARNINGF("GLOSA error 2 x=% y=% s=% t=% v=%", x, y, s, t, v);
#endif
        if (x < u) {
#ifdef DEBUG_GLOSA
            if (DEBUG_COND) {
                std::cout << "veh=" << myVeh.getID() << " cant go slow enough" << "\n";
            }
#endif
            // no reset of speedFactor because in this case, current speed can be kept
            //myVeh.setChosenSpeedFactor(myOriginalSpeedFactor);
            //mySpeedAdviceActive = false;
            solved = true;
        }
        if (x > w) {
#ifdef DEBUG_GLOSA
            if (DEBUG_COND) {
                std::cout << "veh=" << myVeh.getID() << " cant go fast enough" << "\n";
            }
#endif
        }
        return;
    }
    const double targetSpeed = x;
    const double duration = MAX2(y, TS);
    solved = true;
#ifdef DEBUG_GLOSA
    if (DEBUG_COND) {
        std::cout << "  targetSpeed=" << targetSpeed << " duration=" << duration << "\n";
    }
#endif
    if (myIgnoreCFModel) {
        std::vector<std::pair<SUMOTime, double> > speedTimeLine;
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), myVeh.getSpeed()));
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(duration), targetSpeed));
        myVeh.getInfluencer().setSpeedTimeLine(speedTimeLine);
    } else {
        myVeh.setChosenSpeedFactor(targetSpeed / myVeh.getLane()->getSpeedLimit());
    }
    mySpeedAdviceActive = true;
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
