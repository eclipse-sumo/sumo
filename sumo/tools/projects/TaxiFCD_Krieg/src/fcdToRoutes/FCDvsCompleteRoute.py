# -*- coding: Latin-1 -*-
"""
@file    FCDvsCompleteRoute.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-08

Compares the FCD-route of an taxi with the generated route which is used in the simulation.
The output data can be visualized with the script My_mpl_dump_onNet based on mpl_dump_onNet from Daniel.

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

import util.Path as path

#global vars

edgeList=[]
taxi="70_1" #the Taxi for which the output should be generated


def main():
    print "start program"    
    readRoutes()
    writeOutput()
    print "end"
    

def readRoutes():
    """reads the t1CompletePath and the taxiRoutesPath. 
        and sets for each edge in t1CompletePath the color=green
        and if the edge is also in taxiRoutesPath changes the color to red.
    """
    inputFile=open(path.taxiRoutesComplete,'r')
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
    inputFile=open(path.taxiRoutes,'r')
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
    outputFile=open(path.fcdVsCompleteRoute+str(taxi)+".out.xml",'w')
    outputFile.write("<netstats>\n")
    outputFile.write("\t<interval begin=\"0\" end=\"899\" id=\"dump_900\">\n")
    for k in edgeList:         
        outputFile.write("\t\t<edge "+k+" color=\"1.0\"/>\n")
    outputFile.write("\t</interval>\n")        
    outputFile.write("</netstats>")

    
#start the program
main()