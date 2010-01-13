#!/usr/bin/env python
"""
@file    one-shot.py
@author  Daniel.Krajzewicz@dlr.de
@date    2008-03-10
@version $Id$

This script does multiple sumo runs with different rerouting intervals.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os, sys, subprocess
from datetime import datetime
from optparse import OptionParser

def call(command, log):
    print >> log, "-" * 79
    print >> log, command
    retCode = subprocess.call(command, stdout=log, stderr=log)
    if retCode != 0:
        print >> sys.stderr, "Execution of %s failed. Look into %s for details." % (command, log.name)
        sys.exit(retCode) 

def writeSUMOConf(step, options, files):
    fd = open("one_shot_" + str(step) + ".sumo.cfg", "w")
    print >> fd, """<configuration>
    <files>
        <net-file value="%s"/>
        <route-files value="%s"/>
        <vehroutes value="vehroutes_%s.xml"/>""" % (options.net, files, step)
    if not options.noEmissions:
        print >> fd, '        <emissions value="emissions_%s.xml"/>' % step
    if not options.noTripinfo:
        print >> fd, '        <tripinfo value="tripinfo_%s.xml"/>' % step
    add = 'dump_%s.add.xml' % step
    if options.additional:
        add += "," + options.additional
    print >> fd, """        <additional-files value="%s"/>
    </files>
    <process>
        <begin value="%s"/>
        <route-steps value="%s"/>""" % (add, options.begin, options.routeSteps)
    if options.end:
        print >> fd, '        <end value="%s"/>' % options.end
    if options.mesosim:
        print >> fd, '        <mesosim value="True"/>'
    print >> fd, """        <device.routing.probability value="1"/>
        <device.routing.period value="%s"/>
        <device.routing.adaptation-interval value="%s"/>
    </process>
    <reports>
        <verbose value="True"/>
        <suppress-warnings value="%s"/>
    </reports>
</configuration>""" % (step, options.updateInterval, not options.withWarnings)
    fd.close()
    fd = open("dump_%s.add.xml" % step, "w")
    print >> fd, """<a>
    <meandata-edge id="dump_%s_%s" freq="%s" file="dump_%s_%s.xml" excludeEmpty="true"/>
</a>""" % (step, options.aggregation, options.aggregation, step, options.aggregation)
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

optParser.add_option("-E", "--disable-emissions", action="store_true", dest="noEmissions",
                     default=False, help="No emissions are written by the simulation")
optParser.add_option("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                     default=False, help="No tripinfos are written by the simulation")
optParser.add_option("-m", "--mesosim", action="store_true", dest="mesosim",
                     default=False, help="Whether mesosim shall be used")
optParser.add_option("-+", "--additional", dest="additional",
                     default="", help="Additional files")


optParser.add_option("-p", "--path", dest="path",
                     default=os.environ.get("SUMO", ""), help="Path to binaries")
(options, args) = optParser.parse_args()

sumo = "sumo"
if options.mesosim:
    sumo = "meso"
if os.path.isfile(options.path):
    sumoBinary = options.path
elif (sys.platform=="win32"):        
    sumoBinary = os.path.join(options.path, sumo+".exe")
else:
    sumoBinary = os.path.join(options.path, sumo)
log = open("one_shot-log.txt", "w")
starttime = datetime.now()
for step in options.frequencies.split(","):
    step = int(step)
    print "> Running simulation with update frequency %s" % step
    btime = datetime.now()
    print ">> Begin time %s" % btime
    writeSUMOConf(step, options, options.trips)
    call("%s -c one_shot_%s.sumo.cfg" % (sumoBinary, step), log)
    etime = datetime.now()
    print ">> End time %s" % etime
    print "< Step %s ended (duration: %s)" % (step, etime-btime)
    print "------------------\n"
print "one-shot ended (duration: %s)" % (datetime.now() - starttime)

log.close()
