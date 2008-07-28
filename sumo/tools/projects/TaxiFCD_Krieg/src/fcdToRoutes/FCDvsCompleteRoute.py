# -*- coding: Latin-1 -*-
"""
@file    FCDvsCompleteRoute.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-08

Compares the FCD-route of an taxi with the generated route which is used in the simulation.
The output data can be visualized with the script My_mpl_dump_onNet based on mpl_dump_onNet from Daniel.
(avg=False)

Secondly the average of added Edges per route will be calculate (avg=True). 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

import util.Path as path

#global vars

edgeList=[]
routeDict= {}
taxi="70_1" #the Taxi for which the output should be generated
avg=True

def main():
    print "start program"    
    readRoutes()
    if avg: 
        clacAvg()
    else:   
        writeOutput()
    print "end"
    

def readRoutes():
    """reads the t1CompletePath and the taxiRoutesPath. 
        and sets for each edge in t1CompletePath the color=green
        and if the edge is also in taxiRoutesPath changes the color to red.
    """
       
    def countEdges(line): 
        #get Taxi-Id
        id=line.split('"')[1]
        line=inputFile.next() #go to next line with edges
         
        words=line[line.find(">")+1:line.find("</")].split(" ")
        lastEdge=words[0]
        no=0
        for edge in words[1:]:            
            if lastEdge!=edge:
                no+=1
            lastEdge=edge
        #no=len(line[line.find(">")+1:line.find("</")].split(" ")) #splited in single edges
        routeDict.setdefault(id,[]).append(no)        
            
    inputFile=open(path.taxiRoutesComplete,'r')
    for line in inputFile:
        if line.find("<vehicle id=\""+taxi+"\"")!=-1 and not avg:
            line=inputFile.next()
            #         delete edges tag at start and end    
            words=line[line.find(">")+1:line.find("</")].split(" ")
            for edge in words:
                edgeList.append("id=\""+edge+"\" no=\"0.9\"")
            print edgeList
            
        #for calc of avg    
        if line.find("<vehicle id=")!=-1 and avg:
            countEdges(line) 
    inputFile.close()
    
    #read taxiRoutesPath
    inputFile=open(path.taxiRoutes,'r')
    for line in inputFile:
        if line.find("<vehicle id=\""+taxi+"\"")!=-1 and not avg:
            line=inputFile.next()
            #         delete edges tag at start and end    
            words=line[line.find(">")+1:line.find("</")].split(" ")
            for edge in words:                
                if "id=\""+edge+"\" no=\"0.9\"" in edgeList:
                    edgeList[edgeList.index("id=\""+edge+"\" no=\"0.9\"")]="id=\""+edge+"\" no=\"0.1\""
                elif "id=\""+edge+"\" no=\"0.1\"" not in edgeList:
                    # if edge only is in the taxiRoutesPath file (FCD) color =yellow
                    edgeList.append("id=\""+edge+"\" no=\"0.5\"")
            print edgeList
            
        #for calc of avg    
        if line.find("<vehicle id=")!=-1 and avg:
            countEdges(line) 
    inputFile.close()

def clacAvg():
    diffList=[]
    orgList=[]
    compList=[]
    for id,noList in routeDict.iteritems():        
        if len(noList)<2:
            continue       
        orgList.append(noList[1])  
        compList.append(noList[0])  
        diffList.append(noList[0]-noList[1])
        #In Prozent
        # Kanten die nur in Sumo sind mit "/" bei berechnung entfernen
        
        
    print diffList
    print "sum", sum(diffList)
    print "len", len(diffList)
    print "avg", sum(diffList)/(len(diffList)+0.0)
    print "avgOrg", sum(orgList)/(len(orgList)+0.0)," edges"
    print "avgCompleted", sum(compList)/(len(compList)+0.0)," edges"
    print "div comp-org", (sum(compList)/(len(compList)+0.0))-(sum(orgList)/(len(orgList)+0.0))," edges"
    
    
def writeOutput():
    """Writes an XML-File with the extracted results"""
    outputFile=open(path.fcdVsCompleteRoute+str(taxi)+".out.xml",'w')
    outputFile.write("<netstats>\n")
    outputFile.write("\t<interval begin=\"0\" end=\"899\" id=\"dump_900\">\n")
    for k in edgeList:         
        outputFile.write("\t\t<edge "+k+" color=\"1.0\"/>\n")
    outputFile.write("\t</interval>\n")        
    outputFile.write("</netstats>")

    
#start the program
main()