#!/usr/bin/env python
"""
@file    dua_iterate.py
@author  Daniel.Krajzewicz@dlr.de,Michael.Behrisch@dlr.de
@date    2008-02-13
@version $Id$

Run duarouter and sumo alternating to perform a dynamic user assignment.
Based on the Perl script dua_iterate.pl.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os, sys, subprocess, types
from datetime import datetime
from optparse import OptionParser

def call(command, log):
    if not isinstance(args, types.StringTypes):
        command = [str(c) for c in command] 
    print >> log, "-" * 79
    print >> log, command
    log.flush()
    retCode = subprocess.call(command, stdout=log, stderr=log)
    if retCode != 0:
        print >> sys.stderr, "Execution of %s failed. Look into %s for details." % (command, log.name)
        sys.exit(retCode) 

def writeRouteConf(step, options, file, output, withExitTimes):
    fd = open("iteration_" + str(step) + ".rou.cfg", "w")
    print >> fd, """<configuration>
    <input>
        <net-file value="%s"/>""" % options.net
    if(step==0):
        print >> fd, '        <trip-defs value="%s"/>' % file
    else:
        print >> fd, '        <alternatives value="%s"/>' % file
        print >> fd, '        <weights value="dump_%s_%s.xml"/>' % (step-1, options.aggregation)
    print >> fd, """    </input>
    <output>
        <output-file value="%s"/>
        <exit-times value="%s"/>
    </output>""" % (output, withExitTimes)
    print >> fd, """    <processing>
        <continue-on-unbuild value="%s"/>
        <gBeta value="%s"/>
        <gA value="%s"/>
    </processing>""" % (options.continueOnUnbuild, options.gBeta, options.gA)
    print >> fd, '    <random_number><abs-rand value="%s"/></random_number>' % options.absrand
    print >> fd, '    <time><begin value="%s"/>' % options.begin,
    if options.end:
        print >> fd, '<end value="%s"/>' % options.end,
    print >> fd, """</time>
    <report>
        <verbose value="True"/>
        <suppress-warnings value="%s"/>
    </report>
</configuration>""" % options.noWarnings
    fd.close()

def writeSUMOConf(step, options, files):
    fd = open("iteration_" + str(step) + ".sumo.cfg", "w")
    add = ""
    if options.additional != "":
        add = "," + options.additional
    print >> fd, """<configuration>
    <input>
        <net-file value="%s"/>
        <route-files value="%s"/>
        <additional-files value="dua_dump_%s.add.xml%s"/>
    </input>
    <output>""" % (options.net, files, step, add)
    if not options.noEmissions:
        print >> fd, '        <emissions-output value="emissions_%s.xml"/>' % step
    if not options.noTripinfo:
        print >> fd, '        <tripinfo-output value="tripinfo_%s.xml"/>' % step
    if options.routefile == "routesonly":
        print >> fd, '         <vehroute-output value="vehroute_%s.xml"/>' % step
    elif options.routefile == "detailed":
        print >> fd, '         <vehroute-output value="vehroute_%s.xml"/>' % step
        print >> fd, '         <vehroute-output.exit-times value="True"/>'
    print >> fd, "    </output>"
    print >> fd, '    <random_number><abs-rand value="%s"/></random_number>' % options.absrand
    print >> fd, '    <time><begin value="%s"/>' % options.begin,
    if options.timeInc:
        print >> fd, '<end value="%s"/>' % int(options.timeInc * (step + 1)),
    elif options.end:
        print >> fd, '<end value="%s"/>' % options.end,
    print >> fd, """</time>
    <processing>
        <route-steps value="%s"/>""" % options.routeSteps
    if options.incBase>0:
        print >> fd, """        <incremental-dua-step value="%s"/>
        <incremental-dua-base value="%s"/>""" % (options.incValue*(step+1), options.incBase)
    if options.mesosim:
        print >> fd, '        <mesosim value="True"/>'
    print >> fd, """</processing>
    <report>
        <verbose value="True"/>
        <suppress-warnings value="%s"/>
    </report>
</configuration>""" % options.noWarnings
    fd.close()
    fd = open("dua_dump_%s.add.xml" % step, "w")
    print >> fd, """<a>
    <meandata-edge id="dump_%s_%s" freq="%s" file="dump_%s_%s.xml"/>
</a>""" % (step, options.aggregation, options.aggregation, step, options.aggregation)
    fd.close()


optParser = OptionParser()
optParser.add_option("-C", "--continue-on-unbuild", action="store_true", dest="continueOnUnbuild",
                     default=False, help="continues on unbuild routes")
optParser.add_option("-w", "--disable-warnings", action="store_true", dest="noWarnings",
                     default=False, help="disables warnings")

optParser.add_option("-n", "--net-file", dest="net",
                     help="SUMO network (mandatory)", metavar="FILE")
optParser.add_option("-t", "--trips", dest="trips",
                     help="trips in step 0 (mandatory)", metavar="FILE")
optParser.add_option("-+", "--additional", dest="additional",
                     default="", help="Additional files")

optParser.add_option("-b", "--begin", dest="begin",
                     type="int", default=0, help="Set simulation/routing begin [default: %default]")
optParser.add_option("-e", "--end", dest="end",
                     type="int", help="Set simulation/routing end [default: %default]")
optParser.add_option("-R", "--route-steps", dest="routeSteps",
                     type="int", default=200, help="Set simulation route steps [default: %default]")
optParser.add_option("-a", "--aggregation", dest="aggregation",
                     type="int", default=900, help="Set main weights aggregation period [default: %default]")
optParser.add_option("-A", "--gA", dest="gA",
                     type="float", default=.5, help="Sets Gawron's Alpha [default: %default]")
optParser.add_option("-B", "--gBeta", dest="gBeta",
                     type="float", default=.9, help="Sets Gawron's Beta [default: %default]")

optParser.add_option("-E", "--disable-emissions", action="store_true", dest="noEmissions",
                     default=False, help="No emissions are written by the simulation")
optParser.add_option("-T", "--disable-tripinfos", action="store_true", dest="noTripinfo",
                     default=False, help="No tripinfos are written by the simulation")
optParser.add_option("-m", "--mesosim", action="store_true", dest="mesosim",
                     default=False, help="Whether mesosim shall be used")
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
optParser.add_option("-p", "--path", dest="path",
                     default=os.environ.get("SUMO", ""), help="Path to binaries [default: %default]")

optParser.add_option("-d", "--detector-values", dest="detvals",
                     help="adapt to the flow on the given edges", metavar="FILE")
optParser.add_option("-c", "--classpath", dest="classpath",
                     default=os.path.join(os.path.dirname(sys.argv[0]), "..", "contributed", "calibration", "Cadyts.jar"),
                     help="classpath for the calibrator [default: %default]")
optParser.add_option("-s", "--first-calibration-step", dest="calibStep",
                     type="int", default=10, help="step at which to start calibration [default: %default]")

optParser.add_option("-S", "--demandscale", type="float", default=1., help="scaled demand [default: %default]")
optParser.add_option("-o", "--od-matrix", dest="odmatrix",
                     help="sent estimated O-D matrix to", metavar="FILE")
                     
optParser.add_option("-y", "--absrand", dest="absrand", action="store_true",
                     default= False, help="use current time to generate random number")
                     
optParser.add_option("-F", "--freezeit",  dest="freezeit",
                     type="int", default=1000, help="define the number of iterations for stablizing the results in the DTA-calibration")
optParser.add_option("-V", "--varscale",  dest="varscale",
                     type="float", default=1., help="define variance of the measured traffi flows for the DTA-calibration")

optParser.add_option("-x", "--vehroute-file",  dest="routefile", type="choice",
                     choices=('None', 'routesonly', 'detailed'), 
                     default = 'None', help="choose OD type")

(options, args) = optParser.parse_args()
if not options.net or not options.trips:
    optParser.error("At least --net-file and --trips have to be given!")

duaBinary = os.environ.get("DUAROUTER_BINARY", os.path.join(options.path, "duarouter"))
if options.mesosim:
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "meso"))
else:
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "sumo"))
calibrator = ["java", "-cp", options.classpath, "cadyts.interfaces.sumo.SumoController"]
log = open("dua-log.txt", "w+")
tripFiles = options.trips.split(",")
starttime = datetime.now()
for step in range(options.firstStep, options.lastStep):
    btimeA = datetime.now()
    print "> Executing step %s" % step

    # calibration init
    doCalibration = options.detvals != None and step >= options.calibStep
    if options.detvals and step == options.calibStep:
        if options.odmatrix:
            call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
                  "-measfile", options.detvals, "-binsize", options.aggregation,
                  "-odmatrix", options.odmatrix, "-demandscale", options.demandscale], log)
        else:
            call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
                  "-measfile", options.detvals, "-binsize", options.aggregation], log)
    # router
    files = []
    for tripFile in tripFiles:
        file = tripFile
        tripFile = os.path.basename(tripFile)
        if step>0:
            file = tripFile[:tripFile.find(".")] + "_%s.rou.alt.xml" % (step-1)
        output = tripFile[:tripFile.find(".")] + "_%s.rou.xml" % step
        print ">> Running router with " + file
        btime = datetime.now()
        print ">>> Begin time: %s" % btime
        writeRouteConf(step, options, file, output, doCalibration)
        retCode = call([duaBinary, "-c", "iteration_%s.rou.cfg" % step], log)
        etime = datetime.now()
        print ">>> End time: %s" % etime
        print ">>> Duration: %s" % (etime-btime)
        print "<<"
        # calibration choice
        if doCalibration:
            alts = output[:-4] + ".alt.xml"
            if options.odmatrix:
                import addTaz
                fd = open(output[:-4] + ".taz.xml", 'w')
                addTaz.parse(tripFile, alts, fd)
                fd.close()
                alts = fd.name
            call(calibrator + ["CHOICE", "-choicesetfile", alts, "-choicefile", "%s.cal.xml" % output[:-4]], log)
            output = output[:-4] + ".cal.xml"
        files.append(output)
    # simulation
    print ">> Running simulation"
    btime = datetime.now()
    print ">>> Begin time: %s" % btime
    writeSUMOConf(step, options, ",".join(files))
    retCode = call([sumoBinary, "-c", "iteration_%s.sumo.cfg" % step], log)
    etime = datetime.now()
    print ">>> End time: %s" % etime
    print ">>> Duration: %s" % (etime-btime)
    print "<<"
    # calibration update
    if doCalibration: 
        call(calibrator + ["UPDATE", "-netfile", "dump_%s_%s.xml" % (step, options.aggregation)], log)
    print "< Step %s ended (duration: %s)" % (step, datetime.now() - btimeA)
    print "------------------\n"
    log.flush()
print "dua-iterate ended (duration: %s)" % (datetime.now() - starttime)

log.close()
