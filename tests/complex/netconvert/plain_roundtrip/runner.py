#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2013-08-16

"""
export sumo network to pain csv and then convert csv to xml and compare with plain xml
"""
from __future__ import absolute_import


import sys
import os
import subprocess
import difflib

import pandas as pd

sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"),
             os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools", "xml")]
import sumolib  # noqa
import csv2xml  # noqa

format = sys.argv[1]
netconvert = sumolib.checkBinary('netconvert')

subprocess.call([netconvert,
                 '--sumo-net-file', 'input_net.net.xml.gz',
                 '--plain-output', 'plain'])
subprocess.call([netconvert,
                 '--sumo-net-file', 'input_net.net.xml.gz',
                 '--plain-output', 'plain', '--output.format', format])
xsd_dir = os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "data", "xsd")
schemata = {
    "plain.con." + format: "connections_file.xsd",
    "plain.edg." + format: "edges_file.xsd",
    "plain.nod." + format: "nodes_file.xsd",
    "plain.tll." + format: "tllogic_file.xsd",
    "plain.typ." + format: "types_file.xsd"
}
csvs = []
for f, s in schemata.items():
    if format == "parquet":
        with open(f.replace(".parquet", ".csv"), "w") as csv:
            df = pd.read_parquet(f, dtype_backend="numpy_nullable")
            # print(df)
            df.to_csv(csv, index=False, lineterminator='\n', float_format='%.2f', sep=";")
        f = csv.name
    csv2xml.main([f, "-o", f + ".xml", "-x", os.path.join(xsd_dir, s)])
    csvs.append(f)
for f in csvs:
    with open(f + ".xml") as csv_xml:
        csv_xml_lines = [line for line in csv_xml if line.strip()]
    xml_lines = []
    with open(f.replace(".csv", ".xml")) as xml:
        skip = False
        for line in xml:
            if line.strip().startswith("<!--"):
                skip = True
            if line.strip() and not skip:
                xml_lines.append(line.replace("  state", " state"))
            if line.strip().endswith("-->"):
                skip = False
    for d in difflib.unified_diff(csv_xml_lines, xml_lines, n=0):
        for line in d.splitlines():
            if not line.startswith(('---', '+++')):
                print(line)
