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

# TODO: For CATCHUP_FOLLOWER mode could also be set active if intra-platoon gap becomes too large

import traci
from traci.exceptions import TraCIException
import traci.constants as tc

import simpla._config as cfg
import simpla._pvehicle
from simpla._reporting import Warner, Reporter, array2String
from simpla._platoonmode import PlatoonMode
from _collections import defaultdict

warn = Warner("PlatoonManager")
report = Reporter("PlatoonManager")


class PlatoonManager(traci.StepListener):

    '''
    A PlatoonManager coordinates the initialization of platoons
    and adapts the vehicles in a platoon to change their controls accordingly.
    To use it, create a PlatoonManager and call its update() method in each
    simulation step.
    Do not create more than one PlatoonManager, if you cannot guarantee that
    the associated vehicle types are exclusive for each.
    '''

    def __init__(self):
        ''' PlatoonManager()

        Creates and initializes the PlatoonManager
        '''
        # Load parameters from config
        # vehicle type filter
        self._typeSubstrings = cfg.VEH_SELECTORS
        if self._typeSubstrings == [""]:
            warn("No typeSubstring given. Managing all vehicles.")
        # max intra platoon gap
        self._maxPlatoonGap = cfg.MAX_PLATOON_GAP
        # max distance for trying to catch up
        self._catchupDist = cfg.CATCHUP_DIST

        # platoons currently in the simulation
        # map: platoon ID -> platoon objects
        self._platoons = dict()

        # IDs of all potential platoon members currently in the simulation
        # map: ID -> vehicle
        self._connectedVehicles = dict()
        for vehID in traci.vehicle.getIDList():
            if self._isConnected(vehID):
                self._addVehicle(vehID)

        # integration step-length
        self._DeltaT = traci.simulation.getDeltaT() / 1000.

        # rate for executing the platoon logic
        if(1. / cfg.CONTROL_RATE < self._DeltaT):
            warn("Restricting given control rate (= %d per sec.) to 1 per timestep (= %d per sec.)" %
                 (cfg.CONTROL_RATE, self._DeltaT))
            self._controlInterval = self._DeltaT
        else:
            self._controlInterval = 1. / cfg.CONTROL_RATE

        self._timeSinceLastControl = 1000.

        # fill global lookup table for vType parameters (used below in safetycheck)
        knownVTypes = traci.vehicletype.getIDList()
        for origType, mappings in cfg.PLATOON_VTYPES.iteritems():
            if origType not in knownVTypes:
                warn("Unknown vType '%s'" % origType)
                continue
            origLength = traci.vehicletype.getLength(origType)
            origEmergencyDecel = traci.vehicletype.getEmergencyDecel(origType)
            for typeID in mappings.values() + [origType]:
                if typeID not in knownVTypes:
                    warn("Unknown vType '%s'" % typeID)
                    continue
                mappedLength = traci.vehicletype.getLength(typeID)
                mappedEmergencyDecel = traci.vehicletype.getEmergencyDecel(typeID)
                if origLength != mappedLength:
                    warn("length of mapped vType '%s' (%sm.) does not equal length of original vType '%s' (%sm.)\nThis will probably lead to collisions." % (
                        typeID, mappedLength, origID, origLength))
                if origEmergencyDecel != mappedEmergencyDecel:
                    warn("emergencyDecel of mapped vType '%s' (%sm.) does not equal emergencyDecel of original vType '%s' (%sm.)" % (
                        typeID, mappedEmergencyDecel, origID, origEmergencyDecel))
                simpla._pvehicle.vTypeParameters[typeID][tc.VAR_TAU] = traci.vehicletype.getTau(typeID)
                simpla._pvehicle.vTypeParameters[typeID][tc.VAR_DECEL] = traci.vehicletype.getDecel(typeID)
                simpla._pvehicle.vTypeParameters[typeID][tc.VAR_MINGAP] = traci.vehicletype.getMinGap(typeID)
                simpla._pvehicle.vTypeParameters[typeID][
                    tc.VAR_EMERGENCY_DECEL] = traci.vehicletype.getEmergencyDecel(typeID)

    def step(self, t=0):
        '''step(int)

        Manages platoons at each time step.
        NOTE: argument t is unused, larger step sizes than DeltaT are not supported.
        NOTE: (prior to SUMO 1.0): Between two calls to step() it is required to call traci.simulationStep()!
        '''
        # Handle vehicles entering and leaving the simulation
        self._addDeparted()
        self._removeArrived()
        self._timeSinceLastControl += self._DeltaT
        if self._timeSinceLastControl >= self._controlInterval:
            self._updateVehicleStates()
            self._manageFollowers()
            self._updatePlatoonOrdering()
            self._manageLeaders()
            self._adviseLanes()
            self._timeSinceLastControl = 0.

    def stop(self):
        '''stop()

        Immediately resets all vtypes, releases all vehicles from the managers control, and unsubscribes them from traci
        '''
        for veh in self._connectedVehicles.values():
            veh.setPlatoonMode(PlatoonMode.NONE)
            traci.vehicle.unsubscribe(veh.getID())

    def getPlatoonLeaders(self):
        '''getPlatoonLeaders() -> list(PVehicle)

        Returns all vehicles currently leading a platoon (of size > 1).
        These can be in PlatoonMode.LEADER or in PlatoonMode.CATCHUP
        '''
        return [pltn.getVehicles()[0] for pltn in self._platoons.values() if pltn.size() > 1]

    def getSelectionSubstrings(self):
        '''getSelectionSubstring() -> string
        Returns the platoon manager's selection substring.
        '''
        return self._typeSubstrings

    def _updateVehicleStates(self):
        '''_updateVehicleStates()

        This updates the vehicles' states with information from the simulation
        '''
        self._subscriptionResults = traci.vehicle.getSubscriptionResults()
        for veh in self._connectedVehicles.values():
            veh.state.speed = self._subscriptionResults[veh.getID()][tc.VAR_SPEED]
            veh.state.edgeID = self._subscriptionResults[veh.getID()][tc.VAR_ROAD_ID]
            veh.state.laneID = self._subscriptionResults[veh.getID()][tc.VAR_LANE_ID]
            veh.state.laneIX = self._subscriptionResults[veh.getID()][tc.VAR_LANE_INDEX]
            veh.state.leaderInfo = traci.vehicle.getLeader(veh.getID(), self._catchupDist)
            if veh.state.leaderInfo is None:
                veh.state.leader = None
                veh.state.connectedVehicleAhead = False
                continue

            if veh.state.leader is None or veh.state.leader.getID() != veh.state.leaderInfo[0]:
                if self._isConnected(veh.state.leaderInfo[0]):
                    veh.state.leader = self._connectedVehicles[veh.state.leaderInfo[0]]
                    veh.state.connectedVehicleAhead = True
                else:
                    # leader is not connected -> check whether a connected vehicle is located further downstream
                    veh.state.leader = None
                    veh.state.connectedVehicleAhead = False
                    vehAheadID = veh.state.leaderInfo[0]
                    dist = veh.state.leaderInfo[1] + traci.vehicle.getLength(vehAheadID)
                    while dist < self._catchupDist:
                        nextLeaderInfo = traci.vehicle.getLeader(vehAheadID, self._catchupDist - dist)
                        if nextLeaderInfo is None:
                            break
                        vehAheadID = nextLeaderInfo[0]
                        if self._isConnected(vehAheadID):
                            veh.state.connectedVehicleAhead = True
                            if cfg.VERBOSITY >= 4:
                                report("Found connected vehicle '%s' downstream of vehicle '%s' (at distance %s)" %
                                       (vehAheadID, veh.getID(), dist + nextLeaderInfo[1]))
                            break
                        dist += nextLeaderInfo[1] + traci.vehicle.getLength(vehAheadID)

    def _removeArrived(self):
        ''' _removeArrived()

        Remove all vehicles that have left the simulation from _connectedVehicles.
        Returns the number of removed connected vehicles
        '''
        count = 0
        toRemove = defaultdict(list)
        for ID in traci.simulation.getArrivedIDList():
            # first store arrived vehicles platoonwise
            if not self._isConnected(ID):
                continue
            if cfg.VERBOSITY >= 2:
                report("Removing arrived vehicle '%s'" % ID)
            veh = self._connectedVehicles.pop(ID)
            toRemove[veh.getPlatoon().getID()].append(veh)
            count += 1

        for pltnID, vehs in toRemove.iteritems():
            pltn = self._platoons[pltnID]
            pltn.removeVehicles(vehs)
            if pltn.size() == 0:
                # remove empty platoons
                self._platoons.pop(pltn.getID())

        return count

    def _addDeparted(self):
        '''_addDeparted()

        Scans newly departed vehicles for such whose ID indicates that they are
        possible platooning candidates and registers them in _connectedVehicles.
        Returns the number of new vehicles found
        '''
        count = 0
        for newID in traci.simulation.getDepartedIDList():
            # Check if the vehicle selector applies to this vehicle
            connected = False
            typeID = traci.vehicle.getTypeID(newID)
            for s in self._typeSubstrings:
                if typeID.find(s) >= 0:
                    connected = True
                    break
            # create a new PVehicle object
            if connected:
                self._addVehicle(newID)
                count += 1
        return count

    def _addVehicle(self, vehID):
        '''_addVehicle(string)

        Creates a new PVehicle object and registers is soliton platoon
        '''
        try:
            traci.vehicle.subscribe(vehID, (tc.VAR_ROAD_ID, tc.VAR_LANE_INDEX, tc.VAR_LANE_ID, tc.VAR_SPEED))
            veh = simpla._pvehicle.PVehicle(vehID, self._controlInterval)
        except TraCIException:
            warn("Tried to create non-existing vehicle '%s'" % vehID)
            return
        except KeyError as e:
            raise e

        if cfg.VERBOSITY >= 2:
            report("Adding vehicle '%s'" % vehID)
        self._connectedVehicles[vehID] = veh
        self._platoons[veh.getPlatoon().getID()] = veh.getPlatoon()

    def _manageFollowers(self):
        '''_manageFollowers()

        Iterates over platoon-followers and
        1) checks whether a Platoon has to be split due to incoherence persisting over some time
        '''
        newPlatoons = []
        for pltnID, pltn in self._platoons.iteritems():
            # encourage all vehicles to adopt the current mode of the platoon
            # NOTE: for switching between platoon modes, there may be vehicles not
            #       complying immediately. They are asked to do so, here in each turn.
            pltn.adviseMemberModes()
            # splitIndices: indices of vehicles that request a split
            splitIndices = []
            for ix, veh in enumerate(pltn.getVehicles()[1:]):
                # check whether to split the platoon at index ix
                leaderInfo = veh.state.leaderInfo
                if leaderInfo is None or not self._isConnected(leaderInfo[0]) or leaderInfo[1] > self._maxPlatoonGap:
                    # no leader or no leader that allows platooning
                    veh.setSplitConditions(True)
                else:
                    # ego has a connected leader
                    leaderID = leaderInfo[0]
                    leader = self._connectedVehicles[leaderID]
                    if pltn.getVehicles()[ix] == leader:
                        # ok, this is really the leader as registered in the platoon
                        veh.setSplitConditions(False)
                        veh.resetSplitCountDown()
                    elif leader.getPlatoon() == veh.getPlatoon():
                        # the platoon order is violated.
                        if cfg.VERBOSITY >= 2:
                            report("Platoon order for platoon '%s' is violated: real leader '%s' is not registered as leader of '%s'" % (
                                pltnID, leaderID, veh.getID()), 1)
                        veh.setSplitConditions(False)
                    else:
                        # leader is connected but belongs to a different platoon
                        veh.setSplitConditions(True)
                if veh.splitConditions():
                    # eventually increase isolation time
                    timeUntilSplit = veh.splitCountDown(self._timeSinceLastControl)
                    if timeUntilSplit <= 0:
                        splitIndices.append(ix + 1)
            # try to split at the collected splitIndices
            for ix in reversed(splitIndices):
                newPlatoon = pltn.split(ix)
                if newPlatoon is not None:
                    # if the platoon was split, register the splitted platoons
                    newPlatoons.append(newPlatoon)
        for pltn in newPlatoons:
            self._platoons[pltn.getID()] = pltn

    def _manageLeaders(self):
        '''_manageLeaders()

        Iterates over platoon-leaders and
        1) checks whether two platoons (including "one-vehicle platoons") may merge for being sufficiently close
        2) advises platoon-leaders to try to catch up with a platoon in front
        '''
        # list of platoon ids that merged into another platoon
        toRemove = []
        for pltnID, pltn in self._platoons.iteritems():
            # platoon leader
            pltnLeader = pltn.getLeader()
            # try setting back mode to regular platoon mode if leader is kept in FOLLOWER mode due to safety reasons
            # or if the ordering within platoon changed
            if pltnLeader.getCurrentPlatoonMode() == PlatoonMode.FOLLOWER:
                pltn.setModeWithImpatience(PlatoonMode.LEADER, self._controlInterval)
            elif pltnLeader.getCurrentPlatoonMode() == PlatoonMode.CATCHUP_FOLLOWER:
                pltn.setModeWithImpatience(PlatoonMode.CATCHUP, self._controlInterval)
            # get leader of the leader
            leaderInfo = pltnLeader.state.leaderInfo

            if leaderInfo is None or leaderInfo[1] > self._catchupDist:
                # No other vehicles ahead
                # reset vehicle types (could all have been in catchup mode)
                if pltn.size() == 1:
                    pltn.setModeWithImpatience(PlatoonMode.NONE, self._controlInterval)
                else:
                    # try to set mode to regular platoon mode
                    pltn.setModeWithImpatience(PlatoonMode.LEADER, self._controlInterval)
                continue

            if not self._isConnected(leaderInfo[0]):
                # Immediate leader is not connected
                if pltnLeader.state.connectedVehicleAhead:
                    # ... but further downstream there is a potential platooning partner
                    pltn.setModeWithImpatience(PlatoonMode.CATCHUP, self._controlInterval)
                elif pltn.size() == 1:
                    pltn.setModeWithImpatience(PlatoonMode.NONE, self._controlInterval)
                else:
                    # try to set mode to regular platoon mode
                    pltn.setModeWithImpatience(PlatoonMode.LEADER, self._controlInterval)
                continue

            # leader vehicle
            leaderID, leaderDist = leaderInfo
            leader = self._connectedVehicles[leaderID]

            # Commented out -> isLastInPlatoon should not be a hindrance to join platoon
            # tryCatchup = leader.isLastInPlatoon() and leader.getPlatoon() != pltn
            # join = tryCatchup and leaderDist <= self._maxPlatoonGap

            # Check if leader is on pltnLeader's route
            # (sometimes a 'linkLeader' on junction is returned by traci.getLeader())
            # XXX: This prevents joining attempts on internal lanes (probably doesn't hurt so much)
            pltnLeaderRoute = traci.vehicle.getRoute(pltnLeader.getID())
            pltnLeaderRouteIx = traci.vehicle.getRouteIndex(pltnLeader.getID())
            leaderEdge = leader.state.edgeID
            if leaderEdge not in pltnLeaderRoute[pltnLeaderRouteIx:]:
                continue

            if leader.getPlatoon() == pltn:
                # Platoon order is corrupted, don't join own platoon.
                continue

            if leaderDist <= self._maxPlatoonGap:
                # Try to join the platoon in front
                if leader.getPlatoon().join(pltn):
                    toRemove.append(pltnID)
                    # Debug
                    if cfg.VERBOSITY >= 2:
                        report("Platoon '%s' joined Platoon '%s', which now contains " % (pltn.getID(), leader.getPlatoon().getID()) +
                               "vehicles:\n%s" % str([veh.getID() for veh in leader.getPlatoon().getVehicles()]))
                    continue
                else:
                    if cfg.VERBOSITY >= 3:
                        report("Merging of platoons '%s' (%s) and '%s' (%s) would not be safe." % (pltn.getID(), str([veh.getID() for veh in pltn.getVehicles()]),
                                                                                                   leader.getPlatoon().getID(), str([veh.getID() for veh in leader.getPlatoon().getVehicles()])))
            else:
                # Join failed due to too large distance. Try to get closer (change to CATCHUP mode).
                if pltn.setMode(PlatoonMode.CATCHUP):
                    # try to catch up with the platoon in front
                    if cfg.VERBOSITY >= 3:
                        report("Activating 'catch-up' mode for platoon '%s' (%s)" %
                               (pltn.getID(), str([veh.getID() for veh in pltn.getVehicles()])))
                else:
                    if cfg.VERBOSITY >= 3:
                        report("Switch to catchup mode would not be safe for platoon '%s' (%s) chasing platoon '%s' (%s)." % (pltn.getID(), str([veh.getID() for veh in pltn.getVehicles()]),
                                                                                                                              leader.getPlatoon().getID(), str([veh.getID() for veh in leader.getPlatoon().getVehicles()])))

        # remove merged platoons
        for pltnID in toRemove:
            self._platoons.pop(pltnID)

    def _updatePlatoonOrdering(self):
        '''_manageLeaders()

        Iterates through platoons and checks whether they are in an appropriate order.
        '''
        if cfg.VERBOSITY >= 4:
            report("Checking platoon ordering", 3)

        for pltnID, pltn in self._platoons.iteritems():
            if pltn.size() == 1:
                continue
            # collect leaders within platoon
            intraPlatoonLeaders = []
            leaderID = None
            for ix, veh in enumerate(pltn.getVehicles()):
                leaderFromeSamePlatoon = False
                if veh.state.leaderInfo is not None:
                    # leader detected
                    leaderID = veh.state.leaderInfo[0]
                    if self._isConnected(leaderID):
                        # leader is connected
                        leader = self._connectedVehicles[leaderID]
                        if leader.getPlatoon() == pltn:
                            # leader belongs to same platoon
                            leaderFromeSamePlatoon = True
                            intraPlatoonLeaders.append(leader)

                if not leaderFromeSamePlatoon:
                    intraPlatoonLeaders.append(None)

                if cfg.VERBOSITY >= 4:
                    report("Platoon %s: Leader for veh '%s' is '%s' (%s)"
                           % (pltn.getID(), veh.getID(), str(leaderID), ("same platoon" if (intraPlatoonLeaders[-1] is not None) else "not from same platoon")), 3)

            pltn.setVehicles(self.reorderVehicles(pltn.getVehicles(), intraPlatoonLeaders))

    @staticmethod
    def reorderVehicles(vehicles, actualLeaders):
        '''
        reorderVehicles(list(pVehicle), list(pVehicle)) -> list(pVehicle)

        This method reorders the given vehicles such that the newly ordered vehicles fulfill: [None] + vehicles[:-1] == actualLeaders
        (if not several vehicles have the same actual leader. For those it is only guaranteed that one will be associated correctly, not specifying which one)
        '''

        if cfg.VERBOSITY >= 4:
            report("reorderVehicles(vehicles, actualLeaders)\nvehicles = %s\nactualLeaders=%s" %
                   (array2String(vehicles), array2String(actualLeaders)), 3)

        done = False
        # this is needed as abort criterion if two have the same leader (This cannot be excluded for sublane at least)
        iter_count = 0
        nVeh = len(vehicles)
        # make a copy to write into
        actualLeaders = list(actualLeaders)
        while(not done and iter_count < nVeh):
            newVehOrder = None
            registeredLeaders = [None] + vehicles[:-1]
            if cfg.VERBOSITY >= 4:
                report("vehicles: %s" % array2String(vehicles), 3)
                report("Actual leaders: %s" % array2String(actualLeaders), 3)
                report("registered leaders: %s" % array2String(registeredLeaders), 3)
            for (ego, registeredLeader, actualLeader) in reversed(zip(vehicles, registeredLeaders, actualLeaders)):
                if (ego == actualLeader):
                    if cfg.VERBOSITY >= 2:
                        warn("Platoon %s:\nVehicle '%s' was found as its own leader. Platoon order might be corrupted." % (
                            array2String(vehicles), str(ego)))
                    return vehicles

                if actualLeader is None:
                    # No actual leader was found. Could be due to platoon LC maneuver or non-connected vehicle merging in
                    # -> no reordering implications.
                    continue
                if actualLeader == registeredLeader:
                    continue

                # intra-platoon order is corrupted
                if newVehOrder is None:
                    # init newVehOrder
                    newVehOrder = list(vehicles)

                # relocate ego

                if cfg.VERBOSITY >= 4:
                    report("relocating: %s to follow %s" % (str(ego), str(actualLeader)), 3)
                    report("prior newVehOrder: %s" % array2String(newVehOrder), 3)
                    report("prior actualLeaders: %s" % array2String(actualLeaders), 3)
                oldEgoIndex = newVehOrder.index(ego)
                del newVehOrder[oldEgoIndex]
                del actualLeaders[oldEgoIndex]

                if cfg.VERBOSITY >= 4:
                    report("immed newVehOrder: %s" % array2String(newVehOrder), 3)
                    report("immed actualLeaders: %s" % array2String(actualLeaders), 3)
                actualLeaderIndex = newVehOrder.index(actualLeader)
                newVehOrder.insert(actualLeaderIndex + 1, ego)
                actualLeaders.insert(actualLeaderIndex + 1, actualLeader)
                if cfg.VERBOSITY >= 4:
                    report("current newVehOrder: %s" % array2String(newVehOrder), 3)
                    report("current actualLeaders: %s" % array2String(actualLeaders), 3)

            done = newVehOrder is None
            if not done:
                vehicles = newVehOrder
                iter_count += 1

        if iter_count != 0:
            if cfg.VERBOSITY >= 4:
                report("Ordering within Platoon %s was corrupted.\nNew Order: %s\nLeaders: %s" %
                       (vehicles[0].getPlatoon().getID(), array2String(vehicles), array2String(actualLeaders)), 3)

        return vehicles

    def _adviseLanes(self):
        '''_adviseLanes()

        At the moment this only advises all platoon followers to change to their leaders lane
        if it is on a different lane on the same edge. Otherwise, followers are told to keep their
        lane for the next time step.
        NOTE: Future, more sophisticated lc advices should go here.
        '''
        for pltn in self._platoons.values():
            for ix, veh in enumerate(pltn.getVehicles()[1:]):
                laneID = veh.state.laneID
                if laneID == "" or laneID[0] == ":":
                    continue
                # Find the leader in the platoon and request a lanechange if appropriate
                leader = pltn.getVehicles()[ix]
                if leader.state.edgeID == veh.state.edgeID:
                    # leader is on the same edge, advise follower to use the same lane
                    try:
                        traci.vehicle.changeLane(veh.getID(), leader.state.laneIX, self._controlInterval)
                    except traci.exceptions.TraCIException as e:
                        warn("Lanechange advice for vehicle'%s' failed. Message:\n%s" % (veh.getID(), e.message))
                        pass
                else:
                    # leader is on another edge, just stay on the current and hope it is the right one
                    try:
                        traci.vehicle.changeLane(veh.getID(), veh.state.laneIX, self._controlInterval)
                    except traci.exceptions.TraCIException as e:
                        warn("Lanechange advice for vehicle'%s' failed. Message:\n%s" % (veh.getID(), e.message))
                        pass

    def _isConnected(self, vehID):
        '''_isConnected(string) -> bool

        Returns whether the given vehicle is a potential platooning participant
        '''
        if vehID in self._connectedVehicles:
            return True
        else:
            return False
