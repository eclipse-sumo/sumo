# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    _utils.py
# @author Leonhard Luecken
# @date   2018-06-26
# @version $Id$

'''
Utility functions and classes for simpla
'''

import traci

class GapCreator(traci.StepListener):
    '''
    class that manages opening a gap in front of its assigned vehicle 
    '''
    
    def __init__(self, vehID, desiredGap, desiredSpeedDiff, maximumDecel, duration):
        ''' 
        GapCreator()
        
        @see createGap()
        '''
        self._desiredGap = desiredGap
        self._desiredSpeedDiff = desiredSpeedDiff
        self._maximumDecel = maximumDecel
        self._duration = duration
        self._vehID = vehID

        # step-length of simulation
        self._TS = traci.simulation.getDeltaT() / 1000.
        

    def step(self):
        '''
        Perform one control step and count down the activity period for the controller 
        '''
        self._applyControl()
        
        self._duration -= self._TS
        
        return self._duration > 0
    
        
    def _applyControl(self):
        (leaderID, gap) = traci.vehicle.getLeader(self._vehID, self._desiredGap)
        if gap < 0 or gap >= self._desiredGap:
            # no leader or farther away than the desired gap
            return        
        assert(leaderID is not "");
        
        egoSpeed = traci.vehicle.getSpeed(self._vehID)
        leaderSpeed = traci.vehicle.getSpeed(leaderID)
        speedDiff = leaderSpeed - egoSpeed
        
        if speedDiff >= self._desiredSpeedDiff:
            # don't increase speedDiff above the desired
            return
        
        # calculate desired deceleration assuming the leader kept its current speed
        decel = min(self._maximumDecel, (self._desiredSpeedDiff - speedDiff)/self._TS)
        
        # apply the deceleration
        traci.vehicle.setSpeed(self._vehID, egoSpeed - self._TS*decel)
    
        
def openGap(vehID, desiredGap, desiredSpeedDiff, maximumDecel, duration):
    '''
    openGap(string, float>0, float>0, float>0, float>0)

    vehID - ID of the vehicle to be controlled
    desiredGap - gap that shall be established 
    desiredSpeedDiff - rate at which the gap is open if possible
    maximumDecel - maximal deceleration at which the desiredSpeedDiff is tried to be approximated
    duration - The period for which the gap control should be active

    This methods adds a controller for the opening of a gap in front of the given vehicle.
    The controller stays active for a period of the given duration.
    If a leader is closer than the desiredGap, the controller tries to establish the desiredGap by inducing the 
    given speedDifference, while not braking harder than maximumDecel.
    An object of the class GapCreator is created to manage the vehicle state and is added to traci as a stepListener.  
    '''    
    gc = GapCreator(vehID, desiredGap, desiredSpeedDiff, maximumDecel, duration)
    traci.addStepListener(gc)
    
    
    