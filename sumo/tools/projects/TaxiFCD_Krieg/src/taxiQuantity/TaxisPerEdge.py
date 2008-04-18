# -*- coding: Latin-1 -*-
"""
@file    TaxisPerEdge.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-08

Counts for every edge in the given FCD-file the number of Taxis which used this edge. 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

import util.Path as path

#global vars
#outputPath="taxisPerEdge.out.xml"
outputPath="FCD_vs_completeRoute70_1.out.xml"
t1CompletePath="D:/Krieg/Projekte/Diplom/Daten/taxiRouten/t1Complete.rou.xml"

edgeList={}
taxi="70_1" #the Taxi for which the output should be generated


def main():
    global edgeList
    
    print "start program"
    edgeList=[]
    readRoutes()    
    #countTaxisForEachEdge()
    writeOutput()
    print "end"

    
def countTaxisForEachEdge():
    """counts the frequency of each edge"""
    inputFile=open(path.vls,'r')
    for line in inputFile:
        words= line.split("\t")
        edgeList.setdefault(words[1],set())
        edgeList[words[1]].add(words[4])
    
    for k in edgeList:     
        print k
        print len(edgeList[k])
    print len(edgeList)


def readRoutes():
    """reads the t1CompletePath and the taxiRoutesPath. 
        and sets for each edge in t1CompletePath the color=green
        and if the edge is also in taxiRoutesPath changes the color to red.
    """
    inputFile=open(t1CompletePath,'r')
    for line in inputFile:
        if line.find("<vehicle id=\""+taxi+"\"")!=-1:
            line=inputFile.next()
            #         delete edges tag at start and end    
            words=line[line.find(">")+1:line.find("</")].split(" ")
            for edge in words:
                edgeList.append("id=\""+edge+"\" no=\"0.9\"")
            print edgeList
            
    inputFile.close()
    
    #read taxiRoutesPath
    inputFile=open(path.routes,'r')
    for line in inputFile:
        if line.find("<vehicle id=\""+taxi+"\"")!=-1:
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
            
    inputFile.close()

    
def writeOutput():
    """Writes an XML-File with the extracted results"""
    outputFile=open(outputPath,'w')
    outputFile.write("<netstats>\n")
    outputFile.write("\t<interval begin=\"0\" end=\"899\" id=\"dump_900\">\n")
    for k in edgeList:  
        #outputFile.write("\t\t<edge id=\"%s\" no=\"%s\" color=\"1.0\"/>\n" %(k, len(edgeList[k])))
        outputFile.write("\t\t<edge "+k+" color=\"1.0\"/>\n")
    outputFile.write("\t</interval>\n")        
    outputFile.write("</netstats>")

    
#start the program
main()