# -*- coding: utf-8 -*-
"""
@file    vehicle.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2011-03-09
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import struct, traci
import traci.constants as tc

def _readBestLanes(result):
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


RETURN_VALUE_FUNC = {tc.ID_LIST:             traci.Storage.readStringList,
                     tc.VAR_SPEED:           traci.Storage.readFloat,
                     tc.VAR_SPEED_WITHOUT_TRACI: traci.Storage.readFloat,
                     tc.VAR_POSITION:        lambda(result): result.read("!ff"),
                     tc.VAR_ANGLE:           traci.Storage.readFloat,
                     tc.VAR_ROAD_ID:         traci.Storage.readString,
                     tc.VAR_LANE_ID:         traci.Storage.readString,
                     tc.VAR_LANE_INDEX:      traci.Storage.readInt,
                     tc.VAR_TYPE:            traci.Storage.readString,
                     tc.VAR_ROUTE_ID:        traci.Storage.readString,
                     tc.VAR_COLOR:           lambda(result): result.read("!BBBB"),
                     tc.VAR_LANEPOSITION:    traci.Storage.readFloat,
                     tc.VAR_CO2EMISSION:     traci.Storage.readFloat,
                     tc.VAR_COEMISSION:      traci.Storage.readFloat,
                     tc.VAR_HCEMISSION:      traci.Storage.readFloat,
                     tc.VAR_PMXEMISSION:     traci.Storage.readFloat,
                     tc.VAR_NOXEMISSION:     traci.Storage.readFloat,
                     tc.VAR_FUELCONSUMPTION: traci.Storage.readFloat,
                     tc.VAR_NOISEEMISSION:   traci.Storage.readFloat,
                     tc.VAR_EDGE_TRAVELTIME: traci.Storage.readFloat,
                     tc.VAR_EDGE_EFFORT:     traci.Storage.readFloat,
                     tc.VAR_ROUTE_VALID:     lambda(result): bool(result.read("!B")[0]),
                     tc.VAR_EDGES:           traci.Storage.readStringList,
                     tc.VAR_SIGNALS:         traci.Storage.readInt,
                     tc.VAR_LENGTH:          traci.Storage.readFloat,
                     tc.VAR_MAXSPEED:        traci.Storage.readFloat,
                     tc.VAR_VEHICLECLASS:    traci.Storage.readString,
                     tc.VAR_SPEED_FACTOR:    traci.Storage.readFloat,
                     tc.VAR_SPEED_DEVIATION: traci.Storage.readFloat,
                     tc.VAR_EMISSIONCLASS:   traci.Storage.readString,
                     tc.VAR_WIDTH:           traci.Storage.readFloat,
                     tc.VAR_GUIOFFSET:       traci.Storage.readFloat,
                     tc.VAR_SHAPECLASS:      traci.Storage.readString,
                     tc.VAR_ACCEL:           traci.Storage.readFloat,
                     tc.VAR_DECEL:           traci.Storage.readFloat,
                     tc.VAR_IMPERFECTION:    traci.Storage.readFloat,
                     tc.VAR_TAU:             traci.Storage.readFloat,
                     tc.VAR_BEST_LANES:      _readBestLanes,
                     tc.DISTANCE_REQUEST:    traci.Storage.readFloat}
subscriptionResults = {}

def _getUniversal(varID, vehID):
    result = traci._sendReadOneStringCmd(tc.CMD_GET_VEHICLE_VARIABLE, varID, vehID)
    return RETURN_VALUE_FUNC[varID](result)

def getIDList():
    return _getUniversal(tc.ID_LIST, "")

def getSpeed(vehID):
    return _getUniversal(tc.VAR_SPEED, vehID)

def getSpeedWithoutTraCI(vehID):
    return _getUniversal(tc.VAR_SPEED_WITHOUT_TRACI, vehID)

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

def getRoute(vehID):
    return _getUniversal(tc.VAR_EDGES, vehID)

def getLanePosition(vehID):
    return _getUniversal(tc.VAR_LANEPOSITION, vehID)

def getColor(vehID):
    return _getUniversal(tc.VAR_COLOR, vehID)

def getCO2Emission(vehID):
    return _getUniversal(tc.VAR_CO2EMISSION, vehID)

def getCOEmission(vehID):
    return _getUniversal(tc.VAR_COEMISSION, vehID)

def getHCEmission(vehID):
    return _getUniversal(tc.VAR_HCEMISSION, vehID)

def getPMxEmission(vehID):
    return _getUniversal(tc.VAR_PMXEMISSION, vehID)

def getNOxEmission(vehID):
    return _getUniversal(tc.VAR_NOXEMISSION, vehID)

def getFuelConsumption(vehID):
    return _getUniversal(tc.VAR_FUELCONSUMPTION, vehID)

def getNoiseEmission(vehID):
    return _getUniversal(tc.VAR_NOISEEMISSION, vehID)

def getAdaptedTraveltime(vehID, time, edgeID):
    traci._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, vehID, 1+4+1+4+1+4+len(edgeID))
    traci._message.string += struct.pack("!BiBiBi", tc.TYPE_COMPOUND, 2, tc.TYPE_INTEGER, time,
                                         tc.TYPE_STRING, len(edgeID)) + edgeID
    return traci._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, vehID).readFloat()

def getEffort(vehID, time, edgeID):
    traci._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT, vehID, 1+4+1+4+1+4+len(edgeID))
    traci._message.string += struct.pack("!BiBiBi", tc.TYPE_COMPOUND, 2, tc.TYPE_INTEGER, time,
                                         tc.TYPE_STRING, len(edgeID)) + edgeID
    return traci._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT, vehID).readFloat()

def isRouteValid(vehID):
    return _getUniversal(tc.VAR_LENGTH, vehID)

def getSignals(vehID):
    return _getUniversal(tc.VAR_LENGTH, vehID)

def getLength(vehID):
    return _getUniversal(tc.VAR_LENGTH, vehID)

def getMaxSpeed(vehID):
    return _getUniversal(tc.VAR_MAXSPEED, vehID)

def getVehicleClass(vehID):
    return _getUniversal(tc.VAR_VEHICLECLASS, vehID)

def getSpeedFactor(vehID):
    return _getUniversal(tc.VAR_SPEED_FACTOR, vehID)

def getSpeedDeviation(vehID):
    return _getUniversal(tc.VAR_SPEED_DEVIATION, vehID)

def getEmissionClass(vehID):
    return _getUniversal(tc.VAR_EMISSIONCLASS, vehID)

def getWidth(vehID):
    return _getUniversal(tc.VAR_WIDTH, vehID)

def getGUIOffset(vehID):
    return _getUniversal(tc.VAR_GUIOFFSET, vehID)

def getShapeClass(vehID):
    return _getUniversal(tc.VAR_SHAPECLASS, vehID)

def getAccel(vehID):
    return _getUniversal(tc.VAR_ACCEL, vehID)

def getDecel(vehID):
    return _getUniversal(tc.VAR_DECEL, vehID)

def getImperfection(vehID):
    return _getUniversal(tc.VAR_IMPERFECTION, vehID)

def getTau(vehID):
    return _getUniversal(tc.VAR_TAU, vehID)

def getBestLanes(vehID):
    return _getUniversal(tc.VAR_BEST_LANES, vehID)

def getDrivingDistance(vehID, edgeID, pos, laneID=0):
    traci._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST, vehID, 1+4+1+4+len(edgeID)+4+1+1)
    traci._message.string += struct.pack("!BiBi", tc.TYPE_COMPOUND, 2,
                                         tc.POSITION_ROADMAP, len(edgeID)) + edgeID
    traci._message.string += struct.pack("!fBB", pos, laneID, REQUEST_DRIVINGDIST)
    return traci._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST, vehID).readFloat()

def getDrivingDistance2D(vehID, x, y):
    traci._beginMessage(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST, vehID, 1+4+1+4+4+1)
    traci._message.string += struct.pack("!BiBffB", tc.TYPE_COMPOUND, 2,
                                         tc.POSITION_2D, x, y, REQUEST_DRIVINGDIST)
    return traci._checkResult(tc.CMD_GET_VEHICLE_VARIABLE, tc.DISTANCE_REQUEST, vehID).readFloat()


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
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, speed)
    traci._sendExact()

def setStop(vehID, vehID, pos=1., laneIndex=0, duration=2**31-1):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_STOP, vehID, 1+4+1+4+len(vehID)+1+4+1+1+1+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
    traci._message.string += struct.pack("!B", tc.TYPE_STRING)
    traci._message.string += struct.pack("!i", len(vehID)) + vehID
    traci._message.string += struct.pack("!BfBBBi", tc.TYPE_FLOAT, pos, tc.TYPE_BYTE, laneIndex, tc.TYPE_INTEGER, duration)
    traci._sendExact()

def changeLane(vehID, laneIndex, duration):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGELANE, vehID, 1+4+1+1+1+4)
    traci._message.string += struct.pack("!BiBBBi", tc.TYPE_COMPOUND, 2, tc.TYPE_BYTE, laneIndex, tc.TYPE_INTEGER, duration)
    traci._sendExact()

def slowDown(vehID, speed, duration):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_SLOWDOWN, vehID, 1+4+1+4+1+4)
    traci._message.string += struct.pack("!BiBfBi", tc.TYPE_COMPOUND, 2, tc.TYPE_FLOAT, speed, tc.TYPE_INTEGER, duration)
    traci._sendExact()

def changeTarget(vehID, edgeID):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_CHANGETARGET, vehID, 1+4+len(edgeID))
    traci._message.string += struct.pack("!B", tc.TYPE_STRING)
    traci._message.string += struct.pack("!i", len(edgeID)) + edgeID
    traci._sendExact()

def setRouteID(vehID, routeID):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_ROUTE_ID, vehID, 1+4+len(routeID))
    traci._message.string += struct.pack("!B", tc.TYPE_STRING)
    traci._message.string += struct.pack("!i", len(routeID)) + routeID
    traci._sendExact()

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
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(edgeList))
    for edge in edgeList:
        traci._message.string += struct.pack("!i", len(edge)) + edge
    traci._sendExact()

def setAdaptedTraveltime(vehID, begTime, endTime, edgeID, time):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_TRAVELTIME, vehID, 1+4+1+4+1+4+1+4+len(edgeID)+1+4)
    traci._message.string += struct.pack("!BiBiBiBi", tc.TYPE_COMPOUND, 4, tc.TYPE_INTEGER, begTime,
                                         tc.TYPE_INTEGER, endTime, tc.TYPE_STRING, len(edgeID)) + edgeID
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, time)
    traci._sendExact()

def setEffort(vehID, begTime, endTime, edgeID, effort):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_EDGE_EFFORT, vehID, 1+4+1+4+1+4+1+4+len(edgeID)+1+4)
    traci._message.string += struct.pack("!BiBiBiBi", tc.TYPE_COMPOUND, 4, tc.TYPE_INTEGER, begTime,
                                         tc.TYPE_INTEGER, endTime, tc.TYPE_STRING, len(edgeID)) + edgeID
    traci._message.string += struct.pack("!Bf", tc.TYPE_FLOAT, effort)
    traci._sendExact()

def moveTo(vehID, laneID, pos):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_MOVE_TO, vehID, 1+4+1+4+len(laneID)+8)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 2)
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(laneID)) + laneID
    traci._message.string += struct.pack("!Bd", tc.TYPE_DOUBLE, pos)
    traci._sendExact()

def reroute(vehID):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.CMD_REROUTE_TRAVELTIME, vehID, 1+4)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0)
    traci._sendExact()

def setSpeed(vehID, speed):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_SPEED, vehID, 1+8)
    traci._message.string += struct.pack("!Bd", tc.TYPE_DOUBLE, speed)
    traci._sendExact()

def setLanePosition(vehID, position):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_LANEPOSITION, vehID, 1+8)
    traci._message.string += struct.pack("!Bd", tc.TYPE_DOUBLE, position)
    traci._sendExact()

def setColor(vehID, color):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.VAR_COLOR, vehID, 1+1+1+1+1)
    traci._message.string += struct.pack("!BBBBB", tc.TYPE_COLOR, int(color[0]), int(color[1]), int(color[2]), int(color[3]))
    traci._sendExact()

def add(vehID, routeID, depart=-1, pos=0, speed=0, lane=0, typeID="DEFAULT_VEHTYPE"):
    traci._beginMessage(tc.CMD_SET_VEHICLE_VARIABLE, tc.ADD, vehID,
                        1+4+len(typeID) + 1+4+len(routeID) + 1+4, 1+4 + 1+4 + 1+1)
    traci._message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 6)
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(typeID)) + typeID
    traci._message.string += struct.pack("!Bi", tc.TYPE_STRING, len(routeID)) + routeID
    traci._message.string += struct.pack("!Bi", tc.TYPE_INTEGER, depart * 1000)
    traci._message.string += struct.pack("!BfBf", tc.TYPE_FLOAT, pos, tc.TYPE_FLOAT, speed)
    traci._message.string += struct.pack("!BB", tc.TYPE_UBYTE, lane)
    traci._sendExact()
