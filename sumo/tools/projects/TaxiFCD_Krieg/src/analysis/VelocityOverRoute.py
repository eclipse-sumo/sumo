#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    VelocityOverRoute.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-05-26
@version $Id$

Shows the velocity of the chosen taxi over the driven route for all available sources.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


from pylab import *
import profile
import util.Path as path
import util.Reader as reader
from cPickle import load
from analysis.Taxi import * 

#global vars
WEE=True #=withoutEmptyEdges decide which analysis file should be used
taxiId="26_5"
edgeDict={}
taxis=[]

def main(): 
    print "start program"
    global taxis, edgeDict 
    #load data
    edgeDict=load(open(path.edgeLengthDict,'r'))
    taxis=reader.readAnalysisInfo(WEE)
    plotAllTaxis()
    #plotIt(taxiId)
    #reader.readEdgesLength()
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
            savefig(path.vOverRouteDir+"taxi_"+str(taxis[i].id)+".png", format="png")
        close() #close the figure
           
def fetchData(taxiId):    
    """fetch the data for the given taxi"""      
    route=[[],[],[]] #route of the taxi (edge, length, edgeSimFCD(to find doubles))
    values=[[],[],[]] #x,y1,y2 (position, vFCD,vSIMFCD)
    actLen=0
    for step in taxis[taxis.index(taxiId)].getSteps():        
        if step.source==SOURCE_FCD:
            routeLen=edgeDict[step.edge]
            
            if len(route[0])>0 and step.edge==route[0][-1]:
                #print step.edge
                values[1][-1]=(values[1][-1]+step.speed)/2.0
                values[1][-2]=values[1][-1]
            else:    
                #start point of route
                values[0].append(actLen)
                values[1].append(step.speed)
                
                actLen+=routeLen                 
                route[0].append(step.edge) #label                
                route[1].append(actLen) #location
                
                #end point of route            
                values[0].append(actLen)
                values[1].append(step.speed)
        if step.source==SOURCE_SIMFCD:
            if len(values[2])<1:
                values[2]=[None for i in range(len(values[1]))]             
            if step.edge in route[0]: #if edge is used in the original FCD-Route
                #find right value index
                index=(route[0].index(step.edge)*2)
                    
                if len(route[2])>0 and step.edge==route[2][-1]:
                    values[2][index]=(values[2][index]+step.speed)/2.0 #TODO: Mittelwertbildung ist falsch überarbeiten
                    values[2][index+1]=values[2][index]                    
                else:                    
                    #start point of route
                    values[2][index]=step.speed
                    #end point of route    
                    values[2][index+1]=step.speed
                    
                    route[2].append(step.edge)                               
    return route,values
    

def plotIt(taxiId):
    """draws the chart"""
    width=12
    height=9
    route,values=fetchData(taxiId)
    
    #check if a route exists for this vehicle
    if len(route[1])<1 or len(values[1])<1 or len(values[2])<1:        
        return -1
        
    #make nice labels
    lastShown=route[1][0]     
    minDist=(route[1][-1]/(width-4.5))
   
    for i in range(len(route[0])):
        if i==0 or i==len(route[0])-1:           
            route[0][i]=str(route[1][i])+"\n"+route[0][i]                       
        elif route[1][i]-lastShown>minDist: #if distance between last Label location and actual location big enough                         
            route[0][i]=str(route[1][i])+"\n"+route[0][i] 
            lastShown=route[1][i]
        else:
            route[0][i]=""    
    if route[1][-1]-lastShown<minDist: #check if the last shown element troubles the last    
        route[0][route[1].index(lastShown)]=""  
    
    #plot the results
    fig=figure(figsize=(width,height), dpi=96)    
    plot(values[0],values[1],values[0],values[2])      
    legend(("FCD","simulierte FCD"))
    title(U"Geschwindigkeit  \u00FCber Strecke")
    xlabel("\n\ns (m)   unterteilt in Routenabschnitte (Kanten)\n\n")
    ylabel("v (km/h)")
    #set the x scale
    xticks(route[1],route[0])     
    #set that the axis begin at 0 and ends at max V +10km/h
    axis([axis()[0],axis()[1],0,max(max(values[1]),max(values[2]))+10])   
    grid(True)
    
   
    #show()    
    return 1
    
    
#start the program
#profile.run('main()')
main()
