# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _platoonmanager.py
# @author Leonhard Luecken
# @author Mirko Barthauer
# @date   2017-04-09


# TODO: For CATCHUP_FOLLOWER mode could also be set active if intra-platoon gap becomes too large

from collections import defaultdict
import traci
from traci.exceptions import TraCIException
import traci.constants as tc

import simpla._reporting as rp
import simpla._config as cfg
import simpla._pvehicle
from simpla import SimplaException
from simpla._platoonmode import PlatoonMode

warn = rp.Warner("PlatoonManager")
report = rp.Reporter("PlatoonManager")


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
        if rp.VERBOSITY >= 2:
            report("Initializing simpla.PlatoonManager...", True)
        # Load parameters from config
        # vehicle type filter
        self._typeSubstrings = cfg.VEH_SELECTORS
        if self._typeSubstrings == [""] and rp.VERBOSITY >= 1:
            warn("No typeSubstring given. Managing all vehicles.", True)
        elif rp.VERBOSITY >= 2:
            report("Managing all vTypes selected by %s" % str(self._typeSubstrings), True)
        # max intra platoon gap
        self._maxPlatoonGap = cfg.MAX_PLATOON_GAP
        # max intra platoon headway
        self._maxPlatoonHeadway = cfg.MAX_PLATOON_HEADWAY
        # max distance for trying to catch up
        self._catchupDist = cfg.CATCHUP_DIST
        # max headway for trying to catch up
        self._catchupHeadway = cfg.CATCHUP_HEADWAY
        # ego vehicle needs at least this number of future edges in common with leader
        # before agreeing to follow...
        self._edgenumberLookahead = cfg.EDGE_LOOKAHEAD
        # Or the ego vehicle needs at least this distance of common route length with leader
        # before agreeing to follow.
        self._distLookahead = cfg.DIST_LOOKAHEAD
        # no lane change advice if vehicle has less than this distance
        # to the next  junction
        self._lanechangeMinDist = cfg.LC_MINDIST
        # set the platooning join / split criterion: gap distance or headway
        self._useHeadway = cfg.USE_HEADWAY

        # platoons currently in the simulation
        # map: platoon ID -> platoon objects
        self._platoons = dict()

        # IDs of all potential platoon members currently in the simulation
        # map: ID -> vehicle
        self._connectedVehicles = dict()
        for vehID in traci.vehicle.getIDList():
            if self._hasConnectedType(vehID):
                self._addVehicle(vehID)

        # integration step-length
        self._DeltaT = traci.simulation.getDeltaT()

        # rate for executing the platoon logic
        if (1. / cfg.CONTROL_RATE < self._DeltaT):
            if rp.VERBOSITY >= 1:
                warn(
                    "Restricting given control rate (= %d per sec.) to 1 per timestep (= %g per sec.)" %
                    (cfg.CONTROL_RATE, 1. / self._DeltaT), True)
            self._controlInterval = self._DeltaT
        else:
            self._controlInterval = 1. / cfg.CONTROL_RATE

        self._timeSinceLastControl = 1000.

        # Check for undefined vtypes and fill with defaults
        for origType, specialTypes in cfg.PLATOON_VTYPES.items():
            if specialTypes.get(PlatoonMode.FOLLOWER) is None:
                if rp.VERBOSITY >= 2:
                    report("Setting unspecified follower vtype for '%s' to '%s'" %
                           (origType, specialTypes[PlatoonMode.LEADER]), True)
                specialTypes[PlatoonMode.FOLLOWER] = specialTypes[PlatoonMode.LEADER]
            if specialTypes.get(PlatoonMode.CATCHUP) is None:
                if rp.VERBOSITY >= 2:
                    report("Setting unspecified catchup vtype for '%s' to '%s'" % (origType, origType), True)
                specialTypes[PlatoonMode.CATCHUP] = origType
            if specialTypes.get(PlatoonMode.CATCHUP_FOLLOWER) is None:
                if rp.VERBOSITY >= 2:
                    report("Setting unspecified catchup-follower vtype for '%s' to '%s'" %
                           (origType, specialTypes[PlatoonMode.FOLLOWER]), True)
                specialTypes[PlatoonMode.CATCHUP_FOLLOWER] = specialTypes[PlatoonMode.FOLLOWER]
# Commented snippet generated automatically a catchup follower type with a different color
#                 catchupFollowerType = origType + "_catchupFollower"
#                 specialTypes[PlatoonMode.CATCHUP]=catchupFollowerType
#                 if rp.VERBOSITY >= 2:
#                     print("Catchup follower type '%s' for '%s' dynamically created as duplicate of '%s'" %
#                       (catchupFollowerType, origType, specialTypes[PlatoonMode.CATCHUP_FOLLOWER]))
#                 traci.vehicletype.copy(specialTypes[PlatoonMode.CATCHUP_FOLLOWER] , catchupFollowerType)
#                 traci.vehicletype.setColor(catchupFollowerType, (0, 255, 200, 0))

        # fill global lookup table for vType parameters (used below in safetycheck)
        knownVTypes = {typeID: traci.vehicletype.getVehicleClass(typeID) for typeID in traci.vehicletype.getIDList()}
        vTypesToCheck = set()
        if cfg.VEH_SELECTORS[0] == '':
            vTypesToCheck.update([typeID for typeID, vClass in knownVTypes.items()
                                  if vClass not in ('bicycle', 'pedestrian')])

        for origType, mappings in cfg.PLATOON_VTYPES.items():
            if origType not in knownVTypes:
                raise SimplaException(
                    "vType '%s' is unknown to sumo! Note: Platooning vTypes must be defined at startup." % origType)
            origLength = traci.vehicletype.getLength(origType)
            origEmergencyDecel = traci.vehicletype.getEmergencyDecel(origType)
            for typeID in list(mappings.values()) + [origType]:
                if typeID not in knownVTypes:
                    raise SimplaException(
                        "vType '%s' is unknown to sumo! Note: Platooning vTypes must be defined at startup." % typeID)
                mappedLength = traci.vehicletype.getLength(typeID)
                mappedEmergencyDecel = traci.vehicletype.getEmergencyDecel(typeID)
                if origLength != mappedLength:
                    if rp.VERBOSITY >= 1:
                        warn(("length of mapped vType '%s' (%sm.) does not equal length of original vType " +
                              "'%s' (%sm.)\nThis will probably lead to collisions.") % (
                            typeID, mappedLength, origType, origLength), True)
                if origEmergencyDecel != mappedEmergencyDecel:
                    if rp.VERBOSITY >= 1:
                        warn(("emergencyDecel of mapped vType '%s' (%gm.) does not equal emergencyDecel of original " +
                              "vType '%s' (%gm.)") % (
                             typeID, mappedEmergencyDecel, origType, origEmergencyDecel), True)
                vTypesToCheck.add(typeID)
        for typeID in vTypesToCheck:
            simpla._pvehicle.vTypeParameters[typeID][tc.VAR_TAU] = traci.vehicletype.getTau(typeID)
            simpla._pvehicle.vTypeParameters[typeID][tc.VAR_DECEL] = traci.vehicletype.getDecel(typeID)
            simpla._pvehicle.vTypeParameters[typeID][tc.VAR_MINGAP] = traci.vehicletype.getMinGap(typeID)
            simpla._pvehicle.vTypeParameters[typeID][tc.VAR_EMERGENCY_DECEL] = traci.vehicletype.getEmergencyDecel(
                typeID)

    def step(self, t=0):
        '''step(int)

        Manages platoons at each time step.
        NOTE: argument t is unused, larger step sizes than DeltaT are not supported.
        '''
        if not t == 0 and rp.VERBOSITY >= 1:
            warn("Step lengths that differ from SUMO's simulation step length are not supported and probably lead " +
                 "to undesired behavior.\nConsider decreasing simpla's control rate instead.")
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
        # platoon manager can only be removed by calling simpla.stop()
        return True

    def stop(self):
        '''stop()

        Immediately resets all vtypes, releases all vehicles from the managers control, and unsubscribes them from traci
        '''
        for veh in self._connectedVehicles.values():
            veh.setPlatoonMode(PlatoonMode.NONE)
            traci.vehicle.unsubscribe(veh.getID())
        self._connectedVehicles = dict()
        simpla._platoon.Platoon._nextID = 0

    def getAveragePlatoonLength(self):
        '''getAveragePlatoonLength() -> float

        Returns the average number of vehicles in a platoon or 0 if there are no platoons
        '''
        platoonCount = len(self._platoons)
        if platoonCount == 0:
            return 0
        else:
            return sum([platoon.size() for platoon in self._platoons.values()])/platoonCount

    def getAveragePlatoonSpeed(self):
        '''getAveragePlatoonSpeed() -> float

        Returns the average speed of all vehicles in platoons or 0 if there are no platoons / or all in stand
        '''
        vehCount = sum([platoon.size() for platoon in self._platoons.values()])
        if vehCount == 0:
            return 0
        s = sum([vehicle.state.speed for platoon in self._platoons.values() for vehicle in platoon.getVehicles()])
        return s / vehCount

    def getPlatoonIDList(self, edgeID):
        '''getPlatoonIDList(string) -> list(integer)

        Returns the list of platoon IDs where at least one vehicle is currently on the edge given by its ID
        '''
        return [pID for pID, platoon in self._platoons.items()
                if edgeID in [vehicle.state.edgeID for vehicle in platoon.getVehicles()]]

    def getPlatoonLeaderIDList(self):
        '''getPlatoonLeaderIDList() -> list(string)

        Returns the list of all platoon leader vehicles' IDs
        '''
        return [platoon.getLeader().getID() for platoon in self._platoons.values()]

    def getPlatoonInfo(self, platoonID):
        '''getPlatoonInfo(platoonID) -> dict

        Returns a dict with statistical information about the platoon given by its (numerical) ID
        '''
        if platoonID in self._platoons:
            plt = self._platoons[platoonID]
            return {"laneID": plt.getLeader().state.laneID,
                    "members": [vehicle.getID() for vehicle in plt.getVehicles()]}
        else:
            return {}

    def getPlatoonID(self, vehicleID):
        '''getPlatoonID(vehicleID) -> integer

        Returns the ID of the platoon a vehicle belongs to
        '''
        if vehicleID in self._connectedVehicles:
            return self._connectedVehicles[vehicleID].getPlatoon().getID()
        return -1

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
        self._subscriptionResults = traci.vehicle.getAllSubscriptionResults()
        for veh in self._connectedVehicles.values():
            if (veh.getID() not in self._subscriptionResults):
                # FIXME: For some reason, this is in rare occasions called with vehicles,
                #        which have no subscription results.
                return
            veh.state.speed = self._subscriptionResults[veh.getID()][tc.VAR_SPEED]
            veh.state.edgeID = self._subscriptionResults[veh.getID()][tc.VAR_ROAD_ID]
            veh.state.laneID = self._subscriptionResults[veh.getID()][tc.VAR_LANE_ID]
            veh.state.laneIX = self._subscriptionResults[veh.getID()][tc.VAR_LANE_INDEX]
            veh.state.leaderInfo = traci.vehicle.getLeader(
                veh.getID(), self._catchupHeadway*veh.state.speed if self._useHeadway else self._catchupDist)

            # check if there is a new, connected leader and if so, verify if this leader
            # has sufficient edges in common with the ego vehicle
            if veh.state.leaderInfo is not None:
                noLeader = True
                leaderID = veh.state.leaderInfo[0]
                # check if old leader, if any...
                if veh.state.leader is not None:
                    if leaderID == veh.state.leader.getID():
                        # this is the current leader, just keep it
                        noLeader = False
                # check if leaderID could be  a new leader
                if self._isConnected(leaderID) & noLeader:
                    # new potential, connected  leader
                    # check if this leader has enough route in common with ego
                    egoRoute = traci.vehicle.getRoute(veh.getID())
                    leaderRoute = traci.vehicle.getRoute(leaderID)
                    idxLeader = traci.vehicle.getRouteIndex(leaderID)
                    leaderEdgeID = leaderRoute[idxLeader]
                    if leaderEdgeID in egoRoute:
                        idxEgo = egoRoute.index(leaderEdgeID)
                        idxDelta = min(idxEgo+self._edgenumberLookahead, len(egoRoute))-idxEgo
                        idxDelta = min(idxDelta, len(leaderRoute)-idxLeader)
                        if idxDelta >= 0:
                            # check if common distance of routes is sufficient
                            d = 0.0
                            routeInCommon = True
                            for edgeIDEgo, edgeIDLeader in zip(egoRoute[idxEgo:idxEgo+idxDelta],
                                                               leaderRoute[idxLeader:idxLeader+idxDelta]):
                                if edgeIDEgo == edgeIDLeader:
                                    d += traci.lane.getLength(edgeIDEgo+'_0')
                                    # check if lookahead distance is satisfied
                                    if d > self._distLookahead:
                                        # print '  >>>',veh.getID(),'enough dist in common with leader',leaderID
                                        noLeader = False
                                        break
                                else:
                                    routeInCommon = False
                                    break
                            # sufficient edges in common
                            if routeInCommon:
                                noLeader = False
                if noLeader:
                    # print '  set no leader'
                    veh.state.leader = None
                    veh.state.connectedVehicleAhead = False
                    veh.state.leaderInfo = None
                    continue

            if veh.state.leaderInfo is None:
                veh.state.leader = None
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
                            if rp.VERBOSITY >= 4:
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
            if rp.VERBOSITY >= 2:
                report("Removing arrived vehicle '%s'" % ID)
            veh = self._connectedVehicles.pop(ID)
            toRemove[veh.getPlatoon().getID()].append(veh)
            count += 1

        for pltnID, vehs in toRemove.items():
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
            # create a new PVehicle object if new vehicle is connected
            if self._hasConnectedType(newID):
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
            if rp.VERBOSITY >= 1:
                warn("Tried to create non-existing vehicle '%s'" % vehID)
            return
        except KeyError as e:
            raise e

        if rp.VERBOSITY >= 2:
            report("Adding vehicle '%s'" % vehID)
        self._connectedVehicles[vehID] = veh
        self._platoons[veh.getPlatoon().getID()] = veh.getPlatoon()

    def _manageFollowers(self):
        '''_manageFollowers()

        Iterates over platoon-followers and
        1) checks whether a Platoon has to be split due to incoherence persisting over some time
        '''
        newPlatoons = []
        for pltnID, pltn in self._platoons.items():
            # encourage all vehicles to adopt the current mode of the platoon
            # NOTE: for switching between platoon modes, there may be vehicles not
            #       complying immediately. They are asked to do so, here in each turn.
            pltn.adviseMemberModes()
            # splitIndices: indices of vehicles that request a split
            splitIndices = []
            for ix, veh in enumerate(pltn.getVehicles()[1:]):
                # check whether to split the platoon at index ix
                leaderInfo = veh.state.leaderInfo
                maxGap = max(self._maxPlatoonHeadway * veh.state.speed, self._maxPlatoonGap) if self._useHeadway else self._maxPlatoonGap  # noqa
                if leaderInfo is None or not self._isConnected(leaderInfo[0]) or leaderInfo[1] > maxGap:
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
                        if rp.VERBOSITY >= 2:
                            report(("Platoon order for platoon '%s' is violated: real leader '%s' is not registered " +
                                    "as leader of '%s'") % (
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
                    if rp.VERBOSITY >= 2:
                        report("Platoon '%s' splits (ID of new platoon: '%s'):\n" % (pltn.getID(), newPlatoon.getID()) +
                               "    Platoon '%s': %s\n    Platoon '%s': %s" % (
                            pltn.getID(), str([veh.getID() for veh in pltn.getVehicles()]),
                            newPlatoon.getID(), str([veh.getID() for veh in newPlatoon.getVehicles()])))
        for pltn in newPlatoons:
            self._platoons[pltn.getID()] = pltn

    def _manageLeaders(self):
        '''_manageLeaders()

        Iterates over platoon-leaders and
        1) checks whether two platoons (including "one-vehicle platoons") may merge for being sufficiently close
        2) deters catchup vehicles from joining as the max platoon size is reached
        3) advises platoon-leaders to try to catch up with a platoon in front
        '''
        # list of platoon ids that merged into another platoon
        toRemove = []
        for pltnID, pltn in self._platoons.items():
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
            distThreshold = self._catchupHeadway*pltnLeader.state.speed if self._useHeadway else self._catchupDist
            if leaderInfo is None or leaderInfo[1] > distThreshold:
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

            if (pltnLeader.getCurrentPlatoonMode() == PlatoonMode.CATCHUP and
                    leader.getPlatoon().size() + pltn.size() > cfg.MAX_VEHICLES):
                if pltn.size() == 1:
                    pltn.setModeWithImpatience(PlatoonMode.NONE, self._controlInterval)
                else:
                    # try to set mode to regular platoon mode
                    pltn.setModeWithImpatience(PlatoonMode.LEADER, self._controlInterval)
                continue

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

            maxDist = self._maxPlatoonGap
            if self._useHeadway:
                maxDist = max(self._maxPlatoonHeadway * leader.state.speed, maxDist)
            if leaderDist <= maxDist:
                # Try to join the platoon in front
                if leader.getPlatoon().join(pltn):
                    toRemove.append(pltnID)
                    # Debug
                    if rp.VERBOSITY >= 2:
                        report("Platoon '%s' joined Platoon '%s', which now contains " % (pltn.getID(),
                                                                                          leader.getPlatoon().getID()) +
                               "vehicles:\n%s" % str([veh.getID() for veh in leader.getPlatoon().getVehicles()]))
                    continue
                else:
                    if rp.VERBOSITY >= 3:
                        report("Merging of platoons '%s' (%s) and '%s' (%s) would not be safe." %
                               (pltn.getID(), str([veh.getID() for veh in pltn.getVehicles()]),
                                leader.getPlatoon().getID(),
                                str([veh.getID() for veh in leader.getPlatoon().getVehicles()])))
            else:
                # Join failed due to too large distance. Try to get closer (change to CATCHUP mode).
                if (leader.getPlatoon().size() + pltn.size() <= cfg.MAX_VEHICLES and
                        not pltn.setMode(PlatoonMode.CATCHUP)):
                    if rp.VERBOSITY >= 3:
                        report(("Switch to catchup mode would not be safe for platoon '%s' (%s) chasing " +
                                "platoon '%s' (%s).") %
                               (pltn.getID(), str([veh.getID() for veh in pltn.getVehicles()]),
                                leader.getPlatoon().getID(),
                                str([veh.getID() for veh in leader.getPlatoon().getVehicles()])))

        # remove merged platoons
        for pltnID in toRemove:
            self._platoons.pop(pltnID)

    def _updatePlatoonOrdering(self):
        '''_manageLeaders()

        Iterates through platoons and checks whether they are in an appropriate order.
        '''
        if rp.VERBOSITY >= 4:
            report("Checking platoon ordering")

        for pltnID, pltn in self._platoons.items():
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

                if rp.VERBOSITY >= 4:
                    report("Platoon %s: Leader for veh '%s' is '%s' (%s)"
                           % (pltn.getID(), veh.getID(), str(leaderID),
                              ("same platoon" if (intraPlatoonLeaders[-1] is not None) else "not from same platoon")),
                           3)

            pltn.setVehicles(self.reorderVehicles(pltn.getVehicles(), intraPlatoonLeaders))

    @staticmethod
    def reorderVehicles(vehicles, actualLeaders):
        '''
        reorderVehicles(list(pVehicle), list(pVehicle)) -> list(pVehicle)

        This method reorders the given vehicles such that the newly ordered vehicles fulfill:
        [None] + vehicles[:-1] == actualLeaders (if not several vehicles have the same actual leader.
        For those it is only guaranteed that one will be associated correctly, not specifying which one)
        '''

#         if rp.VERBOSITY >= 4:
#             report("reorderVehicles(vehicles, actualLeaders)\nvehicles = %s\nactualLeaders=%s" %
#                    (rp.array2String(vehicles), rp.array2String(actualLeaders)), 3)

        done = False
        # this is needed as abort criterion if two have the same leader (This cannot be excluded for sublane at least)
        iter_count = 0
        nVeh = len(vehicles)
        # make a copy to write into
        actualLeaders = list(actualLeaders)
        while (not done and iter_count < nVeh):
            newVehOrder = None
            registeredLeaders = [None] + vehicles[:-1]
            if rp.VERBOSITY >= 4:
                report("vehicles: %s" % rp.array2String(vehicles), 3)
                report("Actual leaders: %s" % rp.array2String(actualLeaders), 3)
                report("registered leaders: %s" % rp.array2String(registeredLeaders), 3)
            for (ego, registeredLeader, actualLeader) in reversed(
                    list(zip(vehicles, registeredLeaders, actualLeaders))):
                if (ego == actualLeader):
                    if rp.VERBOSITY >= 1:
                        warn(("Platoon %s:\nVehicle '%s' was found as its own leader. " +
                              "Platoon order might be corrupted.") % (
                             rp.array2String(vehicles), str(ego)))
                    return vehicles

                if actualLeader is None:
                    # No actual leader was found. Could be due to platoon LC maneuver or non-connected vehicle
                    # merging in
                    # -> no reordering implications.
                    continue
                if actualLeader == registeredLeader:
                    continue

                # intra-platoon order is corrupted
                if newVehOrder is None:
                    # init newVehOrder
                    newVehOrder = list(vehicles)

                # relocate ego

                if rp.VERBOSITY >= 4:
                    report("relocating: %s to follow %s" % (str(ego), str(actualLeader)), 3)
                    report("prior newVehOrder: %s" % rp.array2String(newVehOrder), 3)
                    report("prior actualLeaders: %s" % rp.array2String(actualLeaders), 3)
                oldEgoIndex = newVehOrder.index(ego)
                del newVehOrder[oldEgoIndex]
                del actualLeaders[oldEgoIndex]

                if rp.VERBOSITY >= 4:
                    report("immed newVehOrder: %s" % rp.array2String(newVehOrder), 3)
                    report("immed actualLeaders: %s" % rp.array2String(actualLeaders), 3)
                actualLeaderIndex = newVehOrder.index(actualLeader)
                newVehOrder.insert(actualLeaderIndex + 1, ego)
                actualLeaders.insert(actualLeaderIndex + 1, actualLeader)
                if rp.VERBOSITY >= 4:
                    report("current newVehOrder: %s" % rp.array2String(newVehOrder), 3)
                    report("current actualLeaders: %s" % rp.array2String(actualLeaders), 3)

            done = newVehOrder is None
            if not done:
                vehicles = newVehOrder
                iter_count += 1

        if iter_count != 0:
            if rp.VERBOSITY >= 3:
                report("Ordering within Platoon %s was corrupted.\nNew Order: %s\nLeaders: %s" %
                       (vehicles[0].getPlatoon().getID(), rp.array2String(vehicles), rp.array2String(actualLeaders)), 3)

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
                    # distance between vehicle and end of edge
                    d = traci.lane.getLength(laneID)-traci.vehicle.getLanePosition(veh.getID())
                    # change lanes  only if greater than minimum distance from end of edge
                    # veh.state.edgeID not in rounaboutEdges:#traci.lane.getLength(laneID)>50.0:
                    if d > self._lanechangeMinDist:
                        # leader is on the same edge, advise follower to use the same lane
                        try:
                            ix = leader.state.laneIX
                            if ix >= 0:
                                traci.vehicle.changeLane(veh.getID(), ix, self._controlInterval)
                        except traci.exceptions.TraCIException as e:
                            if rp.VERBOSITY >= 1:
                                warn("Lanechange advice for vehicle'%s' failed. Message:\n%s" % (veh.getID(), str(e)))
                else:
                    # leader is on another edge, just stay on the current and hope it is the right one
                    try:
                        # distance between vehicle and end of edge
                        d = traci.lane.getLength(laneID)-traci.vehicle.getLanePosition(veh.getID())
                        # change lanes  only if greater than minimum distance from end of edge
                        if d > self._lanechangeMinDist:
                            traci.vehicle.changeLane(veh.getID(), veh.state.laneIX, self._controlInterval)

                    except traci.exceptions.TraCIException as e:
                        if rp.VERBOSITY >= 1:
                            warn("Lanechange advice for vehicle'%s' failed. Message:\n%s" % (veh.getID(), str(e)))

    def _isConnected(self, vehID):
        '''_isConnected(string) -> bool

        Returns whether the given vehicle is a potential platooning participant
        '''
        if vehID in self._connectedVehicles:
            return True
        else:
            return False

    def _hasConnectedType(self, vehID):
        '''_hasConnectedType(string) -> bool

        Determines whether the given vehicle should be connected to the platoon manager
        by comparing its vType with the type selector substrings specified in vehicleSelectors.
        '''
        vtype = traci.vehicle.getTypeID(vehID)
        for selector_str in self._typeSubstrings:
            if selector_str in vtype:
                return True
        return False
