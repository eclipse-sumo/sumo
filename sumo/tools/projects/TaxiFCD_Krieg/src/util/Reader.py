# -*- coding: Latin-1 -*-
"""
@file    CalcTime.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-18

Contains reader modules which are needed frequently

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

import util.Path as path


def readVLS_Edges():    
    """Reads the net file and returns a list of all edges""" 
               
    inputFile=open(path.net,'r')
    for line in inputFile:
        if line.find(" <edges")!=-1:
            #         delete edges tag at start and end    
            words=line[line.find(">")+1:line.find("</")].split(" ")            
            break
    inputFile.close()
    return words