#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    generateParkingAreaRerouters.py
# @author  Lara CODECA
# @date    11-3-2019
# @version $Id$

""" Generate parking area rerouters from the parking area definition. """

import argparse
import collections
import logging
import sys
import xml.etree.ElementTree

import sumolib
import traci


def logs():
    """ Log init. """
    stdout_handler = logging.StreamHandler(sys.stdout)
    logging.basicConfig(handlers=[stdout_handler], level=logging.WARNING,
                        format='[%(asctime)s] %(levelname)s: %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p')


def get_options(cmd_args=None):
    """ Argument Parser. """
    parser = argparse.ArgumentParser(
        prog='generateParkingAreaRerouters.py', usage='%(prog)s [options]',
        description='Generate parking area rerouters from the parking area definition.')
    parser.add_argument(
        '-a', '--parking-areas', type=str, dest='parking_area_definition', required=True,
        help='SUMO parkingArea definition.')
    parser.add_argument(
        '-n', '--sumo-net', type=str, dest='sumo_net_definition', required=True,
        help='SUMO network definition.')
    parser.add_argument(
        '--max-number-alternatives', type=int, dest='num_alternatives', required=True,
        help='Rerouter: max number of alternatives.')
    parser.add_argument(
        '--max-distance-alternatives', type=float, dest='dist_alternatives', required=True,
        help='Rerouter: max distance for the alternatives.')
    parser.add_argument(
        '--min-capacity-visibility-true', type=int, dest='capacity_threshold', required=True,
        help='Rerouter: parking capacity for the visibility threshold.')
    parser.add_argument(
        '--max-distance-visibility-true', type=float, dest='dist_threshold', required=True,
        help='Rerouter: parking distance for the visibility threshold.')
    parser.add_argument(
        '-o', type=str, dest='output', required=True,
        help='Name for the output file.')
    return parser.parse_args(cmd_args)


class ReroutersGeneration(object):
    """ Generate parking area rerouters from the parking area definition. """

    _parking_areas = dict()
    _sumo_net = None

    _num_alternatives = None
    _dist_alternatives = None
    _capacity_threshold = None
    _dist_threshold = None

    _sumo_rerouters = dict()

    def __init__(self, parking_areas, sumo_network, num_alternatives, dist_alternatives,
                 capacity_threshold, dist_threshold):

        self._load_parking_areas_from_file(parking_areas)
        self._sumo_net = sumo_network
        self._num_alternatives = num_alternatives
        self._dist_alternatives = dist_alternatives
        self._capacity_threshold = capacity_threshold
        self._dist_threshold = dist_threshold

        self._generate_rerouters()

    def _load_parking_areas_from_file(self, filename):
        """ Load parkingArea from XML file. """
        xml_tree = xml.etree.ElementTree.parse(filename).getroot()
        for child in xml_tree:
            self._parking_areas[child.attrib['id']] = child.attrib
            self._parking_areas[child.attrib['id']]['edge'] = child.attrib['lane'].split('_')[0]

    # ---------------------------------------------------------------------------------------- #
    #                                 Rerouter Generation                                      #
    # ---------------------------------------------------------------------------------------- #

    def _generate_rerouters(self):
        """ Compute the rerouters for each parking lot for SUMO. """

        traci.start([sumolib.checkBinary('sumo'), '--no-step-log', '-n', self._sumo_net])

        distances = collections.defaultdict(dict)
        for parking_a in self._parking_areas.values():
            for parking_b in self._parking_areas.values():
                if parking_a['id'] == parking_b['id']:
                    continue
                if parking_a['edge'] == parking_b['edge']:
                    continue

                route = None
                try:
                    route = traci.simulation.findRoute(
                        parking_a['edge'], parking_b['edge'])
                except traci.exceptions.TraCIException:
                    route = None

                cost = None
                if route and route.edges:
                    cost = route.travelTime
                else:
                    cost = None

                distances[parking_a['id']][parking_b['id']] = cost

        traci.close()

        # select closest parking areas
        for pid, dists in distances.items():
            list_of_dist = [tuple(reversed(x)) for x in dists.items() if x[1] is not None]
            list_of_dist = sorted(list_of_dist)
            rerouters = [(pid, 0.0)]
            for distance, parking in list_of_dist:
                if len(rerouters) > self._num_alternatives:
                    break
                if distance > self._dist_alternatives:
                    break
                rerouters.append((parking, distance))

            if not list_of_dist:
                logging.fatal('Parking %s has 0 neighbours!', pid)

            self._sumo_rerouters[pid] = {
                'rid': pid,
                'edge': self._parking_areas[pid]['edge'],
                'rerouters': rerouters,
            }

        logging.debug('Computed %d rerouters.', len(self._sumo_rerouters.keys()))

    # ---------------------------------------------------------------------------------------- #
    #                             Save SUMO Additionals to File                                #
    # ---------------------------------------------------------------------------------------- #

    _REROUTER = """
    <rerouter id="{rid}" edges="{edges}">
        <interval begin="0.0" end="86400">
            <!-- in order of distance --> {parkings}
        </interval>
    </rerouter>
"""

    _RR_PARKING = """
            <parkingAreaReroute id="{pid}" visible="{visible}"/> <!-- dist: {dist} -->"""

    def save_rerouters(self, filename):
        """ Save the parking lots into a SUMO XML additional file
            with threshold visibility set to True. """
        logging.info("Creation of %s", filename)
        with open(filename, 'w') as outfile:
            sumolib.xml.writeHeader(outfile, "additional")
            outfile.write("<additional>\n")
            for rerouter in self._sumo_rerouters.values():
                alternatives = ''
                for alt, dist in rerouter['rerouters']:
                    _visibility = 'false'
                    if alt == rerouter['rid']:
                        _visibility = 'true'
                    if (int(self._parking_areas[alt].get('roadsideCapacity', 0)) >=
                            self._capacity_threshold):
                        _visibility = 'true'
                    if dist <= self._dist_threshold:
                        _visibility = 'true'
                    alternatives += self._RR_PARKING.format(pid=alt, visible=_visibility, dist=dist)
                outfile.write(self._REROUTER.format(
                    rid=rerouter['rid'], edges=rerouter['edge'], parkings=alternatives))
            outfile.write("</additional>\n")
        logging.info("%s created.", filename)

    # ----------------------------------------------------------------------------------------- #


def main(cmd_args):
    """ Generate parking area rerouters from the parking area definition. """
    args = get_options(cmd_args)

    rerouters = ReroutersGeneration(args.parking_area_definition, args.sumo_net_definition,
                                    args.num_alternatives, args.dist_alternatives,
                                    args.capacity_threshold, args.dist_threshold)
    rerouters.save_rerouters(args.output)

    logging.info('Done.')


if __name__ == "__main__":
    logs()
    main(sys.argv)
