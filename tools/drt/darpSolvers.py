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

# @file    darpSolver.py
# @author  Giuliana Armellini
# @date    2021-05-27

"""
Contains the methods to solve the Dial a Ride Problem (DARP) of the
drtOnline.py tool.
Methods available: exhaustive_search and simple_rerouting
Track progress https://github.com/eclipse/sumo/issues/8256
"""

import os
import sys
import time

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import traci  # noqa
findRoute = traci.simulation.findRoute


def res_res_pair(options, res1, res2, veh_type, veh_time_pickup,
                 veh_time_dropoff, rv_dict, pairs_dua_times):
    """
    Search all combinations between two reservations. Notation: the first of
    the reservations is defined as 'res1' and the second as 'res2' and 'p'
    refers to pick up and 'd' to drop off a reservation. Then 'res2p_res1d'
    means pickup reservation 2 and then drop off reservation 1.
    """

    # combination 1p2p 2p2d 2d1d
    if (res1.tw_pickup[0] <= res2.tw_pickup[1] and
       res2.tw_dropoff[0] <= res1.tw_dropoff[1]):
        # if earliest pick up of req 1 before latest pick up of req 2 and
        # if earliest drop off of req 2 before latest drop off of req 1

        # calculate travel times for each pair
        res1p_res2p = pairs_dua_times.get("%s_%s" % (res1.fromEdge,
                                          res2.fromEdge))
        if res1p_res2p is None:
            res1p_res2p = int(findRoute(res1.fromEdge, res2.fromEdge, veh_type,
                              routingMode=options.routing_mode).travelTime)
        res1p_res2p += veh_time_pickup
        res2p_res2d = rv_dict.get('%sy_%sz' % (res2.id, res2.id))
        if res2p_res2d is None:
            res2p_res2d = res2.direct + veh_time_dropoff
            pair = '%sy_%sz' % (res2.id, res2.id)  # 2p2d
            rv_dict[pair] = [res2p_res2d, -1, [res2.id, res2.id]]
        else:
            res2p_res2d = res2p_res2d[0]

        res2d_res1d = pairs_dua_times.get("%s_%s" % (res2.toEdge, res1.toEdge))
        if res2d_res1d is None:
            res2d_res1d = int(findRoute(res2.toEdge, res1.toEdge, veh_type,
                              routingMode=options.routing_mode).travelTime)
        res2d_res1d += veh_time_dropoff

        # check if time windows constrains are fulfilled
        time_res2p = res1.tw_pickup[0] + res1p_res2p
        if time_res2p > res2.tw_pickup[1]:
            pass  # not possible
        elif (time_res2p + res2p_res2d) > res2.tw_dropoff[1]:
            pass  # not possible
        elif (time_res2p + res2p_res2d + res2d_res1d) > res1.tw_dropoff[1]:
            pass  # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (res1.id, res2.id)  # 1p2p
            rv_dict[pair] = [res1p_res2p, 1, [res1.id, res2.id]]
            pair = '%sz_%sz' % (res2.id, res1.id)  # 2d1d
            rv_dict[pair] = [res2d_res1d, -1, [res2.id, res1.id]]

    # combination 1p2p 2p1d 1d2d
    if (res1.tw_pickup[0] <= res2.tw_pickup[1] and
       res1.tw_dropoff[0] <= res2.tw_dropoff[1]):
        # if earliest pick up of req 1 before latest pick up of req 2 and
        # if earliest drop off of req 1 before latest drop off of req 2

        # calculate travel times for each pair
        res1p_res2p = rv_dict.get('%sy_%sy' % (res1.id, res2.id))
        if res1p_res2p is None:
            res1p_res2p = pairs_dua_times.get("%s_%s" % (res1.fromEdge,
                                              res2.fromEdge))
            if res1p_res2p is None:
                res1p_res2p = int(findRoute(res1.fromEdge, res2.fromEdge,
                                            veh_type,
                                            routingMode=options.routing_mode).travelTime) # noqa
            res1p_res2p += veh_time_pickup
        else:
            res1p_res2p = res1p_res2p[0]

        res2p_res1d = pairs_dua_times.get("%s_%s" % (res2.fromEdge,
                                                     res1.toEdge))
        if res2p_res1d is None:
            res2p_res1d = int(findRoute(res2.fromEdge, res1.toEdge, veh_type,
                                        routingMode=options.routing_mode).travelTime) # noqa
        res2p_res1d += veh_time_dropoff

        res1d_res2d = pairs_dua_times.get("%s_%s" % (res1.toEdge, res2.toEdge))
        if res1d_res2d is None:
            res1d_res2d = int(findRoute(res1.toEdge, res2.toEdge, veh_type,
                                        routingMode=options.routing_mode).travelTime) # noqa
        res1d_res2d += veh_time_dropoff

        # check if time windows constrains are fulfilled
        time_res2p = res1.tw_pickup[0] + res1p_res2p
        if time_res2p > res2.tw_pickup[1]:
            pass  # not possible
        elif (time_res2p + res2p_res1d) > res1.tw_dropoff[1]:
            pass  # not possible
        elif (time_res2p + res2p_res1d + res1d_res2d) > res2.tw_dropoff[1]:
            pass  # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (res1.id, res2.id)  # 1p2p
            if rv_dict.get(pair) is None:
                rv_dict[pair] = [res1p_res2p, 1, [res1.id, res2.id]]
            pair = '%sy_%sz' % (res2.id, res1.id)  # 2p1d
            rv_dict[pair] = [res2p_res1d, -1, [res2.id, res1.id]]
            pair = '%sz_%sz' % (res1.id, res2.id)  # 1d2d
            rv_dict[pair] = [res1d_res2d, -1, [res1.id, res2.id]]

    # combination 2p1p 1p2d 2d1d
    if (res2.tw_pickup[0] <= res1.tw_pickup[1] and
       res2.tw_dropoff[0] <= res1.tw_dropoff[1]):
        # if earliest pick up of req 2 before latest pick up of req 1 and
        # if earliest drop off of req 2 before latest drop off of req 1

        # calculate travel times for each pair
        res2p_res1p = pairs_dua_times.get("%s_%s" % (res2.fromEdge,
                                          res1.fromEdge))
        if res2p_res1p is None:
            res2p_res1p = int(findRoute(res2.fromEdge, res1.fromEdge, veh_type,
                              routingMode=options.routing_mode).travelTime)
        res2p_res1p += veh_time_pickup

        res1p_res2d = pairs_dua_times.get("%s_%s" % (res1.fromEdge,
                                          res2.toEdge))
        if res1p_res2d is None:
            res1p_res2d = int(findRoute(res1.fromEdge, res2.toEdge, veh_type,
                              routingMode=options.routing_mode).travelTime)
        res1p_res2d += veh_time_dropoff

        res2d_res1d = rv_dict.get('%sz_%sz' % (res2.id, res1.id))
        if res2d_res1d is None:
            # if 2d1d not added to dict in steps before
            res2d_res1d = pairs_dua_times.get("%s_%s" % (res2.toEdge,
                                              res1.toEdge))
            if res2d_res1d is None:
                res2d_res1d = int(findRoute(res2.toEdge, res1.toEdge, veh_type,
                                  routingMode=options.routing_mode).travelTime)
            res2d_res1d += veh_time_dropoff
        else:
            res2d_res1d = res2d_res1d[0]

        # check if time windows constrains are fulfilled
        time_res1p = res2.tw_pickup[0] + res2p_res1p
        if time_res1p > res1.tw_pickup[1]:
            pass  # not possible
        elif (time_res1p + res1p_res2d) > res2.tw_dropoff[1]:
            pass  # not possible
        elif (time_res1p + res1p_res2d + res2d_res1d) > res1.tw_dropoff[1]:
            pass  # not possible
        else:
            # pairs are possible
            pair = '%sy_%sy' % (res2.id, res1.id)  # 2p1p
            rv_dict[pair] = [res2p_res1p, 1, [res2.id, res1.id]]
            pair = '%sy_%sz' % (res1.id, res2.id)  # 1p2d
            rv_dict[pair] = [res1p_res2d, -1, [res1.id, res2.id]]
            pair = '%sz_%sz' % (res1.id, res2.id)  # 2d1d
            if rv_dict.get(pair) is None:
                rv_dict[pair] = [res2d_res1d, -1, [res2.id, res1.id]]

    # combination 2p1p 1p1d 1d2d
    if (res2.tw_pickup[0] <= res1.tw_pickup[1] and
       res1.tw_dropoff[0] <= res2.tw_dropoff[1]):
        # if earliest pick up of req 2 before latest pick up of req 1 and
        # if earliest drop off of req 1 before latest drop off of req 2

        # calculate travel times for each pair
        res2p_res1p = rv_dict.get('%sy_%sy' % (res2.id, res1.id), False)
        res1d_res2d = rv_dict.get('%sz_%sz' % (res1.id, res2.id), False)
        res1p_res1d = rv_dict.get('%sy_%sz' % (res1.id, res1.id), False)
        if not res1p_res1d:
            res1p_res1d = res1.direct + veh_time_dropoff
            pair = '%sy_%sz' % (res1.id, res1.id)  # 1p1d
            rv_dict[pair] = [res1p_res1d, -1, [res1.id, res1.id]]
        else:
            res1p_res1d = res1p_res1d[0]

        if not res2p_res1p or not res1d_res2d:
            # if 2p1p 1d2d not added to dict in steps before
            if not res2p_res1p:
                res2p_res1p = pairs_dua_times.get("%s_%s" % (res2.fromEdge,
                                                  res1.fromEdge))
                if res2p_res1p is None:
                    res2p_res1p = int(findRoute(res2.fromEdge, res1.fromEdge,
                                      veh_type, routingMode=options.routing_mode).travelTime) # noqa
                res2p_res1p += veh_time_pickup
            else:
                res2p_res1p = res2p_res1p[0]
            if not res1d_res2d:
                res1d_res2d = pairs_dua_times.get("%s_%s" % (res1.toEdge,
                                                  res2.toEdge))
                if res1d_res2d is None:
                    res1d_res2d = int(findRoute(res1.toEdge, res2.toEdge,
                                      veh_type, routingMode=options.routing_mode).travelTime) # noqa
                res1d_res2d += veh_time_dropoff
            else:
                res1d_res2d = res1d_res2d[0]
            res1p_res1d = rv_dict.get('%sy_%sz' % (res1.id, res1.id))[0]

            # check if time windows constrains are fulfilled
            time_res1p = res2.tw_pickup[0] + res2p_res1p
            if time_res1p > res1.tw_pickup[1]:
                pass  # not possible
            elif (time_res1p + res1p_res1d) > res1.tw_dropoff[1]:
                pass  # not possible
            elif (time_res1p + res1p_res1d + res1d_res2d) > res2.tw_dropoff[1]:
                pass  # not possible
            else:
                # pairs are possible
                pair = '%sy_%sy' % (res2.id, res1.id)  # 2p1p
                if not rv_dict.get(pair, False):
                    rv_dict[pair] = [res2p_res1p, 1, [res2.id, res1.id]]
                pair = '%sz_%sz' % (res1.id, res2.id)  # 1d2d
                if not rv_dict.get(pair, False):
                    rv_dict[pair] = [res1d_res2d, -1, [res1.id, res2.id]]

    # pair 1d2p
    if res1.tw_dropoff[0] <= res2.tw_pickup[1]:
        # if earliest drop off of req 1 before latest pick up of req 2

        # calculate travel times for each pair
        res1d_res2p = pairs_dua_times.get("%s_%s" % (res1.toEdge,
                                          res2.fromEdge))
        if res1d_res2p is None:
            res1d_res2p = int(findRoute(res1.toEdge, res2.fromEdge, veh_type,
                                        routingMode=options.routing_mode).travelTime) # noqa
        res1d_res2p += veh_time_pickup

        # check if time windows constrains are fulfilled
        if (res1.tw_dropoff[0] + res1d_res2p) < res2.tw_pickup[1]:
            pair = '%sz_%sy' % (res1.id, res2.id)  # 1d2p
            rv_dict[pair] = [res1d_res2p, 1, [res1.id, res2.id]]

    # pair 2d1p
    if res2.tw_dropoff[0] <= res1.tw_pickup[1]:
        # if earliest drop off of req 2 before latest pick up of req 1

        # calculate travel times for each pair
        res2d_res1p = pairs_dua_times.get("%s_%s" % (res2.toEdge,
                                          res1.fromEdge))
        if res2d_res1p is None:
            res2d_res1p = int(findRoute(res2.toEdge, res1.fromEdge, veh_type,
                                        routingMode=options.routing_mode).travelTime)  # noqa
        res2d_res1p += veh_time_pickup

        # check if time windows constrains are fulfilled
        if (res2.tw_dropoff[0] + res2d_res1p) < res1.tw_pickup[1]:
            pair = '%sz_%sy' % (res2.id, res1.id)  # 2d1p
            rv_dict[pair] = [res2d_res1p, 1, [res2.id, res1.id]]


def get_rv(options, res_id_new, res_id_picked, res_all,
           veh_type, veh_time_pickup, veh_time_dropoff, rv_dict, step,
           veh_edges, pairs_dua_times):
    """
    Generates the reservation-vehicle graph, which has the possible
    combinations between two reservations and between a reservation and a
    vehicle with the required travel time, the number of passengers picked up
    or dropped off and the reservation id and/or vehicle id of the pair.
    """
    # remove reservations already served or rejected due to processing time
    id_current = []
    [id_current.extend(veh_id) for veh_id in veh_edges.values()]
    id_current.extend(res_all.keys())
    [rv_dict.pop(key) for key in list(rv_dict)
     if set(rv_dict[key][2]) - set(id_current)]

    res_rv_remove = []
    res_all_remove = []
    for res_id, res in res_all.items():
        if res_id in res_id_new:
            # if reservation is new

            # check if reservation can be serve for a vehicle on time
            res_possible = False
            # add vehicle-reservation pairs
            for edge, vehicles in veh_edges.items():
                pickup_time = pairs_dua_times.get("%s_%s" % (edge,
                                                  res.fromEdge))
                if pickup_time is None:
                    pickup_time = findRoute(edge, res.fromEdge, veh_type,
                                            routingMode=options.routing_mode).travelTime # noqa
                if step+pickup_time > res.tw_pickup[1]:
                    # if vehicle arrives to late
                    continue
                # if vehicle on time, add to rv graph
                for veh_id in vehicles:
                    route_id = '%s_%sy' % (veh_id, res_id)
                    rv_dict[route_id] = [pickup_time+veh_time_pickup, 1, [veh_id, res_id]]  # noqa
                    res_possible = True

            if not res_possible:
                # reject reservation and remove person from simulation
                # TODO add no rejection option #8705
                print("Reservation %s (person %s) cannot be served" %
                      (res_id, res.persons))
                res_all_remove.append(res_id)
                res_rv_remove.extend(['%sy' % res_id, '%sz' % res_id])
                for person in res.persons:
                    traci.person.removeStages(person)
                continue

            # add direct route pair
            route_id = '%sy_%sz' % (res_id, res_id)  # y: pickup / z: drop off
            rv_dict[route_id] = [res.direct+veh_time_dropoff, -1, [res_id, res_id]]  # noqa

            # add reservation-reservation pairs
            reservations2 = set(res_all.keys()) ^ set(res_all_remove)
            for res2 in reservations2:
                if res2 != res_id:
                    res_res_pair(options, res, res_all.get(res2), veh_type,
                                 veh_time_pickup, veh_time_dropoff,
                                 rv_dict, pairs_dua_times)

        elif not res.vehicle:
            # if reservation not assigned
            # check if pick-up still possible
            if res.tw_pickup[1] < step:
                # latest pickup time exceed simulation time, reject reservation
                # TODO add no rejection option #8705
                print("Reservation %s (person %s) cannot be served" %
                      (res_id, res.persons))
                res_all_remove.append(res_id)
                res_rv_remove.extend(['%sy' % res_id, '%sz' % res_id])
                for person in res.persons:
                    traci.person.removeStages(person)
                continue

            remove = True
            for edge, vehicles in veh_edges.items():
                pickup_time = pairs_dua_times.get("%s_%s" % (edge,
                                                  res.fromEdge))
                if pickup_time is None:
                    pickup_time = findRoute(edge, res.fromEdge, veh_type,
                                            routingMode=options.routing_mode).travelTime # noqa
                if step+pickup_time <= res.tw_pickup[1]:
                    # if vehicle on time, add to rv graph
                    for veh_id in vehicles:
                        route_id = '%s_%sy' % (veh_id, res_id)
                        if rv_dict.get(route_id, False):
                            rv_dict[route_id][0] = pickup_time+60
                            remove = False
                else:
                    # remove pair if pick-up not possible
                    for veh_id in vehicles:
                        route_id = '%s_%sy' % (veh_id, res_id)
                        if rv_dict.get(route_id, False):
                            rv_dict.pop(route_id)

            if remove:
                # if no vehicles can pick up on time -> reject reservation
                print("Reservation %s (person %s) cannot be served" %
                      (res_id, res.persons))
                res_all_remove.append(res_id)
                res_rv_remove.extend(['%sy' % res_id, '%sz' % res_id])
                for person in res.persons:
                    traci.person.removeStages(person)

        elif res_id not in res_id_picked:
            # if reservation assigned but not picked up
            route_id = '%s_%sy' % (res.vehicle, res_id)
            if rv_dict.get(route_id, False):
                # update travel time to pickup
                if traci.vehicle.getRoadID(res.vehicle).startswith(':'):
                    # avoid routing error when in intersection TODO #5829
                    edge_index = traci.vehicle.getRouteIndex(res.vehicle) + 1
                    from_edge = traci.vehicle.getRoute(res.vehicle)[edge_index]
                else:
                    from_edge = traci.vehicle.getRoadID(res.vehicle)
                pickup_time = pairs_dua_times.get("%s_%s" % (from_edge,
                                                  res.fromEdge))
                if pickup_time is None:
                    pickup_time = findRoute(from_edge, res.fromEdge, veh_type,
                                            routingMode=options.routing_mode).travelTime # noqa
                rv_dict[route_id][0] = pickup_time+veh_time_pickup
                if options.verbose and step+pickup_time > res.tw_pickup[1]:  # Debug only # noqa
                    print("Time window surpassed by", step+pickup_time - res.tw_pickup[1]) # noqa

        elif res_id in res_id_picked:
            # if reservation picked up, delete pick-up pair
            res_rv_remove.append('%sy' % res_id)
            # update travel time to drop off
            if traci.vehicle.getRoadID(res.vehicle).startswith(':'):
                # avoid routing error when in intersection TODO #5829
                edge_index = traci.vehicle.getRouteIndex(res.vehicle) + 1
                from_edge = traci.vehicle.getRoute(res.vehicle)[edge_index]
            else:
                from_edge = traci.vehicle.getRoadID(res.vehicle)
            dropoff_time = pairs_dua_times.get("%s_%s" % (from_edge,
                                               res.toEdge))
            if dropoff_time is None:
                dropoff_time = int(findRoute(from_edge, res.toEdge, veh_type,
                                             routingMode=options.routing_mode).travelTime)  # noqa
            route_id = '%s_%sz' % (res.vehicle, res_id)
            rv_dict[route_id] = [dropoff_time+veh_time_dropoff, -1, [res.vehicle, res_id]]  # noqa
        else:
            print("Error: Reservation state not considered")

    # remove rejected, served and picked up reservations from rv graph
    if res_rv_remove:
        [rv_dict.pop(key) for key in list(rv_dict) if set(res_rv_remove) & set(key.split("_"))] # noqa

    # remove rejected reservations from res_all
    if res_all_remove:
        [res_all.pop(key) for key in res_all_remove]


def simple_rerouting(options, res_id_unassigned, res_id_picked,
                     res_all, fleet, rv_dict, step):
    """
    Search possible trips allowing vehicles to change their trip in real time
    (rerouting). Not all trips are covered, being the result not exact.
    """
    rtv_dict = {}
    trip_id = {}
    # remove reservations already served or rejected due to processing time
    id_current = []
    id_current.extend(fleet)
    id_current.extend(res_all.keys())
    [rv_dict.pop(key) for key in list(rv_dict)
     if set(rv_dict[key][2]) - set(id_current)]

    # list with all not served requests needed for res_bin for ILP
    rtv_res = list(res_all.keys())
    for veh_id in fleet:
        veh_bin = [0] * len(fleet)  # list of assigned vehicles for ILP
        veh_bin[fleet.index(veh_id)] = 1
        veh_capacity = (traci.vehicle.getPersonCapacity(veh_id)
                        - traci.vehicle.getPersonNumber(veh_id))

        # search possible pairs to consider
        # filter valid reservations id and vehicle id
        res_assigned_veh = [res_id for res_id, res in res_all.items()
                            if res.vehicle == veh_id]

        filter_valid = res_id_unassigned.copy()  # add unassigned reservations
        filter_valid.append(veh_id)  # add vehicle element
        filter_valid.extend(res_assigned_veh)  # add reservations assigned

        # possible pairs to consider, sort by increasing travel time
        pairs = [pair_id for pair_id, pair in
                 sorted(rv_dict.items(), key=lambda pair: pair[1][0])
                 if pair[2][0] in filter_valid and pair[2][1] in filter_valid]

        # reservations already picked up by the vehicle
        res_picked_veh = list(set(res_assigned_veh) & set(res_id_picked))

        first_pairs = []
        # get first pairs
        if res_assigned_veh:
            # if requests assigned, changes only possible after second stops
            # to avoid endless detours. A better way for allow small detours
            # should be consider. TODO for example: tt 1 2 equivalent to
            # tt 3 1 2, if so, add key to first pairs
            for next_stop in traci.vehicle.getStops(veh_id):
                next_act = next_stop.actType.split(",")[0].split(" ")[0]
                next_id = next_stop.actType.split(",")[0].split(" ")[-1][1:-1]
                if next_act == 'pickup' and next_id in res_id_picked:
                    # person already picked up, consider next stop
                    continue
                elif next_act == 'dropOff' and next_id not in res_all.keys():
                    # person already dropped off, consider next stop
                    continue
                else:
                    # next stop valid
                    if next_act == 'pickup':
                        first_pairs = ["%s_%sy" % (veh_id, next_id)]
                    else:
                        first_pairs = ["%s_%sz" % (veh_id, next_id)]
                    break
        else:
            # if not, consider all possible
            first_pairs = [pair for pair in pairs
                           if pair.split("_")[0] == veh_id]

        for first_pair in first_pairs:
            # search all possible trips ids starting with this
            trip = first_pair
            i = 1
            while (i <= (len(filter_valid)-1)*2 and
                   len(trip.split("_"))-1 < (len(filter_valid)-1)*2):
                for pair in pairs:
                    if pair.split("_")[0] != trip.split("_")[-1]:
                        continue  # if pairs are not compatible

                    trip_new = ("_").join([trip, pair.split("_")[-1]])

                    # check if pick up before drop off
                    stops = trip_new.split("_")
                    # consider picked requests
                    trip_pick = res_picked_veh.copy()
                    trip_pick.extend([stop[:-1] for stop in trip_new.split('_')[1:]  # noqa
                                     if stop.endswith("y")])
                    trip_drop = [stop[:-1] for stop in trip_new.split('_')[1:]
                                 if stop.endswith("z")]
                    if (len(trip_pick) != len(set(trip_pick))
                       or len(trip_drop) != len(set(trip_drop))):
                        continue  # stop already in trip

                    abort = False
                    for stop in trip_drop:
                        try:
                            if stop in res_picked_veh:
                                # if request already picked up, set index to -1
                                index_pu = -1
                            else:
                                index_pu = stops.index(('%sy' % stop))
                            index_do = stops.index(('%sz' % stop))
                        except ValueError:
                            abort = True
                            break  # if stop not even picked up

                        if index_pu > index_do:
                            # dropped off before pick up
                            # TODO shouldn't it be covered by time windows?
                            abort = True
                            break
                    if abort:
                        continue

                    # calculate number of passengers (pax)
                    # and travel time (trip_time)
                    if trip == first_pair:
                        pax = rv_dict[first_pair][1] + rv_dict[pair][1]
                        trip_time = (rv_dict[first_pair][0] + step
                                     + rv_dict[pair][0])
                    else:
                        pax = trip_id[trip][1] + rv_dict[pair][1]
                        trip_time = trip_id[trip][0] + rv_dict[pair][0]

                    # check capacity
                    if pax > veh_capacity:
                        continue  # capacity surpass

                    # check time window for stop
                    stop_id = pair.split("_")[-1]
                    if stop_id.endswith('y'):
                        stop_tw = res_all[stop_id[:-1]].tw_pickup
                        if trip_time > stop_tw[1]:
                            continue  # max stop time surpass
                        elif trip_time < stop_tw[0] and pax == 1:
                            # if vehicle is too early at stop, it can wait
                            # if it is empty (equivalent to pax == 1)
                            trip_time = stop_tw[0]  # waiting time at stop
                    else:
                        stop_tw = res_all[stop_id[:-1]].tw_dropoff
                        if trip_time > stop_tw[1]:
                            continue  # maximum stop time surpass
                        # TODO trip_time < stop_tw[0] only relevant if drop off time definition is implemented # noqa

                    # add trip id
                    trip_id[trip_new] = [trip_time, pax]

                    # if len(trip_new.split("_")) % 2 == 1:
                    # check if all reservation served
                    if set(trip_pick) == set(trip_drop):
                        # if requests assigned to vehicle, they must be on trip
                        # picked requests already checked with trip_pick
                        if (set(res_assigned_veh)
                           != set(trip_pick) & set(res_assigned_veh)):
                            # trip not possible
                            continue
                        else:
                            # add possible trip to dict
                            res_bin = [0] * len(rtv_res)  # reservation in trip
                            for stop in trip_pick:
                                res_bin[rtv_res.index(stop)] = 1
                            trip_cost = trip_time - sum(res_bin)*options.c_ko

                            # rtv_dict format:
                            # trip_id: travel time, vehicle, reservations, cost
                            rtv_dict[trip_new] = [trip_time, veh_bin, res_bin,
                                                  trip_cost]

                    trip = trip_new
                i += 1
        if len(fleet) == 1 and rtv_dict:
            # if one vehicle problem, assign the fastest trip with
            # max reservations served (equivalent to minor trip_cost)
            trips_list = list(rtv_dict.keys())
            costs_list = [value[3] for trip, value in rtv_dict.items()]
            trip_index = costs_list.index(min(costs_list))
            trip_id = trips_list[trip_index]
            return {trip_id: rtv_dict[trip_id]}, rtv_res

    return rtv_dict, rtv_res


def search_trips(trip, pairs, res_assigned_veh, res_picked_veh, res_all,
                 rv_dict, rtv_res, veh_capacity, veh_bin, rtv_dict, trips_tree,
                 options, start_time):
    """
    Search all possible new trips by adding a new stop to the current trip.
    If the maximum search time given is not surpass, all possible trips are
    consider.
    """
    # trip means [trip_id, trip_time, trip_pax]
    trip_id, trip_time, trip_pax = trip
    for pair in pairs:
        if (time.perf_counter() - start_time) > options.rtv_time:
            # limit trip search time
            break
        if pair.split("_")[0] != trip_id.split("_")[-1]:
            continue  # if pairs are not compatible

        new_trip_id = ("_").join([trip_id, pair.split("_")[-1]])

        # check if pick up before drop off
        stops = new_trip_id.split("_")
        # consider picked requests
        trip_pick = res_picked_veh.copy()
        trip_pick.extend([stop[:-1] for stop in new_trip_id.split('_')[1:]
                         if stop.endswith("y")])
        trip_drop = [stop[:-1] for stop in new_trip_id.split('_')[1:]
                     if stop.endswith("z")]
        if (len(trip_pick) != len(set(trip_pick))
           or len(trip_drop) != len(set(trip_drop))):
            continue  # stop already in trip

        abort = False
        for stop in trip_drop:
            try:
                if stop in res_picked_veh:
                    # if request already picked up, set index to -1
                    index_pu = -1
                else:
                    index_pu = stops.index(('%sy' % stop))
                index_do = stops.index(('%sz' % stop))
            except ValueError:
                abort = True
                break  # if stop not even picked up

            if index_pu > index_do:
                # dropped off before pick up
                # TODO shouldn't it be covered by time windows?
                abort = True
                break
        if abort:
            continue

        # calculate number of passengers (pax)
        # and travel time (trip_time)
        new_trip_pax = trip_pax + rv_dict[pair][1]
        new_trip_time = trip_time + rv_dict[pair][0]

        # check capacity
        if new_trip_pax > veh_capacity:
            continue  # capacity surpass

        # check time window for stop
        stop_id = pair.split("_")[-1]
        if stop_id.endswith('y'):
            stop_tw = res_all[stop_id[:-1]].tw_pickup
            if new_trip_time > stop_tw[1]:
                continue  # max stop time surpass
            elif new_trip_time < stop_tw[0] and new_trip_pax == 1:
                # if vehicle is too early at stop, it can wait
                # if it is empty (equivalent to pax == 1)
                new_trip_time = stop_tw[0]  # waiting time at stop
        else:
            stop_tw = res_all[stop_id[:-1]].tw_dropoff
            if new_trip_time > stop_tw[1]:
                continue  # max stop time surpass
            # TODO trip_time < stop_tw[0] only relevant if drop off time definition is implemented # noqa

        # add trip id to tree
        trips_tree.append([new_trip_id, new_trip_time, new_trip_pax])

        # check if all reservation served
        if set(trip_pick) == set(trip_drop):
            # if requests assigned to vehicle, they must be on trip
            # picked requests already checked with extension of trip_pick
            if set(res_assigned_veh) != set(trip_pick) & set(res_assigned_veh):
                # trip not possible
                continue
            else:
                # add possible trip to dict
                res_bin = [0] * len(rtv_res)  # reservation in trip
                for stop in trip_pick:
                    res_bin[rtv_res.index(stop)] = 1
                trip_cost = new_trip_time - sum(res_bin)*options.c_ko

                # rtv_dict format:
                # trip_id: travel time, vehicle, reservations, cost
                rtv_dict[new_trip_id] = [new_trip_time, veh_bin, res_bin,
                                         trip_cost]

    return trips_tree, rtv_dict


def exhaustive_search(options, res_id_unassigned, res_id_picked, res_all,
                      fleet, rv_dict, step, memory_problems, veh_edges):
    """
    Search all possible trips for each vehicle allowing dynamic rerouting.
    If the maximum search time given is not surpass, the method is exact.
    """

    rtv_dict = {}
    # list with all requests needed for res_bin for ILP
    rtv_res = list(res_all.keys())

    # find unique vehicles to avoid calculating same trips multiple times
    idle_veh = traci.vehicle.getTaxiFleet(0)
    for vehicles in veh_edges.values():
        # equivalent vehicles must be on same edge
        if len(vehicles) > 1:
            vehicles_capacity = [traci.vehicle.getPersonCapacity(veh_id) for veh_id in vehicles] # noqa
            vehicles_idle = [veh_id in idle_veh for veh_id in vehicles]
            vehicles_unique = []
            for veh_idx, veh_id in enumerate(vehicles):
                if veh_idx == 0:
                    vehicles_unique.append(veh_id)
                    continue
                if not vehicles_idle[veh_idx]:
                    # if veh not idle, then is unique
                    vehicles_unique.append(veh_id)
                    continue
                for compare_veh in range(veh_idx+1):
                    if (vehicles_idle[compare_veh] and
                       vehicles_capacity[veh_idx] == vehicles_capacity[compare_veh]): # noqa
                        # if vehicles idle and same capacity -> equivalent
                        vehicles_unique.append(vehicles[compare_veh])
                        break
        else:
            vehicles_unique = vehicles

        # find routes for unique vehicles
        for veh_id in set(vehicles_unique):
            veh_bin = [0] * len(fleet)  # list of assigned vehicles for ILP
            veh_bin[fleet.index(veh_id)] = 1
            veh_capacity = (traci.vehicle.getPersonCapacity(veh_id)
                            - traci.vehicle.getPersonNumber(veh_id))

            # search possible pairs to consider
            # filter valid reservations id and vehicle id
            res_assigned_veh = [res_id for res_id, res in res_all.items()
                                if res.vehicle == veh_id]

            filter_valid = res_id_unassigned.copy()  # add unassigned
            filter_valid.append(veh_id)  # add vehicle
            filter_valid.extend(res_assigned_veh)  # add assigned reservations

            # select only pairs with valid elements
            pairs = [pair_id for pair_id, pair in
                     sorted(rv_dict.items(), key=lambda pair: pair[1][0])
                     if pair[2][0] in filter_valid and
                     pair[2][1] in filter_valid]

            # reservations already picked up by the vehicle
            res_picked_veh = list(set(res_assigned_veh) & set(res_id_picked))

            trips_tree = [[]]  # list with incomplete trips
            i = 0
            # get first pairs
            if res_assigned_veh:
                # if requests assigned, changes only possible after second
                # stops to avoid detours. # TODO a better way for allow small
                # detours should be consider. Ex: tt 1 2 equivalent to tt 3 1 2
                for next_stop in traci.vehicle.getStops(veh_id):
                    next_act = next_stop.actType.split(",")[0].split(" ")[0]
                    next_id = next_stop.actType.split(",")[0].split(" ")[-1][1:-1] # noqa
                    if next_act == 'pickup' and next_id in res_id_picked:
                        # person already picked up, consider next stop
                        continue
                    elif next_act == 'dropOff' and next_id not in res_all.keys(): # noqa
                        # person already dropped off, consider next stop
                        continue
                    else:
                        # next stop valid
                        if next_act == 'pickup':
                            trip_id = "%s_%sy" % (veh_id, next_id)
                            trip_time = rv_dict[trip_id][0]
                            trips_tree[i] = [[trip_id, trip_time+step, 1]]
                            break
                        else:
                            trip_id = "%s_%sz" % (veh_id, next_id)
                            trip_time = rv_dict[trip_id][0]
                            trips_tree[i] = [[trip_id, trip_time+step, -1]]
                            break
            if not trips_tree[i]:
                # if vehicle not assigned or no valid stop found, consider all
                trips_tree[i] = [[pair, rv_dict[pair][0]+step, 1] for pair in pairs # noqa
                                 if pair.split("_")[0] == veh_id]

            start_time = time.perf_counter()
            while trips_tree[i]:
                # exhaustive search
                trips_tree.append([])
                for trip in trips_tree[i]:
                    trips_tree[i + 1], rtv_dict = search_trips(trip, pairs, res_assigned_veh, res_picked_veh, res_all,  # noqa
                                                               rv_dict, rtv_res, veh_capacity, veh_bin, rtv_dict,  # noqa
                                                               trips_tree[i + 1], options, start_time)  # noqa
                    if (time.perf_counter() - start_time) > options.rtv_time:
                        memory_problems.append(False)
                        break
                del trips_tree[i][:]
                i = i + 1  # next tree depth

        # copy the routes found for vehicles with same characteristics
        for veh_idx, veh_id in enumerate(vehicles):
            if veh_id in vehicles_unique:
                # rtv graph already found
                continue

            equal_veh_id = vehicles_unique[veh_idx]

            veh_bin = [0] * len(fleet)  # list of assigned vehicles for ILP
            veh_bin[fleet.index(veh_id)] = 1
            copy_rtv_keys = [key for key in rtv_dict.keys() if key.split("_")[0] == equal_veh_id] # noqa
            for key in copy_rtv_keys:
                new_trip_id = key.replace(equal_veh_id, veh_id)
                trip_time = rtv_dict[key][0]
                res_bin = rtv_dict[key][2]
                trip_cost = rtv_dict[key][3]

                rtv_dict[new_trip_id] = [trip_time, veh_bin, res_bin,
                                         trip_cost]

    if len(fleet) == 1 and rtv_dict:
        # if one vehicle problem, assign the fastest trip with
        # max reservations served (equivalent to minor trip_cost)
        trips_list = list(rtv_dict.keys())
        costs_list = [value[3] for trip, value in rtv_dict.items()]
        trip_index = costs_list.index(min(costs_list))
        trip_id = trips_list[trip_index]
        return {trip_id: rtv_dict[trip_id]}, None, memory_problems

    return rtv_dict, rtv_res, memory_problems


def main(options, step, fleet, veh_type, veh_time_pickup, veh_time_dropoff,
         res_all, res_id_new, res_id_unassigned, res_id_picked,
         veh_edges, pairs_dua_times):
    """
    Run specified solver
    """
    # define global variables. Needed to not lose information in the traci loop
    if 'rv_dict' not in globals():
        global rv_dict
        rv_dict = {}
    if 'exact_sol' not in globals():
        global exact_sol
        exact_sol = [True]

    if options.darp_solver == 'exhaustive_search':

        # search reservation-vehicles pairs (RV-Graph)
        get_rv(options, res_id_new, res_id_picked, res_all,
               veh_type, veh_time_pickup, veh_time_dropoff, rv_dict, step,
               veh_edges, pairs_dua_times)

        # search trips (RTV-Graph)
        rtv_dict, rtv_res, exact_sol = exhaustive_search(options, res_id_unassigned, res_id_picked,  # noqa
                                                         res_all, fleet, rv_dict, step,  # noqa
                                                         exact_sol, veh_edges)  # noqa

        return (rtv_dict, rtv_res, exact_sol)

    elif options.darp_solver == 'simple_rerouting':

        # search reservation-vehicles pairs (RV-Graph)
        get_rv(options, res_id_new, res_id_picked, res_all,
               veh_type, veh_time_pickup, veh_time_dropoff, rv_dict, step,
               veh_edges, pairs_dua_times)

        # search trips (RTV-Graph)
        rtv_dict, rtv_res = simple_rerouting(options, res_id_unassigned,
                                             res_id_picked,
                                             res_all, fleet, rv_dict, step)

        return (rtv_dict, rtv_res, [False])
