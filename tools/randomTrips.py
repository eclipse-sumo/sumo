#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    randomTrips.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2010-03-06


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import random
import bisect
import subprocess
from collections import defaultdict
import math

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import euclidean, parseTime, intIfPossible  # noqa
from sumolib.geomhelper import naviDegree, minAngleDegreeDiff  # noqa
from sumolib.net.lane import is_vehicle_class  # noqa

DUAROUTER = sumolib.checkBinary('duarouter')

SOURCE_SUFFIX = ".src.xml"
DEST_SUFFIX = ".dst.xml"
VIA_SUFFIX = ".via.xml"

MAXIMIZE_FACTOR = "max"


def get_options(args=None):
    op = sumolib.options.ArgumentParser(description="Generate trips between random locations",
                                        allowed_programs=['duarouter'])
    # input
    op.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=op.net_file,
                    help="define the net file (mandatory)")
    op.add_argument("-a", "--additional-files", category="input", dest="additional", type=op.additional_file,
                    help="define additional files to be loaded by the router")
    op.add_argument("--weights-prefix", category="input", dest="weightsprefix", type=op.file,
                    help="loads probabilities for being source, destination and via-edge from the files named " +
                    "'prefix'.src.xml, 'prefix'.dst.xml and 'prefix'.via.xml")
    # output
    op.add_argument("-o", "--output-trip-file", category="output", dest="tripfile", type=op.route_file,
                    default="trips.trips.xml",
                    help="define the output trip filename")
    op.add_argument("-r", "--route-file", category="output", dest="routefile", type=op.route_file,
                    help="generates route file with duarouter")
    op.add_argument("--vtype-output", category="output", dest="vtypeout", type=op.file,
                    help="Store generated vehicle types in a separate file")
    op.add_argument("--weights-output-prefix", category="output", dest="weights_outprefix", type=op.file,
                    help="generates weights files for visualisation")
    # persons
    op.add_argument("--pedestrians", category="persons", action="store_true", default=False,
                    help="create a person file with pedestrian trips instead of vehicle trips")
    op.add_argument("--personrides", category="persons",
                    help="create a person file with rides using STR as lines attribute")
    op.add_argument("--persontrips", category="persons", action="store_true", default=False,
                    help="create a person file with person trips instead of vehicle trips")
    op.add_argument("--persontrip.transfer.car-walk", category="persons", dest="carWalkMode",
                    help="Where are mode changes from car to walking allowed " +
                    "(possible values: 'ptStops', 'allJunctions' and combinations)")
    op.add_argument("--persontrip.walkfactor", category="persons", dest="walkfactor", metavar="FLOAT", type=float,
                    help="Use FLOAT as a factor on pedestrian maximum speed during intermodal routing")
    op.add_argument("--persontrip.walk-opposite-factor", category="persons", dest="walkoppositefactor",
                    metavar="FLOAT", type=float,
                    help="Use FLOAT as a factor on pedestrian maximum speed against vehicle traffic direction")
    op.add_argument("--from-stops", category="persons", dest="fromStops",
                    help="Create trips that start at stopping places of the indicated type(s). i.e. 'busStop'")
    op.add_argument("--to-stops", category="persons", dest="toStops",
                    help="Create trips that end at stopping places of the indicated type(s). i.e. 'busStop'")
    # attributes
    op.add_argument("--prefix", category="attributes", dest="tripprefix", default="",
                    help="prefix for the trip ids")
    op.add_argument("-t", "--trip-attributes", category="attributes", dest="tripattrs", default="",
                    help="additional trip attributes. When generating pedestrians, attributes for " +
                    "'person' and 'walk' are supported.")
    op.add_argument("--fringe-start-attributes", category="attributes", dest="fringeattrs", default="",
                    help="additional trip attributes when starting on a fringe.")
    op.add_argument("--vehicle-class",
                    help="The vehicle class assigned to the generated trips (adds a standard vType definition " +
                    "to the output file).")
    op.add_argument("--random-departpos", category="attributes", dest="randomDepartPos", action="store_true",
                    default=False, help="Randomly choose a position on the starting edge of the trip")
    op.add_argument("--random-arrivalpos", category="attributes", dest="randomArrivalPos", action="store_true",
                    default=False, help="Randomly choose a position on the ending edge of the trip")
    op.add_argument("--junction-taz", category="attributes", dest="junctionTaz", action="store_true", default=False,
                    help="Write trips with fromJunction and toJunction")
    # weights
    op.add_argument("-l", "--length", category="weights", action="store_true", default=False,
                    help="weight edge probability by length")
    op.add_argument("-L", "--lanes", category="weights", action="store_true", default=False,
                    help="weight edge probability by number of lanes")
    op.add_argument("--edge-param", category="weights", dest="edgeParam",
                    help="use the given edge parameter as factor for edge")
    op.add_argument("--speed-exponent", category="weights", dest="speed_exponent", metavar="FLOAT", type=float,
                    default=0.0, help="weight edge probability by speed^'FLOAT' (default 0)")
    op.add_argument("--fringe-speed-exponent", category="weights", dest="fringe_speed_exponent", metavar="FLOAT",
                    help="weight fringe edge probability by speed^'FLOAT' (default: speed exponent)")
    op.add_argument("--angle", category="weights", dest="angle", default=90.0, type=float,
                    help="weight edge probability by angle [0-360] relative to the network center")
    op.add_argument("--angle-factor", category="weights", dest="angle_weight", default=1.0, type=float,
                    help="maximum weight factor for angle")
    op.add_argument("--random-factor", category="weights", dest="randomFactor", default=1.0, type=float,
                    help="edge weights are dynamically disturbed by a random factor drawn uniformly from [1,FLOAT]")
    op.add_argument("--fringe-factor", category="weights", dest="fringe_factor", default="1.0",
                    help="multiply weight of fringe edges by 'FLOAT' (default 1)" +
                    " or set value 'max' to force all traffic to start/end at the fringe.")
    op.add_argument("--fringe-threshold", category="weights", dest="fringe_threshold", default=0.0, type=float,
                    help="only consider edges with speed above 'FLOAT' as fringe edges (default 0)")
    op.add_argument("--allow-fringe", category="weights", dest="allow_fringe", action="store_true", default=False,
                    help="Allow departing on edges that leave the network and arriving on edges " +
                    "that enter the network (via turnarounds or as 1-edge trips")
    op.add_argument("--allow-fringe.min-length", category="weights", dest="allow_fringe_min_length", type=float,
                    help="Allow departing on edges that leave the network and arriving on edges " +
                    "that enter the network, if they have at least the given length")
    op.add_argument("--fringe-junctions", category="weights", action="store_true", dest="fringeJunctions",
                    default=False, help="Determine fringe edges based on junction attribute 'fringe'")
    op.add_argument("--vclass", "--edge-permission", category="weights", default="passenger",
                    help="only from and to edges which permit the given vehicle class")
    op.add_argument("--via-edge-types", category="weights", dest="viaEdgeTypes",
                    help="Set list of edge types that cannot be used for departure or arrival " +
                    "(unless being on the fringe)")
    op.add_argument("--allow-roundabouts", category="weights", dest="allowRoundabouts", action="store_true",
                    default=False, help="Permit trips that start or end inside a roundabout")
    # processing
    op.add_argument("-s", "--seed", default=42, type=int,
                    help="random seed")
    op.add_argument("--random", action="store_true", default=False,
                    help="use a random seed to initialize the random number generator")
    op.add_argument("--min-distance", dest="min_distance", metavar="FLOAT", default=0.0,
                    type=float, help="require start and end edges for each trip to be at least 'FLOAT' m apart")
    op.add_argument("--min-distance.fringe", dest="min_dist_fringe", metavar="FLOAT", type=float,
                    help="require start and end edges for each fringe to fringe trip to be at least 'FLOAT' m apart")
    op.add_argument("--max-distance", dest="max_distance", metavar="FLOAT", type=float,
                    help="require start and end edges for each trip to be at most 'FLOAT' m " +
                    "apart (default 0 which disables any checks)")
    op.add_argument("-i", "--intermediate", default=0, type=int,
                    help="generates the given number of intermediate way points")
    op.add_argument("--jtrrouter", action="store_true", default=False,
                    help="Create flows without destination as input for jtrrouter")
    op.add_argument("--maxtries", default=100, type=int,
                    help="number of attemps for finding a trip which meets the distance constraints")
    op.add_argument("--remove-loops", dest="remove_loops", action="store_true", default=False,
                    help="Remove loops at route start and end")
    op.add_argument("--random-routing-factor", dest="randomRoutingFactor", default=1, type=float,
                    help="Edge weights for routing are dynamically disturbed "
                    "by a random factor drawn uniformly from [1,FLOAT)")
    op.add_argument("--validate", default=False, action="store_true",
                    help="Whether to produce trip output that is already checked for connectivity")
    op.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    # flow
    op.add_argument("-b", "--begin", category="flow", default=0, type=op.time,
                    help="begin time")
    op.add_argument("-e", "--end", category="flow", default=3600, type=op.time,
                    help="end time (default 3600)")
    group = op.add_mutually_exclusive_group()
    group.add_argument("-p", "--period", nargs="+", metavar="FLOAT", category="flow",
                       action=sumolib.options.SplitAction,
                       help="Generate vehicles with equidistant departure times and period=FLOAT (default 1.0). " +
                       "If option --binomial is used, the expected arrival rate is set to 1/period.")
    group.add_argument("--insertion-rate", dest="insertionRate", nargs="+", metavar="FLOAT", category="flow",
                       action=sumolib.options.SplitAction,
                       help="How much vehicles arrive in the simulation per hour (alternative to the period option).")
    group.add_argument("--insertion-density", dest="insertionDensity", nargs="+", metavar="FLOAT", category="flow",
                       action=sumolib.options.SplitAction,
                       help="How much vehicles arrive in the simulation per hour per kilometer of road " +
                       "(alternative to the period option).")
    op.add_argument("--flows", category="flow", default=0, type=int,
                    help="generates INT flows that together output vehicles with the specified period")
    op.add_argument("--random-depart", category="flow", action="store_true", dest="randomDepart", default=False,
                    help="Distribute departures randomly between begin and end")
    op.add_argument("--binomial",  category="flow", metavar="N", type=int,
                    help="If this is set, the number of departures per second will be drawn from a binomial " +
                    "distribution with n=N and p=PERIOD/N where PERIOD is the argument given to --period")

    options = op.parse_args(args=args)
    if options.vclass and not is_vehicle_class(options.vclass):
        raise ValueError("The string '%s' doesn't correspond to a legit vehicle class." % options.vclass)

    if options.persontrips or options.personrides:
        options.pedestrians = True

    if options.pedestrians:
        options.vclass = 'pedestrian'
        if options.flows > 0:
            raise ValueError("Person flows are not supported yet.")
    if options.validate and options.routefile is None:
        options.routefile = "routes.rou.xml"

    if options.period is None and options.insertionRate is None and options.insertionDensity is None:
        options.period = [1.]

    options.net = sumolib.net.readNet(options.netfile)
    if options.insertionDensity:
        # Compute length of the network
        length = 0.  # In meters
        for edge in options.net.getEdges():
            if edge.allows(options.vclass):
                length += edge.getLaneNumber() * edge.getLength()
        options.insertionRate = [density * (length / 1000.0) for density in options.insertionDensity]

    if options.insertionRate:
        options.period = [3600.0 / rate if rate != 0.0 else 0.0 for rate in options.insertionRate]

    if options.period:
        if any([period < 0 for period in options.period]):
            raise ValueError("Period / insertionRate must be non-negative.")
        options.period = list(map(intIfPossible, options.period))
        if options.binomial:
            for p in options.period:
                if p != 0.0 and 1.0 / p / options.binomial >= 1:
                    print("Warning: Option --binomial %s is too low for insertion period %s." % (options.binomial, p)
                          + " Insertions will not be randomized.", file=sys.stderr)

    if options.jtrrouter and options.flows <= 0:
        raise ValueError("Option --jtrrouter must be used with option --flows.")

    if options.vehicle_class:
        if not is_vehicle_class(options.vehicle_class):
            raise ValueError("The string '%s' doesn't correspond to a legit vehicle class." % options.vehicle_class)

        if options.tripprefix:
            options.vtypeID = "%s_%s" % (options.tripprefix, options.vehicle_class)
        else:
            options.vtypeID = options.vehicle_class

        if 'type=' in options.tripattrs:
            raise ValueError("Trip-attribute 'type' cannot be used together with option --vehicle-class.")

    if options.randomDepartPos:
        if 'departPos' in options.tripattrs:
            raise ValueError("Trip-attribute 'departPos' cannot be used together with option --random-departpos.")

    if options.randomArrivalPos:
        if 'arrivalPos' in options.tripattrs:
            raise ValueError("Trip-attribute 'arrivalPos' cannot be used together with option --random-arrivalpos.")

    if options.weightsprefix:
        weight_files = [options.weightsprefix + s for s in (SOURCE_SUFFIX, DEST_SUFFIX, VIA_SUFFIX)]
        if not any([os.path.isfile(w) for w in weight_files]):
            raise ValueError("None of the weight files '%s' exists." % "', '".join(weight_files))

    if options.randomFactor < 1:
        raise ValueError("Option --random-factor requires a value >= 1.")

    if options.fromStops or options.toStops:
        options.edgeFromStops, options.edgeToStops = loadStops(options)

    if options.viaEdgeTypes:
        options.viaEdgeTypes = options.viaEdgeTypes.split(',')
    if options.fringe_speed_exponent is None:
        options.fringe_speed_exponent = options.speed_exponent

    if options.fringe_factor.lower() == MAXIMIZE_FACTOR:
        options.fringe_factor = MAXIMIZE_FACTOR
    else:
        try:
            options.fringe_factor = float(options.fringe_factor)
            if options.fringe_factor < 0:
                raise ValueError("--fringe-factor argument may not be negative.")
        except ValueError:
            raise ValueError("--fringe-factor argument must be a float or 'max'.")

    return options


class InvalidGenerator(Exception):
    pass


def loadStops(options):
    edgeFromStops = defaultdict(list)  # edge -> [(stopType1, stopID1), ...]
    edgeToStops = defaultdict(list)  # edge -> [(stopType1, stopID1), ...]
    if options.additional is None:
        print("Error: Option %s requires option --additional-files for loading infrastructure elements" %
              ("--from-stops" if options.fromStops else "--to-stops"), file=sys.stderr)
        sys.exit(1)
    stopTypes = []
    if options.fromStops:
        options.fromStops = options.fromStops.split(',')
        stopTypes += options.fromStops
    else:
        options.fromStops = []
    if options.toStops:
        options.toStops = options.toStops.split(',')
        stopTypes += options.toStops
    else:
        options.toStops = []
    stopTypes = list(set(stopTypes))
    typeCounts = defaultdict(lambda: 0)
    for additional in options.additional.split(','):
        for stop in sumolib.xml.parse(additional, stopTypes):
            edgeID = stop.lane.rsplit('_', 1)[0]
            if stop.name in options.fromStops:
                edgeFromStops[edgeID].append((stop.name, stop.id))
            if stop.name in options.toStops:
                edgeToStops[edgeID].append((stop.name, stop.id))
            typeCounts[stop.name] += 1

    if options.fromStops:
        available = sum([typeCounts[t] for t in options.fromStops])
        if available == 0:
            print("No stops of type%s '%s' were found in additional-files %s" % (
                ('' if len(options.fromStops) == 1 else 's'),
                options.fromStops[0], options.additional), file=sys.stderr)
            sys.exit(1)
    if options.toStops:
        available = sum([typeCounts[t] for t in options.toStops])
        if available == 0:
            print("No stops of type%s '%s' were found in additional-files %s" % (
                ('' if len(options.toStops) == 1 else 's'),
                options.toStops[0], options.additional), file=sys.stderr)
            sys.exit(1)
    return edgeFromStops, edgeToStops


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

    def write_weights(self, fname, interval_id, begin, end):
        # normalize to [0,100]
        normalizer = 100.0 / max(1, max(map(self.weight_fun, self.net._edges)))
        weights = [(self.weight_fun(e) * normalizer, e.getID()) for e in self.net.getEdges()]
        weights.sort(reverse=True)
        with open(fname, 'w+') as f:
            f.write('<edgedata>\n')
            f.write('    <interval id="%s" begin="%s" end="%s">\n' % (
                interval_id, begin, end))
            for weight, edgeID in weights:
                f.write('        <edge id="%s" value="%0.2f"/>\n' %
                        (edgeID, weight))
            f.write('    </interval>\n')
            f.write('</edgedata>\n')


class RandomTripGenerator:

    def __init__(self, source_generator, sink_generator, via_generator, intermediate, pedestrians):
        self.source_generator = source_generator
        self.sink_generator = sink_generator
        self.via_generator = via_generator
        self.intermediate = intermediate
        self.pedestrians = pedestrians

    def get_trip(self, min_distance, max_distance, maxtries=100, junctionTaz=False, min_dist_fringe=None):
        for min_dist in [min_distance, min_dist_fringe]:
            if min_dist is None:
                break
            for _ in range(maxtries):
                source_edge = self.source_generator.get()
                intermediate = [self.via_generator.get() for __ in range(self.intermediate)]
                sink_edge = self.sink_generator.get()
                is_fringe2fringe = source_edge.is_fringe() and sink_edge.is_fringe() and not intermediate
                if min_dist == min_dist_fringe and not is_fringe2fringe:
                    continue
                if self.pedestrians:
                    destCoord = sink_edge.getFromNode().getCoord()
                else:
                    destCoord = sink_edge.getToNode().getCoord()
                coords = ([source_edge.getFromNode().getCoord()] +
                          [e.getFromNode().getCoord() for e in intermediate] +
                          [destCoord])
                distance = sum([euclidean(p, q)
                                for p, q in zip(coords[:-1], coords[1:])])
                if (distance >= min_dist
                        and (not junctionTaz or source_edge.getFromNode() != sink_edge.getToNode())
                        and (max_distance is None or distance < max_distance)):
                    return source_edge, sink_edge, intermediate
        raise Exception("Warning: no trip found after %s tries" % maxtries)


def get_prob_fun(options, fringe_bonus, fringe_forbidden, max_length):
    # fringe_bonus None generates intermediate way points
    randomProbs = defaultdict(lambda: 1)
    if options.randomFactor != 1:
        for edge in options.net.getEdges():
            randomProbs[edge.getID()] = random.uniform(1, options.randomFactor)

    roundabouts = set()
    if not options.allowRoundabouts:
        for roundabout in options.net.getRoundabouts():
            roundabouts.update(roundabout.getEdges())

    stopDict = None
    if options.fromStops and fringe_bonus == "_incoming":
        stopDict = options.edgeFromStops
    elif options.toStops and fringe_bonus == "_outgoing":
        stopDict = options.edgeToStops

    def edge_probability(edge):
        bonus_connections = None if fringe_bonus is None else getattr(edge, fringe_bonus)
        forbidden_connections = None if fringe_forbidden is None else getattr(edge, fringe_forbidden)
        if options.vclass and not edge.allows(options.vclass) and not stopDict:
            return 0  # not allowed
        if fringe_bonus is None and edge.is_fringe() and not options.pedestrians:
            return 0  # not suitable as intermediate way point
        if (fringe_forbidden is not None and
                edge.is_fringe(forbidden_connections) and
                not options.pedestrians and
                (options.allow_fringe_min_length is None or edge.getLength() < options.allow_fringe_min_length)):
            return 0  # the wrong kind of fringe
        if (fringe_bonus is not None and options.viaEdgeTypes is not None and
                not edge.is_fringe(bonus_connections, checkJunctions=options.fringeJunctions) and
                edge.getType() in options.viaEdgeTypes):
            return 0  # the wrong type of edge (only allows depart and arrival on the fringe)
        if fringe_bonus is not None and edge.getID() in roundabouts:
            return 0  # traffic typically does not start/end inside a roundabout
        prob = randomProbs[edge.getID()]
        if stopDict:
            prob *= len(stopDict[edge.getID()])
        if options.length:
            if (options.fringe_factor != 1.0 and fringe_bonus is not None and
                    edge.is_fringe(bonus_connections, checkJunctions=options.fringeJunctions)):
                # short fringe edges should not suffer a penalty
                prob *= max_length
            else:
                prob *= edge.getLength()
        if options.lanes:
            prob *= edge.getLaneNumber()
        if edge.is_fringe(bonus_connections, checkJunctions=options.fringeJunctions):
            prob *= (edge.getSpeed() ** options.fringe_speed_exponent)
        else:
            prob *= (edge.getSpeed() ** options.speed_exponent)
        if options.fringe_factor != 1.0 and fringe_bonus is not None:
            isFringe = (edge.getSpeed() > options.fringe_threshold and
                        edge.is_fringe(bonus_connections, checkJunctions=options.fringeJunctions))
            if isFringe and options.fringe_factor != MAXIMIZE_FACTOR:
                prob *= options.fringe_factor
            elif not isFringe and options.fringe_factor == MAXIMIZE_FACTOR:
                prob = 0
        if options.edgeParam is not None:
            prob *= float(edge.getParam(options.edgeParam, 1.0))
        if options.angle_weight != 1.0 and fringe_bonus is not None:
            xmin, ymin, xmax, ymax = edge.getBoundingBox()
            ex, ey = ((xmin + xmax) / 2, (ymin + ymax) / 2)
            nx, ny = options.angle_center
            edgeAngle = naviDegree(math.atan2(ey - ny, ex - nx))
            angleDiff = minAngleDegreeDiff(options.angle, edgeAngle)
            # print("e=%s nc=%s ec=%s ea=%s a=%s ad=%s" % (
            #    edge.getID(), options.angle_center, (ex,ey), edgeAngle,
            #    options.angle, angleDiff))
            # relDist = 2 * euclidean((ex, ey), options.angle_center) / max(xmax - xmin, ymax - ymin)
            # prob *= (relDist * (options.angle_weight - 1) + 1)
            if fringe_bonus == "_incoming":
                # source edge
                prob *= (angleDiff * (options.angle_weight - 1) + 1)
            else:
                prob *= ((180 - angleDiff) * (options.angle_weight - 1) + 1)

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
        max_length = 0
        for edge in net.getEdges():
            if not edge.is_fringe():
                max_length = max(max_length, edge.getLength())
        forbidden_source_fringe = None if options.allow_fringe else "_outgoing"
        forbidden_sink_fringe = None if options.allow_fringe else "_incoming"
        source_generator = RandomEdgeGenerator(
            net, get_prob_fun(options, "_incoming", forbidden_source_fringe, max_length))
        sink_generator = RandomEdgeGenerator(
            net, get_prob_fun(options, "_outgoing", forbidden_sink_fringe, max_length))
        if options.weightsprefix:
            if os.path.isfile(options.weightsprefix + SOURCE_SUFFIX):
                source_generator = RandomEdgeGenerator(
                    net, LoadedProps(options.weightsprefix + SOURCE_SUFFIX))
            if os.path.isfile(options.weightsprefix + DEST_SUFFIX):
                sink_generator = RandomEdgeGenerator(
                    net, LoadedProps(options.weightsprefix + DEST_SUFFIX))
    except InvalidGenerator:
        print("Error: no valid edges for generating source or destination. Try using option --allow-fringe",
              file=sys.stderr)
        return None

    try:
        via_generator = RandomEdgeGenerator(
            net, get_prob_fun(options, None, None, 1))
        if options.weightsprefix and os.path.isfile(options.weightsprefix + VIA_SUFFIX):
            via_generator = RandomEdgeGenerator(
                net, LoadedProps(options.weightsprefix + VIA_SUFFIX))
    except InvalidGenerator:
        if options.intermediate > 0:
            print("Error: no valid edges for generating intermediate points", file=sys.stderr)
            return None
        else:
            via_generator = None

    return RandomTripGenerator(
        source_generator, sink_generator, via_generator, options.intermediate, options.pedestrians)


def is_walk_attribute(attr):
    for cand in ['arrivalPos', 'speed=', 'duration=', 'busStop=']:
        if cand in attr:
            return True
    return False


def is_persontrip_attribute(attr):
    for cand in ['vTypes', 'modes']:
        if cand in attr:
            return True
    return False


def is_person_attribute(attr):
    for cand in ['departPos', 'type']:
        if cand in attr:
            return True
    return False


def is_vehicle_attribute(attr):
    # speedFactor could be used in vType and vehicle but we need it in the vType
    # to allow for the multi-parameter version
    for cand in ['depart', 'arrival', 'line', 'personNumber', 'containerNumber', 'type']:
        if cand in attr:
            return True
    return False


def split_trip_attributes(tripattrs, pedestrians, hasType, verbose):
    # handle attribute values with a space
    # assume that no attribute value includes an '=' sign
    allattrs = []
    for a in tripattrs.split():
        if "=" in a:
            allattrs.append(a)
        else:
            if len(allattrs) == 0:
                print("Warning: invalid trip-attribute '%s'" % a)
            else:
                allattrs[-1] += ' ' + a

    # figure out which of the tripattrs belong to the <person> or <vehicle>,
    # which belong to the <vType> and which belong to the <walk> or <persontrip>
    vehicleattrs = []
    personattrs = []
    vtypeattrs = []
    otherattrs = []
    for a in allattrs:
        if pedestrians:
            if is_walk_attribute(a) or is_persontrip_attribute(a):
                otherattrs.append(a)
            elif is_person_attribute(a):
                personattrs.append(a)
            else:
                vtypeattrs.append(a)
        else:
            if is_vehicle_attribute(a):
                vehicleattrs.append(a)
            else:
                vtypeattrs.append(a)

    if not hasType:
        if pedestrians:
            personattrs += vtypeattrs
        else:
            vehicleattrs += vtypeattrs
        vtypeattrs = []

    return (prependSpace(' '.join(vtypeattrs)),
            prependSpace(' '.join(vehicleattrs)),
            prependSpace(' '.join(personattrs)),
            prependSpace(' '.join(otherattrs)))


def prependSpace(s):
    if len(s) == 0 or s[0] == " ":
        return s
    else:
        return " " + s


def samplePosition(edge):
    return random.uniform(0.0, edge.getLength())


def main(options):
    if all([period == 0 for period in options.period]):
        print("Warning: All intervals are empty.", file=sys.stderr)
        return False

    if not options.random:
        random.seed(options.seed)

    if options.min_distance > options.net.getBBoxDiameter() * (options.intermediate + 1):
        options.intermediate = int(math.ceil(options.min_distance / options.net.getBBoxDiameter())) - 1
        print(("Warning: Using %s intermediate waypoints to achieve a minimum trip length of %s in a network "
               "with diameter %.2f.") % (options.intermediate, options.min_distance, options.net.getBBoxDiameter()),
              file=sys.stderr)

    if options.angle_weight != 1:
        xmin, ymin, xmax, ymax = options.net.getBoundary()
        options.angle_center = (xmin + xmax) / 2, (ymin + ymax) / 2

    trip_generator = buildTripGenerator(options.net, options)
    idx = 0

    vtypeattrs, options.tripattrs, personattrs, otherattrs = split_trip_attributes(
        options.tripattrs, options.pedestrians, options.vehicle_class, options.verbose)

    vias = {}

    time_delta = (parseTime(options.end) - parseTime(options.begin)) / len(options.period)
    times = [parseTime(options.begin) + i * time_delta for i in range(len(options.period) + 1)]
    times = list(map(intIfPossible, times))

    def generate_origin_destination(trip_generator, options):
        source_edge, sink_edge, intermediate = trip_generator.get_trip(
            options.min_distance, options.max_distance, options.maxtries,
            options.junctionTaz, options.min_dist_fringe)
        return source_edge, sink_edge, intermediate

    def generate_attributes(idx, departureTime, arrivalTime, origin, destination, intermediate, options):
        label = "%s%s" % (options.tripprefix, idx)
        combined_attrs = options.tripattrs
        if options.randomDepartPos:
            randomPosition = samplePosition(origin)
            combined_attrs += ' departPos="%.2f"' % randomPosition
        if options.randomArrivalPos:
            randomPosition = samplePosition(destination)
            combined_attrs += ' arrivalPos="%.2f"' % randomPosition
        if options.fringeattrs and origin.is_fringe(
                origin._incoming, checkJunctions=options.fringeJunctions):
            combined_attrs += " " + options.fringeattrs
        if options.junctionTaz:
            attrFrom = ' fromJunction="%s"' % origin.getFromNode().getID()
            attrTo = ' toJunction="%s"' % destination.getToNode().getID()
        else:
            attrFrom = ' from="%s"' % origin.getID()
            attrTo = ' to="%s"' % destination.getID()
        if options.fromStops:
            attrFrom = ' %s="%s"' % random.choice(options.edgeFromStops[origin.getID()])
        if options.toStops:
            attrTo = ' %s="%s"' % random.choice(options.edgeToStops[destination.getID()])
        via = ""
        if intermediate:
            via = ' via="%s" ' % ' '.join(
                [e.getID() for e in intermediate])
            if options.validate:
                vias[label] = via
        return label, combined_attrs, attrFrom, attrTo, via

    def generate_one_person(label, combined_attrs, attrFrom, attrTo, departureTime, intermediate, options):
        fouttrips.write(
            '    <person id="%s" depart="%.2f"%s>\n' % (label, departureTime, personattrs))
        element = "walk"
        attrs = otherattrs
        if options.fromStops:
            fouttrips.write('        <stop%s duration="0"/>\n' % attrFrom)
            attrFrom = ''
        if options.persontrips:
            element = "personTrip"
        elif options.personrides:
            element = "ride"
            attrs = ' lines="%s%s"' % (options.personrides, otherattrs)
        if intermediate:
            fouttrips.write('        <%s%s to="%s"%s/>\n' % (element, attrFrom, intermediate[0].getID(), attrs))
            for edge in intermediate[1:]:
                fouttrips.write('        <%s to="%s"%s/>\n' % (element, edge.getID(), attrs))
            fouttrips.write('        <%s%s%s/>\n' % (element, attrTo, attrs))
        else:
            fouttrips.write('        <%s%s%s%s/>\n' % (element, attrFrom, attrTo, attrs))
        fouttrips.write('    </person>\n')

    def generate_one_flow(label, combined_attrs, departureTime, arrivalTime, period, options, timeIdx):
        if len(options.period) > 1:
            label = label + "#%s" % timeIdx
        if options.binomial:
            for j in range(options.binomial):
                fouttrips.write(('    <flow id="%s#%s" begin="%s" end="%s" probability="%.2f"%s/>\n') % (
                    label, j, departureTime, arrivalTime, 1.0 / period / options.binomial,
                    combined_attrs))
        else:
            fouttrips.write(('    <flow id="%s" begin="%s" end="%s" period="%s"%s/>\n') % (
                label, departureTime, arrivalTime, intIfPossible(period * options.flows), combined_attrs))

    def generate_one_trip(label, combined_attrs, departureTime):
        fouttrips.write('    <trip id="%s" depart="%.2f"%s/>\n' % (
            label, departureTime, combined_attrs))

    def generate_one(idx, departureTime, arrivalTime, period, origin, destination, intermediate, timeIdx=None):
        try:
            label, combined_attrs, attrFrom, attrTo, via = generate_attributes(
                idx, departureTime, arrivalTime, origin, destination, intermediate, options)

            if options.pedestrians:
                generate_one_person(label, combined_attrs, attrFrom, attrTo, departureTime, intermediate, options)
            else:
                if options.jtrrouter:
                    attrTo = ''

                combined_attrs = attrFrom + attrTo + via + combined_attrs

                if options.flows > 0:
                    generate_one_flow(label, combined_attrs, departureTime, arrivalTime, period, options, timeIdx)
                else:
                    generate_one_trip(label, combined_attrs, departureTime)

        except Exception as exc:
            if options.verbose:
                print(exc, file=sys.stderr)

        return idx + 1

    with open(options.tripfile, 'w') as fouttrips:
        sumolib.writeXMLHeader(fouttrips, "$Id$", "routes", options=options)
        if options.vehicle_class:
            vTypeDef = '    <vType id="%s" vClass="%s"%s/>\n' % (
                options.vtypeID, options.vehicle_class, vtypeattrs)
            if options.vtypeout:
                # ensure that trip output does not contain types, file may be
                # overwritten by later call to duarouter
                if options.additional is None:
                    options.additional = options.vtypeout
                else:
                    options.additional = options.additional + "," + options.vtypeout
                with open(options.vtypeout, 'w') as fouttype:
                    sumolib.writeXMLHeader(fouttype, "$Id$", "additional", options=options)
                    fouttype.write(vTypeDef)
                    fouttype.write("</additional>\n")
            else:
                fouttrips.write(vTypeDef)
            options.tripattrs += ' type="%s"' % options.vtypeID
            personattrs += ' type="%s"' % options.vtypeID

        if trip_generator:
            if options.flows == 0:
                for i in range(len(times)-1):
                    time = departureTime = parseTime(times[i])
                    arrivalTime = parseTime(times[i+1])
                    period = options.period[i]
                    if period == 0.0:
                        continue
                    if options.binomial is None:
                        departures = []
                        if options.randomDepart:
                            subsecond = math.fmod(period, 1)
                            while time < arrivalTime:
                                rTime = random.randrange(int(departureTime), int(arrivalTime))
                                time += period
                                if subsecond != 0:
                                    # allow all multiples of subsecond to appear
                                    rSubSecond = math.fmod(
                                        subsecond * random.randrange(int(departureTime), int(arrivalTime)), 1)
                                    rTime = min(arrivalTime, rTime + rSubSecond)
                                departures.append(rTime)
                            departures.sort()
                        else:
                            while departureTime < arrivalTime:
                                departures.append(departureTime)
                                departureTime += period

                        for time in departures:
                            # generate with constant spacing
                            try:
                                origin, destination, intermediate = generate_origin_destination(trip_generator, options)
                                idx = generate_one(idx, time, arrivalTime, period, origin, destination, intermediate)
                            except Exception as exc:
                                print(exc, file=sys.stderr)
                    else:
                        time = departureTime
                        while time < arrivalTime:
                            # draw n times from a Bernoulli distribution
                            # for an average arrival rate of 1 / period
                            prob = 1.0 / period / options.binomial
                            for _ in range(options.binomial):
                                if random.random() < prob:
                                    try:
                                        origin, destination, intermediate = generate_origin_destination(
                                            trip_generator, options)
                                        idx = generate_one(idx, time, arrivalTime, period,
                                                           origin, destination, intermediate)
                                    except Exception as exc:
                                        if options.verbose:
                                            print(exc, file=sys.stderr)
                            time += 1.0
            else:
                try:
                    origins_destinations = [generate_origin_destination(
                        trip_generator, options) for _ in range(options.flows)]
                    for i in range(len(times)-1):
                        for j in range(options.flows):
                            departureTime = times[i]
                            arrivalTime = times[i+1]
                            period = options.period[i]
                            if period == 0.0:
                                continue
                            origin, destination, intermediate = origins_destinations[j]
                            generate_one(j, departureTime, arrivalTime, period, origin, destination, intermediate, i)
                except Exception as exc:
                    if options.verbose:
                        print(exc, file=sys.stderr)

        fouttrips.write("</routes>\n")

    # call duarouter for routes or validated trips
    args = [DUAROUTER, '-n', options.netfile, '-r', options.tripfile, '--ignore-errors',
            '--begin', str(options.begin), '--end', str(options.end),
            '--alternatives-output', 'NUL',
            '--no-step-log']
    if options.additional is not None:
        args += ['--additional-files', options.additional]
    if options.carWalkMode is not None:
        args += ['--persontrip.transfer.car-walk', options.carWalkMode]
    if options.walkfactor is not None:
        args += ['--persontrip.walkfactor', str(options.walkfactor)]
    if options.walkoppositefactor is not None:
        args += ['--persontrip.walk-opposite-factor', str(options.walkoppositefactor)]
    if options.remove_loops:
        args += ['--remove-loops']
    if options.randomRoutingFactor != 1:
        args += ['--weights.random-factor', str(options.randomRoutingFactor)]
    if options.vtypeout is not None:
        args += ['--vtype-output', options.vtypeout]
    if options.junctionTaz:
        args += ['--junction-taz']
    if not options.verbose:
        args += ['--no-warnings']
    else:
        args += ['-v']

    options_to_forward = sumolib.options.get_prefixed_options(options)
    if 'duarouter' in options_to_forward:
        for option in options_to_forward['duarouter']:
            option[0] = '--' + option[0]
            if option[0] not in args:
                args += option
            else:
                raise ValueError("The argument '%s' has already been passed without the duarouter prefix." % option[0])

    if options.routefile:
        args2 = args + ['-o', options.routefile]
        if options.verbose:
            print("calling", " ".join(args2))
            sys.stdout.flush()
        subprocess.call(args2)
        sys.stdout.flush()
        sumolib.xml.insertOptionsHeader(options.routefile, options)

    if options.validate:
        # write to temporary file because the input is read incrementally
        tmpTrips = options.tripfile + ".tmp"
        args2 = args + ['-o', tmpTrips, '--write-trips']
        if options.junctionTaz:
            args2 += ['--write-trips.junctions']
        if options.verbose:
            print("calling", " ".join(args2))
            sys.stdout.flush()
        subprocess.call(args2)
        sys.stdout.flush()
        os.remove(options.tripfile)  # on windows, rename does not overwrite
        os.rename(tmpTrips, options.tripfile)
        sumolib.xml.insertOptionsHeader(options.tripfile, options)

    if trip_generator and options.weights_outprefix:
        idPrefix = ""
        if options.tripprefix:
            idPrefix = options.tripprefix + "."
        trip_generator.source_generator.write_weights(
            options.weights_outprefix + SOURCE_SUFFIX,
            idPrefix + "src", options.begin, options.end)
        trip_generator.sink_generator.write_weights(
            options.weights_outprefix + DEST_SUFFIX,
            idPrefix + "dst", options.begin, options.end)
        if trip_generator.via_generator:
            trip_generator.via_generator.write_weights(
                options.weights_outprefix + VIA_SUFFIX,
                idPrefix + "via", options.begin, options.end)

    # return wether trips could be generated as requested
    return trip_generator is not None


if __name__ == "__main__":
    try:
        if not main(get_options()):
            print("Error: Trips couldn't be generated as requested. "
                  "Try the --verbose option to output more details on the failure.", file=sys.stderr)
            sys.exit(1)
    except ValueError as e:
        print("Error:", e, file=sys.stderr)
        sys.exit(1)
