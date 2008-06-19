# -*- coding: Latin-1 -*-
"""
@file    Reader.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-18

Contains reader modules which are needed frequently

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

import util.Path as path
from analysis.Taxi import * 
from cPickle import dump

def readVLS_Edges():    
    """Reads the net file and returns a list of all edges.""" 
               
    inputFile=open(path.net,'r')
    for line in inputFile:
        if line.find(" <edges")!=-1:
            #         delete edges tag at start and end    
            words=line[line.find(">")+1:line.find("</")].split(" ")            
            break
    inputFile.close()
    return words

def readAnalysisInfo():
    """Reads the analysis file and returns a list off taxis with all available information."""
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
    
    inputFile.close()         
    return taxiList  

def readEdgesLength():
    """Reads the length of each edge with occurs in the analysis-File and saves them in a pickled file."""  
    edgeDict={}
    
    #search the edges
    taxiList=readAnalysisInfo()
    for taxi in taxiList:
        for step in taxi.getSteps():
            if step.source==SOURCE_FCD or step.source==SOURCE_SIMFCD:
                edgeDict[step.edge]=0.0
                
    #search the length  (in m)
    inputFile=open(path.net,'r')
    
    for line in inputFile:
        words=line.split('"')
        try: 
            index=words.index(" length=")            
            if words[1][:-2] in edgeDict and words[1][-2:]=='_0':                
                edgeDict[words[1][:-2]]=float(words[index+1])                            
        except ValueError:
            #do nothing
            None   
                
    #pickles the edgeDict 
    dump(edgeDict, open(path.edgeLengthDict,'w'))               
    inputFile.close()
        
