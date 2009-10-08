#!/usr/bin/env python
"""
@file    getHourlyMeasuredDaten.py
@author  Yun-Pang.Wang@dlr.de
@date    2009-10-08
@version $Id$

generate detected-flow data and create entity files for retriving detected-flow data for the traffic data in southern Nuremberg

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import glob, os, optparse, sys, datetime
from optparse import OptionParser
from xml.sax import saxutils, make_parser, handler


class DetectedFlow:
    def __init__(self, label, allflows , carflows, truckflows, valueValid='TRUE'):
        self.label = label             # wDay+hour, eg. MON-06
        self.allflows = allflows
        self.carflows = carflows
        self.truckflows = truckflows
        self.valid = valueValid

    def __repr__(self):
        return "%s_<%s|%s|%s>" % (self.label, self.allflows, self.carflows, self.truckflows)

def main():
    gpdetMap= {}
    detEdgeMap= {}
    outputDir = os.path.join(pyPath, "input", "detectordata")
    dataFile = os.path.join(pyPath, "input", "rawData.txt")
    detFile = os.path.join(pyPath, "input", "detEdgeIndex.txt")

    if options.verbose:
        print 'read detector - edge indices'

    for line in open(detFile):
        if "#" not in line:
            line = line.split('\n')[0]
            infos = line.split(' ')
            detGp = infos[1].split('_')[0]
            if '/' in detGp:
                detGp = detGp.split('/')[0] + '-' + detGp.split('/')[1]
            if detGp not in detEdgeMap:
                detEdgeMap[detGp]=[]
                detEdgeMap[detGp].append(infos[2]) # edgeId put at the first place
            detEdgeMap[detGp].append(infos[0])    # detIds
            if options.verbose:
                print detGp, detEdgeMap[detGp]

    if options.verbose:
        print 'read detctor raw data'
    timeIndex = -1
    detFlows = {}
    intervalCounter = 0
    detId = None
    for line in open(dataFile):
        if "#" not in line:
            line = line.split('\n')[0]
            if line.split(',')[0] != detId and detId:
                exId = detId
                newId = True
            else:
                newId = False
                
            detId = line.split(',')[0]
            time = line.split(',')[1]
            time = int(time[:2])
            intervalCounter += 1
            if detId not in detFlows:
                detFlows[detId] = []

            if time != timeIndex:
                if  timeIndex != -1:
                    valueValid = 'TRUE'
                    if intervalCounter != 6:
                        valueValid = 'FALSE'
                    curId = detId
                    if newId:
                        curId = exId
                    newFlow = DetectedFlow(timeIndex, allflows/6., carflows/6., truckflows/6., valueValid)
                    detFlows[curId].append(newFlow)
                    intervalCounter = 0
                timeIndex = time
                allflows = 0
                carflows = 0
                truckflows = 0
                allflows += int(line.split(',')[2])
                carflows += int(line.split(',')[4])
                truckflows += int(line.split(',')[6])
            else:
                allflows += int(line.split(',')[2])
                carflows += int(line.split(',')[4])
                truckflows += int(line.split(',')[6])

    if options.verbose:
        print 'get data for each detector group'
    detGpFlows = {}
    invalidDetGp = []
    for gp in detEdgeMap:
        detGpFlows[gp]={}
        for iter, det in enumerate(detEdgeMap[gp]):
            if iter > 0:
                if det in detFlows:
                    for elem in detFlows[det]:
                        if elem.label not in detGpFlows[gp]:
                            detGpFlows[gp][elem.label] = {}
                            detGpFlows[gp][elem.label]["all"] = 0.
                            detGpFlows[gp][elem.label]["cars"] = 0.
                            detGpFlows[gp][elem.label]["trucks"] = 0.

                        detGpFlows[gp][elem.label]["all"] += elem.allflows   # get detGp hourly flows
                        detGpFlows[gp][elem.label]["cars"] += elem.carflows   # get detGp hourly flows
                        detGpFlows[gp][elem.label]["trucks"] += elem.truckflows   # get detGp hourly flows

                        if elem.valid == 'False' and gp not in invalidDetGp:
                            invalidDetGp.append(gp)
                elif gp not in invalidDetGp:
                    invalidDetGp.append(gp)
    print 'invalid detector groups:', invalidDetGp
    
    # output detectedFlows
    for gp in detGpFlows:
        filename = os.path.join(outputDir, "detectedFlows_%s.xml" %gp)
        foutdata = file(filename, 'w')
        print >> foutdata, """<!-- generated on %s by $Id$ -->
    <detectedFlows>""" % datetime.datetime.now()
        foutdata.write('    <dataset edgeid="%s" groupid="%s" detectors="%s" date="%s">\n' %(detEdgeMap[gp][0], gp, len(detEdgeMap[gp])-1, "2007-07-18"))
        for hour in detGpFlows[gp]:
            pgrValid = 'TRUE'
            truckValid = 'FALSE'
            if gp in invalidDetGp:
                pgrValid = 'FALSE'
            foutdata.write('            <flows weekday-time="%s" allvehicles = "%s" passengercars="%s" truckflows="%s" pgrValid="%s" truckValid="%s"/>\n' \
                        %(('WED'+'-'+str(hour)), detGpFlows[gp][hour]["all"], detGpFlows[gp][hour]["cars"], detGpFlows[gp][hour]["trucks"], pgrValid, pgrValid))
        foutdata.write('    </dataset>\n')
        foutdata.write('\n</detectedFlows>\n')
    foutdata.close()

    # output entityfiles
    dataFiles = ",".join([item for item in glob.glob(os.path.join(outputDir, "detectedFlows_*.xml"))])
    elemSet = []
    fileName = os.path.join(outputDir, "detectedFlowEntityDef_WED.xml")
    g = open(fileName, 'w')
    g.write('<?xml version="1.0"?>\n')
    g.write('<!DOCTYPE detectedFlows [\n')
    for name in dataFiles.split(','):
        basename = os.path.basename(name)
        key = name[name.rfind('s')+2:name.rfind('x')-1]
        keyDate = key[:3]
        checkKey = os.path.join(outputDir, "detectedFlows_%s.xml" %key)
        if os.path.exists(checkKey):
            g.write('  <!ENTITY %s SYSTEM "detectedFlows_%s.xml">\n' %(key, key))
            elemSet.append(key)
    g.write(']>\n')
    g.write('<detectedFlows>\n')
    for elem in elemSet:
        g.write('    &%s;\n' %elem)
    g.write('</detectedFlows>')
    g.close()
    print 'success'

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
(options, args) = optParser.parse_args()
pyPath = os.path.abspath(os.path.dirname(sys.argv[0]))

main()