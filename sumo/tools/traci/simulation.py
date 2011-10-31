# -*- coding: utf-8 -*-
"""
@file    simulation.py
@author  Michael.Behrisch@dlr.de
@date    2011-03-15
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import traci, struct
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.VAR_TIME_STEP:                         traci.Storage.readInt,
                     tc.VAR_LOADED_VEHICLES_NUMBER:            traci.Storage.readInt,
                     tc.VAR_LOADED_VEHICLES_IDS:               traci.Storage.readStringList,
                     tc.VAR_DEPARTED_VEHICLES_NUMBER:          traci.Storage.readInt,
                     tc.VAR_DEPARTED_VEHICLES_IDS:             traci.Storage.readStringList,
                     tc.VAR_ARRIVED_VEHICLES_NUMBER:           traci.Storage.readInt,
                     tc.VAR_ARRIVED_VEHICLES_IDS:              traci.Storage.readStringList,
                     tc.VAR_MIN_EXPECTED_VEHICLES:             traci.Storage.readInt,
                     tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER: traci.Storage.readInt,
                     tc.VAR_TELEPORT_STARTING_VEHICLES_IDS:    traci.Storage.readStringList,
                     tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER:   traci.Storage.readInt,
                     tc.VAR_TELEPORT_ENDING_VEHICLES_IDS:      traci.Storage.readStringList,
                     tc.VAR_DELTA_T:                           traci.Storage.readInt,
                     tc.VAR_NET_BOUNDING_BOX:                  lambda(result): (result.read("!dd"), result.read("!dd"))}
subscriptionResults = {}

def _getUniversal(varID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_SIM_VARIABLE, varID, "")
    return RETURN_VALUE_FUNC[varID](result)

def getCurrentTime():
    return _getUniversal(tc.VAR_TIME_STEP)

def getLoadedNumber():
    return _getUniversal(tc.VAR_LOADED_VEHICLES_NUMBER)

def getLoadedIDList():
    return _getUniversal(tc.VAR_LOADED_VEHICLES_IDS)

def getDepartedNumber():
    return _getUniversal(tc.VAR_DEPARTED_VEHICLES_NUMBER)

def getDepartedIDList():
    return _getUniversal(tc.VAR_DEPARTED_VEHICLES_IDS)

def getArrivedNumber():
    return _getUniversal(tc.VAR_ARRIVED_VEHICLES_NUMBER)

def getArrivedIDList():
    return _getUniversal(tc.VAR_ARRIVED_VEHICLES_IDS)

def getMinExpectedNumber():
    return _getUniversal(tc.VAR_MIN_EXPECTED_VEHICLES)

def getStartingTeleportNumber():
    return _getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_NUMBER)

def getStartingTeleportIDList():
    return _getUniversal(tc.VAR_TELEPORT_STARTING_VEHICLES_IDS)

def getEndingTeleportNumber():
    return _getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_NUMBER)

def getEndingTeleportIDList():
    return _getUniversal(tc.VAR_TELEPORT_ENDING_VEHICLES_IDS)

def getDeltaT():
    return _getUniversal(tc.VAR_DELTA_T)

def getNetBoundary():
    return _getUniversal(tc.VAR_NET_BOUNDING_BOX)

def convert2D(edgeID, pos, laneIndex=0, toGeo=False):
    posType = tc.POSITION_2D
    if toGeo:
        posType = tc.POSITION_LAT_LON
    traci._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1+4 + 1+4+len(edgeID)+8+1 + 1+8+8)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
    traci._message.string += struct.pack("!Bi", tc.POSITION_ROADMAP, len(edgeID)) + edgeID
    traci._message.string += struct.pack("!dBBdd", pos, laneIndex, posType, 0., 0.)
    return traci._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "").read("!dd")

def convertRoad(x, y, isGeo=False):
    posType = tc.POSITION_2D
    if isGeo:
        posType = tc.POSITION_LAT_LON
    traci._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "", 1+4 + 1+8+8 + 1+4+8+1)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
    traci._message.string += struct.pack("!Bdd", posType, x, y)
    traci._message.string += struct.pack("!BidB", tc.POSITION_ROADMAP, 0, 0., 0)
    result = traci._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.POSITION_CONVERSION, "")
    return result.readString(), result.readDouble(), result.read("!B")[0]

def getDistance2D(x1, y1, x2, y2, isGeo=False, isDriving=False):
    posType = tc.POSITION_2D
    if isGeo:
        posType = tc.POSITION_LAT_LON
    distType = tc.REQUEST_AIRDIST
    if isDriving:
        distType = tc.REQUEST_DRIVINGDIST
    traci._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "", 1+4 + 1+8+8 + 1+8+8 + 1)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
    traci._message.string += struct.pack("!Bdd", posType, x1, y1)
    traci._message.string += struct.pack("!BddB", posType, x2, y2, distType)
    return traci._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "").readDouble()

def getDistanceRoad(edgeID1, pos1, edgeID2, pos2, isDriving=False):
    distType = tc.REQUEST_AIRDIST
    if isDriving:
        distType = tc.REQUEST_DRIVINGDIST
    traci._beginMessage(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "", 1+4 + 1+4+len(edgeID1)+8+1 + 1+4+len(edgeID2)+8+1 + 1)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 3)
    traci._message.string += struct.pack("!Bi", tc.POSITION_ROADMAP, len(edgeID1)) + edgeID1
    traci._message.string += struct.pack("!dBBi", pos1, 0, tc.POSITION_ROADMAP, len(edgeID2)) + edgeID2
    traci._message.string += struct.pack("!dBB", pos2, 0, distType)
    return traci._checkResult(tc.CMD_GET_SIM_VARIABLE, tc.DISTANCE_REQUEST, "").readDouble()


def subscribe(varIDs=(tc.VAR_DEPARTED_VEHICLES_IDS,), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_SIM_VARIABLE, begin, end, "x", varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(objectID, varID, data):
    subscriptionResults[varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults():
    return subscriptionResults
