---
title: Purgatory/Simulation Loop
permalink: /Purgatory/Simulation_Loop/
---

Each simulation time step, the following procedures are performed.
Please note that this is description for the current (since 12.01.2010)
SVN version; earlier versions differ.

- Process TraCI Commands (if TraCI is not disabled)
- Save Simulation State if needed (currently only in mesosim)
- Execute "Begin of Time Step" Events
- Check for Collisions
- Reset Junction Requests
- Check Traffic Light Changes
- Set Traffic Light Signals on Links
- Assure all not-free Lanes are known
- Compute Vehicle Movement (iterating over all not-free lanes)
- Determine which Vehicles have Right-of-way
- Move Vehicles
- Change Lanes
- Check for Collisions
- Progress Vehicles Loading
- Insert Vehicles
- Execute "End of Time Step" Events
- Save Output
- Increment current Time Step

# Compute Vehicle Movement (iterating over all not-free lanes)

A vehicle's (**EGO**'s) speed during the next time step is constrained
by: a) the leading vehicle (**LEADER**), b) a vehicle on the lane left
to **EGO**'s lane in some cases, c) right-of-way rules at approached
intersections, d) speed limits at approached lanes.

## Interaction with the leading Vehicle

Using a car-following model, and having an **EGO** vehicle, we need its
**LEADER** which parameter are used within the model's equations. Its
pretty easy to determine one on a circular road, but things get more
complicated when moving to real-world road networks:

- we have to consider leading vehicles on the next lanes that may be
  approached

### Interaction with LEADER on same lane

We go through the (sorted) list of a lane's vehicles and use the next
one as **LEADER**. This is the first check in computing the next speed.
We do this only for the next vehicle in front (**LEADER**), discarding
further vehicles in fron (**LEADER+n**) - assuming the **LEADER**'s
model would be responsible for keeping **EGO** collision free by using a
safe velocity in respect to his (**LEADER**'s) **LEADER**.

Obviously, the first vehicle on the lane (the one nearest to the lane's
end) does not have a leading vehicle on this lane. Now, one assumption
is that if a vehicle is on a lane which does not allow to continue its
route, it brakes so that it does not leave it. This means that for the
last vehicle we do not need a **LEADER** if this last vehicle is on a
false lane. Nonetheless, an additional problem may occur for the last
vehicle: a vehicle which is only partially at the lane; vehicles are
administrated by the lanes and the lane stores and moves only vehicles
which front is on the lane - the vehicle's back may nonetheless be
(still) on a predecessing lane. We have to check this even in the case a
vehicle will not leave the lane.

### Interaction with LEADER on consecutive lanes

In the case **EGO** is on a "correct" lane, we have to look into
consequent lanes for **LEADER**s. The reason is quite simple: assume
**EGO** is turning left, but his **LEADER** - being on the same lane -
goes straight and has no **LEADER** itself. The **LEADER** may progress
at high speed, but **EGO** has to watch out for the situation on
subsequent lanes.

# Explaining SUMO

- if the speed for a vehicle is computed, the leader has to have the
position/speed from the last time step (rule for most car-following
models)
  - \--\> we have to ensure, that this applies for leaders on
    subsequent lanes as well. But: we can not sort edges/lanes due
    to possible cyclic dependencies
  - \--\> we have to do two steps
    - a) compute next speed
    - b) move vehicles
  - dkrajzew: *OK, ok, we could do this in one step, too, by keeping
    the last position on the lane. Maybe in the future...*

### Second try for vehicle movement...

#### compute next speed(s)

**step\#1: examine current lane**

At first, we look at the current lane and whether the vehicle is able to
leave it. If not, as it is on a false lane, we may have to brake so that
we stay on this lane. We then would try to leave the lane to reach a
possible continuation lane afterwards. Actually, this distinction
between leaving/not leaving a lane is pretty old and contains several
code duplications. Probably the distinction should be removed...

In: no information

Out: speed bound by leader on current lane, the lane itself if not
appropriate, stops on the current edge, arrivalspeed if current lane; if
the lane is not appropriate, an according drive item which only allows
to move to the end of the lane is generated.

- we **remove** all information about **approaching a link**, will be
reset in this step, see below
- vBeg is the maximum speed after acceleration
- if vehicle **can not use its current lane** to continue the ride
  - let the vehicle halt at the end of the lane<br>
    \--\> vBeg depends on edge interaction<br>
    **todo:** check whether maxDecel/maxAccel is followed is missing

- if it is his last edge
  - compute allowed speed in dependence of the arrivalspeed
    definition
- check interaction with a leading vehicle
  - if existing: the leader on the vehicle's lane
  - otherwise: a maybe in-lapping leader on a consecutive lane (is
    stored in the lane)
  - otherwise: no leader, keep speed<br>
    \--\> vBeg additionally depends on the leader, if existing

- do not overtake right<br>
\--\> vBeg additionally depends on the left-lane leader, if existing

- check stops on current edge<br>
\--\> vBeg additionally depends on current edge's stop, if existing

- assure vBeg\>=0<br>
**todo:** why is it done this way? Should be a part of the model or - less nice - assure here that vBeg\>=v-maxDecel\*dt

- if vehicle **can not use its current lane** to continue the ride
  - save as a drive item; use vWish for speed, the vehicle will not
    move across the next link...
- else (vehicle can continue its drive)
  - jump to *vsafeCriticalCont* for evaluating how speed depends on
    subsequent lanes/edges
  - jump to *checkRewindLinkLanes* for a roll-back of possible
    continuation due to not having enough space on them...

**step\#2: examine subsequent lanes**

We are now going through the next lanes, and compute possible influences
on the vehicle's speed.

In: speed bound by leader on current lane, the lane itself if not
appropriate, stops on the current edge, arrivalspeed if current lane

Out: a list of speed decision points in myLFLinkLanes; each contains the
link (if valid) that will be crossed, the speed if the vehicle will be
later allowed to cross it and the speed if crossing is not allowed, and
some additional parameter.

- seen = myLane-\>getLength() - myState.myPos; (the place in front of
the vehicle on his current lane, what we have seen so far)
- we can abort if the vehicle will not enter the next lane (as we have
already seen the leader, stops, and stuff); it is assumed that the
following conditions must be fulfilled:
  - *this\!=myLane-\>getFirstVehicle()* (it is not the first
    vehicle; **todo:** why? This is pretty old, too. As the vehicle
    will not reach the next lane, we can ignore it\!\!\!
  - *&& seen - cfModel.brakeGap(myState.mySpeed) \> 0* (if the
    vehicle brakes, it will not enter the next lane; **recheck**
    don't get it)
  - *&& seen - SPEED2DIST(boundVSafe) -
    ACCEL2DIST(cfModel.getMaxAccel()) \> 0* (the vehicle will not
    enter next lane even if it accelerates; hmph. ok)
  - if so:
    - save as a drive item; use vWish for speed, the vehicle will
      not move across the next link...
    - return
- nextLane = myLane; (currently examined lane)
- maxV = cfModel.maxNextSpeed(myState.mySpeed); (**recheck:** we have
bound the speed already\!?)
- dist = SPEED2DIST(maxV) + cfModel.brakeGap(maxV); (the distance that
interests us; **recheck:** is this really the upper bound?)
- vLinkPass = boundVSafe; (for the following computations, this will
be the max. speed we can take if we may continue the drive)
- vLinkWait = vLinkPass; (for the following computations, this will be
the max. speed we can take if we have to brake)
- view = 1; (how many lanes we've seen so far)
- go along subsequent edges, following the "best lane continuation"
  - if we have to handle a stop on this edge
    - adapt speed to the stop (min between current value and
      stopping speed for both, vLinkPass and vLinkWait) speeds are bound by the stop<br>
  **todo:** we did this for the current lane already\!\!\!

-   - if the lane does not allow to continue the drive:<br>
      - as in step\#1<br>
      **todo:** we did this for the current lane already\!\!\!

-   - we may quit, here...<br>
    **todo:** we did not take into regard the vehicles on this lane,
    yet\!\!

-   - the current lane is no boundary; vLinkWait = vLinkPass
    (**haeh**, I know it's right, but the explanation's odd)
  - nextLane = next (subsequent) lane; keep whether it is an
    internal one, increment view
  - vmaxNextLane = MAX2(cfModel.ffeV(this, seen,
    nextLane-\>getMaxSpeed()), nextLane-\>getMaxSpeed()); (the
    maximum speed for the next lane; **recheck:** why so
    complicated? Don't we just need the next lane's allowed speed?
  - vsafePredNextLane = interaction with leader on subsequent lane;
    **recheck:** here a strange "assertion" is done for the case the
    distance to leader is \<0 - the vehicle's speed is bound to the
    end of the lane; odd
  - vLinkPass = MIN3(vLinkPass, vmaxNextLane, vsafePredNextLane);
    (not faster than already bound, keep car-following, not faster
    than the next lane allows)
  - vLinkWait = MIN3(vLinkPass, vLinkWait, cfModel.ffeS(this,
    seen)); (not faster than already bound, let vehicle decelerate
    up to the lane's end)
  - process stops<br>
    *todo:* Again? Why?

-   - adapt speed to arrivalspeed<br>
    *todo:* Recheck, have not validated, yet

-   - compute whether the request to pass to the next lane shall be
    set (setRequest)<br>
      - setRequest |=
        ((\*link)-\>getState()\!=MSLink::LINKSTATE_TL_RED&&(vLinkPass\>0&\&dist-seen\>0));
        (if not in front of red, and passing speed\>0 and the end of
        the lane is not beyond what we have seen; **check:** is dist-seen\>0 secure?

-   - if we are in front of yellow/green and we can brake, then use
    vLinkWait, set no request<br>
    **todo:** why don't we stop in this case? The vehicle will not
    pass the intersection...

-   - once again, check for dist-seen\>0; **todo:** why? we did this
    already\!?

-   - save as a drive item; use vLinkPass and vLinkWait for speeds,
    and the computed setRequest
  - add the currently investigated lane's length to seen