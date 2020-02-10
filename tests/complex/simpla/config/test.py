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
sumoHome = os.environ.get("SUMO_HOME", os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..')))
sys.path.append(os.path.join(sumoHome, "tools"))

import traci  # noqa
import sumolib  # noqa
import simpla  # noqa
import simpla._config as cfg  # noqa
import simpla._reporting as rp  # noqa
from simpla import SimplaException  # noqa
from simpla._platoonmode import PlatoonMode  # noqa


class TestConfig(ut.TestCase):

    def setUp(self):
        ut.TestCase.setUp(self)
        self.testDir = os.path.dirname(os.path.realpath(__file__))
        # set config filenames
        self.CFG0 = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'simpla.cfg.xml')
        self.CFG1 = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'simpla_test.cfg.xml')

        # define config contents
        self.cfg_body0 =\
            """
                <vTypeMapFile value="vtype.map" />
                <controlRate value="10." />
                <vehicleSelectors value="pkwFern1,pkwFern2" />
                <maxPlatoonGap value="15.0" />
                <catchupDist value="50.0" />
                <switchImpatienceFactor value="0.1" />
                <platoonSplitTime value="3.0" />
                <lcMode original="597" leader="597" follower="514" catchup="514" catchupFollower="514" />
                <speedFactor original="1.01" leader="1.01" follower="1.11" catchup="1.21" catchupFollower="1.31" >\
</speedFactor>
                <verbosity value="2" />
                <vTypeMap original="origVTypeID" leader="leaderVTypeID" follower="followerVTypeID" \
catchup="catchupVTypeID" catchupFollower="catchupFollowerVTypeID" />
            """
        self.cfg_body1 =\
            """
                <controlRate value="0" />
                <vehicleSelectors>empty</vehicleSelectors>
                <maxPlatoonGap value="-1" />
                <catchupDist value="-1" />
                <switchImpatienceFactor value="-1" />
                <platoonSplitTime value="-1" ></platoonSplitTime>
                <lcMode leader="100000" follower="-1" catchupFollower="0" />
                <speedFactor original="-1" leader="0" follower="0.5" catchup="-4"/>
                <vTypeMap leader="leaderVTypeID" follower="followerVTypeID" catchup="catchupVTypeID" \
catchupFollower="catchupFollowerVTypeID" /><verbosity value="200" ></verbosity>
                <UnknownConfigParam></UnknownConfigParam>
            """
        self.cfg_body2 = '<vTypeMapFile file="vtype2.map"></vTypeMapFile>'
        self.cfg_body3 = '<vTypeMapFile file="vtype3.map"></vTypeMapFile>'
        self.cfg_body4 = '<vTypeMapFile file="FileThatDoesntExist"></vTypeMapFile>'
        self.cfg_body5 = '<vTypeMap original="original_type1" leader="leader_type1" follower="follower_type1" ' + \
                         'catchup ="catchup_type1" catchupFollower ="catchupFollower_type1"/>'

        # start a sumo instance
        self.sumocfg = os.path.join(self.testDir, "sumo.sumocfg")
        self.connectToSumo(self.sumocfg)

    def connectToSumo(self, sumo_cfg):
        # Set up a running sumo instance
        SUMO_BINARY = sumolib.checkBinary('sumo')
        PORT = sumolib.miscutils.getFreeSocketPort()
        # print("PORT=",PORT)
        sumoCall = [SUMO_BINARY, "-c", sumo_cfg, "--remote-port", str(PORT), "-S"]
        # print("sumoCall = '%s'"%sumoCall)
        self.SUMO_PROCESS = subprocess.Popen(sumoCall)
        # Connect
        traci.init(PORT, numRetries=2)

    def tearDown(self):
        ut.TestCase.tearDown(self)
        # clear loaded variables
        cfg.initDefaults()
        rp.initDefaults()
        os.remove(self.CFG1)
        traci.close()
        self.SUMO_PROCESS.wait()
        # print("tearDown() done.")

    def patchConfigFile(self, cfg_body):
        with open(self.CFG0, "r") as empty_cfg, open(self.CFG1, "w") as target_cfg:
            s = empty_cfg.read()
            target_cfg.write(s.format(body=cfg_body))

    def test_empty_config(self):
        print("Testing empty config...")
        self.patchConfigFile("")
        cfg.load(self.CFG1)
        self.assertEqual(rp.VERBOSITY, 1.)
        self.assertEqual(cfg.CONTROL_RATE, 1.)
        self.assertEqual(cfg.VEH_SELECTORS, [''])
        self.assertEqual(cfg.MAX_PLATOON_GAP, 15.)
        self.assertEqual(cfg.CATCHUP_DIST, 50.)
        self.assertEqual(cfg.PLATOON_SPLIT_TIME, 3.)
        self.assertEqual(cfg.SWITCH_IMPATIENCE_FACTOR, 0.1)
        self.assertEqual(cfg.VTYPE_FILE, '')
        for mode in PlatoonMode:
            self.assertTrue(mode in cfg.LC_MODE)
            self.assertTrue(mode in cfg.SPEEDFACTOR)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.NONE], 0b1001010101)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.LEADER], 0b1001010101)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.FOLLOWER], 0b1000000010)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.CATCHUP], 0b1000000010)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.CATCHUP_FOLLOWER], 0b1000000010)
        for tp in cfg.PLATOON_VTYPES:
            for mode in PlatoonMode:
                self.assertTrue(mode in cfg.PLATOON_VTYPES[tp])
        self.assertListEqual(list(rp.WARNING_LOG), [])

    def test_example_config(self):
        print("Testing standard config...")
        self.patchConfigFile(self.cfg_body0)
        cfg.load(self.CFG1)
        self.assertEqual(rp.VERBOSITY, 2.)
        self.assertEqual(cfg.CONTROL_RATE, 10.)
        self.assertEqual(cfg.VEH_SELECTORS, ['pkwFern1', 'pkwFern2'])
        self.assertEqual(cfg.MAX_PLATOON_GAP, 15.)
        self.assertEqual(cfg.CATCHUP_DIST, 50.)
        self.assertEqual(cfg.PLATOON_SPLIT_TIME, 3.)
        self.assertEqual(cfg.SWITCH_IMPATIENCE_FACTOR, 0.1)
        self.assertTrue(cfg.VTYPE_FILE.endswith('vtype.map'))
        for mode in PlatoonMode:
            self.assertTrue(mode in cfg.LC_MODE)
            self.assertTrue(mode in cfg.SPEEDFACTOR)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.NONE], 0b1001010101)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.LEADER], 0b1001010101)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.FOLLOWER], 0b1000000010)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.CATCHUP], 0b1000000010)
        self.assertEqual(cfg.LC_MODE[PlatoonMode.CATCHUP_FOLLOWER], 0b1000000010)
        for tp in cfg.PLATOON_VTYPES:
            for mode in PlatoonMode:
                self.assertTrue(mode in cfg.PLATOON_VTYPES[tp])
        self.assertListEqual(list(rp.WARNING_LOG), [])

    def test_config_warnings(self):
        print("Testing config warnings...")
        self.patchConfigFile(self.cfg_body1)
        cfg.load(self.CFG1)
        # cfg.load(TestConfig.CFG2)
        expected_warnings = [
            "WARNING: No attributes found for tag 'vehicleSelectors'. (Config)",
            "WARNING: Encountered unknown configuration parameter 'UnknownConfigParam'! (Config)",
            "WARNING: Verbosity must be one of [0, 1, 2, 3, 4]! Ignoring given value: 200 (Config)",
            "WARNING: Parameter controlRate must be positive. Ignoring given value: 0.0 (Config)",
            "WARNING: Parameter maxPlatoonGap must be positive. Ignoring given value: -1.0 (Config)",
            "WARNING: Parameter catchupDist must be positive. Ignoring given value: -1.0 (Config)",
            "WARNING: Parameter switchImpatienceFactor must be non-negative. Ignoring given value: -1.0 (Config)",
            "WARNING: Parameter platoonSplitTime must be non-negative. Ignoring given value: -1.0 (Config)",
            "WARNING: Given lane change mode '100000' lies out of admissible range [0,255]. " +
            "Using default mode instead. (Config)",
            "WARNING: Given lane change mode '-1' lies out of admissible range [0,255]. " +
            "Using default mode instead. (Config)",
            "WARNING: vTypeMap must specify original type. Ignoring malformed vTypeMap element. (Config)",
            "WARNING: Given speedFactor 0.0 is invalid. Using default value. (Config)",
            "WARNING: Given speedFactor -4.0 is invalid. Using default value. (Config)",
            "WARNING: Given speedFactor -1.0 is invalid. Using default value. (Config)"
        ]
        warning_list = [w[1] for w in rp.WARNING_LOG]
        for w in expected_warnings:
            self.assertIn(w, warning_list)
        self.assertListEqual([], list(set(warning_list).difference(expected_warnings)))

    def test_corrupted_vTypeMapFile_raises_exception(self):
        print("Testing malformed vtype map file causing exception...")
        self.patchConfigFile(self.cfg_body2)
        with self.assertRaises(SimplaException):
            cfg.load(self.CFG1)

        self.patchConfigFile(self.cfg_body3)
        with self.assertRaises(SimplaException):
            cfg.load(self.CFG1)

        self.patchConfigFile(self.cfg_body4)
        with self.assertRaises(SimplaException):
            cfg.load(self.CFG1)

    def test_corrupted_vTypeMapFile_exception_text(self):
        print("Testing exception messages...")
        self.patchConfigFile(self.cfg_body4)
        try:
            cfg.load(self.CFG1)
        except SimplaException as e:
            # print (str(e))
            self.assertTrue(str(e).startswith("Given vTypeMapFile"))

        self.patchConfigFile(self.cfg_body3)
        try:
            cfg.load(self.CFG1)
        except SimplaException as e:
            self.assertTrue(str(e).startswith("Platoon leader vType must be specified"))

        self.patchConfigFile(self.cfg_body2)
        try:
            cfg.load(self.CFG1)
        except SimplaException as e:
            self.assertTrue(str(e).startswith("Original vType must be specified"))

    def test_only_vTypeMap_given(self):
        print("Testing specification of vTypeMap only...")
        self.patchConfigFile(self.cfg_body5)
        simpla.load(self.CFG1)
        # print ("controlInterval: %s"%simpla._mgr._controlInterval)
        for i in range(10):
            # print ("Time: %s"%traci.simulation.getCurrentTime())
            # print ("Vehicles: %s"%traci.vehicle.getIDList())
            traci.simulationStep()

# ~ # restrict run to specific tests
# ~ selected_test = 5
# ~ tests = [a for a in dir(TestConfig) if a.startswith("test")]
# ~ print (tests, len(tests))
# ~ for i,t in enumerate(tests):
    # ~ if i != selected_test:
        # ~ print("Removing test %s (%s)"%(i, t))
        # ~ delattr(TestConfig, t)
    # ~ else:
        # ~ print("Keeping test %s (%s)"%(i, t))


if __name__ == "__main__":
    if sys.version.startswith("3"):
        ut.main(warnings="ignore")
    else:
        ut.main()
