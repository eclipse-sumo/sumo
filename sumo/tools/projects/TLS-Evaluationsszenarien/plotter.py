# -*- coding: utf-8 -*-
"""
@file    plotter.py
@author  Lena Kalleske
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2009-06-30
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from pylab import *
from ConfigParser import *

def compareData(name1, name2, str):
    filename = name1 + '.ini'
    ini = ConfigParser()
    ini.read(filename)
    
    distrWE = ini.get("demand", "distrWE")
    distr = ("poisson" if (distrWE=='p') else "uniform")
    
    vehphWEA = eval(ini.get("demand", "vehphWEA"))
    vehphNSA = eval(ini.get("demand", "vehphNSA"))
	
    filename1 = name1 + ".dat"
    
    data1 = load(filename1)
    
    flowWEA1 = data1[:,0]
    flowNSA1 = data1[:,1]
    qWEA1 = data1[:,2]
    qNSA1 = data1[:,3]
    avgDelayWEA1 = data1[:,4]
    avgDelayNSA1 = data1[:,5]
    avgDelayA1 = data1[:,6]
    greenWEA1 = data1[:,7]
    greenNSA1 = data1[:,8]
    
    filename2 = name2 + ".dat"
    
    data2 = load(filename2)
    
    flowWEA2 = data2[:,0]
    flowNSA2 = data2[:,1]
    qWEA2 = data2[:,2]
    qNSA2 = data2[:,3]
    avgDelayWEA2 = data2[:,4]
    avgDelayNSA2 = data2[:,5]
    avgDelayA2 = data2[:,6]
    greenWEA2 = data2[:,7]
    greenNSA2 = data2[:,8]    
    
    [X,Y] = meshgrid(vehphWEA, vehphNSA)
    
    figure(figsize=(6,6))

    Z1 = array(griddata(flowWEA1, flowNSA1, avgDelayA1, X, Y))
    Z2 = array(griddata(flowWEA2, flowNSA2, avgDelayA2, X, Y))
    
    Z = Z1 / Z2
    
    subplot(1,1,1)
    contourf(X,Y,Z,concatenate((frange(0,2,0.1),frange(2,3,1))))
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    colorbar()

    title('Average Delay VA vs. FC (ratio), ' + distr)
    savefig(str + ".png")
    
        
    Z1 = array(griddata(flowWEA1, flowNSA1, avgDelayWEA1, X, Y))
    Z2 = array(griddata(flowWEA2, flowNSA2, avgDelayWEA2, X, Y))
    
   
    
    Z = Z1 / Z2

    figure(figsize=(12,6))
    subplot(1,2,1)
    contourf(X,Y,Z,concatenate((frange(0,2,0.1),frange(2,3,1))))
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    title('Average Delay WE')
    colorbar()
 
 
    Z1 = array(griddata(flowWEA1, flowNSA1, avgDelayNSA1, X, Y))
    Z2 = array(griddata(flowWEA2, flowNSA2, avgDelayNSA2, X, Y))
    
    
    Z = Z1 / Z2
 
    subplot(1,2,2)
    contourf(X,Y,Z,concatenate((frange(0,2,0.1),frange(2,3,1))))
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    title('Average Delay NS')
    colorbar()   

    suptitle('Average Delay VA vs. FC (ratio), ' + distr)
    savefig(str + "WENS.png")

def plotDiagram(name):
    filename = name + ".dat"
    
    X = load(filename)

    flowWEA = X[:,0]
    flowNSA = X[:,1]
    qWEA = X[:,2]
    qNSA = X[:,3]
    avgDelayWEA = X[:,4]
    avgDelayNSA = X[:,5]
    avgDelayA = X[:,6]
    greenWEA = X[:,7]
    greenNSA = X[:,8]



    filename = name + '.ini'
    ini = ConfigParser()
    ini.read(filename)
    
    N = ini.getint("general", "N")
    cut = ini.getboolean("general", "cut")
    
    distrWE = ini.get("demand", "distrWE")
    distrNS = ini.get("demand", "distrNS")

    tlType = ini.get("TL", "tlType")

    vehphWEA = eval(ini.get("demand", "vehphWEA"))
    vehphNSA = eval(ini.get("demand", "vehphNSA"))
    

    maxDelay = max(max(avgDelayWEA), max(avgDelayNSA))
 
    maxD = max(avgDelayA)
    c = maxD / (exp(1)-1)
    V = [c*(exp(i)-1) for i in frange(0,1,0.01)]
 
    [X,Y] = meshgrid(vehphWEA, vehphNSA)
    

    Z = griddata(flowWEA, flowNSA, avgDelayWEA, X, Y)
    
   
    figure(figsize=(12,12))
    subplot(2,2,1)
    #contour(X, Y, Z, 100)
    contourf(X,Y,Z,range(0,180,5))
    
    colorbar()
    title('Average Delay WE')
    xlabel('Input flow WE')
    ylabel('Input flow NS')

    Z = griddata(flowWEA, flowNSA, avgDelayNSA, X, Y)

    subplot(2,2,2)
    #contour(X, Y, Z, 100)
    contourf(X,Y,Z,range(0,180,5))
    colorbar()
    title('Average Delay NS')
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    

    Z = griddata(qWEA, qNSA, avgDelayWEA, X, Y)
    
    avgDelayWECut = [Z[i][i] for i in range(len(Z))]

    subplot(2,2,3)
    #contour(X, Y, Z, 100)
    contourf(X,Y,Z,range(0,180,5))
    colorbar()
    title('Average Delay WE')
    xlabel('Flow q WE')
    ylabel('Flow q NS')

    Z = griddata(qWEA, qNSA, avgDelayNSA, X, Y)
    
    avgDelayNSCut = [Z[i][i] for i in range(len(Z))]

    subplot(2,2,4)
    #contour(X, Y, Z, 100)
    contourf(X,Y,Z,range(0,180,5))
    colorbar()
    title('Average Delay NS')
    xlabel('Flow q WE')
    ylabel('Flow q NS')
    
    suptitle("Average Delay (WE " + ("poisson" if distrWE == 'p' else "uniform") + ", NS " + ("poisson" if distrNS == 'p' else "uniform") + ", " + tlType + ")")
    
    savefig(name + "AvgDelayNSWE.png")
    close()    
    
    
    
    Z = griddata(flowWEA, flowNSA, avgDelayA, X, Y)
    
    figure(figsize=(12,6))
    subplot(1,2,1)
    #contour(X, Y, Z, 100)
    contourf(X,Y,Z,range(0,180,5))
    colorbar()
    title('Average Delay')
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    
    Z = griddata(qWEA, qNSA, avgDelayA, X, Y)
    
    avgDelayCut = [Z[i][i] for i in range(len(Z))]
    
    subplot(1,2,2)
    #contour(X, Y, Z, 100)
    contourf(X,Y,Z,range(0,180,5))
#    clim(0, maxDelay)
    colorbar()
    title('Average Delay')
    xlabel('Flow q WE')
    ylabel('Flow q NS')
    
    suptitle("Average Delay (WE " + ("poisson" if distrWE == 'p' else "uniform") + ", NS " + ("poisson" if distrNS == 'p' else "uniform") + ", " + tlType + ")")
    
    savefig(name + "AvgDelay.png")
    close()
    
    
    
    Z = griddata(flowWEA, flowNSA, qWEA, X, Y)
    
    figure(figsize=(12,6))
    subplot(1,2,1)
    contourf(X, Y, Z, range(0,1250,50))
    colorbar()
    title('Flow q WE')
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    
    Z = griddata(flowWEA, flowNSA, qNSA, X, Y)
    
    subplot(1,2,2)
    contourf(X, Y, Z, range(0,1250,50))
    colorbar()
    title('Flow q NS')
    xlabel('Input flow WE')
    ylabel('Input flow NS')

    suptitle("Input flow vs. flow q (WE " + ("poisson" if distrWE == 'p' else "uniform") + ", NS " + ("poisson" if distrNS == 'p' else "uniform") + ", " + tlType + ")")    
    
    savefig(name + "flowvsq.png")
   
    close()
    
    
    maxGreen = max(max(greenWEA), max(greenNSA))
    
    Z = griddata(flowWEA, flowNSA, greenWEA, X, Y)
    
    figure(figsize=(12,12))
    subplot(2,2,1)
    contourf(X, Y, Z, range(0,100,5))
 #   clim(0, maxGreen)
    colorbar()
    title('Green-time WE')
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    
    Z = griddata(flowWEA, flowNSA, greenNSA, X, Y)
    
    subplot(2,2,2)
    contourf(X, Y, Z, range(0,100,5))
#    clim(0, maxGreen)
    colorbar()
    title('Green-time NS')
    xlabel('Input flow WE')
    ylabel('Input flow NS')
    
    Z = griddata(qWEA, qNSA, greenWEA, X, Y)
    
    greenWECut = [Z[i][i] for i in range(len(Z))]
    
    subplot(2,2,3)
    contourf(X, Y, Z, range(0,100,5))
 #   clim(0, maxGreen)
    colorbar()
    title('Green-time WE')
    xlabel('Flow q WE')
    ylabel('Flow q NS')
    
    Z = griddata(qWEA, qNSA, greenNSA, X, Y)
    
    greenNSCut = [Z[i][i] for i in range(len(Z))]
    
    subplot(2,2,4)
    contourf(X, Y, Z, range(0,100,5))
    colorbar()
    title('Green-time NS')
    xlabel('Flow q WE')
    ylabel('Flow q NS')

    suptitle("Green-time (WE " + ("poisson" if distrWE == 'p' else "uniform") + ", NS " + ("poisson" if distrNS == 'p' else "uniform") + ", " + tlType + ")")
    
    savefig(name + "GreenTime.png")
    close()    
    
    
    q = X[0]
    
    figure()
    title("CUT  (WE " + ("poisson" if distrWE == 'p' else "uniform") + ", NS " + ("poisson" if distrNS == 'p' else "uniform") + ", " + tlType + ")")
    xlabel('Flow q')
    plot(q[0:15], avgDelayWECut[0:15],q[0:15], avgDelayNSCut[0:15], q[0:15], avgDelayCut[0:15],q[0:15], greenWECut[0:15], q[0:15], greenNSCut[0:15])
    legend(('avgDelayWE', 'avgDelayNS', 'avgDelay','greenWE', 'greenNS'),loc='upper left')
    savefig(name + "CUT.png")
    close()  