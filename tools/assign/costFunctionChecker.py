#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    costFunctionChecker.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2009-08-31

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import subprocess
from datetime import datetime
from xml.sax import make_parser, handler
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


def call(command, log):
    if not isinstance(command, str):
        command = [str(c) for c in command]
    print("-" * 79, file=log)
    print(command, file=log)
    log.flush()
    retCode = subprocess.call(command, stdout=log, stderr=log)
    if retCode != 0:
        print("Execution of %s failed. Look into %s for details." % (
            command, log.name), file=sys.stderr)
        sys.exit(retCode)


def writeRouteConf(step, options, file, output):
    fd = open("iteration_" + str(step) + ".duarcfg", "w")
    print("""<configuration>
    <input>
        <net-file value="%s"/>""" % options.net, file=fd)
    print('        <route-files value="%s"/>' % file, file=fd)
    if step > 0:
        print('        <weights value="dump_%s_%s.xml"/>' % (
            step - 1, options.aggregation), file=fd)
    print("""    </input>
    <output>
        <output-file value="%s"/>
        <exit-times value="True"/>
    </output>""" % output, file=fd)
    print("""    <processing>
        <continue-on-unbuild value="%s"/>
        <expand-weights value="True"/>
        <gBeta value="%s"/>
        <gA value="%s"/>
    </processing>""" % (options.continueOnUnbuild, options.gBeta, options.gA), file=fd)
    print('    <random_number><abs-rand value="%s"/></random_number>' %
          options.absrand, file=fd)
    print('    <time><begin value="%s"/>' % options.begin, end=' ', file=fd)
    if options.end:
        print('<end value="%s"/>' % options.end, end=' ', file=fd)
    print("""</time>
    <report>
        <verbose value="%s"/>
        <suppress-warnings value="%s"/>
    </report>
</configuration>""" % (options.verbose, options.noWarnings), file=fd)
    fd.close()


class RouteReader(handler.ContentHandler):

    def __init__(self):
        self._edgeWeights = {}
        self._maxDepart = 0

    def startElement(self, name, attrs):
        if name == 'route':
            for edge in attrs['edges'].split():
                if edge not in self._edgeWeights:
                    self._edgeWeights[edge] = 0
                self._edgeWeights[edge] += 1
        elif name == 'vehicle':
            if float(attrs['depart']) > self._maxDepart:
                self._maxDepart = float(attrs['depart'])

    def getWeight(self, edge):
        return self._edgeWeights.get(edge, 0)

    def getMaxDepart(self):
        return self._maxDepart


class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edges = []

    def startElement(self, name, attrs):
        if name == 'edge':
            if 'function' not in attrs or attrs['function'] == 'normal':
                self._edges.append(attrs['id'])

    def getEdges(self):
        return self._edges


def identity(edge, weight):
    return weight


def generateWeights(step, options, edges, weights, costFunction):
    fd = open("dump_%s_%s.xml" % (step, options.aggregation), "w")
    print('<?xml version="1.0"?>\n<netstats>', file=fd)
    for time in range(0, int(reader.getMaxDepart() + 1), options.aggregation):
        print('    <interval begin="%s" end="%s" id="dump_%s">' % (
            time, time + options.aggregation, options.aggregation), file=fd)
        for edge in edges:
            cost = costFunction(edge, weights.getWeight(edge))
            if cost is not None:
                print('        <edge id="%s" traveltime="%s"/>' % (
                    edge, cost), file=fd)
        print('    </interval>', file=fd)
    print('</netstats>', file=fd)
    fd.close()


parser = sumolib.options.ArgumentParser()
parser.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                    default=False, help="tell me what you are doing")
parser.add_argument("--continue-on-unbuild", action="store_true", dest="continueOnUnbuild",
                    default=False, help="continues on unbuild routes")
parser.add_argument("-w", "--disable-warnings", action="store_true", dest="noWarnings",
                    default=False, help="disables warnings")

parser.add_argument("-n", "--net-file", dest="net", type=parser.net_file,
                    help="SUMO network (mandatory)", metavar="FILE")
parser.add_argument("-t", "--trips", dest="trips", type=parser.route_file,
                    help="trips in step 0 (this or flows is mandatory)", metavar="FILE")
parser.add_argument("-F", "--flows", type=parser.route_file,
                    help="flows in step 0 (this or trips is mandatory)", metavar="FILE")
parser.add_argument("-+", "--additional", dest="additional", type=parser.additional_file,
                    default="", help="Additional files")

parser.add_argument("-b", "--begin", dest="begin",
                    type=parser.time, default=0, help="Set simulation/routing begin [default: %(default)s]")
parser.add_argument("-e", "--end", dest="end",
                    type=parser.time, help="Set simulation/routing end [default: %(default)s]")
parser.add_argument("-R", "--route-steps", dest="routeSteps",
                    type=int, default=200, help="Set simulation route steps [default: %(default)s]")
parser.add_argument("-a", "--aggregation", dest="aggregation",
                    type=parser.time, default=900, help="Set main weights aggregation period [default: %(default)s]")
parser.add_argument("-A", "--gA", dest="gA",
                    type=float, default=.5, help="Sets Gawron's Alpha [default: %(default)s]")
parser.add_argument("-B", "--gBeta", dest="gBeta",
                    type=float, default=.9, help="Sets Gawron's Beta [default: %(default)s]")

parser.add_argument("-f", "--first-step", dest="firstStep",
                    type=int, default=0, help="First DUA step [default: %(default)s]")
parser.add_argument("-l", "--last-step", dest="lastStep",
                    type=int, default=50, help="Last DUA step [default: %(default)s]")
parser.add_argument("-p", "--path", dest="path", type=str, help="Path to binaries")

parser.add_argument("-y", "--absrand", dest="absrand", action="store_true",
                    default=False, help="use current time to generate random number")
parser.add_argument("--cost-function", dest="costfunc", type=str,
                    default="identity", help="(python) function to use as cost function")

options = parser.parse_args()

if not options.net or not (options.trips or options.flows):
    parser.error(
        "At least --net-file and --trips or --flows have to be given!")


duaBinary = sumolib.checkBinary("duarouter", options.path)
log = open("dua-log.txt", "w+")

saxparser = make_parser()
reader = NetReader()
saxparser.setContentHandler(reader)
saxparser.parse(options.net)
edges = reader.getEdges()

if "." in options.costfunc:
    idx = options.costfunc.rfind(".")
    module = options.costfunc[:idx]
    func = options.costfunc[idx + 1:]
    exec("from %s import %s as costFunction" % (module, func))
else:
    exec("costFunction = %s" % options.costfunc)

if options.flows:
    tripFiles = options.flows.split(",")
else:
    tripFiles = options.trips.split(",")
starttime = datetime.now()
for step in range(options.firstStep, options.lastStep):
    btimeA = datetime.now()
    print("> Executing step " + str(step))

    # router
    files = []
    for tripFile in tripFiles:
        file = tripFile
        tripFile = os.path.basename(tripFile)
        if step > 0:
            file = tripFile[
                :tripFile.find(".")] + "_%s.rou.alt.xml" % (step - 1)
        output = tripFile[:tripFile.find(".")] + "_%s.rou.xml" % step
        print(">> Running router with " + file)
        btime = datetime.now()
        print(">>> Begin time: %s" % btime)
        writeRouteConf(step, options, file, output)
        retCode = call([duaBinary, "-c", "iteration_%s.duarcfg" % step], log)
        etime = datetime.now()
        print(">>> End time: %s" % etime)
        print(">>> Duration: %s" % (etime - btime))
        print("<<")
        files.append(output)
    # generating weights file
    print(">> Generating weights")
    reader = RouteReader()
    saxparser.setContentHandler(reader)
    for f in files:
        saxparser.parse(f)
    # see evaluation of options.costfunc above
    generateWeights(step, options, edges, reader, costFunction)  # noqa
    print("<<")
    print("< Step %s ended (duration: %s)" % (step, datetime.now() - btimeA))
    print("------------------\n")
    sys.stdout.flush()
print("dua-iterate ended (duration: %s)" % (datetime.now() - starttime))

log.close()
