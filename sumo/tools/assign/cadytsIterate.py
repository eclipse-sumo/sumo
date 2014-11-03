#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    cadytsIterate.py
@author  Jakob Erdmann
@author  Yun-Pang Floetteroed
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2010-09-15
@version $Id$

Run cadyts to calibrate the simulation with given routes and traffic measurements.
Respective traffic zones information has to exist in the given route files.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, sys, subprocess, types
from datetime import datetime
from optparse import OptionParser
from duaIterate import call, writeSUMOConf, addGenericOptions

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
import sumolib

def initOptions():
    optParser = OptionParser()
    addGenericOptions(optParser)

    optParser.add_option("-r", "--route-alternatives", dest="routes",
                         help="route alternatives from sumo (comma separated list, mandatory)", metavar="FILE")
    optParser.add_option("-d", "--detector-values", dest="detvals",
                         help="adapt to the flow on the given edges", metavar="FILE")
    optParser.add_option("-c", "--classpath", dest="classpath",
                         default=os.path.join(os.path.dirname(sys.argv[0]), "..", "contributed", "calibration", "cadytsSumoController.jar"),
                         help="classpath for the calibrator [default: %default]")
    optParser.add_option("-l", "--last-calibration-step", dest="calibStep",
                         type="int", default=100, help="last step of the calibration [default: %default]")
    optParser.add_option("-S", "--demandscale", dest="demandscale", type="float", default=2., help="scaled demand [default: %default]")
    optParser.add_option("-F", "--freezeit",  dest="freezeit",
                         type="int", default=85, help="define the number of iterations for stablizing the results in the DTA-calibration")
    optParser.add_option("-V", "--varscale",  dest="varscale",
                         type="float", default=1., help="define variance of the measured traffic flows for the DTA-calibration")
    optParser.add_option("-P", "--PREPITS",  type="int", dest="PREPITS",
                         default = 5, help="number of preparatory iterations")
    optParser.add_option("-W", "--evaluation-prefix", dest="evalprefix",type='string',
                         help="prefix of flow evaluation files; only for the calibration with use of detector data")
    optParser.add_option("-Y", "--bruteforce", action="store_true", dest="bruteforce",
                         default = False, help="fit the traffic counts as accurate as possible")
    optParser.add_option("-Z", "--mincountstddev", type="float", dest="mincountstddev",
                         default = 25., help="minimal traffic count standard deviation")
    optParser.add_option("-O", "--overridett", action="store_true", dest="overridett",
                         default= False, help="override depart times according to updated link travel times")
    optParser.add_option("-E", "--disable-summary", "--disable-emissions", action="store_true", dest="noSummary",                    
                         default=False, help="No summaries are written by the simulation")                                                 
    optParser.add_option("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                         default=False, help="No tripinfos are written by the simulation")
    optParser.add_option("-M", "--matrix-prefix", dest="fmaprefix", type='string',
                         help="prefix of OD matrix files in visum format")
    optParser.add_option("-N", "--clone-postfix", dest="clonepostfix", type='string',
                         default='-CLONE', help="postfix attached to clone ids")
    optParser.add_option("-X", "--cntfirstlink", action="store_true", dest="cntfirstlink",
                         default=False, help="if entering vehicles are assumed to cross the upstream sensor of their entry link")
    optParser.add_option("-K", "--cntlastlink", action="store_false", dest="cntlastlink",
                         default=True, help="if exiting vehicles are assumed to cross the upstream sensor of their exit link")
    return optParser

def main():
    optParser = initOptions()

    (options, args) = optParser.parse_args()
    if not options.net or not options.routes or not options.detvals:
        optParser.error("--net-file, --routes and --detector-values have to be given!")

    if options.mesosim:
        sumoBinary = sumolib.checkBinary("meso", options.path)
    else:
        sumoBinary = sumolib.checkBinary("sumo", options.path)
    calibrator = ["java", "-cp", options.classpath, "cadyts.interfaces.sumo.SumoController"]
    log = open("cadySumo-log.txt", "w+")

    # calibration init
    starttime = datetime.now()
    evalprefix = None
    if options.evalprefix:
        evalprefix = options.evalprefix

    # begin the calibration
    if options.fmaprefix:
        call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
              "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
               "-bruteforce", options.bruteforce, "-demandscale", options.demandscale,
               "-mincountstddev", options.mincountstddev, "-overridett", options.overridett,"-clonepostfix", options.clonepostfix,
                "-fmaprefix", options.fmaprefix, "-cntfirstlink", options.cntfirstlink, "-cntlastlink",options.cntlastlink], log)
    else:
        call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
              "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
              "-bruteforce", options.bruteforce, "-mincountstddev", options.mincountstddev,"-overridett", options.overridett,
              "-clonepostfix", options.clonepostfix,"-cntfirstlink", options.cntfirstlink, "-cntlastlink",options.cntlastlink], log)

    for step in range(options.calibStep):
        print 'calibration step:', step
        files = []

        # calibration choice
        firstRoute = options.routes.split(",")[0]
        routname = os.path.basename(firstRoute)
        if '_' in routname:
            output = "%s_%03i.cal.xml" % (routname[:routname.rfind('_')], step)
        else:
            output = "%s_%03i.cal.xml" % (routname[:routname.find('.')], step)

        call(calibrator + ["CHOICE", "-choicesetfile", options.routes, "-choicefile", "%s" % output], log)
        files.append(output)
    
        # simulation
        print ">> Running simulation"
        btime = datetime.now()
        print ">>> Begin time: %s" % btime
        writeSUMOConf(sumoBinary, step, options, [], ",".join(files))
        retCode = call([sumoBinary, "-c", "iteration_%03i.sumocfg" % step], log)
        etime = datetime.now()
        print ">>> End time: %s" % etime
        print ">>> Duration: %s" % (etime-btime)
        print "<<"
        
        # calibration update
        if evalprefix:
            call(calibrator + ["UPDATE", "-netfile", "dump_%03i_%s.xml" % (step, options.aggregation), "-flowfile", "%s_%03i.txt" % (evalprefix, step)], log)
        else:
            call(calibrator + ["UPDATE", "-netfile", "dump_%03i_%s.xml" % (step, options.aggregation)], log)
        print "< Step %s ended (duration: %s)" % (step, datetime.now() - btime)
        print "------------------\n"
        log.flush()
            
    print "calibration ended (duration: %s)" % (datetime.now() - starttime)
    log.close()

if __name__ == "__main__":
    main()
