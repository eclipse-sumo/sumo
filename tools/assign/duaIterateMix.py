#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    duaIterateMix.py
# @author  Behzad Bamdad Mehrabani
# @author  Jakob Erdmann
# @date    2022-08-10

"""
Run duarouter and sumo alternating to perform a mixed traffic assignment of user equilibrium and system optimum
(Mixed traffic flow of Human Driven Vehicles (HDVs) and Connected and Autonomous Vehicle (CAVs)).
It is assumed that HDVs follow user equilibrium and CAVs follow system optimum.
"""
from __future__ import print_function
from __future__ import absolute_import

import os
import sys
import subprocess
import glob
import argparse
import xml.etree.ElementTree as ET


from datetime import datetime
from costMemory import CostMemory
from duaIterate import addGenericOptions
from duaIterate import call
from duaIterate import get_scale
from duaIterate import get_dumpfilename
from duaIterate import get_weightfilename
from duaIterate import writeSUMOConf
from duaIterate import filterTripinfo
from duaIterate import assign_remaining_args
from duaIterate import get_basename
from duaIterate import calcMarginalCost
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

import sumolib  # noqa

DEBUGLOG = None
EDGEDATA_ADD = "edgedata.add.xml"


def initOptions():
    argParser = sumolib.options.ArgumentParser(
        description=""" Any options of the form sumo--long-option-name will be passed to sumo.
        These must be given after all the other options
        example: sumo--step-length 0.5 will add the option --step-length 0.5 to sumo.""",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    addGenericOptions(argParser)

    argParser.add_argument("--continue-on-unbuild", action="store_true", dest="continueOnUnbuild",
                           default=False, help="continues on unbuild routes")
    argParser.add_argument("-t", "--trips",
                           help="trips in step 0 (either trips, flows, or routes have to be supplied)", metavar="FILE")
    argParser.add_argument("-r", "--routes",
                           help="routes in step 0 (either trips, flows, or routes have to be supplied)", metavar="FILE")
    argParser.add_argument("-F", "--flows",
                           help="flows in step 0 (either trips, flows, or routes have to be supplied)", metavar="FILE")
    argParser.add_argument("-A", "--gA",
                           type=float, default=.5, help="Sets Gawron's Alpha")
    argParser.add_argument("-B", "--gBeta",
                           type=float, default=.9, help="Sets Gawron's Beta")
    argParser.add_argument("-E", "--disable-summary", "--disable-emissions", action="store_true", dest="noSummary",
                           default=False, help="No summaries are written by the simulation")
    argParser.add_argument("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                           default=False, help="No tripinfos are written by the simulation")
    argParser.add_argument("--tripinfo-filter", dest="tripinfoFilter",
                           help="filter tripinfo attributes")
    argParser.add_argument("--inc-start", dest="incStart",
                           type=float, default=0, help="Start for incrementing scale")
    argParser.add_argument("--inc-max", dest="incMax",
                           type=float, default=1, help="Maximum for incrementing scale")
    argParser.add_argument("--inc-base", dest="incBase", type=int, default=-1,
                           help="Give the incrementation base. Negative values disable incremental scaling")
    argParser.add_argument("--incrementation", dest="incValue",
                           type=int, default=1, help="Give the incrementation")
    argParser.add_argument("--time-inc", dest="timeInc",
                           type=int, default=0, help="Give the time incrementation")
    argParser.add_argument("-f", "--first-step", dest="firstStep",
                           type=int, default=0, help="First DUA step")
    argParser.add_argument("-l", "--last-step", dest="lastStep",
                           type=int, default=50, help="Last DUA step")
    argParser.add_argument("--convergence-iterations", dest="convIt",
                           type=int, default=10, help="Number of iterations to use for convergence calculation")
    argParser.add_argument("--max-convergence-deviation", dest="convDev",
                           type=float, help="Maximum relative standard deviation in travel times")
    argParser.add_argument("-D", "--districts", help="use districts as sources and targets", metavar="FILE")
    argParser.add_argument("-x", "--vehroute-file",  dest="routefile",
                           choices=['None', 'routesonly', 'detailed'],
                           default='None', help="choose the format of the route file")
    argParser.add_argument("-z", "--output-lastRoute",  action="store_true", dest="lastroute",
                           default=False, help="output the last routes")
    argParser.add_argument("-K", "--keep-allroutes", action="store_true", dest="allroutes",
                           default=False, help="save routes with near zero probability")
    argParser.add_argument("--routing-algorithm", default="dijkstra", help="select the routing algorithm")
    argParser.add_argument("--max-alternatives-HDV", default=5, dest="max_alternatives_HDV",
                           help="prune the number of alternatives to INT for HDV")
    argParser.add_argument("--max-alternatives-CAV", default=5, dest="max_alternatives_CAV",
                           help="prune the number of alternatives to INT for CAV")
    argParser.add_argument("--skip-first-routing", action="store_true", dest="skipFirstRouting",
                           default=False, help="run simulation with demands before first routing")
    argParser.add_argument("--logit", action="store_true", default=False, help="use the logit model for route choice")
    argParser.add_argument("-g", "--logitbeta", type=float,
                           default=0.15, help="use the c-logit model for route choice; logit model when beta = 0")
    argParser.add_argument("-i", "--logitgamma", type=float, default=1., help="use the c-logit model for route choice")
    argParser.add_argument("-G", "--logittheta", type=float, help="parameter to adapt the cost unit")
    argParser.add_argument("-J", "--addweights", help="Additional weights for duarouter")
    argParser.add_argument("--convergence-steps", dest="convergenceSteps", type=int,
                           help="Given x, if x > 0 reduce probability to change route by 1/x per step "
                                "(Probabilistic Swapping (PSwap)). "
                                "If x < 0 set probability of rerouting to 1/step after step |x|")
    argParser.add_argument("--addweights.once", dest="addweightsOnce", action="store_true",
                           default=False, help="use added weights only on the first iteration")
    argParser.add_argument("--router-verbose", action="store_true",
                           default=False, help="let duarouter print some statistics")
    argParser.add_argument("--weight-memory", action="store_true", default=False, dest="weightmemory",
                           help="smooth edge weights across iterations")
    argParser.add_argument("--pessimism", default=1, type=float,
                           help="give traffic jams a higher weight when using option --weight-memory")
    argParser.add_argument("--clean-alt", action="store_true", dest="clean_alt",
                           default=False, help="Whether old rou.alt.xml files shall be removed")
    argParser.add_argument("--binary", dest="gzip", action="store_true", default=False,
                           help="alias for --gzip")
    argParser.add_argument("--gzip", action="store_true", default=False,
                           help="writing intermediate and resulting route files in gzipped format")
    argParser.add_argument("--dualog", default="dua.log", help="log file path (default 'dua.log')")
    argParser.add_argument("--log", default="stdout.log", help="stdout log file path (default 'stdout.log')")
    argParser.add_argument("--marginal-cost", action="store_true", default=False,
                           help="use marginal cost to perform system optimal traffic assignment")
    argParser.add_argument("--marginal-cost-reverse", action="store_true", default=False,
                           help="use with mix option for reversing the marginal cost with original travel times")
    argParser.add_argument("--mix", action="store_true", default=False, help="performing mix traffic assignment")
    argParser.add_argument("--marginal-cost.exp", type=float, default=0, dest="mcExp",
                           help="apply the given exponent on the current traffic count when computing marginal cost")
    argParser.add_argument("remaining_args", nargs='*')
    return argParser


def writeRouteConfHDV(duarouterBinary, step, options, dua_args, file,
                      output, routesInfo):
    filename = os.path.basename(file)
    filename = filename.split('.')[0]
    cfgname = "%03i/iteration_%03i_%s.duarcfg" % (step, step, filename)
    args = [
        '--net-file', options.net,
        '--route-files', file,
        '--output-file', output,
        '--exit-times', str(routesInfo == "detailed"),
        '--ignore-errors', str(options.continueOnUnbuild),
        '--with-taz', str(options.districts is not None),
        '--gawron.beta', str(options.gBeta),
        '--gawron.a', str(options.gA),
        '--keep-all-routes', str(options.allroutes),
        '--routing-algorithm', options.routing_algorithm,
        '--max-alternatives', str(options.max_alternatives_HDV),
        '--weights.expand',
        '--logit.beta', str(options.logitbeta),
        '--logit.gamma', str(options.logitgamma),
        '--random', str(options.absrand),
        '--begin', str(options.begin),
        '--verbose', str(options.router_verbose),
        '--no-step-log',
        '--no-warnings', str(options.noWarnings),
    ]
    if options.districts:
        args += ['--additional-files', options.districts]
    if options.logit:
        args += ['--route-choice-method', 'logit']
        if options.MSA:
            probKeepRoute = step/(step+1)
            args += ['--keep-route-probability', str(probKeepRoute)]
        if options.convergenceSteps:
            if options.convergenceSteps > 0:
                probKeepRoute = max(0, min(step / float(options.convergenceSteps), 1))
            else:
                startStep = -options.convergenceSteps
                probKeepRoute = 0 if step > startStep else 1 - 1.0 / (step - startStep)
            args += ['--keep-route-probability', str(probKeepRoute)]

    if step > 0 or options.addweights:
        weightpath = ""
        if step > 0:
            weightpath = get_weightfilename(options, step - 1, "dump")
        if options.addweights and (step == 0 or not options.addweightsOnce):
            if step > 0:
                weightpath += ","
            weightpath += options.addweights
        args += ['--weight-files', weightpath]
    if options.eco_measure:
        args += ['--weight-attribute', options.eco_measure]
    if 'CH' in options.routing_algorithm:
        args += ['--weight-period', str(options.aggregation)]
    if options.logittheta:
        args += ['--logit.theta', str(options.logittheta)]
    if options.end:
        args += ['--end', str(options.end)]

    args += ["--save-configuration", cfgname]

    subprocess.call([duarouterBinary] + args + dua_args)
    return cfgname


def writeRouteConfCAV(duarouterBinary, step, options, dua_args, file,
                      output, routesInfo):
    filename = os.path.basename(file)
    filename = filename.split('.')[0]
    cfgname = "%03i/iteration_%03i_%s.duarcfg" % (step, step, filename)
    args = [
        '--net-file', options.net,
        '--route-files', file,
        '--output-file', output,
        '--exit-times', str(routesInfo == "detailed"),
        '--ignore-errors', str(options.continueOnUnbuild),
        '--with-taz', str(options.districts is not None),
        '--gawron.beta', str(options.gBeta),
        '--gawron.a', str(options.gA),
        '--keep-all-routes', str(options.allroutes),
        '--routing-algorithm', options.routing_algorithm,
        '--max-alternatives', str(options.max_alternatives_CAV),
        '--weights.expand',
        '--logit.beta', str(options.logitbeta),
        '--logit.gamma', str(options.logitgamma),
        '--random', str(options.absrand),
        '--begin', str(options.begin),
        '--verbose', str(options.router_verbose),
        '--no-step-log',
        '--no-warnings', str(options.noWarnings),
    ]
    if options.districts:
        args += ['--additional-files', options.districts]
    if options.logit:
        args += ['--route-choice-method', 'logit']
        if options.MSA:
            probKeepRoute = step/(step+1)
            args += ['--keep-route-probability', str(probKeepRoute)]
        if options.convergenceSteps:
            if options.convergenceSteps > 0:
                probKeepRoute = max(0, min(step / float(options.convergenceSteps), 1))
            else:
                startStep = -options.convergenceSteps
                probKeepRoute = 0 if step > startStep else 1 - 1.0 / (step - startStep)
            args += ['--keep-route-probability', str(probKeepRoute)]

    if step > 0 or options.addweights:
        weightpath = ""
        if step > 0:
            weightpath = get_weightfilename(options, step - 1, "dump")
        if options.addweights and (step == 0 or not options.addweightsOnce):
            if step > 0:
                weightpath += ","
            weightpath += options.addweights
        args += ['--weight-files', weightpath]
    if options.eco_measure:
        args += ['--weight-attribute', options.eco_measure]
    if 'CH' in options.routing_algorithm:
        args += ['--weight-period', str(options.aggregation)]
    if options.logittheta:
        args += ['--logit.theta', str(options.logittheta)]
    if options.end:
        args += ['--end', str(options.end)]

    args += ["--save-configuration", cfgname]

    subprocess.call([duarouterBinary] + args + dua_args)
    return cfgname


def calcMarginalCostReverse(step, options):
    if step > 1:
        if DEBUGLOG:
            log = open("marginal_cost2.log", "w" if step == 2 else "a")
        tree_sumo_cur = ET.parse(get_weightfilename(options, step - 1, "dump"))
        tree_sumo_prv = ET.parse(get_weightfilename(options, step - 2, "dump"))
        for interval_cur in tree_sumo_cur.getroot():
            begin_cur = interval_cur.attrib.get("begin")
            for interval_prv in tree_sumo_prv.getroot():
                begin_prv = interval_prv.attrib.get("begin")
                for edge_cur in interval_cur.iter('edge'):
                    for edge_prv in interval_prv.iter('edge'):
                        if begin_cur == begin_prv and edge_cur.get("id") == edge_prv.get("id"):
                            if edge_cur.get("traveltime") is not None and edge_prv.get(
                                    "traveltime") is not None:
                                tt_cur = float(edge_cur.get("overlapTraveltime"))
                                edge_cur.set("traveltime", str(tt_cur))
        tree_sumo_cur.write(get_weightfilename(options, step - 1, "dump"))

        if DEBUGLOG:
            log.close()


def calcMixTrips(step, options):
    input_demands = options.trips.split(",")
    input_demands_1 = options.routes.split(",")
    routesSuffix = ".xml"
    trips_CAV = ["%03i/%s_%03i.rou%s" % (step, get_basename(f), step, routesSuffix)
                 for f in input_demands_1]
    trips_HDV = ["%03i/%s_%03i.rou%s" % (step, get_basename(f), step, routesSuffix)
                 for f in input_demands]
    src_tree = ET.parse(str(trips_CAV[0]))
    dest_tree = ET.parse(str(trips_HDV[0]))
    src_root = src_tree.getroot()
    dest_root = dest_tree.getroot()

    # temporary remove vtypes
    vType1 = dest_root.find("vType")
    dest_root.remove(vType1)
    vType2 = src_root.find("vType")
    src_root.remove(vType2)

    for n in range(10000000):
        src_tag = src_root.find('./vehicle')
        if src_tag is None:
            break
        dest_root.append(src_tag)
        src_root.remove(src_tag)

    dest_root[:] = sorted(dest_root, key=lambda child: (child.tag, float(child.get("depart"))))
    for n in range(10000000):
        dest_tag_2 = dest_root.find('./vehicle')
        if dest_tag_2 is None:
            break
        src_root.append(dest_tag_2)
        dest_root.remove(dest_tag_2)

    # Put the 'vType' element on HDV file
    src_root.insert(0, vType1)
    src_root.insert(1, vType2)
    ET.ElementTree(src_root).write(os.path.join((str(trips_HDV[0]))))
    # if DEBUGLOG:
    #     log.close()


def main(args=None):
    argParser = initOptions()

    try:
        options = argParser.parse_args(args=args)
    except (NotImplementedError, ValueError) as e:
        print(e, file=sys.stderr)
        sys.exit(1)

    if not options.net:
        argParser.error("Option --net-file is mandatory")
    if (not options.trips and not options.routes and not options.flows):
        argParser.error(
            "Either --trips, --flows, or --routes have to be given!")
    duaBinary = sumolib.checkBinary("duarouter", options.path)
    sumoBinary = sumolib.checkBinary("sumo", options.path)
    if options.addweights and options.weightmemory:
        argParser.error("Options --addweights and --weight-memory are mutually exclusive.")
    if options.marginal_cost and not options.logit:
        print("Warning! --marginal-cost works best with --logit.", file=sys.stderr)

    # make sure BOTH binaries are callable before we start
    try:
        subprocess.call(duaBinary, stdout=subprocess.PIPE)
    except OSError:
        sys.exit(
            ("Error: Could not locate duarouter (%s).\nMake sure its on the search path or set environment " +
             "variable DUAROUTER_BINARY\n") % duaBinary)
    try:
        subprocess.call(sumoBinary, stdout=subprocess.PIPE)
    except OSError:
        sys.exit(
            ("Error: Could not locate sumo (%s).\nMake sure its on the search path or set environment " +
             "variable SUMO_BINARY\n") % sumoBinary)

    sumo_args = assign_remaining_args(sumoBinary, 'sumo', options.remaining_args)
    dua_args = assign_remaining_args(duaBinary, 'duarouter', options.remaining_args)
    sys.stdout = sumolib.TeeFile(sys.stdout, open(options.log, "w+"))
    log = open(options.dualog, "w+")
    if options.zip:
        if options.clean_alt:
            sys.exit("Error: Please use either --zip or --clean-alt but not both.")
        try:
            subprocess.call("7z", stdout=open(os.devnull, 'wb'))
        except Exception:
            sys.exit("Error: Could not locate 7z, please make sure its on the search path.")
        zipProcesses = {}
        zipLog = open("7zip.log", "w+")
    starttime = datetime.now()
    if options.trips:
        input_demands = options.trips.split(",")
    if options.routes:
        input_demands_1 = options.routes.split(",")
    if options.trips and not options.routes:
        print("Please insert HDVs trip by -t and CAVs trips by -r")
    if options.routes and not options.trips:
        print("Please insert HDVs trip by -t and CAVs trips by -r")
    if options.weightmemory:
        costmemory = CostMemory('traveltime', pessimism=options.pessimism, network_file=options.net
                                )
    routesSuffix = ".xml"
    if options.gzip:
        routesSuffix = ".gz"

    if options.weightmemory and options.firstStep != 0:
        # load previous dump files when continuing a run
        print(">> Reassembling cost-memory from previous iteration steps")
        for step in range(0, options.firstStep):
            dumpfile = get_dumpfilename(options, step, "dump")
            print(">>> Loading %s" % dumpfile)
            costmemory.load_costs(dumpfile, step, get_scale(options, step))

    # write detectorfile
    with open(EDGEDATA_ADD, 'w') as fd:
        vTypes = ' vTypes="%s"' % ' '.join(options.measureVTypes.split(',')) if options.measureVTypes else ""
        print("<a>", file=fd)
        print('    <edgeData id="dump_%s" freq="%s" file="%s" excludeEmpty="true" minSamples="1"%s/>' % (
            options.aggregation,
            options.aggregation,
            get_dumpfilename(options, -1, "dump", False),
            vTypes), file=fd)
        if options.eco_measure:
            print(('    <edgeData id="eco_%s" type="hbefa" freq="%s" file="%s" ' +
                   'excludeEmpty="true" minSamples="1"%s/>') % (
                       options.aggregation,
                       options.aggregation,
                       get_dumpfilename(options, step, "dump", False),
                       vTypes), file=fd)
        print("</a>", file=fd)

    avgTT = sumolib.miscutils.Statistics()
    for step in range(options.firstStep, options.lastStep):
        current_directory = os.getcwd()
        final_directory = os.path.join(current_directory, "%03i" % step)
        if not os.path.exists(final_directory):
            os.makedirs(final_directory)
        btimeA = datetime.now()
        print("> Executing step %s" % step)
        # working on CAVs demand
        router_demands = input_demands
        simulation_demands = input_demands
        # demand files have regular names based on the basename and the step
        if not (options.skipFirstRouting and step == 0):
            simulation_demands = ["%03i/%s_%03i.rou%s" % (step, get_basename(f), step, routesSuffix)
                                  for f in input_demands]
        if not ((options.skipFirstRouting and step == 1) or step == 0):
            router_demands = ["%03i/%s_%03i.rou.alt%s" % (step-1, get_basename(f), step-1, routesSuffix)
                              for f in input_demands]

        if not (options.skipFirstRouting and step == options.firstStep):
            # call duarouter
            for router_input, output in zip(router_demands, simulation_demands):
                print(">> Running router on %s" % router_input)
                btime = datetime.now()
                print(">>> Begin time: %s" % btime)
                cfgname = writeRouteConfHDV(duaBinary, step, options, dua_args, router_input,
                                            output, options.routefile)
                log.flush()
                sys.stdout.flush()
                if options.marginal_cost_reverse:
                    calcMarginalCostReverse(step, options)
                call([duaBinary, "-c", cfgname], log)
                if options.clean_alt and router_input not in input_demands:
                    os.remove(router_input)
                etime = datetime.now()
                print(">>> End time: %s" % etime)
                print(">>> Duration: %s" % (etime - btime))
                print("<<")
        # working on HDVs demand
        router_demands = input_demands_1
        simulation_demands = input_demands_1
        # demand files have regular names based on the basename and the step
        if not (options.skipFirstRouting and step == 0):
            simulation_demands = ["%03i/%s_%03i.rou%s" % (step, get_basename(f), step, routesSuffix)
                                  for f in input_demands_1]
        if not ((options.skipFirstRouting and step == 1) or step == 0):
            router_demands = ["%03i/%s_%03i.rou.alt%s" % (step-1, get_basename(f), step-1, routesSuffix)
                              for f in input_demands_1]

        if not (options.skipFirstRouting and step == options.firstStep):
            # call duarouter
            for router_input, output in zip(router_demands, simulation_demands):
                print(">> Running router on %s" % router_input)
                btime = datetime.now()
                print(">>> Begin time: %s" % btime)
                cfgname = writeRouteConfCAV(duaBinary, step, options, dua_args, router_input,
                                            output, options.routefile)
                log.flush()
                sys.stdout.flush()
                if options.marginal_cost:
                    calcMarginalCost(step, options)

                call([duaBinary, "-c", cfgname], log)
                if options.clean_alt and router_input not in input_demands_1:
                    os.remove(router_input)
                etime = datetime.now()
                print(">>> End time: %s" % etime)
                print(">>> Duration: %s" % (etime - btime))
                print("<<")
        # combining trips file
        if options.mix:
            calcMixTrips(step, options)
        # simulation
        simulation_demands = ["%03i/%s_%03i.rou%s" % (step, get_basename(f), step, routesSuffix)
                              for f in input_demands]
        print(">> Running simulation")
        btime = datetime.now()
        print(">>> Begin time: %s" % btime)
        sumocfg = writeSUMOConf(sumoBinary, step, options, sumo_args,
                                ",".join(simulation_demands))  # todo: change 'grou.xml'
        log.flush()
        sys.stdout.flush()
        call([sumoBinary, "-c", sumocfg], log)
        if options.tripinfoFilter:
            filterTripinfo(step, options.tripinfoFilter.split(","))
        etime = datetime.now()
        print(">>> End time: %s" % etime)
        print(">>> Duration: %s" % (etime - btime))
        print("<<")

        if options.weightmemory:
            print(">> Smoothing edge weights")
            costmemory.load_costs(get_dumpfilename(options, step, "dump"), step, get_scale(options, step))
            costmemory.write_costs(get_weightfilename(options, step, "dump"))
            print(">>> Updated %s edges" % costmemory.loaded())
            print(">>> Decayed %s unseen edges" % costmemory.decayed())
            print(">>> Error avg:%.12g mean:%.12g" % (costmemory.avg_error(), costmemory.mean_error()))
            print(">>> Absolute Error avg:%.12g mean:%.12g" %
                  (costmemory.avg_abs_error(), costmemory.mean_abs_error()))

        if options.zip and step - options.firstStep > 1:
            # this is a little hackish since we zip and remove all files by glob, which may have undesired side effects
            # also note that the 7z file does not have an "_" before the
            # iteration number in order to be not picked up by the remove
            for s in list(zipProcesses.keys()):
                if zipProcesses[s].poll() is not None:
                    for f in glob.glob("*_%03i*" % s):
                        try:
                            os.remove(f)
                        except Exception:
                            print("Could not remove %s" % f, file=zipLog)
                    del zipProcesses[s]
            zipStep = step - 2
            zipProcesses[zipStep] = subprocess.Popen(
                ["7z", "a", "iteration%03i.7z" % zipStep, "%03i" % zipStep], stdout=zipLog,
                stderr=zipLog)

        converged = False
        if options.convDev:
            sum = 0.
            count = 0
            for t in sumolib.output.parse_fast("%03i/tripinfo_%03i.xml" % (step, step),
                                               'tripinfo', ['duration']):
                sum += float(t.duration)
                count += 1
            avgTT.add(sum / count)
            relStdDev = avgTT.relStdDev(options.convIt)
            print("< relative travel time deviation in the last %s steps: %.05f" % (
                min(avgTT.count(), options.convIt), relStdDev))
            if avgTT.count() >= options.convIt and relStdDev < options.convDev:
                converged = True

        print("< Step %s ended (duration: %s)" %
              (step, datetime.now() - btimeA))
        print("------------------\n")

        log.flush()
        sys.stdout.flush()
        if converged:
            break
    if options.zip:
        for s in zipProcesses.keys():
            zipProcesses[s].wait()
            for f in glob.glob("*_%03i*" % s):
                try:
                    os.remove(f)
                except Exception:
                    print("Could not remove %s" % f, file=zipLog)
        zipLog.close()
    print("dua-iterate ended (duration: %s)" % (datetime.now() - starttime))

    log.close()


if __name__ == "__main__":
    main()
