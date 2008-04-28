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
from analysis.Taxi import * 


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

def readAnalysisInfo():
    """Reads the analysis file and returns a list off taxis with all available information"""
    taxiList=[]
    i=-1
    inputFile=open(path.analysis,'r')
    for line in inputFile:
        words=line.split('"')
        if words[0]=='\t<vehicle id=':            
            taxiList.append(Taxi(words[1])) 
            i+=1           
        if words[0]=='\t\t<step time=':
            taxiList[i].addStep(Step(words[1],words[3],words[5],words[7],words[9],words[11],words[13]))            
               
    return taxiList    
        
