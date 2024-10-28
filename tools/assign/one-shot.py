#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    one-shot.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Yun-Pang Floetteroed
# @author  Michael Behrisch
# @author  Mirko Barthauer
# @date    2008-03-10

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess
from datetime import datetime
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


def call(command, log):
    print("-" * 79, file=log)
    print(command, file=log)
    retCode = subprocess.call(command, stdout=log, stderr=log)
    if retCode != 0:
        print("Execution of %s failed. Look into %s for details." %
              (command, log.name), file=sys.stderr)
        sys.exit(retCode)


def writeSUMOConf(step, options, files):
    fd = open("one_shot_" + str(step) + ".sumocfg", "w")
    print("""<configuration>
    <files>
        <net-file value="%s"/>
        <route-files value="%s"/>
        <vehroutes value="vehroutes_%s.xml"/>""" % (options.net, files, step), file=fd)
    if not options.noSummary:
        print('        <summary value="summary_%s.xml"/>' % step, file=fd)
    if not options.noTripinfo:
        print('        <tripinfo value="tripinfo_%s.xml"/>' % step, file=fd)
    if options.weightfiles:
        print('        <weight-files value="%s"/>' %
              options.weightfiles, file=fd)

    add = 'dump_%s.add.xml' % step
    if options.costmodifier != 'None':
        add = '%s_dump_%s.add.xml' % (options.costmodifier, step)
    if options.additional:
        add += "," + options.additional
    print("""        <additional-files value="%s"/>
    </files>
    <process>
        <begin value="%s"/>
        <route-steps value="%s"/>""" % (add, options.begin, options.routeSteps), file=fd)

    if options.end:
        print('        <end value="%s"/>' % options.end, file=fd)
    if options.mesosim:
        print('        <mesosim value="True"/>', file=fd)
    if options.routingalgorithm:
        print('        <routing-algorithm value="%s"/>' %
              options.routingalgorithm, file=fd)
    print("""        <device.rerouting.probability value="1"/>
        <device.rerouting.period value="%s"/>
        <device.rerouting.adaptation-interval value="%s"/>
        <device.rerouting.with-taz value="%s"/>
        <device.rerouting.explicit value="%s"/>
        <vehroute-output.last-route value="%s"/>
        <vehroute-output.exit-times value="%s"/>
        <vehroute-output.sorted value="%s"/>
    </process>
    <reports>
        <verbose value="True"/>
        <no-warnings value="%s"/>
    </reports>
</configuration>""" % (step, options.updateInterval, options.withtaz, options.reroutingexplicit, options.lastRoutes,
                       options.withexittime, options.routesorted, not options.withWarnings), file=fd)
    fd.close()
    if options.costmodifier != 'None':
        fd = open("%s_dump_%s.add.xml" % (options.costmodifier, step), "w")
        print("""<a>
        <edgeData id="%s_dump_%s_%s" freq="%s" file="%s_dump_%s_%s.xml" excludeEmpty="true"/>
        </a>""" % (options.costmodifier, step, options.aggregation, options.aggregation, options.costmodifier, step,
                   options.aggregation), file=fd)
        fd.close()
    else:
        fd = open("dump_%s.add.xml" % step, "w")
        print("""<a>
        <edgeData id="dump_%s_%s" freq="%s" file="dump_%s_%s.xml" excludeEmpty="true"/>
        </a>""" % (step, options.aggregation, options.aggregation, step, options.aggregation), file=fd)
        fd.close()


parser = sumolib.options.ArgumentParser()
parser.add_argument("-W", "--with-warnings", action="store_true", dest="withWarnings",
                    default=False, help="enables warnings")

parser.add_argument("-n", "--net-file", dest="net", category="input", type=parser.net_file,
                    help="SUMO network (mandatory)", metavar="FILE")
parser.add_argument("-t", "--trips", dest="trips", category="input", type=parser.route_file,
                    help="trips in step 0", metavar="FILE")

parser.add_argument("-b", "--begin", dest="begin",
                    type=parser.time, default=0, help="Set simulation/routing begin")
parser.add_argument("-e", "--end", dest="end",
                    type=parser.time, help="Set simulation/routing end")
parser.add_argument("-R", "--route-steps", dest="routeSteps",
                    type=int, default=200, help="Set simulation route steps")
parser.add_argument("-a", "--aggregation", dest="aggregation",
                    type=parser.time, default=900, help="Set main weights aggregation period")
parser.add_argument("-f", "--frequencies", dest="frequencies", type=str,
                    default="-1,1800,300,15", help="Set the frequencies to iterate over")
parser.add_argument("-i", "--adaptation-interval", dest="updateInterval",
                    type=parser.time, default=1, help="Set edge weight adaptation interval")

parser.add_argument("-E", "--disable-summary", "--disable-emissions", action="store_true", dest="noSummary",
                    default=False, category="output", help="No summaries are written by the simulation")
parser.add_argument("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                    default=False, help="No tripinfos are written by the simulation")
parser.add_argument("-m", "--mesosim", action="store_true", dest="mesosim",
                    default=False, help="Whether mesosim shall be used")
parser.add_argument("-w", "--with-taz", action="store_true", dest="withtaz",
                    default=False, help="Whether districts shall be used")
parser.add_argument("-+", "--additional", dest="additional", category="input", type=parser.additional_file,
                    default="", help="Additional files")
parser.add_argument("-L", "--lastRoutes", action="store_true", dest="lastRoutes",
                    default=False, help="only save the last routes in the vehroute-output")
parser.add_argument("-F", "--weight-files", dest="weightfiles", type=parser.file,
                    help="Load edge/lane weights from FILE", metavar="FILE")
parser.add_argument("-A", "--routing-algorithm", dest="routingalgorithm", type=str,
                    choices=('dijkstra', 'astar'),
                    default="astar", help="type of routing algorithm [default: %(default)s]")
parser.add_argument("-r", "--rerouting-explicit", dest="reroutingexplicit", type=str,
                    default="", help="define the ids of the vehicles that should be re-routed.")
parser.add_argument("-x", "--with-exittime", action="store_true", dest="withexittime",
                    default=False, help="Write the exit times for all edges")
parser.add_argument("-s", "--route-sorted", action="store_true", dest="routesorted",
                    default=False, help="sorts the output by departure time")
parser.add_argument("-p", "--path", dest="path", type=parser.file, help="Path to binaries")
parser.add_argument("--cost-modifier", dest="costmodifier", type=str,
                    choices=('grohnde', 'isar', 'None'),
                    default='None', help="Whether to modify link travel costs of the given routes")
options = parser.parse_args()

sumo = "sumo"
if options.mesosim:
    sumo = "meso"
sumoBinary = sumolib.checkBinary(sumo, options.path)
if options.costmodifier != 'None':
    pyPath = os.path.abspath(os.path.dirname(sys.argv[0]))
    sys.path.append(
        os.path.join(pyPath, "..", "..", "..", "..", "..", "tools", "kkwSim"))
    from kkwCostModifier import costModifier
    print('use the cost modifier')

log = open("one_shot-log.txt", "w")
starttime = datetime.now()
for step in options.frequencies.split(","):
    step = int(step)
    print("> Running simulation with update frequency %s" % step)
    btime = datetime.now()
    print(">> Begin time %s" % btime)
    if options.costmodifier != 'None':
        currentDir = os.getcwd()
        print(options.costmodifier)
        outputfile = '%s_weights_%s.xml' % (options.costmodifier, step)
        costModifier(outputfile, step, "dump", options.aggregation,
                     currentDir, options.costmodifier, 'one-shot')
    writeSUMOConf(step, options, options.trips)
    call([sumoBinary, "-c", "one_shot_%s.sumocfg" % step], log)
    etime = datetime.now()
    print(">> End time %s" % etime)
    print("< Step %s ended (duration: %s)" % (step, etime - btime))
    print("------------------\n")
print("one-shot ended (duration: %s)" % (datetime.now() - starttime))

log.close()
