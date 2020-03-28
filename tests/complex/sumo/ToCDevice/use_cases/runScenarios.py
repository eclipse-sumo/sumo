#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runScenarios.py
# @author  Yun-Pang Floetteroed
# @date    2018-05-30

from __future__ import print_function
import os
import sys
import subprocess
import glob
import random
from optparse import OptionParser

SUMO_HOME = os.environ['SUMO_HOME']
ROOT_DIR = os.path.join(os.path.dirname(__file__))
runnerPy = os.path.join(ROOT_DIR, "runner.py")

# parameters
timeUntilMRM = [10]  # , 8, 6]
# ~ initialAwareness= [0.95, 0.75, 0.5]
initialAwareness = [0.5]
responseTime = [2]  # , 5, 9, 15] # [[1, 5], [3, 9], [3,15]] ? give a range or a certain value?
recoveryRate = [0.05]  # , 0.05, 0.05]
mrmDecel = [1.1]  # , 1.3, 1.5]
routeMap = {}
routeMap['UC1_1'] = ['r0', 'start approach1 approach2 safetyzone1_1 safetyzone1_2 workzone safetyzone2_1 ' +
                     'safetyzone2_2 leave end']
routeMap['UC5_1'] = ['r0', 'e0']
outputDirName = "OUTPUT"
random.seed(606)

# TODO: set up the planed 45 scenarios in regard to the demands (3 levels),
# vehicular compositions (2 vehicle types) and 5 parameter sets
# demandMap = {"UC1_1":[525, 825, 1155], "UC5_1":[665, 1015, 1463]}
# compositionMap = {}
# componentMap = {"UC1_1":[[0.7, 0.3]], "UC5_1":[665, 1015, 1463]}


def checkDir(dir, outputDirName):
    if not os.path.exists(dir):
        print("%s does not exist." % dir)
    else:
        outputdir = os.path.join(dir, outputDirName)
        if not os.path.exists(outputdir):
            os.mkdir(outputdir)
        return outputdir


def getInputFiles(DATA_DIR):
    netfile = glob.glob(os.path.join(DATA_DIR, '*.net.xml'))[0]
    closedLaneFile = glob.glob(os.path.join(DATA_DIR, 'close*.add.xml'))
    shapefile = glob.glob(os.path.join(DATA_DIR, 'shape*.add.xml'))
    viewfile = glob.glob(os.path.join(DATA_DIR, 'view*.add.xml'))[0]

    return netfile, closedLaneFile, shapefile, viewfile


def generateRouteFile(DATA_DIR, i, j, k, l, rList, code):
    routeID = rList[0]
    edges = rList[1]
    if len(edges.split(' ')) > 1:
        start = edges.split(' ')[0]
        end = edges.split(' ')[-1]
    else:
        start = edges
        end = edges
    # define the route file
    fname = 'input_routes_' + str(i) + '_' + str(j) + '_' + str(k) + '_' + str(l) + '.rou.xml'
    routefile = os.path.join(DATA_DIR, fname)

    # define the ssm output file
    outputdir = checkDir(DATA_DIR, outputDirName)
    fname = 'ssm_%s_%s_%s_%s.xml' % (i, j, k, l)
    ssmfile = os.path.join(outputdir, fname)

    fd = open(routefile, 'w')
    print("""<?xml version="1.0" encoding="UTF-8"?>
    <routes>
        <!-- vType definitions -->
        <!-- vClass custom1 disallows vehicles to enter the bus lane at construction site -->
        <!-- default vClass is passenger -->
        <vType id="toc" sigma="0." speedFactor="1" vClass="custom1" emergencyDecel="9" />
        <vType id="automated" sigma="0." speedFactor="1" vClass="custom1" emergencyDecel="9" />
        <vType id="manual" sigma="0.5" speedFactor="normc(0.8,0.1,0.5,1.5)" emergencyDecel="9" \
guiShape="passenger/van" driverState="true"/>
        <route id="%s" edges="%s"/>
        <!-- one  hour automated vehicle flow -->
        <flow id="AVflowToC" type="toc" route="%s" begin="0" end="3600" number="100" color="red">
            <param key="has.toc.device" value="true"/>
            <param key="device.toc.manualType" value="manual"/>
            <param key="device.toc.automatedType" value="toc"/>
            <param key="device.toc.initialAwareness" value="%s"/>
            <param key="device.toc.responseTime" value="%s"/>
            <param key="device.toc.recoveryRate" value="%s"/>
            <param key="device.toc.mrmDecel" value="%s"/>
            <param key="device.toc.useColorScheme" value="true"/>
            <param key="has.ssm.device" value="true"/>
            <param key="device.ssm.measures" value="TTC DRAC PET"/>
            <param key="device.ssm.file" value="%s"/>
        </flow>""" % (routeID, edges, routeID, i, j, k, l, ssmfile), file=fd)
    if code == "UC1_1":
        print("""    <flow id="AVflow" type="automated" route="%s" begin="0" end="3600" number="100" color="white">
                <param key="has.toc.device" value="false"/>
                <param key="device.toc.manualType" value="manual"/>
                <param key="device.toc.automatedType" value="automated"/>
                <param key="device.toc.useColorScheme" value="true"/>
            </flow>""" % (routeID), file=fd)
    print("""    <!-- one  hour manually driven vehicle flow -->
        <flow id="LVflow" type="manual" from="%s" to="%s" begin="0" end="3600" number="300"/>
    </routes>""" % (start, end), file=fd)
    fd.close()

    return routefile


def setAddOutputFiles(DATA_DIR, outputDirName, freq, i, j, k, l, t, closedLaneFile, shapefile):
    outputdir = checkDir(DATA_DIR, outputDirName)
    # set set additional file
    addfile = os.path.join(DATA_DIR, "input_additional_%s_%s_%s_%s_%s.add.xml" % (i, j, k, l, t))
    edgeFile = os.path.join(outputDirName, "edges_%s_%s_%s_%s_%s_%s.xml" % (freq, i, j, k, l, t))
    emissionFile = os.path.join(outputDirName, "emissions_%s_%s_%s_%s_%s_%s.xml" % (freq, i, j, k, l, t))
    laneFile = os.path.join(outputDirName, "lanes_%s_%s_%s_%s_%s_%s.xml" % (freq, i, j, k, l, t))
    fd = open(addfile, 'w')
    print("""
    <additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" \
xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">
        <edgeData id="edge_%s" freq="%s" file="%s" excludeEmpty="true"/>
        <edgeData id="edge_%s" type= "emissions" freq="%s" file="%s" excludeEmpty="true"/>
        <laneData id="lane_%s" freq="%s" file="%s" excludeEmpty="true"/>
    </additional>""" % (freq, freq, edgeFile, freq, freq, emissionFile, freq, freq, laneFile), file=fd)
    fd.close()

    addfiles = addfile
    if closedLaneFile:
        addfiles = addfiles + ',' + closedLaneFile[0]
    if shapefile:
        addfiles = addfiles + ',' + shapefile[0]

    # set the output files
    for prefix in ['fcd', 'summary', 'lanechange', 'log']:
        fname = prefix + '_' + str(i) + '_' + str(j) + '_' + str(k) + '_' + str(l) + '_' + str(t) + '.xml'
        if prefix == 'fcd':
            fcdfile = os.path.join(outputdir, fname)
        elif prefix == 'summary':
            summaryfile = os.path.join(outputdir, fname)
        elif prefix == 'lanechange':
            lcfile = os.path.join(outputdir, fname)
        else:
            logfile = os.path.join(outputdir, fname)

    return addfiles, fcdfile, summaryfile, lcfile, logfile


def printParameterSet(i, j, k, l, t):
    print("initialAwareness: %s" % (i))
    print("responseTime: %s" % (j))
    print("recoveryRate: %s" % (k))
    print("mrmDecel: %s" % (l))
    print("timeUntilMRM: %s" % (t))


if __name__ == "__main__":

    optParser = OptionParser()
    optParser.add_option("--codes", help="scenario code", default="UC1_1 UC5_1")
    optParser.add_option("--frequency", help="output frequency", default=300)
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args()

    options.codes = options.codes.split()
    if options.verbose:
        print("output frequency: %s" % (options.frequency))
    # two use cases
    for code in options.codes:
        DATA_DIR = os.path.join(ROOT_DIR, code)
        # get input files
        netfile, closedLaneFile, shapefile, viewfile = getInputFiles(DATA_DIR)

        # generate the route file and the additional files for each scenario and run the simulation
        for i in initialAwareness:
            for j in responseTime:
                for k in recoveryRate:
                    for l in mrmDecel:
                        if routeMap[code]:
                            routefile = generateRouteFile(DATA_DIR, i, j, k, l, routeMap[code], code)
                        else:
                            print("Error: no route information exists.")

                        for t in timeUntilMRM:
                            addfiles, fcdfile, summaryfile, lcfile, logfile = setAddOutputFiles(
                                DATA_DIR, outputDirName, options.frequency, i, j, k, l, t, closedLaneFile, shapefile)
                            if options.verbose:
                                printParameterSet(i, j, k, l, t)

                            cmd = [
                                'python',
                                runnerPy,
                                '--net-file', netfile,
                                '--route-file', routefile,
                                '--add-file', addfiles,
                                '--fcd-file', fcdfile,
                                '--view-file', viewfile,
                                '--time-MRM', str(t),
                                '--code', code,
                                '--summary', summaryfile,
                                '--lanechange-file', lcfile,
                                '--error-log', logfile,
                                '--nogui', "false",
                                # '--verbose',
                            ]
                            print("calling %s" % ' '.join(cmd))
                            subprocess.call(cmd, stdout=sys.stdout, stderr=sys.stderr)
