/****************************************************************************/
/// @file    MSCFModel.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Leonhard Lücken
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <math.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSCFModel.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel::MSCFModel(const MSVehicleType* vtype, double accel,
                     double decel, double emergencyDecel, double apparentDecel, double headwayTime) :
    myType(vtype),
    myAccel(accel),
    myDecel(decel),
    myEmergencyDecel(emergencyDecel),
    myApparentDecel(apparentDecel),
    myHeadwayTime(headwayTime) {
}


MSCFModel::~MSCFModel() {}


MSCFModel::VehicleVariables::~VehicleVariables() {}


double
MSCFModel::brakeGap(const double speed, const double decel, const double headwayTime) {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        /* one possibility to speed this up is to calculate speedReduction * steps * (steps+1) / 2
        	for small values of steps (up to 10 maybe) and store them in an array */
        const double speedReduction = ACCEL2SPEED(decel);
        const int steps = int(speed / speedReduction);
        return SPEED2DIST(steps * speed - speedReduction * steps * (steps + 1) / 2) + speed * headwayTime;
    } else {
        // ballistic
        if (speed <= 0) {
            return 0.;
        } else {
            return speed * (headwayTime + 0.5 * speed / decel);
        }
    }
}


double
MSCFModel::freeSpeed(const double currentSpeed, const double decel, const double dist, const double targetSpeed, const bool onInsertion) {
    // XXX: (Leo) This seems to be exclusively called with decel = myDecel (max deceleration) and is not overridden
    // by any specific CFModel. That may cause undesirable hard braking (at junctions where the vehicle
    // changes to a road with a lower speed limit).

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // adapt speed to succeeding lane, no reaction time is involved
        // when breaking for y steps the following distance g is covered
        // (drive with v in the final step)
        // g = (y^2 + y) * 0.5 * b + y * v
        // y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
        const double v = SPEED2DIST(targetSpeed);
        if (dist < v) {
            return targetSpeed;
        }
        const double b = ACCEL2DIST(decel);
        const double y = MAX2(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
        const double yFull = floor(y);
        const double exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
        const double fullSpeedGain = (yFull + (onInsertion ? 1. : 0.)) * ACCEL2SPEED(decel);
        return DIST2SPEED(MAX2(0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + targetSpeed;
    } else {
        // ballistic update (Leo)
        // calculate maximum next speed vN that is adjustable to vT=targetSpeed after a distance d=dist
        // and given a maximal deceleration b=decel, denote the current speed by v0.
        // the distance covered by a trajectory that attains vN in the next timestep and decelerates afterwards
        // with b is given as
        // d = 0.5*dt*(v0+vN) + (t-dt)*vN - 0.5*b*(t-dt)^2, (1)
        // where time t of arrival at d with speed vT is
        // t = dt + (vN-vT)/b.  (2)
        // We insert (2) into (1) to obtain
        // d = 0.5*dt*(v0+vN) + vN*(vN-vT)/b - 0.5*b*((vN-vT)/b)^2
        // 0 = (dt*b*v0 - vT*vT - 2*b*d) + dt*b*vN + vN*vN
        // and solve for vN

        assert(currentSpeed >= 0);
        assert(targetSpeed >= 0);

        const double dt = onInsertion ? 0 : TS; // handles case that vehicle is inserted just now (at the end of move)
        const double v0 = currentSpeed;
        const double vT = targetSpeed;
        const double b = decel;
        const double d = dist - NUMERICAL_EPS; // prevent returning a value > targetSpeed due to rounding errors

        // Solvability for positive vN (if d is small relative to v0):
        // 1) If 0.5*(v0+vT)*dt > d, we set vN=vT.
        // (In case vT<v0, this implies that on the interpolated trajectory there are points beyond d where
        //  the interpolated velocity is larger than vT, but at least on the temporal discretization grid, vT is not exceeded)
        // 2) We ignore the (possible) constraint vN >= v0 - b*dt, which could lead to a problem if v0 - t*b > vT.
        //    (moveHelper() is responsible for assuring that the next velocity is chosen in accordance with maximal decelerations)

        if (0.5 * (v0 + vT)*dt >= d) {
            return vT;    // (#)
        }

        const double q = ((dt * v0 - 2 * d) * b - vT * vT); // (q < 0 is fulfilled because of (#))
        const double p = 0.5 * b * dt;
        return -p + sqrt(p * p - q);
    }
}

double
MSCFModel::moveHelper(MSVehicle* const veh, double vPos) const {
    const double oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const double vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    double vMin, vNext;
    const double vMax = MIN3(veh->getMaxSpeedOnLane(), maxNextSpeed(oldV, veh), vSafe);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // we cannot rely on never braking harder than maxDecel because TraCI or strange cf models may decide to do so
        vMin = MIN2(getSpeedAfterMaxDecel(oldV), vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
    } else {
        // for ballistic update, negative vnext must be allowed to
        // indicate a stop within the coming timestep (i.e., to attain negative values)
        vMin =  MIN2(minNextSpeed(oldV, veh), vMax);
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
        // (Leo) moveHelper() is responsible for assuring that the next
        // velocity is chosen in accordance with maximal decelerations.
        // At this point vNext may also be negative indicating a stop within next step.
        // Moreover, because maximumSafeStopSpeed() does not consider deceleration bounds
        // vNext can be a large negative value at this point. We cap vNext here.
        vNext = MAX2(vNext, vMin);
    }

    return vNext;
}


double
MSCFModel::interactionGap(const MSVehicle* const veh, double vL) const {
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const double vNext = MIN2(maxNextSpeed(veh->getSpeed(), veh), veh->getLane()->getVehicleMaxSpeed(veh));
    const double gap = (vNext - vL) *
                       ((veh->getSpeed() + vL) / (2.*myDecel) + myHeadwayTime) +
                       vL * myHeadwayTime;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


double
MSCFModel::maxNextSpeed(double speed, const MSVehicle* const /*veh*/) const {
    return MIN2(speed + (double) ACCEL2SPEED(getMaxAccel()), myType->getMaxSpeed());
}

double
MSCFModel::minNextSpeed(double speed, const MSVehicle* const /*veh*/) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
//        return MAX2(speed - ACCEL2SPEED(getMaxDecel()), 0.);
        return MAX2(speed - ACCEL2SPEED(myEmergencyDecel), 0.);
    } else {
        // NOTE: ballistic update allows for negative speeds to indicate a stop within the next timestep
//        return speed - ACCEL2SPEED(getMaxDecel());
        return speed - ACCEL2SPEED(myEmergencyDecel);
    }
}


double
MSCFModel::freeSpeed(const MSVehicle* const /* veh */, double speed, double seen, double maxSpeed, const bool onInsertion) const {
    double vSafe = freeSpeed(speed, myDecel, seen, maxSpeed, onInsertion);
    return vSafe;
}


double
MSCFModel::insertionFollowSpeed(const MSVehicle* const /* v */, double speed, double gap2pred, double predSpeed, double predMaxDecel) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel);
    } else {
        // NOTE: Even for ballistic update, the current speed is irrelevant at insertion, therefore passing 0. (Leo)
        return maximumSafeFollowSpeed(gap2pred, 0., predSpeed, predMaxDecel, true);
    }
}


double
MSCFModel::insertionStopSpeed(const MSVehicle* const veh, double speed, double gap) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return stopSpeed(veh, speed, gap);
    } else {
        return MIN2(maximumSafeStopSpeed(gap, 0., true, 0.), myType->getMaxSpeed());
    }
}


SUMOTime
MSCFModel::getMinimalArrivalTime(double dist, double currentSpeed, double arrivalSpeed) const {
    const double accel = (arrivalSpeed >= currentSpeed) ? getMaxAccel() : -getMaxDecel();
    const double accelTime = (arrivalSpeed - currentSpeed) / accel;
    const double accelWay = accelTime * (arrivalSpeed + currentSpeed) * 0.5;
    const double nonAccelWay = MAX2(0., dist - accelWay);
    // will either drive as fast as possible and decelerate as late as possible
    // or accelerate as fast as possible and then hold that speed
    const double nonAccelSpeed = MAX3(currentSpeed, arrivalSpeed, SUMO_const_haltingSpeed);
    return TIME2STEPS(accelTime + nonAccelWay / nonAccelSpeed);
}


double
MSCFModel::estimateArrivalTime(double dist, double speed, double maxSpeed, double accel) const {
    assert(speed >= 0.);
    assert(dist >= 0.);

    if (dist == 0.) {
        return 0.;
    }

    if ((accel < 0. && -0.5*speed*speed/accel < dist) || (accel <= 0. && speed == 0.)) {
        // distance will never be covered with these values
        return INVALID_DOUBLE;
    }

    if (accel == 0.) return dist/speed;

    double p = speed/accel;

    if (accel < 0.){
        // we already know, that the distance will be covered despite breaking
        return (-p - sqrt(p*p + 2*dist/accel));
    }

    // Here, accel > 0
    // t1 is the time to use the given acceleration
    double t1 = (maxSpeed - speed)/accel;
    // distance covered until t1
    double d1 = speed*t1 + 0.5*accel*t1*t1;
    if (d1 >= dist) {
        // dist is covered before changing the speed
        return (-p + sqrt(p*p + 2*dist/accel));
    } else {
        return (-p + sqrt(p*p + 2*d1/accel)) + (dist - d1)/maxSpeed;
    }

}



double
MSCFModel::getMinimalArrivalSpeed(double dist, double currentSpeed) const {
    // ballistic update
    return estimateSpeedAfterDistance(dist - currentSpeed * getHeadwayTime(), currentSpeed, -getMaxDecel());
}


double
MSCFModel::getMinimalArrivalSpeedEuler(double dist, double currentSpeed) const {
    double arrivalSpeedBraking;
    // Because we use a continuous formula for computing the possible slow-down
    // we need to handle the mismatch with the discrete dynamics
    if (dist < currentSpeed) {
        arrivalSpeedBraking = INVALID_SPEED; // no time left for braking after this step
        //	(inserted max() to get rid of arrivalSpeed dependency within method) (Leo)
    } else if (2 * (dist - currentSpeed * getHeadwayTime()) * -getMaxDecel() + currentSpeed * currentSpeed >= 0) {
        arrivalSpeedBraking = estimateSpeedAfterDistance(dist - currentSpeed * getHeadwayTime(), currentSpeed, -getMaxDecel());
    } else {
        arrivalSpeedBraking = getMaxDecel();
    }
    return arrivalSpeedBraking;
}




double
MSCFModel::gapExtrapolation(const double duration, const double currentGap, double v1,  double v2, double a1, double a2, const double maxV1, const double maxV2) {

    double newGap = currentGap;

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        for (unsigned int steps = 1; steps * TS <= duration; ++steps) {
            v1 = MIN2(MAX2(v1 + a1, 0.), maxV1);
            v2 = MIN2(MAX2(v2 + a2, 0.), maxV2);
            newGap += TS * (v1 - v2);
        }
    } else {
        // determine times t1, t2 for which vehicles can break until stop (within duration)
        // and t3, t4 for which they reach their maximal speed on their current lanes.
        double t1 = 0, t2 = 0, t3 = 0, t4 = 0;

        // t1: ego veh stops
        if (a1 < 0 && v1 > 0) {
            const double leaderStopTime =  - v1 / a1;
            t1 = MIN2(leaderStopTime, duration);
        } else if (a1 >= 0) {
            t1 = duration;
        }
        // t2: veh2 stops
        if (a2 < 0 && v2 > 0) {
            const double followerStopTime = -v2 / a2;
            t2 = MIN2(followerStopTime, duration);
        } else if (a2 >= 0) {
            t2 = duration;
        }
        // t3: ego veh reaches vMax
        if (a1 > 0 && v1 < maxV1) {
            const double leaderMaxSpeedTime = (maxV1 - v1) / a1;
            t3 = MIN2(leaderMaxSpeedTime, duration);
        } else if (a1 <= 0) {
            t3 = duration;
        }
        // t4: veh2 reaches vMax
        if (a2 > 0 && v2 < maxV2) {
            const double followerMaxSpeedTime = (maxV2 - v2) / a2;
            t4 = MIN2(followerMaxSpeedTime, duration);
        } else if (a2 <= 0) {
            t4 = duration;
        }

        // NOTE: this assumes that the accelerations a1 and a2 are constant over the next
        //       followerBreakTime seconds (if no vehicle stops before or reaches vMax)
        std::list<double> l;
        l.push_back(t1);
        l.push_back(t2);
        l.push_back(t3);
        l.push_back(t4);
        l.sort();
        std::list<double>::const_iterator i;
        double tLast = 0.;
        for (i = l.begin(); i != l.end(); ++i) {
            if (*i != tLast) {
                double dt = MIN2(*i, duration) - tLast; // time between *i and tLast
                double dv = v1 - v2; // current velocity difference
                double da = a1 - a2; // current acceleration difference
                newGap += dv * dt + da * dt * dt / 2.; // update gap
                v1 += dt * a1;
                v2 += dt * a2;
            }
            if (*i == t1 || *i == t3) {
                // ego veh reached velocity bound
                a1 = 0.;
            }

            if (*i == t2 || *i == t4) {
                // veh2 reached velocity bound
                a2 = 0.;
            }

            tLast = MIN2(*i, duration);
            if (tLast == duration) {
                break;
            }
        }

        if (duration != tLast) {
            // (both vehicles have zero acceleration)
            assert(a1 == 0. && a2 == 0.);
            double dt = duration - tLast; // remaining time until duration
            double dv = v1 - v2; // current velocity difference
            newGap += dv * dt; // update gap
        }
    }

    return newGap;
}



double
MSCFModel::passingTime(const double lastPos, const double passedPos, const double currentPos, const double lastSpeed, const double currentSpeed) {

    assert(passedPos <= currentPos && passedPos >= lastPos && currentPos > lastPos);
    assert(currentSpeed >= 0);

    if (passedPos > currentPos || passedPos < lastPos) {
        std::stringstream ss;
        // Debug (Leo)
        if (!MSGlobals::gSemiImplicitEulerUpdate) {
            // NOTE: error is guarded to maintain original test output for euler update (Leo).
            ss << "passingTime(): given argument passedPos = " << passedPos << " doesn't lie within [lastPos, currentPos] = [" << lastPos << ", " << currentPos << "]\nExtrapolating...";
            std::cout << ss.str() << "\n";
            WRITE_ERROR(ss.str());
        }
        const double lastCoveredDist = currentPos - lastPos;
        const double extrapolated = passedPos > currentPos ? TS * (passedPos - lastPos) / lastCoveredDist : TS * (currentPos - passedPos) / lastCoveredDist;
        return extrapolated;
    } else if (currentSpeed < 0) {
        WRITE_ERROR("passingTime(): given argument 'currentSpeed' is negative. This case is not handled yet.");
        return -1;
    }

    const double distanceOldToPassed = passedPos - lastPos; // assert: >=0

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // euler update (constantly moving with currentSpeed during [0,TS])
        const double t = distanceOldToPassed / currentSpeed;
        return MIN2(TS, MAX2(0., t)); //rounding errors could give results out of the admissible result range

    } else {
        // ballistic update (constant acceleration a during [0,TS], except in case of a stop)

        // determine acceleration
        double a;
        if (currentSpeed > 0) {
            // the acceleration was constant within the last time step
            a = SPEED2ACCEL(currentSpeed - lastSpeed);
        } else {
            // the currentSpeed is zero (the last was not because lastPos<currentPos).
            assert(currentSpeed == 0 && lastSpeed != 0);
            // In general the stop has taken place within the last time step.
            // The acceleration (a<0) is obtained from
            // deltaPos = - lastSpeed^2/(2*a)
            a = lastSpeed * lastSpeed / (2 * (lastPos - currentPos));

            assert(a < 0);
        }

        // determine passing time t
        // we solve distanceOldToPassed = lastSpeed*t + a*t^2/2
        if (fabs(a) < NUMERICAL_EPS) {
            // treat as constant speed within [0, TS]
            const double t = 2 * distanceOldToPassed / (lastSpeed + currentSpeed);
            return MIN2(TS, MAX2(0., t)); //rounding errors could give results out of the admissible result range
        } else if (a > 0) {
            // positive acceleration => only one positive solution
            const double va = lastSpeed / a;
            const double t = -va + sqrt(va * va + 2 * distanceOldToPassed / a);
            assert(t < 1 && t >= 0);
            return t;
        } else {
            // negative acceleration => two positive solutions (pick the smaller one.)
            const double va = lastSpeed / a;
            const double t = -va - sqrt(va * va + 2 * distanceOldToPassed / a);
            assert(t < 1 && t >= 0);
            return t;
        }
    }
}


double
MSCFModel::speedAfterTime(const double t, const double v0, const double dist) {
    assert(dist >= 0);
    assert(t >= 0 && t <= TS);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // euler: constant speed within [0,TS]
        return DIST2SPEED(dist);
    } else {
        // ballistic: piecewise constant acceleration in [0,TS] (may become 0 for a stop within TS)
        // We reconstruct acceleration at time t=0. Note that the covered distance in case
        // of a stop exactly at t=TS is TS*v0/2.
        if (dist <  TS * v0 / 2) {
            // stop must have occured within [0,TS], use dist = -v0^2/(2a) (stopping dist),
            // i.e., a = -v0^2/(2*dist)
            const double accel = - v0 * v0 / (2 * dist);
            // The speed at time t is then
            return v0 + accel * t;
        } else {
            // no stop occured within [0,TS], thus (from dist = v0*TS + accel*TS^2/2)
            const double accel = 2 * (dist / TS - v0) / TS;
            // The speed at time t is then
            return v0 + accel * t;
        }
    }
}




double
MSCFModel::estimateSpeedAfterDistance(const double dist, const double v, const double accel) const {
    // dist=v*t + 0.5*accel*t^2, solve for t and use v1 = v + accel*t
    return MAX2(0., MIN2(myType->getMaxSpeed(),
                         (double)sqrt(2 * dist * accel + v * v)));
}



double
MSCFModel::maximumSafeStopSpeed(double g /*gap*/, double v /*currentSpeed*/, bool onInsertion, double headway) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return maximumSafeStopSpeedEuler(g);
    } else {
        return maximumSafeStopSpeedBallistic(g, v, onInsertion, headway);
    }
}


double
MSCFModel::maximumSafeStopSpeedEuler(double gap) const {
    gap -= NUMERICAL_EPS; // lots of code relies on some slack XXX: it shouldn't...
    if (gap <= 0) {
        return 0;
    } else if (gap <= ACCEL2SPEED(myDecel)) {
        // workaround for #2310
        return MIN2(ACCEL2SPEED(myDecel), DIST2SPEED(gap));
    }
    const double g = gap;
    const double b = ACCEL2SPEED(myDecel);
    const double t = myHeadwayTime;
    const double s = TS;


    // h = the distance that would be covered if it were possible to stop
    // exactly after gap and decelerate with b every simulation step
    // h = 0.5 * n * (n-1) * b * s + n * b * t (solve for n)
    //n = ((1.0/2.0) - ((t + (pow(((s*s) + (4.0*((s*((2.0*h/b) - t)) + (t*t)))), (1.0/2.0))*sign/2.0))/s));
    const double n = floor(.5 - ((t + (sqrt(((s * s) + (4.0 * ((s * (2.0 * g / b - t)) + (t * t))))) * -0.5)) / s));
    const double h = 0.5 * n * (n - 1) * b * s + n * b * t;
    assert(h <= g + NUMERICAL_EPS);
    // compute the additional speed that must be used during deceleration to fix
    // the discrepancy between g and h
    const double r = (g - h) / (n * s + t);
    const double x = n * b + r;
    assert(x >= 0);
    return x;
}


double
MSCFModel::maximumSafeStopSpeedBallistic(double g /*gap*/, double v /*currentSpeed*/, bool onInsertion, double headway) const {
    // decrease gap slightly (to avoid passing end of lane by values of magnitude ~1e-12, when exact stop is required)
    g = MAX2(0., g - NUMERICAL_EPS);
    headway = headway >= 0 ? headway : myHeadwayTime;

    // (Leo) Note that in contrast to the Euler update, for the ballistic update
    // the distance covered in the coming step depends on the current velocity, in general.
    // one exception is the situation when the vehicle is just being inserted.
    // In that case, it will not cover any distance until the next timestep by convention.

    // We treat the latter case first:
    if (onInsertion) {
        // The distance covered with constant insertion speed v0 until time tau is given as
        // G1 = tau*v0
        // The distance covered between time tau and the stopping moment at time tau+v0/b is
        // G2 = v0^2/(2b),
        // where b is an assumed constant deceleration (= myDecel)
        // We solve g = G1 + G2 for v0:
        const double btau = myDecel * headway;
        const double v0 = -btau + sqrt(btau * btau + 2 * myDecel * g);
        return v0;
    }

    // In the usual case during the driving task, the vehicle goes by
    // a current speed v0=v, and we seek to determine a safe acceleration a (possibly <0)
    // such that starting to break after accelerating with a for the time tau
    // still allows us to stop in time.

    const double tau = headway;
    const double v0 = MAX2(0., v);
    // We first consider the case that a stop has to take place within time tau
    if (v0 * tau >= 2 * g) {
        if (g == 0.) {
            if (v0 > 0.) {
                // indicate to brake as hard as possible
                return -INVALID_SPEED;
            } else {
                // stay stopped
                return 0.;
            }
        }
        // In general we solve g = v0^2/(-2a), where the the rhs is the distance
        // covered until stop when breaking with a<0
        const double a = -v0 * v0 / (2 * g);
        return v0 + a * TS;
    }

    // The last case corresponds to a situation, where the vehicle may go with a positive
    // speed v1 = v0 + tau*a after time tau.
    // The distance covered until time tau is given as
    // G1 = tau*(v0+v1)/2
    // The distance covered between time tau and the stopping moment at time tau+v1/b is
    // G2 = v1^2/(2b),
    // where b is an assumed constant deceleration (= myDecel)
    // We solve g = G1 + G2 for v1>0:
    // <=> 0 = v1^2 + b*tau*v1 + b*tau*v0 - 2bg
    //  => v1 = -b*tau/2 + sqrt( (b*tau)^2/4 + b(2g - tau*v0) )

    const double btau2 = myDecel * tau / 2;
    const double v1 = -btau2 + sqrt(btau2 * btau2 + myDecel * (2 * g - tau * v0));
    const double a = (v1 - v0) / tau;
    return v0 + a * TS;
}


/** Returns the SK-vsafe. */
double
MSCFModel::maximumSafeFollowSpeed(double gap, double egoSpeed, double predSpeed, double predMaxDecel, bool onInsertion) const {
    // the speed is safe if allows the ego vehicle to come to a stop behind the leader even if
    // the leaders starts braking hard until stopped
    // unfortunately it is not sufficient to compare stopping distances if the follower can brake harder than the leader
    // (the trajectories might intersect before both vehicles are stopped even if the follower has a shorter stopping distance than the leader)
    // To make things safe, we ensure that the leaders brake distance is computed with an deceleration that is at least as high as the follower's.
    // @todo: this is a conservative estimate for safe speed which could be increased

//    // For negative gaps, we return the lowest meaningful value by convention
//    // XXX: check whether this is desireable (changes test results, therefore I exclude it for now (Leo), refs. #2575)
//    if(gap<0){
//        if(MSGlobals::gSemiImplicitEulerUpdate){
//            return 0.;
//        } else {
//            return -INVALID_SPEED;
//        }
//    }

    // The following commented code is a variant to assure brief stopping behind a stopped leading vehicle:
    // if leader is stopped, calculate stopSpeed without time-headway to prevent creeping stop
    // NOTE: this can lead to the strange phenomenon (for the Krauss-model at least) that if the leader comes to a stop,
    //       the follower accelerates for a short period of time. Refs #2310 (Leo)
//    const double headway = predSpeed > 0. ? myHeadwayTime : 0.;

    const double headway = myHeadwayTime;
    const double x = maximumSafeStopSpeed(gap + brakeGap(predSpeed, MAX2(myDecel, predMaxDecel), 0), egoSpeed, onInsertion, headway);
    assert(x >= 0 || !MSGlobals::gSemiImplicitEulerUpdate);
    assert(!ISNAN(x));
    return x;
}


/****************************************************************************/
