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

import traci
import traci.constants as tc

from simpla._platoon import Platoon
import simpla._config as cfg
from simpla._reporting import Warner, Reporter
from simpla._platoonmode import PlatoonMode

warn = Warner("PVehicle")
report = Reporter("PVehicle")
    
# lookup table for vType parameters
vTypeParameters = defaultdict(dict)

class pVehicleState(object):
    def __init__(self, ID):
        self.speed  = traci.vehicle.getSpeed(ID)
        self.edgeID = traci.vehicle.getRoadID(ID)
        self.laneID = traci.vehicle.getLaneID(ID)
        self.laneIX = traci.vehicle.getLaneIndex(ID)
        self.leaderInfo = traci.vehicle.getLeader(ID, 50.)
        self.leader = None # must be set by vehicle creator (PlatoonManager._addVehicle()) to guarantee function in first step

class PVehicle(object):
    '''
    Vehicle objects for platooning    
    '''
    def __init__(self, ID):
        '''Constructor(string ID)
        
        Create a PVehicle representing a SUMOVehicle for the PlatoonManager
        '''        
        # vehicle ID (should be the one used in SUMO)
        self._ID = ID
        # vehicle state (is updated by platoon manager in every step)
        self.state = pVehicleState(ID)
        
        # store the vehicle's vTypes, speedfactors and lanechangemodes
        self._vTypes = dict()
        self._speedFactors = dict()
        self._laneChangeModes = dict()
        # original vtype, speedFactor and lanechangemodes
        self._vTypes[PlatoonMode.NONE] = traci.vehicle.getTypeID(ID)
        self._speedFactors[PlatoonMode.NONE] = traci.vehicle.getSpeedFactor(ID)
        # This is the default mode
        self._laneChangeModes[PlatoonMode.NONE] = 0b1001010101
        
        # vTypes, speedFactors and lanechangemodes parametrizing the platoon behaviour
        for mode in [PlatoonMode.LEADER, PlatoonMode.FOLLOWER, PlatoonMode.CATCHUP]:
            self._vTypes[mode] = self._determinePlatoonVType(mode)
            self._laneChangeModes[mode] = cfg.LC_MODE[mode]
            self._speedFactors[mode] = cfg.SPEEDFACTOR[mode]

        # Initialize platoon mode to none
        self._currentPlatoonMode = PlatoonMode.NONE
        # create a new platoon containing only this vehicle
        self._platoon = Platoon([self])
        # the time left until splitting from a platoon if loosing coherence as a follower
        self._timeUntilSplit = cfg.PLATOON_SPLIT_TIME
        # Whether split conditions are fulfilled (i.e. leader in th platoon 
        # is not found directly in front of the vehicle)
        self._splitConditions = False

        
    def _determinePlatoonVType(self, mode):
        '''_determinePlatoonVType(PlatoonMode) -> string
        
        Returns the type ID corresponding to the given mode. Uses the vehicles vType and the global map PLATOON_VTYPES
        between original and platoon-vTypes. If the original vType is not mapped to any platoon-vtypes, 
        the original vType is used for platooning as well
        ''' 
        # original vType
        origVType = self._vTypes[PlatoonMode.NONE]
        if not cfg.PLATOON_VTYPES.has_key(origVType) \
        or not cfg.PLATOON_VTYPES[origVType].has_key(mode) \
        or cfg.PLATOON_VTYPES[origVType][mode] is "":
            if cfg.PLATOON_VTYPES.has_key("default") and cfg.PLATOON_VTYPES["default"].has_key(mode):
                if cfg.VERBOSITY >= 2: 
                    warn("Using default vType '%s' for vehicle '%s' (PlatoonMode: '%s')."%(cfg.PLATOON_VTYPES["default"][mode], self._ID, PlatoonMode(mode).name))
                return cfg.PLATOON_VTYPES["default"][mode]
            else:
                warn("No vType specified for PlatoonMode '%s' for vehicle '%s'. Behavior within platoon is NOT altered."%(PlatoonMode(mode).name, self._ID))
                return origVType
        if cfg.VERBOSITY >= 3: 
            report("Using vType '%s' for vehicle '%s' (PlatoonMode: '%s')."%(cfg.PLATOON_VTYPES[origVType][mode], self._ID, PlatoonMode(mode).name))
        return cfg.PLATOON_VTYPES[origVType][mode]
        
        
        
    def getID(self):
        '''getID() -> string
        
        Returns the vehicle's ID corresponding to the ID of the associated SUMOVehicle.
        '''
        return self._ID
                
    def getVType(self, mode):
        '''getVType(PlatoonMode) -> string
        
        Returns the vehicle type associated with the given platooning mode
        '''
        return self._vTypes[mode]
                
    def getCurrentVType(self):
        '''getCurrentVType() -> string
        
        Returns the vehicle type associated with the currently active platooning mode
        '''
        return self._vTypes[self._currentPlatoonMode]
        
    def setPlatoon(self, platoon):
        '''setPlatoon(_platoon)
        
        Sets the vehicle's platoon to the given.
        '''
        self._platoon = platoon
    
    def getPlatoon(self):
        '''getPlatoon() -> Platoon
        
        Returns the vehicle's platoon.
        '''
        return self._platoon
    
    
    def isLastInPlatoon(self):
        '''isLastInPlatoon() -> bool
        
        Returns whether the ego is the last vehicle in its platoon
        '''
        return self._platoon.getVehicles()[-1] == self
            
    def setPlatoonMode(self, mode):
        '''setPlatoonMode(PlatoonMode)
        
        Assign this vehicle the vType corresponding to the given 'mode'
        'mode' is from Globals.PlatoonMode Enum. (safety checks have to be done at caller site)
        '''
        if self._currentPlatoonMode == mode:
            # do nothing mode is already chosen
            return
        
        if cfg.VERBOSITY >= 3: 
            report("Vehicle '%s': Setting PlatoonMode '%s'"%(self._ID, PlatoonMode(mode).name))
                
        if self._vTypes[mode] != self._vTypes[self._currentPlatoonMode]:
            traci.vehicle.setType(self._ID, self._vTypes[mode])
        if self._speedFactors[mode] != self._speedFactors[self._currentPlatoonMode]:
            traci.vehicle.setSpeedFactor(self._ID, self._speedFactors[mode])
        if self._laneChangeModes[mode] != self._laneChangeModes[self._currentPlatoonMode]:
            traci.vehicle.setLaneChangeMode(self._ID, self._laneChangeModes[mode])
                    
        self.resetSplitCountDown()
        self._splitConditions = False
        self._currentPlatoonMode = mode
        
        
    def getCurrentPlatoonMode(self):
        ''' getCurrentPlatoonMode() -> PlatoonMode
        
        Returns the current platoon mode of the vehicle
        '''
        return self._currentPlatoonMode
        
    def splitCountDown(self, dt):
        '''splitCountDown(double)
        
        Decreases the time until the vehicle is split from its platoon
        '''
        self._timeUntilSplit -= dt
        if cfg.VERBOSITY >= 3:
            report("Time until split from platoon for veh '%s': %s"%(self._ID, self._timeUntilSplit))
        return self._timeUntilSplit
    
    def resetSplitCountDown(self):
        '''resetSplitCountDown(double)
        
        Resets the time until the vehicle is split from its platoon to PLATOON_SPLIT_TIME
        '''
        self._timeUntilSplit = cfg.PLATOON_SPLIT_TIME

    
    def setSplitConditions(self, b=True):
        ''' splitConditions(bool) -> void
        Sets whether splitConditions are satisfied.
        '''
        self._splitConditions = b
    
    
    def splitConditions(self):
        ''' splitConditions() -> bool
        Returns whether vehicle did not find its leader in its current platoon in this step.
        '''
        return self._splitConditions
      
    
    def isSwitchSafe(self, targetMode, switchImpatience = 0.):
        '''isSwitchSafe(PlatoonMode, double) -> bool
        
        Checks whether it is safe for this vehicle to continue in the target mode.
        The parameter switchImpatience \in [0,1] indicates the emergency of the switch
        and controls to which degree the vehicle is disposed to break harder than 
        its preferred decel.
        '''
        global vTypeParameters
        
        # if target mode already equals the current, no safety check is required
        if targetMode == self._currentPlatoonMode:
            return True
                
        # Check value of switchImpatience
        if (switchImpatience > 1.):
            warn("Given parameter switchImpatience > 1. Assuming == 1.")
            switchImpatience = 1.
        elif (switchImpatience < 0.):
            warn("Given parameter switchImpatience < 0. Assuming == 0.")
            switchImpatience = 0.            
        
        # obtain the preferred deceleration and the tau of the target vType
        decel = vTypeParameters[self._vTypes[targetMode]][tc.VAR_DECEL] 
        tau   = vTypeParameters[self._vTypes[targetMode]][tc.VAR_TAU]
        speed = self.state.speed
        
        # If time until switch decreases below 0, this indicates that a switch from platoon to normal mode is required
        maxDecel = vTypeParameters[self._vTypes[targetMode]][tc.VAR_EMERGENCY_DECEL]*switchImpatience \
                   + (1.-switchImpatience)*decel
        
        # check whether a halt at the end of the lane would prohibit the switch to a lower deceleration
        # TODO: restrict check to situations where a halt is really required
        if vTypeParameters[self._vTypes[self._currentPlatoonMode]][tc.VAR_DECEL] > decel:
            distToLaneEnd = traci.lane.getLength(self.state.laneID) - traci.vehicle.getLanePosition(self._ID)
            if self.brakeGap(speed, maxDecel) > distToLaneEnd:
                return False
        
        # check whether the ego vehicle has a leader, which must be considered
        if self.state.leaderInfo is None:
            return True
        
        leader = self.state.leader
        gap = self.state.leaderInfo[1]
        minGapDifference = vTypeParameters[self._vTypes[targetMode]][tc.VAR_MINGAP] - vTypeParameters[self.getCurrentVType()][tc.VAR_MINGAP]
        gap -= minGapDifference 
        
        if gap < 0.:
            # may arise when minGap of target type differs
            return False
        
        if leader is None:
            # This may occur if the leader is not connected, so no corresponding PVehicle exists
            # I'm not sure if this is ever called, but wouldn't exclude the possibility
            leaderDecel = traci.vehicle.getDecel(self.state.leaderInfo[0])
            leaderSpeed = traci.vehicle.getSpeed(self.state.leaderInfo[0])
        else:
            leaderDecel = vTypeParameters[leader.getCurrentVType()][tc.VAR_DECEL]
            leaderSpeed = leader.state.speed
        
        # simplified safety check: consider leaderDecel = max(leaderDecel, decel) and compare brakegaps
        leaderDecel = max(leaderDecel, decel)
        leaderBrakeGap = PVehicle.brakeGap(leaderSpeed, leaderDecel)
        headwayDist = speed*tau
        followerBrakeGap = PVehicle.brakeGap(speed, maxDecel)
        
        if cfg.VERBOSITY >= 3: 
            report("leaderSpeed = %s"%leaderSpeed
                +"\nleaderDecel = %s"%leaderDecel
                +"\ngap = %s"%gap
                +"\nleaderBrakeGap = %s"%leaderBrakeGap
                +"\nspeed = %s"%speed
                +"\ndecel = %s"%decel
                +"\nfollowerBrakeGap = %s"%followerBrakeGap
                +"\nheadwayDist = %s"%headwayDist)
        
        # TODO: test without headway...
        return gap + leaderBrakeGap - followerBrakeGap - headwayDist > 0
        
    
    @staticmethod
    def brakeGap(speed, decel):
        '''_brakeGap(double) -> double
        
        Return the break gap given a constant deceleration
        '''
        if decel <= 0.:
            return float("inf")
        return speed*speed/(2.0*decel)




