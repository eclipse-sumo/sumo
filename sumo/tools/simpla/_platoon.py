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
    
    
    def __init__(self, vehicles = []):
        '''Platoon()
        
        Create a Platoon object that holds an ordered list of its members, which is
        inititialized with 'vehicles'. Creator is responsible for setting the platoon mode of the vehicles
        '''        
        self._ID = Platoon._nextID
        Platoon._nextID += 1 
        self._vehicles = vehicles
        vehicles[0].setPlatoon(self)
        for veh in vehicles[1:]:
            veh.setPlatoon(self)
            
    
    def getLeader(self):
        '''getLeader() -> PVehicle
        
        Returns the current platoon leader
        '''
        return self._vehicles[0]
    
    
    def removeVehicle(self, veh):
        '''removeVehicle(PVehicle)
        
        Removes the vehicle from the platoon
        '''
        
        self._vehicles.remove(veh)
        if self.size() == 0:
            return
        
        if self.size() == 1:
            self.setMode(PlatoonMode.NONE)
            return
        
        if self.getMode() == PlatoonMode.CATCHUP:
            # no changes required
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
        Only checks for safety with regard to leaders and assumes that the following changes are always safe:
        PlatoonMode.LEADER -> (PlatoonMode.FOLLOWER <-> PlatoonMode.CATCHUP) -> PlatoonMode.NONE
        Note: This is (with regard to leaders) true if decel.LEADER <= decel.FOLLOWER == decel.CATCHUP <= decel.NONE
        '''
        if mode == PlatoonMode.NONE:
            if self.size() == 1:
                self._vehicles[0].setPlatoonMode(mode)
                return True
            else:
                # PlatoonMode.NONE is only admissible for solitons
                return False
        
        elif mode == PlatoonMode.CATCHUP or mode == PlatoonMode.FOLLOWER:
            if self._vehicles[0].getCurrentPlatoonMode() != PlatoonMode.NONE:
                for veh in self._vehicles:
                    veh.setPlatoonMode(mode)
                return True
            elif self._vehicles[0].isSwitchSafe(mode):
                for veh in self._vehicles:
                    veh.setPlatoonMode(mode)
                return True
            else:
                return False
            
        elif mode == PlatoonMode.LEADER:
            if self._vehicles[0].isSwitchSafe(mode):
                self._vehicles[0].setPlatoonMode(mode)
                for veh in self._vehicles[1:]:
                    veh.setPlatoonMode(PlatoonMode.FOLLOWER)
                return True
            else:
                return False
        
        else:
            raise ValueError("Unknown PlatoonMode %s"%str(mode))
        
    
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
        if not splitLeader.isSwitchSafe(mode, splitImpatience):
            # could not split off platoon safely
            return None
        elif splitImpatience != 0.:
            # TODO: consider decreasing the speedfactor (must be reset somewhere afterwards!)
            pass 
        
        # split off successful -> reduce vehicles in this platoon and create new platoon from subset of this platoon's vehicles
        pltn = Platoon(self._vehicles[index:])
        pltn.setMode(mode)
        self._vehicles = self._vehicles[:index]
        if len(self._vehicles) == 1:
            # only one vehicle remains, turn off its platoon-specific behavior (this is always considered safe)
            self._vehicles[0].setPlatoonMode(PlatoonMode.NONE)
        
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
            # this implies that size == 1
            if not self.getLeader().isSwitchSafe(PlatoonMode.LEADER):
                # leader cant safely switch to LEADER mode -> don't join
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
        