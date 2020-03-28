#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Leonhard Luecken
# @date    2017

import unittest as ut
import os
import sys
import subprocess

# Put tools into PYTHONPATH
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci  # noqa
import simpla  # noqa
import sumolib  # noqa
import simpla._reporting as rp  # noqa
import simpla._config as cfg  # noqa
from simpla._platoonmode import PlatoonMode  # noqa
from functools import reduce  # noqa


class TestPlatoonManager(ut.TestCase):

    def setUp(self):
        print("\n")
        ut.TestCase.setUp(self)
        testDir = os.path.dirname(os.path.realpath(__file__))
        # Declare simpla config files
        self.CFG0 = 'simpla.cfg.xml'
        self.CFG1 = 'simpla_test.cfg.xml'

        self.SIMPLA_CFG = os.path.join(testDir, "simpla.cfg.xml")
        self.SIMPLA_CFG_WARN = os.path.join(testDir, "simpla_test_warn.cfg.xml")
        self.SIMPLA_CFG_VTYPEMAP = os.path.join(testDir, "simpla_test_vtypemap.cfg.xml")

        # define config contents
        self.cfg_body0 =\
            """
                <vTypeMapFile value="vtype.map" />
                <controlRate value="10." />
                <vehicleSelectors value="connected" />
                <maxPlatoonGap value="15.0" />
                <catchupDist value="50.0" />
                <switchImpatienceFactor value="0.1" />
                <platoonSplitTime value="3.0" />
                <lcMode original="597" leader="597" follower="514" catchup="514" catchupFollower="514" />
                <speedFactor original="1.01" leader="1.01" follower="1.11" catchup="1.21" catchupFollower="1.31" >\
</speedFactor>
                <verbosity value="2" />
            """

        self.cfg_body1 =\
            """
                <vTypeMapFile file="vtype.map"/>
                <controlRate value="1000." />
                <vehicleSelectors value="connected" />
                <maxPlatoonGap value="15.0" />
                <catchupDist value="50.0" />
                <switchImpatienceFactor value="0.1" />
                <platoonSplitTime value="3.0" />
                <lcMode original="597" leader="597" follower="514" catchup="514" catchupFollower="514" />
                <speedFactor original="1.01" leader="1.01" follower="1.11" catchup="1.21" catchupFollower="1.31" >\
</speedFactor>
                <verbosity value="2" />
            """

        self.cfg_body2 =\
            """
                <vTypeMap original="unknownVTypeID" leader="leaderVTypeID" follower="followerVTypeID" \
catchup="catchupVTypeID" catchupFollower="catchupFollowerVTypeID" />
            """

        self.cfg_body3 =\
            """
                <controlRate value="10." />
                <vehicleSelectors value="connected" />
                <maxPlatoonGap value="15.0" />
                <catchupDist value="50.0" />
                <switchImpatienceFactor value="0.1" />
                <platoonSplitTime value="3.0" />
                <lcMode original="597" leader="597" follower="514" catchup="514" catchupFollower="514" />
                <speedFactor original="1.01" leader="1.01" follower="1.11" catchup="1.21" catchupFollower="1.31" >\
</speedFactor>
                <verbosity value="2" />
                <vTypeMap original="connected" leader="connected_pLeader" follower="connected_pFollower" \
catchup="connected_pCatchup" catchupFollower="connected_pCatchupFollower" />
            """
        # template still needs to insert definite values for placeholders
        self.SUMO_CFG_TEMPLATE = os.path.join(testDir, "sumo.sumocfg")
        self.SUMO_CFG = os.path.join(testDir, "test.sumocfg")
        self.patchSumoConfig()  # write default routes file and net file into config

    def connectToSumo(self, sumo_cfg):
        # Set up a running sumo instance
        SUMO_BINARY = sumolib.checkBinary('sumo')
        PORT = sumolib.miscutils.getFreeSocketPort()
        sumoCall = "%s -c %s --remote-port %s" % (SUMO_BINARY, sumo_cfg, PORT)
        # print("sumoCall = '%s'"%sumoCall)
        self.SUMO_PROCESS = subprocess.Popen(sumoCall, shell=True, stdout=sys.stdout)
        # Connect
        traci.init(PORT, numRetries=2)

    def patchSumoConfig(self, routes_fn="input_routes.rou.xml", net_fn="input_net.net.xml",
                        vtypes_fn="input_types.typ.xml"):
        # replace routes_fn in config template
        with open(self.SUMO_CFG_TEMPLATE, "r") as cfg_template, open(self.SUMO_CFG, "w") as cfg:
            template_str = cfg_template.read()
            cfg_str = template_str.format(routes_file=routes_fn, net_file=net_fn, vtypes_file=vtypes_fn)
            cfg.write(cfg_str)

    def patchConfigFile(self, cfg_body):
        # patch simpla config
        with open(self.CFG0, "r") as empty_cfg, open(self.CFG1, "w") as target_cfg:
            s = empty_cfg.read()
            # print(s.format(body=cfg_body))
            target_cfg.write(s.format(body=cfg_body))

    def tearDown(self):
        ut.TestCase.tearDown(self)
        # stop simpla
        simpla.stop()
        # close TraCI connection
        traci.close()
        # wait for sumo to terminate
        self.SUMO_PROCESS.wait()
        # reset simpla
        rp.initDefaults()
        cfg.initDefaults()
        if os.path.exists(self.SUMO_CFG):
            os.remove(self.SUMO_CFG)

    def test_init(self):
        print("Testing platoon manager initialization...")
        self.patchSumoConfig()
        self.connectToSumo(self.SUMO_CFG)
        self.patchConfigFile(self.cfg_body0)
        simpla.load(self.CFG1)
        self.assertListEqual([r for t, r in rp.WARNING_LOG], [])

        expectedVTypes = ["connected", "connected_pLeader", "connected_pFollower",
                          "connected_pCatchup", "connected_pCatchupFollower"]
        registeredPlatoonVTypes = list(
            set(reduce(lambda x, y: x + y, [[orig] + list(mapped.values())
                                            for orig, mapped in cfg.PLATOON_VTYPES.items()])))
        expectedVTypes.sort()
        registeredPlatoonVTypes.sort()
        self.assertListEqual(expectedVTypes, registeredPlatoonVTypes)

    def test_init_vtypemap(self):
        print("Testing specification per vtypemap xml-element...")
        self.patchSumoConfig()
        self.connectToSumo(self.SUMO_CFG)
        self.patchConfigFile(self.cfg_body3)
        simpla.load(self.CFG1)
#         simpla.load(self.SIMPLA_CFG_VTYPEMAP)
        self.assertListEqual([r for t, r in rp.WARNING_LOG], [])

        expectedVTypes = ["connected", "connected_pLeader", "connected_pFollower",
                          "connected_pCatchup", "connected_pCatchupFollower"]
        registeredPlatoonVTypes = list(
            set(reduce(lambda x, y: x + y, [[orig] + list(mapped.values())
                                            for orig, mapped in cfg.PLATOON_VTYPES.items()])))
        expectedVTypes.sort()
        registeredPlatoonVTypes.sort()
        self.assertListEqual(expectedVTypes, registeredPlatoonVTypes)

    def test_init_warn(self):
        print("Testing Warnings...")
        self.patchSumoConfig(vtypes_fn="input_types2.typ.xml")
        self.connectToSumo(self.SUMO_CFG)
        # print (self.cfg_body1)
        self.patchConfigFile(self.cfg_body1)
        simpla.load(self.CFG1)
        # simpla.load(self.SIMPLA_CFG_WARN)
        expected_warnings = [
            "WARNING: Restricting given control rate (= 1000 per sec.) to 1 per timestep (= 10 per sec.) " +
            "(PlatoonManager)",
            "WARNING: emergencyDecel of mapped vType 'connected_pCatchupFollower' (10.5m.) does not equal " +
            "emergencyDecel of original vType 'connected' (4.5m.) (PlatoonManager)",
            "WARNING: emergencyDecel of mapped vType 'connected_pFollower' (1.7m.) does not equal emergencyDecel " +
            "of original vType 'connected' (4.5m.) (PlatoonManager)",
            "WARNING: emergencyDecel of mapped vType 'connected_pCatchup' (0.5m.) does not equal emergencyDecel of " +
            "original vType 'connected' (4.5m.) (PlatoonManager)",
            "WARNING: length of mapped vType 'connected_pLeader' (10.0m.) does not equal length of original vType " +
            "'connected' (5.0m.)\nThis will probably lead to collisions. (PlatoonManager)",
            "WARNING: length of mapped vType 'connected_pCatchupFollower' (3.0m.) does not equal length of original " +
            "vType 'connected' (5.0m.)\nThis will probably lead to collisions. (PlatoonManager)",
        ]
        warnings_list = [r for t, r in rp.WARNING_LOG]
        # for w in warnings_list:
        #    print(w)
        for w in expected_warnings:
            self.assertIn(w, warnings_list)
        self.assertListEqual([], list(set(warnings_list).difference(expected_warnings)))
        traci.simulationStep(1.)
        self.assertEqual(rp.WARNING_LOG[-1][0], "1.0")
        self.assertEqual(
            rp.WARNING_LOG[-1][1], "WARNING: Step lengths that differ from SUMO's simulation step length are not " +
            "supported and probably lead to undesired behavior.\nConsider decreasing simpla's control rate instead. " +
            "(PlatoonManager)")

    def test_unknown_vtypes(self):
        print("Testing Exceptions for unknown vTypes...")
        self.patchSumoConfig(vtypes_fn="input_types2.typ.xml")
        self.connectToSumo(self.SUMO_CFG)
        # print (self.cfg_body1)
        self.patchConfigFile(self.cfg_body2)
        try:
            simpla.load(self.CFG1)
            self.assertTrue(False, "PlatoonManager() should raise an exception in case of unknown vtypes")
        except simpla.SimplaException as e:
            self.assertEqual(
                str(e), "vType 'unknownVTypeID' is unknown to sumo! Note: Platooning vTypes must be defined at " +
                "startup.")

    def test_add_and_remove(self):
        print("Testing adding and removing connected vehicles...")
        self.patchSumoConfig()
        self.connectToSumo(self.SUMO_CFG)

        self.patchConfigFile(self.cfg_body0)
        simpla.load(self.CFG1)
#         simpla.load(self.SIMPLA_CFG)
        mgr = simpla._mgr

#         # load simpla without adding a step listener
#         simpla._config.load(self.SIMPLA_CFG)
#         simpla._mgr = simpla._platoonmanager.PlatoonManager()

        self.assertTupleEqual((), traci.vehicle.getIDList())

        traci.simulationStep()
        self.assertTupleEqual(('connected.1',), traci.vehicle.getIDList())
        self.assertListEqual(['connected.1'], [vehID for vehID in mgr._connectedVehicles.keys()])

        self.assertEqual(rp.REPORT_LOG[-1][0], "0.1")
        self.assertEqual(rp.REPORT_LOG[-1][1], "Adding vehicle 'connected.1' (PlatoonManager)")

        while traci.simulation.getTime() < 2:
            traci.simulationStep()

        self.assertListEqual(list(sorted(['connected.1', 'conventional.1'])), list(sorted(traci.vehicle.getIDList())))
        self.assertListEqual(['connected.1'], [vehID for vehID in mgr._connectedVehicles.keys()])

        while traci.simulation.getTime() <= 5:
            traci.simulationStep()

        self.assertEqual(rp.REPORT_LOG[-1][0], "3.1")
        self.assertEqual(rp.REPORT_LOG[-1][1], "Adding vehicle 'IAMconnectedTOO' (PlatoonManager)")

        self.assertTrue(mgr._hasConnectedType("IAMconnectedTOO"))
        self.assertListEqual(list(sorted(['connected.1', 'conventional.1', 'IAMconnectedTOO'])), list(
            sorted(traci.vehicle.getIDList())))
        self.assertListEqual(list(sorted(['connected.1', 'IAMconnectedTOO'])), list(
            sorted([vehID for vehID in mgr._connectedVehicles.keys()])))

        while traci.simulation.getTime() <= 14:
            traci.simulationStep()

        self.assertEqual(rp.REPORT_LOG[-1][0], "11.4")
        self.assertEqual(
            rp.REPORT_LOG[-1][1], "Platoon '1' joined Platoon '0', which now contains vehicles:\n['connected.1', " +
            "'IAMconnectedTOO'] (PlatoonManager)")

        while traci.simulation.getTime() <= 17:
            traci.simulationStep()

        self.assertEqual(rp.REPORT_LOG[-1][0], "16.0")
        self.assertEqual(
            rp.REPORT_LOG[-1][1], "Platoon '0' splits (ID of new platoon: '2'):\n    Platoon '0': ['connected.1']\n" +
            "    Platoon '2': ['IAMconnectedTOO'] (PlatoonManager)")

        while traci.simulation.getTime() <= 18:
            traci.simulationStep()

        self.assertEqual(rp.REPORT_LOG[-1][0], "17.3")
        self.assertEqual(rp.REPORT_LOG[-1][1], "Removing arrived vehicle 'connected.1' (PlatoonManager)")
        self.assertListEqual(list(sorted(['conventional.1', 'IAMconnectedTOO'])),
                             list(sorted(traci.vehicle.getIDList())))
        self.assertListEqual(['IAMconnectedTOO'], [vehID for vehID in mgr._connectedVehicles.keys()])

        while traci.simulation.getTime() <= 20:
            traci.simulationStep()

        self.assertEqual(rp.REPORT_LOG[-1][0], "19.8")
        self.assertEqual(rp.REPORT_LOG[-1][1], "Removing arrived vehicle 'IAMconnectedTOO' (PlatoonManager)")

    def test_platoon_formation(self):
        print("Testing platoon formation...")
        self.patchSumoConfig(net_fn="input_net2.net.xml", routes_fn="input_routes2.rou.xml")
        self.connectToSumo(self.SUMO_CFG)

        self.patchConfigFile(self.cfg_body0)
        simpla.load(self.CFG1)
#         simpla.load(self.SIMPLA_CFG)
        mgr = simpla._mgr

        while traci.simulation.getTime() <= 5:
            traci.simulationStep()

        self.assertIn("connected.1", mgr._connectedVehicles)
        self.assertIn("connected.2", mgr._connectedVehicles)
        veh1 = mgr._connectedVehicles["connected.1"]
        self.assertEqual(veh1.getCurrentPlatoonMode(), PlatoonMode.NONE)
        veh2 = mgr._connectedVehicles["connected.2"]
        self.assertEqual(veh2.getCurrentPlatoonMode(), PlatoonMode.CATCHUP)

        while traci.simulation.getTime() <= 20:
            traci.simulationStep()

#         self.assertEqual(rp.REPORT_LOG[-1][0], "13.7")
#         self.assertEqual(rp.REPORT_LOG[-1][1], "Platoon '1' joined Platoon '0', which now contains " +
#                          "vehicles:\n['connected.1', 'connected.2'] (PlatoonManager)")

        self.assertEqual(veh1.getCurrentPlatoonMode(), PlatoonMode.LEADER)
        self.assertEqual(veh2.getCurrentPlatoonMode(), PlatoonMode.FOLLOWER)

        vehs = mgr.getPlatoonLeaders()[0].getPlatoon().getVehicles()
        vehIDs = [v.getID() for v in vehs]
        self.assertEqual(len(vehIDs), 2)
        self.assertIn("connected.1", vehIDs)
        self.assertIn("connected.2", vehIDs)

        while traci.vehicle.getLaneID(vehIDs[0]) == traci.vehicle.getLaneID(vehIDs[1]):
            traci.simulationStep()

        self.assertFalse(veh1.state.laneID == veh2.state.laneID)
        t0 = traci.simulation.getTime()
        self.assertEqual(t0, 71.4)

        expected_split_time = t0 + mgr._DeltaT + cfg.PLATOON_SPLIT_TIME

        while traci.simulation.getTime() <= expected_split_time:
            traci.simulationStep()

        self.assertAlmostEqual(veh2._timeUntilSplit, 0.0, 9)

        while traci.simulation.getTime() <= expected_split_time + 1.:
            traci.simulationStep()

        self.assertEqual(
            rp.REPORT_LOG[-1][1], "Platoon '0' splits (ID of new platoon: '6'):\n    Platoon '0': ['connected.1']\n" +
            "    Platoon '6': ['connected.2'] (PlatoonManager)")


# # ignore some tests for faster execution
# ignore_tests = [
#     "test_init",
#     "test_init_vtypemap",
# #     "test_init_warn",
#     "test_add_and_remove",
#     "test_platoon_formation",
#     ]
#
# for t in ignore_tests:
#     delattr(TestPlatoonManager,t)


if __name__ == "__main__":
    if sys.version.startswith("3"):
        ut.main(warnings="ignore")
    else:
        ut.main()
