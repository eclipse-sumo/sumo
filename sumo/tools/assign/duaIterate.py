#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    duaIterate.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@author  Jakob Erdmann
@author  Yun-Pang Wang
@date    2008-02-13
@version $Id$

Run duarouter and sumo alternating to perform a dynamic user assignment.
Based on the Perl script dua_iterate.pl.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, sys, subprocess, types
import StringIO
from datetime import datetime
from optparse import OptionParser

def initOptions():
    optParser = OptionParser()
    optParser.add_option("-w", "--disable-warnings", action="store_true", dest="noWarnings",
                         default=False, help="disables warnings")
    optParser.add_option("-n", "--net-file", dest="net",
                         help="SUMO network (mandatory)", metavar="FILE")
    optParser.add_option("-+", "--additional", dest="additional",
                         default="", help="Additional files")
    optParser.add_option("-b", "--begin",
                         type="int", default=0, help="Set simulation/routing begin [default: %default]")
    optParser.add_option("-e", "--end",
                         type="int", help="Set simulation/routing end [default: %default]")
    optParser.add_option("-R", "--route-steps", dest="routeSteps",
                         type="int", default=200, help="Set simulation route steps [default: %default]")
    optParser.add_option("-a", "--aggregation",
                         type="int", default=900, help="Set main weights aggregation period [default: %default]")
    optParser.add_option("-m", "--mesosim", action="store_true",
                         default=False, help="Whether mesosim shall be used")
    optParser.add_option("-p", "--path",
                         default=os.environ.get("SUMO_BINDIR", ""), help="Path to binaries [default: %default]")
    optParser.add_option("-y", "--absrand", action="store_true",
                         default= False, help="use current time to generate random number")
    optParser.add_option("-I", "--nointernal-link", action="store_true", dest="internallink",
                         default = False, help="not to simulate internal link: true or false")
    optParser.add_option("-L", "--lanechange-allowed", dest="lanechangeallowed", action="store_true",
                         default = False, help="lane change allowed to swap")
    optParser.add_option("-j", "--meso-junctioncontrol", dest="mesojunctioncontrol", action="store_true",
                         default = False, help="Enable mesoscopic traffic light and priority junciton handling")
    optParser.add_option("-q", "--meso-multiqueue", dest="mesomultiqueue", action="store_true",
                         default = False, help="Enable multiple queues at edge ends")
    optParser.add_option("-Q", "--eco-measure", dest="ecomeasure", type="choice",
                         choices=('CO', 'CO2', 'PMx', 'HC', 'NOx', 'fuel', 'noise'),
                         help="define the applied eco measure, e.g. fuel, CO2, noise")
    optParser.add_option("-s", "--sloppy-insert", action="store_true",
                         default=False, help="sloppy insertion tests (may speed up the sim considerably)")
    return optParser

def call(command, log):
    command = [str(c) for c in command]
    print >> log, "-" * 79
    print >> log, command
    log.flush()
    retCode = subprocess.call(command, stdout=log, stderr=log)
    if retCode != 0:
        print >> sys.stderr, "Execution of %s failed. Look into %s for details." % (command, log.name)
        sys.exit(retCode) 

def writeRouteConf(step, options, file, output, routesInfo, initial_type):
    filename = os.path.basename(file)
    filename = filename.split('.')[0]
    cfgname = "iteration_%03i_%s.duarcfg" % (step, filename)
    withExitTimes = False
    if routesInfo == "detailed":
        withExitTimes = True
    fd = open(cfgname, "w")
    print >> fd, """<configuration>
    <input>
        <net-file value="%s"/>""" % options.net
    if options.districts:
        print >> fd, '        <taz-files value="%s"/>' % options.districts
    if step==0:
        print >> fd, '        <%s-files value="%s"/>' % (initial_type, file)
    else:
        print >> fd, '        <alternative-files value="%s"/>' % file
        print >> fd, '        <weights value="dump_%03i_%s.xml"/>' % (step-1, options.aggregation)
    if options.ecomeasure:
        print >> fd, '        <weight-attribute value="%s"/>' % options.ecomeasure
    print >> fd, """    </input>
    <output>
        <output-file value="%s"/>
        <exit-times value="%s"/>
    </output>""" % (output, withExitTimes)
    print >> fd, """    <processing>
        <ignore-errors value="%s"/>
        <with-taz value="%s"/>
        <gawron.beta value="%s"/>
        <gawron.a value="%s"/>
        <keep-all-routes value="%s"/>
    </processing>""" % (options.continueOnUnbuild, bool(options.districts), options.gBeta, options.gA, options.allroutes)
    print >> fd, '    <random_number><random value="%s"/></random_number>' % options.absrand
    print >> fd, '    <time><begin value="%s"/>' % options.begin,
    if options.end:
        print >> fd, '<end value="%s"/>' % options.end,
    print >> fd, """</time>
    <report>
        <verbose value="True"/>
        <no-warnings value="%s"/>
    </report>
</configuration>""" % options.noWarnings
    fd.close()
    return cfgname

def writeSUMOConf(step, options, files):
    fd = open("iteration_%03i.sumocfg" % step, "w")
    add = ""
    if options.additional != "":
        add = "," + options.additional
    print >> fd, """<configuration>
    <input>
        <net-file value="%s"/>
        <route-files value="%s"/>
        <additional-files value="dua_dump_%03i.add.xml%s"/>
    </input>
    <output>""" % (options.net, files, step, add)
    if hasattr(options, "noSummary") and not options.noSummary:
        print >> fd, '        <summary-output value="summary_%03i.xml"/>' % step
    if hasattr(options, "noTripinfo") and not options.noTripinfo:
        print >> fd, '        <tripinfo-output value="tripinfo_%03i.xml"/>' % step
        if hasattr(options, "ecomeasure"):
            print >> fd, '        <device.hbefa.probability value="1"/>'
    if hasattr(options, "routefile"):
        if options.routefile == "routesonly":
            print >> fd, '        <vehroute-output value="vehroute_%03i.xml"/>' % step
        elif options.routefile == "detailed":
            print >> fd, '        <vehroute-output value="vehroute_%03i.xml"/>' % step
            print >> fd, '        <vehroute-output.exit-times value="True"/>'
    if hasattr(options, "lastroute") and options.lastroute:
        print >> fd, '          <vehroute-output.last-route value="%s"/>' % options.lastroute
    print >> fd, "    </output>"
    print >> fd, '    <random_number><random value="%s"/></random_number>' % options.absrand
    print >> fd, '    <time><begin value="%s"/>' % options.begin,
    if hasattr(options, "timeInc") and options.timeInc:
        print >> fd, '<end value="%s"/>' % int(options.timeInc * (step + 1)),
    elif options.end:
        print >> fd, '<end value="%s"/>' % options.end,
    print >> fd, """</time>
    <processing>
        <route-steps value="%s"/>""" % options.routeSteps
    print >> fd, '        <no-internal-links value="%s"/>' % options.internallink
    print >> fd, '        <lanechange.allow-swap value="%s"/>' % options.lanechangeallowed
    print >> fd, '        <sloppy-insert value="%s"/>' % options.sloppy_insert
    if hasattr(options, "incBase") and options.incBase > 0:
        print >> fd, '        <scale value="%s"/>' % (options.incValue*float(step+1) / options.incBase)
    if options.mesosim:
        print >> fd, '        <mesosim value="True"/>'
        if options.mesomultiqueue:
            print >> fd, '        <meso-multi-queue value="True"/>'
        if options.mesojunctioncontrol:
            print >> fd, '        <meso-junction-control value="True"/>'
    print >> fd, """    </processing>
    <report>
        <verbose value="True"/>
        <no-warnings value="%s"/>
    </report>
</configuration>""" % options.noWarnings
    fd.close()
    suffix = "_%03i_%s" % (step, options.aggregation)
    fd = open("dua_dump_%03i.add.xml" % step, "w")
    print >> fd, "<a>"
    print >> fd, '    <edgeData id="dump%s" freq="%s" file="dump%s.xml"/>' % (suffix, options.aggregation, suffix)
    if options.ecomeasure:
        print >> fd, '    <edgeData id="eco%s" type="hbefa" freq="%s" file="dump%s.xml"/>' % (suffix, options.aggregation, suffix)
    print >> fd, "</a>"
    fd.close()

def main():
    optParser = initOptions()
    optParser.add_option("-C", "--continue-on-unbuild", action="store_true", dest="continueOnUnbuild",
                         default=False, help="continues on unbuild routes")
    optParser.add_option("-t", "--trips",
                         help="trips in step 0 (either trips, flows, or routes have to be supplied)", metavar="FILE")
    optParser.add_option("-r", "--routes",
                         help="routes in step 0 (either trips, flows, or routes have to be supplied)", metavar="FILE")
    optParser.add_option("-F", "--flows",
                         help="flows in step 0 (either trips, flows, or routes have to be supplied)", metavar="FILE")
    optParser.add_option("-A", "--gA",
                         type="float", default=.5, help="Sets Gawron's Alpha [default: %default]")
    optParser.add_option("-B", "--gBeta",
                         type="float", default=.9, help="Sets Gawron's Beta [default: %default]")
    optParser.add_option("-E", "--disable-summary", "--disable-emissions", action="store_true", dest="noSummary",
                         default=False, help="No summaries are written by the simulation")
    optParser.add_option("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                         default=False, help="No tripinfos are written by the simulation")
    optParser.add_option("--inc-base", dest="incBase",
                         type="int", default=-1, help="Give the incrementation base")
    optParser.add_option("--incrementation", dest="incValue",
                         type="int", default=1, help="Give the incrementation")
    optParser.add_option("--time-inc", dest="timeInc",
                         type="int", default=0, help="Give the time incrementation")
    optParser.add_option("-f", "--first-step", dest="firstStep",
                         type="int", default=0, help="First DUA step [default: %default]")
    optParser.add_option("-l", "--last-step", dest="lastStep",
                         type="int", default=50, help="Last DUA step [default: %default]")
    optParser.add_option("-D", "--districts", help="use districts as sources and targets", metavar="FILE")
    optParser.add_option("-x", "--vehroute-file",  dest="routefile", type="choice",
                         choices=('None', 'routesonly', 'detailed'), 
                         default = 'None', help="choose the format of the route file")
    optParser.add_option("-z", "--output-lastRoute",  action="store_true", dest="lastroute",
                         default = False, help="output the last routes")
    optParser.add_option("-K", "--keep-allroutes", action="store_true", dest="allroutes",
                         default = False, help="save routes with near zero probability")
    
    (options, args) = optParser.parse_args()
    if not options.net:
        optParser.error("Option --net-file is mandatory")
    if (not options.trips and not options.routes and not options.flows) or (options.trips and options.routes):
        optParser.error("Either --trips, --flows, or --routes have to be given!")
    duaBinary = os.environ.get("DUAROUTER_BINARY", os.path.join(options.path, "duarouter"))
    if options.mesosim:
        sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "meso"))
    else:
        sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "sumo"))

    # make sure BOTH binaries are callable before we start
    try:
        subprocess.call(duaBinary, stdout=subprocess.PIPE)
    except OSError:
        sys.exit("Error: Could not locate duarouter.\nMake sure its on the search path or set environment variable DUAROUTER_BINARY\n")
    try:
        subprocess.call(sumoBinary, stdout=subprocess.PIPE)
    except OSError:
        sys.exit("Error: Could not locate sumo.\nMake sure its on the search path or set environment variable SUMO_BINARY\n")

    
    log = open("dua-log.txt", "w+")
    starttime = datetime.now()
    if options.trips:
        input_demands = options.trips.split(",")
        initial_type = "trip"
    elif options.flows:
        input_demands = options.flows.split(",")
        initial_type = "flow"
    else:
        input_demands = options.routes.split(",")
        initial_type = "route"
    
    for step in range(options.firstStep, options.lastStep):
        btimeA = datetime.now()
        print "> Executing step %s" % step
        
        # dua-router
        files = []
        for demand_file in input_demands:
            basename = os.path.basename(demand_file)
            basename = basename[:basename.find(".")]
            output =  basename + "_%03i.rou.xml" % step
            if step>0:
                # output of previous step
                demand_file = basename + "_%03i.rou.alt.xml" % (step-1)
    
            print ">> Running router"
            btime = datetime.now()
            print ">>> Begin time: %s" % btime
            cfgname = writeRouteConf(step, options, demand_file, output, options.routefile, initial_type)
            call([duaBinary, "-c", cfgname], log)
            etime = datetime.now()
            print ">>> End time: %s" % etime
            print ">>> Duration: %s" % (etime-btime)
            print "<<"
            files.append(output)
    
        # simulation
        print ">> Running simulation"
        btime = datetime.now()
        print ">>> Begin time: %s" % btime
        writeSUMOConf(step, options, ",".join(files))
        call([sumoBinary, "-c", "iteration_%03i.sumocfg" % step], log)
        etime = datetime.now()
        print ">>> End time: %s" % etime
        print ">>> Duration: %s" % (etime-btime)
        print "<<"
    
        print "< Step %s ended (duration: %s)" % (step, datetime.now() - btimeA)
        print "------------------\n"
        log.flush()
    print "dua-iterate ended (duration: %s)" % (datetime.now() - starttime)
    
    log.close()

if __name__ == "__main__":
    main()
