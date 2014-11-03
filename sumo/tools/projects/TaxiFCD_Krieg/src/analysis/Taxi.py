# -*- coding: Latin-1 -*-
"""
@file    CalcTime.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-17
@version $Id$

Represents a single Taxi.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

#Constants
SOURCE_VTYPE='vtypeProbe'
SOURCE_FCD='FCD'
SOURCE_SIMFCD='simFCD'


class Step(object):
    """Represents a single Step like its used in the analysis File.""" 
    def __init__(self, time, source, speed, rawSpeed, edge, lat, lon):
        self.time=int(time)
        self.source=source
        self.speed=int(speed)
        if rawSpeed=='None':
            self.rawSpeed=None
        else:
            self.rawSpeed=int(rawSpeed)
        self.edge=edge
        if lat=='None':
            self.lat=None
        else:
            self.lat=lat
        if lon=='None':
            self.lon=None
        else:
            self.lon=lon
    def __str__(self):
        return "(%s, %s, %s, %s, %s, %s, %s)" %(self.time, self.source, self.speed, self.rawSpeed, self.edge, self.lat, self.lon)  
    def __repr__(self):
        return self.__str__()      
    
     
class Taxi(object):
    """Represents a Taxi (with ID and all available Data form the analysis File)."""
    def __init__ (self,id):
        self.id=id
        self.steps=[]
        
    def addStep(self,step):        
        self.steps.append(step)
    
    def getSteps(self):
        return self.steps            
    
    def __str__(self):
        return "%s, %s" %(self.id, self.steps)  
    def __repr__(self):
        return self.__str__()   
    
    def __eq__(self, id):
        return self.id==id 
