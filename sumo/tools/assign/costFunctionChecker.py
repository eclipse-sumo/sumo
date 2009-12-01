#!/usr/bin/env python
"""
@file    costFunctionChecker.py
@author  Michael.Behrisch@dlr.de
@date    2009-08-31
@version $Id: dua-iterate.py 7619 2009-08-10 07:50:52Z yunpangwang $

Run duarouter repeatedly and simulate weight changes via a cost function.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
import os, sys, subprocess
from datetime import datetime
from optparse import OptionParser
from xml.sax import make_parser, handler

class TeeFile:
    """A helper class which allows simultaneous writes to several files"""
    def __init__(self, *files):
        self.files = files
    def write(self, txt):
        """Writes the text to all files"""
        for fp in self.files:
            fp.write(txt)
    def flush(self):
        """Flush all files"""
        for fp in self.files:
            fp.flush()

def writeRouteConf(step, options, file, output):
    fd = open("iteration_" + str(step) + ".rou.cfg", "w")
    print >> fd, """<configuration>
    <input
        net-file="%s" """ % options.net
    if(step==0):
        print >> fd, '        trip-defs="%s"' % file
    else:
        print >> fd, '        alternatives="%s"' % file
        print >> fd, '        weights="dump_%s_%s.xml"' % (step-1, options.aggregation)
    print >> fd, """    />
    <output
        output-file="%s"
        exit-times="True"
    />""" % output
    print >> fd, """    <processing
        continue-on-unbuild="%s"
        gBeta="%s"
        gA="%s"
    />""" % (options.continueOnUnbuild, options.gBeta, options.gA)
    print >> fd, '    <random_number abs-rand="%s"/>' % options.absrand
    print >> fd, '    <time begin="%s"' % options.begin,
    if options.end:
        print >> fd, 'end="%s"' % options.end,
    print >> fd, """/>
    <report
        verbose="%s"
        suppress-warnings="%s"
    />
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
            if int(attrs['depart']) > self._maxDepart:
                self._maxDepart = int(attrs['depart'])

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
    for time in range(0, reader.getMaxDepart(), options.aggregation):
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
if not options.net or not options.trips:
    optParser.error("At least --net-file and --trips have to be given!")


if (sys.platform=="win32"):
    duaBinary = os.path.join(options.path, "duarouter.exe")
else:
    duaBinary = os.path.join(options.path, "sumo-duarouter")
log = open("dua-log.txt", "w+")
logQuiet = open("dua-log-quiet.txt", "w")
sys.stdout = TeeFile(sys.stdout, logQuiet)
sys.stderr = TeeFile(sys.stderr, logQuiet)

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
        if options.verbose:
            print "> Call: %s -c iteration_%s.rou.cfg" % (duaBinary, step)
            p = subprocess.Popen("%s -c iteration_%s.rou.cfg" % (duaBinary, step),
                                 shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            for l in p.communicate()[0]:
                log.write(l)
                sys.__stdout__.write(l)
        else:
            subprocess.call("%s -c iteration_%s.rou.cfg" % (duaBinary, step),
                            shell=True, stdout=log, stderr=log)
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
logQuiet.close()
