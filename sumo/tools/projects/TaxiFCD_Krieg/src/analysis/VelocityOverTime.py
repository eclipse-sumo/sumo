# -*- coding: Latin-1 -*-
"""
@file    VelocityOverTime.py
@author  Sascha.Krieg@dlr.de
@date    2008-05-29

Shows the velocityCurve of the chosen taxi for all available sources, thereby the time duration per Edge is apparent.

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

from pylab import *
import profile
import util.Path as path
import util.Reader as reader
from cPickle import load
from analysis.Taxi import * 

from matplotlib.collections import LineCollection
from matplotlib.colors import colorConverter

#global vars
WEE=True #=withoutEmptyEdges decide which analysis file should be used
edgeDict={}
taxis=[]

def main(): 
    print "start program"
    global taxis, edgeDict 
    
    #decide if you want to save charts for every taxi or show a single one 
    all=False; 
    taxiId="316_3"
    
    #load data
    edgeDict=load(open(path.edgeLengthDict,'r'))
    taxis=reader.readAnalysisInfo(WEE)
    #reader.readEdgesLength()
    
    if all:
        plotAllTaxis()
    else:    
        plotIt(taxiId)
        show()    
    
    print "end"

def plotAllTaxis():
    """plot all taxis to an folder."""
    #kind of progress bar :-)
    allT=len(taxis)
    lastProz=0    
    for i in range(5,105,5): 
        s="%02d" %i
        print s,    
    print "%"   
     
    for i in range(allT):
        actProz=(100*i/allT)                       
        if actProz!=lastProz and actProz%5==0:
            print "**",
            lastProz=actProz
        if plotIt(taxis[i].id)!=-1:
            savefig(path.vOverTimeDir+"taxi_"+str(taxis[i].id)+".png", format="png")
        close() #close the figure
           
def fetchData(taxiId):    
    """fetch the data for the given taxi"""      
    route=[[],[],[],[]] #route of the taxi (edge, length, edgeSimFCD(to find doubles))
    values=[[],[],[],[]] #x,y1,x2,y2 (position, vFCD,vSIMFCD)
    actLen=0
    x=0
    def getTime(s,v): 
        if v==0:
            return 0
        return s/(v/3.6)
    
    for step in taxis[taxis.index(taxiId)].getSteps():        
        if step.source==SOURCE_FCD or step.source==SOURCE_SIMFCD:
            routeLen=edgeDict[step.edge]
            
            #save the simFCD infos in apart Lists
            if step.source==SOURCE_SIMFCD and len(values[2])<=0:
                x=2
                actLen=0   
                
            if len(route[0+x])>0 and step.edge==route[0+x][-1]:
                #print step.edge
                values[1+x][-1]=(values[1+x][-1]+step.speed)/2.0
                values[1+x][-2]=values[1+x][-1]
            else:    
                #start point of route
                values[0+x].append(actLen)
                values[1+x].append(step.speed)
                
                actLen+=getTime(routeLen,step.speed) 
                print "l ",actLen," rL ",routeLen," s ",step.speed                 
                route[0+x].append(step.edge) #label                
                route[1+x].append(actLen) #location
                
                #end point of route            
                values[0+x].append(actLen)
                values[1+x].append(step.speed)

    return route,values
    

def plotIt(taxiId):
    """draws the chart"""
    width=12 #1200px
    height=9 #900px
    
    #fetch data
    route,values=fetchData(taxiId)
      
    #check if a route exists for this vehicle
    if len(route[1])<1 or len(route[3])<1:
        return -1
    #make nice labels 
    maxRoute=max((route[1][-1]),route[3][-1])
    minDist=(maxRoute/(width-4.5))
    
    def makethemNice(source=SOURCE_FCD):
        """create labels of the x-Axis for the FCD and simFCD chart"""
        if source==SOURCE_FCD:
            label=0;  loc=1
        elif source==SOURCE_SIMFCD:
            label=2; loc=3
        else:
            assert False
            
        lastShown=route[loc][0]                  
        for i in range(len(route[label])):
            if i==0 or i==len(route[label])-1:           
                route[label][i]=str(int(round(route[loc][i])))+"\n"+route[label][i]                       
            elif route[loc][i]-lastShown>minDist: #if distance between last Label location and actual location big enough                         
                route[label][i]=str(int(round(route[loc][i])))+"\n"+route[label][i] 
                lastShown=route[loc][i]
            else:
                route[label][i]=""    
        if route[loc][-1]-lastShown<minDist: #check if the last shown element troubles the last    
            route[label][route[loc].index(lastShown)]=""  
    
    makethemNice(SOURCE_FCD)
    makethemNice(SOURCE_SIMFCD)
    
    
    #plot the results
    fig=figure(figsize=(width,height), dpi=96)
        
    subplot(211)
    title(U"Geschwindigkeit  \u00FCber Zeit pro Kante")    
    ylabel("v (km/h)")
    grid(True)     
    #set the x scale
    xticks(route[1],route[0])    
    plot(values[0],values[1], label='FCD')    
    legend() 
    #set that the axis 
    axis([axis()[0],maxRoute,0,max(max(values[1]),max(values[3]))+10])  
    
    subplot(212)    
    xlabel("\n\nt (s)   unterteilt in Routenabschnitte (Kanten)\n\n")
    ylabel("v (km/h)")
    grid(True)
    #set the x scale
    xticks(route[3],route[2])    
    plot(values[2],values[3], label='simulierte FCD', color='g')
    legend() 
    #set that the axis 
    axis([axis()[0],maxRoute,0,max(max(values[1]),max(values[3]))+10])  
    
    return 1
    

    
#start the program
#profile.run('main()')
main()