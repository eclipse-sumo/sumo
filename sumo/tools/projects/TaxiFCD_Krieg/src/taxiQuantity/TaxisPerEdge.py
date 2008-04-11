# -*- coding: Latin-1 -*-
"""
@file    TaxisPerEdge.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-08

Counts for every edge in the given FCD-file the number of Taxis which used this edge. 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

edgeList={}
vlsPath="D:/Krieg/Projekte/Diplom/Daten/originalFCD/Proz-fcd_nuremberg_VLS_2007-07-18.dat"
outputPath="taxisPerEdge.out.xml"

def TaxisPerEdge():
    print "start program"
    countTaxisForEachEdge()
    writeOutput()
    print "end"
    
def countTaxisForEachEdge():
    """counts the frequency of each edge"""
    inputFile=open(vlsPath,'r')
    for line in inputFile:
        words= line.split("\t")
        edgeList.setdefault(words[1],set())
        edgeList[words[1]].add(words[4])
    
    for k in edgeList:     
        print k
        print len(edgeList[k])
    print len(edgeList)

def writeOutput():
    """Writes an XML-File with the extracted results"""
    outputFile=open(outputPath,'w')
    outputFile.write("<netstats>\n")
    outputFile.write("\t<interval begin=\"0\" end=\"899\" id=\"dump_900\">\n")
    for k in edgeList:  
        outputFile.write("\t\t<edge id=\"%s\" no=\"%s\" color=\"1.0\"/>\n" %(k, len(edgeList[k])))
    outputFile.write("\t</interval>\n")        
    outputFile.write("</netstats>")
    
#start the program
TaxisPerEdge()