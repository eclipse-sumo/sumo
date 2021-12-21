#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateParkingAreaRerouters.py
# @author  Lara CODECA
# @author  Jakob Erdmann
# @date    11-3-2019

""" Generate parking area rerouters from the parking area definition. """

import collections
import functools
import multiprocessing
import sys
import xml.etree.ElementTree
import numpy
import sumolib

if not hasattr(functools, "lru_cache"):
    # python 2.7 fallback (lru_cache is a decorater with arguments: a function that returns a decorator)
    def lru_cache_dummy(maxsize):
        class Cache_info:
            hits = -1
            misses = -1

        def deco(fun):
            fun.cache_info = lambda: Cache_info()
            return fun
        return deco
    functools.lru_cache = lru_cache_dummy


def get_options(cmd_args=None):
    """ Argument Parser. """
    parser = sumolib.options.ArgumentParser(
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
        '--opposite-visible', action="store_true", dest='opposite_visible',
        default=False, help="ParkingArea on the opposite side of the road is always visible")
    parser.add_argument(
        '--prefer-visible', action="store_true", dest='prefer_visible',
        default=False, help="ParkingAreas which are visible are preferentially")
    parser.add_argument(
        '--min-capacity', type=int, dest='min_capacity', default=1,
        help='Do no reroute to parkingAreas with less than min-capacity')
    parser.add_argument(
        '--distribute', dest='distribute',
        help='Distribute alternatives by distance according to the given weights. "3,1"'
        + 'means that 75 percent of the alternatives are below the median distance of all'
        + 'alternatives in range and 25 percent are above the median distance')
    parser.add_argument(
        '--visible-ids', dest='visible_ids', default="",
        help='set list of parkingArea ids as always visible')
    parser.add_argument(
        '--processes', type=int, dest='processes', default=1,
        help='Number of processes spawned to compute the distance between parking areas.')
    parser.add_argument(
        '-o', '--output', type=str, dest='output', required=True,
        help='Name for the output file.')
    parser.add_argument(
        '--tqdm', dest='with_tqdm', action='store_true',
        help='Enable TQDM feature.')
    parser.set_defaults(with_tqdm=False)

    options = parser.parse_args(cmd_args)

    if options.distribute is not None:
        dists = options.distribute.split(',')
        for x in dists:
            try:
                x = float(x)
            except ValueError:
                print("Value '%s' in option --distribute must be numeric" % x,
                      file=sys.stderr)
                sys.exit()

    options.visible_ids = set(options.visible_ids.split(','))

    return options


def initRTree(all_parkings):
    try:
        import rtree  # noqa
    except ImportError:
        sys.stdout.write("Warning: Module 'rtree' not available. Using slow brute-force search for alternative parkingAreas\n")  # noqa
        return None

    result = None
    result = rtree.index.Index()
    result.interleaved = True
    # build rtree for parkingAreas
    for index, parking in enumerate(all_parkings.values()):
        x, y = parking['pos']
        r = 1
        bbox = (x - r, y - r, x + r, y + r)
        result.add(index, bbox)
    return result


class ReroutersGeneration(object):
    """ Generate parking area rerouters from the parking area definition. """

    def __init__(self, options):

        self._opt = options
        self._parking_areas = dict()
        self._sumo_rerouters = dict()

        print('Loading SUMO network: {}'.format(options.sumo_net_definition))
        self._sumo_net = sumolib.net.readNet(options.sumo_net_definition)
        for pafile in options.parking_area_definition.split(','):
            print('Loading parking file: {}'.format(pafile))
            self._load_parking_areas_from_file(pafile)

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

            laneID = child.attrib['lane']
            lane = self._sumo_net.getLane(laneID)

            endPos = lane.getLength()
            if 'endPos' in child.attrib:
                endPos = float(child.attrib['endPos'])
                if endPos < 0:
                    endPos = lane.getLength()
            else:
                child.attrib['endPos'] = endPos

            if 'startPos' not in child.attrib:
                child.attrib['startPos'] = 0

            self._parking_areas[child.attrib['id']]['edge'] = lane.getEdge().getID()
            self._parking_areas[child.attrib['id']]['pos'] = sumolib.geomhelper.positionAtShapeOffset(lane.getShape(), endPos)  # noqa
            self._parking_areas[child.attrib['id']]['capacity'] = (
                int(child.get('roadsideCapacity', 0))
                + len(child.findall('space')))

    # ---------------------------------------------------------------------------------------- #
    #                                 Rerouter Generation                                      #
    # ---------------------------------------------------------------------------------------- #

    def _generate_rerouters(self):
        """ Compute the rerouters for each parking lot for SUMO. """
        print('Computing distances and sorting parking alternatives.')
        pool = multiprocessing.Pool(processes=self._opt.processes)
        list_parameters = list()
        splits = numpy.array_split(list(self._parking_areas.keys()), self._opt.processes)
        for parkings in splits:
            parameters = {
                'selection': parkings,
                'all_parking_areas': self._parking_areas,
                'net_file': self._opt.sumo_net_definition,
                'with_tqdm': self._opt.with_tqdm,
                'num_alternatives': self._opt.num_alternatives,
                'dist_alternatives': self._opt.dist_alternatives,
                'dist_threshold': self._opt.dist_threshold,
                'capacity_threshold': self._opt.capacity_threshold,
                'min_capacity': self._opt.min_capacity,
                'opposite_visible': self._opt.opposite_visible,
                'prefer_visible': self._opt.prefer_visible,
                'distribute': self._opt.distribute,
                'visible_ids': self._opt.visible_ids,
            }
            list_parameters.append(parameters)
        for res in pool.imap_unordered(generate_rerouters_process, list_parameters):
            for key, value in res.items():
                self._sumo_rerouters[key] = value
        print('Computed {} rerouters.'.format(len(self._sumo_rerouters.keys())))

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
            <parkingAreaReroute id="{pid}" visible="{visible}"/> <!-- dist: {dist:.1f} -->"""

    def _save_rerouters(self):
        """ Save the parking lots into a SUMO XML additional file
            with threshold visibility set to True. """
        print("Creation of {}".format(self._opt.output))
        with open(self._opt.output, 'w') as outfile:
            sumolib.writeXMLHeader(outfile, "$Id$", "additional", options=self._opt)  # noqa
            # remove the randomness introduced by the multiprocessing and allows meaningful diffs
            ordered_rerouters = sorted(self._sumo_rerouters.keys())
            for rerouter_id in ordered_rerouters:
                rerouter = self._sumo_rerouters[rerouter_id]
                opposite = None
                if self._opt.opposite_visible:
                    rrEdge = self._sumo_net.getEdge(rerouter['edge'])
                    for e in rrEdge.getToNode().getOutgoing():
                        if e.getToNode() == rrEdge.getFromNode():
                            opposite = e
                            break

                alternatives = ''
                for alt, dist in rerouter['rerouters']:
                    altEdge = self._sumo_net.getEdge(self._parking_areas[alt]['edge'])
                    if altEdge == opposite:
                        opposite = None
                    _visibility = isVisible(rerouter_id, alt, dist,
                                            self._sumo_net,
                                            self._parking_areas,
                                            self._opt.dist_threshold,
                                            self._opt.capacity_threshold,
                                            self._opt.opposite_visible,
                                            self._opt.visible_ids)
                    _visibility = str(_visibility).lower()
                    alternatives += self._RR_PARKING.format(pid=alt, visible=_visibility, dist=dist)

                edges = [rerouter['edge']]
                if opposite is not None:
                    # there is no rerouter on the opposite edge but the current
                    # parkingArea should be visible from there
                    edges.append(opposite.getID())

                outfile.write(self._REROUTER.format(
                    rid=rerouter['rid'], edges=' '.join(edges), parkings=alternatives))
            outfile.write("</additional>\n")
        print("{} created.".format(self._opt.output))

    # ----------------------------------------------------------------------------------------- #


def isVisible(pID, altID, dist, net, parking_areas, dist_threshold,
              capacity_threshold, opposite_visible, visible_ids):
    if altID == pID:
        return True
    if (int(parking_areas[altID].get('roadsideCapacity', 0)) >= capacity_threshold):
        return True
    if dist <= dist_threshold:
        return True
    if opposite_visible:
        rrEdge = net.getEdge(parking_areas[pID]['edge'])
        altEdge = net.getEdge(parking_areas[altID]['edge'])
        if rrEdge.getFromNode() == altEdge.getToNode() and rrEdge.getToNode() == altEdge.getFromNode():
            return True
    if altID in visible_ids:
        return True
    return False


def generate_rerouters_process(parameters):
    """ Compute the rerouters for the given parking areas."""

    sumo_net = sumolib.net.readNet(parameters['net_file'])
    rtree = initRTree(parameters['all_parking_areas'])
    ret_rerouters = dict()

    @functools.lru_cache(maxsize=None)
    def _cached_get_shortest_path(from_edge, to_edge, fromPos, toPos):
        """ Calls and caches sumolib: net.getShortestPath. """
        return sumo_net.getShortestPath(from_edge, to_edge, fromPos=fromPos, toPos=toPos)

    distances = collections.defaultdict(dict)
    routes = collections.defaultdict(dict)
    sequence = None
    if parameters['with_tqdm']:
        from tqdm import tqdm
        sequence = tqdm(parameters['selection'])
    else:
        sequence = parameters['selection']

    distWeights = None
    distWeightSum = None
    distThresholds = None
    if parameters['distribute'] is not None:
        distWeights = list(map(float, parameters['distribute'].split(',')))
        distWeightSum = sum(distWeights)

    for parking_id in sequence:
        parking_a = parameters['all_parking_areas'][parking_id]
        from_edge = sumo_net.getEdge(parking_a['edge'])
        fromPos = float(parking_a['endPos'])
        candidates = parameters['all_parking_areas'].values()
        if rtree is not None:
            allParkings = list(candidates)
            candidates = []
            x, y = parking_a['pos']
            r = parameters['dist_alternatives']
            for i in rtree.intersection((x - r, y - r, x + r, y + r)):
                candidates.append(allParkings[i])

        for parking_b in candidates:
            if parking_a['id'] == parking_b['id']:
                continue
            toPos = float(parking_b['startPos'])
            route, cost = _cached_get_shortest_path(from_edge,
                                                    sumo_net.getEdge(parking_b['edge']),
                                                    fromPos, toPos)
            if route:
                distances[parking_a['id']][parking_b['id']] = cost
                routes[parking_a['id']][parking_b['id']] = route

    cache_info = _cached_get_shortest_path.cache_info()
    total = float(cache_info.hits + cache_info.misses)
    perc = cache_info.hits * 100.0
    if total:
        perc /= float(cache_info.hits + cache_info.misses)
    print('Cache: hits {}, misses {}, used {}%.'.format(
        cache_info.hits, cache_info.misses, perc))

    # select closest parking areas
    sequence = None
    if parameters['with_tqdm']:
        from tqdm import tqdm
        sequence = tqdm(distances.items())
    else:
        sequence = distances.items()

    for pid, dists in sequence:
        list_of_dist = [tuple(reversed(x)) for x in dists.items() if x[1] is not None]
        list_of_dist = sorted(list_of_dist)
        temp_rerouters = [(pid, 0.0)]

        numAlternatives = min(len(list_of_dist), parameters['num_alternatives'])

        used = set()
        if parameters['prefer_visible']:
            for distance, parking in list_of_dist:
                if parameters['all_parking_areas'][parking].get('capacity') < parameters['min_capacity']:
                    continue
                if len(temp_rerouters) > parameters['num_alternatives']:
                    break
                if isVisible(pid, parking, distance, sumo_net,
                             parameters['all_parking_areas'],
                             parameters['dist_threshold'],
                             parameters['capacity_threshold'],
                             parameters['opposite_visible'],
                             parameters['visible_ids']):
                    temp_rerouters.append((parking, distance))
                    used.add(parking)

        dist = None
        distThresholds = None
        if distWeights is not None:
            dist = [int(x / distWeightSum * numAlternatives) for x in distWeights]
            distThresholdIndex = [int(i * len(list_of_dist) / len(dist)) for i in range(len(dist))]
            distThresholds = [list_of_dist[i][0] for i in distThresholdIndex]
            # print("distWeights=%s" % distWeights)
            # print("dist=%s" % dist)
            # print("distances=%s" % [x[0] for x in list_of_dist])
            # print("distThresholdIndex=%s" % distThresholdIndex)
            # print("distThresholds=%s" % distThresholds)

        distIndex = 0
        found = 0
        required = dist[distIndex] if dist else None
        for distance, parking in list_of_dist:
            route = routes[pid][parking]
            if parking in used:
                found += 1
                continue

            if parameters['all_parking_areas'][parking].get('capacity') < parameters['min_capacity']:
                continue
            # optionally enforce distance distribution
            # if dist is not None:
            #     print("found=%s required=%s distIndex=%s threshold=%s" % (found, required,
            #         distIndex, distThresholds[distIndex]))

            if dist is not None and found >= required:
                if distIndex + 1 < len(dist):
                    distIndex += 1
                    required += dist[distIndex]
            if distThresholds is not None and distance < distThresholds[distIndex]:
                continue

            if len(temp_rerouters) > parameters['num_alternatives']:
                break
            if distance > parameters['dist_alternatives']:
                break
            found += 1
            temp_rerouters.append((parking, distance))

        if not list_of_dist:
            print('Parking {} has 0 neighbours!'.format(pid))

        ret_rerouters[pid] = {
            'rid': pid,
            'edge': parameters['all_parking_areas'][pid]['edge'],
            'rerouters': temp_rerouters,
        }
    return ret_rerouters


def main(cmd_args):
    """ Generate parking area rerouters from the parking area definition. """
    args = get_options(cmd_args)
    ReroutersGeneration(args)
    print('Done.')


if __name__ == "__main__":
    main(sys.argv[1:])
