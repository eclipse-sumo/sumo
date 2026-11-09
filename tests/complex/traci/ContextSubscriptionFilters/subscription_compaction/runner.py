#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

import os
import sys
import contextlib
import io

sys.path.insert(0, os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib
import traci
from traci import constants as tc

for keep_context in (True, False):
    for position in (0, 2, 4):
        with contextlib.redirect_stdout(io.StringIO()):
            traci.start([sumolib.checkBinary('sumo'), '-n', 'input_net.net.xml', '--no-step-log', '--no-warnings'])
        traci.route.add('route', ['road'])
        traci.vehicletype.copy('DEFAULT_VEHTYPE', 'other')
        for index in range(32):
            traci.vehicle.add('v%s' % index, 'route', typeID='other' if index % 2 == 0 else 'DEFAULT_VEHTYPE',
                              departPos=str(10 + 20 * index), departSpeed='0')
        traci.simulationStep()
        assert traci.vehicle.getIDCount() == 32
        # Retain vector capacity so this case isolates compaction from creation/reallocation.
        for index in range(32):
            traci.vehicle.subscribe('v%s' % index, [tc.VAR_SPEED])
        for index in range(32):
            traci.vehicle.unsubscribe('v%s' % index)
        for index in range(5):
            if index == position:
                traci.vehicle.subscribeContext('v0', tc.CMD_GET_VEHICLE_VARIABLE, 1000., [tc.VAR_SPEED],
                                               end=1000. if keep_context else 2.)
            else:
                traci.vehicle.subscribe('v%s' % (index + 1), [tc.VAR_SPEED], end=2. if keep_context else 1000.)
        # Select the existing context again after ordinary subscriptions deactivate filter additions.
        traci.vehicle.subscribeContext('v0', tc.CMD_GET_VEHICLE_VARIABLE, 1000., [tc.VAR_SPEED],
                                       end=1000. if keep_context else 2.)
        traci.simulationStep()
        assert traci.vehicle.getContextSubscriptionResults('v0')
        traci.simulationStep()
        if keep_context:
            traci.vehicle.addSubscriptionFilterVType(['other'])
            traci.simulationStep()
            result = traci.vehicle.getContextSubscriptionResults('v0')
            assert set(result) == {'v%s' % index for index in range(0, 32, 2)}, result
        else:
            try:
                traci.vehicle.addSubscriptionFilterVType(['other'])
                raise AssertionError('removed context still accepts a filter')
            except traci.TraCIException:
                pass
        traci.close()
        print('context at %s: %s' % (position, 'kept and filtered' if keep_context else 'removed'))
