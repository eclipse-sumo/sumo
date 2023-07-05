# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2021-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    ortools_pdp.py
# @author  Philip Ritzer
# @author  Johannes Rummel
# @date    2021-12-16

"""
Capacitated vehicle routing problem with pickup and delivery.

Based on https://developers.google.com/optimization/routing/pickup_delivery#complete_programs
"""

from __future__ import print_function

import numpy as np

from ortools.constraint_solver import routing_enums_pb2
from ortools.constraint_solver import pywrapcp


def get_solution(data, manager, routing, solution, verbose):
    """Returns the solution as a dict with one entry for each
    vehicle (vehicle id: [0, ..., n_veh-1]) including the
    route (list of nodes) and costs."""
    if verbose:
        print('Objective: %s' % solution.ObjectiveValue())
    time_dimension = routing.GetDimensionOrDie('Time')
    solution_dict = {}
    total_cost = 0
    for vehicle_id in range(data['num_vehicles']):
        route = []
        index = routing.Start(vehicle_id)
        if verbose:
            plan_output = 'Route for vehicle %s:\n    ' % vehicle_id
        route_cost = 0
        route_load = 0
        while not routing.IsEnd(index):
            current_node = manager.IndexToNode(index)
            route_load += data['demands'][current_node]
            time_var = time_dimension.CumulVar(index)
            if verbose:
                plan_output += (' %s (L: %s, C: %s, T: (%s,%s))\n -> ' %
                                (current_node, route_load, route_cost, solution.Min(time_var), solution.Max(time_var)))
            route.append(current_node)
            previous_index = index
            index = solution.Value(routing.NextVar(index))
            route_cost += routing.GetArcCostForVehicle(previous_index, index, vehicle_id)
        last_node = manager.IndexToNode(index)
        route_load += data['demands'][last_node]
        time_var = time_dimension.CumulVar(index)
        route.append(last_node)
        if verbose:
            plan_output += (' %s (L: %s, C: %s, T: (%s,%s))\n' %
                            (last_node, route_load, route_cost, solution.Min(time_var), solution.Max(time_var)))
            plan_output += 'Costs of the route: %s\n' % route_cost
            print(plan_output)
        total_cost += route_cost
        solution_dict[vehicle_id] = [route, total_cost]
    if verbose:
        print('Total cost of the routes: %s' % total_cost)
    return solution_dict


def set_travel_cost(data, routing, manager, verbose):
    assert(type(data) == dict)
    assert(type(routing) == pywrapcp.RoutingModel)
    assert(type(manager) == pywrapcp.RoutingIndexManager)
    assert(type(verbose) == bool)
    # Create and register a transit callback.

    def distance_callback(from_index, to_index):
        """Returns the distance between the two nodes."""
        # Convert from routing variable Index to distance matrix NodeIndex.
        from_node = manager.IndexToNode(from_index)
        to_node = manager.IndexToNode(to_index)
        return data['cost_matrix'][from_node][to_node]

    if verbose:
        print(' Register distance callback.')
    transit_callback_index = routing.RegisterTransitCallback(distance_callback)

    # Define cost of each arc.
    routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index)

    return transit_callback_index


def add_cost_constraint(data, routing, transit_callback_index, verbose):
    # Add costs/distance constraint.
    if verbose:
        print(' Add distance constraints...')
    matrix_costs = int(np.sum(data['cost_matrix']))
    dimension_name = 'Costs'
    routing.AddDimension(
        transit_callback_index,
        0,  # no slack
        matrix_costs,  # TODO reasonable max costs/distance
        True,  # start cumul to zero
        dimension_name)
    distance_dimension = routing.GetDimensionOrDie(dimension_name)
    # the following tries to reduce the route costs of the vehicle with maximum costs
    # distance_dimension.SetGlobalSpanCostCoefficient(0)
    # the following tries to reduce the sum of all route costs
    # distance_dimension.SetSpanCostCoefficientForAllVehicles(0)
    return distance_dimension


def add_transportation_requests_constraint(data, routing, manager, solver, distance_dimension, verbose):
    if verbose:
        print(' Add pickup and delivery constraints...')
    for request in data['pickups_deliveries']:
        pickup_index = manager.NodeToIndex(request.from_node)
        delivery_index = manager.NodeToIndex(request.to_node)
        if verbose:
            print('pickup/dropoff nodes: %s/%s' % (request.from_node, request.to_node))
        routing.AddPickupAndDelivery(pickup_index, delivery_index)  # helps the solver
        # use same veh for pickup and dropoff
        solver.Add(routing.VehicleVar(pickup_index) == routing.VehicleVar(delivery_index))
        solver.Add(
            distance_dimension.CumulVar(pickup_index) <=
            distance_dimension.CumulVar(delivery_index))  # define order: first pickup then dropoff
        if hasattr(request, 'is_new') and request.is_new:  # is that a new request?
            # allows to reject the order but gives penalty
            if verbose:
                print('allow to reject new reservation %s' % (request.id))
            routing.AddDisjunction([pickup_index, delivery_index], 10000, 2)


def add_direct_route_factor_constraint(data, routing, manager, solver, distance_dimension, verbose):
    if data['drf'] != -1:
        if verbose:
            print(' Add direct route factor constraints...')
        for request in data['pickups_deliveries']:  # if the costs changed and cannot hold anymore? --> see 'dropoffs'
            pickup_index = manager.NodeToIndex(request.from_node)
            delivery_index = manager.NodeToIndex(request.to_node)
            # let the route cost be less or equal the direct route cost times drf
            direct_route_cost = request.direct_route_cost
            solver.Add(
                distance_dimension.CumulVar(delivery_index) - distance_dimension.CumulVar(pickup_index) <=  # route cost
                solver.IntConst(round(direct_route_cost * data['drf'])))  # direct route cost times direct route factor
            route_cost = distance_dimension.CumulVar(delivery_index) - distance_dimension.CumulVar(pickup_index)
            direct_route_cost_drf = solver.IntConst(round(direct_route_cost * data['drf']))
            if verbose:
                print('reservation %s with direct route cost %s * drf %s = max cost %s' % (
                    request.id, direct_route_cost, data['drf'], direct_route_cost_drf.Value()))
        # if possible, let the route costs of the dropoffs less than the drf allows,
        # else minimize the route costs (in case the costs became larger than expected)
        for request in data['dropoffs']:  # TODO: not sure that it works, test needed!
            direct_route_cost = request.direct_route_cost
            direct_route_cost_drf = solver.IntConst(round(direct_route_cost * data['drf']))
            delivery_index = manager.NodeToIndex(request.to_node)
            route_cost = (distance_dimension.CumulVar(delivery_index) +  # cost from veh start to dropoff node
                          solver.IntConst(round(request.current_route_cost)))  # cost from pickup to veh start
            # max_cost = solver.Max(route_cost, direct_route_cost_drf)
            # objective_to_minimize = solver.Minimize(max_cost, 10)
            # objective_to_minimize.EnterSearch()
            # solver.Add(max_cost <= objective_to_minimize)
            # routing.AddVariableMinimizedByFinalizer(max_cost.Var())
            # solver.Add(
            #    route_cost <=  # route cost
            #    direct_route_cost_drf  # direct route cost times direct route factor
            # )
            # routing_dimension = routing.GetDimensionOrDie('Costs')
            distance_dimension.SetCumulVarSoftUpperBound(delivery_index, round(
                direct_route_cost * data['drf'] - request.current_route_cost), 10)
            if verbose:
                print('reservation %s with max cost %s, already used costs %s, possible route costs %s' %
                      (request.id, direct_route_cost_drf.Value(), request.current_route_cost, route_cost))


def add_dropoff_constraint(data, routing, manager, verbose):
    if verbose:
        print(' Add dropoff constraints...')
    # for veh_index, do_list in enumerate(data['dropoffs']):
    #    if verbose:
    #        print('vehicle %s with %s dropoffs' % (veh_index, len(do_list)))
    #    for do in do_list:
    #        index = manager.NodeToIndex(do[0])
    #        # start node
    #        veh_node = data['starts'][veh_index]
    #        if verbose:
    #            print('vehicle %s (%s), dropoff %s (%s), res_id %s' % (veh_index, veh_node, do[0], index, do[1]))
    #        #routing.VehicleVar(index).SetValues([-1,veh_index])
    #        routing.SetAllowedVehiclesForIndex([veh_index],index)
    for res in data['dropoffs']:
        if verbose:
            print('reservation %s in veh %s(%s), droppoff node: %s' %
                  (res.id, res.vehicle, res.vehicle_index, res.to_node))
        index = manager.NodeToIndex(res.to_node)
        routing.SetAllowedVehiclesForIndex([res.vehicle_index], index)


def add_allocation_constraint(data, routing, manager, verbose):
    if verbose:
        print(' Add "no re-allocation" constraints...')
    for res in data['pickups_deliveries']:
        if hasattr(res, 'vehicle_index'):
            if verbose:
                print('reservation %s in veh id=%s' % (res.id, res.vehicle_index))
            index = manager.NodeToIndex(res.to_node)
            routing.SetAllowedVehiclesForIndex([res.vehicle_index], index)


def add_capacity_constraint(data, routing, manager, verbose):
    if verbose:
        print(' Add capacity constraints...')

    def demand_callback(from_index):
        """Returns the demand of the node."""
        # Convert from routing variable Index to demands NodeIndex.
        from_node = manager.IndexToNode(from_index)
        return data['demands'][from_node]
    demand_callback_index = routing.RegisterUnaryTransitCallback(demand_callback)
    routing.AddDimensionWithVehicleCapacity(
        demand_callback_index,
        0,  # null capacity slack
        data['vehicle_capacities'],  # vehicle maximum capacities
        True,  # start cumul to zero
        'Capacity')


def add_time_windows_constraint(data, routing, manager, verbose):
    if verbose:
        print(' Add time windows constraints...')

    def time_callback(from_index, to_index):
        """Returns the travel time between the two nodes."""
        # Convert from routing variable Index to time matrix NodeIndex.
        from_node = manager.IndexToNode(from_index)
        to_node = manager.IndexToNode(to_index)
        return data['time_matrix'][from_node][to_node]

    time_callback_index = routing.RegisterTransitCallback(time_callback)
    # routing.SetArcCostEvaluatorOfAllVehicles(time_callback_index)
    dimension_name = 'Time'
    routing.AddDimension(
        time_callback_index,
        int(data['max_time']),  # allow waiting time
        int(data['max_time']),  # maximum time per vehicle
        False,  # Don't force start cumul to zero.
        dimension_name)
    time_dimension = routing.GetDimensionOrDie(dimension_name)
    # Add time window constraints for each location except depot.
    for location_idx, time_window in enumerate(data['time_windows']):
        # if location_idx == data['depot']:
        # if location_idx in data['starts'] or location_idx == 0:
        if location_idx == 0:
            continue
        index = manager.NodeToIndex(location_idx)
        if verbose:
            print('window for node %s: [%s, %s]' % (location_idx, time_window[0], time_window[1]))
        time_dimension.CumulVar(index).SetRange(time_window[0], time_window[1])  # TODO: check if set, else ignore it
    # TODO: check if the followwing is needed
    # # Add time window constraints for each vehicle start node.
    # depot_idx = data['depot']
    # for vehicle_id in range(data['num_vehicles']):
    #     index = routing.Start(vehicle_id)
    #     time_dimension.CumulVar(index).SetRange(
    #         data['time_windows'][depot_idx][0],
    #         data['time_windows'][depot_idx][1])
    # for i in range(data['num_vehicles']):
    #     routing.AddVariableMinimizedByFinalizer(
    #         time_dimension.CumulVar(routing.Start(i)))
    #     routing.AddVariableMinimizedByFinalizer(
    #         time_dimension.CumulVar(routing.End(i)))


def solve_from_initial_solution(routing, manager, search_parameters, data, verbose):
    solution_requests = data['initial_routes']
    # get inital solution
    inital_solution = []
    if solution_requests is not None:
        for index_vehicle in solution_requests:
            # use request ids ([0]) here and align with current status of the requests
            request_order = solution_requests[index_vehicle][0].copy()
            for request_id in set(solution_requests[index_vehicle][0]):
                # 0: done
                # 1: only drop-off left
                # 2: pick-up and drop-off left
                old_status = solution_requests[index_vehicle][0].count(request_id)
                new_status = 0
                if request_id in [req.id for req in data['pickups_deliveries']]:
                    new_status = 2
                elif request_id in [req.id for req in data['dropoffs']]:
                    new_status = 1
                if new_status == 0:
                    # remove complete request
                    request_order = [req for req in request_order if req != request_id]
                if old_status == 2 and new_status == 1:
                    # remove first occurance of the request
                    request_order.remove(request_id)
            # translate new requests order (ids) to nodes order
            # (e.g. [0,1,2,1,2] -> [0.to_node, 1.from_node, 2.from_node, 1.to_node, 2.to_node])
            request_id_set = set(request_order)  # e.g. [0,1,2]
            # first occurance from behind (will be "to_node")
            reverserd_request_order = request_order.copy()
            reverserd_request_order.reverse()  # e.g. [2,1,2,1,0]
            first_occurance_from_behind = [reverserd_request_order.index(id) for id in request_id_set]  # e.g. [0,1,4]
            all_requests = data['pickups_deliveries'].copy()
            all_requests.extend(data['dropoffs'].copy())
            nodes_order = []
            for index, req_id in enumerate(reverserd_request_order):
                req = [r for r in all_requests if r.id == req_id][0]
                if index in first_occurance_from_behind:
                    nodes_order.insert(0, manager.NodeToIndex(req.to_node))
                else:
                    nodes_order.insert(0, manager.NodeToIndex(req.from_node))
            # nodes_order = solution_requests[index_vehicle][2]  # [2] for nodes
            inital_solution.append(nodes_order)
    routing.CloseModelWithParameters(search_parameters)
    if verbose:
        print('Initial solution:')
        for index_vehicle, index_list in enumerate(inital_solution):
            print('veh %s: %s' % (index_vehicle, [manager.IndexToNode(index) for index in index_list]))
    initial_solution = routing.ReadAssignmentFromRoutes(inital_solution, True)
    solution = routing.SolveFromAssignmentWithParameters(initial_solution, search_parameters)
    return solution


def set_first_solution_heuristic(time_limit_seconds, verbose):
    if verbose:
        print(' Set solution heuristic...')
    search_parameters = pywrapcp.DefaultRoutingSearchParameters()
    search_parameters.first_solution_strategy = routing_enums_pb2.FirstSolutionStrategy.PATH_MOST_CONSTRAINED_ARC
    # search_parameters.first_solution_strategy = routing_enums_pb2.FirstSolutionStrategy.PATH_CHEAPEST_ARC
    # search_parameters.local_search_metaheuristic = routing_enums_pb2.LocalSearchMetaheuristic.GUIDED_LOCAL_SEARCH
    search_parameters.time_limit.FromSeconds(time_limit_seconds)
    return search_parameters


def main(data, time_limit_seconds=10, verbose=False):
    """Entry point of the program."""
    # Create the routing index manager.
    manager = pywrapcp.RoutingIndexManager(
        len(data['cost_matrix']), data['num_vehicles'],
        data['starts'], data['ends'])

    # Create Routing Model.
    routing = pywrapcp.RoutingModel(manager)
    # get solver
    solver = routing.solver()

    # define transit_callback and set travel cost
    transit_callback_index = set_travel_cost(data, routing, manager, verbose)
    # Add costs/distance constraint.
    distance_dimension = add_cost_constraint(data, routing, transit_callback_index, verbose)

    # Define Transportation Requests.
    add_transportation_requests_constraint(data, routing, manager, solver, distance_dimension, verbose)

    # Set direct route factor.
    add_direct_route_factor_constraint(data, routing, manager, solver, distance_dimension, verbose)

    # Force the vehicle to drop-off the reservations it already picked up
    add_dropoff_constraint(data, routing, manager, verbose)

    # If no reallocation is desired
    if data['fix_allocation']:
        add_allocation_constraint(data, routing, manager, verbose)

    # Add Capacity constraint.
    add_capacity_constraint(data, routing, manager, verbose)

    # Add time window constraints.
    add_time_windows_constraint(data, routing, manager, verbose)

    print('## Done')
    # Setting first solution heuristic.
    search_parameters = set_first_solution_heuristic(time_limit_seconds, verbose)

    # Solve the problem.
    if verbose:
        print('Start solving the problem.')
    if data['initial_routes']:
        # TODO: change ids of the nodes, due to already picked up or droped off requests after last solution!
        solution = solve_from_initial_solution(routing, manager, search_parameters, data, verbose)
    else:
        solution = routing.SolveWithParameters(search_parameters)
#    solution = routing.SolveWithParameters(search_parameters)

    if solution:
        return get_solution(data, manager, routing, solution, verbose)
    else:
        if verbose:
            print('There is no solution.')
    return None
