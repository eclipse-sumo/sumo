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

import sys, math
from simpla._reporting import Warner, Reporter
from simpla._platoonmode import PlatoonMode
import simpla._config as cfg

warn = Warner("Platoon")
report = Reporter("Platoon")

class Platoon(object):
    '''
    '''
    
    # static platoon ID counter
    _nextID = 0
    
    
    def __init__(self, vehicles = [], registerVehicles=True):
        '''Platoon(list(PVehicle), bool) -> Platoon
        
        Create a Platoon object that holds an ordered list of its members, which is inititialized with 'vehicles'. 
        Creator is responsible for setting the platoon mode of the vehicles. if registerVehicles is set, the vehicle's
        platoon reference veh._platoon is set to the newly created platoon.
        '''        
        self._ID = Platoon._nextID
        Platoon._nextID += 1 
        self._vehicles = vehicles
        if registerVehicles:
            self.registerVehicles()
    
    
    def registerVehicles(self):
        ''' registerVehicles() -> void
        Sets reference to this platoon at member-vehicles' side 
        '''
        for veh in self._vehicles:
            veh.setPlatoon(self)
            
    
    def getLeader(self):
        '''getLeader() -> PVehicle
        
        Returns the current platoon leader
        '''
        return self._vehicles[0]
    
    
    def removeVehicles(self, vehs):
        '''removeVehicles(PVehicle)
        
        Removes the vehicles from the platoon
        '''
        for veh in vehs:
            self._vehicles.remove(veh)
        
        if self.size() == 0:
            return
        
        if self.size() == 1:
            self.setMode(PlatoonMode.NONE)
            return
        
        if self.getMode() == PlatoonMode.CATCHUP:
            # no changes required. TODO: adapt if platoon internal catchup is introduced
            return
        
        # remains the regular platoon situation
        self.setMode(PlatoonMode.LEADER)
        
        
    def getID(self):
        '''getID() -> int
        
        Returns the platoon's id
        '''
        return self._ID
    
    def getVehicles(self):
        '''getVehicles() -> list(PVehicle)
        
        Returns the platoon members as an ordered list. The leader is at index 0.
        '''
        return self._vehicles
    
    def size(self):
        '''size() -> int
        
        Returns number of vehicles currently in the platoon
        '''
        return len(self._vehicles)
    
    def setMode(self, mode):
        '''setMode(PlatoonMode) -> bool
        
        Returns whether the change to the requested platoon mode could be performed safely.
        Only checks for safety with regard to leaders.
        Note: safety assumptions of previous versions are dropped, now
        '''
        if mode == PlatoonMode.NONE:
            if self.size() == 1 and self._vehicles[0].isSwitchSafe(mode):
                self._vehicles[0].setPlatoonMode(mode)
                return True
            else:
                # PlatoonMode.NONE is only admissible for solitons
                return False
        
        elif mode == PlatoonMode.CATCHUP or mode == PlatoonMode.FOLLOWER:
            if self._vehicles[0].isSwitchSafe(mode):
                self._vehicles[0].setPlatoonMode(mode)
                for veh in self._vehicles:
                    if veh.isSwitchSafe(mode):
                        veh.setPlatoonMode(mode)
                return True
            else:
                return False
            
        elif mode == PlatoonMode.LEADER:
            if self._vehicles[0].isSwitchSafe(mode):
                self._vehicles[0].setPlatoonMode(mode)
                for veh in self._vehicles[1:]:
                    if veh.isSwitchSafe(PlatoonMode.FOLLOWER):
                        veh.setPlatoonMode(PlatoonMode.FOLLOWER)
                return True
            else:
                return False
        
        else:
            raise ValueError("Unknown PlatoonMode %s"%str(mode))
        
    def adviseMemberModes(self):
        ''' adviseMemberModes() -> void
        Advise all member vehicles to adopt the adequate platoon mode if safely possible.
        '''
        mode = self.getMode()
        vehs = self._vehicles
        
        # impose mode for leader
        if vehs[0].isSwitchSafe(mode):
            vehs[0].setPlatoonMode(mode)
        else:
            # TODO: handle switch impatience?
            pass
            
        if mode == PlatoonMode.LEADER: 
            # use follower mode for followers if platoon is in normal mode
            mode = PlatoonMode.FOLLOWER
        
        # impose mode for followers
        for veh in vehs[1:]:
            if veh.isSwitchSafe(mode):
                veh.setPlatoonMode(mode)
            else:
                # TODO: handle switch impatience?
                pass
            
        
            
    
    
    def split(self, index):
        '''split(int) -> Platoon
        
        Splits off a subplatoon from the end of the platoon at the given index.
        The given index must correspond to the new leader (of the splitoff subplatoon)
        Returns the splitoff platoon.
        '''
        if index <= 0 or index > self.size():
            raise ValueError("Platoon.split(index) expected and index in [1,%d]. Given value: %s"%(self.size(), index))

        splitLeader = self._vehicles[index]
        mode = PlatoonMode.LEADER if (index < self.size()-1) else PlatoonMode.NONE
        splitImpatience = 1. - math.exp(min([0., splitLeader._timeUntilSplit]))
        pltn = Platoon(self._vehicles[index:], False)
        
        if not pltn.setMode(mode):
            # could not split off platoon safely
            return None
            if splitImpatience != 0.:
                # TODO: consider decreasing the speedfactor (must be reset somewhere afterwards!)
                pass 
        
        # split can be taken out safely -> reduce vehicles in this platoon
        self._vehicles = self._vehicles[:index]
        # set reference to new platoon in splitted vehicles
        pltn.registerVehicles()
        
        if len(self._vehicles) == 1:
            # only one vehicle remains, turn off its platoon-specific behavior
            self.setMode(PlatoonMode.NONE)
        
        if cfg.VERBOSITY >= 2:
            report("Platoon '%s' splits (newly formed platoon is '%s'):\n"%(self._ID, pltn.getID()) \
                   + "Platoon '%s': %s\nPlatoon '%s': %s"%(self._ID, str([veh.getID() for veh in self._vehicles]), 
                                                                        pltn.getID(), str([veh.getID() for veh in pltn.getVehicles()])),1)
        return pltn
    
    
    
    def join(self, pltn):
        '''join(Platoon)
        
        Tries to add the given platoon to the end of this. Returns True if this could safely be executed.
        '''
        vehs = pltn.getVehicles()
        if self.getMode() == PlatoonMode.CATCHUP:
            if pltn.setMode(PlatoonMode.CATCHUP):
                for v in vehs: v.setPlatoon(self)
                self._vehicles.extend(vehs)
                return True
            else:
                return False
        
        if self.getMode() == PlatoonMode.NONE:
            # this implies that size == 1, i.e. the platoon is a solitary vehicle
            if not self.getLeader().isSwitchSafe(PlatoonMode.LEADER):
                # vehicle cant safely switch to LEADER mode -> don't join
                return False
        
        # At this point either this has PlatoonMode.LEADER or PlatoonMode.NONE (size==1), and switch to leader was safe
        if pltn.setMode(PlatoonMode.FOLLOWER):
            for v in vehs: v.setPlatoon(self)
            self._vehicles.extend(vehs)
            self.getLeader().setPlatoonMode(PlatoonMode.LEADER)
            return True
        else:
            return False
        
        
    def getMode(self):
        '''getMode() -> PlatoonMode
        
        Returns the platoon leader's current PlatoonMode
        '''
        return self._vehicles[0].getCurrentPlatoonMode()
        
    