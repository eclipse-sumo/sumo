# -*- coding: Latin-1 -*-
"""
@file    SeparateVLSArea.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-07

Separates from an FCD-File (from Nürnberg) all routes which belongs to the VLS-Area and saves them in a new file. 

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import util.Path as path
import util.Reader as reader

def main():    
    print "start"
    generateVLS_FCD_File()
    print"end"
    
    
def generateVLS_FCD_File():
    """Creates a new FCD-file which contains only the rows which edges belongs to the VLS-Area"""    
    outputVLSFile=open(path.vls,'w')
    inputFile=open(path.fcd,'r')
    
    vlsEdgeList=reader.readVLS_Edges()   
    
    for line in inputFile:
        words=line.split("\t")
        #check if edge belongs to the VLS-Area
        if words[1] in vlsEdgeList:            
            outputVLSFile.write(line) 
    inputFile.close()    
    outputVLSFile.close()    
    
    
#start the program
main()