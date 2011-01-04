# -*- coding: Latin-1 -*-
"""
@file    ProgressBar.py
@author  Sascha.Krieg@dlr.de
@date    2008-06-19

A simple progress bar for the console

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
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