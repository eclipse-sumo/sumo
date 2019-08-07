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
# @version $Id$  # noqa

""" Generate parking area rerouters from the parking area definition. """

import argparse
import collections
import functools
import logging
import multiprocessing
import numpy
import sys
import xml.etree.ElementTree
import sumolib

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
        '--max-number-alternatives', type=int, dest='num_alternatives', default=10,
        help='Rerouter: max number of alternatives.')
    parser.add_argument(
        '--max-distance-alternatives', type=float, dest='dist_alternatives', default=500.0,
        help='Rerouter: max distance for the alternatives.')
    parser.add_argument(
        '--min-capacity-visibility-true', type=int, dest='capacity_threshold', default=25,
        help='Rerouter: parking capacity for the visibility threshold.')
    parser.add_argument(
        '--max-distance-visibility-true', type=float, dest='dist_threshold', default=250.0,
        help='Rerouter: parking distance for the visibility threshold.')
    parser.add_argument(
        '--processes', type=int, dest='processes', default=1,
        help='Number of processes spawned to compute the distance between parking areas.')
    parser.add_argument(
        '-o', type=str, dest='output', required=True,
        help='Name for the output file.')
    parser.add_argument(
        '--tqdm', dest='with_tqdm', action='store_true',
        help='Enable TQDM feature.')
    parser.set_defaults(with_tqdm=False)
    return parser.parse_args(cmd_args)

class ReroutersGeneration(object):
    """ Generate parking area rerouters from the parking area definition. """

    _opt = None

    _sumo_net = None
    _parking_areas = dict()
    _sumo_rerouters = dict()

    def __init__(self, options):

        self._opt = options

        logging.info('Loading SUMO network: %s', options.sumo_net_definition)
        self._sumo_net = sumolib.net.readNet(options.sumo_net_definition)
        logging.info('Loading parking file: %s', options.parking_area_definition)
        self._load_parking_areas_from_file(options.parking_area_definition)

        self._generate_rerouters()
        self._save_rerouters()

    def _load_parking_areas_from_file(self, filename):
        """ Load parkingArea from XML file. """
        xml_tree = xml.etree.ElementTree.parse(filename).getroot()
        sequence = None
        if self._opt.with_tqdm:
            from tqdm import tqdm
            sequence = tqdm(xml_tree)
        else:
            sequence = xml_tree
        for child in sequence:
            self._parking_areas[child.attrib['id']] = child.attrib
            self._parking_areas[child.attrib['id']]['edge'] = self._sumo_net.getEdge(
                child.attrib['lane'].split('_')[0])

    # ---------------------------------------------------------------------------------------- #
    #                                 Rerouter Generation                                      #
    # ---------------------------------------------------------------------------------------- #

    @functools.lru_cache(maxsize=None)
    def _cached_get_shortest_path(self, from_edge, to_edge):
        """ Calls and caches sumolib: net.getShortestPath. """
        return self._sumo_net.getShortestPath(from_edge, to_edge)

    def _generate_rerouters(self):
        """ Compute the rerouters for each parking lot for SUMO. """
        logging.info('Computing distances and sorting parking alternatives.')
        splits = numpy.array_split(list(self._parking_areas.values()), self._opt.processes)
        manager = multiprocessing.Manager()
        return_dict = manager.dict()
        processes = list()
        for split in splits:
            _sorting_process = multiprocessing.Process(target=self._generate_rerouters_process,
                                                       args=(split, return_dict,))
            processes.append(_sorting_process)
            _sorting_process.start()
        for sorting_process in processes:
            sorting_process.join()
        self._sumo_rerouters = return_dict
        logging.info('Computed %d rerouters.', len(self._sumo_rerouters.keys()))

    def _generate_rerouters_process(self, parking_areas, rerouters):
        """ Compute the rerouters for the given parking areas."""
        distances = collections.defaultdict(dict)
        sequence = None
        if self._opt.with_tqdm:
            from tqdm import tqdm
            sequence = tqdm(parking_areas)
        else:
            sequence = parking_areas
        for parking_a in sequence:
            for parking_b in self._parking_areas.values():
                if parking_a['id'] == parking_b['id']:
                    continue
                if parking_a['edge'].getID() == parking_b['edge'].getID():
                    continue
                route, cost = self._cached_get_shortest_path(parking_a['edge'], parking_b['edge'])
                if route:
                    distances[parking_a['id']][parking_b['id']] = cost
        cache_info = self._cached_get_shortest_path.cache_info()
        used = cache_info.hits * 100.0 / float(cache_info.hits + cache_info.misses)
        logging.info('Cache: hits %d, misses %d, used %.2f%%.', 
                     cache_info.hits, cache_info.misses, used)

        # select closest parking areas
        sequence = None
        if self._opt.with_tqdm:
            from tqdm import tqdm
            sequence = tqdm(distances.items())
        else:
            sequence = distances.items()
        for pid, dists in sequence:
            list_of_dist = [tuple(reversed(x)) for x in dists.items() if x[1] is not None]
            list_of_dist = sorted(list_of_dist)
            temp_rerouters = [(pid, 0.0)]
            for distance, parking in list_of_dist:
                if len(temp_rerouters) > self._opt.num_alternatives:
                    break
                if distance > self._opt.dist_alternatives:
                    break
                temp_rerouters.append((parking, distance))

            if not list_of_dist:
                logging.fatal('Parking %s has 0 neighbours!', pid)

            rerouters[pid] = {
                'rid': pid,
                'edge': self._parking_areas[pid]['edge'].getID(),
                'rerouters': temp_rerouters,
            }

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

    def _save_rerouters(self):
        """ Save the parking lots into a SUMO XML additional file
            with threshold visibility set to True. """
        logging.info("Creation of %s", self._opt.output)
        with open(self._opt.output, 'w') as outfile:
            sumolib.xml.writeHeader(outfile, "additional")
            outfile.write("<additional>\n")
            for rerouter in self._sumo_rerouters.values():
                alternatives = ''
                for alt, dist in rerouter['rerouters']:
                    _visibility = 'false'
                    if alt == rerouter['rid']:
                        _visibility = 'true'
                    if (int(self._parking_areas[alt].get('roadsideCapacity', 0)) >=
                            self._opt.capacity_threshold):
                        _visibility = 'true'
                    if dist <= self._opt.dist_threshold:
                        _visibility = 'true'
                    alternatives += self._RR_PARKING.format(pid=alt, visible=_visibility, dist=dist)
                outfile.write(self._REROUTER.format(
                    rid=rerouter['rid'], edges=rerouter['edge'], parkings=alternatives))
            outfile.write("</additional>\n")
        logging.info("%s created.", self._opt.output)

    # ----------------------------------------------------------------------------------------- #


def main(cmd_args):
    """ Generate parking area rerouters from the parking area definition. """
    args = get_options(cmd_args)
    _ = ReroutersGeneration(args)
    logging.info('Done.')


if __name__ == "__main__":
    logs()
    main(sys.argv[1:])
