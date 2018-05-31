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
import os, sys, subprocess, glob
from optparse import OptionParser

SUMO_HOME = os.environ['SUMO_HOME']
ROOT_DIR = os.path.join(os.path.dirname(__file__))
runnerPy = os.path.join(ROOT_DIR,"runner.py")

# parameters
timeUntilMRM =[10]#, 8, 6]
initialAwareness= [0.95]#, 0.75, 0.5]
responseTime= [5] # [[1, 5], [3, 9], [3,15]] ? give a range or a certain value?
recoveryRate= [0.05]#, 0.05, 0.05]
mrmDecel= [1.1]#, 1.3, 1.5]
routeMap= {}
routeMap['UC1_1'] = ['r0', 'start approach1 approach2 safetyzone1_1 safetyzone1_2 workzone safetyzone2_1 safetyzone2_2 leave end']
routeMap['UC5_1'] = ['r0', 'e0']

def checkDir(dir):
    if not os.path.exists(dir):
        print ("%s does not exist." %dir)
    else:
        outputdir = os.path.join(dir, 'OUTPUT')
        if not os.path.exists(outputdir):
            os.mkdir(outputdir)
        return outputdir

def generateRouteFile(routefile,i,j,k,l,rList):
    if len(rList[1].split(' ')) > 1:
        start = rList[1].split(' ')[0]
        end = rList[1].split(' ')[-1]
    else:
        start = rList[1]
        end = rList[1]
    f = open(routefile, 'w')
    f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    f.write('<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/routes_file.xsd">\n')
    f.write('    <!-- vType definitions -->\n')
    f.write('    <!-- vClass custom1 disallows vehicles to enter bus lane at construction site -->\n')
    f.write('    <!-- default vClass is passenger -->\n')
    f.write('    <vType id="automated" sigma="0." speedFactor="1" vClass="custom1"/>\n')
    f.write('    <vType id="manual" sigma="0.5" speedFactor="normc(0.8,0.1,0.5,1.5)" emergencyDecel="9" guiShape="passenger/van" carFollowModel="TCI"/>\n')
    f.write('    <route id="%s" edges="%s"/>\n' %(rList[0], rList[1]))
    f.write('    <!-- one  hour automated vehicle flow -->\n')
    f.write('    <flow id="AVflow" type="automated" route="%s" begin="0" end="3600" probability="0.01" color="red">\n' %(rList[0]))
    f.write('        <param key="has.toc.device" value="true"/>\n')
    f.write('        <param key="device.toc.manualType" value="manual"/>\n')
    f.write('        <param key="device.toc.automatedType" value="automated"/>\n')
    f.write('        <param key="device.toc.initialAwareness" value="%s"/>\n'%(i))
    f.write('        <param key="device.toc.responseTime" value="%s"/>\n'%(j))
    f.write('        <param key="device.toc.recoveryRate" value="%s"/>\n'%(k))
    f.write('        <param key="device.toc.mrmDecel" value="%s"/>\n'%(l))
    f.write('    </flow>\n')
    f.write('    <!-- one  hour manually driven vehicle flow -->\n')
    f.write('    <flow id="LVflow" type="manual" from="%s" to="%s" begin="0" end="3600" probability="0.01"/>\n' %(start, end))
    f.write('</routes>\n')
    f.close()
    
def generateAddFile(addfile, frequency, outputdir):
    freq = frequency
    f = open(addfile, 'w')
    edgeFile = os.path.join(outputdir, "edges_%s.xml" %freq)
    laneFile = os.path.join(outputdir, "lanes_%s.xml" %freq)
    f.write('<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">\n')
    f.write('    <edgeData id="edge_%s" freq="%s" file="%s" excludeEmpty="true"/>\n' %(freq,freq,edgeFile))
    f.write('    <laneData id="lane_%s" freq="%s" file="%s" excludeEmpty="true"/>\n' %(freq,freq,laneFile))
    f.write('</additional>\n')
    f.close()

if __name__ == "__main__":

    optParser = OptionParser()
    optParser.add_option("--codes", help="scenario code", default="UC1_1 UC5_1")
    optParser.add_option("--frequency", help="output frequency", default= 300)
    optParser.add_option("-v", "--verbose", action="store_true", dest="verbose", 
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args()

    options.codes = options.codes.split()
    if options.verbose:
        print ("output frequency: %s" %(options.frequency))
    for code in options.codes:
    # generate input files
        OUT_DIR = os.path.join(ROOT_DIR, code)
        outputdir = checkDir(OUT_DIR)
        # ? do not need an additional file for rerouting?
        addfile = os.path.join(ROOT_DIR, "input_additional.add.xml")
        generateAddFile(addfile, options.frequency, outputdir)
        
        for i in initialAwareness:
            if options.verbose:
                print ("initialAwareness: %s" %(i))
            for j in responseTime:
                if options.verbose:
                    print ("responseTime: %s" %(j))
                for k in recoveryRate:
                    if options.verbose:
                        print ("recoveryRate: %s" %(k))
                    for l in mrmDecel:
                        if options.verbose:
                            print ("mrmDecel: %s" %(l))
                        filename =  'input_routes_' + str(i) + '_' + str(j) + '_' + str(k) + '_' + str(l) + '.rou.xml'
                        routefile = os.path.join(OUT_DIR, filename)
                        if routeMap[code]:
                            generateRouteFile(routefile,i,j,k,l,routeMap[code])
                        else:
                            print 'Error: no route information exists.'
                        
                        for t in timeUntilMRM:
                            if options.verbose:
                                print ("timeUntilMRM: %s" %(t))
                            fcdname = 'fcd_' + str(i) + '_' + str(j) + '_' + str(k) + '_' + str(l) + '_' + str(t) + '.xml'
                            fcdfile = os.path.join(outputdir, fcdname)
                            cmd = [
                                'python',
                                runnerPy,
                                '--net-file', glob.glob(os.path.join(OUT_DIR, '*.net.xml'))[0],
                                '--route-file', routefile,
                                '--add-file', addfile,
                                '--output-file', fcdfile,
                                '--time-MRM', str(t),
                                '--verbose',
                                ]
                            print("calling %s" % ' '.join(cmd))
                            subprocess.call(cmd, stdout=sys.stdout, stderr=sys.stderr)
