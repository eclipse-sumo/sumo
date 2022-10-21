# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2021-2022 German Aerospace Center (DLR) and others.
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
        print('Objective: ', solution.ObjectiveValue())
    solution_dict = {}
    total_cost = 0
    for vehicle_id in range(data['num_vehicles']):
        route = []
        index = routing.Start(vehicle_id)
        if verbose:
            plan_output = 'Route for vehicle %s:\n' % vehicle_id
        route_cost = 0
        route_load = 0
        while not routing.IsEnd(index):
            current_node = manager.IndexToNode(index)
            route_load += data['demands'][current_node]
            if verbose:
                plan_output += ' %s (L: %s, C: %s) -> ' % (current_node, route_load, route_cost)
            route.append(current_node)
            previous_index = index
            index = solution.Value(routing.NextVar(index))
            route_cost += routing.GetArcCostForVehicle(previous_index, index, vehicle_id)
        last_node = manager.IndexToNode(index)
        route_load += data['demands'][last_node]
        route.append(last_node)
        if verbose:
            plan_output += '%s (L: %s, C: %s)\n' % (last_node, route_load, route_cost)
            plan_output += 'Costs of the route: %s\n' % route_cost
            print(plan_output)
        total_cost += route_cost
        solution_dict[vehicle_id] = [route, total_cost]
    if verbose:
        print('Total cost of the routes: %s' % total_cost)
    return solution_dict


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
    distance_dimension.SetGlobalSpanCostCoefficient(100)

    # Define Transportation Requests.
    if verbose:
        print(' Add pickup and delivery constraints...')
    for request in data['pickups_deliveries']:
        pickup_index = manager.NodeToIndex(request.from_node)
        delivery_index = manager.NodeToIndex(request.to_node)
        routing.AddPickupAndDelivery(pickup_index, delivery_index)  # helps the solver
<<<<<<< HEAD
        # use same veh for pickup and dropoff
        routing.solver().Add(routing.VehicleVar(pickup_index) == routing.VehicleVar(delivery_index))
        routing.solver().Add(
            distance_dimension.CumulVar(pickup_index) <=
            distance_dimension.CumulVar(delivery_index))  # define order: first pickup then dropoff
        if request[2]:  # is that a new request?
            # allows to reject the order but gives penalty
            routing.AddDisjunction([pickup_index, delivery_index], 100_000, 2)
=======
        solver.Add(
            routing.VehicleVar(pickup_index) == routing.VehicleVar(delivery_index))  # use same veh for pickup and dropoff
        solver.Add(
            distance_dimension.CumulVar(pickup_index) <=
            distance_dimension.CumulVar(delivery_index))  # define order: first pickup then dropoff
        if hasattr(request, 'is_new') and request.is_new==True:  # is that a new request?
            routing.AddDisjunction([pickup_index, delivery_index], 100_000, 2)  # allows to reject the order but gives penalty
>>>>>>> 05cbc7934bb (add direct route factor, ref. #11429)

    # Set direct route factor.
    if data['drf'] != -1:
        if verbose:
            print(' Add direct route factor constraints...')        
        for request in data['pickups_deliveries']: # if the costs changed and cannot hold anymore? --> see 'dropoffs'
            pickup_index = manager.NodeToIndex(request.from_node)
            delivery_index = manager.NodeToIndex(request.to_node)
            # let the route cost be less or equal the direct route cost times drf
            direct_route_cost = request.direct_route_cost
            solver.Add(
                distance_dimension.CumulVar(delivery_index) - distance_dimension.CumulVar(pickup_index) <=  # route cost
                solver.IntConst(round(direct_route_cost * data['drf'])))  # direct route cost times direct route factor
            route_cost = distance_dimension.CumulVar(delivery_index) - distance_dimension.CumulVar(pickup_index)
            direct_route_cost_drf = solver.IntConst(round(direct_route_cost * data['drf']))
        # if possible, let the route costs of the dropoffs less than the drf allows,
        # else minimize the route costs (in case the costs become larger than expected)
        for request in data['dropoffs']:  # TODO: not sure that it works, test needed!
            direct_route_cost_drf = solver.IntConst(round(direct_route_cost * data['drf']))
            delivery_index = manager.NodeToIndex(request.to_node)
            route_cost = ( distance_dimension.CumulVar(delivery_index) +  # cost from veh start to dropoff node
                solver.IntConst(round(request.current_route_cost)) )  # cost from pickup to veh start
            #objective_to_minimize = solver.Minimize(solver.Max(route_cost, direct_route_cost_drf), 10)
            routing.AddVariableMinimizedByFinalizer(solver.Max(route_cost, direct_route_cost_drf).Var())
        


    # Force the vehicle to drop-off the reservations it already picked up
    if verbose:
        print(' Add dropoff constraints...')
    #for veh_index, do_list in enumerate(data['dropoffs']):
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
<<<<<<< HEAD
            print('vehicle %s with %s dropoffs' % (veh_index, len(do_list)))
        for do in do_list:
            index = manager.NodeToIndex(do[0])
            # start node
            veh_node = data['starts'][veh_index]
            if verbose:
                print('vehicle %s (%s), dropoff %s (%s), res_id %s' % (veh_index, veh_node, do[0], index, do[1]))
            # routing.VehicleVar(index).SetValues([-1,veh_index])
            routing.SetAllowedVehiclesForIndex([veh_index], index)
=======
            print('reservation %s in veh %s (%s)' % (res.id, res.vehicle, res.vehicle_index))
        index = manager.NodeToIndex(res.to_node)
        routing.SetAllowedVehiclesForIndex([res.vehicle_index],index)

>>>>>>> 05cbc7934bb (add direct route factor, ref. #11429)

    # Add Capacity constraint.
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

    # Setting first solution heuristic.
    if verbose:
        print(' Set solution heuristic...')
    search_parameters = pywrapcp.DefaultRoutingSearchParameters()
    search_parameters.first_solution_strategy = (routing_enums_pb2.FirstSolutionStrategy.PATH_MOST_CONSTRAINED_ARC)
    search_parameters.time_limit.FromSeconds(time_limit_seconds)

    # Solve the problem.
    if verbose:
        print('Start solving the problem.')
    solution = routing.SolveWithParameters(search_parameters)

    if solution:
        return get_solution(data, manager, routing, solution, verbose)
    else:
        if verbose:
            print('There is no solution.')
    return None
