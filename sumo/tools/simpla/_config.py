# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    _config.py
# @author Leonhard Luecken
# @date   2017-04-09
# @version $Id$


from collections import defaultdict
import os
import traci
from simpla._platoonmode import PlatoonMode
import simpla._reporting as rp
from simpla import SimplaException

warn = rp.Warner("Config")
report = rp.Reporter("Config")

# Default values for the configuration parameters are specified first.
# They can be overriden by specification in a configuration file.
# (see load() method below)

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

# The switch impatience factor determines the magnitude of the effect
# that an increasing waiting time has on the active speed factor of a vehicle:
# activeSpeedFactor = modeSpecificSpeedFactor/(1+impatienceFactor*waitingTime)
SWITCH_IMPATIENCE_FACTOR = 0.1

# Lanechange modes for the different platooning modes
LC_MODE = {
    # for solitary mode use default mode
    PlatoonMode.NONE: 0b1001010101,
    # for platoon leader use default mode
    PlatoonMode.LEADER: 0b1001010101,
    # for platoon follower do not change lanes, except for traci commands
    # of for strategic reasons (these override traci)
    PlatoonMode.FOLLOWER: 0b1000000010,
    # for platoon catchup as for follower
    PlatoonMode.CATCHUP: 0b1000000010,
    # for platoon catchup follower as for follower
    PlatoonMode.CATCHUP_FOLLOWER: 0b1000000010
}

# speedfactors for the different platooning modes
SPEEDFACTOR = {
    PlatoonMode.LEADER: 1.0,
    PlatoonMode.FOLLOWER: 1.0,
    PlatoonMode.CATCHUP: 1.1,
    PlatoonMode.CATCHUP_FOLLOWER: None  # is set to the same as for catchup mode below if not explicitely set
}

# file with vtype maps for platoon types
VTYPE_FILE = ""

# map of original to platooning vTypes
PLATOON_VTYPES = defaultdict(dict)

import xml.etree.ElementTree as ET


def loadVTypeMap(fn):
    '''loadVTypeMap(string) -> dict

    Reads lines of the form 'origMode:leadMode:followMode:catchupMode:catchupFollowMode' (last three elements can be omitted) from a given file and write corresponding key:value pairs to PLATOON_VTYPES
    '''
    global PLATOON_VTYPES

    with open(fn, "r") as f:
        #         if rp.VERBOSITY >= 2:
        if rp.VERBOSITY >= 0:
            print("Loading vehicle type mappings from file '%s'..." % fn)
        splits = [l.split(":") for l in f.readlines()]
        NrBadLines = 0
        for j, spl in enumerate(splits):
            if len(spl) >= 2 and len(spl) <= 5:
                stripped = list(map(lambda x: x.strip(), spl))
                origType = stripped[0]
                if origType == "":
                    raise SimplaException("Original vType must be specified in line %s of vType file '%s'!" % (j, fn))
                if rp.VERBOSITY >= 2:
                    print("original type: '%s'" % origType)

                leadType = stripped[1]
                if leadType == "":
                    raise SimplaException("Platoon leader vType must be specified in line %s of vType file '%s'!" % (j, fn))
                if rp.VERBOSITY >= 2:
                    print("platoon leader type: '%s'" % leadType)

                if (len(stripped) >= 3 and stripped[2] != ""):
                    followerType = stripped[2]
                    if rp.VERBOSITY >= 2:
                        print("platoon follower type: '%s'" % followerType)
                else:
                    followerType = leadType

                if (len(stripped) >= 4 and stripped[3] != ""):
                    catchupType = stripped[3]
                    if rp.VERBOSITY >= 2:
                        print("catchup type: '%s'" % catchupType)
                else:
                    catchupType = origType

                if len(stripped) >= 5 and stripped[4] != "":
                    catchupFollowerType = stripped[4]
                    if rp.VERBOSITY >= 2:
                        print("catchup follower type: '%s'" % catchupFollowerType)
                else:
                    catchupFollowerType = origType + "_catchupFollower"
                    print("NOTE: Catchup follower type '%s' for '%s' dynamically created as duplicate of '%s'" %
                          (catchupFollowerType, origType, followerType))
                    traci.vehicletype.copy(followerType, catchupFollowerType)
                    traci.vehicletype.setColor(catchupFollowerType, (0, 255, 200, 0))

                PLATOON_VTYPES[origType][PlatoonMode.NONE] = origType
                PLATOON_VTYPES[origType][PlatoonMode.LEADER] = leadType
                PLATOON_VTYPES[origType][PlatoonMode.FOLLOWER] = followerType
                PLATOON_VTYPES[origType][PlatoonMode.CATCHUP] = catchupType
                PLATOON_VTYPES[origType][PlatoonMode.CATCHUP_FOLLOWER] = catchupFollowerType
            else:
                NrBadLines += 1
        if NrBadLines > 0:
            warn("vType file '%s' contained %d lines that were not parsed into a colon-separated sequence of strings!" % (fn, NrBadLines))


def load(filename):
    '''load(string)

    This loads configuration parameters from a file and overwrites default values.
    '''
    global CONTROL_RATE, VEH_SELECTORS, MAX_PLATOON_GAP, CATCHUP_DIST, PLATOON_SPLIT_TIME
    global VTYPE_FILE, PLATOON_VTYPES, LC_MODE, SPEEDFACTOR, SWITCH_IMPATIENCE_FACTOR

    configDir = os.path.dirname(filename)
    configElements = ET.parse(filename).getroot().getchildren()
    parsedTags = []
    for e in configElements:
        parsedTags.append(e.tag)
        if e.tag == "verbosity":
            rp.VERBOSITY = int(list(e.attrib.values())[0])
        elif e.tag == "controlRate":
            rate = float(list(e.attrib.values())[0])
            if rate <= 0.:
                warn("Parameter controlRate must be positive. Ignoring given value %s." % (rate))
            else:
                CONTROL_RATE = float(rate)
        elif e.tag == "vehicleSelectors":
            VEH_SELECTORS = list(map(lambda x: x.strip(), list(e.attrib.values())[0].split(",")))
        elif e.tag == "maxPlatoonGap":
            MAX_PLATOON_GAP = float(list(e.attrib.values())[0])
        elif e.tag == "catchupDist":
            CATCHUP_DIST = float(list(e.attrib.values())[0])
        elif e.tag == "switchImpatienceFactor":
            SWITCH_IMPATIENCE_FACTOR = max(float(list(e.attrib.values())[0]), 0.)
        elif e.tag == "platoonSplitTime":
            PLATOON_SPLIT_TIME = float(list(e.attrib.values())[0])
        elif e.tag == "lcMode":
            if ("leader" in e.attrib):
                LC_MODE[PlatoonMode.LEADER] = int(e.attrib["leader"])
            if ("follower" in e.attrib):
                LC_MODE[PlatoonMode.FOLLOWER] = int(e.attrib["follower"])
            if ("catchup" in e.attrib):
                LC_MODE[PlatoonMode.CATCHUP] = int(e.attrib["catchup"])
            if ("catchupFollower" in e.attrib):
                LC_MODE[PlatoonMode.CATCHUP_FOLLOWER] = int(e.attrib["catchupFollower"])
            if ("original" in e.attrib):
                LC_MODE[PlatoonMode.NONE] = int(e.attrib["original"])
        elif e.tag == "speedFactor":
            if ("leader" in e.attrib):
                SPEEDFACTOR[PlatoonMode.LEADER] = float(e.attrib["leader"])
            if ("follower" in e.attrib):
                SPEEDFACTOR[PlatoonMode.FOLLOWER] = float(e.attrib["follower"])
            if ("catchup" in e.attrib):
                SPEEDFACTOR[PlatoonMode.CATCHUP] = float(e.attrib["catchup"])
            if ("catchupFollower" in e.attrib):
                SPEEDFACTOR[PlatoonMode.CATCHUP_FOLLOWER] = float(e.attrib["catchupFollower"])
            if ("original" in e.attrib):
                SPEEDFACTOR[PlatoonMode.NONE] = float(e.attrib["original"])
        elif e.tag == "vTypeMapFile":
            VTYPE_FILE = os.path.join(configDir, list(e.attrib.values())[0])
        elif e.tag == "vTypeMap":
            origType = e.attrib["original"]
            PLATOON_VTYPES[origType][PlatoonMode.NONE] = origType
            if ("leader" in e.attrib):
                leaderType = e.attrib["leader"]
                PLATOON_VTYPES[origType][PlatoonMode.LEADER] = leaderType
                # print("Registering vtype map '%s':'%s'"%(origType,leaderType))
            if ("follower" in e.attrib):
                followerType = e.attrib["follower"]
                PLATOON_VTYPES[origType][PlatoonMode.FOLLOWER] = followerType
                # print("Registering vtype map '%s':'%s'"%(origType,followerType))
            if ("catchup" in e.attrib):
                catchupType = e.attrib["catchup"]
                PLATOON_VTYPES[origType][PlatoonMode.CATCHUP] = catchupType
                # print("Registering vtype map '%s':'%s'"%(origType,followerType))
            if ("catchupFollower" in e.attrib):
                catchupFollowerType = e.attrib["catchupFollower"]
                PLATOON_VTYPES[origType][PlatoonMode.CATCHUP_FOLLOWER] = catchupFollowerType
                # print("Registering vtype map '%s':'%s'"%(origType,followerType))
        else:
            warn("Encountered unknown configuration parameter '%s'!" % e.tag)

    if "vTypeMapFile" in parsedTags:
        # load vType mapping from file
        loadVTypeMap(VTYPE_FILE)

    if SPEEDFACTOR[PlatoonMode.CATCHUP_FOLLOWER] is None:
        # if unset, set speedfactor for catchupfollower mode to the same as in catchup mode
        SPEEDFACTOR[PlatoonMode.CATCHUP_FOLLOWER] = SPEEDFACTOR[PlatoonMode.CATCHUP]
