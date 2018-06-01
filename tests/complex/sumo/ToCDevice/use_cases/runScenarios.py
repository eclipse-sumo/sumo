#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runScenarios.py
@author  Yun-Pang Floetteroed
@date    2018-05-30
@version $Id: runScenarios.py

- generate *.rou.xml files according to the parameters of the pre-defined scenarios
- Run runner.py for each scenario for UC1_1 and UC5

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2018 DLR (http://www.dlr.de/) and contributors

"""
from __future__ import print_function
import os, sys, subprocess, glob
from optparse import OptionParser

SUMO_HOME = os.environ['SUMO_HOME']
ROOT_DIR = os.path.join(os.path.dirname(__file__))
runnerPy = os.path.join(ROOT_DIR,"runner.py")

# parameters
timeUntilMRM =[10]#, 8, 6]
#~ initialAwareness= [0.95, 0.75, 0.5]
initialAwareness= [0.5]
responseTime= [2]#, 5, 9, 15] # [[1, 5], [3, 9], [3,15]] ? give a range or a certain value?
recoveryRate= [0.05]#, 0.05, 0.05]
mrmDecel= [1.1]#, 1.3, 1.5]
routeMap= {}
routeMap['UC1_1'] = ['r0', 'start approach1 approach2 safetyzone1_1 safetyzone1_2 workzone safetyzone2_1 safetyzone2_2 leave end']
routeMap['UC5_1'] = ['r0', 'e0']
# TODO: set up the planed 45 scenarios in regard to the demands, vehicular compositions and 5 parameter sets
#demandMap = {"UC1_1":[525, 825, 1155], "UC5_1":[665, 1015, 1463]}
#compositionMap = {}
#componentMap = {"UC1_1":[[0.7, 0.3]], "UC5_1":[665, 1015, 1463]}

def checkDir(dir):
    if not os.path.exists(dir):
        print ("%s does not exist." %dir)
    else:
        outputdir = os.path.join(dir, 'OUTPUT')
        if not os.path.exists(outputdir):
            os.mkdir(outputdir)
        return outputdir

def getInputFiles(OUT_DIR):
    netfile = glob.glob(os.path.join(OUT_DIR, '*.net.xml'))[0]
    closedLaneFile = glob.glob(os.path.join(OUT_DIR, 'close*.add.xml'))
    shapefile = glob.glob(os.path.join(OUT_DIR, 'shape*.add.xml'))
    viewfile = glob.glob(os.path.join(OUT_DIR, 'view*.add.xml'))[0]

    return netfile, closedLaneFile, shapefile, viewfile

def generateRouteFile(routefile,i,j,k,l,rList,code):
    routeID = rList[0]
    edges = rList[1]
    if len(edges.split(' ')) > 1:
        start = edges.split(' ')[0]
        end = edges.split(' ')[-1]
    else:
        start = edges
        end = edges
    fd = open(routefile, 'w')
    print("""<?xml version="1.0" encoding="UTF-8"?>
    <routes>
        <!-- vType definitions -->
        <!-- vClass custom1 disallows vehicles to enter bus lane at construction site -->
        <!-- default vClass is passenger -->
        <vType id="toc" sigma="0." speedFactor="1" vClass="custom1" emergencyDecel="9" />
        <vType id="automated" sigma="0." speedFactor="1" vClass="custom1" emergencyDecel="9" />
        <vType id="manual" sigma="0.5" speedFactor="normc(0.8,0.1,0.5,1.5)" emergencyDecel="9" guiShape="passenger/van" driverState="true"/>
        <route id="%s" edges="%s"/>
        <!-- one  hour automated vehicle flow -->
        <flow id="AVflowToC" type="toc" route="%s" begin="0" end="3600" number="150" color="red">
            <param key="has.toc.device" value="true"/>
            <param key="device.toc.manualType" value="manual"/>
            <param key="device.toc.automatedType" value="toc"/>
            <param key="device.toc.initialAwareness" value="%s"/>
            <param key="device.toc.responseTime" value="%s"/>
            <param key="device.toc.recoveryRate" value="%s"/>
            <param key="device.toc.mrmDecel" value="%s"/>
            <param key="device.toc.useColorScheme" value="true"/>
        </flow>""" %(routeID, edges, routeID, i, j, k, l), file=fd)
    if code == "UC1_1":
        print("""    <flow id="AVflow" type="automated" route="%s" begin="0" end="3600" number="150" color="white">
                <param key="has.toc.device" value="false"/>
                <param key="device.toc.manualType" value="manual"/>
                <param key="device.toc.automatedType" value="automated"/>
                <param key="device.toc.useColorScheme" value="true"/>
            </flow>""" %(routeID), file=fd)
    print("""    <!-- one  hour manually driven vehicle flow -->
        <flow id="LVflow" type="manual" from="%s" to="%s" begin="0" end="3600" number="200"/>
    </routes>""" %(start, end), file=fd)
    fd.close()
    
def generateAddFile(addfile, frequency, outputdir, i, j, k, l, t):
    freq = frequency
    fd = open(addfile, 'w')
    edgeFile = os.path.join('OUTPUT', "edges_%s_%s_%s_%s_%s_%s.xml" %(freq, i, j, k, l, t))
    laneFile = os.path.join('OUTPUT', "lanes_%s_%s_%s_%s_%s_%s.xml" %(freq, i, j, k, l, t))
    print("""
    <additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">
        <edgeData id="edge_%s" freq="%s" file="%s" excludeEmpty="true"/>
        <laneData id="lane_%s" freq="%s" file="%s" excludeEmpty="true"/>
    </additional>""" %(freq,freq,edgeFile,freq,freq,laneFile), file=fd)
    fd.close()

def printParameterSet(i,j,k,l,t):
    print ("initialAwareness: %s" %(i))
    print ("responseTime: %s" %(j))
    print ("recoveryRate: %s" %(k))
    print ("mrmDecel: %s" %(l))
    print ("timeUntilMRM: %s" %(t))

if __name__ == "__main__":

    optParser = OptionParser()
    optParser.add_option("--codes", help="scenario code", default="UC1_1")# UC5_1")
    optParser.add_option("--frequency", help="output frequency", default= 300)
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose", 
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args()

    options.codes = options.codes.split()
    if options.verbose:
        print ("output frequency: %s" %(options.frequency))
    # two use cases
    for code in options.codes:
        OUT_DIR = os.path.join(ROOT_DIR, code)
        outputdir = checkDir(OUT_DIR)
        # get input files
        netfile, closedLaneFile, shapefile, viewfile = getInputFiles(OUT_DIR)
        
        # generate the route file and the additional files for each scenario and run the simulation
        for i in initialAwareness:
            for j in responseTime:
                for k in recoveryRate:
                    for l in mrmDecel:
                        filename =  'input_routes_' + str(i) + '_' + str(j) + '_' + str(k) + '_' + str(l) + '.rou.xml'
                        routefile = os.path.join(OUT_DIR, filename)

                        if routeMap[code]:
                            generateRouteFile(routefile,i,j,k,l,routeMap[code],code)
                        else:
                            print("Error: no route information exists.")
                        
                        for t in timeUntilMRM:
                            addfile = os.path.join(OUT_DIR, "input_additional_%s_%s_%s_%s_%s.add.xml" %(i,j,k,l,t))
                            generateAddFile(addfile, options.frequency, outputdir, i, j, k, l, t)
                            if options.verbose:
                                printParameterSet(i,j,k,l,t)
                            fcdname = 'fcd_' + str(i) + '_' + str(j) + '_' + str(k) + '_' + str(l) + '_' + str(t) + '.xml'
                            fcdfile = os.path.join(outputdir, fcdname)
                            addfiles = addfile
                            if closedLaneFile:
                                addfiles = addfiles + ',' + closedLaneFile[0]
                            if shapefile:
                                addfiles = addfiles + ',' + shapefile[0]
                            
                            cmd = [
                                'python',
                                runnerPy,
                                '--net-file', netfile,
                                '--route-file', routefile,
                                '--add-file', addfiles,
                                '--output-file', fcdfile,
                                '--view-file', viewfile,
                                '--time-MRM', str(t),
                                '--code', code,
                                '--nogui', "false",
                                '--verbose',
                                ]
                            print("calling %s" % ' '.join(cmd))
                            subprocess.call(cmd, stdout=sys.stdout, stderr=sys.stderr)
