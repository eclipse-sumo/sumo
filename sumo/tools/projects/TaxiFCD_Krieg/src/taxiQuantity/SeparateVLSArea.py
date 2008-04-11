# -*- coding: Latin-1 -*-
"""
@file    SeparateVLSArea.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-07

Separates from an FCD-File (from Nürnberg) all routes which belongs to the VLS-Area and saves them in a new file. 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

#global vars
fcdPath="D:/Krieg/Projekte/Diplom/Daten/originalFCD/Proz-fcd_nuremberg_2007-07-18.dat"
vlsPath="D:/Krieg/Projekte/Diplom/Daten/originalFCD/Proz-fcd_nuremberg_VLS_2007-07-18.dat"
netPath="D:/Krieg/Projekte/Diplom/Daten/sumoNetzFilesNurnbergIIProjektion/nuernberg_vls_new.net.xml"

def SeparateVLSArea():
    """Main"""
    print "start"
    generateVLS_FCD_File()
    print"end"
    
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

def generateVLS_FCD_File():
    """Creates a new FCD-file which contains only the rows which edges belongs to the VLS-Area"""    
    outputVLSFile=open(vlsPath,'w')
    inputFile=open(fcdPath,'r')
    
    vlsEdgeList=readVLS_Edges()    
    
    for line in inputFile:
        words=line.split("\t")
        #check if edge belongs to the VLS-Area
        if words[1] in vlsEdgeList:            
            outputVLSFile.write(line) 
    inputFile.close()    
    outputVLSFile.close()    
    
    
#start the program
SeparateVLSArea()