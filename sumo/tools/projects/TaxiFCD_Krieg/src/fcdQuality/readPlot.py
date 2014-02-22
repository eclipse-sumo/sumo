#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    readPlot.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-07-26
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from pylab import * 
from os import walk
from os.path import join
import util.Path as path
#index = int(sys.argv[2])

#0: Frequenz; ->belegt
#1: Ausstattungsgrad; ->belegt
#2: ausgewaehlterFhzg; ->nein
#3: Proben; ->?was ist das
#4: erkannter Kanten; ->abs Wert rel Besser
#5: nichtErkannterKanten; ->abs Wert rel Besser
#6: edgeSmaples(immer gleich);> ->?was ist das 
#7: FhzgSamples; ->nein
#8: GeschwEdge; ->nein
#9: GeschwFhzg; ->nein
#10: absDiffGeschw; ->rel besser
#11: relDiffGeschw; ->ja
#12: %erkannter Kanten; ->darstellung 13 besser doch 12
#13: %nicht erkannterKanten ->ja
XML=True
TXT=False
RELSPEED=11
EDGENO=12
filePath=path.main+"/fcdQualitaet/readPlotData/outDir/"

load=XML

if load==XML: 
    RELSPEED=15; EDGENO=17 
    #filePath=path.main+"/fcdQualitaet/output/plotData/vtypeImmerAlleKanten/"
    #filePath=path.main+"/fcdQualitaet/output/plotData/vtypeNurBefahreneKanten/"
    #filePath=path.main+"/fcdQualitaet/output/plotData/edgeDumpImmerAlleKanten/"
    #filePath=path.main+"/fcdQualitaet/output/plotData/edgeDumpNurBefahreneKanten/"
    #filePath=path.main+"/fcdQualitaet/output/plotData/prozFCD_vtypeImmerAlleKanten/"
    filePath=path.main+"/fcdQualitaet/output/plotData/prozFCD_vtypeNurBefahreneKanten"
showVal=RELSPEED
#showVal=EDGENO

#absSwitch decide if the Value should be transformed into an absolute Value
absSwitch=True
xt = []
yt = []

def main(): 
    print "start program"
    plotData()
    print "end"


def fetchData(): 
    valueDict = {}
    firstFile=True
    period=0.0   
    
    def readTXT(line):    
        line = line.strip()
        words = line.split(";")        
        period = float(words[0])
        quota = float(words[1])
        value = float(words[showVal])        
        if absSwitch:
            value = abs(value)
        valueDict.setdefault(period,{}).setdefault(quota,[]).append(value)
        #get the used periods and quotas
        if firstFile and period not in xt:
            xt.append(period)
        if firstFile and len(xt)==1:            
            yt.append(quota)
    
    def readXML(line):
        global period  
        words = line.split('"')        
        if words[0].find("period=")!=-1:
            period = float(words[1])
            if firstFile:
                xt.append(period)
        elif words[0].find("values")!=-1:            
            quota = float(words[1])            
            value = float(words[showVal])                  
            if absSwitch:
                value = abs(value) 
                if value>=40: 
                    value=-1  
                                     
            valueDict.setdefault(period,{}).setdefault(quota,[]).append(value)
            #get the used quotas
            if firstFile and len(xt)==1:            
                yt.append(quota)
                
    for root, dirs, files in walk(filePath):
        for fileName in files:       
            inputFile=open(join(root,fileName),'r')            
            
            for line in inputFile:
                if load==XML:
                    readXML(line)
                else:
                    readTXT(line)
            inputFile.close()
            firstFile=False
            
    #create the array for the plot    
    marr = []    
    for period in xt:
        marr.append([])
        for quota in yt:
            #calc avg of the Value list            
            valueDict[period][quota]=sum(valueDict[period][quota])/len(valueDict[period][quota])             
            #add avg value
            marr[-1].append(valueDict[period][quota]) 
           
           
    return marr 
            
        
def plotData(): 
    marr=fetchData()       
    textsize=18
    
    yticks([0,1,2,3,4,5,6,7,8,9,10], xt, size=textsize)
    ylabel("Periode [s]", size=textsize)
    xticks([0,1,2,3,4,5,6,7,8,9,10], yt, size=textsize)
    xlabel("Ausstattung [%]", size=textsize)
    title("Abweichung der Geschwindigkeit zwischen FCD und des simulierten Verkehrs", size=textsize)
    #title("Relative Anzahl erfasster Kanten", size=textsize)
    figtext(0.7865,0.92,'[%]', size=textsize)
        
    contourf(marr, 50)#levels=arange(mmin-mmin*.1, mmax+mmax*.1, (mmax-mmin)/10.))     
    #set fontsize and ticks for the colorbar:
    if showVal==EDGENO:
        cb = colorbar(ticks=[0,10,20,30,40,50,60,70,80,90,100]) 
    else:
        #cb = colorbar(ticks=range(17)) 
        cb = colorbar()
    for t in cb.ax.get_yticklabels(): #set colorbar fontsize of each tick    
         t.set_fontsize(textsize)
         
    show()
    
    
#start the program
main()
