"""
@author Leonhard Luecken
@date   2017-04-09

-----------------------------------
SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
-----------------------------------
"""

from collections import defaultdict
from simpla._platoonmode import PlatoonMode

# Default values for the configuration parameters are specified first.
# They can be overriden by specification in a configuration file.
# (see load() method below)


# control level of verbosity
VERBOSITY = 1

# Rate for updating the platoon manager checks and advices
CONTROL_RATE = 1.0

# specify substring for vtypes, that should be controlled by platoon manager
VEH_SELECTORS = [""]

# Distance in meters below which a vehicle joins a leading platoon
MAX_PLATOON_GAP = 15.0

# Distance in meters below which a vehicle tries to catch up with a platoon in front
CATCHUP_DIST = 50.0

# Latency time in secs. until a platoon is split if vehicles exceed PLATOON_SPLIT_DISTANCE to their 
# leaders within a platoon (or if they are not the direct follower),
# or drive on different lanes than their leader within the platoon 
PLATOON_SPLIT_TIME = 3.0

# Lanechange modes for the different platooning modes
LC_MODE = {
    # for solitary mode use default mode
    PlatoonMode.NONE   : 0b1001010101,
    # for platoon leader use default mode
    PlatoonMode.LEADER   : 0b1001010101,
    # for platoon follower do not change lanes, except for traci commands
    # of for strategic reasons (these override traci)
    PlatoonMode.FOLLOWER : 0b1000000010,
    # for platoon catchup as for follower
    PlatoonMode.CATCHUP  : 0b1000000010
    }

# speedfactors for the different platooning modes
SPEEDFACTOR = {
    PlatoonMode.LEADER   : 1.0,
    PlatoonMode.FOLLOWER : 1.0,
    PlatoonMode.CATCHUP  : 1.1
    }


# files with pickled vtype maps for platoon leaders and followers  
VTYPE_FILE_LEADER = ""  
VTYPE_FILE_FOLLOWER = ""
VTYPE_FILE_CATCHUP = ""

# map of original to platooning vTypes 
PLATOON_VTYPES = defaultdict(dict)

import xml.etree.ElementTree as ET


def loadVTypeMap(fn):
    '''readPairs(string) -> dict
    
    Reads lines of the form 'origMode:leadMode:followMode:catchupMode' (last two elements can be omitted) from a given file and write corresponding key:value pairs to PLATOON_VTYPES 
    '''
    global PLATOON_VTYPES
    
    with open(fn,"r") as f:
        splits = [l.split(":") for l in f.readlines()]
        NrBadLines = 0
        for j, spl in enumerate(splits):
            if len(spl)>=2 and len(spl)<=4:
                stripped = map(lambda x: x.strip(), spl)
                origType = stripped[0]
                if origType == "":
                    print("WARNING: original vType must be specified in line %s of vType file '%s'!"%(j, fn))
                    continue
                leadType = stripped[1]
                if leadType == "":
                    print("WARNING: platoon leader vType must be specified in line %s of vType file '%s'!"%(j, fn))
                    continue
                followerType = stripped[2] if (len(stripped)>=3 and stripped[2]!="") else leadType
                catchupType = stripped[3] if (len(stripped)>=4 and stripped[3]!="") else origType
                PLATOON_VTYPES[origType][PlatoonMode.NONE] = origType
                PLATOON_VTYPES[origType][PlatoonMode.LEADER] = leadType
                PLATOON_VTYPES[origType][PlatoonMode.FOLLOWER] = followerType
                PLATOON_VTYPES[origType][PlatoonMode.CATCHUP] = catchupType
            else:
                NrBadLines+=1
        if NrBadLines > 0:
            print("WARNING: vType file '%s' contained %d lines that were not parsed into a colon-separated sequence of strings!"%(fn, NrBadLines))


def load(filename):
    '''load(string)
    
    This loads configuration parameters from a file and overwrites default values.
    '''
    global VERBOSITY, CONTROL_RATE, VEH_SELECTORS, MAX_PLATOON_GAP, CATCHUP_DIST, PLATOON_SPLIT_TIME
    global VTYPE_FILE, PLATOON_VTYPES, LC_MODE, SPEEDFACTOR
    
    configElements = ET.parse(filename).getroot().getchildren()
    parsedTags = []
    for e in configElements:
        parsedTags.append(e.tag)
        if e.tag == "verbosity":
            VERBOSITY = int(e.attrib.values()[0])
        elif e.tag == "controlRate":
            rate = e.attrib.values()[0]
            if rate <= 0.:             
                print("WARNING: Parameter controlRate must be positive. Ignoring given value %s."%(rate))
            else:
                CONTROL_RATE = float(rate)
        elif e.tag == "vehicleSelectors":
            VEH_SELECTORS = map(lambda x: x.strip(), e.attrib.values()[0].split(","))
        elif e.tag == "maxPlatoonGap":
            MAX_PLATOON_GAP = float(e.attrib.values()[0])
        elif e.tag == "catchupDist":
            CATCHUP_DIST = float(e.attrib.values()[0])
        elif e.tag == "platoonSplitTime":
            PLATOON_SPLIT_TIME = float(e.attrib.values()[0])
        elif e.tag == "lcMode":
            if (e.attrib.has_key("leader")):
                LC_MODE[PlatoonMode.LEADER] = int(e.attrib["leader"])
            if (e.attrib.has_key("follower")):
                LC_MODE[PlatoonMode.FOLLOWER] = int(e.attrib["follower"])
            if (e.attrib.has_key("catchup")):
                LC_MODE[PlatoonMode.CATCHUP] = int(e.attrib["catchup"])
            if (e.attrib.has_key("original")):
                LC_MODE[PlatoonMode.NONE] = int(e.attrib["original"])
        elif e.tag == "speedFactor":
            if (e.attrib.has_key("leader")):
                SPEEDFACTOR[PlatoonMode.LEADER] = float(e.attrib["leader"])
            if (e.attrib.has_key("follower")):
                SPEEDFACTOR[PlatoonMode.FOLLOWER] = float(e.attrib["follower"])
            if (e.attrib.has_key("catchup")):
                SPEEDFACTOR[PlatoonMode.CATCHUP] = float(e.attrib["catchup"])
            if (e.attrib.has_key("original")):
                SPEEDFACTOR[PlatoonMode.NONE] = float(e.attrib["original"])
        elif e.tag == "vTypeMapFile":
            VTYPE_FILE = e.attrib.values()[0]
        elif e.tag == "vTypeMap":
            origType = e.attrib["original"]
            PLATOON_VTYPES[origType][PlatoonMode.NONE] = origType
            if (e.attrib.has_key("leader")):
                leaderType = e.attrib["leader"]
                PLATOON_VTYPES[origType][PlatoonMode.LEADER] = leaderType
                #print("Registering vtype map '%s':'%s'"%(origType,leaderType))
            if (e.attrib.has_key("follower")):
                followerType = e.attrib["follower"]
                PLATOON_VTYPES[origType][PlatoonMode.FOLLOWER] = followerType
                #print("Registering vtype map '%s':'%s'"%(origType,followerType))
            if (e.attrib.has_key("catchup")):
                catchupType = e.attrib["catchup"]
                PLATOON_VTYPES[origType][PlatoonMode.CATCHUP] = origType
                #print("Registering vtype map '%s':'%s'"%(origType,followerType))
        else:
            print("WARNING: Encountered unknown configuration parameter '%s'!"%e.tag)
    
    if "vTypeMapFile" in parsedTags:
        # load vType mapping from file
        loadVTypeMap(VTYPE_FILE)


if __name__=="__main__":
    ## Statements for debugging
    import os
#     print(os.getcwd())
#     
#     VTYPE_FILE_LEADER = "../AIMsubnet/pkwPlatoonLeaderVTypes.map"
#     VTYPE_FILE_FOLLOWER = "../AIMsubnet/pkwPlatoonFollowerVTypes.map"
#     loadVTypeMap(VTYPE_FILE_LEADER, PlatoonMode.LEADER)
    
    os.chdir("..")

    configfile = "simpla_example.cfg.xml"
    load(configfile)
    
    print(PLATOON_VTYPES)
    print(VEH_SELECTORS)



