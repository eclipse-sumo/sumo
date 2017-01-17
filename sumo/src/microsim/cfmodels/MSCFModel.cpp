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
MSCFModel::MSCFModel(const MSVehicleType* vtype, const SUMOReal accel,
                     const SUMOReal decel, const SUMOReal headwayTime)
    : myType(vtype), myAccel(accel), myDecel(decel), myHeadwayTime(headwayTime) {
}


MSCFModel::~MSCFModel() {}


MSCFModel::VehicleVariables::~VehicleVariables() {}


SUMOReal
MSCFModel::brakeGap(const SUMOReal speed, const SUMOReal decel, const SUMOReal headwayTime) {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        /* one possibility to speed this up is to calculate speedReduction * steps * (steps+1) / 2
        	for small values of steps (up to 10 maybe) and store them in an array */
        const SUMOReal speedReduction = ACCEL2SPEED(decel);
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


SUMOReal
MSCFModel::freeSpeed(const SUMOReal currentSpeed, const SUMOReal decel, const SUMOReal dist, const SUMOReal targetSpeed, const bool onInsertion) {
    // XXX: (Leo) This seems to be exclusively called with decel = myDecel (max deceleration) and is not overridden
    // by any specific CFModel. That may cause undesirable hard braking (at junctions where the vehicle
    // changes to a road with a lower speed limit). It relies on the same logic as the maximalSafeSpeed calculations.
    // XXX: Don't duplicate the code from there, if possible!!! Can maximumSafeStopSpeed() call freeSpeed?  (Leo)
    //      -> not is the reaction time / desired headway is to be regarded...

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // adapt speed to succeeding lane, no reaction time is involved
        // when breaking for y steps the following distance g is covered
        // (drive with v in the final step)
        // g = (y^2 + y) * 0.5 * b + y * v
        // y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
        const SUMOReal v = SPEED2DIST(targetSpeed);
        if (dist < v) {
            return targetSpeed;
        }
        const SUMOReal b = ACCEL2DIST(decel);
        const SUMOReal y = MAX2(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
        const SUMOReal yFull = floor(y);
        const SUMOReal exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
        const SUMOReal fullSpeedGain = (yFull + (onInsertion ? 1. : 0.)) * ACCEL2SPEED(decel);
        return DIST2SPEED(MAX2((SUMOReal)0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + targetSpeed;
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

        const SUMOReal dt = onInsertion ? 0 : TS; // handles case that vehicle is inserted just now (at the end of move)
        const SUMOReal v0 = currentSpeed;
        const SUMOReal vT = targetSpeed;
        const SUMOReal b = decel;
        const SUMOReal d = dist - NUMERICAL_EPS; // prevent returning a value > targetSpeed due to rounding errors

        // Solvability for positive vN (if d is small relative to v0):
        // 1) If 0.5*(v0+vT)*dt > d, we set vN=vT.
        // (In case vT<v0, this implies that on the interpolated trajectory there are points beyond d where
        //  the interpolated velocity is larger than vT, but at least on the temporal discretization grid, vT is not exceeded)
        // 2) We ignore the (possible) constraint vN >= v0 - b*dt, which could lead to a problem if v0 - t*b > vT.
        //    (moveHelper() is responsible for assuring that the next velocity is chosen in accordance with maximal decelerations)

        if (0.5 * (v0 + vT)*dt >= d) {
            return vT;    // (#)
        }

        const SUMOReal q = ((dt * v0 - 2 * d) * b - vT * vT); // (q < 0 is fulfilled because of (#))
        const SUMOReal p = 0.5 * b * dt;
        return -p + sqrt(p * p - q);
    }
}

SUMOReal
MSCFModel::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    SUMOReal vMin, vNext;
    const SUMOReal vMax = MIN3(veh->getMaxSpeedOnLane(), maxNextSpeed(oldV, veh), vSafe);
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


SUMOReal
MSCFModel::interactionGap(const MSVehicle* const veh, SUMOReal vL) const {
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const SUMOReal vNext = MIN2(maxNextSpeed(veh->getSpeed(), veh), veh->getLane()->getVehicleMaxSpeed(veh));
    const SUMOReal gap = (vNext - vL) *
                         ((veh->getSpeed() + vL) / (2.*myDecel) + myHeadwayTime) +
                         vL * myHeadwayTime;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


SUMOReal
MSCFModel::maxNextSpeed(SUMOReal speed, const MSVehicle* const /*veh*/) const {
    return MIN2(speed + (SUMOReal) ACCEL2SPEED(getMaxAccel()), myType->getMaxSpeed());
}

SUMOReal
MSCFModel::minNextSpeed(SUMOReal speed, const MSVehicle* const /*veh*/) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return MAX2(speed - ACCEL2SPEED(getMaxDecel()), (SUMOReal)0.);
    } else {
        // NOTE: ballistic update allows for negative speeds to indicate a stop within the next timestep
        return speed - ACCEL2SPEED(getMaxDecel());
    }
}


SUMOReal
MSCFModel::freeSpeed(const MSVehicle* const /* veh */, SUMOReal speed, SUMOReal seen, SUMOReal maxSpeed, const bool onInsertion) const {
    SUMOReal vSafe = freeSpeed(speed, myDecel, seen, maxSpeed, onInsertion);
    return vSafe;
}


SUMOReal
MSCFModel::insertionFollowSpeed(const MSVehicle* const /* v */, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel);
    } else {
        // NOTE: Even for ballistic update, the current speed is irrelevant at insertion, therefore passing 0. (Leo)
        return maximumSafeFollowSpeed(gap2pred, 0., predSpeed, predMaxDecel, true);
    }
}


SUMOReal
MSCFModel::insertionStopSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return stopSpeed(veh, speed, gap);
    } else {
        return MIN2(maximumSafeStopSpeed(gap, 0., true, 0.), myType->getMaxSpeed());
    }
}


SUMOTime
MSCFModel::getMinimalArrivalTime(SUMOReal dist, SUMOReal currentSpeed, SUMOReal arrivalSpeed) const {
    const SUMOReal accel = (arrivalSpeed >= currentSpeed) ? getMaxAccel() : -getMaxDecel();
    const SUMOReal accelTime = (arrivalSpeed - currentSpeed) / accel;
    const SUMOReal accelWay = accelTime * (arrivalSpeed + currentSpeed) * 0.5;
    const SUMOReal nonAccelWay = MAX2(SUMOReal(0), dist - accelWay);
    // will either drive as fast as possible and decelerate as late as possible
    // or accelerate as fast as possible and then hold that speed
    const SUMOReal nonAccelSpeed = MAX3(currentSpeed, arrivalSpeed, SUMO_const_haltingSpeed);
    return TIME2STEPS(accelTime + nonAccelWay / nonAccelSpeed);
}


SUMOReal
MSCFModel::getMinimalArrivalSpeed(SUMOReal dist, SUMOReal currentSpeed) const {
    // ballistic update
    return estimateSpeedAfterDistance(dist - currentSpeed * getHeadwayTime(), currentSpeed, -getMaxDecel());
}


SUMOReal
MSCFModel::getMinimalArrivalSpeedEuler(SUMOReal dist, SUMOReal currentSpeed) const {
    SUMOReal arrivalSpeedBraking;
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




SUMOReal
MSCFModel::gapExtrapolation(const SUMOReal duration, const SUMOReal currentGap, SUMOReal v1,  SUMOReal v2, SUMOReal a1, SUMOReal a2, const SUMOReal maxV1, const SUMOReal maxV2) {

    SUMOReal newGap = currentGap;

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        for (unsigned int steps = 1; steps * TS <= duration; ++steps) {
            v1 = MIN2(MAX2(v1 + a1, (SUMOReal)0.), maxV1);
            v2 = MIN2(MAX2(v2 + a2, (SUMOReal)0.), maxV2);
            newGap += TS * (v1 - v2);
        }
    } else {
        // determine times t1, t2 for which vehicles can break until stop (within duration)
        // and t3, t4 for which they reach their maximal speed on their current lanes.
        SUMOReal t1 = 0, t2 = 0, t3 = 0, t4 = 0;

        // t1: ego veh stops
        if (a1 < 0 && v1 > 0) {
            const SUMOReal leaderStopTime =  - v1 / a1;
            t1 = MIN2(leaderStopTime, duration);
        } else if (a1 >= 0) {
            t1 = duration;
        }
        // t2: veh2 stops
        if (a2 < 0 && v2 > 0) {
            const SUMOReal followerStopTime = -v2 / a2;
            t2 = MIN2(followerStopTime, duration);
        } else if (a2 >= 0) {
            t2 = duration;
        }
        // t3: ego veh reaches vMax
        if (a1 > 0 && v1 < maxV1) {
            const SUMOReal leaderMaxSpeedTime = (maxV1 - v1) / a1;
            t3 = MIN2(leaderMaxSpeedTime, duration);
        } else if (a1 <= 0) {
            t3 = duration;
        }
        // t4: veh2 reaches vMax
        if (a2 > 0 && v2 < maxV2) {
            const SUMOReal followerMaxSpeedTime = (maxV2 - v2) / a2;
            t4 = MIN2(followerMaxSpeedTime, duration);
        } else if (a2 <= 0) {
            t4 = duration;
        }

        // NOTE: this assumes that the accelerations a1 and a2 are constant over the next
        //       followerBreakTime seconds (if no vehicle stops before or reaches vMax)
        std::list<SUMOReal> l;
        l.push_back(t1);
        l.push_back(t2);
        l.push_back(t3);
        l.push_back(t4);
        l.sort();
        std::list<SUMOReal>::const_iterator i;
        SUMOReal tLast = 0.;
        for (i = l.begin(); i != l.end(); ++i) {
            if (*i != tLast) {
                SUMOReal dt = MIN2(*i, duration) - tLast; // time between *i and tLast
                SUMOReal dv = v1 - v2; // current velocity difference
                SUMOReal da = a1 - a2; // current acceleration difference
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
            SUMOReal dt = duration - tLast; // remaining time until duration
            SUMOReal dv = v1 - v2; // current velocity difference
            newGap += dv * dt; // update gap
        }
    }

    return newGap;
}



SUMOReal
MSCFModel::passingTime(const SUMOReal lastPos, const SUMOReal passedPos, const SUMOReal currentPos, const SUMOReal lastSpeed, const SUMOReal currentSpeed) {

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
        const SUMOReal lastCoveredDist = currentPos - lastPos;
        const SUMOReal extrapolated = passedPos > currentPos ? TS * (passedPos - lastPos) / lastCoveredDist : TS * (currentPos - passedPos) / lastCoveredDist;
        return extrapolated;
    } else if (currentSpeed < 0) {
        WRITE_ERROR("passingTime(): given argument 'currentSpeed' is negative. This case is not handled yet.");
        return -1;
    }

    const SUMOReal distanceOldToPassed = passedPos - lastPos; // assert: >=0

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // euler update (constantly moving with currentSpeed during [0,TS])
        const SUMOReal t = distanceOldToPassed / currentSpeed;
        return t;

    } else {
        // ballistic update (constant acceleration a during [0,TS], except in case of a stop)

        // determine acceleration
        SUMOReal a;
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
            const SUMOReal t = 2 * distanceOldToPassed / (lastSpeed + currentSpeed);
            return t;
        } else if (a > 0) {
            // positive acceleration => only one positive solution
            const SUMOReal va = lastSpeed / a;
            const SUMOReal t = -va + sqrt(va * va + 2 * distanceOldToPassed / a);
            assert(t < 1 && t >= 0);
            return t;
        } else {
            // negative acceleration => two positive solutions (pick the smaller one.)
            const SUMOReal va = lastSpeed / a;
            const SUMOReal t = -va - sqrt(va * va + 2 * distanceOldToPassed / a);
            assert(t < 1 && t >= 0);
            return t;
        }
    }
}


SUMOReal
MSCFModel::speedAfterTime(const SUMOReal t, const SUMOReal v0, const SUMOReal dist) {
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
            const SUMOReal accel = - v0 * v0 / (2 * dist);
            // The speed at time t is then
            return v0 + accel * t;
        } else {
            // no stop occured within [0,TS], thus (from dist = v0*TS + accel*TS^2/2)
            const SUMOReal accel = 2 * (dist / TS - v0) / TS;
            // The speed at time t is then
            return v0 + accel * t;
        }
    }
}




SUMOReal
MSCFModel::estimateSpeedAfterDistance(const SUMOReal dist, const SUMOReal v, const SUMOReal accel) const {
    // dist=v*t + 0.5*accel*t^2, solve for t and use v1 = v + accel*t
    return MAX2((SUMOReal)0., MIN2(myType->getMaxSpeed(),
                                   (SUMOReal)sqrt(2 * dist * accel + v * v)));
}



SUMOReal
MSCFModel::maximumSafeStopSpeed(SUMOReal g /*gap*/, SUMOReal v /*currentSpeed*/, bool onInsertion, SUMOReal headway) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return maximumSafeStopSpeedEuler(g);
    } else {
        return maximumSafeStopSpeedBallistic(g, v, onInsertion, headway);
    }
}


SUMOReal
MSCFModel::maximumSafeStopSpeedEuler(SUMOReal gap) const {
    gap -= NUMERICAL_EPS; // lots of code relies on some slack XXX: it shouldn't...
    if (gap <= 0) {
        return 0;
    } else if (gap <= ACCEL2SPEED(myDecel)) {
        // workaround for #2310
        return MIN2(ACCEL2SPEED(myDecel), DIST2SPEED(gap));
    }
    const SUMOReal g = gap;
    const SUMOReal b = ACCEL2SPEED(myDecel);
    const SUMOReal t = myHeadwayTime;
    const SUMOReal s = TS;


    // h = the distance that would be covered if it were possible to stop
    // exactly after gap and decelerate with b every simulation step
    // h = 0.5 * n * (n-1) * b * s + n * b * t (solve for n)
    //n = ((1.0/2.0) - ((t + (pow(((s*s) + (4.0*((s*((2.0*h/b) - t)) + (t*t)))), (1.0/2.0))*sign/2.0))/s));
    const SUMOReal n = floor(.5 - ((t + (sqrt(((s * s) + (4.0 * ((s * (2.0 * g / b - t)) + (t * t))))) * -0.5)) / s));
    const SUMOReal h = 0.5 * n * (n - 1) * b * s + n * b * t;
    assert(h <= g + NUMERICAL_EPS);
    // compute the additional speed that must be used during deceleration to fix
    // the discrepancy between g and h
    const SUMOReal r = (g - h) / (n * s + t);
    const SUMOReal x = n * b + r;
    assert(x >= 0);
    return x;
}


SUMOReal
MSCFModel::maximumSafeStopSpeedBallistic(SUMOReal g /*gap*/, SUMOReal v /*currentSpeed*/, bool onInsertion, SUMOReal headway) const {
    // decrease gap slightly (to avoid passing end of lane by values of magnitude ~1e-12, when exact stop is required)
    g = MAX2((SUMOReal)0., g - NUMERICAL_EPS);
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
        const SUMOReal btau = myDecel * headway;
        const SUMOReal v0 = -btau + sqrt(btau * btau + 2 * myDecel * g);
        return v0;
    }

    // In the usual case during the driving task, the vehicle goes by
    // a current speed v0=v, and we seek to determine a safe acceleration a (possibly <0)
    // such that starting to break after accelerating with a for the time tau
    // still allows us to stop in time.

    const SUMOReal tau = headway;
    const SUMOReal v0 = MAX2((SUMOReal)0., v);
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
        const SUMOReal a = -v0 * v0 / (2 * g);
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

    const SUMOReal btau2 = myDecel * tau / 2;
    const SUMOReal v1 = -btau2 + sqrt(btau2 * btau2 + myDecel * (2 * g - tau * v0));
    const SUMOReal a = (v1 - v0) / tau;
    return v0 + a * TS;
}


/** Returns the SK-vsafe. */
SUMOReal
MSCFModel::maximumSafeFollowSpeed(SUMOReal gap, SUMOReal egoSpeed, SUMOReal predSpeed, SUMOReal predMaxDecel, bool onInsertion) const {
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
//    const SUMOReal headway = predSpeed > 0. ? myHeadwayTime : 0.;

    const SUMOReal headway = myHeadwayTime;
    const SUMOReal x = maximumSafeStopSpeed(gap + brakeGap(predSpeed, MAX2(myDecel, predMaxDecel), 0), egoSpeed, onInsertion, headway);
    assert(x >= 0 || !MSGlobals::gSemiImplicitEulerUpdate);
    assert(!ISNAN(x));
    return x;
}


/****************************************************************************/
