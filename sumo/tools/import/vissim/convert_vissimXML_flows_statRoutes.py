#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@file    convert_vissimXML_flows_statRoutes.py
@author  Lukas Grohmann <Lukas.Grohmann@ait.ac.at>
@date    Jun 09 2015
@version $Id$

Parses flows and static routes from a VISSIM .inpx file
and writes converted information to a given .rou.xml file.
see documentation (TODO ref.)

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
# TODO: usage doc.ref
# FIXME: better split in 2 ?

import argparse
from xml.dom import minidom
import numpy as np
from xml.dom.minidom import Document


def dict_from_node_attributes(node):
    """takes a xml node and returns a dictionary with its attributes"""
    return dict((attn, node.getAttribute(attn)) for attn in
                node.attributes.keys())


# FUNKTIONEN
def parse_flows(xmldoc):
    """parses the vehicle inputs from the VISSIM data"""
    flow_d = dict()
    for v_input in xmldoc.getElementsByTagName('vehicleInput'):
        v_input_d = dict_from_node_attributes(v_input)
        v_input_d["vehComp"] = []
        v_input_d["volType"] = []
        v_input_d["flow"] = []
        for volume in v_input.getElementsByTagName('timeIntervalVehVolume'):
            v_input_d["vehComp"].append(volume.getAttribute('vehComp'))
            v_input_d["volType"].append(volume.getAttribute('volType'))
            v_input_d["flow"].append(
                [float(volume.getAttribute('timeInt').split(" ")[1]) / 1000,
                 float(volume.getAttribute('volume')),
                 float(volume.getAttribute('vehComp'))])
        v_input_d["flow"] = np.array(v_input_d["flow"])
        flow_d[v_input_d["link"]] = v_input_d
    return flow_d


def parse_max_acc(xmldoc):
    """parses the vehicle acceleration distributions from the VISSIM data"""
    acc_d = dict()
    for max_acc in xmldoc.getElementsByTagName('maxAccelerationFunction'):
        acc_d[max_acc.getAttribute('no')] = max_acc.getElementsByTagName(
            'accelerationFunctionDataPoint')[0].getAttribute('y')
    return acc_d


def parse_max_speed(xmldoc):
    """parses the vehicle speed distribution from the VISSIM data"""
    speed_d = dict()
    for max_speed in xmldoc.getElementsByTagName('desSpeedDistribution'):
        # get mean speed
        num = 0.
        sum_val = 0.
        data_points = max_speed.getElementsByTagName(
            'speedDistributionDataPoint')
        for point in data_points:
            num += 1
            sum_val += float(point.getAttribute('x'))
        speed_d[max_speed.getAttribute('no')] = str((sum_val / num) / 3.6)
    return speed_d


def parse_length(xmldoc):
    """parses the vehicle length from the VISSIM data"""
    len_d = dict()
    model_d = dict()
    # get model data
    for model in xmldoc.getElementsByTagName('model2D3D'):
        model_d[model.getAttribute('no')] = model.getElementsByTagName(
            'model2D3DSegment')[0].getAttribute('length')
    # calculate length data
    for model_dist in xmldoc.getElementsByTagName('model2D3DDistribution'):
        elements = model_dist.getElementsByTagName(
            'model2D3DDistributionElement')
        length = 0
        total_probability = 0
        for element in elements:
            total_probability += float(element.getAttribute('share'))
        for element in elements:
            length += (
                float(element.getAttribute('share')) / total_probability) * \
                float(model_d[element.getAttribute('model2D3D')])
        len_d[model_dist.getAttribute('no')] = str(length)
    return len_d


def parse_veh_comp(xmldoc):
    """parses the vehicle composition from the VISSIM data"""
    veh_comp_d = dict()
    for vehicle_comp in xmldoc.getElementsByTagName('vehicleComposition'):
        rel_flows = vehicle_comp.getElementsByTagName(
            'vehicleCompositionRelativeFlow')
        flow_l = []
        for flow in rel_flows:
            flow_d = {}
            flow_d['desSpeedDistr'] = flow.getAttribute('desSpeedDistr')
            flow_d['rel_flow'] = flow.getAttribute('relFlow')
            flow_d['vehType'] = flow.getAttribute('vehType')
            flow_l.append(flow_d)
        # list of dictionaries
        veh_comp_d[vehicle_comp.getAttribute('no')] = flow_l
    return veh_comp_d


def parse_vehicle_types(xmldoc, acc_d, length_d):
    """parses the vehicle types from the VISSIM data"""
    veh_type_d = dict()
    for veh_type in xmldoc.getElementsByTagName('vehicleType'):
        type_d = {}
        type_d['id'] = veh_type.getAttribute('no')
        type_d['length'] = length_d[veh_type.getAttribute('model2D3DDistr')]
        type_d['acc'] = acc_d[veh_type.getAttribute('maxAccelFunc')]
        veh_type_d[veh_type.getAttribute('no')] = type_d
    return veh_type_d


# FIXME: not necessarily nicely done
#	name doesn't fit functionality
def is_verbinder(xmldoc):
    """checks if a given link is a verbinder"""
    # simple implementation of static variable
    # if not hasattr(is_verbinder, "v_dic"):
    # is_verbinder.v_dic = dict()  # doesn't exist yet, so initialize
    # FIXME: xmldoc is in the way
    is_verbinder_d = dict()
    for link in xmldoc.getElementsByTagName("link"):
        if len(link.getElementsByTagName("fromLinkEndPt")) > 0:
            is_verbinder_d[link.getAttribute("no")] = True
        else:
            is_verbinder_d[link.getAttribute("no")] = False
    # returning a dict...
    return is_verbinder_d


# FIXME: just for static routes
def parse_routes(xmldoc, edge_list, is_verbinder_d):
    """parses the VISSIM .inpx route information
    of statically defined routes ONLY"""
    routes_by_start_d = dict()     # dictionary[start_link] = list(<Route>)
    for decision in xmldoc.getElementsByTagName('vehicleRoutingDecisionStatic'):
        start_link = decision.getAttribute('link')
        routes_by_start_d[start_link] = []
        for statistic in decision.getElementsByTagName('vehicleRouteStatic'):
            route_d = {}
            route_d["start_link"] = start_link
            route_d["dest_link"] = statistic.getAttribute('destLink')
            route_d["r_id"] = statistic.getAttribute('no')
            i = 1
            route_d["rel_flow"] = []
            temp = statistic.getAttribute('relFlow').split(" ")
            while i < len(temp):
                var = temp[i].split(":")
                var[0] = float(var[0]) / 1000
                var[1] = float(var[1].replace(",", ""))
                route_d["rel_flow"].append(var)
                i = i + 2
            route_d["rel_flow"] = np.array(route_d["rel_flow"])
            route_d["links"] = [route_d["start_link"]]
            for link in statistic.getElementsByTagName('intObjectRef'):
                # exclude VISSIM connectors (usually id > 10k)
                if not is_verbinder_d[link.getAttribute('key')]:
                    if link.getAttribute('key') in edge_list:
                        route_d["links"].append(link.getAttribute('key'))
                    else:
                        split_edge_list = [
                            e for e in edge_list if e[-1] == ']']
                        check = 1
                        i = 0
                        while check == 1:
                            if link.getAttribute(
                                'key') + "[" + str(i) + "]" \
                                    in split_edge_list:
                                route_d["links"].append(
                                    link.getAttribute('key') +
                                    "[" + str(i) + "]")
                                i = i + 1
                            else:
                                check = 0

            route_d["links"].append(route_d["dest_link"])
            # add route object to dictionary
            routes_by_start_d[route_d["start_link"]].append(route_d)
    return routes_by_start_d


def set_probability(routes_by_start_d, flow_d):
    """computes the route probabilies"""
    for start_link in routes_by_start_d.keys():
        absolute_flow = flow_d[start_link]["flow"][:, 1]
        cumulated_flow = np.zeros_like(absolute_flow)
        veh_comp = flow_d[start_link]["vehComp"]
        for route in routes_by_start_d[start_link]:
            cumulated_flow = cumulated_flow + route["rel_flow"][:, 1]
            # set the vehicle type for each route
            route["type"] = veh_comp
        for route in routes_by_start_d[start_link]:
            route["probability"] = np.zeros_like(absolute_flow)
            zero_comp_flow_sel = cumulated_flow == 0.
            route["probability"][~zero_comp_flow_sel] = \
                (route["rel_flow"][~zero_comp_flow_sel, 1]
                 / cumulated_flow[~zero_comp_flow_sel])

# VISSIM BUG!!: Relative Zuflüsse mit dem Wert 1.0 gehen bei der
# Konversion von .inp zu .inpx verloren


def validate_rel_flow(routes_by_start_d):
    """checks if a relative flow is missing and completes it if necessary"""
    # compare alle rel_flows with the reference flow
    for start_link in routes_by_start_d.keys():
        reference_time = flow_d[start_link]["flow"][:, 0]
        for route in routes_by_start_d[start_link]:
            if np.array_equal(reference_time,
                              route["rel_flow"][:, 0]) == False:
                i = 0
                while i < len(route["rel_flow"]):
                    if reference_time[i] != route["rel_flow"][i, 0]:
                        # add missing time interval with relative flow 1.0
                        route["rel_flow"] = np.insert(
                            route["rel_flow"], i, np.array(
                                (reference_time[i], 1.0)), 0)
                    i = i + 1


def set_v_types(veh_comp_d, route_doc, root, speed_d):
    """writes the vehicle distribution data into the given dom document"""
    for c_id, comps in veh_comp_d.items():
        v_type_dist = route_doc.createElement("vTypeDistribution")
        v_type_dist.setAttribute("id", c_id)
        root.appendChild(v_type_dist)
        for comp in comps:
            v_type = route_doc.createElement("vType")
            v_type.setAttribute(
                "id",
                "t{}_D{}".format(
                    veh_type_d[comp["vehType"]]["id"],
                    c_id))
            v_type.setAttribute("accel", veh_type_d[comp["vehType"]]["acc"])
            v_type.setAttribute("length",
                                veh_type_d[comp["vehType"]]["length"])
            v_type.setAttribute("probability", comp["rel_flow"])
            v_type.setAttribute("maxSpeed", speed_d[comp["desSpeedDistr"]])
            v_type_dist.appendChild(v_type)
    return route_doc


def set_route_distributions(route_doc, routes_by_start_d, root):
    """writes the route distribution data into the given dom document"""
    for start_id in routes_by_start_d:
        i = 0
        if len(routes_by_start_d[start_id]) > 0:
            ref_time = flow_d[start_id]["flow"][:, 0]
            for time in ref_time:
                route_dist = route_doc.createElement("routeDistribution")
                route_dist.setAttribute("id", "_".join([start_id,
                                                        str(time)]))
                root.appendChild(route_dist)
                for route in routes_by_start_d[start_id]:
                    if np.abs(route["probability"][i]) != 0:
                        route_node = route_doc.createElement("route")
                        route_node.setAttribute("id", route["r_id"])
                        route_node.setAttribute("edges",
                                                " ".join(route["links"]))
                        route_node.setAttribute("probability",
                                                str(np.abs(
                                                    route["probability"][i])))
                        route_dist.appendChild(route_node)
                if not route_dist.hasChildNodes():
                    root.removeChild(route_dist)
                i += 1
    return route_doc


def set_flows(routes_by_start_d, flow_d, route_doc, root):
    """writes the flow data into the given dom document"""
    sim_end = XMLDOC.getElementsByTagName(
        "simulation")[0].getAttribute("simPeriod")
    dom_flow_l = []
    for start_id in routes_by_start_d:
        ref_time = flow_d[start_id]["flow"][:, 0]
        for index, time in enumerate(ref_time):
            if len(routes_by_start_d[start_id]) > 0:
                in_flow = [fl for fl in flow_d[start_id]["flow"] if
                           fl[0] == time][0]
                if in_flow[1] > 0:
                    flow = route_doc.createElement("flow")
                    flow.setAttribute("id", "fl{}_st{}".format(start_id,
                                                               time))
                    flow.setAttribute("color", "1,1,0")
                    flow.setAttribute("begin", str(time))
                    if index < len(ref_time) - 1 and len(ref_time) > 1:
                        flow.setAttribute("end",
                                          str(time + ref_time[index + 1]))
                    else:
                        flow.setAttribute("end", sim_end)
                    flow.setAttribute("vehsPerHour", str(in_flow[1]))
                    flow.setAttribute("type", str(int(in_flow[2])))
                    flow.setAttribute('route', "_".join([start_id,
                                                         str(time)]))
                    dom_flow_l.append(flow)
    dom_flow_l = sorted(dom_flow_l,
                        key=lambda dom: float(dom.getAttribute("begin")))
    for dom_obj in dom_flow_l:
        root.appendChild(dom_obj)
    return route_doc


# MAIN
if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='road network conversion utility '
        ' (VISSIM.inpx to SUMO)')
    parser.add_argument('--vissim-input', '-V', type=str,
                        help='VISSIM inpx file path')
    parser.add_argument('--SUMO-net', '-S', type=str,
                        help='SUMO net file path')
    parser.add_argument('--output-file', '-o', type=str,
                        help='output file name')
    args = parser.parse_args()
    print("\n", args, "\n")
    print('\n---\n\n* loading VISSIM net:\n\t', args.vissim_input)
    XMLDOC = minidom.parse(args.vissim_input)
    print('\n---\n\n* loading SUMO net:\n\t', args.SUMO_net,)
    SUMODOC = minidom.parse(args.SUMO_net)

    edge_list = []
    ed_list = SUMODOC.getElementsByTagName('edge')
    for edge in ed_list:
        # is it a normal edge ?
        if not edge.hasAttribute("function"):
            edge_list.append(edge.getAttribute("id"))

    # to check if a link is a verbinder
    is_verbinder_d = is_verbinder(XMLDOC)

    #
    # Vehicle Types #########
    #

    # parse vehicle type data
    speed_d = parse_max_speed(XMLDOC)
    # get the vehicle distribution
    veh_comp_d = parse_veh_comp(XMLDOC)
    # parse vehTypes and combine the information with the acceleration and
    # length data
    veh_type_d = parse_vehicle_types(XMLDOC,
                                     parse_max_acc(XMLDOC),
                                     parse_length(XMLDOC))

    #
    # Flows and Routes ########
    #

    # TODO: can we make flows and routes conversion switchable by option ?
    # parse flows
    flow_d = parse_flows(XMLDOC)
    # parse routes
    routes_by_start_d = parse_routes(XMLDOC, edge_list, is_verbinder_d)
    # validate relative flows
    validate_rel_flow(routes_by_start_d)
    # computes the probability for each route
    set_probability(routes_by_start_d, flow_d)

    #
    # XML Handling ##########
    #

    # create dom document and define routes + flows
    result_doc = Document()
    root = result_doc.createElement("routes")
    result_doc.appendChild(root)

    result_doc = set_v_types(veh_comp_d, result_doc, root, speed_d)
    result_doc = set_route_distributions(result_doc, routes_by_start_d, root)
    result_doc = set_flows(routes_by_start_d, flow_d, result_doc, root)
    # write the data into a .rou.xml file
    with open("%s.rou.xml" % args.output_file, "w") as ofh:
        result_doc.writexml(ofh, addindent='    ', newl='\n')
        ofh.close()
