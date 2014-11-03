#!/usr/bin/env python
"""
@file    one-shot.py
@author  Daniel Krajzewicz
@author  Jakob Erdmann
@author  Yun-Pang Floetteroed
@author  Michael Behrisch
@date    2008-03-10
@version $Id$

This script does multiple sumo runs with different rerouting intervals.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
import os, sys, subprocess
from datetime import datetime
from optparse import OptionParser
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib

def call(command, log):
    print("-" * 79, file=log)
    print(command, file=log)
    retCode = subprocess.call(command, stdout=log, stderr=log)
    if retCode != 0:
        print("Execution of %s failed. Look into %s for details." % (command, log.name), file=sys.stderr)
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
        print('        <weight-files value="%s"/>' % options.weightfiles, file=fd)

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
        print('        <routing-algorithm value="%s"/>' % options.routingalgorithm, file=fd)
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
</configuration>""" % (step, options.updateInterval, options.withtaz, options.reroutingexplicit, options.lastRoutes, options.withexittime, options.routesorted, not options.withWarnings), file=fd)
    fd.close()
    if options.costmodifier != 'None':
        fd = open("%s_dump_%s.add.xml" % (options.costmodifier, step), "w")
        print("""<a>
        <edgeData id="%s_dump_%s_%s" freq="%s" file="%s_dump_%s_%s.xml" excludeEmpty="true"/>
        </a>""" % (options.costmodifier, step, options.aggregation, options.aggregation, options.costmodifier, step, options.aggregation), file=fd)
        fd.close()
    else:
        fd = open("dump_%s.add.xml" % step, "w")
        print("""<a>
        <edgeData id="dump_%s_%s" freq="%s" file="dump_%s_%s.xml" excludeEmpty="true"/>
        </a>""" % (step, options.aggregation, options.aggregation, step, options.aggregation), file=fd)
        fd.close()

optParser = OptionParser()
optParser.add_option("-W", "--with-warnings", action="store_true", dest="withWarnings",
                     default=False, help="enables warnings")

optParser.add_option("-n", "--net-file", dest="net",
                     help="SUMO network (mandatory)", metavar="FILE")
optParser.add_option("-t", "--trips", dest="trips",
                     help="trips in step 0", metavar="FILE")

optParser.add_option("-b", "--begin", dest="begin",
                     type="int", default=0, help="Set simulation/routing begin")
optParser.add_option("-e", "--end", dest="end",
                     type="int", help="Set simulation/routing end")
optParser.add_option("-R", "--route-steps", dest="routeSteps",
                     type="int", default=200, help="Set simulation route steps")
optParser.add_option("-a", "--aggregation", dest="aggregation",
                     type="int", default=900, help="Set main weights aggregation period")
optParser.add_option("-f", "--frequencies", dest="frequencies",
                     default="-1,1800,300,15", help="Set the frequencies to iterate over")
optParser.add_option("-i", "--adaptation-interval", dest="updateInterval",
                     type="int", default=1, help="Set edge weight adaptation interval")

optParser.add_option("-E", "--disable-summary", "--disable-emissions", action="store_true", dest="noSummary",
                     default=False, help="No summaries are written by the simulation")
optParser.add_option("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                     default=False, help="No tripinfos are written by the simulation")
optParser.add_option("-m", "--mesosim", action="store_true", dest="mesosim",
                     default=False, help="Whether mesosim shall be used")
optParser.add_option("-w", "--with-taz", action="store_true", dest="withtaz",
                     default=False, help="Whether districts shall be used")
optParser.add_option("-+", "--additional", dest="additional",
                     default="", help="Additional files")
optParser.add_option("-L", "--lastRoutes", action="store_true", dest="lastRoutes",
                     default=False, help="only save the last routes in the vehroute-output")
optParser.add_option("-F", "--weight-files", dest="weightfiles",
                     help="Load edge/lane weights from FILE", metavar="FILE")
optParser.add_option("-A", "--routing-algorithm", dest="routingalgorithm", type="choice",
                    choices=('dijkstra', 'astar'),
                    default="astar", help="type of routing algorithm [default: %default]")
optParser.add_option("-r", "--rerouting-explicit", dest="reroutingexplicit", type="string",
                     default = "", help="define the ids of the vehicles that should be re-routed.")
optParser.add_option("-x", "--with-exittime", action="store_true", dest="withexittime",
                    default= False, help="Write the exit times for all edges")
optParser.add_option("-s", "--route-sorted", action="store_true", dest="routesorted",
                    default= False, help="sorts the output by departure time") 
optParser.add_option("-p", "--path", dest="path", help="Path to binaries")
optParser.add_option("--cost-modifier", dest="costmodifier", type="choice",
                     choices=('grohnde', 'isar', 'None'), 
                     default='None', help="Whether to modify link travel costs of the given routes")
(options, args) = optParser.parse_args()

sumo = "sumo"
if options.mesosim:
    sumo = "meso"
sumoBinary = sumolib.checkBinary(sumo, options.path)
if options.costmodifier != 'None':
    pyPath = os.path.abspath(os.path.dirname(sys.argv[0]))
    sys.path.append(os.path.join(pyPath, "..", "..", "..", "..","..", "tools", "kkwSim"))
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
        costModifier(outputfile, step, "dump", options.aggregation, currentDir, options.costmodifier, 'one-shot')
    writeSUMOConf(step, options, options.trips)
    call([sumoBinary, "-c", "one_shot_%s.sumocfg" % step], log)
    etime = datetime.now()
    print(">> End time %s" % etime)
    print("< Step %s ended (duration: %s)" % (step, etime-btime))
    print("------------------\n")
print("one-shot ended (duration: %s)" % (datetime.now() - starttime))

log.close()
