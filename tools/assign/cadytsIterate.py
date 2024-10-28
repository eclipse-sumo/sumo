#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    cadytsIterate.py
# @author  Jakob Erdmann
# @author  Yun-Pang Floetteroed
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Mirko Barthauer
# @date    2010-09-15

"""
- Run Cadyts to calibrate the SUMO simulation with given routes (including alternatives and
  exit time) for each edge and traffic measurements.
- Edges' exit-time information can be obtained by setting the option "exit-times" True when
  running DUARouter/duaIterate.py
- The file format of the route alternatives (together with exit-times info) corresponds to
  the DUAROUTER assignment result (*.alt.xml)
- Respective traffic zones information has to exist in the given route files.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import glob
from datetime import datetime
from duaIterate import call, writeSUMOConf, addGenericOptions, generateEdgedataAddFile

if 'SUMO_HOME' in os.environ:
    TOOLS_DIR = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(TOOLS_DIR)
    import sumolib  # noqa
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def initOptions():
    jars = glob.glob(os.path.join(TOOLS_DIR, "contributed", "calibration", "*", "target", "*.jar")) + \
        glob.glob(os.path.join(TOOLS_DIR, "..", "bin", "*.jar"))
    argParser = sumolib.options.ArgumentParser()
    addGenericOptions(argParser)
    argParser.add_argument("-r", "--route-alternatives", dest="routes", type=argParser.route_file,
                           help="route alternatives from sumo (comma separated list, mandatory)", metavar="FILE")
    argParser.add_argument("-d", "--detector-values", dest="detvals", type=argParser.file,
                           help="adapt to the flow on the given edges", metavar="FILE")
    argParser.add_argument("--classpath", dest="classpath", default=os.pathsep.join(jars), type=str,
                           help="classpath for the calibrator [default: %(default)s]")
    argParser.add_argument("-l", "--last-calibration-step", dest="calibStep",
                           type=int, default=100, help="last step of the calibration [default: %(default)s]")
    argParser.add_argument("-S", "--demandscale", dest="demandscale",
                           type=float, default=2., help="scaled demand [default: %(default)s]")
    argParser.add_argument("-F", "--freezeit",  dest="freezeit",
                           type=int, default=85, help="define the number of iterations for stablizing the results " +
                                                      "in the DTA-calibration")
    argParser.add_argument("-V", "--varscale",  dest="varscale",
                           type=float, default=1., help="define variance of the measured traffic flows for the" +
                                                        "DTA-calibration")
    argParser.add_argument("-P", "--PREPITS",  type=int, dest="PREPITS",
                           default=5, help="number of preparatory iterations")
    argParser.add_argument("-W", "--evaluation-prefix", dest="evalprefix",
                           help="prefix of flow evaluation files; only for the calibration with use of detector data")
    argParser.add_argument("-Y", "--bruteforce", action="store_true", dest="bruteforce",
                           default=False, help="fit the traffic counts as accurate as possible")
    argParser.add_argument("-Z", "--mincountstddev", type=float, dest="mincountstddev",
                           default=25., help="minimal traffic count standard deviation")
    argParser.add_argument("-O", "--overridett", action="store_true", dest="overridett",
                           default=False, help="override depart times according to updated link travel times")
    argParser.add_argument("-E", "--disable-summary", "--disable-emissions", action="store_true", dest="noSummary",
                           default=False, help="No summaries are written by the simulation")
    argParser.add_argument("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                           default=False, help="No tripinfos are written by the simulation")
    argParser.add_argument("-M", "--matrix-prefix", dest="fmaprefix", type=str,
                           help="prefix of OD matrix files in visum format")
    argParser.add_argument("-N", "--clone-postfix", dest="clonepostfix", type=str,
                           default='-CLONE', help="postfix attached to clone ids")
    argParser.add_argument("-X", "--cntfirstlink", action="store_true", dest="cntfirstlink",
                           default=False, help="if entering vehicles are assumed to cross the upstream sensor of" +
                                               "their entry link")
    argParser.add_argument("-K", "--cntlastlink", action="store_true", dest="cntlastlink",
                           default=False, help="if exiting vehicles are assumed to cross the upstream sensor of" +
                           "their exit link")
    argParser.add_argument("remaining_args", nargs='*')
    return argParser


def main():
    argParser = initOptions()

    options = argParser.parse_args()
    if not options.net or not options.routes or not options.detvals:
        argParser.error("--net-file, --routes and --detector-values have to be given!")
    if "cadyts" not in options.classpath:
        print("""Warning! No cadyts.jar has been found or given. Please use the --classpath option
 to point to an existing jar file. You may also download it
 from https://sumo.dlr.de/daily/cadyts.jar and place it in %s/bin.""" % os.environ['SUMO_HOME'], file=sys.stderr)
    sumoBinary = sumolib.checkBinary("sumo", options.path)
    calibrator = ["java", "-cp", options.classpath, "-Xmx1G",
                  "floetteroed.cadyts.interfaces.sumo.SumoController"]
    log = open("cadySumo-log.txt", "w+")

    # calibration init
    starttime = datetime.now()
    evalprefix = None
    if options.evalprefix:
        evalprefix = options.evalprefix
    EDGEDATA_ADD = "edgedata.add.xml"

    # begin the calibration
    if options.fmaprefix:
        call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
                           "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
                           "-bruteforce", options.bruteforce, "-demandscale", options.demandscale,
                           "-mincountstddev", options.mincountstddev, "-overridett", options.overridett,
                           "-clonepostfix", options.clonepostfix, "-fmaprefix", options.fmaprefix,
                           "-cntfirstlink", options.cntfirstlink, "-cntlastlink", options.cntlastlink], log)
    else:
        call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
                           "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
                           "-bruteforce", options.bruteforce, "-demandscale", options.demandscale,
                           "-mincountstddev", options.mincountstddev, "-overridett", options.overridett,
                           "-clonepostfix", options.clonepostfix, "-cntfirstlink", options.cntfirstlink,
                           "-cntlastlink", options.cntlastlink], log)
    #
    generateEdgedataAddFile(EDGEDATA_ADD, options)
    for step in range(options.calibStep):
        print('calibration step:', step)
        files = []
        current_directory = os.getcwd()
        final_directory = os.path.join(current_directory, "%03i" % step)
        if not os.path.exists(final_directory):
            os.makedirs(final_directory)

        # calibration choice
        firstRoute = options.routes.split(",")[0]
        routname = os.path.basename(firstRoute)
        if '_' in routname:
            output = "%s_%03i.cal.xml" % (routname[:routname.rfind('_')], step)
        else:
            output = "%s_%03i.cal.xml" % (routname[:routname.find('.')], step)

        call(calibrator + ["CHOICE", "-choicesetfile",
                           options.routes, "-choicefile", "%s/%s" % (step, output)], log)
        files.append(output)

        # simulation
        print(">> Running simulation")
        btime = datetime.now()
        print(">>> Begin time: %s" % btime)
        writeSUMOConf(sumoBinary, step, options, [], ",".join(files))
        call([sumoBinary, "-c", "%03i/iteration_%03i.sumocfg" % (step, step)], log)
        etime = datetime.now()
        print(">>> End time: %s" % etime)
        print(">>> Duration: %s" % (etime - btime))
        print("<<")

        # calibration update
        if evalprefix:
            call(calibrator + ["UPDATE", "-netfile", "%s/dump_%03i_%s.xml" % (
                step, step, options.aggregation), "-flowfile", "%s_%03i.txt" % (evalprefix, step)], log)
        else:
            call(calibrator + ["UPDATE", "-netfile",
                               "%03i/dump_%03i_%s.xml" % (step, step, options.aggregation)], log)
        print("< Step %s ended (duration: %s)" %
              (step, datetime.now() - btime))
        print("------------------\n")
        log.flush()

    print("calibration ended (duration: %s)" % (datetime.now() - starttime))
    log.close()


pyPath = os.path.abspath(os.path.dirname(sys.argv[0]))
if __name__ == "__main__":
    main()
