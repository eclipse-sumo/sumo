# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2017-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    __init__.py
# @author Leonhard Luecken
# @author Mirko Barthauer
# @date   2017-04-09

"""
simpla - A simple platooning plugin for TraCI

simpla is a configurable, simple platooning plugin for TraCI.
A platooning configuration has to be created before using.
Its possible elements are given in the example configuration file
'simpla_example.cfg.xml'

Information about vType mappings between original and
platooning vTypes has to be supplied. This can be done directly
in the configuration xml-file by using 'vTypeMapLeader', 'vTypeMapFollower' and 'vTypeMapCatchup'
elements or by reference to seperate files which define the mappings as
'originalVType : mappedVType'

All specified vTypes should be available within the simulation, the "default" type
is optional and used whenever information is missing for some original type
if no default is specified, the original type remains unchanged within the platoon.

For the definition of platooning vTypes for existing basic vTypes,
and generating vTypeMapping-files see the script generateModifiedVTypes.py.

Usage:
1) import simpla into your traci script.
2) After establishing a connection to SUMO with traci, call simpla.load(<configuration_filename>)
3) Only applies to SUMO version < 0.30: After starting simpla, call simpla.update() after each call to
   traci.simulationStep()
4) Optionally query information about current platoons using functions like simpla.getAveragePlatoonSpeed(),
   simpla.getPlatoonIDList(), ...

Notes:
1) simpla changes the vehicle types, speedfactors, and lane changemodes of all connected vehicles.
   If your application does so as well, this might have unintended consequences.
2) Currently, steps of lengths other than DeltaT are not supported (i.e. if traci.simulationStep()
   is called with argument when simpla is running this may yield undesired behaviour).
3) simpla adds subscriptions to VAR_ROAD_ID, VAR_LANE_INDEX (and currently VAR_LANE_ID) and removes them when stopped
"""

import sys
import os

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci  # noqa
from ._utils import openGap  # noqa
from ._utils import SimplaException  # noqa

import simpla._config  # noqa
import simpla._reporting as rp  # noqa
import simpla._platoonmanager  # noqa

warn = rp.Warner("simpla")
_mgr = None
_mgr_listenerID = None
_mgrs = {}
_mgr_listenerIDs = {}
_useStepListener = 'addStepListener' in dir(traci)
_emergencyDecelImplemented = 'VAR_EMERGENCY_DECEL' in dir(traci.constants)


def _getActiveLabel(connectionLabel=None):
    if connectionLabel is not None:
        return connectionLabel
    try:
        return traci.getLabel()
    except Exception:
        return "default"


def _runWithLabel(label, func, *args):
    prev = None
    try:
        prev = traci.getLabel()
    except Exception:
        prev = None
    switched = prev is not None and label is not None and prev != label
    if switched:
        traci.switch(label)
    try:
        return func(*args)
    finally:
        if switched:
            traci.switch(prev)


class _BoundStepListener(traci.StepListener):
    def __init__(self, label, mgr):
        self._label = label
        self._mgr = mgr

    def step(self, t=0):
        return _runWithLabel(self._label, self._mgr.step, t)


def _getMgr(connectionLabel=None):
    lbl = _getActiveLabel(connectionLabel)
    return _mgrs.get(lbl, _mgr if connectionLabel is None else None)


if not _emergencyDecelImplemented:
    # Old traci version. No emergency decel present.
    if rp.VERBOSITY >= 1:
        warn("Using old traci version assuming emergency decel == decel", True)
    # Set emergency decel to decel
    traci.constants.VAR_EMERGENCY_DECEL = 0x7b
    traci.vehicletype.getEmergencyDecel = traci.vehicletype.getDecel


def load(config_filename, connectionLabel=None):
    '''
    Load the config from file and create a Platoon Manager
    '''
    global _mgr, _mgr_listenerID
    lbl = _getActiveLabel(connectionLabel)

    def _init():
        simpla._config.load(config_filename)
        mgr = simpla._platoonmanager.PlatoonManager()
        lid = traci.addStepListener(_BoundStepListener(lbl, mgr)) if _useStepListener else None
        return mgr, lid

    mgr, lid = _runWithLabel(lbl, _init)
    _mgrs[lbl] = _mgr = mgr
    _mgr_listenerIDs[lbl] = _mgr_listenerID = lid


def stop(connectionLabel=None):
    '''
    Stop the PlatoonManager
    '''
    global _mgr, _mgr_listenerID
    lbl = _getActiveLabel(connectionLabel)
    mgr = _mgrs.pop(lbl, _mgr if connectionLabel is None else None)
    lid = _mgr_listenerIDs.pop(lbl, _mgr_listenerID if connectionLabel is None else None)
    if mgr is not None:
        _runWithLabel(lbl, mgr.stop)
        if lid is not None:
            _runWithLabel(lbl, traci.removeStepListener, lid)
    _mgr = next(reversed(_mgrs.values()), None)


def update(connectionLabel=None):
    '''
    Function called each simulation step. Only to be used for SUMO version < 1.0
    '''
    lbl = _getActiveLabel(connectionLabel)
    mgr = _getMgr(connectionLabel)
    if mgr is not None:
        _runWithLabel(lbl, mgr.step)
    elif rp.VERBOSITY >= 1:
        warn("call simpla.load(<config_file>) before simpla.update()!")


def getAveragePlatoonLength():
    '''
    Get average platoon length from simpla.PlatoonManager
    '''
    global _mgr, warn
    if _mgr is not None:
        return _mgr.getAveragePlatoonLength()
    else:
        raise SimplaException("call simpla.load(<config_file>) before simpla.getAveragePlatoonLength()!")
    return 0


def getAveragePlatoonSpeed():
    '''
    Get average platoon speed from simpla.PlatoonManager
    '''
    global _mgr, warn
    if _mgr is not None:
        return _mgr.getAveragePlatoonSpeed()
    else:
        raise SimplaException("call simpla.load(<config_file>) before simpla.getAveragePlatoonLength()!")
    return 0


def getPlatoonIDList(edgeID):
    '''
    getPlatoonIDList(string) -> list(integer)

    Get the platoon IDs (positive numbers) currently present on the edge given by its ID
    '''
    global _mgr, warn
    if _mgr is not None:
        return _mgr.getPlatoonIDList(edgeID)
    else:
        raise SimplaException("call simpla.load(<config_file>) before simpla.getPlatoonIDList()!")
    return []


def getPlatoonLeaderIDList(connectionLabel=None):
    '''
    getPlatoonLeaderIDList() -> list(string)

    Get the platoon leader vehicle IDs of all platoons managed by simpla
    '''
    mgr = _getMgr(connectionLabel)
    if mgr is not None:
        return mgr.getPlatoonLeaderIDList()
    raise SimplaException("call simpla.load(<config_file>) before simpla.getPlatoonLeaderIDList()!")


def getPlatoonInfo(platoonID):
    '''

    getPlatoonInfo(integer) -> dict()

    Returns a dict with statistical information about the platoon given by its (numerical) ID.
    If the platoon has been dissolved, the dict is empty.
    '''
    global _mgr, warn
    if _mgr is not None:
        return _mgr.getPlatoonInfo(platoonID)
    else:
        raise SimplaException("call simpla.load(<config_file>) before simpla.getPlatoonInfo()!")
    return 0


def getPlatoonID(vehicleID):
    '''

    getPlatoonID(string) -> integer

    Returns the platoon ID the vehicle given by its ID belongs too. If the vehicle does not belong to
    any platoon, -1 is returned.
    '''
    global _mgr, warn
    if _mgr is not None:
        return _mgr.getPlatoonID(vehicleID)
    else:
        raise SimplaException("call simpla.load(<config_file>) before simpla.getPlatoonID()!")
    return 0
