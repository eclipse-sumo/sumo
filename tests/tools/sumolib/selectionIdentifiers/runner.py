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

# @file    runner.py
# @author  Mark Colley
# @date    2026-11-16

import importlib.util
import os
import sys
import tempfile
from pathlib import Path

root = Path(os.environ['SUMO_HOME'])
sys.path.insert(0, str(root / 'tools'))
import sumolib  # noqa

selection = sumolib.files.selection
if len(sys.argv) > 1:
    spec = importlib.util.spec_from_file_location('selection_before', sys.argv[1])
    selection = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(selection)

entries = {'edge': {':J_0', 'district:road:segment_2', 'ordinary'},
           'lane': {':J_0_0', 'district:road:segment_2_10', 'ordinary_0'},
           'junction': {'city:junction:4', 'normal'},
           'connection': {'from:J_0_0toedge:other_0'},
           'poi': {'stop:platform:1'}}
with tempfile.TemporaryDirectory() as temporary:
    path = Path(temporary) / 'selection.txt'
    with path.open('w') as output:
        selection.write(output, entries)
    actual = selection.read(path, lanes2edges=False)
    assert actual == entries, actual
    print('Selection writer and reader preserve complete identifiers containing colons')
    converted = selection.read(path)
    assert converted == {name: values for name, values in entries.items() if name != 'lane'}, converted
    print('Lane conversion retains complete edge identifiers and merges explicit edge selections')

    network = Path(__file__).resolve().parents[1] / 'net_intermodal' / 'input_net.net.xml'
    net = sumolib.net.readNet(str(network), withInternal=True, withPedestrianConnections=True)
    internal = [edge for edge in net.getEdges(withInternal=True) if edge.isSpecial()]
    assert internal and all(edge.getID().startswith(':') for edge in internal)
    chosen = internal[:5]
    entries = {'edge': {edge.getID() for edge in chosen},
               'lane': {edge.getLane(0).getID() for edge in chosen}}
    with path.open('w') as output:
        selection.write(output, entries)
    actual = selection.read(path, lanes2edges=False)
    assert actual == entries
    assert {net.getEdge(name) for name in actual['edge']} == set(chosen)
    assert {net.getLane(name).getEdge() for name in actual['lane']} == set(chosen)
    converted = selection.read(path)
    assert converted == {'edge': entries['edge']}
    print('Selected internal lanes and edges resolve in the parsed pedestrian network')
