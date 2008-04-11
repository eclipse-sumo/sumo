# -*- coding: Latin-1 -*-
"""
@file    GenerateTaxiRoutes.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-07

Creates an Route-File, which contains for each Taxi the route, from an FCD-File.

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

from time import strptime
from time import mktime

#global vars
fcdPath="D:/Krieg/Projekte/Diplom/Daten/originalFCD/Proz-fcd_nuremberg_2007-07-18.dat"
routesPath="D:/Krieg/Projekte/Diplom/Daten/taxiRouten/taxiRoutes.rou.xml"
netPath="D:/Krieg/Projekte/Diplom/Daten/sumoNetzFilesNurnbergIIProjektion/nuernberg_vls_new.net.xml"

format="%Y-%m-%d %H:%M:%S" 
# used simulation date in seconds
simDate=mktime((2007,7,18,0,0,0,2,199,1))

taxis=[]
routes=[]
vlsEdges=[]
taxiIdDict={} #contains for each Taxi the actual "TaxiId" based on the number of single routes which are created for them

def GenerateTaxiRoutes():
    """Main"""
    global vlsEdges
    
    print "start program"
    vlsEdges=readVLS_Edges()    
    readFCD()
    writeRoutes()
    
    print "end"
    
def readVLS_Edges():
    """Reads the net file and returns a list of all edges"""        
    inputFile=open(netPath,'r')
    for line in inputFile:
        if line.find(" <edges")!=-1:
            #         delete edges tag at start and end    
            words=line[line.find(">")+1:line.find("</")].split(" ")            
            break
    inputFile.close()
    return words

def getTaxiId(taxi):       
    return "%s_%s" %(taxi,taxiIdDict.setdefault(taxi,0))
    
    

def readFCD(): 
    """Reads the FCD and creates a list of Taxis and for each a list of routes"""
       
    inputFile=open(fcdPath,'r')
    for line in inputFile:
        words= line.split("\t")
        #add route
        taxiId=getTaxiId(words[4])              
        if taxiId in taxis:           
            if words[1] in vlsEdges:
                routes[taxis.index(taxiId)].append(words[1])
            else:
                taxiIdDict[words[4]]+=1                
        elif words[1] in vlsEdges: #if the edge is in the VLS-Area a new route is created 
            taxis.append(taxiId)
            #                 departTime               
            routes.append([(int)(mktime(strptime(words[0],format))-simDate),words[1]])
           
    inputFile.close() 
    print len(taxis)
        
def writeRoutes():
    """Writes the collected values in a Sumo-Routes-File"""
    outputFile=open(routesPath,'w')
    
    outputFile.write("<routes>\n")
    # known for like used in java
    for i in xrange(len(taxis)):              
        if len(routes[i])>3: 
            outputFile.write("\t<vehicle id=\""+taxis[i]+"\" type=\"taxi\" depart=\""+ str(routes[i].pop(0))+"\" color=\"1,0,0\">\n")
            outputFile.write("\t\t<route>")
            for edge in routes[i]:
                  outputFile.write(edge+" ")
            outputFile.seek(-1,1) #delete the space between the last edge and </route>
            outputFile.write("</route>\n")
            outputFile.write("\t</vehicle>\n")
       
    outputFile.write("</routes>")
    outputFile.close()
    
    
#start the program
GenerateTaxiRoutes()