#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    fcdReplay.py
# @author  Jakob Erdmann
# @date    2023-01-11

"""
Replay an fcd-file as moving POIs on top of a simulation (or empty network)
"""

from __future__ import print_function
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.environ.get(
    "SUMO_HOME", os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')), 'tools'))
import sumolib  # noqa
import traci  # noqa

NULL_OUTPUTS = ("", "NUL", "nul", "/dev/null")


def _extract_cli_option(args, names):
    for i, arg in enumerate(args):
        for name in names:
            if arg == name and i + 1 < len(args):
                return args[i + 1]
            if arg.startswith(name + "="):
                return arg[len(name) + 1:]
    return None


def build_sumo_args(options):
    fcd_files = [f for f in options.fcdFiles.split(',') if f] if options.fcdFiles else []
    input_paths = set(os.path.abspath(f) for f in fcd_files)

    cli_fcd = _extract_cli_option(options.sumo_args, ("--fcd-output",))
    cli_person_fcd = _extract_cli_option(options.sumo_args, ("--person-fcd-output", "--person-fcd"))
    for out_file in (cli_fcd, cli_person_fcd):
        if out_file and out_file not in NULL_OUTPUTS and os.path.abspath(out_file) in input_paths:
            sys.exit("Error: FCD output file '%s' must not overwrite input FCD file." % out_file)

    cfg_file = _extract_cli_option(options.sumo_args, ("-c", "--configuration-file")) or options.sumo_config
    extra_args = []
    if cfg_file and os.path.exists(cfg_file):
        has_fcd = False
        has_person_fcd = False
        for elem in sumolib.xml.parse(cfg_file, ['fcd-output', 'person-fcd-output', 'person-fcd']):
            val = elem.getAttributeSecure("value", "")
            if val not in NULL_OUTPUTS:
                if elem.name == 'fcd-output':
                    has_fcd = True
                else:
                    has_person_fcd = True
        if has_fcd and cli_fcd is None:
            if options.verbose:
                print("Disabling fcd-output from '%s'" % cfg_file)
            extra_args += ["--fcd-output", "NUL"]
        if has_person_fcd and cli_person_fcd is None:
            if options.verbose:
                print("Disabling person-fcd-output from '%s'" % cfg_file)
            extra_args += ["--person-fcd-output", "NUL"]

    if _extract_cli_option(options.sumo_args, ("-c", "--configuration-file")) is not None:
        return options.sumo_args + extra_args
    return ["-c", options.sumo_config] + options.sumo_args + extra_args


def main(args=None):
    parser = sumolib.options.ArgumentParser()
    parser.add_argument("-k", "--sumo-config", category="input", default="sumo.sumocfg", help="sumo config file")
    parser.add_argument("-f", "--fcd-files", category="processing", dest="fcdFiles", help="the fcd files to replay")
    parser.add_argument("--geo",  category="processing", action="store_true", default=False,
                        help="use fcd data in lon,lat format")
    parser.add_argument("-v", "--verbose", category="processing", action="store_true", default=False,
                        help="tell me what you are doing")
    parser.add_argument("sumo_args", nargs="*", catch_all=True, help="additional sumo arguments")
    options = parser.parse_args(args=args)

    sumo_args = build_sumo_args(options)
    sumoBinary = sumolib.checkBinary("sumo-gui")
    traci.start([sumoBinary] + sumo_args)
    t = traci.simulation.getTime()
    deltaT = traci.simulation.getDeltaT()

    fcdData = defaultdict(list)  # time -> objects
    lastTime = {}  # objectID -> last known time
    fcd_files = [f for f in options.fcdFiles.split(',') if f] if options.fcdFiles else []
    for fname in fcd_files:
        if options.verbose:
            print("Loading fcd data from '%s'" % fname)
        for ts in sumolib.xml.parse(fname, 'timestep'):
            time = sumolib.miscutils.parseTime(ts.time)
            if time < t:
                continue
            for obj in ts.getChildList():
                obj.x = float(obj.x)
                obj.y = float(obj.y)
                if options.geo:
                    obj.x, obj.y = traci.simulation.convertGeo(obj.x, obj.y, True)
                fcdData[time].append(obj)
                lastTime[obj.id] = time

    removeAtTime = defaultdict(list)  # time -> objects to remove
    for oID, rt in lastTime.items():
        removeAtTime[rt + deltaT].append(oID)

    end = max(max(lastTime.values()), traci.simulation.getEndTime()) if lastTime else traci.simulation.getEndTime()
    created = set()

    while t <= end:
        for obj in fcdData.get(t, []):
            if obj.id in created:
                traci.poi.setPosition(obj.id, obj.x, obj.y)
            else:
                traci.poi.add(obj.id, obj.x, obj.y, (255, 0, 0, 255), layer=340)
                created.add(obj.id)
            for a, v in obj.getAttributes():
                if a == 'x' or a == 'y' or a == 'id':
                    continue
                traci.poi.setParameter(obj.id, a, v)
        for objID in removeAtTime.get(t, []):
            traci.poi.remove(objID)

        traci.simulationStep()
        t = traci.simulation.getTime()

    traci.close()


if __name__ == "__main__":
    main()
