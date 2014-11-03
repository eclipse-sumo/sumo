# -*- coding: utf-8 -*-
"""
@file    evaluator.py
@author  Lena Kalleske
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2009-06-30
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import xml.dom.minidom as dom
from genRoutes import *
import subprocess, sys
from changeNet import *
from pylab import *
import libxml2

durWE = []
durNS = []

routeLenWE = []
routeLenNS = []

minTravelTime = 69.0

def evalTrips(begin, N):
    global durWE, durNS
    
    doc = libxml2.parseFile('tripinfo.txt')

    lastTWE = max([int(d.content) for d in doc.xpathEval("/tripinfos/tripinfo[@vtype='carRight']/@depart")])
    lastTNS = max([int(d.content) for d in doc.xpathEval("/tripinfos/tripinfo[@vtype='carDown']/@depart")])
    nWE = len(doc.xpathEval("/tripinfos/tripinfo[@vtype='carRight']"))
    nNS = len(doc.xpathEval("/tripinfos/tripinfo[@vtype='carDown']"))
    
    durWE = [int(d.content) for d in doc.xpathEval("/tripinfos/tripinfo[@vtype='carRight'][@depart>%d]/@duration" % begin)]
    durNS = [int(d.content) for d in doc.xpathEval("/tripinfos/tripinfo[@vtype='carDown'][@depart>%d]/@duration" % begin)]
    
    doc.freeDoc()
    
    inputNS = nNS * 3600.0 / N
    inputWE = nWE * 3600.0 / N 
    
    flowNS = nNS * 3600.0 / lastTNS
    flowWE = nWE * 3600.0 / lastTWE
    
    return inputWE, inputNS, flowWE, flowNS

def getAvgDelayWE():
#    avgRouteLen = sum(routeLenWE) / len(routeLenWE)
#    minTravelTime = avgRouteLen / (16.67 - 0.5 * 0.5 * 2.6) # maxspeed - 0.5 * sigma * accel 
    return (1.0 * sum(durWE)/len(durWE) - minTravelTime)

def getAvgDelayNS():
#    avgRouteLen = sum(routeLenNS) / len(routeLenNS)
#    minTravelTime = avgRouteLen / (16.67 - 0.5 * 0.5 * 2.6) # maxspeed - 0.5 * sigma * accel 
    return (1.0 * sum(durNS)/len(durNS) - minTravelTime)

def getAvgDelay():
#    avgRouteLen = (sum(routeLenWE)+sum(routeLenNS)) / (len(routeLenWE)+len(routeLenNS))
#    minTravelTime = avgRouteLen / (16.67 - 0.5 * 0.5 * 2.6) # maxspeed - 0.5 * sigma * accel 
    return (1.0 * (sum(durWE)+sum(durNS))/(len(durWE)+len(durNS)) - minTravelTime)

def getAvgGreenTime(intergreen1, intergreen2):
    doc = libxml2.parseFile('tls.out')

    lNS = doc.xpathEval("count(/tls-states/tlsstate[@phase='0'])")
    lWE = doc.xpathEval("count(/tls-states/tlsstate[@phase='2'])")
    
    lIG1 = doc.xpathEval("count(/tls-states/tlsstate[@phase='1'])")
    lIG2 = doc.xpathEval("count(/tls-states/tlsstate[@phase='3'])")
    
    doc.freeDoc()
    
    greenNS = lNS / ceil((lIG1 / intergreen1))
    greenWE = lWE / ceil((lIG2 / intergreen2))

    return greenWE, greenNS


def getSaturationFlow():
    genRoutes(300, 'u', 3600, 'u', 0, 'u', 0, 'u', 0)
    
    gui = False
    
    setTLType('sumoConfig\cross_sat\cross_sat.net.xml', 'static')
    changePhaseDurations('sumoConfig\cross_sat\cross_sat.net.xml', 1800, 6, 300, 6)
    
    if gui:
        sumoExe = "guisim"
        sumoConfig = "sumoConfig\cross_sat\cross_sat.sumocfg"
        sumoProcess = subprocess.Popen("%s -c %s" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)
    else:
        sumoExe = "sumo"
        sumoConfig = "sumoConfig\cross_sat\cross_sat.sumocfg"
        sumoProcess = subprocess.Popen("%s -c %s --no-duration-log --no-step-log --time-to-teleport 10000000" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)
           
    sumoProcess.wait()
    
    
    doc = libxml2.parseFile('sumoConfig\cross_sat\cross.out')
    veh = [int(d.content) for d in doc.xpathEval("/detector/interval[@id='SAT']/@nVehEntered")]
    doc.freeDoc()
    
    sumVeh = [sum(veh[1:(i+1)]) for i in range(len(veh))]
        
  
    xSat = range(323,538)
    ySat = sumVeh[323:538]
    [aSat,b] = linreg(xSat,ySat)
    print 'Saturation flow: ', aSat * 3600
    ySatN = [aSat*xSat[i]+b for i in range(len(xSat))]

    x = range(700,850)
    y = sumVeh[700:850]
    [a,b] = linreg(x,y)
    print 'Saturation flow 2: ', a * 3600
    ynew = [a*x[i]+b for i in range(len(x))]

#    plot(range(len(veh)), sumVeh, xSat, ySatN, x, ynew)
#    legend(('Anzahl Fahrzeuge', 'saturation flow = %f' % (aSat * 3600), 'flow = %f' % (a * 3600)))
#    xlabel('Time t')
#    ylabel('# vehicles')
#  
#    show()
    
    return (aSat * 3600)


def getMinTravelTime():
    genRoutes(1, 'u', 1, 'u', 0, 'u', 0, 'u', 0)
    
    gui = False
    
    setTLType('sumoConfig\one_intersection\cross.net.xml', 'static')
    changePhaseDurations('sumoConfig\one_intersection\cross.net.xml', 1000, 6, 1, 6)
    
    if gui:
        sumoExe = "guisim"
        sumoConfig = "sumoConfig\one_intersection\cross.sumocfg"
        sumoProcess = subprocess.Popen("%s -c %s" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)
    else:
        sumoExe = "sumo"
        sumoConfig = "sumoConfig\one_intersection\cross.sumocfg"
        sumoProcess = subprocess.Popen("%s -c %s --no-duration-log --no-step-log --time-to-teleport 10000000" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)
           
    sumoProcess.wait()
    
	
    doc = libxml2.parseFile('tripinfo.txt')
    travelDur = int(doc.xpathEval("/tripinfos/tripinfo/@duration")[0].content)
    doc.freeDoc()
	
    return travelDur

# Webster formula
def getOptGreenTimes(satFlowRateWE, satFlowRateNS, intergreenLengthWE, intergreenLengthNS):
    L = intergreenLengthWE + intergreenLengthNS
    Y = satFlowRateWE + satFlowRateNS
    C = (5 + 1.5 * L) / (1 - Y)
    GWE = (C - L) * satFlowRateWE / Y
    GNS = (C - L) * satFlowRateNS / Y
    return GWE, GNS

# linear regression
def linreg(X, Y):
    """
    Summary
        Linear regression of y = ax + b
    Usage
        real, real = linreg(list, list)
    Returns coefficients to the regression line "y=ax+b" from x[] and y[]
    """
    if len(X) != len(Y):  raise ValueError, 'unequal length'
    N = len(X)
    Sx = Sy = Sxx = Syy = Sxy = 0.0
    for x, y in map(None, X, Y):
        Sx = Sx + x
        Sy = Sy + y
        Sxx = Sxx + x*x
        Syy = Syy + y*y
        Sxy = Sxy + x*y
    det = Sxx * N - Sx * Sx
    a, b = (Sxy * N - Sy * Sx)/det, (Sxx * Sy - Sx * Sxy)/det
    return a, b
    