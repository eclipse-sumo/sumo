#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    cadyts-iterate.py
@author  Yun-Pang.Wang@dlr.de
@date    2010-09-15
@version $Id: cadyts-iterate.py 9061 2010-09-07 12:24:28Z yunpangwang $

Run cadyts to calibrate the simulation with given routes and traffic measurements.
Respective traffic zones information has to exist in the given route files.

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
    print >> fd, '   <no-internal-links value="%s"/>' % options.internallink
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
                     help="trips file for adding taz information in routes files (mandatory)", metavar="FILE")
optParser.add_option("-r", "--routes", dest="routes",
                     help="routes (mandatory)", metavar="FILE")
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

optParser.add_option("-m", "--mesosim", action="store_true", dest="mesosim",
                     default=False, help="Whether mesosim shall be used")
optParser.add_option("--inc-base", dest="incBase",
                     type="int", default=-1, help="Give the incrementation base")
optParser.add_option("--incrementation", dest="incValue",
                     type="int", default=1, help="Give the incrementation")
optParser.add_option("--time-inc", dest="timeInc",
                     type="int", default=0, help="Give the time incrementation")
optParser.add_option("-I", "--nointernal-link", action="store_true", dest="internallink",
                     default = False, help="not to simulate internal link: true or false")
optParser.add_option("-y", "--absrand", dest="absrand", action="store_true",
                     default= False, help="use current time to generate random number")
optParser.add_option("-p", "--path", dest="path",
                     default=os.environ.get("SUMO", ""), help="Path to binaries [default: %default]")

optParser.add_option("-d", "--detector-values", dest="detvals",
                     help="adapt to the flow on the given edges", metavar="FILE")
optParser.add_option("-c", "--classpath", dest="classpath",
                     default=os.path.join(os.path.dirname(sys.argv[0]), "..", "contributed", "calibration", "cadytsSumoController.jar"),
                     help="classpath for the calibrator [default: %default]")
optParser.add_option("-s", "--last-calibration-step", dest="calibStep",
                     type="int", default=100, help="last step of the calibration [default: %default]")

optParser.add_option("-S", "--demandscale", dest="demandscale", type="float", default=2., help="scaled demand [default: %default]")
optParser.add_option("-o", "--od-matrix", dest="odmatrix",
                     help="sent estimated O-D matrix to", metavar="FILE")

optParser.add_option("-F", "--freezeit",  dest="freezeit",
                     type="int", default=85, help="define the number of iterations for stablizing the results in the DTA-calibration")
optParser.add_option("-V", "--varscale",  dest="varscale",
                     type="float", default=1., help="define variance of the measured traffi flows for the DTA-calibration")
optParser.add_option("-P", "--PREPITS",  type="int", dest="PREPITS",
                     default = 5, help="number of preparatory iterations")
optParser.add_option("-W", "--flows-evaluation", dest="flowevaluation",
                     help="sent flow evaluation to", metavar="FILE")
optParser.add_option("-X", "--measformat",  type="choice", dest="measformat",
                     choices=('SUMO', 'Cadyts'), 
                     default = 'Cadyts',help="choose measurement format: SUMO or Cadyts")
optParser.add_option("-Y", "--bruteforce", action="store_true", dest="bruteforce",
                     default = False, help="fit the traffic counts as accurate as possible")
optParser.add_option("-Z", "--mincountstddev", type="float", dest="mincountstddev",
                     default = 25., help="minimal traffic count standard deviation")
optParser.add_option("-O", "--overridett", action="store_true", dest="overridett",
                     default= False, help="override depart times according to updated link travel times", metavar="FILE")
optParser.add_option("-z", "--equiprate", type="float", dest="equiprate",
                     default = 0., help="vehicle re-identification equipment rate [default: %default]")


(options, args) = optParser.parse_args()
if not options.net or not options.detvals:
    optParser.error("--net-file, --routes and --detector-values have to be given!")

duaBinary = os.environ.get("DUAROUTER_BINARY", os.path.join(options.path, "duarouter"))
if options.mesosim:
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "meso"))
else:
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "sumo"))
calibrator = ["java", "-cp", options.classpath, "cadyts.interfaces.sumo.SumoController"]
log = open("cadySumo-log.txt", "w+")

# calibration init
starttime = datetime.now()
evalprefix = None
if options.flowevaluation:
    evalprefix = options.flowevaluation[:options.flowevaluation.rfind('.')]
check = 0
routeFiles = options.routes.split(',') 

fileslist = list(routeFiles)
#TODO: remove when taz information are generated automatically by od2trips as default
#if options.odmatrix:
#    import addTaz
#    fileslist = []
#    for routeFile in routeFiles:
#        prefix = routeFile[:routeFile.find(".")]
#        fd = open(prefix + ".taz.xml", 'w')
#        alts = routeFile
#        addTaz.parse(tripFile, alts, fd)
#        fd.close()
#        fileslist.append(fd.name)

# TODO: does not work with Cadyts yet: Cadyts cannot parse the routeEntityFile
#routeEntityFile = 'routesEntityDef.xml'
#foutf = open(routeEntityFile, 'w')
#foutf.write('<?xml version="1.0"?>\n')
#foutf.write('<!DOCTYPE routeSets [\n')
#for i, p in enumerate(fileslist):
#    foutf.write('  <!ENTITY routeset_%s SYSTEM "%s">\n' % (i, p))
#foutf.write(']>\n')
#foutf.write('<routeSets>\n')
#for j in range(0, i+1):
#    foutf.write('    &routeset_%s;\n' % j)
#foutf.write('</routeSets>')
#foutf.close()
routeEntityFile = fileslist[0] # TODO: if the parse with many files works with Cadyts, this line should be removed.
prefix = fileslist[0][:fileslist[0].find(".")]

# begin the calibration
if options.odmatrix:
    check = call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
          "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
           "-measformat", options.measformat, "-bruteforce", options.bruteforce, "-demandscale", options.demandscale,
           "-mincountstddev", options.mincountstddev, "-overridett", options.overridett, "-equiprate", options.equiprate], log)
else:
    check = call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
          "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
           "-measformat", options.measformat, "-bruteforce", options.bruteforce, "-mincountstddev", options.mincountstddev,
           "-overridett", options.overridett, "-equiprate", options.equiprate], log)

if check != 0 and check != None:
    print 'KATASTROPHE! calibration exit code = ', check

for step in range(0, options.calibStep):
    print 'calibration step:', step
    files = []

    # calibration choice
    output = "%s_%s.cal.xml" % (prefix, step)
    if options.odmatrix:
        matrixfile = options.odmatrix[:options.odmatrix.rfind('.')] + '_%s.xml' % step
        call(calibrator + ["CHOICE", "-choicesetfile", routeEntityFile, "-choicefile", "%s" % output, "-odmatrix", matrixfile], log)
    else:
        call(calibrator + ["CHOICE", "-choicesetfile", routeEntityFile, "-choicefile", "%s" % output], log)
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
    if evalprefix:
        call(calibrator + ["UPDATE", "-netfile", "dump_%s_%s.xml" % (step, options.aggregation), "-flowfile", "%s_%s.txt" % (evalprefix, step)], log)
    else:
        call(calibrator + ["UPDATE", "-netfile", "dump_%s_%s.xml" % (step, options.aggregation)], log)
    print "< Step %s ended (duration: %s)" % (step, datetime.now() - btime)
    print "------------------\n"
    log.flush()
        
print "calibration ended (duration: %s)" % (datetime.now() - starttime)
log.close()