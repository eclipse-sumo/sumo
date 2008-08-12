# -*- coding: Latin-1 -*-
"""
@file    FetchData.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-15

Fetches Information of the simulated Data and the FCD.
For each taxi the time, speed and position of every step will be collected and stored in a file. 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

import profile
import util.Path as path
from util.CalcTime import getTimeInSecs
from fcdToRoutes.GenerateTaxiRoutes import readFCDComplete,readSimFCDComplete,getSimTaxiId  
from util.Reader import readRoute_Edges

#global vars
rawFcdDict={}
fcdDict={}
simRawFcdDict={}
simFcdDict={}
vtypeDict={}
withoutEmptyEdges=True # if True edges with no computed traffic in the net will be ignored and not pushed in the analysis file.


def main(): 
    print "start program"
    arrangeData()
    print "end"
     
     
def arrangeData():
    """
    Arranges all available Data in a file which looks like:
    <vehicle id=" ">
                <step time="(s)" source=" " speed="(km/h)" edge=" " lat=" " lon=" "/> 
                ...
    </vehicle>
    """
    fcdTuple= None
    rawTuple= (None,None,None,None)
    
    #read needed files    
    print "read rawFcdDict file"; rawFcdDict=readRawFCD(path.rawFcd)
    print "read simRawFcdDict file"; simRawFcdDict=readRawFCD(path.simRawFcd, True)
    
    print "read readVtypeprobe file"; readVtypeprobe()    
    
    
    print "read fcdDict file"; fcdDict=readFCDComplete(path.fcd)
    print "read simFcdDict file"; simFcdDict=readSimFCDComplete(path.simFcd)
    
    if withoutEmptyEdges: 
        print "read drivenEdges file"; drivenEdgesSet=readRoute_Edges()
        outputFile=open(path.analysisWEE,'w')
    else: outputFile=open(path.analysis,'w')
    outputFile.write("<vehicles>\n")
    
    print "write Infos"  
    #iterate over the lowest common denominator
   
    for taxiId in simFcdDict.keys():       
           outputFile.write("\t<vehicle id=\"%s\">\n" %taxiId)      
           mainId=taxiId.split("_")[0]
           #write vtypeprobe Infos           
           try:
               for vtypeTuple in vtypeDict[taxiId]:
                   #skip edges without traffic
                   if withoutEmptyEdges and vtypeTuple[1] not in drivenEdgesSet:
                       continue                    
                   outputFile.write("\t\t<step time=\"%s\" source=\"vtypeProbe\" speed=\"%s\" rawSpeed=\"%s\" edge=\"%s\" lat=\"%s\" lon=\"%s\"/>\n" 
                                    %(vtypeTuple[0],vtypeTuple[4],None,vtypeTuple[1],vtypeTuple[2],vtypeTuple[3]))
                
               #write fcd Infos (enhanced with Infos of raw-FCD)
               for fcdTuple in fcdDict[taxiId]: 
                   #skip edges without traffic
                   if withoutEmptyEdges and fcdTuple[1] not in drivenEdgesSet:
                       continue                                      
                   #search for proper tuple in raw-FCD
                   for tuple in rawFcdDict[mainId]:                   
                       if fcdTuple[0]-5<tuple[0]<fcdTuple[0]+5: #if time is equal (+/- 5 secs)
                           rawTuple=tuple
                           break  
                  
                   #write results in file
                   outputFile.write("\t\t<step time=\"%s\" source=\"FCD\" speed=\"%s\" rawSpeed=\"%s\" edge=\"%s\" lat=\"%s\" lon=\"%s\"/>\n" 
                                    %(fcdTuple[0],fcdTuple[2],rawTuple[3],fcdTuple[1],rawTuple[1],rawTuple[2]))
                   
                   #Reset values after write of line:                   
                   rawTuple= (None,None,None,None)
                
               #write simFcd Infos (enhanced with Infos of raw-FCD)                            
               for fcdTuple in simFcdDict[taxiId]:
                   #skip edges without traffic
                   if withoutEmptyEdges and fcdTuple[1] not in drivenEdgesSet:
                       continue 
                   #search for proper tuple in raw-FCD
                   for tuple in simRawFcdDict[taxiId]:                   
                       if fcdTuple[0]-2<tuple[0]<fcdTuple[0]+2: #if time is equal (+/- 5 secs)
                           rawTuple=tuple
                           break  
                   
                   #write results in file
                   outputFile.write("\t\t<step time=\"%s\" source=\"simFCD\" speed=\"%s\" rawSpeed=\"%s\" edge=\"%s\" lat=\"%s\" lon=\"%s\"/>\n" 
                                    %(fcdTuple[0],fcdTuple[2],rawTuple[3],fcdTuple[1],rawTuple[1],rawTuple[2]))
                   
                   #Reset values after write of line:                   
                   rawTuple= (None,None,None,None)
           except KeyError:
               print "Warning: taxId %s not found!" %taxiId 
           outputFile.write("\t</vehicle>\n")                 
           
     
    outputFile.write("</vehicles>\n")
    outputFile.close()


def readRawFCD(rawFcdPath, sim=False):
    """Reads the Raw-FCD-File and creates a list of Id's with a belonging List of Data tuples."""
    rawDict={}
    inputFile=open(rawFcdPath,'r')
    inputFile.seek(30)
    for line in inputFile:
        words= line.split("\t") 
        if sim: #id's of simulation raw data must be converted
            words[0]=getSimTaxiId(words[0])
              
        if words[0] in rawDict:            
            #           Veh_ID            time                     lat       lon        speed
            rawDict[words[0]].append((getTimeInSecs(words[1]), words[3], words[2], words[5][:-1]))
        else:
            #           Veh_ID            time               lat       lon        speed
            rawDict[words[0]]=[(getTimeInSecs(words[1]),words[3], words[2], words[5][:-1])]        
    inputFile.close()
    return rawDict
         
def readVtypeprobe():
    """Reads the vtypeprobe-File and creates a list of Id's with a belonging List of Data tuples."""       
    inputFile=open(path.vtypeprobe,'r')
    for line in inputFile:
        if line.find("<timestep")!=-1:
            words=line.split('"')
            time=words[1]
        elif line.find("<vehicle id=")!=-1:
            words=line.split('"')
            #set comma on 2nd position 
            words[11]=words[11][:2]+"."+words[11].replace('.','')[2:]
            words[13]=words[13][:2]+"."+words[13].replace('.','')[2:]
            if words[1] in vtypeDict:
                #        Veh_ID            time  edge             lat       lon       speed form m/s in km/h
                vtypeDict[words[1]].append((time,words[3][:-2],words[11], words[13],str(int(round(float(words[15])*3.6)))))
            else:
                #         Veh_ID      time  edge             lat       lon       speed form m/s in km/h
                vtypeDict[words[1]]=[(time,words[3][:-2],words[11], words[13],str(int(round(float(words[15])*3.6))))]
                  
                             
def testFCDUniqueness():
    "Test if for every timestamp in vls-File an unique timestamp in rawFCD exists."
    taxiId="464"
    list=set()
        
    inputFile=open(path.rawFcd,'r')
    for line in inputFile:
        words= line.split("\t")
        if words[0]==taxiId and words[4]=="90":
            list.add(words[1])
    inputFile.close()
    print len(list)
    
    inputFile=open(path.fcd,'r')
    for line in inputFile:
        words= line.split("\t")
        if words[4]==taxiId:
            if words[0]in list:                
                list.remove(words[0])    
    print len(list)   
    print list             
    inputFile.close()
    
    
    
#start the program
#profile.run('main()')
main()