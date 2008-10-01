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
import os, sys, subprocess
from datetime import datetime
from optparse import OptionParser

class TeeFile:
    """A helper class which allows simultaneous writes to several files"""
    def __init__(self, *files):
        self.files = files
    def write(self, txt):
        """Writes the text to all files"""
        for fp in self.files:
            fp.write(txt)

def writeRouteConf(step, options, file, output):
    fd = open("iteration_" + str(step) + ".rou.cfg", "w")
    fd.write("<configuration>\n")
    fd.write("<remove-loops>x</remove-loops>\n") # !!!
    fd.write("   <files>\n")
    fd.write("      <net-file>" + options.net + "</net-file>\n")
    fd.write("      <output>" + output + "</output>\n")
    if(step==0):
        fd.write("      <t>" + file + "</t>\n")
    else:
        fd.write("      <alternatives>" + file + "</alternatives>\n")
        fd.write("      <weights>dump_" + str(step-1) + "_" + str(options.aggregation) + ".xml</weights>\n")
    fd.write("   </files>\n")
    fd.write("   <process>\n")
    fd.write("      <begin>" + str(options.begin) + "</begin>\n")
    fd.write("      <end>" + str(options.end) + "</end>\n")
    fd.write("   </process>\n")
    fd.write("   <reports>\n")
    if options.verbose:
        fd.write("      <verbose>x</verbose>\n")
    if options.continueOnUnbuild:
        fd.write("      <continue-on-unbuild>x</continue-on-unbuild>\n")
    if not options.withWarnings:
        fd.write("      <suppress-warnings>x</suppress-warnings>\n")
    fd.write("   </reports>\n")
    fd.write("</configuration>\n")
    fd.close()

def writeSUMOConf(step, options, files):
    fd = open("iteration_" + str(step) + ".sumo.cfg", "w")
    fd.write("<configuration>\n")
    fd.write("   <files>\n")
    fd.write("      <net-file>" + options.net + "</net-file>\n")
    fd.write("      <route-files>" + files + "</route-files>\n")
    fd.write("      <dump-basename>dump_" + str(step) + "</dump-basename>\n")
    fd.write("      <dump-intervals>" + str(options.aggregation) + "</dump-intervals>\n")
    if not options.noEmissions:
        fd.write("      <emissions>emissions_" + str(step) + ".xml</emissions>\n")
    if not options.noTripinfo:
        fd.write("      <tripinfo>tripinfo_" + str(step) + ".xml</tripinfo>\n")
    if options.additional!="":
        fd.write("      <additional-files>" + options.additional + "</additional-files>\n")
    fd.write("   </files>\n")
    fd.write("   <process>\n")
    fd.write("      <begin>" + str(options.begin) + "</begin>\n")
    if not options.timeInc:
        fd.write("      <end>" + str(options.end) + "</end>\n")
    else:
        endTime = int(options.timeInc * (step + 1))
        fd.write("      <end>" + str(endTime) + "</end>\n")
    fd.write("      <route-steps>" + str(options.routeSteps) + "</route-steps>\n")
    if options.incBase>0:
        fd.write("      <incremental-dua-base>" + str(options.incBase) + "</incremental-dua-base>\n")
        fd.write("      <incremental-dua-step>" + str(options.incValue*(step+1)) + "</incremental-dua-step>\n")
    if options.mesosim:
        fd.write("      <mesosim>x</mesosim>\n")
    fd.write("   </process>\n")
    fd.write("   <reports>\n")
    if options.verbose:
        fd.write("      <verbose>x</verbose>\n")
    if not options.withWarnings:
        fd.write("      <suppress-warnings>x</suppress-warnings>\n")
    fd.write("   </reports>\n")
    fd.write("</configuration>\n")
    fd.close()

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-C", "--continue-on-unbuild", action="store_true", dest="continueOnUnbuild",
                     default=False, help="continues on unbuild routes")
optParser.add_option("-W", "--with-warnings", action="store_true", dest="withWarnings",
                     default=False, help="enables warnings")

optParser.add_option("-n", "--net-file", dest="net",
                     help="SUMO network (mandatory)", metavar="FILE")
optParser.add_option("-t", "--trips", dest="trips",
                     help="trips in step 0", metavar="FILE")
optParser.add_option("-+", "--additional", dest="additional",
                     default="", help="Additional files")

optParser.add_option("-b", "--begin", dest="begin",
                     type="int", default=0, help="Set simulation/routing begin")
optParser.add_option("-e", "--end", dest="end",
                     type="int", default=86400, help="Set simulation/routing end")
optParser.add_option("-R", "--route-steps", dest="routeSteps",
                     type="int", default=200, help="Set simulation route steps")
optParser.add_option("-a", "--aggregation", dest="aggregation",
                     type="int", default=900, help="Set main weights aggregation period")
optParser.add_option("-A", "--gA", dest="gA",
                     type="float", default=.5, help="Sets Gawron's Alpha")
optParser.add_option("-B", "--gBeta", dest="gBeta",
                     type="float", default=.9, help="Sets Gawron's Beta")

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
                     type="int", default=0, help="First DUA step")
optParser.add_option("-l", "--last-step", dest="lastStep",
                     type="int", default=50, help="Last DUA step")
optParser.add_option("-p", "--path", dest="path",
                     default=os.environ.get("SUMO", ""), help="Path to binaries")
(options, args) = optParser.parse_args()


if (sys.platform=="win32"):        
    duaBinary = os.path.join(options.path, "duarouter.exe")
    if options.mesosim:
        sumoBinary = os.path.join(options.path, "meso.exe")
    else:
        sumoBinary = os.path.join(options.path, "sumo.exe")
else:
    duaBinary = os.path.join(options.path, "sumo-duarouter")
    if options.mesosim:
        sumoBinary = os.path.join(options.path, "meso")
    else:
        sumoBinary = os.path.join(options.path, "sumo")
log = open("dua-log.txt", "w")
logQuiet = open("dua-log-quiet.txt", "w")
sys.stdout = TeeFile(sys.stdout, logQuiet)
sys.stderr = TeeFile(sys.stderr, logQuiet)
tripFiles = options.trips.split(",")
starttime = datetime.now()
for step in range(options.firstStep, options.lastStep):
    btimeA = datetime.now()
    print "> Executing step " + str(step)

    # router
    files = []
    for tripFile in tripFiles:
        file = tripFile
        tripFile = os.path.basename(tripFile)
        if step>0:
            file = tripFile[:tripFile.find(".")] + "_%s.rou.xml.alt" % (step-1)
        output = tripFile[:tripFile.find(".")] + "_%s.rou.xml" % step
        print ">> Running router with " + file
        btime = datetime.now()
        print ">>> Begin time %s" % btime
        writeRouteConf(step, options, file, output)
        if options.verbose:
            print "> Call: %s -c iteration_%s.rou.cfg" % (duaBinary, step)
            subprocess.call("%s -c iteration_%s.rou.cfg" % (duaBinary, step),
                            shell=True, stdout=TeeFile(sys.__stdout__, log),
                            stderr=TeeFile(sys.__stderr__, log))
        else:
            subprocess.call("%s -c iteration_%s.rou.cfg" % (duaBinary, step),
                            shell=True, stdout=log, stderr=log)
        etime = datetime.now()
        print ">>> End time %s" % etime
        print ">>> Duration %s" % (etime-btime)
        print "<<"
        files.append(output)

    # simulation
    print ">> Running simulation"
    btime = datetime.now()
    print ">>> Begin time %s" % btime
    writeSUMOConf(step, options, ",".join(files))
    if options.verbose:
        print "> Call: %s -c iteration_%s.sumo.cfg" % (sumoBinary, step)
        subprocess.call("%s -c iteration_%s.sumo.cfg" % (sumoBinary, step),
                        shell=True, stdout=TeeFile(sys.__stdout__, log),
                        stderr=TeeFile(sys.__stderr__, log))
    else:
        subprocess.call("%s -c iteration_%s.sumo.cfg" % (sumoBinary, step),
                        shell=True, stdout=log, stderr=log)
    etime = datetime.now()
    print ">>> End time %s" % etime
    print ">>> Duration %s" % (etime-btime)
    print "<<"
    print "< Step %s ended (duration: %s)" % (step, datetime.now() - btimeA)
    print "------------------\n"
print "dua-iterate ended (duration: %s)" % (datetime.now() - starttime)

log.close()
logQuiet.close()
