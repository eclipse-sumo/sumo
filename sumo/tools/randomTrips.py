#!/usr/bin/env python
"""
@file    randomTrips.py
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2010-03-06
@version $Id$

Generates random trips for the given network.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import random
import bisect
import datetime
import subprocess
from collections import defaultdict
import math
import optparse

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib
import route2trips

DUAROUTER = sumolib.checkBinary('duarouter')

SOURCE_SUFFIX = ".src.xml"
SINK_SUFFIX = ".dst.xml"
VIA_SUFFIX = ".via.xml"


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-n", "--net-file", dest="netfile",
                         help="define the net file (mandatory)")
    optParser.add_option("-a", "--additional-files", dest="additional",
                         help="define additional files to be loaded by the router")
    optParser.add_option("-o", "--output-trip-file", dest="tripfile",
                         default="trips.trips.xml", help="define the output trip filename")
    optParser.add_option("-r", "--route-file", dest="routefile",
                         help="generates route file with duarouter")
    optParser.add_option("--weights-prefix", dest="weightsprefix",
                         help="loads probabilities for being source, destination and via-edge from the files named <prefix>.src.xml, <prefix>.sink.xml and <prefix>.via.xml")
    optParser.add_option("--weights-output-prefix", dest="weights_outprefix",
                         help="generates weights files for visualisation")
    optParser.add_option("--pedestrians", action="store_true",
                         default=False, help="create a person file with pedestrian trips instead of vehicle trips")
    optParser.add_option("--persontrips", action="store_true",
                         default=False, help="create a person file with person trips instead of vehicle trips")
    optParser.add_option("--prefix", dest="tripprefix",
                         default="", help="prefix for the trip ids")
    optParser.add_option("-t", "--trip-attributes", dest="tripattrs",
                         default="", help="additional trip attributes. When generating pedestrians, attributes for <person> and <walk> are supported.")
    optParser.add_option(
        "-b", "--begin", type="float", default=0, help="begin time")
    optParser.add_option(
        "-e", "--end", type="float", default=3600, help="end time (default 3600)")
    optParser.add_option(
        "-p", "--period", type="float", default=1, help="Generate vehicles with equidistant departure times and period=FLOAT (default 1.0). If option --binomial is used, the expected arrival rate is set to 1/period.")
    optParser.add_option("-s", "--seed", type="int", help="random seed")
    optParser.add_option("-l", "--length", action="store_true",
                         default=False, help="weight edge probability by length")
    optParser.add_option("-L", "--lanes", action="store_true",
                         default=False, help="weight edge probability by number of lanes")
    optParser.add_option("--speed-exponent", type="float", dest="speed_exponent",
                         default=0.0, help="weight edge probability by speed^<FLOAT> (default 0)")
    optParser.add_option("--fringe-factor", type="float", dest="fringe_factor",
                         default=1.0, help="multiply weight of fringe edges by <FLOAT> (default 1")
    optParser.add_option("--fringe-threshold", type="float", dest="fringe_threshold",
                         default=0.0, help="only consider edges with speed above <FLOAT> as fringe edges (default 0)")
    optParser.add_option("--allow-fringe", dest="allow_fringe", action="store_true",
                         default=False, help="Allow departing on edges that leave the network and arriving on edges that enter the network (via turnarounds or as 1-edge trips")
    optParser.add_option("--min-distance", type="float", dest="min_distance",
                         default=0.0, help="require start and end edges for each trip to be at least <FLOAT> m appart")
    optParser.add_option("--max-distance", type="float", dest="max_distance",
                         default=None, help="require start and end edges for each trip to be at most <FLOAT> m appart (default 0 which disables any checks)")
    optParser.add_option("-i", "--intermediate", type="int",
                         default=0, help="generates the given number of intermediate way points")
    optParser.add_option("--maxtries", type="int",
                         default=100, help="number of attemps for finding a trip which meets the distance constraints")
    optParser.add_option("--binomial", type="int", metavar="N",
                         help="If this is set, the number of departures per seconds will be drawn from a binomial distribution with n=N and p=PERIOD/N where PERIOD is the argument given to option --period. Tnumber of attemps for finding a trip which meets the distance constraints")
    optParser.add_option(
        "-c", "--vclass", "--edge-permission", default="passenger",
        help="only from and to edges which permit the given vehicle class")
    optParser.add_option(
        "--vehicle-class", help="The vehicle class assigned to the generated trips (adds a standard vType definition to the output file).")
    optParser.add_option("--validate", default=False, action="store_true",
                         help="Whether to produce trip output that is already checked for connectivity")
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args(args=args)
    if not options.netfile:
        optParser.print_help()
        sys.exit(1)

    if options.persontrips:
        options.pedestrians = True

    if options.pedestrians:
        options.vclass = 'pedestrian'

    if options.validate and options.routefile is None:
        options.routefile = "routes.rou.xml"

    if options.period <= 0:
        print("Error: Period must be positive", file=sys.stderr)
        sys.exit(1)
    return options


# euclidean distance between two coordinates in the plane
def euclidean(a, b):
    return math.sqrt((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2)


class InvalidGenerator(Exception):
    pass

# assigns a weight to each edge using weight_fun and then draws from a discrete
# distribution with these weights


class RandomEdgeGenerator:

    def __init__(self, net, weight_fun):
        self.net = net
        self.weight_fun = weight_fun
        self.cumulative_weights = []
        self.total_weight = 0
        for edge in self.net._edges:
            # print edge.getID(), weight_fun(edge)
            self.total_weight += weight_fun(edge)
            self.cumulative_weights.append(self.total_weight)
        if self.total_weight == 0:
            raise InvalidGenerator()

    def get(self):
        r = random.random() * self.total_weight
        index = bisect.bisect(self.cumulative_weights, r)
        return self.net._edges[index]

    def write_weights(self, fname):
        # normalize to [0,100]
        normalizer = 100.0 / max(1, max(map(self.weight_fun, self.net._edges)))
        with open(fname, 'w+') as f:
            f.write('<edgedata>\n')
            f.write('    <interval begin="0" end="10">\n')
            for i, edge in enumerate(self.net._edges):
                f.write('        <edge id="%s" value="%0.2f"/>\n' %
                        (edge.getID(), self.weight_fun(edge) * normalizer))
            f.write('    </interval>\n')
            f.write('</edgedata>\n')


class RandomTripGenerator:

    def __init__(self, source_generator, sink_generator, via_generator, intermediate, pedestrians):
        self.source_generator = source_generator
        self.sink_generator = sink_generator
        self.via_generator = via_generator
        self.intermediate = intermediate
        self.pedestrians = pedestrians

    def get_trip(self, min_distance, max_distance, maxtries=100):
        for i in range(maxtries):
            source_edge = self.source_generator.get()
            intermediate = [self.via_generator.get()
                            for i in range(self.intermediate)]
            sink_edge = self.sink_generator.get()
            if self.pedestrians:
                destCoord = sink_edge.getFromNode().getCoord()
            else:
                destCoord = sink_edge.getToNode().getCoord()

            coords = ([source_edge.getFromNode().getCoord()]
                      + [e.getFromNode().getCoord() for e in intermediate]
                      + [destCoord])
            distance = sum([euclidean(p, q)
                            for p, q in zip(coords[:-1], coords[1:])])
            if distance >= min_distance and (max_distance is None or distance < max_distance):
                return source_edge, sink_edge, intermediate
        raise Exception("no trip found after %s tries" % maxtries)


def get_prob_fun(options, fringe_bonus, fringe_forbidden):
    # fringe_bonus None generates intermediate way points
    def edge_probability(edge):
        if options.vclass and not edge.allows(options.vclass):
            return 0  # not allowed
        if fringe_bonus is None and edge.is_fringe() and not options.pedestrians:
            return 0  # not suitable as intermediate way point
        if fringe_forbidden is not None and edge.is_fringe(getattr(edge, fringe_forbidden)) and not options.pedestrians:
            return 0  # the wrong kind of fringe
        prob = 1
        if options.length:
            prob *= edge.getLength()
        if options.lanes:
            prob *= edge.getLaneNumber()
        prob *= (edge.getSpeed() ** options.speed_exponent)
        if (options.fringe_factor != 1.0
                and not options.pedestrians
                and fringe_bonus is not None
                and edge.getSpeed() > options.fringe_threshold
                and edge.is_fringe(getattr(edge, fringe_bonus))):
            prob *= options.fringe_factor
        return prob
    return edge_probability


class LoadedProps:

    def __init__(self, fname):
        self.weights = defaultdict(lambda: 0)
        for edge in sumolib.output.parse_fast(fname, 'edge', ['id', 'value']):
            self.weights[edge.id] = float(edge.value)

    def __call__(self, edge):
        return self.weights[edge.getID()]


def buildTripGenerator(net, options):
    try:
        forbidden_source_fringe = None if options.allow_fringe else "_outgoing"
        forbidden_sink_fringe = None if options.allow_fringe else "_incoming"
        source_generator = RandomEdgeGenerator(
            net, get_prob_fun(options, "_incoming", forbidden_source_fringe))
        sink_generator = RandomEdgeGenerator(
            net, get_prob_fun(options, "_outgoing", forbidden_sink_fringe))
        if options.weightsprefix:
            if os.path.isfile(options.weightsprefix + SOURCE_SUFFIX):
                source_generator = RandomEdgeGenerator(
                    net, LoadedProps(options.weightsprefix + SOURCE_SUFFIX))
            if os.path.isfile(options.weightsprefix + SINK_SUFFIX):
                sink_generator = RandomEdgeGenerator(
                    net, LoadedProps(options.weightsprefix + SINK_SUFFIX))
    except InvalidGenerator:
        print("Error: no valid edges for generating source or destination. Try using option --allow-fringe",
              file=sys.stderr)
        return None

    try:
        via_generator = RandomEdgeGenerator(
            net, get_prob_fun(options, None, None))
        if options.weightsprefix and os.path.isfile(options.weightsprefix + VIA_SUFFIX):
            via_generator = RandomEdgeGenerator(
                net, LoadedProps(options.weightsprefix + VIA_SUFFIX))
    except InvalidGenerator:
        if options.intermediate > 0:
            print(
                "Error: no valid edges for generating intermediate points", file=sys.stderr)
            return None
        else:
            via_generator = None

    return RandomTripGenerator(source_generator, sink_generator, via_generator, options.intermediate, options.pedestrians)


def is_walk_attribute(attr):
    for cand in ['departPos', 'arrivalPos', 'speed', 'duration', 'busStop']:
        if cand in attr:
            return True
    return False


def is_persontrip_attribute(attr):
    for cand in ['vTypes', 'modes']:
        if cand in attr:
            return True
    return False


def split_trip_attributes(tripattrs):
    # figure out which of the tripattrs belong to the <person> and which
    # belong to the <walk> or <persontrip>
    personattrs = []
    otherattrs = []
    for a in tripattrs.split():
        if is_walk_attribute(a) or is_persontrip_attribute(a):
            otherattrs.append(a)
        else:
            personattrs.append(a)
    return prependSpace(' '.join(personattrs)), prependSpace(' '.join(otherattrs))


def prependSpace(s):
    if len(s) == 0 or s[0] == " ":
        return s
    else:
        return " " + s


def main(options):
    if options.seed:
        random.seed(options.seed)

    net = sumolib.net.readNet(options.netfile)
    if options.min_distance > net.getBBoxDiameter() * (options.intermediate + 1):
        options.intermediate = int(
            math.ceil(options.min_distance / net.getBBoxDiameter())) - 1
        print("Warning: setting number of intermediate waypoints to %s to achieve a minimum trip length of %s in a network with diameter %.2f." % (
            options.intermediate, options.min_distance, net.getBBoxDiameter()))

    trip_generator = buildTripGenerator(net, options)
    idx = 0

    if options.pedestrians:
        personattrs, otherattrs = split_trip_attributes(options.tripattrs)
    options.tripattrs = prependSpace(options.tripattrs)

    vias = {}

    def generate_one(idx):
        label = "%s%s" % (options.tripprefix, idx)
        try:
            source_edge, sink_edge, intermediate = trip_generator.get_trip(
                options.min_distance, options.max_distance, options.maxtries)
            via = ""
            if len(intermediate) > 0:
                via = ' via="%s" ' % ' '.join(
                    [e.getID() for e in intermediate])
                if options.validate:
                    vias[label] = via
            if options.pedestrians:
                fouttrips.write(
                    '    <person id="%s" depart="%.2f"%s>\n' % (label, depart, personattrs))
                if options.persontrips:
                    fouttrips.write(
                        '        <personTrip from="%s" to="%s"%s/>\n' % (source_edge.getID(), sink_edge.getID(), otherattrs))
                else:
                    fouttrips.write(
                        '        <walk from="%s" to="%s"%s/>\n' % (source_edge.getID(), sink_edge.getID(), otherattrs))
                fouttrips.write('    </person>\n')
            else:
                fouttrips.write('    <trip id="%s" depart="%.2f" from="%s" to="%s"%s%s/>\n' % (
                    label, depart, source_edge.getID(), sink_edge.getID(), via, options.tripattrs))
        except Exception as exc:
            print(exc, file=sys.stderr)
        return idx + 1

    with open(options.tripfile, 'w') as fouttrips:
        sumolib.writeXMLHeader(
            fouttrips, "$Id$", "routes")
        if options.vehicle_class:
            fouttrips.write('    <vType id="%s" vClass="%s" />\n' %
                            (options.vehicle_class, options.vehicle_class))
            options.tripattrs += ' type="%s"' % options.vehicle_class
        depart = options.begin
        if trip_generator:
            while depart < options.end:
                if options.binomial is None:
                    # generate with constant spacing
                    idx = generate_one(idx)
                    depart += options.period
                else:
                    # draw n times from a bernouli distribution
                    # for an average arrival rate of 1 / period
                    prob = 1.0 / options.period / options.binomial
                    for i in range(options.binomial):
                        if random.random() < prob:
                            idx = generate_one(idx)
                    depart += 1
        fouttrips.write("</routes>\n")

    if options.routefile:
        args = [DUAROUTER, '-n', options.netfile, '-t', options.tripfile, '-o', options.routefile, '--ignore-errors',
                '--begin', str(options.begin), '--end', str(options.end), '--no-step-log', '--no-warnings']
        if options.additional is not None:
            args += ['--additional-files', options.additional]
        print("calling ", " ".join(args))
        subprocess.call(args)

    if options.validate:
        print("calling route2trips")
        route2trips.main([options.routefile], outfile=options.tripfile,
                         vias=vias, calledBy=" via randomTrips.py")

    if options.weights_outprefix:
        trip_generator.source_generator.write_weights(
            options.weights_outprefix + SOURCE_SUFFIX)
        trip_generator.sink_generator.write_weights(
            options.weights_outprefix + SINK_SUFFIX)
        trip_generator.via_generator.write_weights(
            options.weights_outprefix + VIA_SUFFIX)

    # return wether trips could be genreated as requested
    return trip_generator is not None

if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
