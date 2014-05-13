#!/usr/bin/env python
"""
@file    randomTrips.py
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2010-03-06
@version $Id$

Generates random trips for the given network.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys, random, bisect, datetime, subprocess
import math
import optparse

SUMO_HOME = os.environ.get('SUMO_HOME', 
        os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib.net

DUAROUTER = os.path.join(SUMO_HOME, 'bin', 'duarouter')


def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("-n", "--net-file", dest="netfile",
                            help="define the net file (mandatory)")
    optParser.add_option("-a", "--additional-files", dest="additional",
                            help="define additional files to be loaded by the router")
    optParser.add_option("-o", "--output-trip-file", dest="tripfile",
                         default="trips.trips.xml", help="define the output trip filename")
    optParser.add_option("-r", "--route-file", dest="routefile",
                         help="generates route file with duarouter")
    optParser.add_option("--pedestrians", action="store_true",
                         default=False, help="create a person file with pedestrian trips instead of vehicle trips")
    optParser.add_option("--prefix", dest="tripprefix",
                         default="", help="prefix for the trip ids")
    optParser.add_option("-t", "--trip-attributes", dest="tripattrs",
                         default="", help="additional trip attributes")
    optParser.add_option("-b", "--begin", type="float", default=0, help="begin time")
    optParser.add_option("-e", "--end", type="float", default=3600, help="end time (default 3600)")
    optParser.add_option("-p", "--period", type="float", default=1, help="repetition period (default 1)")
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
    optParser.add_option("--min-distance", type="float", dest="min_distance",
                         default=0.0, help="require start and end edges for each trip to be at least <FLOAT> m appart")
    optParser.add_option("--max-distance", type="float", dest="max_distance",
                         default=None, help="require start and end edges for each trip to be at most <FLOAT> m appart (default 0 which disables any checks)")
    optParser.add_option("-i", "--intermediate", type="int", 
                         default=0, help="generates the given number of intermediate way points")
    optParser.add_option("--maxtries", type="int", 
                         default=100, help="number of attemps for finding a trip which meets the distance constraints")
    optParser.add_option("-c", "--vclass", 
                         help="only from and to edges which permit <vClass>")
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args()
    if not options.netfile:
        optParser.print_help()
        sys.exit()

    if options.pedestrians and options.vclass is None:
        options.vclass = 'pedestrian'
    return options



# euclidean distance between two coordinates in the plane
def euclidean(a, b):
    return math.sqrt((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2)

# assigns a weight to each edge using weight_fun and then draws from a discrete
# distribution with these weights
class RandomEdgeGenerator:
    def __init__(self, net, weight_fun):
        self.net = net
        self.cumulative_weights = []
        self.total_weight = 0
        for edge in self.net._edges:
            #print edge.getID(), weight_fun(edge)
            self.total_weight += weight_fun(edge)
            self.cumulative_weights.append(self.total_weight)
        assert(self.total_weight > 0)

    def get(self):
        r = random.random() * self.total_weight
        index = bisect.bisect(self.cumulative_weights, r)
        return self.net._edges[index]


class RandomTripGenerator:
    def __init__(self, source_generator, sink_generator, via_generator, intermediate):
        self.source_generator = source_generator
        self.sink_generator = sink_generator
        self.via_generator = via_generator
        self.intermediate = intermediate

    def get_trip(self, min_distance, max_distance, maxtries=100):
        for i in range(maxtries):
            source_edge = self.source_generator.get()
            intermediate = [self.via_generator.get() for i in range(self.intermediate)]
            sink_edge = self.sink_generator.get()
            coords = ([source_edge.getFromNode().getCoord()]
                    + [e.getFromNode().getCoord() for e in intermediate] 
                    + [sink_edge.getToNode().getCoord()])
            distance = sum([euclidean(p,q) for p,q in zip(coords[:-1], coords[1:])])
            if distance >= min_distance and (max_distance is None or distance < max_distance):
                return source_edge, sink_edge, intermediate
        raise Exception("no trip found after %s tries" % maxtries)


def get_prob_fun(options, fringe_bonus, fringe_forbidden):
    # fringe_bonus None generates intermediate way points
    def edge_probability(edge):
        if options.vclass and not edge.allows(options.vclass):
            return 0
        if fringe_bonus is None and edge.is_fringe():
            return 0
        if fringe_forbidden is not None and edge.is_fringe(getattr(edge, fringe_forbidden)):
            return 0
        prob = 1
        if options.length:
            prob *= edge.getLength()
        if options.lanes:
            prob *= edge.getLaneNumber()
        prob *= (edge.getSpeed() ** options.speed_exponent)
        if (options.fringe_factor != 1.0 
                and fringe_bonus is not None 
                and edge.getSpeed() > options.fringe_threshold 
                and edge.is_fringe(getattr(edge, fringe_bonus))):
            prob *= options.fringe_factor
        return prob
    return edge_probability


def main(options):
    if options.seed:
        random.seed(options.seed)

    net = sumolib.net.readNet(options.netfile)
    if options.min_distance > net.getBBoxDiameter() * (options.intermediate + 1):
        options.intermediate = int(math.ceil(options.min_distance / net.getBBoxDiameter())) - 1
        print("Warning: setting number of intermediate waypoints to %s to achieve a minimum trip length of %s in a network with diameter %s." % (
                options.intermediate, options.min_distance, net.getBBoxDiameter()))

    edge_generator = RandomTripGenerator(
            RandomEdgeGenerator(net, get_prob_fun(options, "_incoming", "_outgoing")),
            RandomEdgeGenerator(net, get_prob_fun(options, "_outgoing", "_incoming")),
            RandomEdgeGenerator(net, get_prob_fun(options, None, None)),
            options.intermediate)

    idx = 0
    with open(options.tripfile, 'w') as fouttrips:
        print >> fouttrips, """<?xml version="1.0"?>
<!-- generated on %s by $Id$ 
  options: %s
-->
<trips>""" % (datetime.datetime.now(), (' '.join(sys.argv[1:]).replace('--','<doubleminus>')))
        depart = options.begin
        while depart < options.end:
            label = "%s%s" % (options.tripprefix, idx)
            try:
                source_edge, sink_edge, intermediate = edge_generator.get_trip(options.min_distance, options.max_distance, options.maxtries)
                via = ""
                if len(intermediate) > 0:
                    via='via="%s" ' % ' '.join([e.getID() for e in intermediate])
                if options.pedestrians:
                    print >> fouttrips, '    <person id="%s" depart="%.2f" %s>' % (label, depart, options.tripattrs)
                    print >> fouttrips, '        <walk from="%s" to="%s"/>' % (source_edge.getID(), sink_edge.getID())
                    print >> fouttrips, '    </person>' 
                else:
                    print >> fouttrips, '    <trip id="%s" depart="%.2f" from="%s" to="%s" %s%s/>' % (
                            label, depart, source_edge.getID(), sink_edge.getID(), via, options.tripattrs)
            except Exception, exc:
                print exc
            idx += 1
            depart += options.period
        fouttrips.write("</trips>")

    if options.routefile:
        args = [DUAROUTER, '-n', options.netfile, '-t', options.tripfile, '-o', options.routefile, '--ignore-errors',
            '--begin', str(options.begin), '--end', str(options.end), '--no-step-log']
        if options.additional is not None:
            args += ['--additional-files', options.additional]
        print "calling ", " ".join(args)
        subprocess.call(args)


if __name__ == "__main__":
    main(get_options())
