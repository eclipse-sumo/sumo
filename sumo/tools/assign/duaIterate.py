#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    duaIterate.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@author  Jakob Erdmann
@author  Yun-Pang Floetteroed
@date    2008-02-13
@version $Id$

Run duarouter and sumo alternating to perform a dynamic user assignment.
Based on the Perl script dua_iterate.pl.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import print_function
import os, sys, subprocess, types, shutil, glob
from datetime import datetime
from optparse import OptionParser
from costMemory import CostMemory

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib
from sumolib.options import get_long_option_names


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
    optParser.add_option("-p", "--path", help="Path to binaries")
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
    optParser.add_option("--eager-insert", action="store_true",
                         default=False, help="eager insertion tests (may slow down the sim considerably)")
    optParser.add_option("--time-to-teleport", dest="timetoteleport", type="int", default=300,
                         help="Delay before blocked vehicles are teleported where -1 means no teleporting")
    optParser.add_option("--time-to-teleport.highways", dest="timetoteleport_highways", type="int", default=0,
                         help="Delay before blocked vehicles are teleported on wrong highway lanes")
    optParser.add_option("--cost-modifier", dest="costmodifier", type="choice",
                         choices=('grohnde', 'isar', 'None'), 
                         default='None', help="Whether to modify link travel costs of the given routes")
    optParser.add_option("-7", "--zip", action="store_true",
                         default=False, help="zip old iterations using 7zip")

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
    optParser.add_option("--inc-max", dest="incMax",
                         type="float", default=1, help="Maximum for incrementing scale")
    optParser.add_option("--inc-base", dest="incBase",
                         type="int", default=-1, help="Give the incrementation base. Negative values disable incremental scaling")
    optParser.add_option("--incrementation", dest="incValue",
                         type="int", default=1, help="Give the incrementation")
    optParser.add_option("--time-inc", dest="timeInc",
                         type="int", default=0, help="Give the time incrementation")
    optParser.add_option("-f", "--first-step", dest="firstStep",
                         type="int", default=0, help="First DUA step [default: %default]")
    optParser.add_option("-l", "--last-step", dest="lastStep",
                         type="int", default=50, help="Last DUA step [default: %default]")
    optParser.add_option("--convergence-iterations", dest="convIt",
                         type="int", default=10, help="Number of iterations to use for convergence calculation [default: %default]")
    optParser.add_option("--max-convergence-deviation", dest="convDev",
                         type="float", help="Maximum relative standard deviation in travel times [default: %default]")
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
    optParser.add_option("-N", "--calculate-oldprob", action="store_true", dest="caloldprob",
                         default=False, help="calculate the old route probabilities with the free-flow travel times when using the external gawron calculation")   
    optParser.add_option("--weight-memory", action="store_true", default=False, dest="weightmemory",
                         help="smooth edge weights across iterations")    
    optParser.add_option("--pessimism", default=1, type="float", help="give traffic jams a higher weight")
    optParser.add_option("--clean-alt", action="store_true", dest="clean_alt",
                         default=False, help="Whether old rou.alt.xml files shall be removed")
    optParser.add_option("--binary", action="store_true",
                         default=False, help="Use binary format for intermediate and resulting route files")
    return optParser

def call(command, log):
    command = [str(c) for c in command]
    print("-" * 79, file=log)
    print(command, file=log)
    log.flush()
    retCode = subprocess.call(command, stdout=log, stderr=log)
    if retCode != 0:
        print("Execution of %s failed. Look into %s for details." % (command, log.name), file=sys.stderr)
        sys.exit(retCode) 

def writeRouteConf(step, options, file, output, routesInfo, initial_type):
    filename = os.path.basename(file)
    filename = filename.split('.')[0]
    cfgname = "iteration_%03i_%s.duarcfg" % (step, filename)
    withExitTimes = False
    if routesInfo == "detailed":
        withExitTimes = True
    fd = open(cfgname, "w")
    print("""<configuration>
    <input>
        <net-file value="%s"/>""" % options.net, file=fd)
    if options.districts:
        print('        <taz-files value="%s"/>' % options.districts, file=fd)
    if step==0:
        print('        <%s-files value="%s"/>' % (initial_type, file), file=fd)
    else:
        print('        <alternative-files value="%s"/>' % file, file=fd)
        print('        <weights value="%s"/>' % get_weightfilename(options, step-1, "dump"), file=fd)
    if options.ecomeasure:
        print('        <weight-attribute value="%s"/>' % options.ecomeasure, file=fd)
    print("""    </input>
    <output>
        <output-file value="%s"/>
        <exit-times value="%s"/>
    </output>""" % (output, withExitTimes), file=fd)
    print("""    <processing>
        <ignore-errors value="%s"/>
        <with-taz value="%s"/>
        <gawron.beta value="%s"/>
        <gawron.a value="%s"/>
        <keep-all-routes value="%s"/>
        <routing-algorithm value="%s"/>%s
        <max-alternatives value="%s"/>
        <weights.expand value="true"/>
        <logit value="%s"/>
        <logit.beta value="%s"/>
        <logit.gamma value="%s"/>""" % (
                options.continueOnUnbuild, 
                bool(options.districts), 
                options.gBeta, 
                options.gA, 
                options.allroutes, 
                options.routing_algorithm, 
                ("" if 'CH' not in options.routing_algorithm else '\n<weight-period value="%s"/>\n' % options.aggregation),
                options.max_alternatives, 
                options.logit, 
                options.logitbeta, 
                options.logitgamma), file=fd)
    if options.logittheta:
        print('        <logit.theta value="%s"/>' % options.logittheta, file=fd)
    print('    </processing>', file=fd)
                
    print('    <random_number><random value="%s"/></random_number>' % options.absrand, file=fd)
    print('    <time><begin value="%s"/>' % options.begin, end=' ', file=fd)
    if options.end:
        print('<end value="%s"/>' % options.end, end=' ', file=fd)
    print("""</time>
    <report>
        <verbose value="%s"/>
        <no-step-log value="True"/>
        <no-warnings value="%s"/>
    </report>
</configuration>""" % (options.router_verbose, options.noWarnings), file=fd)
    fd.close()
    return cfgname

def get_scale(options, step):
    # compute scaling factor for simulation
    # using incValue = 1 (default) and incBase = 10 would produce 
    # iterations with increasing scale 0.1, 0.2, ... 0.9, 1, 1, 1, ...
    if options.incBase > 0:
        return min(options.incStart + options.incValue * float(step + 1) / options.incBase, options.incMax)
    else:
        return options.incMax

def get_dumpfilename(options, step, prefix):
    # the file to which edge costs (traveltimes) are written
        return "%s_%03i_%s.xml" % (prefix, step, options.aggregation)

def get_weightfilename(options, step, prefix):
    # the file from which edge costs are loaded
    # this defaults to the dumpfile writen by the simulation but may be
    # different if one of the options --addweights, --memory-weights or --cost-modifier is used
    if options.addweights:
        prefix = "%s,%s" % (options.addweights, prefix)
    if options.weightmemory:
        prefix = "memory_" + prefix
    if options.costmodifier != 'None':
        prefix = options.costmodifier + "_" + prefix
    return get_dumpfilename(options, step, prefix)

def writeSUMOConf(sumoBinary, step, options, additional_args, route_files):
    detectorfile = "dua_dump_%03i.add.xml" % step
    comma = (',' if options.additional != "" else '')
    sumoCmd = [sumoBinary,
        '--save-configuration', "iteration_%03i.sumocfg" % step,
        '--log', "iteration_%03i.sumo.log" % step,
        '--net-file', options.net,
        '--route-files', route_files,
        '--additional-files', "%s%s%s" % (detectorfile, comma, options.additional),
        '--no-step-log',
        '--random', options.absrand,
        '--begin', options.begin,
        '--route-steps', options.routeSteps,
        '--no-internal-links', options.internallink,
        '--lanechange.allow-swap', options.lanechangeallowed,
        '--eager-insert', options.eager_insert,
        '--time-to-teleport', options.timetoteleport,
        '--time-to-teleport.highways', options.timetoteleport_highways,
        '--verbose',
        '--no-warnings', options.noWarnings,
        ] + additional_args

    if hasattr(options, "noSummary") and not options.noSummary:
        sumoCmd += ['--summary-output', "summary_%03i.xml" % step]
    if hasattr(options, "noTripinfo") and not options.noTripinfo:
        sumoCmd += ['--tripinfo-output', "tripinfo_%03i.xml" % step]
        if options.ecomeasure:
            sumoCmd += ['--device.hbefa.probability', '1']
    if hasattr(options, "routefile"):
        if options.routefile == "routesonly":
            sumoCmd += ['--vehroute-output', "vehroute_%03i.xml" % step]
        elif options.routefile == "detailed":
            sumoCmd += ['--vehroute-output', "vehroute_%03i.xml" % step,
                    '--vehroute-output.exit-times']
    if hasattr(options, "lastroute") and options.lastroute:
        sumoCmd += ['--vehroute-output.last-route', options.lastroute]
    if hasattr(options, "timeInc") and options.timeInc:
        sumoCmd += ['--end', int(options.timeInc * (step + 1))]
    elif options.end:
        sumoCmd += ['--end', options.end]

    if hasattr(options, "incBase") and options.incBase > 0:
        sumoCmd += ['--scale', get_scale(options, step)]
    if options.mesosim:
        sumoCmd += ['--mesosim', 
                '--meso-recheck', options.mesorecheck]
        if options.mesomultiqueue:
            sumoCmd += ['--meso-multi-queue']
        if options.mesojunctioncontrol:
            sumoCmd += ['--meso-junction-control']

    # make sure all arguments are strings
    sumoCmd = list(map(str, sumoCmd))
    # use sumoBinary to write a config file
    subprocess.call(sumoCmd, stdout=subprocess.PIPE)

    # write detectorfile
    with open(detectorfile, 'w') as fd:
        suffix = "_%03i_%s" % (step, options.aggregation)
        print("<a>", file=fd)
        if options.costmodifier !='None':
            print('    <edgeData id="dump%s" freq="%s" file="%s" excludeEmpty="defaults" minSamples="1"/>' % (
                    suffix, options.aggregation, get_dumpfilename(options, step, "dump")), file=fd)
        else:
            print('    <edgeData id="dump%s" freq="%s" file="%s" excludeEmpty="true" minSamples="1"/>' % (
                    suffix, options.aggregation, get_dumpfilename(options, step, "dump")), file=fd)
        if options.ecomeasure:
            print('    <edgeData id="eco%s" type="hbefa" freq="%s" file="dump%s.xml" excludeEmpty="true" minSamples="1"/>' % (suffix, options.aggregation, suffix), file=fd)
        print("</a>", file=fd)

def filterTripinfo(step, attrs):
    attrs.add("id")
    inFile = "tripinfo_%03i.xml" % step
    if os.path.exists(inFile):
        out = open(inFile + ".filtered", 'w')
        print("<tripinfos>", file=out)
        hadOutput = False
        for line in open(inFile):
            if "<tripinfo " in line:
                if hadOutput:
                    print("/>", file=out)
                print("    <tripinfo", end=' ', file=out)
                for a in attrs:
                    pos = line.find(a)
                    if pos >= 0:
                        pos += len(a)+2
                        print('%s="%s"' % (a, line[pos:line.find('"', pos)]), end=' ', file=out)
                hadOutput = True
            if "<emission" in line:
                for a in attrs:
                    pos = line.find(a)
                    if pos >= 0:
                        pos += len(a)+2
                        print('%s="%s"' % (a, line[pos:line.find('"', pos)]), end=' ', file=out)
        if hadOutput:
            print("/>", file=out)
        print("</tripinfos>", file=out)
        out.close()
        os.remove(inFile)
        os.rename(out.name, inFile)

def assign_remaining_args(application, prefix, args):
    # assign remaining args [ prefix--o1 a1 prefix--o2 prefix--o3 a3  ...]
    # only handles long options!
    assigned = []
    ## split into options and arguments
    items = []
    item = None
    for arg in args:
        if "--" in arg:
            if item != None:
                items.append(item)
            item = [arg]
        else:
            if item == None:
                sys.exit('Encounted argument "%s" without a preceeding option' % arg)
            item.append(arg)
    if item != None:
        items.append(item)

    # assign to programs 
    valid_options = set(get_long_option_names(application))
    for item in items:
        prefixed = item[0]
        if prefixed[0:len(prefix)] == prefix:
            option = prefixed[len(prefix):]
            if option in valid_options:
                assigned.append(option)
                assigned += item[1:]
            else:
                sys.exit('"%s" is not a valid option for "%s"' % (option, application))
                unassigned += item

    return assigned

def get_basename(demand_file):
    basename = os.path.basename(demand_file)
    if 'alt' in basename:
        return  basename[:-12]
    elif 'trips' in basename:
        return basename[:-10]
    else:
        return basename[:basename.find(".")]

def main(args=None):
    optParser = initOptions()
    
    options, remaining_args = optParser.parse_args(args=args)
    if not options.net:
        optParser.error("Option --net-file is mandatory")
    if (not options.trips and not options.routes and not options.flows) or (options.trips and options.routes):
        optParser.error("Either --trips, --flows, or --routes have to be given!")
    duaBinary = sumolib.checkBinary("duarouter", options.path)
    if options.mesosim:
        sumoBinary = sumolib.checkBinary("meso", options.path)
    else:
        sumoBinary = sumolib.checkBinary("sumo", options.path)
    if options.addweights and options.weightmemory:
        optParser.error("Options --addweights and --weight-memory are mutually exclusive.")

    # make sure BOTH binaries are callable before we start
    try:
        subprocess.call(duaBinary, stdout=subprocess.PIPE)
    except OSError:
        sys.exit("Error: Could not locate duarouter (%s).\nMake sure its on the search path or set environment variable DUAROUTER_BINARY\n" % duaBinary)
    try:
        subprocess.call(sumoBinary, stdout=subprocess.PIPE)
    except OSError:
        sys.exit("Error: Could not locate sumo (%s).\nMake sure its on the search path or set environment variable SUMO_BINARY\n" % sumoBinary)

    sumo_args = assign_remaining_args(sumoBinary, 'sumo', remaining_args)
    
    sys.stdout = sumolib.TeeFile(sys.stdout, open("stdout.log", "w+"))
    log = open("dua.log", "w+")
    if options.zip:
        if options.clean_alt:
            sys.exit("Error: Please use either --zip or --clean-alt but not both.")
        try:
            subprocess.call("7z", stdout=open(os.devnull, 'wb'))
        except:
            sys.exit("Error: Could not locate 7z, please make sure its on the search path.")
        zipProcesses = {}
        zipLog = open("7zip.log", "w+")
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
    if options.externalgawron:
        # avoid dependency on numpy for normal duaIterate
        from routeChoices import getRouteChoices, calFirstRouteProbs
        print('use externalgawron')
        edgesMap = {}
    if options.weightmemory:
        costmemory = CostMemory('traveltime'
                ,pessimism=options.pessimism
                ,network_file=options.net
                )
    routesSuffix = ".xml"
    if options.binary:
        routesSuffix = ".sbx"
    if options.costmodifier != 'None':
        pyPath = os.path.abspath(os.path.dirname(sys.argv[0]))
        sys.path.append(os.path.join(pyPath, "..", "..", "..", "..","..", "tools", "kkwSim"))
        from kkwCostModifier import costModifier
        print('Use the cost modifier for KKW simulation')

    if options.weightmemory and options.firstStep != 0:
        # load previous dump files when continuing a run
        print(">> Reassembling cost-memory from previous iteration steps")
        for step in range(0, options.firstStep):
            dumpfile = get_dumpfilename(options, step,"dump")
            print(">>> Loading %s" % dumpfile)
            costmemory.load_costs(dumpfile, step, get_scale(options, step))

    avgTT = sumolib.miscutils.Statistics()
    for step in range(options.firstStep, options.lastStep):
        btimeA = datetime.now()
        print("> Executing step %s" % step)
        
        router_demands = input_demands
        simulation_demands = input_demands
        # demand files have regular names based on the basename and the step
        if not (options.skipFirstRouting and step == 0):
            simulation_demands = [get_basename(f) + "_%03i.rou%s" % (step, routesSuffix) for f in input_demands]
        if not ((options.skipFirstRouting and step == 1) or step == 0):
            router_demands = [get_basename(f) + "_%03i.rou.alt%s" % (step-1, routesSuffix) for f in input_demands]

        if not (options.skipFirstRouting and step == options.firstStep):
            # call duarouter
            for router_input, output in zip(router_demands, simulation_demands):
                print(">> Running router on %s" % router_input)
                btime = datetime.now()
                print(">>> Begin time: %s" % btime)
                cfgname = writeRouteConf(step, options, router_input, output, options.routefile, initial_type)
                log.flush()
                call([duaBinary, "-c", cfgname], log)
                if options.clean_alt and not router_input in input_demands:
                    os.remove(router_input)
                etime = datetime.now()
                print(">>> End time: %s" % etime)
                print(">>> Duration: %s" % (etime-btime))
                print("<<")
                # use the external gawron
                if options.externalgawron:
                    ecomeasure = None
                    if options.ecomeasure:
                        ecomeasure = options.ecomeasure
                    if step == options.firstStep + 1 and options.skipFirstRouting:
                        if options.caloldprob:
                            calFirstRouteProbs("dump_000_%s.xml" % (options.aggregation), basename + "_001.rou.alt.xml",options.addweights,ecomeasure)
                        else:
                            shutil.copy(basename + "_001.rou.alt.xml", basename + "_001.rou.galt.xml")
                            shutil.copy(basename + "_001.rou.xml", basename + "_001.grou.xml")
                    if step == options.firstStep and not options.skipFirstRouting:
                        shutil.copy(basename + "_000.rou.alt.xml", basename + "_000.rou.galt.xml")
                        shutil.copy(basename + "_000.rou.xml", basename + "_000.grou.xml")
                    else:
                        print('step:', step)
                        print('get externalgawron')
                        dumpfile = "dump_%03i_%s.xml" % (step-1, options.aggregation)
                        if (not options.skipFirstRouting) or (options.skipFirstRouting and step > 1):
                            output, edgesMap = getRouteChoices(edgesMap,dumpfile,basename + "_%03i.rou.alt.xml" % step,options.net,options.addweights, options.gA, options.gBeta,step,ecomeasure)

        # simulation
        print(">> Running simulation")
        btime = datetime.now()
        print(">>> Begin time: %s" % btime)
        writeSUMOConf(sumoBinary, step, options, sumo_args,
                ",".join(simulation_demands))   #  todo: change 'grou.xml'
        log.flush()
        call([sumoBinary, "-c", "iteration_%03i.sumocfg" % step], log)
        if options.tripinfoFilter:
            filterTripinfo(step, set(options.tripinfoFilter.split(",")))
        etime = datetime.now()
        print(">>> End time: %s" % etime)
        print(">>> Duration: %s" % (etime-btime))
        print("<<")

        if options.weightmemory:
            print(">> Smoothing edge weights")
            costmemory.load_costs(
                    get_dumpfilename(options, step,"dump"), step, get_scale(options, step))
            costmemory.write_costs(get_weightfilename(options, step, "dump"))
            print(">>> Updated %s edges" % costmemory.loaded())
            print(">>> Decayed %s unseen edges" % costmemory.decayed())
            print(">>> Error avg:%s mean:%s" % (costmemory.avg_error(), costmemory.mean_error()))
            print(">>> Absolute Error avg:%s mean:%s" % (costmemory.avg_abs_error(), costmemory.mean_abs_error()))

        if options.costmodifier != 'None':
            currentDir = os.getcwd()
            costModifier(get_weightfilename(options, step, "dump"), step, "dump", options.aggregation, currentDir, options.costmodifier, 'dua-iterate')

        if options.zip and step - options.firstStep > 1:
            # this is a little hackish since we zip and remove all files by glob, which may have undesired side effects
            # also note that the 7z file does not have an "_" before the iteration number in order to be not picked up by the remove
            for s in zipProcesses.keys():
                if zipProcesses[s].poll() is not None:
                    for f in glob.glob("*_%03i*" % s):
                        try:
                            os.remove(f)
                        except:
                            print("Could not remove %s" % f, file=zipLog)
                    del zipProcesses[s]
            zipStep = step - 2
            zipProcesses[zipStep] = subprocess.Popen(["7z", "a", "iteration%03i.7z" % zipStep] + glob.glob("*_%03i*" % zipStep), stdout=zipLog, stderr=zipLog)

        converged = False
        if options.convDev:
            sum = 0.
            count = 0
            for t in sumolib.output.parse_fast("tripinfo_%03i.xml" % step, 'tripinfo', ['duration']):
                sum += float(t.duration)
                count += 1
            avgTT.add(sum / count)
            relStdDev = avgTT.relStdDev(options.convIt)
            print("< relative travel time deviation in the last %s steps: %.05f" % (min(avgTT.count(), options.convIt), relStdDev))
            if avgTT.count() >= options.convIt and relStdDev < options.convDev:
                converged = True
    
        print("< Step %s ended (duration: %s)" % (step, datetime.now() - btimeA))
        print("------------------\n")

        log.flush()
        if converged:
            break
    if options.zip:
        for s in zipProcesses.keys():
            zipProcesses[s].wait()
            for f in glob.glob("*_%03i*" % s):
                try:
                    os.remove(f)
                except:
                    print("Could not remove %s" % f, file=zipLog)
        zipLog.close()
    print("dua-iterate ended (duration: %s)" % (datetime.now() - starttime))
    
    log.close()

if __name__ == "__main__":
    main()
