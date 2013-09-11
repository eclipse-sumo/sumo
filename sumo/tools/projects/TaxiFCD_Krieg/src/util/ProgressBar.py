# -*- coding: Latin-1 -*-
"""
@file    ProgressBar.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-06-19

A simple progress bar for the console

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

def startTask(list, fct):    
    """its a simple progress bar for the console. 
    list=list of elements over which would be iterated
    fct= function which is called for every element of the list
    """    
    listLen=len(list)
    lastProz=0    
    for i in range(5,105,5): 
        s="%02d" %i
        print s,    
    print "%"   
     
    for i in range(listLen):
        actProz=(100*i/listLen)                       
        if actProz!=lastProz and actProz%5==0:
            print "**",
            lastProz=actProz
        #call the function
        fct(list[i])