#!/usr/bin/env python
"""
@file    traciControl.py
@author  Michael.Behrisch@dlr.de, Lena Kalleske
@date    2008-10-09
@version $Id$

Python implementation of the TraCI interface.

Copyright (C) 2008, 2009 DLR/TS, Germany
All rights reserved
"""
import socket, time, struct
import traciconstants as tc

RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}

class FatalTraCIError:
    def __init__(self, desc):
        self._desc = desc

class Phase:
    def __init__(self, duration, duration1, duration2, phaseDef):
        self._duration = duration
        self._duration1 = duration1
        self._duration2 = duration2
        self._phaseDef = phaseDef
        
    def write(self):
        print("Phase:")
        print("duration: ", self._duration)
        print("duration1: ", self._duration1)
        print("duration2: ", self._duration2)
        print("phaseDef: ", self._phaseDef)
        
class Logic:
    def __init__(self, subID, type, subParameter, currentPhaseIndex, phases):
        self._subID = subID
        self._type = type
        self._subParameter = subParameter
        self._currentPhaseIndex = currentPhaseIndex
        self._phases = phases
        
    def write(self):
        print("Logic:")
        print("subID:", self._subID)
        print("type:", self._type)
        print("subParameter:", self._subParameter)
        print("currentPhaseIndex:", self._currentPhaseIndex)
        for i in range(len(self._phases)):
            self._phases[i].write()

class Message:
    string = ""
    queue = []



_socket = socket.socket()
_message = Message()

class Storage:
    def __init__(self, content):
        self._content = content
        self._pos = 0

    def read(self, format):
        oldPos = self._pos
        self._pos += struct.calcsize(format)
        return struct.unpack(format, self._content[oldPos:self._pos])

    def readString(self):
        length = self.read("!i")[0]
        return self.read("!%ss" % length)[0]
    
    def readStringList(self):
        n = self.read("!i")[0]
        list = []
        for i in range(n):
            list.append(self.readString())
        return list     
    
    def ready(self):
        return self._pos < len(self._content) 


def _recvExact():
    global _socket
    try:
        result = ""
        while len(result) < 4:
            t = _socket.recv(4 - len(result))
            if not t:
                return None
            result += t
        length = struct.unpack("!i", result)[0] - 4
        result = ""
        while len(result) < length:
            t = _socket.recv(length - len(result))
            if not t:
                return None
            result += t
        return Storage(result)
    except socket.error:
        return None

def _sendExact():
    global _socket
    length = struct.pack("!i", len(_message.string)+4)
    _socket.send(length)
    _socket.send(_message.string)
    _message.string = ""
    result = _recvExact()
    if not result:
        _socket.close()
        _socket = None
        raise FatalTraCIError("connection closed by SUMO")
    for command in _message.queue:
        prefix = result.read("!BBB")
        err = result.readString()
        if prefix[2] or err:
            print prefix, RESULTS[prefix[2]], err
        elif prefix[1] != command:
            print "Error! Received answer %s for command %s." % (prefix[1], command)
        elif prefix[1] == tc.CMD_STOP:
            length = result.read("!B")[0] - 1
            result.read("!%sx" % length)
    _message.queue = []
    return result

def readHead(result):
    result.read("!B")
    result.read("!B")
    result.read("!B")
    result.read("!B")
    result.read("!B")     # Length
    result.read("!B")     # Identifier
    result.read("!B")     # Variable
    result.readString()   # Induction Loop ID // Multi-Entry/Multi-Exit Detector ID // Traffic Light ID
    result.read("!B")     # Return type of the variable
    

def buildSendReadNew1StringParamCmd(domainID, cmdID, objID):
    _message.queue.append(domainID)
    length = 1+1+1+4+len(objID)
    if length<=255:
        _message.string += struct.pack("!BBBi", length, domainID, cmdID, len(objID)) + objID
    else:
        _message.string += struct.pack("!BiBBi", 0, length+4, domainID, cmdID, len(objID)) + objID
    result = _sendExact()
    readHead(result)
    return result

def beginChangeMessage(domainID, length, cmdID, objID):
    _message.queue.append(domainID)
    if length<=255:
        _message.string += struct.pack("!BBBi", length, domainID, cmdID, len(objID)) + objID
    else:
        _message.string += struct.pack("!BiBBi", 0, length+4, domainID, cmdID, len(objID)) + objID

def initTraCI(port):
    global _socket
    _socket = socket.socket()
    for wait in range(10):
        try:
            _socket.connect(("localhost", port))
            _socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            break
        except socket.error:
            time.sleep(wait)

            
def cmdSimulationStep(step, position=True):
    """
    Make simulation step and simulate up to "step" second in sim time.
    If *position* is True, ten roadmap position coordinates (vehicle number,
    edge number, distance from start) will be returned.
    If *position* is False - step is made and only empty list is returned.
    """
    if position:
        return_type = tc.POSITION_ROADMAP
    else:
        return_type = tc.POSITION_NONE
    
    _message.queue.append(tc.CMD_SIMSTEP)
    _message.string += struct.pack("!BBdB", 1+1+8+1, tc.CMD_SIMSTEP, float(step), return_type)
    result = _sendExact()
    updates = []
    while result.ready():
        if result.read("!BB")[1] == tc.CMD_MOVENODE: 
            updates.append((result.read("!idB")[0], result.readString(), result.read("!fB")[0]))
    return updates



# ===================================================
# induction loop interaction
# ===================================================
def cmdGetInductionLoopVariable_idList(IndLoopID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, tc.ID_LIST, IndLoopID)
    return result.readStringList() # Variable value 

def cmdGetInductionLoopVariable_lastStepVehicleNumber(IndLoopID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, tc.LAST_STEP_VEHICLE_NUMBER, IndLoopID)
    return result.read("!i")[0] # Variable value

def cmdGetInductionLoopVariable_lastStepMeanSpeed(IndLoopID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, tc.LAST_STEP_MEAN_SPEED, IndLoopID)
    return result.read("!f")[0] # Variable value    

def cmdGetInductionLoopVariable_vehicleIds(IndLoopID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, tc.LAST_STEP_VEHICLE_ID_LIST, IndLoopID)
    return result.readStringList() # Variable value
  
def cmdGetInductionLoopVariable_lastStepOccupancy(IndLoopID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, tc.LAST_STEP_OCCUPANCY, IndLoopID)
    return result.read("!f")[0] # Variable value
  
def cmdGetInductionLoopVariable_lastMeanLength(IndLoopID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, tc.LAST_STEP_LENGTH, IndLoopID)
    return result.read("!f")[0] # Variable value
  
def cmdGetInductionLoopVariable_timeSinceDetection(IndLoopID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_INDUCTIONLOOP_VARIABLE, tc.LAST_STEP_TIME_SINCE_DETECTION, IndLoopID)
    return result.read("!f")[0] # Variable value
  
  

# ===================================================
# multi-entry/multi-exit detector interaction
# ===================================================
def cmdGetMultiEntryExitDetectorVariable_idList(MultiEntryExitDetID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, tc.ID_LIST, MultiEntryExitDetID)
    return result.readStringList() # Variable value 

def cmdGetMultiEntryExitDetectorVariable_lastStepVehicleNumber(MultiEntryExitDetID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, tc.LAST_STEP_VEHICLE_NUMBER, MultiEntryExitDetID)
    return result.read("!i")[0] # Variable value

def cmdGetMultiEntryExitDetectorVariable_lastStepMeanSpeed(MultiEntryExitDetID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, tc.LAST_STEP_MEAN_SPEED, MultiEntryExitDetID)
    return result.read("!f")[0] # Variable value    

def cmdGetMultiEntryExitDetectorVariable_vehicleIds(MultiEntryExitDetID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, tc.LAST_STEP_VEHICLE_ID_LIST, MultiEntryExitDetID)
    return result.readStringList() # Variable value

def cmdGetMultiEntryExitDetectorVariable_haltingNumber(MultiEntryExitDetID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, tc.LAST_STEP_VEHICLE_HALTING_NUMBER, MultiEntryExitDetID)
    return result.read("!i")[0] # Variable value



# ===================================================
# traffic lights interaction
# ===================================================
# ---------------------------------------------------
# get state
# ---------------------------------------------------
def cmdGetTrafficLightsVariable_idList():
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.ID_LIST, "-")
    return result.readStringList()  # Variable value 

def cmdGetTrafficLightsVariable_stateRYG(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_RED_YELLOW_GREEN_STATE, TLID)
    return result.readString() # Variable value 

def cmdGetTrafficLightsVariable_statePBY(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_PHASE_BRAKE_YELLOW_STATE, TLID)
    return result.readStringList() # Variable value     

def cmdGetTrafficLightsVariable_completeDefinitionRYG(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_COMPLETE_DEFINITION_RYG, TLID)
    result.read("!i") # Length
    result.read("!B") # Type of Number of logics
    nbLogics = result.read("!i")[0]    # Number of logics
    logics = []
    for i in range(nbLogics):
        result.read("!B")                       # Type of SubID
        subID = result.readString()
        result.read("!B")                       # Type of Type
        type = result.read("!i")[0]             # Type
        result.read("!B")                       # Type of SubParameter
        subParameter = result.read("!i")[0]     # SubParameter
        result.read("!B")                       # Type of Current phase index
        currentPhaseIndex = result.read("!i")[0]    # Current phase index
        result.read("!B")                       # Type of Number of phases
        nbPhases = result.read("!i")[0]         # Number of phases
        phases = []
        for j in range(nbPhases):
            result.read("!B")                   # Type of Duration
            duration = result.read("!i")[0]     # Duration
            result.read("!B")                   # Type of Duration1
            duration1 = result.read("!i")[0]    # Duration1
            result.read("!B")                   # Type of Duration2
            duration2 = result.read("!i")[0]    # Duration2
            result.read("!B")                   # Type of Phase Definition
            phaseDef = result.readString()      # Phase Definition
            phase = Phase(duration, duration1, duration2, phaseDef)
            phases.append(phase)
        logic = Logic(subID, type, subParameter, currentPhaseIndex, phases)
        logics.append(logic)
    return logics


def cmdGetTrafficLightsVariable_completeDefinitionPBY(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_COMPLETE_DEFINITION_PBY, TLID)
    result.read("!i") # Length
    result.read("!B") # Type of Number of logics
    nbLogics = result.read("!i")[0]    # Number of logics
    logics = []
    for i in range(nbLogics):
        result.read("!B")                       # Type of SubID
        subID = result.readString()
        result.read("!B")                       # Type of Type
        type = result.read("!i")[0]             # Type
        result.read("!B")                       # Type of SubParameter
        subParameter = result.read("!i")[0]     # SubParameter
        result.read("!B")                       # Type of Current phase index
        currentPhaseIndex = result.read("!i")[0]    # Current phase index
        result.read("!B")                       # Type of Number of phases
        nbPhases = result.read("!i")[0]         # Number of phases
        phases = []
        for j in range(nbPhases):
            result.read("!B")                   # Type of Duration
            duration = result.read("!i")[0]     # Duration
            result.read("!B")                   # Type of Duration1
            duration1 = result.read("!i")[0]    # Duration1
            result.read("!B")                   # Type of Duration2
            duration2 = result.read("!i")[0]    # Duration2
            result.read("!B")                   # Type of Phase Definition
            phaseDef = result.readStringList()  # Phase Definition
            phase = Phase(duration, duration1, duration2, phaseDef)
            phases.append(phase)
        logic = Logic(subID, type, subParameter, currentPhaseIndex, phases)
        logics.append(logic)
    return logics


def cmdGetTrafficLightsVariable_controlledLanes(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_CONTROLLED_LANES, TLID)
    return result.readStringList() # Variable value 

def cmdGetTrafficLightsVariable_controlledLinks(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_CONTROLLED_LINKS, TLID)
    result.read("!iB")
    nbSignals = result.read("!i")[0] # Length
    signals = []
    for i in range(nbSignals):
        result.read("!B")                           # Type of Number of Controlled Links
        nbControlledLinks = result.read("!i")[0]    # Number of Controlled Links
        controlledLinks = []
        for j in range(nbControlledLinks):
            result.read("!B")                       # Type of Link j
            link = result.readStringList()          # Link j
            controlledLinks.append(link)
        signals.append(controlledLinks)
    return signals


def cmdGetTrafficLightsVariable_program(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_CURRENT_PROGRAM, TLID)
    return result.readString() # Variable value 

def cmdGetTrafficLightsVariable_phase(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_CURRENT_PHASE, TLID)
    return result.read("!i")[0] # Variable value

def cmdGetTrafficLightsVariable_nextSwitchTime(TLID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_TL_VARIABLE, tc.TL_NEXT_SWITCH, TLID)
    return result.read("!f")[0] # Variable value


# ---------------------------------------------------
# change state
# ---------------------------------------------------
def cmdChangeTrafficLightsVariable_statePBY(TLID, state):
    [phase, brake, yellow] = state
    beginChangeMessage(tc.CMD_SET_TL_VARIABLE, 1+1+1+4+len(TLID)+1+4+4+len(phase)+4+len(brake)+4+len(yellow), tc.TL_PHASE_BRAKE_YELLOW_STATE, TLID)
    _message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, 3)
    _message.string += struct.pack("!i", len(phase)) + phase
    _message.string += struct.pack("!i", len(brake)) + brake
    _message.string += struct.pack("!i", len(yellow)) + yellow
    _sendExact()
    
def cmdChangeTrafficLightsVariable_stateRYG(TLID, state):
    beginChangeMessage(tc.CMD_SET_TL_VARIABLE, 1+1+1+4+len(TLID)+1+4+len(state), tc.TL_RED_YELLOW_GREEN_STATE, TLID)
    _message.string += struct.pack("!B", tc.TYPE_STRING)
    _message.string += struct.pack("!i", len(state)) + state
    _sendExact()

def cmdChangeTrafficLightsVariable_phaseIndex(TLID, index):
    beginChangeMessage(tc.CMD_SET_TL_VARIABLE, 1+1+1+4+len(TLID)+1+4, tc.TL_PHASE_INDEX, TLID)
    _message.string += struct.pack("!Bi", tc.TYPE_INTEGER, index)
    _sendExact()

def cmdChangeTrafficLightsVariable_programID(TLID, programID):
    beginChangeMessage(tc.CMD_SET_TL_VARIABLE, 1+1+1+4+len(TLID)+1+4+len(programID), tc.TL_PROGRAM, TLID)
    _message.string += struct.pack("!Bi", tc.TYPE_STRING, len(programID)) + programID
    _sendExact()

def cmdChangeTrafficLightsVariable_phaseDuration(TLID, phaseDuration):
    beginChangeMessage(tc.CMD_SET_TL_VARIABLE, 1+1+1+4+len(TLID)+1+4, tc.TL_PHASE_DURATION, TLID)
    _message.string += struct.pack("!Bi", tc.TYPE_INTEGER, phaseDuration)
    _sendExact()

def cmdChangeTrafficLightsVariable_completeRYG(TLID, tls):
    length = 1+1+1+4+len(TLID) # basic
    itemNo = 0
    length = length + 1+4 + 1+4+len(tls._subID) + 1+4 + 1+4 + 1+4 + 1+4 # tls parameter
    itemNo = 1+1+1+1+1
    for p in tls._phases:
        length = length + 1+4 + 1+4 + 1+4 + 1+4+len(p._phaseDef)
        itemNo = itemNo + 4
    beginChangeMessage(tc.CMD_SET_TL_VARIABLE, length, tc.TL_COMPLETE_PROGRAM_RYG, TLID)
    _message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, itemNo) # itemNo
    _message.string += struct.pack("!Bi", tc.TYPE_STRING, len(tls._subID)) + tls._subID # programID
    _message.string += struct.pack("!Bi", tc.TYPE_INTEGER, 0) # type
    _message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 0) # subitems
    _message.string += struct.pack("!Bi", tc.TYPE_INTEGER, tls._currentPhaseIndex) # index
    _message.string += struct.pack("!Bi", tc.TYPE_INTEGER, len(tls._phases)) # phaseNo
    for p in tls._phases:
        _message.string += struct.pack("!BiBiBi", tc.TYPE_INTEGER, p._duration, tc.TYPE_INTEGER, p._duration1, tc.TYPE_INTEGER, p._duration2)
        _message.string += struct.pack("!Bi", tc.TYPE_STRING, len(p._phaseDef)) + p._phaseDef
    _sendExact()
    
    


# ===================================================
# vehicle interaction
# ===================================================
# ---------------------------------------------------
# get state
# ---------------------------------------------------
def cmdGetVehicleVariable_idList(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.ID_LIST, vehID)
    return result.readStringList()  # Variable value 

def cmdGetVehicleVariable_speed(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_SPEED, vehID)
    return result.read("!f")[0] # Variable value

def cmdGetVehicleVariable_position(vehID):
    """
    Returns the position of the named vehicle within the last step [m,m]
    """
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_POSITION, vehID)
    return result.read("!ff")

def cmdGetVehicleVariable_angle(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_ANGLE, vehID)
    return result.read("!f")[0] # Variable value

def cmdGetVehicleVariable_roadID(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_ROAD_ID, vehID)
    return result.readString() # Variable value

def cmdGetVehicleVariable_laneID(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_LANE_ID, vehID)
    return result.readString() # Variable value

def cmdGetVehicleVariable_laneIndex(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_LANE_INDEX, vehID)
    return result.read("!i")[0] # Variable value

def cmdGetVehicleVariable_typeID(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_TYPE, vehID)
    return result.readString() # Variable value

def cmdGetVehicleVariable_routeID(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_ROUTE_ID, vehID)
    return result.readString() # Variable value

def cmdGetVehicleVariable_lanePosition(vehID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLE_VARIABLE, tc.VAR_LANEPOSITION, vehID)
    return result.read("!f")[0] # Variable value


# ---------------------------------------------------
# change state
# ---------------------------------------------------
def cmdChangeVehicleVariable_maxSpeed(vehID, speed):
    beginChangeMessage(tc.CMD_SET_VEHICLE_VARIABLE, 1+1+1+4+len(vehID)+1+4, tc.CMD_SETMAXSPEED, vehID)
    _message.string += struct.pack("!Bf", tc.TYPE_FLOAT, speed)
    _sendExact()

def cmdChangeVehicleVariable_stop(vehID, edgeID, pos, laneIndex, duration):
    beginChangeMessage(tc.CMD_SET_VEHICLE_VARIABLE, 1+1+1+4+len(vehID)+1+4+1+4+len(edgeID)+1+4+1+1+1+4, tc.CMD_STOP, vehID)
    _message.string += struct.pack("!Bi", tc.TYPE_COMPOUND, 4)
    _message.string += struct.pack("!B", tc.TYPE_STRING)
    _message.string += struct.pack("!i", len(edgeID)) + edgeID
    _message.string += struct.pack("!BfBBBf", tc.TYPE_FLOAT, pos, tc.TYPE_BYTE, laneIndex, tc.TYPE_FLOAT, duration)
    _sendExact()

def cmdChangeVehicleVariable_changeLane(vehID, laneIndex, duration):
    beginChangeMessage(tc.CMD_SET_VEHICLE_VARIABLE, 1+1+1+4+len(vehID)+1+4+1+1+1+4, tc.CMD_CHANGELANE, vehID)
    _message.string += struct.pack("!BiBBBf", tc.TYPE_COMPOUND, 2, tc.TYPE_BYTE, laneIndex, tc.TYPE_FLOAT, duration)
    _sendExact()

def cmdChangeVehicleVariable_slowDown(vehID, speed, duration):
    beginChangeMessage(tc.CMD_SET_VEHICLE_VARIABLE, 1+1+1+4+len(vehID)+1+4+1+4+1+4, tc.CMD_SLOWDOWN, vehID)
    _message.string += struct.pack("!BiBfBf", tc.TYPE_COMPOUND, 2, tc.TYPE_FLOAT, speed, tc.TYPE_FLOAT, duration)
    _sendExact()

def cmdChangeVehicleVariable_changeTarget(vehID, edgeID):
    beginChangeMessage(tc.CMD_SET_VEHICLE_VARIABLE, 1+1+1+4+len(vehID)+1+4+len(edgeID), tc.CMD_CHANGETARGET, vehID)
    _message.string += struct.pack("!B", tc.TYPE_STRING)
    _message.string += struct.pack("!i", len(edgeID)) + edgeID
    _sendExact()

def cmdChangeVehicleVariable_changeRoute(vehID, edgeList):
    """
    changes the vehicle route to given edges list.
    The first edge in the list has to be the one that the vehicle is at at the moment.
    
    example usasge:
    cmdChangeVehicleVariable_changeRoute('1', ['1', '2', '4', '6', '7'])
    
    this changes route for vehicle id 1 to edges 1-2-4-6-7
    """
    beginChangeMessage(tc.CMD_SET_VEHICLE_VARIABLE, 1+1+1+4+len(vehID)+1+4+sum(map(len, edgeList))+4*len(edgeList), tc.VAR_ROUTE, vehID)
    _message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(edgeList))
    for edge in edgeList:
        _message.string += struct.pack("!i", len(edge)) + edge
    _sendExact()


# ===================================================
# vehicle type interaction
# ===================================================
# ---------------------------------------------------
# get state
# ---------------------------------------------------
def cmdGetVehicleTypeVariable_idList(vehTypeID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_VEHICLETYPE_VARIABLE, tc.ID_LIST, vehTypeID)
    return result.readStringList()  # Variable value 



# ===================================================
# route interaction
# ===================================================
# ---------------------------------------------------
# get state
# ---------------------------------------------------
def cmdGetRouteVariable_idList(routeID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_ROUTE_VARIABLE, tc.ID_LIST, routeID)
    return result.readStringList() # Variable value 

def cmdGetRouteVariable_edges(routeID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_ROUTE_VARIABLE, tc.VAR_EDGES, routeID)
    return result.readStringList() # Variable value 



# ===================================================
# nodes (junction/intersection) interaction
# ===================================================
# ---------------------------------------------------
# get state
# ---------------------------------------------------
def cmdGetJunctionVariable_idList(nodeID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_JUNCTION_VARIABLE, tc.ID_LIST, nodeID)
    return result.readStringList()  # Variable value 

def cmdGetJunctionVariable_position(nodeID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_JUNCTION_VARIABLE, tc.VAR_POSITION, nodeID)
    return result.read("!ff") # Variable value



# ===================================================
# lanes interaction
# ===================================================
# ---------------------------------------------------
# get state
# ---------------------------------------------------
def cmdGetLaneVariable_idList(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.ID_LIST, laneID)
    return result.readStringList()  # Variable value 

def cmdGetLaneVariable_length(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_LENGTH, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_speed(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_MAXSPEED, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_allowed(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.LANE_ALLOWED, laneID)
    return result.readStringList()  # Variable value 

def cmdGetLaneVariable_disallowed(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.LANE_DISALLOWED, laneID)
    return result.readStringList()  # Variable value 

def cmdGetLaneVariable_linkNumber(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.LANE_LINK_NUMBER, laneID)
    return result.read("!i")[0] # Variable value

def cmdGetLaneVariable_edgeID(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.LANE_EDGE_ID, laneID)
    return result.readString() # Variable value

def cmdGetLaneVariable_speed(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_MAXSPEED, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_CO2emission(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_CO2EMISSION, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_COemission(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_COEMISSION, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_HCemission(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_HCEMISSION, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_PMxemission(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_PMXEMISSION, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_NOxemission(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_NOXEMISSION, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_fuelConsumption(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_FUELCONSUMPTION, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_noiseEmission(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.VAR_NOISEEMISSION, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_meanSpeed(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.LAST_STEP_MEAN_SPEED, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_occupancy(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.LAST_STEP_OCCUPANCY, laneID)
    return result.read("!f")[0] # Variable value

def cmdGetLaneVariable_vehicleIDs(laneID):
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_LANE_VARIABLE, tc.LAST_STEP_VEHICLE_ID_LIST, laneID)
    return result.readStringList()  # Variable value 


# ---------------------------------------------------
# change state
# ---------------------------------------------------
def cmdChangeLaneVariable_allowed(laneID, allowedClasses):
    beginChangeMessage(tc.CMD_SET_LANE_VARIABLE, 1+1+1+4+len(laneID)+1+4+sum(map(len, allowedClasses))+4*len(allowedClasses), tc.LANE_ALLOWED, laneID)
    _message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(allowedClasses))
    for c in allowedClasses:
        _message.string += struct.pack("!i", len(c)) + c
    _sendExact()

def cmdChangeLaneVariable_disallowed(laneID, disallowedClasses):
    beginChangeMessage(tc.CMD_SET_LANE_VARIABLE, 1+1+1+4+len(laneID)+1+4+sum(map(len, disallowedClasses))+4*len(disallowedClasses), tc.LANE_DISALLOWED, laneID)
    _message.string += struct.pack("!Bi", tc.TYPE_STRINGLIST, len(disallowedClasses))
    for c in disallowedClasses:
        _message.string += struct.pack("!i", len(c)) + c
    _sendExact()

def cmdChangeLaneVariable_maxSpeed(laneID, speed):
    beginChangeMessage(tc.CMD_SET_LANE_VARIABLE, 1+1+1+4+len(laneID)+1+4, tc.VAR_MAXSPEED, laneID)
    _message.string += struct.pack("!Bf", tc.TYPE_FLOAT, speed)
    _sendExact()

def cmdChangeLaneVariable_length(laneID, length):
    beginChangeMessage(tc.CMD_SET_LANE_VARIABLE, 1+1+1+4+len(laneID)+1+4, tc.VAR_LENGTH, laneID)
    _message.string += struct.pack("!Bf", tc.TYPE_FLOAT, length)
    _sendExact()



# ===================================================
# simulation interaction
# ===================================================
# ---------------------------------------------------
# get state
# ---------------------------------------------------
def cmdGetSimulationVariable_currentTime():
    result = buildSendReadNew1StringParamCmd(tc.CMD_GET_SIM_VARIABLE, tc.VAR_TIME_STEP, "x")
    return result.read("!d")[0] # Variable value







# ===================================================
# 
# ===================================================
def cmdStopNode(edge, objectID, pos=1., duration=10000.):
    _message.queue.append(tc.CMD_STOP)
    _message.string += struct.pack("!BBiBi", 1+1+4+1+4+len(edge)+4+1+4+8, tc.CMD_STOP, objectID, tc.POSITION_ROADMAP, len(edge)) + edge
    _message.string += struct.pack("!fBfd", pos, 0, 1., duration)

def cmdChangeTarget(edge, objectID):
    _message.queue.append(tc.CMD_CHANGETARGET)
    _message.string += struct.pack("!BBii", 1+1+4+4+len(edge), tc.CMD_CHANGETARGET, objectID, len(edge)) + edge

def cmdClose():
    global _socket
    if _socket:
        _message.queue.append(tc.CMD_CLOSE)
        _message.string += struct.pack("!BB", 1+1, tc.CMD_CLOSE)
        _sendExact()
        _socket.close()

