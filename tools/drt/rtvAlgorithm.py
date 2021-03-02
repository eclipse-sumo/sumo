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

# @file    rtvAlgo.py
# @author  Giuliana Armellini
# @author  Pablo Alvarez
# @author  Philip Ritzer
# @date    2020-02-26

"""
Search the RTV Graph with the possible routes in DRTonline.py.
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

import traci

def simple(options, r_id_unassigned, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step, r_id_rejected):
    # search possible routes only for vehicles without route
    # the route search is approximate with an modifcated ec
    # simple algorithm
    rtv_dict = {}
    route_id = {}
    # list with all not served requests needed for r_bin for ILP
    r_id_rtv = list(r_all.keys())
    r_id_rtv = list(set(r_id_rtv) - set(r_id_served))
    for v_id in fleet:
        v_bin = [0] * len(fleet) # list of assigned vehicles for ILP
        v_bin[fleet.index(v_id)] = 1
        v_capacity = traci.vehicle.getPersonCapacity(v_id)
        next_stops = [s.actType for s in traci.vehicle.getStops(v_id) if s.actType]
        if len(next_stops) <= 1 and not traci.vehicle.getPersonIDList(v_id):
            # if vehicle idle
            # if 'not next_stops' not enough, last drop off stops stay in list #TODO this could be solved with #8168
            # search possible pairs to consider
            pairs = [x_id for x_id, x in sorted(rv_dict.items(), key=lambda e: e[1][0]) if set(r_id_unassigned) & set(x[2])]
            # get first pair with faster route
            first_pairs = [x for x in pairs if x.startswith(v_id)]
            for first_pair in first_pairs:
                # search all possible routes ids starting with this
                route = first_pair
                i = 1
                while i < 2*len(r_id_unassigned) and len(route.split("_"))-1 < 2*len(r_id_unassigned):
                    for pair in pairs:
                        if len(route.split("_"))-1 >= 2*len(r_id_unassigned):
                            break # no more stops possible

                        if not pair.startswith(route.split("_")[-1]):
                            continue # if pairs are not compatible

                        route_new = ("_").join([route, pair.split("_")[-1]])

                        # check if pick up before drop off
                        stops = route_new.split("_")
                        trip_pick = [s[:-1] for s in route_new.split('_')[1:] if s.endswith("y")]
                        trip_drop = [s[:-1] for s in route_new.split('_')[1:] if s.endswith("z")]
                        if len(trip_pick) != len(set(trip_pick)) or len(trip_drop) != len(set(trip_drop)):
                            continue # stop already in route

                        abort = False
                        for stop in trip_drop:
                            try:
                                index_pu = stops.index(('%sy' % stop))
                                index_do = stops.index(('%sz' % stop))
                            except:
                                abort = True
                                break # if stop not even picked up

                            if index_pu > index_do:
                                abort = True
                                break # request drop off before pick up (should not be assured with time windows?)
                        if abort:
                            continue

                        if route == first_pair:
                            pax = rv_dict[first_pair][1] + rv_dict[pair][1] # num passenger
                            route_tw = rv_dict[first_pair][0]+step + rv_dict[pair][0] # travel time
                        else:
                            pax = route_id[route][1] + rv_dict[pair][1] # num passenger
                            route_tw = route_id[route][0] + rv_dict[pair][0] # travel time
                        
                        # check capacity
                        if pax > v_capacity:
                            continue # capacity surpass

                        # check time window for stop
                        stop_id = pair.split("_")[-1]
                        if stop_id.endswith('y'):
                            stop_tw = r_all[stop_id[:-1]].tw_pickup
                            if route_tw > stop_tw[1]:
                                continue # max stop time surpass
                            elif route_tw < stop_tw[0] and pax == 1:
                                # if veh to early at stop, only possible if vehicle is empty (equivalent to pax = 1)
                                route_tw = stop_tw[0] # consider the extra stop time
                        else:
                            stop_tw = r_all[stop_id[:-1]].tw_dropoff
                            if route_tw > stop_tw[1]:
                                continue # max stop time surpass
                            # route_tw < stop_tw[0] only relevant if drop off time definition is implemented
                        
                        # add route id
                        route_id[route_new] = [route_tw, pax] # route time window and current passenger number
                        
                        #if len(route_new.split("_")) % 2 == 1:
                        # check if all reservation served                                
                        if set(trip_pick) == set(trip_drop):      
                            # add possible route to dict
                            r_bin = [0] * len(r_id_rtv) # list of assigned reservations for ILP
                            for s in trip_pick:
                                r_bin[r_id_rtv.index(s)] = 1
                            rtv_dict[route_new] = [route_tw, v_bin, r_bin, route_tw-sum(r_bin)*options.c_ko] 
                            # route_id: route travel time, served reservations, vehicle, requests, value
                            
                        route = route_new
                    i += 1
            if len(fleet) == 1:
                # if one vehicle darp, assign the fastest route with max reservation served
                key_list = list(rtv_dict.keys())
                value_list = [value[3] for key, value in rtv_dict.items()]
                key_index = value_list.index(min(value_list))
                route_id = key_list[key_index]
                return {route_id: rtv_dict[route_id]}, r_id_rtv
    
    return rtv_dict, r_id_rtv

def simple_rerouting(options, r_id_unassigned, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step):
    # search possible routes
    # simple algorithm
    rtv_dict = {}
    route_id = {}
    # list with all not served requests needed for r_bin for ILP
    r_id_rtv = list(r_all.keys())
    r_id_rtv = list(set(r_id_rtv) - set(r_id_served))
    for v_id in fleet:
        v_bin = [0] * len(fleet) # list of assigned vehicles for ILP
        v_bin[fleet.index(v_id)] = 1
        v_capacity = traci.vehicle.getPersonCapacity(v_id) - traci.vehicle.getPersonNumber(v_id) # left capacity
        
        # search possible pairs to consider
        assigned_v = [x_id for x_id, x in r_all.items() if x.vehicle == v_id] # add assigned to vehicle
        assigned_v = list(set(assigned_v) - set(r_id_served)) # remove already served
        picked_v = list(set(assigned_v) & set(r_id_picked)) # add picked up by vehicle

        filter_pairs = r_id_unassigned.copy() # add unassigned
        filter_pairs.append(v_id) # add vehicle
        filter_pairs.extend(assigned_v) # add to pairs

        pairs = [x_id for x_id, x in sorted(rv_dict.items(), key=lambda e: e[1][0]) if x[2][0] in filter_pairs and x[2][1] in filter_pairs]
        first_pairs = []
        # get first pairs
        if assigned_v:
            # if requests assigned, changes only possible after second stops to avoid detours
            # a better way for allow small detours should be consider. TODO for example:
            # tt 1 2 equivalent to tt 3 1 2, if so, add key to first pairs
            next_act = traci.vehicle.getStops(v_id, 1)[0]
            if next_act.arrival > 0: # if stop is occurring in this step
                next_act = traci.vehicle.getStops(v_id, 2)[1].actType.split(",")
            else:
                next_act = next_act.actType.split(",")
            next_id = []
            for s in next_act:
                if 'pickup' in s:
                    next_id.append('%sy' % s.split(" ")[1])
                else:
                    next_id.append('%sz' % s.split(" ")[1])
            first_pairs = ["%s_%s" % (v_id, "_".join(next_id))]
        else:
            # if not, consider all possible
            first_pairs = [x for x in pairs if x.startswith(v_id)]

        for first_pair in first_pairs:
            # search all possible routes ids starting with this
            route = first_pair
            i = 1
            while i <= (len(filter_pairs)-1)*2 and len(route.split("_"))-1 < (len(filter_pairs)-1)*2:
                for pair in pairs:
                    if not pair.startswith(route.split("_")[-1]):
                        continue # if pairs are not compatible

                    route_new = ("_").join([route, pair.split("_")[-1]])

                    # check if pick up before drop off
                    stops = route_new.split("_")
                    # consider picked requests
                    trip_pick = picked_v.copy()
                    trip_pick.extend([s[:-1] for s in route_new.split('_')[1:] if s.endswith("y")])
                    trip_drop = [s[:-1] for s in route_new.split('_')[1:] if s.endswith("z")]
                    if len(trip_pick) != len(set(trip_pick)) or len(trip_drop) != len(set(trip_drop)):
                        continue # stop already in route

                    abort = False
                    for stop in trip_drop:
                        try:
                            if stop in picked_v:
                                index_pu = -1 # if request already picked up, index is -1
                            else:
                                index_pu = stops.index(('%sy' % stop))
                            index_do = stops.index(('%sz' % stop))
                        except:
                            abort = True
                            break # if stop not even picked up

                        if index_pu > index_do:
                            abort = True
                            break # request drop off before pick up (should not be assured with time windows?)
                    if abort:
                        continue

                    if route == first_pair:
                        # if more than 1 reservation in stop, cannot call rvGraph
                        first_pair_pax = first_pair.count('y') - first_pair.count('z')
                        pax = first_pair_pax + rv_dict[pair][1] # num passenger
                        first_pair = ("_").join(first_pair.split("_")[:2]) # consider only one time
                        route_tw = rv_dict[first_pair][0]+step + rv_dict[pair][0] # travel time
                    else:
                        try:
                            pax = route_id[route][1] + rv_dict[pair][1] # num passenger
                            route_tw = route_id[route][0] + rv_dict[pair][0] # travel time
                        except:
                            print("Check")
                    # check capacity
                    if pax > v_capacity:
                        continue # capacity surpass

                    # check time window for stop
                    stop_id = pair.split("_")[-1]
                    if stop_id.endswith('y'):
                        stop_tw = r_all[stop_id[:-1]].tw_pickup
                        if route_tw > stop_tw[1]:
                            continue # max stop time surpass
                        elif route_tw < stop_tw[0] and pax == 1:
                            # if veh to early at stop, only possible if vehicle is empty (equivalent to pax = 1)
                            route_tw = stop_tw[0] # consider the extra stop time
                    else:
                        stop_tw = r_all[stop_id[:-1]].tw_dropoff
                        if route_tw > stop_tw[1]:
                            continue # max stop time surpass
                        # route_tw < stop_tw[0] only relevant if drop off time definition is implemented
                    
                    # add route id
                    route_id[route_new] = [route_tw, pax] # route time window and current passenger number
                    
                    #if len(route_new.split("_")) % 2 == 1:
                    # check if all reservation served                                
                    if set(trip_pick) == set(trip_drop):                            
                        # if requests assigned to vehicle, they must be on route
                        # picked requests already checked with extension of trip_pick
                        if set(assigned_v) != set(trip_pick) & set(assigned_v):
                            # route not possible
                            continue
                        else:
                            # add possible route to dict
                            r_bin = [0] * len(r_id_rtv) # list of all consider reservations for ILP
                            for s in trip_pick:
                                r_bin[r_id_rtv.index(s)] = 1
                            rtv_dict[route_new] = [route_tw, v_bin, r_bin, route_tw-sum(r_bin)*options.c_ko] 
                            # route_id: route travel time, served reservations, vehicle, requests, value
                        
                    route = route_new
                i += 1
        if len(fleet) == 1:
            # if one vehicle darp, assign the fastest route with max reservation served
            key_list = list(rtv_dict.keys())
            value_list = [value[3] for key, value in rtv_dict.items()]
            key_index = value_list.index(min(value_list))
            route_id = key_list[key_index]
            return {route_id: rtv_dict[route_id]}, r_id_rtv
    
    return rtv_dict, r_id_rtv

def search_routes(route, pairs, assigned_v, picked_v, r_all, rv_dict, r_id_rtv, v_capacity, v_bin, rtv_dict, routes_tree, options):
    # route means [route_id, route_tw, route_pax]
    route_id, route_tw, route_pax = route
    for pair in pairs:
        if not pair.startswith(route_id.split("_")[-1]):
            continue # if pairs are not compatible

        newroute_id = ("_").join([route_id, pair.split("_")[-1]])

        # check if pick up before drop off
        stops = newroute_id.split("_")
        # consider picked requests
        trip_pick = picked_v.copy()
        trip_pick.extend([s[:-1] for s in newroute_id.split('_')[1:] if s.endswith("y")])
        trip_drop = [s[:-1] for s in newroute_id.split('_')[1:] if s.endswith("z")]
        if len(trip_pick) != len(set(trip_pick)) or len(trip_drop) != len(set(trip_drop)):
            continue # stop already in route

        abort = False
        for stop in trip_drop:
            try:
                if stop in picked_v:
                    index_pu = -1 # if request already picked up, index is -1
                else:
                    index_pu = stops.index(('%sy' % stop))
                index_do = stops.index(('%sz' % stop))
            except:
                abort = True
                break # if stop not even picked up

            if index_pu > index_do:
                abort = True
                break # request drop off before pick up (should not be assured with time windows?)
        if abort:
            continue
        
        newroute_pax = route_pax + rv_dict[pair][1] # num passenger
        newroute_tw = route_tw + rv_dict[pair][0] # travel time
        
        # check capacity
        if newroute_pax > v_capacity:
            continue # capacity surpass

        # check time window for stop
        stop_id = pair.split("_")[-1]
        if stop_id.endswith('y'):
            stop_tw = r_all[stop_id[:-1]].tw_pickup
            if newroute_tw > stop_tw[1]:
                continue # max stop time surpass
            elif newroute_tw < stop_tw[0] and newroute_pax == 1:
                # if veh to early at stop, only possible if vehicle is empty (equivalent to pax = 1)
                newroute_tw = stop_tw[0] # consider the extra stop time
        else:
            stop_tw = r_all[stop_id[:-1]].tw_dropoff
            if newroute_tw > stop_tw[1]:
                continue # max stop time surpass
            # newroute_tw < stop_tw[0] only relevant if drop off time definition is implemented
        
        # add route id to tree
        routes_tree.append([newroute_id, newroute_tw, newroute_pax]) # route time window and current passenger number
        
        # check if all reservation served                                
        if set(trip_pick) == set(trip_drop):                            
            # if requests assigned to vehicle, they must be on route
            # picked requests already checked with extension of trip_pick
            if set(assigned_v) != set(trip_pick) & set(assigned_v):
                # route not possible
                continue
            else:
                # add possible route to dict
                r_bin = [0] * len(r_id_rtv) # list of all consider reservations for ILP
                for s in trip_pick:
                    r_bin[r_id_rtv.index(s)] = 1
                rtv_dict[newroute_id] = [newroute_tw, v_bin, r_bin, newroute_tw-sum(r_bin)*options.c_ko] 
                # route_id: route travel time, served reservations, vehicle, requests, value
            
    return routes_tree, rtv_dict

def exhaustive_search(options, r_id_unassigned, r_id_picked, r_id_served, r_all, fleet, v_type, rv_dict, step, memory_problems):
    # search possible routes
    # exhaustive search
    rtv_dict = {}
    # list with all not served requests needed for r_bin for ILP
    r_id_rtv = list(r_all.keys())
    r_id_rtv = list(set(r_id_rtv) - set(r_id_served))
    # TODO allow checking for vehicles with same characteristics and copy the 
    # rtv for them. To be the same -> no route assigned, same position and same capacity
    for v_id in fleet:
        v_bin = [0] * len(fleet) # list of assigned vehicles for ILP
        v_bin[fleet.index(v_id)] = 1
        v_capacity = traci.vehicle.getPersonCapacity(v_id) - traci.vehicle.getPersonNumber(v_id)
        
        # search possible pairs to consider
        assigned_v = [x_id for x_id, x in r_all.items() if x.vehicle == v_id] # add assigned to vehicle
        assigned_v = list(set(assigned_v) - set(r_id_served)) # remove already served
        picked_v = list(set(assigned_v) & set(r_id_picked)) # add picked up by vehicle

        filter_pairs = r_id_unassigned.copy() # add unassigned
        filter_pairs.append(v_id) # add vehicle
        filter_pairs.extend(assigned_v) # add to pairs

        pairs = [x_id for x_id, x in sorted(rv_dict.items(), key=lambda e: e[1][0]) if x[2][0] in filter_pairs and x[2][1] in filter_pairs]
        
        routes_tree = [[]] # list with incomplete routes
        i = 0
        # get first pairs
        if assigned_v:
            # if requests assigned, changes only possible after second stops to avoid detours
            # a better way for allow small detours should be consider. TODO for example:
            # tt 1 2 equivalent to tt 3 1 2, if so, add key to first pairs
            next_act = traci.vehicle.getStops(v_id, 1)[0]
            if next_act.arrival > 0: # if stop is occurring in this step
                next_act, next_id = traci.vehicle.getStops(v_id, 2)[1].actType.split(",")[0].split(" ")
            else:
                next_act, next_id = next_act.actType.split(",")[0].split(" ")
            if next_act == 'pickup':
                next_act = 'y'
            else:
                next_act = 'z'
            route_id = "%s_%s%s" % (v_id, next_id,next_act)
            route_tw = rv_dict[route_id][0]
            routes_tree[i] = [[route_id, route_tw+step, 1]]
        else:
            # if not, consider all possible
            routes_tree[i] = [[x, rv_dict[x][0]+step, 1] for x in pairs if x.startswith(v_id)]
        
        start_time = time.perf_counter()
        while routes_tree[i]:
            # exhaustive search
            routes_tree.append([])
            for route in routes_tree[i]:
                routes_tree[i + 1], rtv_dict = search_routes(route, pairs, assigned_v, picked_v, r_all, rv_dict, r_id_rtv, v_capacity, v_bin, rtv_dict, routes_tree[i + 1], options)
                if (time.perf_counter() - start_time) > options.rtv_time:
                    memory_problems.append(1)
                    break
            del routes_tree[i][:]
            i = i + 1  # next tree depth
        
    if len(fleet) == 1:
        # if one vehicle darp, assign the fastest route with max reservation served
        key_list = list(rtv_dict.keys())
        value_list = [value[3] for key, value in rtv_dict.items()]
        key_index = value_list.index(min(value_list))
        route_id = key_list[key_index]
        return {route_id: rtv_dict[route_id]}, r_id_rtv, memory_problems
    
    return rtv_dict, r_id_rtv, memory_problems