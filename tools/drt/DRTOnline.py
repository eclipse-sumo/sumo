#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    DRTOnline.py
# @author  Giuliana Armellini
# @author  Pablo Alvarez
# @author  Philip Ritzer
# @date    2020-02-15

import os
import sys
from argparse import ArgumentParser

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci

def initOptions():
    argParser = ArgumentParser()
    
    argParser.add_argument("-n", "--network", dest="network", help="SUMO network file", metavar="FILE", required=True)
    argParser.add_argument("-r", "--reservations", dest="reservations", help="File with reservations (persons)", metavar="FILE", required=True)
    argParser.add_argument("-v", "--taxi", dest="taxis", help="File with drt vehicles (vehicles with taxi device)", metavar="FILE", required=True)

    argParser.add_argument("--c_ko", dest="c_ko", help="cost of ignoring a request", type=float, default=1000000000000, required=False)
    argParser.add_argument("--cost_per_trip", dest="cost_per_trip", help="avoid using multiple vehicles if trip time is similar", type=float, default=600, required=False)

    argParser.add_argument("--debug", help="get all info steps", action='store_true')
    
    argParser.add_argument("--drf", dest="drf", help="direct route factor", type=int, default=2, required=False)
    argParser.add_argument("--drf_min", dest="drf_min", help="minimum time for cases with short trips", type=int, default=600, required=False)
    argParser.add_argument("--max_wait", dest="max_wait", help="maximum waiting time for pickup", type=int, default=900, required=False)
    argParser.add_argument("--max_diff", dest="max_diff", help="maximum difference with assigned time", type=int, default=300, required=False)
    argParser.add_argument("--veh_wait", dest="veh_wait", help="maximum waiting time for passenger in the vehicle", type=int, default=180, required=False)
    argParser.add_argument("--sim_step", dest="sim_step", help="time window for request collection", type=int, default=30, required=False)

    return argParser

def get_rv(options, r_new, r_pending, r_ids, fleet, v_type, rv_dict, step):
    r_rejected = []

    for x in r_ids:

        if x.rejected:
            # if reservation has been rejected
            continue
        
        if x.id in r_new:
            # add direct route pair
            route_id = '%sy_%sz' % (x.id, x.id)
            rv_dict[route_id] = (x.direct+60, -1, [x.id, x.id]) # TODO default stop time
        
        if x.state != 8: 
            # if not picked up
            if x.tw_pickup[1] < step:
                # pickup time window surpass simulation time -> reject request
                # TODO persons stay in simulation, how can they be deleted?
                # TODO no rejection option should be implement in future
                x.rejected = True
                r_rejected.append(x.id)
                print("reservation %s cannot be served" % x.id)
        
            else:
                # check if vehicles can arrive to pickup on time
                for v_id in fleet:
                    # calculate travel time to pickup
                    pickup_time = int(traci.simulation.findRoute(traci.vehicle.getRoadID(v_id), x.toEdge, v_type, routingMode=0).travelTime)
                    if step+pickup_time <= x.tw_pickup[1]:
                        # if vehicle on time, add to rv graph
                        route_id = '%s_%sz' % (v_id, x.id)
                        if rv_dict.get(route_id, False):
                            # update key
                            rv_dict[route_id][0] = pickup_time+60
                        else:
                            rv_dict[route_id] = [pickup_time+60, 1, [v_id, x.id]] # TODO default stop time
                    
                    else:
                        # if not, remove from rv
                        x.rejected = True
                        r_rejected.append(x.id)
                        print("reservation %s cannot be served" % x.id)




    # remove rejected reservations from rv graph
    for key, value in rv_dict.items():
        if set(value[2]) & set(r_rejected):
            del rv_dict[key]
                           


def main():
    
    # read inputs
    argParser = initOptions()
    options = argParser.parse_args()

    r_ids = []
    r_new = []
    rv_dict = {}

    # start traci
    traci.start(['sumo-gui', '--net-file', '%s' %options.network, '-r', 
    '%s,%s' % (options.reservations, options.taxis), 
    "--device.taxi.dispatch-algorithm", "traci"])

    # execute the TraCI control loop
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep(step)

        # get fleet and vType for route calculation 
        # only fleets with one vType are considered
        fleet = traci.vehicle.getTaxiFleet(-1)
        v_type = traci.vehicle.getTypeID(fleet[0])

        # get new reservations
        for x in traci.person.getTaxiReservations(1):           

            # search direct travel time 
            #TODO check if we can calculate all travel times at once (like duarouter)
            direct = int(traci.simulation.findRoute(x.fromEdge, x.toEdge, v_type, x.depart, routingMode=0).travelTime)

            # add new reservation attributes            
            setattr(x, 'direct', direct) # direct travel time
            setattr(x, 'tw_pickup', [x.depart, x.depart+options.max_wait]) # pickup time window
            # drop off time window
            if x.direct*options.drf < options.drf_min:
                setattr(x, 'tw_dropoff', [x.tw_pickup[0]+direct, x.tw_pickup[1]+direct+options.drf_min])
            else:
                setattr(x, 'tw_dropoff', [x.tw_pickup[0]+direct, x.tw_pickup[1]+direct*options.drf])
            setattr(x, 'served', False) # if reservation served
            setattr(x, 'rejected', False) # if reservation rejected
            
            # add to new reservations
            r_new.append(x)
            # add reservation to list
            r_ids.append(x)

        # check reservation not assigned yet
        r_pending = [x.id for x in traci.person.getTaxiReservations(2)]

        if r_pending:
            # search request-vehicles pairs
            get_rv(options, r_new, r_pending, r_ids, fleet, v_type, rv_dict, step)

        step += options.sim_step

    traci.close()

if __name__ == "__main__":
    main()