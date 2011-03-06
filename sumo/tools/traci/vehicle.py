# -*- coding: utf-8 -*-
"""
@file    vehicle.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct, traci
import traci.constants as tc

RETURN_VALUE_FUNC = {tc.ID_LIST:        traci.Storage.readStringList,
                     tc.VAR_SPEED:      traci.Storage.readFloat,
                     tc.VAR_POSITION:   lambda(result): result.read("!ff"),
                     tc.VAR_ANGLE:      traci.Storage.readFloat,
                     tc.VAR_ROAD_ID:    traci.Storage.readString,
                     tc.VAR_LANE_ID:    traci.Storage.readString,
                     tc.VAR_LANE_INDEX: traci.Storage.readInt,
                     tc.VAR_TYPE:       traci.Storage.readString,
                     tc.VAR_ROUTE_ID:   traci.Storage.readString,
                     tc.VAR_LANEPOSITION:      traci.Storage.readFloat,
                     tc.VAR_COLOR:      lambda(result): result.read("!BBBB")}
subscriptionResults = {}

def _getUniversal(varID, vehID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_VEHICLE_VARIABLE, varID, vehID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getSpeed(vehID):
    return _getUniversal(tc.VAR_SPEED, vehID)

def getPosition(vehID):
    """
    Returns the position of the named vehicle within the last step [m,m]
    """
    return _getUniversal(tc.VAR_POSITION, vehID)

def getAngle(vehID):
    return _getUniversal(tc.VAR_ANGLE, vehID)

def getRoadID(vehID):
    return _getUniversal(tc.VAR_ROAD_ID, vehID)

def getLaneID(vehID):
    return _getUniversal(tc.VAR_LANE_ID, vehID)

def getLaneIndex(vehID):
    return _getUniversal(tc.VAR_LANE_INDEX, vehID)

def getTypeID(vehID):
    return _getUniversal(tc.VAR_TYPE, vehID)

def getRouteID(vehID):
    return _getUniversal(tc.VAR_ROUTE_ID, vehID)

def getLanePosition(vehID):
    return _getUniversal(tc.VAR_LANEPOSITION, vehID)

def getColor(vehID):
    return _getUniversal(tc.VAR_COLOR, vehID)

def getBestLanes(vehID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_BEST_LANES, vehID)
    result.read("!iB")
    nbLanes = result.read("!i")[0] # Length
    lanes = []
    for i in range(nbLanes):
        result.read("!B")
        laneID = result.readString()
        length = result.read("!Bf")[1]
        occupation = result.read("!Bf")[1]
        offset = result.read("!Bb")[1]
        allowsContinuation = result.read("!BB")[1]
        nextLanesNo = result.read("!Bi")[1]
        nextLanes = []
        for j in range(nextLanesNo):
            nextLanes.append(result.readString())
        lanes.append( [laneID, length, occupation, offset, allowsContinuation, nextLanes ] )
    return lanes


def subscribe(vehID, varIDs=(tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION), begin=0, end=2**31-1):
    _resetSubscriptionResults()
    traci._subscribe(tc.CMD_SUBSCRIBE_VEHICLE_VARIABLE, begin, end, vehID, varIDs)

def _resetSubscriptionResults():
    subscriptionResults.clear()

def _addSubscriptionResult(vehID, varID, data):
    if vehID not in subscriptionResults:
        subscriptionResults[vehID] = {}
    subscriptionResults[vehID][varID] = RETURN_VALUE_FUNC[varID](data)

def getSubscriptionResults(vehID=None):
    if vehID == None:
        return subscriptionResults
    return subscriptionResults.get(vehID, None)


def setMaxSpeed(vehID, speed):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_SETMAXSPEED, vehID, 1+4)
    _message.string += struct.pack("!Bf", tc.TYPE_FLOAT, speed)
    _sendExact()

def setSpeed(vehID, speed):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SPEED, vehID, 1+8)
    _message.string += struct.pack("!Bd", tc.TYPE_DOUBLE, speed)
    _sendExact()

def setLanePosition(vehID, position):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_LANEPOSITION, vehID, 1+8)
    _message.string += struct.pack("!Bd", tc.TYPE_DOUBLE, position)
    _sendExact()

def setStop(vehID, edgeID, pos, laneIndex, duration):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_STOP, vehID, 1+4+1+4+len(edgeID)+1+4+1+1+1+4)
    _message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
    _message.string += struct.pack("!B", tc.TYPE_STRING)
    _message.string += struct.pack("!i", len(edgeID)) + edgeID
    _message.string += struct.pack("!BfBBBi", tc.TYPE_FLOAT, pos, tc.TYPE_BYTE, laneIndex, tc.TYPE_INTEGER, duration)
    _sendExact()

def changeLane(vehID, laneIndex, duration):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGELANE, vehID, 1+4+1+1+1+4)
    _message.string += struct.pack("!BiBBBi", tc.TYPE_COMPOUND, 2, tc.TYPE_BYTE, laneIndex, tc.TYPE_INTEGER, duration)
    _sendExact()

def slowDown(vehID, speed, duration):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_SLOWDOWN, vehID, 1+4+1+4+1+4)
    _message.string += struct.pack("!BiBfBi", tc.TYPE_COMPOUND, 2, tc.TYPE_FLOAT, speed, tc.TYPE_INTEGER, duration)
    _sendExact()

def changeTarget(vehID, edgeID):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGETARGET, vehID, 1+4+len(edgeID))
    _message.string += struct.pack("!B", tc.TYPE_STRING)
    _message.string += struct.pack("!i", len(edgeID)) + edgeID
    _sendExact()

def setRoute(vehID, edgeList):
    """
    changes the vehicle route to given edges list.
    The first edge in the list has to be the one that the vehicle is at at the moment.
    
    example usage:
    setRoute('1', ['1', '2', '4', '6', '7'])
    
    this changes route for vehicle id 1 to edges 1-2-4-6-7
    """
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_ROUTE, vehID,
                        1+4+sum(map(len, edgeList))+4*len(edgeList))
    _message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(edgeList))
    for edge in edgeList:
        _message.string += struct.pack("!i", len(edge)) + edge
    _sendExact()

def moveTo(vehID, laneID, pos):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_MOVE_TO, vehID, 1+4+1+4+len(laneID)+8)
    _message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
    _message.string += struct.pack("!Bi", tc.TYPE_STRING, len(laneID)) + laneID
    _message.string += struct.pack("!Bd", tc.TYPE_DOUBLE, pos)
    _sendExact()

def reroute(vehID):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_REROUTE_TRAVELTIME, vehID, 1+4)
    _message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
    _sendExact()

def setColor(vehID, color):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_COLOR, vehID, 1+1+1+1+1)
    _message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    _sendExact()
