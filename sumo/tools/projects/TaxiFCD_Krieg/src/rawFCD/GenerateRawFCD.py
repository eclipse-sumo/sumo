# -*- coding: Latin-1 -*-
"""
@file    GenerateRawFCD.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-01

Converts the vtypeProbe XML-File to the raw FCD-Format which can afterwards used to process FCD. 

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import util.Path as path
from util.CalcTime import getDateFromDepart
from cPickle import dump

#global vars
vehIdDict={}
vehId=0


def main():
    print "starting"
    #change path for multi
    path.vtypeprobe=path.newPath(path.main,"simRawFCD/t2MitVerschiedenenLosfahrzeiten/vtypeprobe_-4.out.xml")
    path.simulatedRawFCD=path.newPath(path.main,"simRawFCD/t2MitVerschiedenenLosfahrzeiten/simulatedRawFCD_-4.out.dat") 
    inputFile=open(path.vtypeprobe,'r')
    outputFile=open(path.simulatedRawFCD,'w')
    for line in inputFile:
        words=line.split('"')
        
        #get time
        if line.find("<timestep")!=-1:
            time=words[1]
        #if line starts with "<vehicle"
        if line.find("<vehicle id")!=-1:        
            #                veh_id                       date (time to simDate+time)                            x (remove and set comma new)             
            outputFile.write(str(getVehId(words[1]))+'\t'+getDateFromDepart(time)+'\t'+words[13][0:2]+'.'+words[13][2:7]+words[13][8:]+
                             #     y (remove and set comma new)                       status      speed form m/s in km/h
                             '\t'+words[11][0:2]+'.'+words[11][2:7]+words[11][8:]+'\t'+"90"+'\t'+str(int(round(float(words[15])*3.6)))+'\n')    
    outputFile.close()    
    inputFile.close()
    for key in vehIdDict: 
        print key, vehIdDict[key],"\n"
    #pickles the vehIdList 
    dump(vehIdDict, open(path.rawFcdVehIdList,'w'))    
    print "end"
    


def getVehId(orgId):
    """creates new vehicle id's which consists only numerics"""
    global vehId    
    value=vehIdDict.get(orgId,vehId)
    if value is vehId:
        vehIdDict[orgId]=vehId
        vehId+=1        
    return value


#start the program
main()
    
    
    
    
    
    