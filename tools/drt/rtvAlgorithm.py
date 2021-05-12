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

# @file    rtvAlgorithm.py
# @author  Giuliana Armellini
# @author  Pablo Alvarez
# @author  Philip Ritzer
# @date    2020-02-26

"""
Search the RTV Graph with the possible trips in DRTonline.py.
The script contains different methods/algorithms for the search.
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


def simple(options, res_id_unassigned, res_id_picked, res_id_served, res_all, fleet, veh_type, rv_dict, step):  # noqa
    """
    Search possible trips only for vehicles without an assigned trip (no
    rerouting). Not all trips are covered, being the result not exact.
    """
    rtv_dict = {}
    trip_id = {}

    # list with all not served requests needed for res_bin for ILP
    rtv_res = list(res_all.keys())
    rtv_res = list(set(rtv_res) - set(res_id_served))
    for veh_id in fleet:
        veh_bin = [0] * len(fleet)  # list of assigned vehicles for ILP
        veh_bin[fleet.index(veh_id)] = 1
        veh_capacity = traci.vehicle.getPersonCapacity(veh_id)
        next_stops = [stop.actType for stop in traci.vehicle.getStops(veh_id)
                      if stop.actType]
        if len(next_stops) <= 1 and not traci.vehicle.getPersonIDList(veh_id):
            # if vehicle idle
            # possible pairs to consider, sort by increasing travel time
            pairs = [pair_id for pair_id, pair in
                     sorted(rv_dict.items(), key=lambda pair: pair[1][0])
                     if set(res_id_unassigned) & set(pair[2])]
            # get first pair with faster trip
            first_pairs = [pair for pair in pairs if
                           pair.split("_")[0] == veh_id]
            for first_pair in first_pairs:
                # search all possible trips ids starting with first_pair
                trip = first_pair
                i = 1
                while (i < 2*len(res_id_unassigned) and
                       len(trip.split("_"))-1 < 2*len(res_id_unassigned)):
                    for pair in pairs:
                        if len(trip.split("_"))-1 >= 2*len(res_id_unassigned):
                            break  # no more stops possible

                        if pair.split("_")[0] != trip.split("_")[-1]:
                            continue  # if pairs are not compatible

                        trip_new = ("_").join([trip, pair.split("_")[-1]])

                        # check if pick up before drop off
                        stops = trip_new.split("_")
                        trip_pick = [stop[:-1] for stop in trip_new.split('_')[1:]  # noqa
                                     if stop.endswith("y")]
                        trip_drop = [stop[:-1] for stop in trip_new.split('_')[1:]  # noqa
                                     if stop.endswith("z")]
                        if (len(trip_pick) != len(set(trip_pick)) or
                           len(trip_drop) != len(set(trip_drop))):
                            continue  # stop already in trip

                        abort = False
                        for stop in trip_drop:
                            try:
                                index_pu = stops.index(('%sy' % stop))
                                index_do = stops.index(('%sz' % stop))
                            except ValueError:
                                # dropped of but not picked up
                                abort = True
                                break

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
                            # add possible trip to dict
                            res_bin = [0] * len(rtv_res)  # reservation in trip
                            for stop in trip_pick:
                                res_bin[rtv_res.index(stop)] = 1
                            trip_cost = trip_time-sum(res_bin)*options.c_ko

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


def simple_rerouting(options, res_id_unassigned, res_id_picked, res_id_served, res_all, fleet, veh_type, rv_dict, step):  # noqa
    """
    Search possible trips allowing vehicles to change their trip in real time
    (rerouting). Not all trips are covered, being the result not exact.
    """
    rtv_dict = {}
    trip_id = {}
    # list with all not served requests needed for res_bin for ILP
    rtv_res = list(res_all.keys())
    rtv_res = list(set(rtv_res) - set(res_id_served))
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


def search_trips(trip, pairs, res_assigned_veh, res_picked_veh, res_all, rv_dict, rtv_res,  # noqa
                  veh_capacity, veh_bin, rtv_dict, trips_tree, options, start_time):  # noqa
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


def exhaustive_search(options, res_id_unassigned, res_id_picked, res_all, fleet, veh_type, rv_dict, step, memory_problems, veh_edges):  # noqa
    """
    Search all possible trips for each vehicle allowing dynamic rerouting.
    If the maximum search time given is not surpass, the method is exact.
    """

    rtv_dict = {}
    # list with all requests needed for res_bin for ILP
    rtv_res = list(res_all.keys())
    # TODO allow checking for vehicles with same characteristics and copy the
    # rtv for them. To be the same -> no trip assigned, same position and
    # same capacity
    idle_veh = traci.vehicle.getTaxiFleet(0)
    for edge, vehicles in veh_edges.items():
        if len(vehicles) > 1:
            vehicles_capacity = [traci.vehicle.getPersonCapacity(veh_id) for veh_id in vehicles]
            vehicles_idle = [veh_id in idle_veh for veh_id in vehicles]
            vehicles_unique = []
            for veh_idx, veh_id in enumerate(vehicles):
                if veh_idx == 0:
                    vehicles_unique.append(veh_id)
                    continue
                if not vehicles_idle[veh_idx]:
                    vehicles_unique.append(veh_id)
                    continue
                for compare_veh in range(veh_idx+1):
                    if (vehicles_idle[compare_veh] and
                       vehicles_capacity[veh_idx] == vehicles_capacity[compare_veh]):
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
                     if pair[2][0] in filter_valid and pair[2][1] in filter_valid]

            # reservations already picked up by the vehicle
            res_picked_veh = list(set(res_assigned_veh) & set(res_id_picked))

            trips_tree = [[]]  # list with incomplete trips
            i = 0
            # get first pairs
            if res_assigned_veh:
                # if requests assigned, changes only possible after second stops to
                # avoid detours. # TODO a better way for allow small detours should
                # be consider. for example: tt 1 2 equivalent to tt 3 1 2
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
                            trip_id = "%s_%sy" % (veh_id, next_id)
                            trip_time = rv_dict[trip_id][0]
                            trips_tree[i] = [[trip_id, trip_time+step, 1]]
                            break
                        else:
                            trip_id = "%s_%sz" % (veh_id, next_id)
                            trip_time = rv_dict[trip_id][0]
                            trips_tree[i] = [[trip_id, trip_time+step, -1]]
                            break
                        if not rv_dict.get(trip_id, False):
                            # TODO manage teleports
                            continue
            if not trips_tree[i]:
                # if vehicle not assigned or no valid stop found, consider all
                trips_tree[i] = [[pair, rv_dict[pair][0]+step, 1] for pair in pairs
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
                        memory_problems.append(1)
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
            copy_rtv_keys = [key for key in rtv_dict.keys() if key.split("_")[0] == equal_veh_id]
            for key in copy_rtv_keys:
                new_trip_id = key.replace(equal_veh_id, veh_id)
                trip_time = rtv_dict[key][0]
                res_bin = rtv_dict[key][2]
                trip_cost = rtv_dict[key][3]

                rtv_dict[new_trip_id] = [trip_time, veh_bin, res_bin, trip_cost]

    if len(fleet) == 1 and rtv_dict:
        # if one vehicle problem, assign the fastest trip with
        # max reservations served (equivalent to minor trip_cost)
        trips_list = list(rtv_dict.keys())
        costs_list = [value[3] for trip, value in rtv_dict.items()]
        trip_index = costs_list.index(min(costs_list))
        trip_id = trips_list[trip_index]
        return {trip_id: rtv_dict[trip_id]}, rtv_res, memory_problems

    return rtv_dict, rtv_res, memory_problems
