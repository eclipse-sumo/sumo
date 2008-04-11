# -*- coding: Latin-1 -*-
"""
@file    GenerateRawFCD.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-01

Converts the vtypeProbe XML-File to the raw FCD-Format which can afterwards used to process FCD. 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""
from time import localtime
from time import mktime
from time import strftime

#creates new vehicle id's which consists only numerics
def getVehId(orgId):
    global vehId    
    value=vehIdList.get(orgId,vehId)
    if value is vehId:
        vehIdList[orgId]=vehId
        vehId+=1        
    return value
     

#Main
print "starting"
generallPath="D:/Krieg/Projekte/Diplom/Daten/simRawFCD/"
inputPath="vtypeprobeTestlauf1.out.xml"
outputPath="simulatedRawFCD.out.dat"

inputFile=open(generallPath+inputPath,'r')
outputFile=open(generallPath+outputPath,'w')

# used simulation date in seconds
simDate=mktime((2007,7,18,0,0,0,2,199,1))
#used Date-format in the database
format="%Y-%m-%d %H:%M:%S" 
vehIdList={}
vehId=0
  
for line in inputFile:
    words=line.split('"')
    
    #get time
    if line.find("<timestep")!=-1:
        time=words[1]
    #if line starts with "<vehicle"
    if line.find("<vehicle id")!=-1:        
        #                veh_id                       date (time to simDate+time)                            x (remove and set comma new)             
        outputFile.write(str(getVehId(words[1]))+'\t'+strftime(format,localtime(simDate+ int(time)))+'\t'+words[13][0:2]+'.'+words[13][2:7]+words[13][8:]+
                         #     y (remove and set comma new)                       status      speed form m/s in km/h
                         '\t'+words[11][0:2]+'.'+words[11][2:7]+words[11][8:]+'\t'+"90"+'\t'+str(int(round(float(words[15])*3.6)))+'\n')    
outputFile.close()    
inputFile.close()
print 'END conversion'



    
    
    
    
    
    