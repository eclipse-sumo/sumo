# -*- coding: Latin-1 -*-
"""
@file    GenerateTaxiRoutesMain.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-08-30

Reads taxi routes and create several new routeFile with different depart times for each route.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import util.Path as path

#global vars
taxiDict={}

def main():        
    print "start program"    
    readRouteFile()   
    writeRoutes() 
    print "end"

def readRouteFile():
    inputFile=open(path.taxiRoutesComplete,'r')    
    taxi=''
    for line in inputFile:
        words=line.split('"')
        if words[0].find("<vehicle")!=-1:
            #                    id                  depart          
            taxiDict.setdefault(words[1],[]).append(int(words[5]))  
            taxi=words[1]          
        elif words[0].find("<route>")!=-1:            
            words=line[line.find(">")+1:line.find("</")].split(" ")
            taxiDict[taxi].append(words)          
    inputFile.close()
    
def writeRoutes():
    """Writes the collected values in a Sumo-Routes-File"""
    
    for i in range(-13,15,2):
        print i
        
        outputFile=open(path.taxiRoutesDiffDepart+str(i)+".rou.xml",'w')
        
        outputFile.write("<routes>\n")
        # known for like used in java
        for taxi,elm in taxiDict.iteritems():            
            outputFile.write("\t<vehicle id=\""+taxi+"\" type=\"taxi\" depart=\""+ str(elm[0]+i)+"\" color=\"1,0,0\">\n")
            outputFile.write("\t\t<route>")
            for edge in elm[1]:
                outputFile.write(str(edge)+" ")
            outputFile.seek(-1,1) #delete the space between the last edge and </route>
            outputFile.write("</route>\n")
            outputFile.write("\t</vehicle>\n")    
        
        outputFile.write("</routes>")
        outputFile.close()    
        


#start the program
main()