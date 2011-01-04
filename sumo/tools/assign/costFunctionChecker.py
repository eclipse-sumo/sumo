#!/usr/bin/env python
"""
@file    costFunctionChecker.py
@author  Michael.Behrisch@dlr.de
@date    2009-08-31
@version $Id$

Run duarouter repeatedly and simulate weight changes via a cost function.

Copyright (C) 2009-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, sys, subprocess, types
from datetime import datetime
from optparse import OptionParser
from xml.sax import make_parser, handler

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

def writeRouteConf(step, options, file, output):
    fd = open("iteration_" + str(step) + ".rou.cfg", "w")
    print >> fd, """<configuration>
    <input>
        <net-file value="%s"/>""" % options.net
    if step==0:
        if options.flows:
            print >> fd, '        <flow-definition value="%s"/>' % file
        else:
            print >> fd, '        <trip-defs value="%s"/>' % file
    else:
        print >> fd, '        <alternatives value="%s"/>' % file
        print >> fd, '        <weights value="dump_%s_%s.xml"/>' % (step-1, options.aggregation)
    print >> fd, """    </input>
    <output>
        <output-file value="%s"/>
        <exit-times value="True"/>
    </output>""" % output
    print >> fd, """    <processing>
        <continue-on-unbuild value="%s"/>
        <expand-weights value="True"/>
        <gBeta value="%s"/>
        <gA value="%s"/>
    </processing>""" % (options.continueOnUnbuild, options.gBeta, options.gA)
    print >> fd, '    <random_number><abs-rand value="%s"/></random_number>' % options.absrand
    print >> fd, '    <time><begin value="%s"/>' % options.begin,
    if options.end:
        print >> fd, '<end value="%s"/>' % options.end,
    print >> fd, """</time>
    <report>
        <verbose value="%s"/>
        <suppress-warnings value="%s"/>
    </report>
</configuration>""" % (options.verbose, options.noWarnings)
    fd.close()

class RouteReader(handler.ContentHandler):

    def __init__(self):
        self._edgeWeights = {}
        self._maxDepart = 0

    def startElement(self, name, attrs):
        if name == 'route':
            for edge in attrs['edges'].split():
                if not edge in self._edgeWeights:
                    self._edgeWeights[edge] = 0
                self._edgeWeights[edge] += 1
        elif name == 'vehicle':
            if float(attrs['depart']) > self._maxDepart:
                self._maxDepart = float(attrs['depart'])

    def getWeight(self, edge):
        return self._edgeWeights.get(edge, 0)

    def getMaxDepart(self):
        return self._maxDepart

class NetReader(handler.ContentHandler):

    def __init__(self):
        self._edges = []

    def startElement(self, name, attrs):
        if name == 'edge':
            if attrs['function'] == 'normal':
                self._edges.append(attrs['id'])

    def getEdges(self):
        return self._edges

def identity(edge, weight):
    return weight

def generateWeights(step, options, edges, weights, costFunction):
    fd = open("dump_%s_%s.xml" % (step, options.aggregation), "w")
    print >> fd, '<?xml version="1.0"?>\n<netstats>'
    for time in range(0, int(reader.getMaxDepart()+1), options.aggregation):
        print >> fd, '    <interval begin="%s" end="%s" id="dump_%s">' % (time, time + options.aggregation, options.aggregation)
        for edge in edges:
            cost = costFunction(edge, weights.getWeight(edge))
            if cost != None:
                print >> fd, '        <edge id="%s" traveltime="%s"/>' % (edge, cost)
        print >> fd, '    </interval>'
    print >> fd, '</netstats>'
    fd.close()

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-C", "--continue-on-unbuild", action="store_true", dest="continueOnUnbuild",
                     default=False, help="continues on unbuild routes")
optParser.add_option("-w", "--disable-warnings", action="store_true", dest="noWarnings",
                     default=False, help="disables warnings")

optParser.add_option("-n", "--net-file", dest="net",
                     help="SUMO network (mandatory)", metavar="FILE")
optParser.add_option("-t", "--trips", dest="trips",
                     help="trips in step 0 (this or flows is mandatory)", metavar="FILE")
optParser.add_option("-F", "--flows",
                     help="flows in step 0 (this or trips is mandatory)", metavar="FILE")
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

optParser.add_option("-f", "--first-step", dest="firstStep",
                     type="int", default=0, help="First DUA step [default: %default]")
optParser.add_option("-l", "--last-step", dest="lastStep",
                     type="int", default=50, help="Last DUA step [default: %default]")
optParser.add_option("-p", "--path", dest="path",
                     default=os.environ.get("SUMO", ""), help="Path to binaries [default: %default]")

optParser.add_option("-y", "--absrand", dest="absrand", action="store_true",
                     default=False, help="use current time to generate random number")
optParser.add_option("-c", "--cost-function", dest="costfunc",
                     default="identity", help="(python) function to use as cost function")
(options, args) = optParser.parse_args()
if not options.net or not (options.trips or options.flows):
    optParser.error("At least --net-file and --trips or --flows have to be given!")


duaBinary = os.environ.get("DUAROUTER_BINARY", os.path.join(options.path, "duarouter"))
log = open("dua-log.txt", "w+")

parser = make_parser()
reader = NetReader()
parser.setContentHandler(reader)
parser.parse(options.net)
edges = reader.getEdges()

if "." in options.costfunc:
    idx = options.costfunc.rfind(".")
    module = options.costfunc[:idx]
    func = options.costfunc[idx+1:]
    exec("from %s import %s as costFunction" % (module, func))
else:
    exec("costFunction = %s" % options.costfunc)

if options.flows:
    tripFiles = options.flows.split(",")
else:
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
            file = tripFile[:tripFile.find(".")] + "_%s.rou.alt.xml" % (step-1)
        output = tripFile[:tripFile.find(".")] + "_%s.rou.xml" % step
        print ">> Running router with " + file
        btime = datetime.now()
        print ">>> Begin time: %s" % btime
        writeRouteConf(step, options, file, output)
        retCode = call([duaBinary, "-c", "iteration_%s.rou.cfg" % step], log)
        etime = datetime.now()
        print ">>> End time: %s" % etime
        print ">>> Duration: %s" % (etime-btime)
        print "<<"
        files.append(output)
    # generating weights file
    print ">> Generating weights"
    reader = RouteReader()
    parser.setContentHandler(reader)
    for f in files:
        parser.parse(f)
    generateWeights(step, options, edges, reader, costFunction)
    print "<<"
    print "< Step %s ended (duration: %s)" % (step, datetime.now() - btimeA)
    print "------------------\n"
    sys.stdout.flush()
print "dua-iterate ended (duration: %s)" % (datetime.now() - starttime)

log.close()
