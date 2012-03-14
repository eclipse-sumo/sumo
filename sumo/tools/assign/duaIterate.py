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
import os, sys, subprocess, types, shutil
import StringIO
from datetime import datetime
from optparse import OptionParser
from routeChoices import getRouteChoices


def addGenericOptions(optParser):
    # add options which are used by duaIterate and cadytsIterate
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
    optParser.add_option("--meso-recheck", dest="mesorecheck", type="int", default=0, 
                         help="Delay before checking whether a jam is gone. (higher values can lead to a big speed increase)")
    optParser.add_option("-Q", "--eco-measure", dest="ecomeasure", type="choice",
                         choices=('CO', 'CO2', 'PMx', 'HC', 'NOx', 'fuel', 'noise'),
                         help="define the applied eco measure, e.g. fuel, CO2, noise")
    optParser.add_option("-s", "--sloppy-insert", action="store_true",
                         default=False, help="sloppy insertion tests (may speed up the sim considerably)")
    optParser.add_option("--time-to-teleport", dest="timetoteleport", type="int", default=300,
                         help="Delay before blocked vehicles are teleported where -1 means no teleporting")

def initOptions():
    optParser = OptionParser()
    addGenericOptions(optParser)

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
    optParser.add_option("--tripinfo-filter", dest="tripinfoFilter",
                         help="filter tripinfo attributes")
    optParser.add_option("--inc-start", dest="incStart",
                         type="float", default=0, help="Start for incrementing scale")
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
    optParser.add_option("--routing-algorithm", default="dijkstra", help="select the routing algorithm")
    optParser.add_option("--max-alternatives", default=5, help="prune the number of alternatives to INT")
    optParser.add_option("--skip-first-routing", action="store_true", dest="skipFirstRouting",
                         default=False, help="run simulation with demands before first routing")
    optParser.add_option("--logit", action="store_true", dest="logit",
                         default=False, help="use the logit model for route choice")
    optParser.add_option("-g", "--logitbeta", type="float", dest="logitbeta",
                         default=0.15, help="use the c-logit model for route choice; logit model when beta = 0")
    optParser.add_option("-i", "--logitgamma", type="float", dest="logitgamma",
                         default= 1., help="use the c-logit model for route choice")
    optParser.add_option("-G", "--logittheta", type="float", dest="logittheta",
                         help="parameter to adapt the cost unit")
    optParser.add_option("-J", "--addweights", dest="addweights",
                         help="Additional weightes for duarouter")
    optParser.add_option("--router-verbose", action="store_true",
                         default=False, help="let duarouter print some statistics")
    optParser.add_option("-M", "--external-gawron", action="store_true", dest="externalgawron",
                         default=False, help="use the external gawron calculation")    
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
    addweights = ""
    if options.addweights:
        addweights = options.addweights + ","
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
        print >> fd, '        <weights value="%sdump_%03i_%s.xml"/>' % (addweights, step-1, options.aggregation)
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
        <routing-algorithm value="%s"/>%s
        <max-alternatives value="%s"/>
        <logit value="%s"/>
        <logit.beta value="%s"/>
        <logit.gamma value="%s"/>""" % (
                options.continueOnUnbuild, 
                bool(options.districts), 
                options.gBeta, 
                options.gA, 
                options.allroutes, 
                options.routing_algorithm, 
                ("" if options.routing_algorithm != 'CH' else '\n<weight-period value="%s"/>\n' % options.aggregation),
                options.max_alternatives, 
                options.logit, 
                options.logitbeta, 
                options.logitgamma)
    if options.logittheta:
        print >> fd, '        <logit.theta value="%s"/>' % options.logittheta
    print >> fd, '    </processing>'
                
    print >> fd, '    <random_number><random value="%s"/></random_number>' % options.absrand
    print >> fd, '    <time><begin value="%s"/>' % options.begin,
    if options.end:
        print >> fd, '<end value="%s"/>' % options.end,
    print >> fd, """</time>
    <report>
        <verbose value="%s"/>
        <no-warnings value="%s"/>
    </report>
</configuration>""" % (options.router_verbose, options.noWarnings)
    fd.close()
    return cfgname

def get_scale(options, step):
    # compute scaling factor for simulation
    # using incValue = 1 (default) and incBase = 10 would produce 
    # iterations with increasing scale 0.1, 0.2, ... 0.9, 1, 1, 1, ...
    return min(options.incStart + options.incValue * float(step + 1) / options.incBase, 1)


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
    print >> fd, '        <no-step-log value="True"/>'
    if hasattr(options, "noSummary") and not options.noSummary:
        print >> fd, '        <summary-output value="summary_%03i.xml"/>' % step
    if hasattr(options, "noTripinfo") and not options.noTripinfo:
        print >> fd, '        <tripinfo-output value="tripinfo_%03i.xml"/>' % step
        if options.ecomeasure:
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
    print >> fd, '        <time-to-teleport value="%s"/>' % options.timetoteleport
    if hasattr(options, "incBase") and options.incBase > 0:
        print >> fd, '        <scale value="%s"/>' % get_scale(options, step)
    if options.mesosim:
        print >> fd, '        <mesosim value="True"/>'
        print >> fd, '        <meso-recheck value="%s"/>' % options.mesorecheck
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
    print >> fd, '    <edgeData id="dump%s" freq="%s" file="dump%s.xml" excludeEmpty="true" minSamples="1"/>' % (suffix, options.aggregation, suffix)
    if options.ecomeasure:
        print >> fd, '    <edgeData id="eco%s" type="hbefa" freq="%s" file="dump%s.xml" excludeEmpty="true" minSamples="1"/>' % (suffix, options.aggregation, suffix)
    print >> fd, "</a>"
    fd.close()

def filterTripinfo(step, attrs):
    attrs.add("id")
    inFile = "tripinfo_%03i.xml" % step
    if os.path.exists(inFile):
        out = open(inFile + ".filtered", 'w')
        print >> out, "<tripinfos>"
        hadOutput = False
        for line in open(inFile):
            if "<tripinfo " in line:
                if hadOutput:
                    print >> out, "/>"
                print >> out, "    <tripinfo",
                for a in attrs:
                    pos = line.find(a)
                    if pos >= 0:
                        pos += len(a)+2
                        print >> out, '%s="%s"' % (a, line[pos:line.find('"', pos)]),
                hadOutput = True
            if "<emission" in line:
                for a in attrs:
                    pos = line.find(a)
                    if pos >= 0:
                        pos += len(a)+2
                        print >> out, '%s="%s"' % (a, line[pos:line.find('"', pos)]),
        if hadOutput:
            print >> out, "/>"
        print >> out, "</tripinfos>"
        out.close()
        os.remove(inFile)
        os.rename(out.name, inFile)

def main(args=None):
    optParser = initOptions()
    
    (options, args) = optParser.parse_args(args=args)
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
    if options.externalgawron:#debug
        print 'use externalgawron'
        edgesMap = {}
    for step in range(options.firstStep, options.lastStep):
        btimeA = datetime.now()
        print "> Executing step %s" % step
        
        # dua-router
        if options.skipFirstRouting and step == 0:
            files = input_demands
        else:
            files = []
            for demand_file in input_demands:
                absPath = os.path.abspath(demand_file)
                basename = os.path.basename(demand_file)
                if 'alt' in basename:
                    basename = basename[:-12]
                elif 'trips' in basename:
                    basename = basename[:-10]
                #basename = basename[:basename.find(".")]
                output =  basename + "_%03i.rou.xml" % step

                if step > 0 and not (options.skipFirstRouting and step == 1):
                    # output of previous step
                    demand_file = basename + "_%03i.rou.alt.xml" % (step-1)
        
                print ">> Running router"
                btime = datetime.now()
                print ">>> Begin time: %s" % btime
                cfgname = writeRouteConf(step, options, demand_file, output, options.routefile, initial_type)
                log.flush()
                call([duaBinary, "-c", cfgname], log)
                etime = datetime.now()
                print ">>> End time: %s" % etime
                print ">>> Duration: %s" % (etime-btime)
                print "<<"
                # use the external gawron
                if options.externalgawron:
                    if step == 1 and options.skipFirstRouting:
                        shutil.copy(basename + "_001.rou.alt.xml", basename + "_001.rou.galt.xml")
                        shutil.copy(basename + "_001.rou.xml", basename + "_001.grou.xml")
                    if step == 0 and not options.skipFirstRouting:
                        shutil.copy(basename + "_000.rou.alt.xml", basename + "_000.rou.galt.xml")
                        shutil.copy(basename + "_000.rou.xml", basename + "_000.grou.xml")
                    else:
                        print 'step:', step
                        print 'get externalgawron'
                        dumpfile = "dump_%03i_%s.xml" % (step-1, options.aggregation)
                        ecomeasure = None
                        if options.ecomeasure:
                            ecomeasure = options.ecomeasure
                        if (not options.skipFirstRouting) or (options.skipFirstRouting and step > 1):
                            output, edgesMap = getRouteChoices(edgesMap,dumpfile,basename + "_%03i.rou.alt.xml" % step,options.net,options.addweights, options.gA, options.gBeta,step,ecomeasure)
                files.append(output)
                
        # simulation
        print ">> Running simulation"
        btime = datetime.now()
        print ">>> Begin time: %s" % btime
        writeSUMOConf(step, options, ",".join(files))   #  todo: change 'grou.xml'
        log.flush()
        call([sumoBinary, "-c", "iteration_%03i.sumocfg" % step], log)
        if options.tripinfoFilter:
            filterTripinfo(step, set(options.tripinfoFilter.split(",")))
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
